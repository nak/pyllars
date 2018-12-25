from pyllars.cppparser.generation.base2 import GeneratorBody, GeneratorHeader
from pyllars.cppparser.parser import code_structure
from .base import qualified_name


class GeneratorBodyTypedefDecl(GeneratorBody):

    def generate(self):
        with self._scoped(self._stream, indent=b"            "):
            self._stream.write(b"""

                """ % {b'name': self._element.name.encode('utf-8'),
                       b'pyllars_scope': self._element.pyllars_scope.encode('utf-8')})
            if isinstance(self._element.target_type, code_structure.BuiltinType):
                suffix = + b'_mod' if isinstance(self._element, code_structure.NamespaceDecl) else b''
                self._stream.write(b"""
                     status_t %(name)s_init(PyObject* global_mod){
                         static bool %(name)s_inited = false;
                         if (%(name)s_inited) return 0;// if already initialized
                         %(name)s_inited = true;

                         int status = 0;
                         if(! %(parent_mod)s ){
                             status = -1;
                         } else {
                             PyModule_AddObject(%(parent_mod)s, "%(name)s", (PyObject*)&%(target_name)s);
                         }
                         return status;
                     }
                     """  % {
                    b'parent_mod': self._element.parent.full_name.encode('utf-8') + suffix if
                    self._element.parent and self._element.parent.name else b"global_mod",
                    b'name': self._element.name.encode('utf-8'),
                    b'target_name': self._element.target_type.python_type_name.encode('utf-8')
                })
            else:
                target_parent = self._element.target_type.parent
                classes = []
                if not target_parent or isinstance(target_parent, code_structure.TranslationUnitDecl):
                    target_parent_mod = "PyImport_ImportModule(\"pyllars_mod\")"
                else:
                    target_parent_mod = ""
                    while target_parent and target_parent.name:
                        if not isinstance(target_parent, code_structure.NamespaceDecl):
                            classes.append(target_parent)
                            target_parent = target_parent.parent
                            continue
                        target_parent_mod = target_parent.name + '.' + target_parent_mod
                        target_parent = target_parent.parent
                    if not target_parent_mod:
                        target_parent_mod = "PyImport_ImportModule(\"pyllars_mod\")"
                    else:
                        target_parent_mod = "PyImport_ImportModule(\"%s\")" % target_parent_mod
                suffix = + b'_mod' if isinstance(self._element, code_structure.NamespaceDecl) else b''
                self._stream.write(b"""
                    
                    status_t %(name)s_init(PyObject* global_mod){
                         static bool %(name)s_inited = false;
                         if (%(name)s_inited) return 0;// if already initialized
                         %(name)s_inited = true;
                        
                         int status = 0;
                        
                         if (!%(parent_mod)s){
                             status = -2;
                         } else {
                             PyObject *o = %(target_mod)s;
                             if(!o){
                                 PyErr_Clear();
                                 return -3;
                             }
                    """  % {
                    b'name': self._element.name.encode('utf-8'),
                    b'basic_name': self._element.name.encode('utf-8'),
                    b'fullname': self._element.full_name.encode('utf-8'),
                    b'parent_mod': self._element.parent.full_name.encode('utf-8') + suffix if
                        self._element.parent and self._element.parent.name else b"global_mod",
                    b'target_mod': target_parent_mod.encode('utf-8')
                })
                for o in classes + [self._element.target_type.name]:
                    self._stream.write(b"""
                            o = PyObject_GetAttrString(o, "%s");
                            if (!o) {
                                PyErr_Clear();
                                return -1;
                            }
                    """ % o.encode('utf-8'))
                self._stream.write(b"""
                            PyObject_SetAttrString(%(parent_mod)s, "%(name)s", o);
                        } 
                        return status;
                    } // end init
                    
                    """ % {
                    b'parent_mod': self._element.parent.full_name.encode('utf-8') + suffix if
                        self._element.parent and self._element.parent.name else b"global_mod",
                    b'name': self._element.name.encode('utf-8'),
                })
                self._stream.write(b"""
                    class Initializer_%(basic_name)s: public pyllars::Initializer{
                    public:
                        Initializer_%(basic_name)s():pyllars::Initializer(){
                            %(parent_name)s_register(this);                          
                        }
        
                        virtual int init(PyObject * const global_mod){
                           int status = %(basic_name)s_init(global_mod);
                           return status == 0? pyllars::Initializer::init(global_mod) : status;
                        }
                    private:
                        static Initializer_%(basic_name)s *initializer;
                     };
                     
                    Initializer_%(basic_name)s *Initializer_%(basic_name)s::initializer = new Initializer_%(basic_name)s();
                    """ % {
                    b'basic_name': self._element.name.encode('utf-8'),
                    b'parent_name': (self._element.parent.name or "pyllars").encode('utf-8'),
                })
                self._stream.write(b"""
                    /**
                     * For children to register initializers to be called before this namespace PyObject is inited
                     **/
                    int %(name)s_register( pyllars::Initializer* const init ){
                        static Initializer_%(name)s _initializer = Initializer_%(name)s();
                        static int status = pyllars%(parent_name)s::%(parent)s_register(&_initializer);
                        return status==0?_initializer.register_init_last(init):status;
                     }
                     
                    """ % {
                    b'name': self._element.name.encode('utf-8'),
                    b'pyllars_scope': self._element.pyllars_scope.encode('utf-8'),
                    b'parent_name': (self._element.scope if self._element.scope != '::' else "").encode('utf-8'),
                    b'parent': qualified_name(self._element.parent.name).encode('utf-8'),

                })




class GeneratorBodyNamespaceDecl(GeneratorBody):

    def generate(self):
        with self._scoped(self._stream, indent=b"            "):
            self._stream.write(b"""

                """ % {b'name': self._element.name.encode('utf-8'),
                       b'pyllars_scope': self._element.pyllars_scope.encode('utf-8')})
            self._stream.write(b"""
                PyObject * %(name)s_mod = nullptr;

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
                    self._element.parent and self._element.parent.name else b"global_mod"
            })
            self._stream.write(b"""
                class Initializer_%(basic_name)s: public pyllars::Initializer{
                public:
                    Initializer_%(basic_name)s():pyllars::Initializer(){
                        %(parent_name)s_register(this);                          
                    }

                    virtual int init(PyObject * const global_mod){
                       int status = %(basic_name)s_init(global_mod);
                       return status == 0? pyllars::Initializer::init(global_mod) : status;
                    }
                private:
                    static Initializer_%(basic_name)s *initializer;

                 };

                Initializer_%(basic_name)s *Initializer_%(basic_name)s::initializer = new Initializer_%(basic_name)s();

                """ % {
                b'basic_name': self._element.name.encode('utf-8'),
                b'parent_name': (self._element.parent.name or "pyllars").encode('utf-8'),
            })
            self._stream.write(b"""
                /**
                 * For children to register initializers to be called before this namespace PyObject is inited
                 **/
                int %(name)s_register( pyllars::Initializer* const init ){
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

