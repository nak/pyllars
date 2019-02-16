from pyllars.cppparser.generation.base2 import GeneratorBody, GeneratorHeader
from pyllars.cppparser.parser import code_structure
from .base import qualified_name


class GeneratorBodyTypedefDecl(GeneratorBody):

    def generate(self):
        with self._scoped(self._stream, indent=b"            "):
            self._stream.write(self.decorate(self.INITIALIZER_CODE% {
                'name': self._element.name,
                'parent_name': self._element.parent.name or "pyllars"
            }).encode('utf-8'))

            self._stream.write(self.decorate(self.INITIALIZER_INSTANTIATION_CODE % {
                'name': self._element.name
            }).encode('utf-8'))
            suffix = + b'_mod' if isinstance(self._element, code_structure.NamespaceDecl) else b''
            self._stream.write(b"""
                
                status_t %(name)s_init(PyObject* global_mod){
                     static bool %(name)s_inited = false;
                     if (%(name)s_inited) return 0;// if already initialized
                     %(name)s_inited = true;
                    
                     int status = 0;
                     PyTypeObject* obj = __pyllars_internal::PythonClassWrapper<%(fullname)s>::getPyType();
                     if (!%(parent_mod)s){
                         status = -2;
                     } else {
                        PyObject_SetAttrString(%(parent_mod)s, "%(name)s", (PyObject*)obj);
                    } 
                    return status;
                } // end init
                
                """ % {
                b'parent_mod': self._element.parent.full_name.encode('utf-8') + suffix if
                    self._element.parent and self._element.parent.name else b"global_mod",
                b'name': self._element.name.encode('utf-8'),
                b'fullname': self._element.full_name.encode('utf-8'),
            })

            self._stream.write(b"""
                /**
                 * For children to register initializers to be called before this namespace PyObject is inited
                 **/
                int %(name)s_register( pyllars::Initializer* const init ){
                    return 0;
                }
                 
                """ % {
                b'name': self._element.name.encode('utf-8'),
                b'pyllars_scope': self._element.pyllars_scope.encode('utf-8'),
                b'parent_name': (self._element.scope if self._element.scope != '::' else "").encode('utf-8'),
                b'parent': qualified_name(self._element.parent.name).encode('utf-8'),

            })


class GeneratorHeaderNamespaceDecl(GeneratorHeader):

    def generate_spec(self):
        super().generate_spec()
        self._stream.write(b"""
           PyObject * %(name)s_module();
        """ % {b'name': self._element.name.encode('utf-8')})


class GeneratorBodyNamespaceDecl(GeneratorBody):

    def generate(self):
        with self._scoped(self._stream, indent=b"            "):
            self._stream.write(b"""

                """ % {b'name': self._element.name.encode('utf-8'),
                       b'pyllars_scope': self._element.pyllars_scope.encode('utf-8')})
            self._stream.write(b"""
                PyObject * %(name)s_module(){
                    static PyObject* %(name)s_mod = nullptr;
                    if (!%(name)s_mod){
                        #if PY_MAJOR_VERSION==3
    
                            // Initialize Python3 module associated with this namespace
                            static PyModuleDef %(name)s_moddef = {
                                PyModuleDef_HEAD_INIT,
                                "%(name)s",
                                "Example module that creates an extension type.",
                                -1,
                                NULL, NULL, NULL, NULL, NULL
                            };
                            %(name)s_mod = PyModule_Create(&%(name)s_moddef);
    
                        #else
    
                            // Initialize Python2 module associated with this namespace
                            %(name)s_mod = Py_InitModule3("%(name)s", nullptr,
                                                          "Module corresponding to C++ namespace %(fullname)s");
    
                        #endif
                    }
                    return %(name)s_mod;
                }
                
                status_t %(name)s_init(PyObject* global_mod){
                    static bool inited = false;
                    if (inited) return 0;// if already initialized
                    inited = true;
                    int status = 0;

                    if (!%(parent_mod)s || !%(name)s_module()){
                        status = -2;
                    } else {
                        PyModule_AddObject( %(parent_mod)s, "%(name)s", %(name)s_module());
                    } 
                    return status;
                } // end init

                """ % {
                b'name': self._element.name.encode('utf-8'),
                b'fullname': self._element.full_name.encode('utf-8'),
                b'parent_mod': self._element.parent.name.encode('utf-8') + b'_module()' if \
                    self._element.parent and self._element.parent.name else b"global_mod"
            })
            self._stream.write((self.INITIALIZER_CODE % {
                'name': self._element.name,
                'parent_name': self._element.parent.name if self._element.parent.name else "pyllars"
            }).encode('utf-8'))
            self._stream.write((self.INITIALIZER_INSTANTIATION_CODE % {
                'name': self._element.name,
            }).encode('utf-8'))
            self._stream.write((self.REGISTRATION_CODE % {
                'name': self._element.name,
            }).encode('utf-8'))

