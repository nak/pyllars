import os

from pyllars.cppparser.generation import GeneratorBody, GeneratorHeader, qualified_name
from pyllars.cppparser.parser import code_structure
import logging
log = logging.getLogger(__name__)


class GeneratorHeaderFieldDecl(GeneratorHeader):
    pass


class GeneratorBodyFieldDecl(GeneratorBody):

    def write_include_directives(self):
        super().write_include_directives()
        self._stream.write(b"\n#include <pyllars/pyllars_classmembersemantics.impl>\n")
        self._stream.write(b"#include <pyllars/pyllars_membersemantics.impl>\n")
        self._stream.write(b"#include <pyllars/pyllars_pointer.impl>\n")

    def generate(self):
        if not self._element.name and not self._element.target_type.name:
            raise Exception("Anonymously typed anonymously name field encountered")

        imports = set([])

        ##if self._element.parent and self._element.target_type and self._element.target_type.scope != self._element.parent.scope:
        ##    imports.add(self._element.scope)
        with self._scoped(self._stream) as stream:

            self._stream.write(self.decorate(self.INITIALIZER_CODE % {
                'name': self._element.name,
                'parent_name': self._element.parent.name or "pyllars"
            }).encode('utf-8'))

            self._stream.write(self.decorate(self.INITIALIZER_INSTANTIATION_CODE % {
                'name': self._element.name
            }).encode('utf-8'))
            if self._element.bit_size is None:
                if self._element.target_type.array_size is not None:
                    stream.write(b"""
                                constexpr cstring name = "%(name)s";
                                //generated from %(file)s GeneraorBodyFieldDecl.generate
                                // FUNCTION %(name)s THROWS
                                """ % {
                        'file': __file__.encode('utf-8'),
                        'name': self._element_name.encode('utf-8')
                    });
                    stream.write(self.decorate("""
                        
                        status_t %(name)s_init(PyObject * const global_mod){
                        
                           status_t status = 0;
                           %(imports)s
                            __pyllars_internal::PythonClassWrapper< %(typename)s ::%(scope)s >::template addArrayAttribute<name, %(array_size)s, ::%(full_type_name)s>
                           return status;
                        }
                                """ %{
                        'name': self._element.name,
                        'imports': "\n".join(
                           ["if(!PyImport_ImportModule(\"pyllars::%s\")){return -1;} " % n.replace("::", ".") for n in
                            imports]),
                        'array_size': self._element.target_type.array_size,
                        'full_type_name': self._element.target_type.full_name,
                        'scope': self._element.scope,
                        'typename': 'typename' if not self._element.parent.is_union else "" ,
                    }).encode('utf-8'))

                else:
                    stream.write(("""
                        constexpr cstring name = "%(name)s";
                        //generated from %(file)s.generate_body_proper #2
                        // FUNCTION %(name)s THROWS

                        status_t %(name)s_init(PyObject* const global_mod){
                           status_t status = 0;
                           %(imports)s
                            __pyllars_internal::PythonClassWrapper< %(typename)s ::%(scope)s >::template addAttribute%(qual)s<name, decltype(::%(scope)s::%(name)s)>
                               ( &::%(scope)s::%(name)s);
                           return status;
                        }


            """ % {
                        'file': __file__,
                        'name': self._element.name,
                        'imports': "\n".join(
                            ["if(!PyImport_ImportModule(\"pyllars::%s\")){return -1;} " % n.replace("::", ".") for n in
                             imports]),
                        'qual': 'Const' if self._element.target_type.is_const else "",
                        'scope': self._element.scope,
                        'typename': 'typename' if not (self._element.parent and self._element.parent.is_union) else ""
                    }).encode('utf-8'))
            else:
                setter_code = """
                           static std::function< %(full_type_name)s(%(parent_full_name)s&, %(qual2)s %(full_type_name)s &)> setter =
                               [](%(parent_full_name)s &self, %(qual2)s %(full_type_name)s &v)->%(full_type_name)s{return self.%(name)s = v;};
            """ % {
                    'parent_full_name': self._element.parent.full_name,
                    'qual2': 'const' if not self._element.target_type.is_const else "",
                    'name': self._element.name,
                    'full_type_name': self._element.target_type.full_name
                }
                stream.write(("""
                            constexpr cstring name = "%(name)s";
                            //generated from %(file)s.generate_body_proper #3
                            // FUNCTION %(name)s THROWS
                            %(template_decl)s
                            status_t %(pyllars_scope)s::%(name)s_init(PyObject * const global_mod){
                               status_t status = 0;
                               %(imports)s
                               static std::function< %(full_type_name)s(const %(scope)s&)> getter =
                                   [](const %(scope)s &self)->%(full_type_name)s{return self.%(name)s;};
                               %(setter_code)s
                                __pyllars_internal::PythonClassWrapper< %(typename)s %(scope)s >::template addBitField%(qual)s<name, %(full_type_name)s, %(bit_size)s>
                                   ( getter %(setter)s);
                               return status;
                            }
    
                          
    
                          
            """ % {
                    'file': __file__,
                    'bit_size': self._element.bit_size,
                    'imports': "\n".join(
                        ["if(!PyImport_ImportModule(\"pyllars::%s\")){return -1;} " % n.replace("::", ".") for n in
                         imports]),
                    'name': self._element.name,
                    'full_name': self._element.target_type.full_name,
                    'full_type_name': self._element.target_type.full_param_name,
                    'pyllars_scope': self._element.pyllars_scope,
                    'qual': 'Const' if self._element.target_type.is_const else "",
                    'qual2': 'const' if not self._element.target_type.is_const else "",
                    'setter': ", setter" if not self._element.target_type.is_const else "",
                    'setter_code': setter_code if not self._element.target_type.is_const else "",
                    'scope': self._element.block_scope,
                    'template_args': self._element.template_arguments_string(),
                    'template_decl': template_decl(self),
                    'typename': 'typename' if not self._element.parent.is_union else ""
                }).encode('utf-8'))


class GeneratorBodyVarDecl(GeneratorBody):
    
    def generate(self):
        if not self._element.target_type.name:
            self._stream.write(("""
                              namespace __pyllars_internal{
                                    template<>
                                    const char* const _Types<decltype(%(name)s)>::type_name(){
                                      static const char* const name = "anonymous type";
                                      return name;
                                    } 
                              }
                      """ % {'name': self._element.full_name}).encode('utf-8'))
        with self._scoped(self._stream) as stream:
            stream.write(("\n                    //generated from: %(file)s GeneratorBodyVarDecl.generate\n" % {
                'file': os.path.basename(__file__),
            }).encode('utf-8'))

            self._stream.write(self.decorate(self.INITIALIZER_CODE % {
                'name': self._element.name,
                'parent_name': self._element.parent.name or "pyllars"
            }).encode('utf-8'))

            self._stream.write(self.decorate(self.INITIALIZER_INSTANTIATION_CODE % {
                'name': self._element.name
            }).encode('utf-8'))
            stream.write(("""
                        status_t %(name)s_register( pyllars::Initializer* const){
                            status_t status = 0;
                            // do nothing
                            return status;
                        }


                        """ % {
                'name': self._element.name or "anonymous_%s" % self._element.tag,
            }).encode('utf-8'))
            imports = set([])
            ##if self._element.parent and self._element.target_type and self._element.target_type.scope != self._element.parent.scope:
            ##    imports.add(self._element.scope)
            if isinstance(self._element.parent, code_structure.RecordTypeDefn):
                # static class member var:
                stream.write(("""
                        constexpr cstring name = "%(name)s";
                            """% {
                    'name': self._element.name
                }).encode('utf-8'))
                if self._element.is_constexpr:
                    stream.write(self.decorate("""
                           status_t %(name)s_init(PyObject * const global_mod){
                               static decltype(%(parent_full_name)s::%(name)s) static_var = %(parent_full_name)s::%(name)s;
                               status_t status = 0;
                               %(imports)s
                                  __pyllars_internal::PythonClassWrapper<%(parent_full_name)s>::addClassAttribute%(qual)s<name, decltype(%(parent_full_name)s::%(name)s)>
                                 ( &static_var);
                               return status;
                           }
                                       """ % {
                        'basic_type_name': self._element.target_type.name,
                        'name': self._element.name or "anonymous_%s" % self._element.tag,
                        'parent_full_name': self._element.parent.full_name,
                        'type_mod': self._element.pyllars_scope + "::" + self._element.parent.name + "_module()" if self._element.parent.name != '' else "global_mod",
                        'full_type_name': self._element.target_type.full_name,
                        'imports': "\n".join(
                            ["if(!PyImport_ImportModule(\"pylllars.%s\")){PyErr_Clear(); } " % n.replace("::", ".") for
                             n in
                             imports if n]),
                        'qual': 'Const' if self._element.target_type.is_const else '',
                    }).encode('utf-8'))
                else:
                    stream.write(self.decorate("""
                            status_t %(name)s_init(PyObject * const global_mod){
                                status_t status = 0;
                                %(imports)s
                                   __pyllars_internal::PythonClassWrapper<%(parent_full_name)s>::addClassAttribute%(qual)s<name, decltype(%(parent_full_name)s::%(name)s)>
                                  ( &%(parent_full_name)s::%(name)s);
                                return status;
                            }
                            """% {
                        'basic_type_name': self._element.target_type.name,
                        'name': self._element.name or "anonymous_%s" % self._element.tag,
                        'parent_full_name': self._element.parent.full_name,
                        'type_mod': self._element.pyllars_scope + "::" + self._element.parent.name + "_module()" if self._element.parent.name != '' else "global_mod",
                        'full_type_name': self._element.target_type.full_name,
                        'imports': "\n".join(
                            ["if(!PyImport_ImportModule(\"pylllars.%s\")){PyErr_Clear(); } " % n.replace("::", ".") for n in
                             imports if n]),
                        'qual': 'Const' if self._element.target_type.is_const else '',
                    }).encode('utf-8'))

            elif isinstance(self._element.parent, (code_structure.NamespaceDecl, code_structure.TranslationUnitDecl)):
                # global or namespace var:
                if self._element.is_constexpr:
                    stream.write(("""
                               constexpr cstring name = "%(name)s";
                               static decltype(%(parent)s::%(name)s) static_var =  %(parent)s::%(name)s;
                               status_t %(name)s_init(PyObject * const global_mod){
                                   status_t status = 0;

                                   %(imports)s
                                   if(__pyllars_internal::PythonClassWrapper<decltype(%(parent)s::%(name)s)>::initialize() != 0){
                                       status = -1;
                                   } else {
                                       PyObject *module = %(type_mod)s;
                                       if(module){
                                           PyModule_AddObject(module, "%(name)s", (PyObject*) __pyllars_internal::PythonClassWrapper<decltype(%(parent)s::%(name)s)>::getPyType());
                                       }
                                       if( !__pyllars_internal::GlobalVariable::createGlobalVariable<decltype(%(parent)s::%(name)s)>("%(name)s", 
                                          &static_var,  %(type_mod)s, %(array_size)s)){
                                          status = -1;
                                        }
                                    }
                                   return status;
                               }

                           """ % {
                        'pyllars_scope': self._element.pyllars_scope,
                        'basic_type_name': self._element.target_type.name,  # name
                        'name': self._element.name or "anonymous_%s" % self._element.tag,
                        'parent': ('::' + self._element.parent.full_name) if not isinstance(self._element.parent,
                                                                                            code_structure.TranslationUnitDecl) else "",
                        'module_name': self._element.parent.python_cpp_module_name if not isinstance(
                            self._element.parent,
                            code_structure.TranslationUnitDecl) else "PyImport_Module(\"pyllars\")",
                        'type_mod': self._element.pyllars_scope + "::" + self._element.parent.name + "_module()" if not isinstance(
                            self._element.parent, code_structure.TranslationUnitDecl) else "global_mod",
                        'array_size': self._element.target_type.array_size or 0,
                        'qual': 'Const' if self._element.target_type.is_const else 'cont',
                        'imports': "\n".join(
                            ["if(!PyImport_ImportModule(\"pylllars.%s\")){PyErr_Clear();} " % n.replace("::", ".") for n
                             in
                             imports if n]),
                    }).encode('utf-8'))
                else:
                    stream.write(("""
                                constexpr cstring name = "%(name)s";
        
                                status_t %(name)s_init(PyObject * const global_mod){
                                    status_t status = 0;
        
                                    %(imports)s
                                   status = __pyllars_internal::PythonClassWrapper<decltype(%(parent)s::%(name)s)>::initialize();
                                   if(status == 0){
                                       PyObject *module = %(type_mod)s;
                                       if(module){
                                           PyModule_AddObject(module, "%(name)s", (PyObject*)__pyllars_internal::PythonClassWrapper<decltype(%(parent)s::%(name)s)>::getPyType());
                                       }
                                        if( !__pyllars_internal::GlobalVariable::createGlobalVariable<decltype(%(parent)s::%(name)s)>("%(name)s",
                                            &%(parent)s::%(name)s, %(type_mod)s, %(array_size)s)){
                                           status = -1;
                                       }
                                    }
                                    return status;
                                }
    
                """ % {
                        'pyllars_scope': self._element.pyllars_scope,
                        'basic_type_name': self._element.target_type.name,  # name
                        'name': self._element.name or "anonymous_%s" % self._element.tag,
                        'parent': ('::' + self._element.parent.full_name) if not isinstance(self._element.parent, code_structure.TranslationUnitDecl) else "",
                        'module_name': self._element.parent.python_cpp_module_name  if not isinstance(self._element.parent, code_structure.TranslationUnitDecl)  else "PyImport_Module(\"pyllars\")",
                        'type_mod': self._element.pyllars_scope + "::" + self._element.parent.name + "_module()" if not isinstance(self._element.parent, code_structure.TranslationUnitDecl)  else "global_mod",
                        'array_size': self._element.target_type.array_size or 0,
                        'qual': 'Const' if self._element.target_type.is_const else 'cont',
                        'imports': "\n".join(
                            ["if(!PyImport_ImportModule(\"pylllars.%s\")){PyErr_Clear();} " % n.replace("::", ".") for n in
                             imports if n]),
                    }).encode('utf-8'))
            else:
                log.error("Unknown parent type for global var")