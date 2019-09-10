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
        header_stream = open(os.path.join(self.my_root_dir, self._node.name + '.hpp'), 'w',
                             encoding='utf-8')
        body_stream = open(os.path.join(self.my_root_dir, self._source_path_root, self._node.name + '.cpp'), 'w',
                           encoding='utf-8')

        parent = self._node.parent
        parent_name = parent.name if parent else "pyllars"
        parent_full_name = parent.full_cpp_name if parent else "pyllars"
        name = self._node.name
        full_cpp_name = self._node.full_cpp_name
        try:
            # generate header
            header_stream.write(Generator.COMMON_HEADER)
            header_stream.write(self._wrap_in_namespaces(f"""
            """, True))

            # generate body
            body_stream.write(f"""\n
#include <pyllars/pyllars_function_wrapper.hpp>
#include "{self.source_path}" 
#include \"{self._node.name}.hpp"
            """)
            if self._node.parent:
                body_stream.write(f"\n#include \"..{os.sep}{parent_name}.hpp\"\n")
            params = [c for c in self._node.children if isinstance(c, NodeType.ParmVarDecl)]
            param_count = len(params)
            param_list = ",".join([f"\"{p.name}\"" for p in params] + ["nullptr"])
            full_signature = self._full_signature()
            namec = name.replace(' ', '______').replace('[]', '_____array')
            if not parent or not parent.name:
                addobj_code = f"""
                    PyObject *pyllars_mod = PyImport_ImportModule("pyllars");
                    status |= PyModule_AddObject(top_level_mod, "{name}", pyobj);
                    status |= PyModule_AddObject(pyllars_mod, "{name}", pyobj);
                """
            else:
                addobj_code = f"status |= pyllars::{parent_full_name}_addPyObject(\"{name}\", pyobj);"
            body_stream.write(self._wrap_in_namespaces(f"""
            namespace {{
                //From: FunctionDeclGenerator.generate
                typedef const char* const kwlist_t[{param_count+1}];
                static constexpr kwlist_t kwlist = {{{param_list}}};
                class Initializer_{namec}: public pyllars::Initializer{{
                public:
                    Initializer_{namec}():pyllars::Initializer(){{

                        pyllars::{parent_full_name}_register(this);                          
                    }}

                    int set_up() override{{
                        static int status = -1;
                        using namespace __pyllars_internal;
                        static bool inited = false;
                        if (inited){{
                            return status;
                        }}
                        status = 0;
                       
                        inited = true;
                        return status;
                    }}

                    int ready(PyObject * const top_level_mod) override{{
                       int status = pyllars::Initializer::ready(top_level_mod);
                        PyObject* pyobj = (PyObject*)__pyllars_internal::PythonFunctionWrapper<{full_signature}>::createPy<kwlist, ::{full_cpp_name}>("{name}"); 
                        {addobj_code}
                        return status;
                    }}

                    static Initializer_{namec}* initializer;

                    static Initializer_{namec} *singleton(){{
                        static  Initializer_{namec} _initializer;
                        return &_initializer;
                    }}
                 }};


                //ensure instance is created on global static initialization, otherwise this
                //element would never be reigstered and picked up
                Initializer_{namec} * Initializer_{namec}::initializer = singleton();

            }}
                """, True))

        finally:
            header_stream.close()
            body_stream.close()
        return header_stream.name, body_stream.name

