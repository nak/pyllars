from io import TextIOBase

from pyllars.cppparser.generation.base2 import GeneratorBody
from .base import Generator, qualified_name


class NamespaceDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return True

    def generate_header_core(self, stream: TextIOBase, as_top=False) -> None:
        if self.element.name:
            stream.write(("""
        extern PyModuleObject *%s_mod;
    """ % self.element.name).encode('utf-8'))

    def generate_spec(self):
        file_name = self.header_file_name()
        if self.element.name == "::" or not self.element.name:
            with self.folder.open(file_name) as stream:
                stream.write(b"")
                return
        self.folder.purge(file_name)
        with self.folder.open(file_name=file_name) as stream:
            with self.guarded(stream) as guarded:
                guarded.write(self.basic_includes())
                if self.parent:
                    guarded.write(("""#include "%s" """ % self.parent.header_file_path()).encode("utf-8"))
                    guarded.write(("""#include "%s" """ % self._src_path ).encode("utf-8"))
                with self.scoped(guarded) as scoped:
                    scoped.write(("""
                        int %(qname)s_register( pyllars::Initializer* const);
                        extern PyObject* %(name)s_mod;
                    """ % {'name': self.sanitize(self.element.name),
                           'qname': qualified_name(self.element.name),
                           }).encode('utf-8'))

    def generate_body_proper(self, scoped: TextIOBase, as_top: bool = False) -> None:
        if not self.element.parent:
            return

        scoped.write(("""
            using namespace %(pyllars_scope)s::%(name)s;
            status_t %(name)s_init(PyObject* global_mod){
                if (%(name)s_mod) return 0;// if already initialized
                int status = 0;
                #if PY_MAJOR_VERSION==3
                
                    // initialize Python3 module assocaited with this namespace
                    static PyModuleDef %(name)s_moddef = {
                        PyModuleDef_HEAD_INIT,
                        "%(basic_name)s",
                        "Example module that creates an extension type.",
                        -1,
                        NULL, NULL, NULL, NULL, NULL
                    };
                    %(name)s_mod = PyModule_Create(&%(name)s_moddef);
                #else
                    // initialize Python3 module assocaited with this namespace
                    %(name)s_mod = Py_InitModule3("%(name)s", nullptr,
                                                  "Module corresponding to C++ namespace %(fullname)s");
                #endif

                if(! %(name)s_mod ){
                    status = -1;
                } else {
                    if (!%(parent_mod)s){
                        status = -2;
                    } else {
                        PyModule_AddObject( %(parent_mod)s, "%(basic_name)s", %(basic_name)s_mod);
                    } 
                }
                return status;
            } // end init
            
            
            class Initializer_%(basic_name)s: public pyllars::Initializer{
            public:
                Initializer_%(basic_name)s():pyllars::Initializer(){
                  
                }

                virtual int init(PyObject * const global_mod){
                   int status = %(basic_name)s_init(global_mod);
                   return status == 0? pyllars::Initializer::init(global_mod) : status;
                }

             };
             
             
             
            int %(pyllars_scope)s::%(name)s::%(name)s_register( pyllars::Initializer* const init ){
                static Initializer_%(basic_name)s _initializer = Initializer_%(basic_name)s();
                static int status = pyllars%(parent_name)s::%(parent)s_register(&_initializer);
                return status==0?_initializer.register_init(init):status;
             }


             PyObject *%(pyllars_scope)s::%(name)s::%(name)s_mod = nullptr;

""" % {
            'name': self.sanitize(self.element.name),
            'basic_name': self.element.name,
            'parent_mod': self.element.parent.python_cpp_module_name if self.element.parent else "pyllars_mod",
            'pyllars_scope': self.element.pyllars_scope,
            'fullname': self.element.full_name,
            'parent': qualified_name(
                self.element.parent.name if self.element.parent.name and self.element.parent.name != "::" else "pyllars"),
            'parent_name': (self.element.scope if self.element.scope != '::' else ""),
        }).encode('utf-8'))


class GeneratorBodyNamespaceDecl(GeneratorBody):

    def generate(self):
        with self._scoped(self._stream, indent=b"            "):
            self._stream.write(b"""
                using namespace %(pyllars_scope)s::%(name)s;
                
                """ % {b'name': self._element.name.encode('utf-8'),
                       b'pyllars_scope': self._element.pyllars_scope.encode('utf-8')})
            self._stream.write(b"""
                status_t %(name)s_init(PyObject* global_mod){
                    if (%(name)s_mod) return 0;// if already initialized
                    int status = 0;
                    
                    #if PY_MAJOR_VERSION==3
    
                        // Initialize Python3 module associated with this namespace
                        static PyModuleDef %(name)s_moddef = {
                            PyModuleDef_HEAD_INIT,
                            "%(basic_name)s",
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
                    if(! %(name)s_mod ){
                        status = -1;
                    } else {
                        if (!%(parent_mod)s){
                            status = -2;
                        } else {
                            PyModule_AddObject( %(parent_mod)s, "%(basic_name)s", %(basic_name)s_mod);
                        } 
                    }
                    return status;
                } // end init
                
                """ % {
                b'name': self._element.name.encode('utf-8'),
                b'basic_name': self._element.name.encode('utf-8'),
                b'fullname': self._element.full_name.encode('utf-8'),
                b'parent_mod': self._element.parent.name.encode('utf-8') + b'_mod' if \
                    self._element.parent and self._element.parent.name else b"pyllars_mod"
            })
            self._stream.write(b"""
                class Initializer_%(basic_name)s: public pyllars::Initializer{
                public:
                    Initializer_%(basic_name)s():pyllars::Initializer(){
                      
                    }
    
                    virtual int init(PyObject * const global_mod){
                       int status = %(basic_name)s_init(global_mod);
                       return status == 0? pyllars::Initializer::init(global_mod) : status;
                    }
    
                 };
                 
                """ % {
                b'basic_name': self._element.name.encode('utf-8'),
            })
            self._stream.write(b"""
                /**
                 * For children to register initializers to be called before this namespace PyObject is inited
                 **/
                int %(pyllars_scope)s::%(name)s::%(name)s_register( pyllars::Initializer* const init ){
                    static Initializer_%(name)s _initializer = Initializer_%(name)s();
                    static int status = pyllars%(parent_name)s::%(parent)s_register(&_initializer);
                    return status==0?_initializer.register_init(init):status;
                 }
                 
                """ % {
                b'name': self._element.name.encode('utf-8'),
                b'pyllars_scope': self._element.pyllars_scope.encode('utf-8'),
                b'parent_name': (self._element.scope if self._element.scope != '::' else "").encode('utf-8'),
                b'parent': qualified_name(self._element.parent.name).encode('utf-8'),

            })
            self._stream.write(b"""
                 PyObject *%(pyllars_scope)s::%(name)s::%(name)s_mod = nullptr;
                 
                 """ % {
                b'pyllars_scope': self._element.pyllars_scope.encode('utf-8'),
                b'name': self._element.name.encode('utf-8')
            })
