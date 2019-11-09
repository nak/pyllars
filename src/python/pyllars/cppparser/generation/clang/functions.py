import os
from typing import List

from pyllars.cppparser.parser.clang_translator import NodeType
from .generator import Generator


class FunctionDeclGenerator(Generator):
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
                    return "::" + ("::".join([parent.full_cpp_name, rest]) if rest else "::" + parent.full_cpp_name)
                # possibly an internally defined class or type:
                if parent and hasattr(parent, "children"):
                    for child in parent.children:
                        if hasattr(child, 'name') and child.name == first:
                            return '::' + child.full_cpp_name + ("::" + rest if rest else "")
                parent = parent.parent
            return "::" + t

        for index, typ in enumerate(parts):
            base_type = typ
            while base_type and base_type[0] in ['*', '&']:
                base_type = base_type[1:]
            if not base_type in self.KEYWORDS and base_type:
                parts[index] = full_name(typ)
        return ' '.join(parts)

    def _full_signature(self):
        ret_type = self._scoped_type_name(self._node.spec.split('(')[0])
        qualifiers = self._node.spec.rsplit(')', maxsplit=1)[-1]
        params = [self._scoped_type_name(p.type_text) for p in self._node.children if isinstance(p, NodeType.ParmVarDecl)]
        if '...' in self._node.spec:
            params.append("...")
        params = ", ".join(params)
        return f"{ret_type} ({params}) {qualifiers}"

    def generate(self):
        if 'implicit' in self._node.prequalifiers:
            return None, None
        header_stream = open(os.path.join(self.my_root_dir, self._node.name + '.hpp'), 'w',
                             encoding='utf-8')
        body_stream = open(os.path.join(self.my_root_dir, self._source_path_root, self._node.name + '.cpp'), 'w',
                           encoding='utf-8')

        parent = self._node.parent
        name = self._node.name
        try:
            # generate body
            body_stream.write(f"""\n
#include <pyllars/pyllars_function.hpp>
#include "{self.source_path}" 

            """)

            def namespace_wrapper(node: NodeType.Node):
                if not node or not node.name:
                    return "GlobalNamespace"
                return f"PyllarsNamsepace< {node.name}_ name, {namespace_wrapper(node.parent)} > "

            names = []
            while n and n.name:
                if n.name not in names:
                    names.append(n.name)
                    body_stream.write(f"    constexpr cstring {n.name}_name = \"{n.name}\";\n")
                n = n.parent
            body_stream.write("}\n")
            body_stream.write("using namespace pyllars;\n")

            params = [c for c in self._node.children if isinstance(c, NodeType.ParmVarDecl)]
            param_list = ",".join([f"\"{p.name}\"" for p in params] + ["nullptr"])
            full_signature = self._full_signature()
            body_stream.write("using namespace pyllars;\n")
            body_stream.write(f"static const char* kwlist[{len(params) +1}] = {{{param_list}}};\n")
            body_stream.write(f"constexpr cstring fname = \"{self._node.name}\";\n")
            body_stream.write(f"template class PyllarsFunction<fname, kwlist, {full_signature}, &{self._node.full_cpp_name}, {namespace_wrapper(self._node.parent)}>;\n")
        finally:
            header_stream.close()
            body_stream.close()
        return header_stream.name, body_stream.name

