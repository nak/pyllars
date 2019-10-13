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
#include <pyllars/pyllars_classwrapper.impl.hpp>
#include "{self.source_path}" 
#include \"{name}.hpp"
            """)
            if self._node.parent and parent_name != 'pyllars':
                body_stream.write(f"\n#include \"..{os.sep}{parent_name}.hpp\"\n")
            body_stream.write(f"""
                    namespace __pyllars_internal{{
                        template<>
                        struct _Types<{full_cpp_name}>{{
                            static const char* const type_name;
                        }};
                        const char* const _Types<{full_cpp_name}>::type_name =  "{name}";
                    }}
            """)

            if not self._node.name or 'definition' in self._node.qualifiers or 'implicit' in self._node.qualifiers:
                return header_stream.name, body_stream.name
            named_parent = parent
            while named_parent and (not hasattr(named_parent, "name'") or not named_parent.name):
                named_parent = named_parent.parent
                if isinstance(named_parent, NodeType.TranslationUnitDecl):
                    named_parent = None

            if named_parent:
                parent_full_name = named_parent.full_cpp_name
                add_pyobject_code = f"""
                                       status |= ::pyllars::{parent_full_name}_addPyObject("{name}", 
                                                    (PyObject*) __pyllars_internal::PythonClassWrapper< typename ::{full_cpp_name} >::getPyType());

"""
            else:
                parent_full_name = ""
                add_pyobject_code = f"""
                                      PyObject *pyllars_mod = PyImport_ImportModule("pyllars");
                                      status |= PyModule_AddObject(top_level_mod, "{name}",
                                         (PyObject*) __pyllars_internal::PythonClassWrapper< typename ::{full_cpp_name} >::getPyType());
                                      if (pyllars_mod){{
                                         status |= PyModule_AddObject(pyllars_mod, "{name}",
                                            (PyObject*) __pyllars_internal::PythonClassWrapper< typename ::{full_cpp_name} >::getPyType());
                                      }}

"""
            setup_code = ""
            for elem in elements:
                if name:
                    setup_code += f"""
                       status |= __pyllars_internal::PythonClassWrapper<  ::{full_cpp_name} >::addEnumValue("{elem.name}", ::{c_scope}{self._node.name}{"::" if self._node.name else ""}{elem.name});
                   """
                elif named_parent and not isinstance(named_parent, NodeType.NamespaceDecl):
                    setup_code += f"""
                    status |= __pyllars_internal::PythonClassWrapper<  ::{named_parent.full_cpp_name} >::addEnumValue("{elem.name}", ::{c_scope}{self._node.name}{"::" if self._node.name else ""}{elem.name});
                """
                elif named_parent:
                    setup_code += f"""
                    status |=  PyModule_AddObject(pyllars::{named_parent.full_cpp_name}_module(), "{name}",   __pyllars_internal::toPyObject(::{c_scope}{self._node.name}{"::" if self._node.name else ""}{elem.name}, 1));
                """
                else:
                    setup_code += f"""
                     status |= PyModule_AddObject(top_level_mod, "{name}", __pyllars_internal::toPyObject( ::{c_scope}{self._node.name}{"::" if self._node.name else ""}{elem.name}, 1));
                """
            body_stream.write(self._wrap_in_namespaces(f"""
            namespace {{
                //From: EnumDeclGenerator.generate

                class Initializer_{name}: public pyllars::Initializer{{
                public:
                    Initializer_{name}():pyllars::Initializer(){{
                        pyllars::{parent_full_name or "pyllars"}_register(this);
                        if (__pyllars_internal::PythonClassWrapper< ::{full_cpp_name} >::initialize() != 0){{
                            PyErr_SetString(PyExc_SystemError, "Internal error when adding method/constructor");
                        }}                           
                    }}

                    int set_up() override{{
                        return 0;
                    }}

                    int ready(PyObject * const top_level_mod) override{{
                       static int status = 0;
                       static bool readied = false;
                       if (readied){{ return status; }}
                       readied = true; 
                       {add_pyobject_code}
                       {setup_code}
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
            header_stream.close()
            body_stream.close()
        return header_stream.name, body_stream.name

