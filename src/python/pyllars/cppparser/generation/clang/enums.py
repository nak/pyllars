import os
from typing import List

from pyllars.cppparser.parser.clang_translator import NodeType
from .generator import Generator


class EnumDeclGenerator(Generator):

    def generate(self):
        elements = [c for c in self._node.children if isinstance(c, NodeType.EnumConstantDecl)]
        if 'implicit' in self._node.qualifiers or not elements:
            return None, None
        parent = self._node.parent
        while hasattr(parent, 'name') and not parent.name:
            parent = parent.parent
        parent_name = parent.name if parent else "pyllars"
        c_scope = parent.full_cpp_name + "::" if parent and parent.full_cpp_name else ""
        if self._node.name:
            name = self._node.name
            full_cpp_name = self._node.full_cpp_name
        else:
            name = f"decltype(::{c_scope}{elements[0].name})"
            full_cpp_name = f"decltype(::{c_scope}{elements[0].name})"
        header_stream = open(os.path.join(self.my_root_dir, name + '.hpp'), 'w',
                             encoding='utf-8')
        body_stream = open(os.path.join(self.my_root_dir, self._source_path_root, name + '.cpp'), 'w',
                           encoding='utf-8')

        try:
            # generate header
            header_stream.write(Generator.COMMON_HEADER)
            header_stream.write(self._wrap_in_namespaces(f"""
            """, True))

            # generate body
            body_stream.write(f"""\n
#include <pyllars/pyllars_enum.hpp>
#include "{self.source_path}" 
#include \"{name}.hpp"
            """)
            if self._node.parent and parent_name != 'pyllars':
                body_stream.write(f"\n#include \"..{os.sep}{parent_name}.hpp\"\n")
            body_stream.write(f"""
namespace __pyllars_internal{{
                          
    template<>
    const char* const TypeInfo<{full_cpp_name}>::type_name = \"{name}\";
    
}}
            """)
            body_stream.write("using namespace pyllars;\n")
            body_stream.write("using namespace __pyllars_internal;\n")

            def namespace_wrapper(node):
                if node is None:
                    return "GlobalNamespace"
                if isinstance(node, NodeType.NamespaceDecl):
                    if not node or not node.name:
                        return "GlobalNameSpace"
                    return f"PyllarsNamespace< names::{node.full_cpp_name}::Tag_{node.name}, {namespace_wrapper(node.parent)} > "
                else:
                    return f"PyllarsClass< {node.full_cpp_name}, {namespace_wrapper(node.parent)} >;\n"


            if not self._node.name or 'definition' in self._node.qualifiers or 'implicit' in self._node.qualifiers:
                return header_stream.name, body_stream.name
            named_parent = parent
            while named_parent and (not hasattr(named_parent, "name'") or not named_parent.name):
                named_parent = named_parent.parent
                if isinstance(named_parent, NodeType.TranslationUnitDecl):
                    named_parent = None
            if hasattr(self._node, "name") and self._node.name:
                body_stream.write(f"""
namespace{{
    extern const char* const enum_type_name = "{self._node.name}";
}}
""")
                if not named_parent or isinstance(named_parent, NodeType.TranslationUnitDecl):
                    template_instantiation = f"template class pyllars::PyllarsEnum<enum_type_name, {self._node.full_cpp_name}, pyllars::GlobalNamespace>"

                elif isinstance(named_parent, NodeType.NamespaceDecl):
                    template_instantiation = f"template class pyllars::PyllarsEnum<enum_type_name>, {self._node.full_cpp_name}, {namespace_wrapper(parent)}>"
                else:
                    template_instantiation = f"template class pyllars::PyllarsEnum<enum_type_name, {self._node.full_cpp_name}, {parent.full_cpp_name}>"

                body_stream.write(f"{template_instantiation};")
                body_stream.write(f"using EnumWrapper = {template_instantiation};")
            for elem in elements:
                body_stream.write(f"template EnumWrapper::template Value<{elem.name}>;")

        finally:
            header_stream.close()
            body_stream.close()
        return header_stream.name, body_stream.name

