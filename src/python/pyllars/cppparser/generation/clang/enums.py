import os
from typing import List

from pyllars.cppparser.parser.clang_translator import NodeType
from .generator import Generator


class EnumDeclGenerator(Generator):

    def generate(self):
        if 'class' in self._node.qualifiers:
            scoped = f"{self._node.full_cpp_name}::"
        else:
            scoped = self._node.parent.full_cpp_name + "::" if self._node.parent else ""
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
namespace pyllars_internal{{
                          
    template<>
    struct DLLEXPORT TypeInfo<{full_cpp_name}>{{
        static constexpr char const type_name[] = \"{name}\";
    }};
    
}}
            \n\n""")

            def namespace_wrapper(node):
                if node is None:
                    return "pyllars::GlobalNS"
                if not node or not node.name:
                    return "pyllars::GlobalNS"
                body_stream.write(f"""
namespace{{
/*extern*/ const char parent_nsname[] = \"{node.full_cpp_name}\";
}}
""")
                return f"pyllars::NSInfo<parent_nsname>"

            if 'definition' in self._node.qualifiers or 'implicit' in self._node.qualifiers:
                return header_stream.name, body_stream.name
            named_parent = parent
            while named_parent and (not hasattr(named_parent, "name") or not named_parent.name):
                named_parent = named_parent.parent
                if isinstance(named_parent, NodeType.TranslationUnitDecl):
                    named_parent = None
            if hasattr(self._node, "name") and self._node.name:
                body_stream.write(f"""
namespace{{
    extern const char  enum_type_name[] = "{self._node.name}";
}}
""")
                typename = self._node.full_cpp_name
            else:
                body_stream.write(f"""
namespace{{
    extern const char enum_type_name[] = "<<anon_{self._node.node_id}>>";
}}
""")
                typename = f"decltype({named_parent.full_cpp_name}::{elements[0].name})"
            vnames = []
            values = []
            for elem in elements:
                vnames.append(f"""\"{elem.name}\"""")
                values.append(f"{scoped}{elem.name}")
            vnames_str = f"{{{', '.join(vnames)}}}"
            values_str = f"{', '.join(values)}"
            body_stream.write("namespace {\n")
            body_stream.write(f"    constexpr const char* vnames[] = {vnames_str};\n")
            body_stream.write("}\n\n")

            if not named_parent or isinstance(named_parent, NodeType.TranslationUnitDecl):
                template_instantiation = f"template class pyllars::PyllarsEnum<enum_type_name, {typename}, pyllars::GlobalNS, vnames, {values_str}>"

            elif isinstance(named_parent, NodeType.NamespaceDecl):
                template_instantiation = f"template class pyllars::PyllarsEnum<enum_type_name, {typename}, {namespace_wrapper(parent)}, vnames, {values_str}>"
            else:
                template_instantiation = f"template class pyllars::PyllarsEnum<enum_type_name, {typename}, {parent.full_cpp_name}, vnames, {values_str}>"

            body_stream.write(f"{template_instantiation};\n")
        finally:
            header_stream.close()
            body_stream.close()
        return header_stream.name, body_stream.name

