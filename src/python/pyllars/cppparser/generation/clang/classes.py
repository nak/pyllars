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
        typename_qualifier = "typename" if 'union' not in self._node.qualifiers else ""

        def find_typename(node: NodeType.Node, recurse: bool=False):
            if node is None:
                return None
            if node.name:
                typename = f"{typename_qualifier} ::{node.full_cpp_name}"
            elif not self._node.parent:
                typename = None
            else:
                index = self._node.parent.children.index(self._node)
                if index < 0:
                    raise Exception("invalid code structure encountered")
                if len(self._node.parent.children) > index + 1 and isinstance(self._node.parent.children[index + 1],
                                                                              NodeType.FieldDecl):
                    field = self._node.parent.children[index + 1]
                    field_name = field.full_cpp_name
                    typename = f"decltype(::{field_name})" if field.name else None
                elif recurse and node.parent and not isinstance(node.parent, NodeType.NamespaceDecl):
                    return find_typename(node.parent, recurse)
                else:
                    typename = None
            return typename

        typename = find_typename(self._node)
        if not typename:
            return None, None
        header_stream = open(os.path.join(self.my_root_dir, name+'.hpp'), 'w',
                             encoding='utf-8')
        body_stream = open(os.path.join(self.my_root_dir, self._source_path_root, name+'.cpp'), 'w',
                           encoding='utf-8')


        try:
            parent = self._node.parent

            # generate body

            body_stream.write(f"#include <pyllars/pyllars_class.hpp>\n")
            body_stream.write(f"#include <{self.source_path}>\n")
            if self._node.name:
                body_stream.write(f"#include \"{self._node.name}.hpp\"\n\n")
            body_stream.write("namespace {\n")
            if isinstance(parent, NodeType.NamespaceDecl):
                if parent:
                    body_stream.write(f"""
        extern const char parent_fullnsname[] = "{parent.full_cpp_name}"; 
        using Parent = pyllars::NSInfo<parent_fullnsname>;
                    """)
                else:
                    body_stream.write("using Parent = pyllars:GlobalNS;\n")
            else:
                body_stream.write(f"using Parent = {find_typename(self._node.parent, True) or 'pyllars::GlobalNS'};\n")

            if self._node.bases:
                bases = ", " + ", ".join([c.full_name for c in self._node.bases])
            else:
                bases = ""
            body_stream.write("}\n\n")
            body_stream.write(f"""

namespace pyllars_internal{{
    
    template<>
    struct DLLEXPORT TypeInfo<{typename}>{{
        static constexpr const char type_name[] = \"{self._node.name if self._node.name else "<<anonymous type>>"}\"; 
    }};
    
}}

            """)
            body_stream.write(f"template class pyllars::PyllarsClass<{typename}, Parent{bases}>;\n")
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

        while parent and not parent.name and isinstance(parent, NodeType.CXXRecordDecl):
            parent = parent.parent
            if not parent:
                return None, None
        body_stream = open(
            os.path.join(self.my_root_dir, self._source_path_root, class_name + ' default_constructor.cpp'), 'w',
            encoding='utf-8')

        try:
            # generate body
            body_stream.write(f"""\n#include \"{self.source_path}\" 
#include <pyllars/pyllars_classconstructor.hpp>

                    """)
            body_stream.write("namespace {\n")
            body_stream.write("   static const char* const empty_list[] = {nullptr};\n")
            body_stream.write("}\n")
            body_stream.write(f"template class pyllars::PyllarsClassConstructor<empty_list, "
                              f"{self._node.parent.parent.full_cpp_name}>;")
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
#include <pyllars/pyllars_classconstructor.hpp>
                    """)
            body_stream.write("using namespace pyllars;\nnamespace{\n")
            body_stream.write("   const char* const kwlist[] = {\"object\", nullptr};")
            body_stream.write("}\n\n")
            body_stream.write(f"template class PyllarsClassConstructor<kwlist, {class_full_cpp_name}, const {class_full_cpp_name}&>;")

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
            #include <pyllars/pyllars_classconstructor.hpp>
                                """)
            body_stream.write("using namespace pyllars;\nnamespace{\n")
            body_stream.write("   const char* const kwlist[] = {\"object\", nullptr};")
            body_stream.write("}\n\n")
            body_stream.write(
                f"template class PyllarsClassConstructor<kwlist, {class_full_cpp_name}, const {class_full_cpp_name}&&>;")
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
#include <pyllars/pyllars_classmethod.hpp>

                    """)

            body_stream.write(f"""
                    using namespace pyllars;
                    namespace {{
                        //From: DefaultConstructorDeclGenerator.generate

                        /**
                        * clang does not properly delete default assignment operator, so must use compile-time check
                        * instead to prevent compiler error from generated code that shouldn't be
                        */
                        template<const char* const name, const char* const kwlist[], typename T>
                        static int template_set_up(){{
                            if constexpr (std::is_copy_assignable<T>::value){{
                               typedef T& (T::*method_t)(const T&);
                               PyllarsClassMethod<name, kwlist, method_t, &T::operator= >();
                            }}
                            return 0;
                        }}
                        
                        typedef const char* const kwlist_t[2];
                        constexpr kwlist_t kwlist = {{"assign_to", nullptr}};
                        constexpr cstring this_name = "this";
                        const int status =  template_set_up<this_name, kwlist, {class_full_cpp_name}>();
                    }}
                """)
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
#include <pyllars/pyllars_classmethod.hpp>

                    """)

            body_stream.write(f"""
                    using namespace pyllars;
                    namespace {{
                        //From: DefaultConstructorDeclGenerator.generate

                        /**
                        * clang does not properly delete default assignment operator, so must use compile-time check
                        * instead to prevent compiler error from generated code that shouldn't be
                        */
                        template<const char* const name, const char* const kwlist[], typename T>
                        static int template_set_up(){{
                            if constexpr (std::is_copy_assignable<T>::value){{
                               typedef T& (T::*method_t)(const T&&);
                               PyllarsClassMethod<name, kwlist, method_t, &T::operator= >();
                            }}
                            return 0;
                        }}
                        
                        typedef const char* const kwlist_t[2];
                        constexpr kwlist_t kwlist = {{"assign_to", nullptr}};
                        constexpr cstring this_name = "this";
                        const int status =  template_set_up<this_name, kwlist, {class_full_cpp_name}>();
                    }}
                """)
        finally:
            body_stream.close()
        return None, body_stream.name


class CXXConstructorDeclGenerator(Generator):

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
        qualifiers = self._node.signature.rsplit(')', maxsplit=1)[-1]
        params = [self._scoped_type_name(p.type_text) for p in self._node.children if isinstance(p, NodeType.ParmVarDecl)]
        if '...' in self._node.signature:
            params.append("...")
        params = ", ".join(params)
        class_qualifier = f"(::{self._node.parent.full_cpp_name}::*)"
        return f"{class_qualifier}({params}) {qualifiers}"

    def generate(self):
        class_name = self._node.parent.name
        parent = self._node.parent
        body_stream = open(
            os.path.join(self.my_root_dir, self._source_path_root, class_name + '::' + self._node.name.replace("/", " div") + self._node.signature + '.cpp'), 'w',
            encoding='utf-8')
        body_stream.write(f"""\n#include \"{self.source_path}\"\n\n""")
        #grand_parent = parent
        #while grand_parent and grand_parent.name:
        #    if isinstance(grand_parent, NodeType.NamespaceDecl):
        #        body_stream.write(f"using namespace {grand_parent.full_cpp_name};\n")
        #    grand_parent = grand_parent.parent
        try:
            #parent_name = parent.name
            # generate body
            body_stream.write(f"""\n#include \"{self.source_path}\" 
#include <pyllars/pyllars_classconstructor.hpp>
                            \n""")

            name = self._node.name
            signature = self._full_signature()
            kwlist = []
            args = []
            has_default = []
            for c in[c for c in self._node.children if isinstance(c, NodeType.ParmVarDecl)]:
                if c.name:
                    kwlist.append(f"\"{c.name}\"")
                args.append(c.type_text)
                has_default.append(c.has_default_value())
            has_default.append(True)
            body_stream.write("namespace{\n")
            body_stream.write(f"    constexpr cstring name = \"{name}\";\n")
            body_stream.write("}\n\n")
            counter = 0
            while has_default[len(args)]:
                args_text = (", " + ", ".join(args)) if args else ""
                kwlist_items = ", ".join(kwlist + ["nullptr"])
                body_stream.write("namespace{\n")
                body_stream.write(f"    static const char* const kwlist{counter}[] = {{{kwlist_items}}};\n")
                body_stream.write("}\n\n")

                body_stream.write(f"template class pyllars::PyllarsClassConstructor<kwlist{counter}, {self._node.parent.full_cpp_name} {args_text}>;")
                counter += 1
                if not args:
                    break
                args = args[:-1]

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
        class_qualifier = f"(::{self._node.parent.full_cpp_name}::*)" if not is_static else "(*)"
        return f"{ret_type} {class_qualifier}({params}) {qualifiers}"

    def generate(self):
        class_name = self._node.parent.name
        parent = self._node.parent
        while parent and not parent.name and isinstance(parent, NodeType.CXXRecordDecl):
            parent = parent.parent
            if not parent:
                return None, None
        body_stream = open(
            os.path.join(self.my_root_dir, self._source_path_root, class_name + '::' + self._node.name.replace("/", " div")  + self._node.signature + '.cpp'), 'w',
            encoding='utf-8')
        body_stream.write(f"""\n#include \"{self.source_path}\"\n\n""")
        grand_parent = parent
        while grand_parent and grand_parent.name:
            if isinstance(grand_parent, NodeType.NamespaceDecl):
                body_stream.write(f"using namespace {grand_parent.full_cpp_name};\n")
            grand_parent = grand_parent.parent
        if self._node.name == "operator=":
            return self.generate_assignment(body_stream)
        if self._node.name.startswith("operator"):
            return self.generate_operator(body_stream)
        try:
            parent_name = parent.name
            # generate body
            if 'static' in self._node.qualifiers:
                method_qualifier = "Static"
                class_param = f"{self._node.parent.full_cpp_name}, "
                body_stream.write(f"""\n#include \"{self.source_path}\" 
#include <pyllars/pyllars_classstaticmethod.hpp>
                            """)
            else:
                method_qualifier = ""
                class_param = ""
                body_stream.write(f"""\n#include \"{self.source_path}\" 
#include <pyllars/pyllars_classmethod.hpp>
                            \n""")

            name = self._node.name
            signature = self._full_signature()
            kwlist = []
            for c in reversed([c for c in self._node.children if isinstance(c, NodeType.ParmVarDecl)]):
                if not c.name:
                    break
                kwlist.insert(0, f"\"{c.name}\"")
            kwlist_items = ", ".join(kwlist + ["nullptr"])
            body_stream.write("namespace{\n")
            body_stream.write(f"    static const char* const kwlist[] = {{{kwlist_items}}};\n")
            body_stream.write(f"    constexpr cstring name = \"{name}\";\n")
            body_stream.write("}\n\n")
            body_stream.write(f"template class pyllars::PyllarsClass{method_qualifier}Method<name, kwlist, {class_param}{signature}, &{self._node.full_cpp_name}>;")
        finally:
            body_stream.close()
        return None, body_stream.name

    def generate_assignment(self, body_stream):
        if 'default_delete' in self._node.qualifiers:
            return None, None
        class_name = self._node.parent.name
        class_full_cpp_name = self._node.parent.full_cpp_name
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
#include <pyllars/pyllars_classmethod.hpp>
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
            body_stream.write("namespace{\n")
            body_stream.write(f"    static const char* const kwlist[] = {{{kwlist_items}}};\n")
            body_stream.write(f"    constexpr cstring name = \"{name}\";\n")
            body_stream.write("}\n\n")
            body_stream.write(f"template class pyllars::PyllarsClassMethod<name, kwlist, {signature},  &::{class_full_cpp_name}::operator= >;")
        finally:
            body_stream.close()
        return None, body_stream.name

    def generate_operator(self, body_stream):
        unary_mapping = {
            '~' : 'pyllars::OpUnaryEnum::INV',
            '+' : 'pyllars::OpUnaryEnum::POS',
            '-' : 'pyllars::OpUnaryEnum::NEG',
        }
        binary_mapping = {
            '+': 'pyllars::OpBinaryEnum::ADD',
            '-': 'pyllars::OpBinaryEnum::SUB',
            '*': 'pyllars::OpBinaryEnum::MUL',
            '/': 'pyllars::OpBinaryEnum::DIV',
            '&': 'pyllars::OpBinaryEnum::AND',
            '|': 'pyllars::OpBinaryEnum::OR',
            '^': 'pyllars::OpBinaryEnum::XOR',
            '<<': 'pyllars::OpBinaryEnum::LSHIFT',
            '>>': 'pyllars::OpBinaryEnum::RSHIFT',
            '%': 'pyllars::OpBinaryEnum::MOD',
            '+=': 'pyllars::OpBinaryEnum::IADD',
            '-=': 'pyllars::OpBinaryEnum::ISUB',
            '*=': 'pyllars::OpBinaryEnum::IMUL',
            '/=': 'pyllars::OpBinaryEnum::IDIV',
            '&=': 'pyllars::OpBinaryEnum::IAND',
            '|=': 'pyllars::OpBinaryEnum::IOR',
            '^=': 'pyllars::OpBinaryEnum::IXOR',
            '<<=': 'pyllars::OpBinaryEnum::ILSHIFT',
            '>>=': 'pyllars::OpBinaryEnum::IRSHIFT',
            '%=': 'pyllars::OpBinaryEnum::IMOD',
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
#include <{self.source_path}>\n""")
            if cpp_op_name == 'Map':
                body_stream.write("#include <pyllars/pyllars_classmapoperator.hpp>\n\n")
                body_stream.write(f"""template class pyllars::PyllarsClassMapOperator<{self._full_signature()}, 
                    &{class_full_cpp_name}::{self._node.name}>;""")
            else:
                body_stream.write("#include <pyllars/pyllars_classoperator.hpp>\n\n")
                body_stream.write(f"""template class pyllars::PyllarsClassOperator<{self._full_signature()}, 
                    &{class_full_cpp_name}::{self._node.name}, {cpp_op_name}>;""")
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
    if parent.parent:
        index = parent.parent.children.index(parent)
        if index < 0:
            raise Exception("Invalid structure in hierarchy")
        is_named_attribute = len(parent.parent.children) -1 > index and isinstance(parent.parent.children[index + 1], NodeType.FieldDecl)
    else:
        is_named_attribute = False
    if parent.name:
        if recursed:
            return f"pyllars_internal::PythonAnonymousClassWrapper< ::{parent.full_cpp_name} >",\
                   f"::{parent.full_cpp_name}", \
                   f"decltype(::{parent.full_cpp_name}::{node.name})",\
                   f"::{parent.full_cpp_name}::{node.name}"
        else:
            return f"pyllars_internal::PythonClassWrapper< ::{parent.full_cpp_name} >", \
                   f"::{parent.full_cpp_name}", \
                   f"decltype(::{parent.full_cpp_name}::{node.name})",\
                   f"::{parent.full_cpp_name}::{node.name}"
    elif is_named_attribute:
        # parent is anonymous type with a named field declaration, so this element is referenced to direct parent (field)
        parent_field_name = parent.parent.children[index + 1].name
        if parent_field_name:
            return f"pyllars_internal::PythonClassWrapper<decltype(::{parent.parent.full_cpp_name}::{parent_field_name})>", \
                   f"decltype(::{parent.parent.full_cpp_name}::{parent_field_name})", \
                   f"decltype(::{parent.parent.full_cpp_name}::{parent_field_name}.{node.name})", \
                   f"decltype(::{parent.parent.full_cpp_name}::{parent_field_name})::{node.name}"
        elif parent.parent.name:
            return f"pyllars_internal::PythonClassWrapper<::{parent.parent.full_cpp_name}>", \
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
            return f"pyllars_internal::PythonClassWrapper<decltype(::{parent.parent.full_cpp_name}::{parent_field_name})>", \
                   f"{parent.parent.full_cpp_name}", \
                   f"decltype(::{parent.parent.full_cpp_name}::{parent_field_name})",\
                   f"::{parent.parent.full_cpp_name}::{parent_field_name}"
        elif parent.parent.name:
            return f"pyllars_internal::PythonClassWrapper<decltype(::{parent.parent.full_cpp_name})>", \
                   f"::{parent.parent.full_cpp_name}",\
                   f"decltype(::{parent.parent.full_cpp_name}::{node.name})",\
                   f"::{parent.parent.full_cpp_name}::{node.name}"
        else:
            return _parent_wrapper_name(parent, node)
    else:
        # parent is anonymous type without associated field, so element belongs to parent's parent when referenced in code
        if parent.parent.name:
            return f"pyllars_internal::PythonAnonymousClassWrapper< ::{parent.parent.full_cpp_name} >", \
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
        if 'public' not in self._node.qualifiers and\
                (self._node.parent is None or not hasattr(self._node.parent, 'qualifiers')\
                 or 'struct' not in self._node.parent.qualifiers):
            return None, None
        if isinstance(self._node, NodeType.IndirectFieldDecl):
            return None, None
        if not self._node.name:
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
            os.path.join(self.my_root_dir, self._source_path_root,
                         (parent.name or f"anon_{parent.node_id}") + '  ' +
                         (self._node.name or "anon_" + self._node.node_id) + '.cpp'), 'w',
            encoding='utf-8')
        try:
            parent_name = parent.name
            parent_header_path = os.path.join("..", parent_name)
            # generate body
            if 'static' in self._node.qualifiers:
                member_qualifier = "Static"
            else:
                member_qualifier = ""
            body_stream.write(f"""\n#include \"{self.source_path}\" 
#include \"{parent_header_path}.hpp\"
#include <pyllars/pyllars_class{member_qualifier.lower()}member.hpp>
                                    """)
            if not self._node.name:
                return None, None
            wrapper, parent_type_name, attribute_type_name, attribute_full_cpp_name = _parent_wrapper_name(self._node)
            body_stream.write("using namespace pyllars;\n\nnamespace{\n")
            body_stream.write(f"    constexpr cstring name = \"{self._node.name}\";\n")
            body_stream.write("}\n\n")
            body_stream.write(f"template class pyllars::PyllarsClass{member_qualifier}Member<name, {parent.full_cpp_name}, {attribute_type_name}, &{attribute_full_cpp_name}>;")
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
            os.path.join(self.my_root_dir, self._source_path_root, (self._node.parent.name or f"anon_{self._node.parent.node_id}") + '  ' +
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
#include <pyllars/pyllars_classbitfield.hpp>
            """)

            name = self._node.name
            if not name:
                return None, None
            typename = self._scoped_type_name(self._node.type_text)
            const_typename = 'const ' + typename if 'const' not in typename.split() else typename
            setter = "" if is_const else f"static std::function<{typename}({parent_type_name}&, {const_typename}&)> setter = []({parent_type_name} & obj, {const_typename}& value)->{typename}{{obj.{name} = value; return value;}};"
            body_stream.write(f"""
namespace{{
    extern const char name[] = "{name}";
    static std::function<{typename}(const {parent_type_name}&)> getter = [](const {parent_type_name} & obj)->{typename}{{return  obj.{name};}};
    constexpr std::function<{typename}(const {parent_type_name}&)>* getter_p = &getter;
""")
            if setter:
                body_stream.write(f"""
    {setter}
    constexpr std::function<{typename}({parent_type_name}&, {const_typename}&)>* setter_p = &setter;
""")
            body_stream.write("}\n\n")
            body_stream.write(f"""template class pyllars::PyllarsClassBitField<name, {parent_type_name}, {typename}, {size}, getter_p, {"setter_p" if setter else "nullptr"}>;""")

        finally:
            body_stream.close()
        return None, body_stream.name


class VarDeclGenerator(FieldDeclGenerator):
    pass


class IndirectFieldDeclGenerator(FieldDeclGenerator):
    pass
