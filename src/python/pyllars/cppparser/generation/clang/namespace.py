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
            header_stream.write(f"#ifndef _{self._node.full_cpp_name.replace('::', '__')}\n")
            header_stream.write(f"#define _{self._node.full_cpp_name.replace('::', '__')}\n")
            if parent and parent.name:
                header_stream.write(f"#include \"../{parent.name}.hpp\"\n\n")
            header_stream.write("namespace __pyllars_internal{\n")
            header_stream.write("    namespace names{\n")
            header_stream.write(self._wrap_in_namespaces(f"""

            namespace {self._node.name}{{
                    struct Tag_{self._node.name}{{
                        typedef const char* const cstring;
                        static constexpr cstring name = \"{self._node.name}\";
                    }};
            }}
            \n""", False))
            header_stream.write("    }\n")
            header_stream.write("}\n")
            header_stream.write("#endif")

            # generate body
            body_stream.write('#include <pyllars/pyllars_namespacewrapper.hpp>\n')
            body_stream.write(f"#include <{self.source_path}>\n\n")
            body_stream.write(f"#include \"{self._node.name}.hpp\"\n\n")
            body_stream.write("using namespace pyllars;\n")

            def namespace_wrapper(node: NodeType.Node):
                if not node or not node.name:
                    return "GlobalNamespace"
                return f"PyllarsNamespace< __pyllars_internal::names::{node.full_cpp_name}::Tag_{node.name}, {namespace_wrapper(node.parent)} > " if \
                    node.parent and node.parent.name else "GlobalNamespace"

            body_stream.write(f"template class pyllars::PyllarsNamespace<__pyllars_internal::names::{self._node.full_cpp_name}::Tag_{self._node.name}, {namespace_wrapper(self._node.parent)}>;\n")
        finally:
            body_stream.close()
            header_stream.close()
        return header_stream.name, body_stream.name