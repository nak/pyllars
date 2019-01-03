import os

from pyllars.cppparser.generation.base2 import GeneratorBody, GeneratorHeader
from .base import qualified_name


class GeneratorBodyCXXConstructorDecl(GeneratorBody):

    def generate(self):
        imports = set([])
        for elem in self._element.params:
            if elem and elem.type_.namespace_name != self._element.namespace_name and elem.type_.namespace_name != "::":
                imports.add(elem.namespace_name)
        if self._element.return_type and \
                self._element.return_type.namespace_name != self._element.namespace_name and self._element.return_type.namespace_name != "::":
            imports.add(self._element.return_type.namespace_name)

        with self._scoped(self._stream) as stream:
            arguments = (',' if len(self._element.params) > 0 else "") + ', '.join([t.type_.full_name for
                                                                                         t in self._element.params])
            argument_names = ','.join(
                    ["\"%s\"" % (arg.name if arg.name else "_%s" % (index + 1)) for index, arg in
                     enumerate(self._element.params)]) + (',' if self._element.params else '')
            stream.write(b"""
            namespace %s_constructor{
            """ % self._element_name.encode('utf-8'))
            stream.write(self.decorate("""
                        class Initializer_%(basic_name)s: public pyllars::Initializer{
                        public:
                            Initializer_%(basic_name)s():pyllars::Initializer(){
                                %(parent_name)s::%(parent_name)s_register(this);                          
                            }

                            virtual int init(PyObject * const global_mod){
                               int status = %(basic_name)s_init(global_mod);
                               return status == 0? pyllars::Initializer::init(global_mod) : status;
                            }
                            static Initializer_%(basic_name)s *initializer;
                         };

                                        """ % {
                'basic_name': self._element.name,
                'parent_name': self._element.parent.name or "pyllars"
            }).encode('utf-8'))
            stream.write(("""
    
                        //generated from %(file)s GeneratorBodyCXXConstructorDecl.generate
                        // CONSTRUCTOR %(name)s THROWS %(throws)s
                        status_t %(name)s_init(PyObject * const global_mod){
                           static const char* const argumentNames[] = {%(argument_names)s nullptr};
                           status_t status = 0;
                           %(imports)s
                           %(func_decl)s
                           return status;
                        }
    
                        status_t %(name)s_register(pyllars::Initializer* const init){
                            return 0;
                        }
    
                        Initializer_%(name)s *Initializer_%(name)s::initializer = new Initializer_%(name)s();
                    """ % {
                'arguments': arguments,
                'argument_names': argument_names,
                'file': __file__,
                'func_decl': self._func_declaration(),
                'imports': "\n".join(
                    ["if(!PyImport_ImportModule(\"pyllars::%s\")){return -1;} " % n.replace("::", ".") for n in
                     imports]),
                'name': self._element.name,
                'pyllars_scope': self._element.pyllars_scope,
                'throws': "" if self._element.throws is None else "void" if len(self._element.throws) == 0
                else ",".join(self._element.throws),
            }).encode('utf-8'))
            stream.write(b"""
            }""")

    def _func_declaration(self):
        # if self._element.has_varargs:
        #    method_name += "Varargs"

        return """
                            __pyllars_internal::PythonClassWrapper< %(typename)s ::%(scope)s>::template addConstructor<%(args)s>
                            ( argumentNames);

        """ % {
            'args': ", ".join([p.type_.full_param_name for p in self._element.params]),
            'scope': self._element.scope,  # block_scope
            'typename': 'typename' if not (self._element.parent and self._element.parent.is_union) else ""
        }


class GeneratorHeaderCXXConstructorDecl(GeneratorHeader):
    def generate_spec(self):
        self._stream.write(b"""
        
        namespace %s_constructor{
        """ % self._element_name.encode('utf-8'))
        super().generate_spec()
        self._stream.write(b"""
        }
        
        """)
