import os

from pyllars.cppparser.parser.clang_translator import NodeType
from .generator import Generator


class NamespaceDeclGenerator(Generator):

    def generate(self):
        header_stream = open(os.path.join(self.my_root_dir, self._node.name+'.hpp'), 'w',
                             encoding='utf-8')
        body_stream = open(os.path.join(self.my_root_dir, self._source_path_root, self._node.name+'.cpp'), 'w',
                           encoding='utf-8')
        try:
            parent = self._node.parent
            if parent and parent.name:
                ns_parent = "using ParentNS = pyllars::NSInfo<parent_fullnsname>;"
            else:
                ns_parent = f"using ParentNS = pyllars::GlobalNS;"

            # generate body
            body_stream.write('#include <pyllars/pyllars_namespacewrapper.hpp>\n')
            body_stream.write(f"#include <{self.source_path}>\n\n")
            body_stream.write(f"""

namespace {{
    extern const char full_name[] = "{self._node.full_cpp_name}";
    extern const char parent_fullnsname[] = "{self._node.parent.full_cpp_name if self._node.parent and self._node.parent.name else ""}";
    using NS =  pyllars::NSInfo<full_name>;
    {ns_parent}
}}
            
            """)

            body_stream.write(f"template class pyllars::PyllarsNamespace<NS, ParentNS>;\n")
        finally:
            body_stream.close()
            header_stream.close()
        return header_stream.name, body_stream.name