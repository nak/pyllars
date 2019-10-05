import os
from typing import List, Optional

from pyllars.cppparser.parser.clang_translator import NodeType
from .generator import Generator

class CXXRecordDeclGenerator(Generator):

    def generate(self):
        self._node.normalize()
        if 'implicit' in self._node.qualifiers:
            return None, None

        name = self._node.name or "anonymous_%s" % self._node.node_id
        parent = self._node.parent
        parent_name = parent.name if parent else "pyllars"
        typename_qualifier = "typename" if 'union' not in self._node.qualifiers else ""

        if self._node.name:
            typename = f"{typename_qualifier} ::{self._node.full_cpp_name}"
        else:
            index = self._node.parent.children.index(self._node)
            if index < 0:
                raise Exception("invalid code structure encountered")
            if len(self._node.parent.children) > index + 1 and isinstance(self._node.parent.children[index + 1],
                                                                          NodeType.FieldDecl):
                field = self._node.parent.children[index + 1]
                field_name = field.full_cpp_name
                typename = f"decltype(::{field_name})" if field.name else None
            else:
                typename = None
        if not typename:
            return None, None
        header_stream = open(os.path.join(self.my_root_dir, name+'.hpp'), 'w',
                             encoding='utf-8')
        body_stream = open(os.path.join(self.my_root_dir, self._source_path_root, name+'.cpp'), 'w',
                           encoding='utf-8')
        try:
            if self._node.name:
                add_pyobject_code = f"PyObject_SetAttrString((PyObject*)__pyllars_internal::PythonClassWrapper< {typename}>::getPyType(), name, pyobj)"
            else:
                parent_name = self._node.parent.name or "anonymous_%s" % self._node.node_id
                add_pyobject_code = f"{parent_name}_addPyObject(name, pyobj)"

            # generate header
            header_stream.write(Generator.COMMON_HEADER)
            header_stream.write(self._wrap_in_namespaces(f"""
               /**
                * Register a subcomponent for initialization
                **/
               status_t {name}_register(::pyllars::Initializer*);
               
               /**
                * add a child object
                **/
               status_t {name}_addPyObject(const char* const name, PyObject* pyobj);
               
            """, True))

            #generate body
            body_stream.write(f"""\n#include "{self.source_path}" 
#include \"{name}.hpp"
            """)
            if self._node.parent:
                body_stream.write(f"\n#include \"..{os.sep}{parent_name}.hpp\"\n")
            if typename:
                body_stream.write(f"""
                namespace __pyllars_internal{{
                    template<>
                    struct _Types<{typename}>{{
                        static const char* const type_name;
                    }};
                    const char* const _Types<{typename}>::type_name =  "{name}";
                }}
            """)
            class_full_name = self._node.full_cpp_name if self._node.name else None
            inheritance_code = ""
            ready_code = ""
            if typename:
                for base in self._node.bases or []:
                    base_class_name = base.full_name
                    inheritance_code += f"""
                        status |= pyllars::{base.full_name}_set_up();
                        __pyllars_internal::PythonClassWrapper< {typename_qualifier} ::{class_full_name}>::addBaseClass<::{base_class_name}>();
    
                    """
                for base in self._node.bases or []:
                    ready_code += f"""
                        status |= pyllars::{base.full_name}_ready(top_level_mod);
                    """
                for base in self._node.bases or []:
                    hierarchy = base.full_name.split('::')[:-1]
                    text = "// Difficult to get header location as source of inheritance class, so declase set_up & ready explicitly here\n" \
                        "namespace pyllars{\n" +"\n".join([f"namespace {name}{{" for name in hierarchy])
                    text += f"\nstatus_t {base.name}_set_up();\n"
                    text += f"\nstatus_t {base.name}_ready(PyObject*);\n"
                    text += "}\n"*(len(hierarchy) + 1)
                    body_stream.write(text + "\n\n")
            if not self._node.name:
                add_myobject_code = ""
            else:
                add_myobject_code = f"status |= {parent_name}_addPyObject(\"{name}\", (PyObject*)__pyllars_internal::PythonClassWrapper<{typename}>::getPyType())"
            initializer_code = f"status |= __pyllars_internal::PythonClassWrapper< {typename} >::initialize();" if typename else ""
            body_stream.write(self._wrap_in_namespaces(f"""
            status_t {name}_addPyObject(const char* const name, PyObject* pyobj){{
               return {add_pyobject_code};
            }}             
                
            namespace {{
                //From: CXXRecordDeclGenerator.generate
                class Initializer_{name}: public pyllars::Initializer{{
                public:
                    Initializer_{name}():pyllars::Initializer(){{
                    
                        {parent_name}_register(this);                          
                    }}

                    int set_up() override{{
                        static int status = -1;
                        using namespace __pyllars_internal;
                        static bool inited = false;
                        if (inited){{
                            return status;
                        }}
                        {initializer_code}
                        inited = true;
                        status = 0;
                        return status;
                    }}

                    int ready(PyObject * const top_level_mod) override{{
                       int status = pyllars::Initializer::ready(top_level_mod);
                       {ready_code}
                       {inheritance_code}
                     
                       {add_myobject_code};
                     
                       return status;
                    }}
                    
                    static Initializer_{name}* initializer;
                    
                    static Initializer_{name} *singleton(){{
                        static  Initializer_{name} _initializer;
                        return &_initializer;
                    }}
                 }};
                 
                
                //ensure instance is created on global static initialization, otherwise this
                //element would never be reigstered and picked up
                Initializer_{name} * Initializer_{name}::initializer = singleton();
                     
                status_t {name}_register(::pyllars::Initializer* const init){{
                    return Initializer_{name}::initializer->register_init(init);
                }}
            
    
            }}
                """, True))

        finally:
            header_stream.close()
            body_stream.close()
        return header_stream.name, body_stream.name


class DefinitionDataGenerator(Generator):

    def generate(self):
        return None, None


class DefaultConstructorGenerator(Generator):

    def generate(self):
        parent = self._node.parent.parent
        class_name = self._node.parent.parent.name
        if not class_name:
            return None, None
        if 'default_is_constexpr' in self._node.classifiers:
            return None, None
        class_full_cpp_name = self._node.parent.parent.full_cpp_name

        while parent and not parent.name and isinstance(parent, NodeType.CXXRecordDecl):
            parent = parent.parent
            if not parent:
                return None, None
        body_stream = open(
            os.path.join(self.my_root_dir, self._source_path_root, class_name + ' default_constructor.cpp'), 'w',
            encoding='utf-8')

        try:
            parent_name = parent.name
            parent_header_path = os.path.join("..", parent_name)
            # generate body
            body_stream.write(f"""\n#include \"{self.source_path}\" 
#include \"{parent_header_path}.hpp\"
                    """)
            name = class_name + "_default_constructor"
            body_stream.write(self._wrap_in_namespaces(f"""
            
                    namespace {{
                        //From: DefaultConstructorDeclGenerator.generate

                        class Initializer_{name}: public pyllars::Initializer{{
                        public:
                            Initializer_{name}():pyllars::Initializer(){{
                                {parent_name}_register(this);                          
                            }}

                            int set_up() override{{
                                return 0; //nothing to do on setup
                            }}

                            int ready(PyObject * const top_level_mod) override{{
                               static const char* const kwlist[] = {{}};
                               int status = pyllars::Initializer::ready(top_level_mod);
                               __pyllars_internal::PythonClassWrapper< ::{class_full_cpp_name} >::addConstructor<>(kwlist);
                               return status;
                            }}

                            static Initializer_{name}* initializer;

                            static Initializer_{name} *singleton(){{
                                static  Initializer_{name} _initializer;
                                return &_initializer;
                            }}
                         }};


                        //ensure instance is created on global static initialization, otherwise this
                        //element would never be reigstered and picked up
                        Initializer_{name} * Initializer_{name}::initializer = singleton();

                    }}
                """, True))
        finally:
            body_stream.close()
        return None, body_stream.name


class CopyConstructorGenerator(Generator):

    def generate(self):
        class_name = self._node.parent.parent.name
        if not class_name:
            return None, None
        if 'user_declared' in self._node.classifiers or not self._node.classifiers:
            return None, None
        class_full_cpp_name = self._node.parent.parent.full_cpp_name
        parent = self._node.parent.parent
        while parent and not parent.name and isinstance(parent, NodeType.CXXRecordDecl):
            parent = parent.parent
            if not parent:
                return None, None
        body_stream = open(
            os.path.join(self.my_root_dir, self._source_path_root, class_name + ' default_copy_constructor.cpp'), 'w',
            encoding='utf-8')
        try:
            parent_name = parent.name
            parent_header_path = os.path.join("..", parent_name)
            # generate body
            body_stream.write(f"""\n#include \"{self.source_path}\" 
#include \"{parent_header_path}.hpp\"
                    """)
            name = class_name + "_default_copy_constructor"
            body_stream.write(self._wrap_in_namespaces(f"""

                    namespace {{
                        //From: DefaultConstructorDeclGenerator.generate

                        class Initializer_{name}: public pyllars::Initializer{{
                        public:
                            Initializer_{name}():pyllars::Initializer(){{
                                {parent_name}_register(this);                          
                            }}

                            int set_up() override{{
                                return 0; //nothing to do on setup
                            }}

                            int ready(PyObject * const top_level_mod) override{{
                               static const char* const kwlist[] = {{"object"}};
                               int status = pyllars::Initializer::ready(top_level_mod);
                               __pyllars_internal::PythonClassWrapper< ::{class_full_cpp_name} >::addConstructor<const ::{class_full_cpp_name}&>(kwlist);
                               return status;
                            }}

                            static Initializer_{name}* initializer;

                            static Initializer_{name} *singleton(){{
                                static  Initializer_{name} _initializer;
                                return &_initializer;
                            }}
                         }};


                        //ensure instance is created on global static initialization, otherwise this
                        //element would never be reigstered and picked up
                        Initializer_{name} * Initializer_{name}::initializer = singleton();

                    }}
                """, True))
        finally:
            body_stream.close()
        return None, body_stream.name


class MoveConstructorGenerator(Generator):

    def generate(self):
        class_name = self._node.parent.parent.name
        if not class_name:
            return None, None
        if 'user_declared' in self._node.classifiers or not self._node.classifiers:
            return None, None
        class_full_cpp_name = self._node.parent.parent.full_cpp_name
        parent = self._node.parent.parent
        while parent and not parent.name and isinstance(parent, NodeType.CXXRecordDecl):
            parent = parent.parent
            if not parent:
                return None, None
        body_stream = open(
            os.path.join(self.my_root_dir, self._source_path_root, class_name + ' default_move_constructor.cpp'), 'w',
            encoding='utf-8')
        try:
            parent_name = parent.name
            parent_header_path = os.path.join("..", parent_name)
            # generate body
            body_stream.write(f"""\n#include \"{self.source_path}\" 
#include \"{parent_header_path}.hpp\"
                    """)
            name = class_name + "_default_move_constructor"
            body_stream.write(self._wrap_in_namespaces(f"""

                    namespace {{
                        //From: DefaultConstructorDeclGenerator.generate

                        class Initializer_{name}: public pyllars::Initializer{{
                        public:
                            Initializer_{name}():pyllars::Initializer(){{

                                {parent_name}_register(this);                          
                            }}

                            int set_up() override{{
                                return 0; //nothing to do on setup
                            }}

                            int ready(PyObject * const top_level_mod) override{{
                               static const char* const kwlist[] = {{"object"}};
                               int status = pyllars::Initializer::ready(top_level_mod);
                               __pyllars_internal::PythonClassWrapper< ::{class_full_cpp_name} >::addConstructor<const ::{class_full_cpp_name}&&>(kwlist);
                               return status;
                            }}

                            static Initializer_{name}* initializer;

                            static Initializer_{name} *singleton(){{
                                static  Initializer_{name} _initializer;
                                return &_initializer;
                            }}
                         }};


                        //ensure instance is created on global static initialization, otherwise this
                        //element would never be reigstered and picked up
                        Initializer_{name} * Initializer_{name}::initializer = singleton();

                    }}
                """, True))
        finally:
            body_stream.close()
        return None, body_stream.name


class CopyAssignmentGenerator(Generator):

    def generate(self):
        class_name = self._node.parent.parent.name
        if not class_name:
            return None, None
        if 'user_declared' in self._node.classifiers or not self._node.classifiers:
            return None, None
        class_full_cpp_name = self._node.parent.parent.full_cpp_name
        parent = self._node.parent.parent
        while parent and not parent.name and isinstance(parent, NodeType.CXXRecordDecl):
            parent = parent.parent
            if not parent:
                return None, None
        body_stream = open(
            os.path.join(self.my_root_dir, self._source_path_root, class_name + ' default_copy_assignment.cpp'), 'w',
            encoding='utf-8')
        try:
            parent_name = parent.name
            parent_header_path = os.path.join("..", parent_name)
            # generate body
            body_stream.write(f"""\n#include \"{self.source_path}\" 
#include \"{parent_header_path}.hpp\"
#include <cstddef>
#include <type_traits>

                    """)
            name = class_name + "_default_copy_assignment"
            body_stream.write(self._wrap_in_namespaces(f"""

                    namespace {{
                        //From: DefaultConstructorDeclGenerator.generate

                        /**
                        * clang does not properly delete default assignment operator, so must use compile-time check
                        * instead to prevent compiler error from generated code that shouldn't be
                        */
                        template<const char* const name, const char* const kwlist[], typename T>
                        static int ready_template(){{
                            if constexpr (std::is_copy_assignable<T>::value){{
                               typedef T& (T::*method_t)(const T&);
                               __pyllars_internal::PythonClassWrapper<T>::template addMethod<name, kwlist, method_t, &T::operator= >();	
                            }}
                            return 0;
                        }}

                       typedef const char* const kwlist_t[2];
                       constexpr kwlist_t kwlist = {{"assign_to", nullptr}};
                       constexpr cstring this_name = "this";
                       
                        class Initializer_{name}: public pyllars::Initializer{{
                        public:
                            Initializer_{name}():pyllars::Initializer(){{

                                {parent_name}_register(this);                          
                            }}

                            int set_up() override{{
                                return 0; //nothing to do on setup
                            }}

                            int ready(PyObject * const top_level_mod) override{{
                                return ready_template<this_name, kwlist, ::{class_full_cpp_name}>();
                            }}

                            static Initializer_{name}* initializer;

                            static Initializer_{name} *singleton(){{
                                static  Initializer_{name} _initializer;
                                return &_initializer;
                            }}
                         }};


                        //ensure instance is created on global static initialization, otherwise this
                        //element would never be reigstered and picked up
                        Initializer_{name} * Initializer_{name}::initializer = singleton();

                    }}
                """, True))
        finally:
            body_stream.close()
        return None, body_stream.name


class MoveAssignmentGenerator(Generator):

    def generate(self):
        class_name = self._node.parent.parent.name
        if not class_name:
            return None, None
        if 'user_declared' in self._node.classifiers or not self._node.classifiers:
            return None, None
        class_name = self._node.parent.parent.name
        class_full_cpp_name = self._node.parent.parent.full_cpp_name
        parent = self._node.parent.parent
        while parent and not parent.name and isinstance(parent, NodeType.CXXRecordDecl):
            parent = parent.parent
            if not parent:
                return None, None
        body_stream = open(
            os.path.join(self.my_root_dir, self._source_path_root, class_name + ' default_move_assignment.cpp'), 'w',
            encoding='utf-8')
        try:
            parent_name = parent.name
            parent_header_path = os.path.join("..", parent_name)
            # generate body
            body_stream.write(f"""\n#include \"{self.source_path}\" 
#include \"{parent_header_path}.hpp\"
#include <cstddef>
                    """)
            name = class_name + "_default_move_assignment"
            body_stream.write(self._wrap_in_namespaces(f"""

                    namespace {{
                        //From: DefaultConstructorDeclGenerator.generate

                       typedef const char* const kwlist_t[2];
                       constexpr kwlist_t kwlist = {{"assign_to", nullptr}};
                       constexpr cstring this_name = "this";
                       
                        class Initializer_{name}: public pyllars::Initializer{{
                        public:
                            Initializer_{name}():pyllars::Initializer(){{

                                {parent_name}_register(this);                          
                            }}

                            int set_up() override{{
                                return 0; //nothing to do on setup
                            }}

                            int ready(PyObject * const top_level_mod) override{{
                               int status = pyllars::Initializer::ready(top_level_mod);
                               __pyllars_internal::PythonClassWrapper< ::{class_full_cpp_name} >::addMethod<this_name, kwlist, 
                                   ::{class_full_cpp_name}& (::{class_full_cpp_name}::*)(const ::{class_full_cpp_name}&&),
                                   &::{class_full_cpp_name}::operator= >();
                               return status;
                            }}

                            static Initializer_{name}* initializer;

                            static Initializer_{name} *singleton(){{
                                static  Initializer_{name} _initializer;
                                return &_initializer;
                            }}
                         }};


                        //ensure instance is created on global static initialization, otherwise this
                        //element would never be reigstered and picked up
                        Initializer_{name} * Initializer_{name}::initializer = singleton();

                    }}
                """, True))
        finally:
            body_stream.close()
        return None, body_stream.name


class CXXMethodDeclGenerator(Generator):

    def _scoped_type_name(self, typ):
        parts = typ.strip().split(' ')

        def full_name(t):
            if "::" in t:
                first, rest = t.split("::", maxsplit=1)
            else:
                first, rest = t, ""
            # search upward for enclosing definition
            parent = self._node
            while parent:
                if hasattr(parent, 'name') and parent.name == first:
                    return "::" + ("::".join([parent.full_cpp_name, rest]) if rest else parent.full_cpp_name)
                parent = parent.parent
            # possibly an internally defined class or type:
            for child in self._node.parent.children:
                if hasattr(child, 'name') and child.name == t:
                    return '::' + child.full_cpp_name
            return t

        for index, typ in enumerate(parts):
            if not typ in self.KEYWORDS:
                parts[index] = full_name(typ)
        return ' '.join(parts)

    def _full_signature(self):
        is_static = 'static' in self._node.qualifiers
        ret_type = self._scoped_type_name(self._node.signature.split('(')[0])
        qualifiers = self._node.signature.rsplit(')', maxsplit=1)[-1]
        params = [self._scoped_type_name(p.type_text) for p in self._node.children if isinstance(p, NodeType.ParmVarDecl)]
        if '...' in self._node.signature:
            params.append("...")
        params = ", ".join(params)
        class_qualifier = f"(::{self._node.parent.full_cpp_name}::*method_t)" if not is_static else "(method_t)"
        return f"{ret_type} {class_qualifier}({params}) {qualifiers}"

    def generate(self):
        if self._node.name == "operator=":
            return self.generate_assignment()
        if self._node.name.startswith("operator"):
            return self.generate_operator()
        class_name = self._node.parent.name
        class_full_cpp_name = self._node.parent.full_cpp_name
        parent = self._node.parent
        while parent and not parent.name and isinstance(parent, NodeType.CXXRecordDecl):
            parent = parent.parent
            if not parent:
                return None, None
        body_stream = open(
            os.path.join(self.my_root_dir, self._source_path_root, class_name + '::' + self._node.name + '.cpp'), 'w',
            encoding='utf-8')
        try:
            parent_name = parent.name
            parent_header_path = os.path.join("..", parent_name)
            # generate body
            body_stream.write(f"""\n#include \"{self.source_path}\" 
#include \"{parent_header_path}.hpp\"
#include <{self.source_path}>
                            """)
            name = self._node.name
            method_qualifier = "Class" if 'static' in self._node.qualifiers else ""
            signature = self._full_signature()
            kwlist = []
            for c in reversed([c for c in self._node.children if isinstance(c, NodeType.ParmVarDecl)]):
                if not c.name:
                    break
                kwlist.insert(0, f"\"{c.name}\"")
            kwlist_items = ", ".join(kwlist + ["nullptr"])
            body_stream.write(self._wrap_in_namespaces(f"""

                            namespace {{
                               //From: CXXMethodDeclGenerator.generate

                               typedef const char* const kwlist_t[{len(kwlist)+1}];
                               constexpr kwlist_t kwlist = {{{kwlist_items}}};
                               constexpr cstring this_name = "{name}";

                                class Initializer_{name}: public pyllars::Initializer{{
                                public:
                                    Initializer_{name}():pyllars::Initializer(){{
                                        {parent_name}_register(this);                          
                                    }}

                                    int set_up() override{{
                                        return 0; //nothing to do on setup
                                    }}
                                    typedef {signature};
                                    int ready(PyObject * const top_level_mod) override{{
                                       int status = pyllars::Initializer::ready(top_level_mod);
                                       __pyllars_internal::PythonClassWrapper< ::{class_full_cpp_name} >::add{method_qualifier}Method<this_name, kwlist, 
                                           method_t,
                                           &::{class_full_cpp_name}::{name} >();
                                       return status;
                                    }}

                                    static Initializer_{name}* initializer;

                                    static Initializer_{name} *singleton(){{
                                        static  Initializer_{name} _initializer;
                                        return &_initializer;
                                    }}
                                 }};


                                //ensure instance is created on global static initialization, otherwise this
                                //element would never be reigstered and picked up
                                Initializer_{name} * Initializer_{name}::initializer = singleton();

                            }}
                        """, True))
        finally:
            body_stream.close()
        return None, body_stream.name

    def generate_assignment(self):
        if 'default_delete' in self._node.qualifiers:
            return None, None
        class_name = self._node.parent.name
        class_full_cpp_name = self._node.parent.full_cpp_name
        body_stream = open(
            os.path.join(self.my_root_dir, self._source_path_root, class_name + '::' + self._node.name + '.cpp'), 'w',
            encoding='utf-8')
        try:

            parent = self._node.parent
            while parent and not parent.name and isinstance(parent, NodeType.CXXRecordDecl):
                parent = parent.parent
                if not parent:
                    return None, None
            parent_name = parent.name
            parent_header_path = os.path.join("..", parent_name)
            # generate body
            body_stream.write(f"""\n#include \"{self.source_path}\" 
#include \"{parent_header_path}.hpp\"
#include <{self.source_path}>
                            """)
            name = "this"
            signature = self._full_signature()
            kwlist = []
            for c in reversed([c for c in self._node.children if isinstance(c, NodeType.ParmVarDecl)]):
                if not c.name:
                    break
                kwlist.insert(0, f"\"{c.name}\"")
            if len(kwlist) == 1:
                kwlist = ["assign_to"]
            kwlist_items = ", ".join(kwlist + ["nullptr"])
            body_stream.write(self._wrap_in_namespaces(f"""

                            namespace {{
                               //From: CXXMethodDeclGenerator.generate

                               typedef const char* const kwlist_t[{len(kwlist)+1}];
                               constexpr kwlist_t kwlist = {{{kwlist_items}}};
                               constexpr cstring this_name = "this";

                                class Initializer_{name}: public pyllars::Initializer{{
                                public:
                                    Initializer_{name}():pyllars::Initializer(){{
                                        {parent_name}_register(this);                          
                                    }}

                                    int set_up() override{{
                                        return 0; //nothing to do on setup
                                    }}

                                    int ready(PyObject * const top_level_mod) override{{
                                       int status = pyllars::Initializer::ready(top_level_mod);
                                       typedef {signature};
                                       __pyllars_internal::PythonClassWrapper< ::{class_full_cpp_name}  >::addMethod<this_name, kwlist, 
                                           method_t,
                                           &::{class_full_cpp_name}::operator= >();
                                       return status;
                                    }}

                                    static Initializer_{name}* initializer;

                                    static Initializer_{name} *singleton(){{
                                        static  Initializer_{name} _initializer;
                                        return &_initializer;
                                    }}
                                 }};


                                //ensure instance is created on global static initialization, otherwise this
                                //element would never be reigstered and picked up
                                Initializer_{name} * Initializer_{name}::initializer = singleton();

                            }}
                        """, True))
        finally:
            body_stream.close()
        return None, body_stream.name

    def generate_operator(self):
        unary_mapping = {
            '~' : 'Inv',
            '+' : 'Pos',
            '-' : 'Neg',
        }
        binary_mapping = {
            '+': 'Add',
            '-': 'Sub',
            '*': 'Mul',
            '/': 'Div',
            '&': 'And',
            '|': 'Or',
            '^': 'Xor',
            '<<': 'Lshift',
            '>>': 'Rshift',
            '%': 'Mod',
            '+=': 'InplaceAdd',
            '-=': 'InplaceSub',
            '*=': 'InplaceMul',
            '/=': 'InplaceDiv',
            '&=': 'InplaceAnd',
            '|=': 'InplaceOr',
            '^=': 'InplaceXor',
            '<<=': 'InplaceLshift',
            '>>=': 'InplaceRshift',
            '%=': 'InplaceMod',
            '[]': 'Map'
        }
        if 'default_delete' in self._node.qualifiers:
            return None, None
        operator_kind = self._node.name.replace("operator", '')
        params = [p for p in self._node.children if isinstance(p, NodeType.ParmVarDecl)]
        if len(params) > 1:
            raise Exception("Unexpected number of operator params")
        cpp_op_name = unary_mapping.get(operator_kind) if len(params) == 0 else binary_mapping.get(operator_kind)
        if cpp_op_name is None:
            raise Exception(f"Unknown operator: {operator_kind}")

        class_name = self._node.parent.name
        class_full_cpp_name = self._node.parent.full_cpp_name
        body_stream = open(
            os.path.join(self.my_root_dir, self._source_path_root, class_name + '::' + self._node.name.replace("/", " div") + '.cpp'), 'w',
            encoding='utf-8')
        try:
            parent = self._node.parent
            while parent and not parent.name and isinstance(parent, NodeType.CXXRecordDecl):
                parent = parent.parent
                if not parent:
                    return None, None
            parent_name = parent.name
            parent_header_path = os.path.join("..", parent_name)
            # generate body
            body_stream.write(f"""\n#include \"{self.source_path}\" 
#include \"{parent_header_path}.hpp\"
#include <{self.source_path}>
                            """)
            name = self._node.name.replace(operator_kind, cpp_op_name)
            signature = self._full_signature()
            kwlist = []
            for index, c in enumerate(params):
                if not c.name:
                    kwlist.append(f"\"_{index}\"")
                else:
                    kwlist.append(f"\"{c.name}\"")
            kwlist_items = ", ".join(kwlist + ["nullptr"])
            kwlist_if = "" if len(params) == 0 else "kwlist,"
            body_stream.write(self._wrap_in_namespaces(f"""

                            namespace {{
                               //From: CXXMethodDeclGenerator.generate

                               typedef const char* const kwlist_t[{len(kwlist)+1}];
                               constexpr kwlist_t kwlist = {{{kwlist_items}}};

                                class Initializer_{name}: public pyllars::Initializer{{
                                public:
                                    Initializer_{name}():pyllars::Initializer(){{
                                        {parent_name}_register(this);                          
                                    }}

                                    int set_up() override{{
                                        return 0; //nothing to do on setup
                                    }}
                                    typedef {signature};
                                    int ready(PyObject * const top_level_mod) override{{
                                       int status = pyllars::Initializer::ready(top_level_mod);
                                       __pyllars_internal::PythonClassWrapper< ::{class_full_cpp_name} >::add{cpp_op_name}Operator<{kwlist_if}
                                           method_t,
                                           &::{class_full_cpp_name}::{self._node.name} >();
                                       return status;
                                    }}

                                    static Initializer_{name}* initializer;

                                    static Initializer_{name} *singleton(){{
                                        static  Initializer_{name} _initializer;
                                        return &_initializer;
                                    }}
                                 }};


                                //ensure instance is created on global static initialization, otherwise this
                                //element would never be reigstered and picked up
                                Initializer_{name} * Initializer_{name}::initializer = singleton();

                            }}
                        """, True))
        finally:
            body_stream.close()
        return None, body_stream.name


def _parent_wrapper_name(node: NodeType.Node, recursed: Optional[NodeType.Node] = None):
    if False and not node.name:
        if node.parent is None:
            return None, None, None
        return _parent_wrapper_name(node.parent, recursed)
    parent = node.parent
    if recursed:
        node = recursed
    index = parent.parent.children.index(parent)
    if index < 0:
        raise Exception("Invalid structure in hierarchy")
    is_named_attribute = len(parent.parent.children) -1 > index and isinstance(parent.parent.children[index + 1], NodeType.FieldDecl)
    if parent.name:
        if recursed:
            return f"__pyllars_internal::PythonAnonymousClassWrapper< ::{parent.full_cpp_name} >",\
                   f"::{parent.full_cpp_name}", \
                   f"decltype(::{parent.full_cpp_name}::{node.name})",\
                   f"::{parent.full_cpp_name}::{node.name}"
        else:
            return f"__pyllars_internal::PythonClassWrapper< ::{parent.full_cpp_name} >", \
                   f"::{parent.full_cpp_name}", \
                   f"decltype(::{parent.full_cpp_name}::{node.name})",\
                   f"::{parent.full_cpp_name}::{node.name}"
    elif is_named_attribute:
        # parent is anonymous type with a named field declaration, so this element is referenced to direct parent (field)
        parent_field_name = parent.parent.children[index + 1].name
        if parent_field_name:
            return f"__pyllars_internal::PythonClassWrapper<decltype(::{parent.parent.full_cpp_name}::{parent_field_name})>", \
                   f"decltype(::{parent.parent.full_cpp_name}::{parent_field_name})", \
                   f"decltype(::{parent.parent.full_cpp_name}::{parent_field_name}.{node.name})", \
                   f"decltype(::{parent.parent.full_cpp_name}::{parent_field_name})::{node.name}"
        elif parent.parent.name:
            return f"__pyllars_internal::PythonClassWrapper<::{parent.parent.full_cpp_name}>", \
                   f"::{parent.parent.full_cpp_name}", \
                   f"decltype(::{parent.parent.full_cpp_name}::{node.name})", \
                   f"::{parent.parent.full_cpp_name}::{node.name}"
        else:
            return _parent_wrapper_name(parent, node)
    elif recursed:
        return _parent_wrapper_name(parent, node)

    index = parent.parent.children.index(parent)
    if index < 0:
        raise Exception("Invalid structure in hierarchy")

    if is_named_attribute:
        # parent is anonymous type with a named field declaration, so this element is referenced to direct parent (field)
        parent_field_name = parent.parent.children[index + 1].name
        if parent_field_name:
            return f"__pyllars_internal::PythonClassWrapper<decltype(::{parent.parent.full_cpp_name}::{parent_field_name})>", \
                   f"{parent.parent.full_cpp_name}", \
                   f"decltype(::{parent.parent.full_cpp_name}::{parent_field_name})",\
                   f"::{parent.parent.full_cpp_name}::{parent_field_name}"
        elif parent.parent.name:
            return f"__pyllars_internal::PythonClassWrapper<decltype(::{parent.parent.full_cpp_name})>", \
                   f"::{parent.parent.full_cpp_name}",\
                   f"decltype(::{parent.parent.full_cpp_name}::{node.name})",\
                   f"::{parent.parent.full_cpp_name}::{node.name}"
        else:
            return _parent_wrapper_name(parent, node)
    else:
        # parent is anonymous type without associated field, so element belongs to parent's parent when referenced in code
        if parent.parent.name:
            return f"__pyllars_internal::PythonAnonymousClassWrapper< ::{parent.parent.full_cpp_name} >", \
                   f"::{parent.parent.full_cpp_name}", \
                   f"decltype(::{parent.parent.full_cpp_name}::{node.name})", \
                   f"::{parent.parent.full_cpp_name}::{node.name}"
        else:
            return _parent_wrapper_name(parent, node)


class FieldDeclGenerator(Generator):

    def _scoped_type_name(self, typ):
        parts = typ.strip().split(' ')

        def full_name(t):
            if "::" in t:
                first, rest = t.split("::", maxsplit=1)
            else:
                first, rest = t, ""
            # search upward for enclosing definition
            parent = self._node
            while parent:
                if hasattr(parent, 'name') and parent.name == first:
                    return "::" + ("::".join([parent.full_cpp_name, rest]) if rest else parent.full_cpp_name)
                parent = parent.parent
            # possibly an internally defined class or type:
            for child in self._node.parent.children:
                if hasattr(child, 'name') and child.name == t:
                    return '::' + child.full_cpp_name
            return t

        for index, typ in enumerate(parts):
            if not typ in self.KEYWORDS:
                parts[index] = full_name(typ)
        return ' '.join(parts)

    def generate(self):
        if 'public' not in self._node.qualifiers and 'struct' not in self._node.parent.qualifiers:
            return None, None
        if isinstance(self._node, NodeType.IndirectFieldDecl):
            return None, None
        parent = self._node.parent
        while parent and not parent.name:
            parent = parent.parent
        if not parent:
            return None, None
        bitfield_specs = [c for c in self._node.children if isinstance(c, NodeType.IntegerLiteral)]
        if not isinstance(self, VarDeclGenerator) and bitfield_specs:
            return self.generate_bitfield(bitfield_specs)

        body_stream = open(
            os.path.join(self.my_root_dir, self._source_path_root,  (parent.name or f"anon_{parent.node_id}") + '::' + (self._node.name or "anon_" + self._node.node_id) + '.cpp'), 'w',
            encoding='utf-8')
        try:
            parent = self._node.parent
            while parent and not parent.name and isinstance(parent, NodeType.CXXRecordDecl):
                parent = parent.parent
                if not parent:
                    return None, None
            parent_name = parent.name
            parent_header_path = os.path.join("..", parent_name)
            # generate body
            body_stream.write(f"""\n#include \"{self.source_path}\" 
        #include \"{parent_header_path}.hpp\"
        #include <{self.source_path}>
                                    """)
            name = self._node.name or f"anon_{self._node.node_id}"
            if not self._node.name:
                return None, None
            wrapper, _, attribute_type_name, attribute_full_cpp_name = _parent_wrapper_name(self._node)
            member_qualifier = "Class" if 'static' in self._node.qualifiers else ""
            body_stream.write(self._wrap_in_namespaces(f"""

                                    namespace {{
                                       //From: FieldDeclGenerator.generate

                                       constexpr cstring this_name = "{name}";

                                        class Initializer_{name}: public pyllars::Initializer{{
                                        public:
                                            Initializer_{name}():pyllars::Initializer(){{
                                                {parent_name}_register(this);                          
                                            }}

                                            int set_up() override{{
                                                return 0; //nothing to do on setup
                                            }}
                                            int ready(PyObject * const top_level_mod) override{{
                                               int status = pyllars::Initializer::ready(top_level_mod);
                                               {wrapper}::template add{member_qualifier}Attribute<this_name, 
                                                   {attribute_type_name}>(&{attribute_full_cpp_name});
                                               return status;
                                            }}

                                            static Initializer_{name}* initializer;

                                            static Initializer_{name} *singleton(){{
                                                static  Initializer_{name} _initializer;
                                                return &_initializer;
                                            }}
                                         }};


                                        //ensure instance is created on global static initialization, otherwise this
                                        //element would never be reigstered and picked up
                                        Initializer_{name} * Initializer_{name}::initializer = singleton();

                                    }}
                                """, True))
        finally:
            body_stream.close()
        return None, body_stream.name

    def generate_bitfield(self, specs: List["NodeType.IntegerLiteral"]):
        if len(specs) > 1:
            raise Exception("multiple size specs provided for bit feild")
        size = specs[0].value
        is_const = 'const' in self._node.type_text.split()
        name = self._node.name or f"anon_{self._node.node_id}"
        wrapper, parent_type_name, attribute_type_name, attribute_full_cpp_name = _parent_wrapper_name(self._node)
        body_stream = open(
            os.path.join(self.my_root_dir, self._source_path_root, (self._node.parent.name or f"anon_{self._node.parent.node_id}") + '::' +
                         (self._node.name or "anon_{self._node.node_id}") + '.cpp'), 'w',
            encoding='utf-8')
        try:
            parent = self._node.parent
            while parent and not parent.name and isinstance(parent, NodeType.CXXRecordDecl):
                parent = parent.parent
                if not parent:
                    return None, None
            parent_name = parent.name
            parent_header_path = os.path.join("..", parent_name)
            # generate body
            body_stream.write(f"""\n#include \"{self.source_path}\" 
        #include \"{parent_header_path}.hpp\"
        #include <{self.source_path}>
                                    """)
            name = self._node.name
            typename = self._scoped_type_name(self._node.type_text)
            const_typename = 'const ' + typename if 'const' not in typename.split() else typename
            setter = "" if is_const else f"static std::function<{typename}({parent_type_name}&, {const_typename}&)> setter = []({parent_type_name} & obj, {const_typename}& value)->{typename}{{obj.{name} = value; return value;}};"
            setter_value = "nullptr" if is_const else "&setter"
            body_stream.write(self._wrap_in_namespaces(f"""

                                    namespace {{
                                       //From: CXXMethodDeclGenerator.generate

                                       constexpr cstring this_name = "{name}";

                                        class Initializer_{name}: public pyllars::Initializer{{
                                        public:
                                            Initializer_{name}():pyllars::Initializer(){{
                                                {parent_name}_register(this);                          
                                            }}

                                            int set_up() override{{
                                                return 0; //nothing to do on setup
                                            }}
                                            int ready(PyObject * const top_level_mod) override{{
                                               int status = pyllars::Initializer::ready(top_level_mod);
                                               static std::function<{typename}(const {parent_type_name}&)> getter = [](const {parent_type_name} & obj)->{typename}{{return  obj.{name};}};
                                               {setter}
                                               {wrapper}::template addBitField<this_name, 
                                                   {typename}, {size}>(getter, {setter_value});
                                               return status;
                                            }}

                                            static Initializer_{name}* initializer;

                                            static Initializer_{name} *singleton(){{
                                                static  Initializer_{name} _initializer;
                                                return &_initializer;
                                            }}
                                         }};


                                        //ensure instance is created on global static initialization, otherwise this
                                        //element would never be reigstered and picked up
                                        Initializer_{name} * Initializer_{name}::initializer = singleton();

                                    }}
                                """, True))
        finally:
            body_stream.close()
        return None, body_stream.name


class VarDeclGenerator(FieldDeclGenerator):
    pass


class IndirectFieldDeclGenerator(FieldDeclGenerator):
    pass
