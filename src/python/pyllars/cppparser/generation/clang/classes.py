import os
from typing import List

from pyllars.cppparser.parser.clang_translator import NodeType
from .generator import Generator

class CXXRecordDeclGenerator(Generator):

    def generate(self):
        self._node.normalize()
        if 'implicit' in self._node.qualifiers or not self._node.name:
            return None, None
        header_stream = open(os.path.join(self.my_root_dir, self._node.name+'.hpp'), 'w',
                             encoding='utf-8')
        body_stream = open(os.path.join(self.my_root_dir, self._source_path_root, self._node.name+'.cpp'), 'w',
                           encoding='utf-8')
        try:
            # generate header
            header_stream.write(Generator.COMMON_HEADER)
            header_stream.write(self._wrap_in_namespaces(f"""
               /**
                * Register a subcomponent for initialization
                **/
               status_t {self._node.name or "pyllars"}_register(::pyllars::Initializer*);
               
               /**
                * add a child object
                **/
               status_t {self._node.name}_addPyObject(const char* const name, PyObject* pyobj){{
                   return PyObject_SetAttrString((PyObject*)__pyllars_internal::PythonClassWrapper< typename ::{self._node.full_cpp_name} >::getPyType(),
                       name, pyobj);
               }}
            """, True))
            parent = self._node.parent
            parent_name = parent.name if parent else "pyllars"

            #generate body
            body_stream.write(f"""\n#include "{self.source_path}" 
#include \"{self._node.name}.hpp"
            """)
            if self._node.parent:
                body_stream.write(f"\n#include \"..{os.sep}{parent_name}.hpp\"\n")
            if not self._node.name and self._node.children:
                body_stream.write(f"""
                    namespace __pyllars_internal{{
                        template<>
                        struct _Types<decltype({self._node.full_cpp_name})>{{
                            static const char* const type_name;
                        }};
                        const char* const _Types<decltype({self._node.full_cpp_name})>::type_name =  "(anonymous enum)";
                    }}
                """)
            else:
                body_stream.write(f"""
                   namespace __pyllars_internal{{
                       template<>
                       struct _Types<::{self._node.full_cpp_name}>{{
                           static const char* const type_name;
                       }};
                       const char* const _Types<::{self._node.full_cpp_name}>::type_name =  "{self._node.name}";
                   }}
               """)
            if not self._node.name or 'implicit' in self._node.qualifiers:
                return header_stream.name, body_stream.name
            class_name = self._node.name if self._node.name else "anonymous_"
            parent_full_name = parent.full_cpp_name if parent else ""

            typename = "typename" if 'union' in self._node.qualifiers else ""
            class_full_name = self._node.full_cpp_name
            inheritance_code = f""
            for base in self._node.bases or []:
                base_class_name = base.full_name
                inheritance_code += f"""
                    status |= pyllars::{base.full_name}_set_up();
                    __pyllars_internal::PythonClassWrapper< {typename} ::{class_full_name}>::addBaseClass<::{base_class_name}>();

                """
            ready_code = ""
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
            if parent_full_name:
                add_pyobject_code = f"""
                                       status |= ::pyllars::{parent_full_name}_addPyObject("{self._node.name}", 
                                                    (PyObject*) __pyllars_internal::PythonClassWrapper< typename ::{self._node.full_cpp_name} >::getPyType());
                     
"""
            else:
                add_pyobject_code = f"""
                                      PyObject *pyllars_mod = PyImport_ImportModule("pyllars");
                                      status |= PyModule_AddObject(top_level_mod, "{self._node.name}",
                                         (PyObject*) __pyllars_internal::PythonClassWrapper< typename ::{self._node.full_cpp_name} >::getPyType());
                                      if (pyllars_mod){{
                                         status |= PyModule_AddObject(pyllars_mod, "{self._node.name}",
                                            (PyObject*) __pyllars_internal::PythonClassWrapper< typename ::{self._node.full_cpp_name} >::getPyType());
                                      }}

"""
            body_stream.write(self._wrap_in_namespaces(f"""
            namespace {{
                //From: CXXRecordDeclGenerator.generate
             
                class Initializer_{class_name}: public pyllars::Initializer{{
                public:
                    Initializer_{class_name}():pyllars::Initializer(){{
                    
                        pyllars::{parent_full_name or "pyllars"}_register(this);                          
                    }}

                    int set_up() override{{
                        static int status = -1;
                        using namespace __pyllars_internal;
                        static bool inited = false;
                        if (inited){{
                            return status;
                        }}
                        typedef typename ::{self._node.full_cpp_name}  main_type;
                        status |= __pyllars_internal::PythonClassWrapper< main_type >::initialize();  //classes
                        //not really much to do here
                        inited = true;
                        status = 0;
                        return status;
                    }}

                    int ready(PyObject * const top_level_mod) override{{
                       int status = pyllars::Initializer::ready(top_level_mod);
                       {ready_code}
                       {inheritance_code}
                     
                       {add_pyobject_code}
                     
                       return status;
                    }}
                    
                    static Initializer_{class_name}* initializer;
                    
                    static Initializer_{class_name} *singleton(){{
                        static  Initializer_{class_name} _initializer;
                        return &_initializer;
                    }}
                 }};
                 
                
                //ensure instance is created on global static initialization, otherwise this
                //element would never be reigstered and picked up
                Initializer_{class_name} * Initializer_{class_name}::initializer = singleton();
    
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
        class_name = self._node.parent.parent.name
        if not class_name:
            return None, None
        if 'default_is_constexpr' in self._node.classifiers:
            return None, None
        class_full_cpp_name = self._node.parent.parent.full_cpp_name

        parent = self._node.parent.parent
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
                               typedef typename ::{class_full_cpp_name}  main_type;
                               __pyllars_internal::PythonClassWrapper< main_type >::addConstructor<>(kwlist);
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
                               typedef typename ::{class_full_cpp_name}  main_type;
                               __pyllars_internal::PythonClassWrapper< main_type >::addConstructor<const ::{class_full_cpp_name}&>(kwlist);
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
                               typedef typename ::{class_full_cpp_name}  main_type;
                               __pyllars_internal::PythonClassWrapper< main_type >::addConstructor<const ::{class_full_cpp_name}&&>(kwlist);
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
                               typedef typename ::{class_full_cpp_name}  main_type;
                               __pyllars_internal::PythonClassWrapper< main_type >::addMethod<this_name, kwlist, 
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
                                       typedef typename ::{class_full_cpp_name}  main_type;
                                       __pyllars_internal::PythonClassWrapper< main_type >::add{method_qualifier}Method<this_name, kwlist, 
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
                                       typedef typename ::{class_full_cpp_name}  main_type;
                                       typedef {signature};
                                       __pyllars_internal::PythonClassWrapper< main_type >::addMethod<this_name, kwlist, 
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
                                       typedef typename ::{class_full_cpp_name}  main_type;
                                       __pyllars_internal::PythonClassWrapper< main_type >::add{cpp_op_name}Operator<{kwlist_if}
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


class FieldDeclGenerator(Generator):

    def normalize(self):
        # find parent
        if self._node.parent.name:
            self.parent_name = self._node.parent.name
        else:
            index = self._node.parent.parent.children.index(self._node.parent)
            if index < 0:
                raise Exception("Invalid structure in hierarchy")

            def find(parent: NodeType.Node):
                if len(parent.parent.children) > index and isinstance(parent.parent.children[index+1], NodeType.FieldDecl):
                    # parent is anonymous type with a named field declaration, so this element is referenced to direct parent (field)
                    parent_field_name = self._node.parent.parent.children[index+1].name
                    return f"decltype(::{self._node.parent.parent.full_cpp_name}::{parent_field_name})"
                else:
                    # parent is anonymous type without associated field, so element belongs to parent's parent when referenced in code
                    if parent.parent.name:
                        return self.parent.parent.name
                    else:
                        return find(parent.parent)

            self.parent_name = find(self._node.parent)

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
        self.normalize()
        if 'public' not in self._node.qualifiers and 'struct' not in self._node.parent.qualifiers:
            return None, None
        parent = self._node.parent
        while parent and not parent.name:
            parent = parent.parent
        if not parent:
            return None, None
        bitfield_specs = [c for c in self._node.children if isinstance(c, NodeType.IntegerLiteral)]
        if not isinstance(self, VarDeclGenerator) and bitfield_specs:
            return self.generate_bitfield(bitfield_specs)

        class_name = parent.name
        class_full_cpp_name = parent.full_cpp_name
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
            name = self._node.name
            member_qualifier = "Class" if 'static' in self._node.qualifiers else ""
            typename = self.parent_name  # self._scoped_type_name(self._node.type_text) if 'anonymous struct' not in self._node.type_text else f"decltype(::{self._node.full_cpp_name})"
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
                                               typedef typename ::{class_full_cpp_name}  main_type;
                                               __pyllars_internal::PythonClassWrapper< main_type >::template add{member_qualifier}Attribute<this_name, 
                                                   {typename}>(&::{class_full_cpp_name}::{name});
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
            name = self._node.name
            typename = self._scoped_type_name(self._node.type_text)
            const_typename = 'const ' + typename if 'const' not in typename.split() else typename
            setter = "" if is_const else f"static std::function<{typename}(::{class_full_cpp_name}&, {const_typename}&)> setter = [](::{class_full_cpp_name} & obj, {const_typename}& value)->{typename}{{obj.{self._node.name} = value; return value;}};"
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
                                               static std::function<{typename}(const ::{class_full_cpp_name}&)> getter = [](const ::{class_full_cpp_name} & obj)->{typename}{{return  obj.{self._node.name};}};
                                               {setter}
                                               typedef typename ::{class_full_cpp_name}  main_type;
                                               __pyllars_internal::PythonClassWrapper< main_type >::template addBitField<this_name, 
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