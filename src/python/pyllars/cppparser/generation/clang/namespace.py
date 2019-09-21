import os
from .generator import Generator


class NamespaceDeclGenerator(Generator):

    def generate(self):
        header_stream = open(os.path.join(self.my_root_dir, self._node.name+'.hpp'), 'w',
                             encoding='utf-8')
        body_stream = open(os.path.join(self.my_root_dir, self._source_path_root, self._node.name+'.cpp'), 'w',
                           encoding='utf-8')
        try:
            # generate header
            header_stream.write(Generator.COMMON_HEADER)
            parent = self._node.parent
            code = f"""
            PyObject *{self._node.name}_module(); 
            
            /**
             * static initializer method to register initialization routine
             **/
            status_t {self._node.name}_register(::pyllars::Initializer* const);
            
            /**
             * called back on initialization to initialize Python wrapper for this C construct
             * @param top_level_mod:  mod to which the wrapper Python object should belong
             **/
            status_t {self._node.name}_ready(PyObject * const top_level_mod);
            
            /**
             * add child object
             **/
            status_t {self._node.name}_addPyObject(const char* const name, PyObject* pyobj){{
                return PyModule_AddObject(::pyllars::{self._node.full_cpp_name}_module(), name, pyobj);
            }}
            """
            header_stream.write(self._wrap_in_namespaces(code, True))
            # generate body
            parent_mod = f"::pyllars::{parent.full_cpp_name}_module()" if parent else "global_mod"
            hash = "#"
            code = f"""
            
                    PyObject * {self._node.name}_module(){{
                        static PyObject* {self._node.name}_mod = nullptr;
                        if (!{self._node.name}_mod){{
                            {hash}if PY_MAJOR_VERSION==3
        
                                // Initialize Python3 module associated with this namespace
                                static PyModuleDef {self._node.name}_moddef = {{
                                    PyModuleDef_HEAD_INIT,
                                    "{self._node.name}",
                                    "Example module that creates an extension type.",
                                    -1,
                                    NULL, NULL, NULL, NULL, NULL
                                }};
                                {self._node.name}_mod = PyModule_Create(&{self._node.name}_moddef);
        
                            {hash}else
        
                                // Initialize Python2 module associated with this namespace
                                {self._node.name}_mod = Py_InitModule3("{self._node.name}", nullptr,
                                                              "Module corresponding to C++ namespace {self._node.name}");
        
                            {hash}endif
                        }}
                        return {self._node.name}_mod;
                    }}
            
                    
                    status_t {self._node.name}__ready(PyObject* global_mod){{
                      static bool inited = false;
                        if (inited) return 0;// if already initialized
                        inited = true;
                        int status = 0;
    
                        if (!{parent_mod} || {self._node.name}_module()){{
                            status = -2;
                        }} else {{
                            PyModule_AddObject( {parent_mod}, "{self._node.name}", {self._node.name}_module());
                        }}
                        return status;
                    }}
                    
                    status_t {self._node.name}_set_up(){{
                        return {self._node.name}_module()?0:-2;
                    }} // end init
    
            """
            code += self.INITIALIZER_CODE % {
                'name': self._node.name,
                'parent_name': self._node.parent.full_cpp_name if self._node.parent else "pyllars"
            }
            code += self.INITIALIZER_INSTANTIATION_CODE % {
                'name': self._node.name,
            }
            code += self.REGISTRATION_CODE % {
                'name': self._node.name,
            }
            body_stream.write(f"#include \"{self._node.name}.hpp\"\n")
            if self._node.parent:
                body_stream.write(f"#include \"../{self._node.parent.name}.hpp\"\n")
            body_stream.write(self._wrap_in_namespaces(code, True))

        finally:
            body_stream.close()
            header_stream.close()
        return header_stream.name, body_stream.name