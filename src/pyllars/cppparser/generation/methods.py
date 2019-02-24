import os

from pyllars.cppparser.generation.base2 import GeneratorBody, GeneratorHeader
from pyllars.cppparser.parser import code_structure
from .base import qualified_name


class GeneratorBodyCXXConstructorDecl(GeneratorBody):

    def generate(self):
        if not self._element.parent.is_definition:
            return
        imports = set([])
        for elem in self._element.params:
            if elem and elem.target_type.namespace_name != self._element.namespace_name and elem.target_type.namespace_name != "::":
                imports.add(elem.namespace_name)

        with self._scoped(self._stream) as stream:
            arguments = (',' if len(self._element.params) > 0 else "") + ', '.join([t.target_type.full_name for
                                                                                         t in self._element.params])
            argument_names = ','.join(
                    ["\"%s\"" % (arg.name if arg.name else "_%s" % (index + 1)) for index, arg in
                     enumerate(self._element.params)]) + (',' if self._element.params else '')
            stream.write(b"""
            namespace %s_constructor{
            """ % self._element_name.encode('utf-8'))
            stream.write(self.decorate(self.INITIALIZER_CODE % {
                'name': self._element.name,
                'parent_name': "pyllars" + self._element.parent.full_name + "::" + self._element.parent.name  or "pyllars"
            }).encode('utf-8'))
            stream.write(self.decorate(self.INITIALIZER_INSTANTIATION_CODE % {
                'name': self._element.name,
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

        return """
                            __pyllars_internal::PythonClassWrapper< %(typename)s ::%(scope)s>::template addConstructor<%(args)s>
                            ( argumentNames);

        """ % {
            'args': ", ".join([p.target_type.full_param_name for p in self._element.params]),
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


class GeneratorHeaderCXXMethodDecl(GeneratorHeader):

    METHOD_NAMES = {'operator-': ['__inv__', '__sub__'],
                    'operator+': ['__pos__', '__add__'],
                    'operator*': ['__deref__', '__mul__'],
                    'operator/': [None, '__div__'],
                    'operator&': ['_addr__', '__and__'],
                    'operator|': [None, '__or__'],
                    'operator^': [None, '__xor__'],
                    'operator<<': [None, '__lshift__'],
                    'operator>>': [None, '__rshift__'],
                    'operator%' : [None, '__mod__'],
                    'operator+=': [None, '__iadd__'],
                    'operator-=': [None, '__isub__'],
                    'operator*=': [None, '__imul__'],
                    'operator%=': [None, '__imod__'],
                    'operator&=': [None, '__iand__'],
                    'operator|=': [None, '__ior__'],
                    'operator^=': [None, '__ixor__'],
                    'operator<<=': [None, '__ilshift__'],
                    'operator>>=': [None, '__irshift__'],
                    'operator=': [None, 'assign_to']
                    }

    def generate_spec(self):
        if "operator delete" in self._element.name:
            return
        elif self._element.name in self.METHOD_NAMES:
            arg_count = len(self._element.params)
            if arg_count not in (0, 1):
                raise Exception("Unnexpected argument count for operator '%s'" % self._element.name)
            self._output_function_spec(
                comment="static initializer method to register initialization routine for initialization "
                        "on dynamic load of library",
                spec="status_t %s_register( pyllars::Initializer* const);" % self.sanitize(self._element_name, arg_count),
                indent=b"                ")
            self._output_function_spec(
                comment="called back on initialization to initialize Python wrapper for this C construct "
                        "@param global_mod:  mod to which the wrapper Python object should belong",
                spec="status_t %s_init(PyObject * const global_mod);" % self.sanitize(self._element_name, arg_count),
                indent=b"                ")
        else:
            super().generate_spec()


class GeneratorBodyCXXMethodDecl(GeneratorBody):

    METHOD_NAMES = GeneratorHeaderCXXMethodDecl.METHOD_NAMES

    def _func_declaration(self):
        base = "addClass" if self._element.is_static else "add"
        method_name = ('addMethod' + self.METHOD_NAMES.get(self._element.name)) if self._element.name in self.METHOD_NAMES \
                                                                                   else ("%sMethod" % base)
        if self._element.has_varargs:
            method_name += "Varargs"

        return """
                __pyllars_internal::PythonClassWrapper< %(typename)s ::%(scope)s>::template %(py_method_name)s<%(is_const)s name, %(return_type)s %(args)s>
                   ( &::%(scope)s::%(method_name)s, argumentNames);
    
        """ % {
                    'args': ("," if self._element.params else "") + ", ".join([p.target_type.full_param_name for p in self._element.params]),
                    'is_const': str(self._element.is_const).lower() + "," if not self._element.is_static else "",
                    'method_name': self._element.name or "anonymous_%s" % self._element.tag,
                    'names': ",".join(['"%s"' % (e.name or "param_%d" % index) for index, e in enumerate(self._element.params)]),
                    'py_method_name': method_name,
                    'return_type': self._element.return_type.full_name if self._element.return_type else "void",
                    'scope': self._element.scope,
                    'template_prefix': 'template ' if self._element.parent.parent.is_template_macro else "",
                    'typename': 'typename' if not self._element.is_union else ""
                }

    def sanitize(self, name: str, arg_count):
        n = self.METHOD_NAMES.get(name)
        return  n[arg_count] if n else name

    def generate(self) -> None:
        if "operator delete" in self._element.name:
            return

        with self._scoped(self._stream) as stream:
            arg_count = len(self._element.params)
            if arg_count not in (0,1):
                raise Exception("Unexpected argument count for operator '%s'" % self._element.name)
            arguments = (',' if len(self._element.params) > 0 else "") + ', '.join([t.target_type.full_name for
                                                                                   t in self._element.params])
            argument_names = ','.join(
                    ["\"%s\"" % (arg.name if arg.name else "_%s" % (index + 1)) for index, arg in
                     enumerate(self._element.params)]) + (',' if self._element.params else '')
            stream.write(self.decorate("""
                  class Initializer_%(name)s: public pyllars::Initializer{
                  public:
                      Initializer_%(name)s():pyllars::Initializer(){
                          %(parent_name)s::%(parent_name)s_register(this);                          
                      }

                      virtual int init(PyObject * const global_mod){
                         int status = %(name)s_init(global_mod);
                         return status == 0? pyllars::Initializer::init(global_mod) : status;
                      }
                      static Initializer_%(name)s *initializer;
                   };

                                                  """ % {
                'name': self._element.name,
                'parent_name': self._element.parent.name or "pyllars"
            }).encode('utf-8'))
            stream.write(("""
                constexpr cstring name = "%(sanitized_name)s";
    
                //generated from %(file)s.generate_body_proper
                // FUNCTION %(name)s THROWS %(throws)s
                """ %{
                'file': __file__,
                'name': self._element.name,
                'sanitized_name': self.sanitize(self._element.name, arg_count),
                'throws': "" if self._element.throws is None else "void" if len(self._element.throws) == 0
                else ",".join(self._element.throws),
            }).encode('utf-8'))
            stream.write(self.decorate("""
                status_t %(sanitized_name)s_init(PyObject * const global_mod){
                   static const char* const argumentNames[] = {%(argument_names)s nullptr};
                   status_t status = 0;
                   %(func_decl)s
                   return status;
                }""" % {
                    'argument_names': argument_names,
                    'sanitized_name': self.sanitize(self._element.name, arg_count),
                    'func_decl': self._func_declaration(),
                }).encode('utf-8'))
            stream.write(("""
                status_t %(sanitized_name)s_register(pyllars::Initializer* const init){
                    return 0;
                }
            """ % {
                'sanitized_name': self.sanitize(self._element.name, arg_count),
            }).encode('utf-8'))
            stream.write(self.decorate("""
                Initializer_%(sanitized_name)s *Initializer_%(sanitized_name)s::initializer =
                    new Initializer_%(sanitized_name)s();
    """ % {
                'arguments': arguments,
                'argument_names': argument_names,
                'sanitized_name': self.sanitize(self._element.name, arg_count),
                'func_decl': self._func_declaration(),
                'name': self._element.name,

            }).encode('utf-8'))


class GeneratorBodyFunctionDecl(GeneratorBody):
    METHOD_NAMES = GeneratorHeaderCXXMethodDecl.METHOD_NAMES

    def _func_declaration(self):
        has_varargs = self._element.has_varargs
        altenative_code = self._element.full_name
        argslambda = ", ".join([arg.target_type.full_name + " " + arg.as_function_argument(index) for
                                index, arg in enumerate(self._element.params)])
        if self._element.is_template:
            template_args = "< %s >" % ",".join([arg.type_and_var_name(index) for index, arg in
                                                 enumerate(self._element.template_args)])
        else:
            template_args = ""

        arguments = ((',' if len(self._element.params) > 0 else "") +
                         ', '.join([p.target_type.full_name for p in self._element.params]))
        lambda_code = "[](%(argslambda)s)->%(return_type)s{%(return_cast)s %(func_name)s%(template_args)s(%(params)s);}" % {
            'return_type': self._element.return_type.full_name if self._element.return_type else "void",
            'func_name': self._element.full_name,
            'return_cast': "" if self._element.return_type is None or self._element.return_type.full_name == "void" else
                "return (%s)" % self._element.return_type.full_name,
            'argslambda': argslambda,
            'template_args': template_args,
            'params': ", ".join(["%s" % arg.name if arg.name else
                                 "p%s" % index for index, arg in enumerate(self._element.params)]),
        } if not has_varargs else altenative_code
        return """
            __pyllars_internal::FuncContainer<%(has_varargs)s, %(return_type)s %(arguments)s>::Type<0, %(throws)s> func_container;
            func_container._cfunc = %(lambdacode)s;
            return  PyModule_AddObject(
                        %(module_name)s, "%(func_name)s",
                        (PyObject*)__pyllars_internal::PythonFunctionWrapper<__pyllars_internal::
                        is_complete< %(return_type)s >::value, %(has_varargs)s, %(return_type)s %(arguments)s>::
                        template Wrapper<%(throws)s>::create("%(func_name)s", func_container, argumentNames));
""" % {
            'module_name': self._element.parent.python_cpp_module_name if not isinstance(self._element.parent, code_structure.TranslationUnitDecl) else "global_mod",
            'return_type': self._element.return_type.full_name if self._element.return_type else "void",
            'arguments': arguments,
            'lambdacode': lambda_code,
            'has_varargs': str(self._element.has_varargs).lower(),
            'throws': ",".join(self._element.throws) if self._element.throws else "void",
            'func_name': self._element.name,
        }

    def write_include_directives(self):
        if 'operator delete' in self._element.name or 'operator new' in self._element.name:
            return
        super().write_include_directives()

    def generate(self) -> None:
        if 'operator delete' in self._element.name or 'operator new' in self._element.name:
            return
        with self._scoped(self._stream) as stream:
            arg_count = len(self._element.params)
            stream.write(self.decorate("""
                  class Initializer_%(name)s: public pyllars::Initializer{
                  public:
                      Initializer_%(name)s():pyllars::Initializer(){
                          %(parent_name)s::%(parent_name)s_register(this);                          
                      }

                      virtual int init(PyObject * const global_mod){
                         int status = %(name)s_init(global_mod);
                         return status == 0? pyllars::Initializer::init(global_mod) : status;
                      }
                      static Initializer_%(name)s *initializer;
                   };

                                                  """ % {
                'name': self._element.name,
                'parent_name': self._element.parent.name if not isinstance(self._element.parent, code_structure.TranslationUnitDecl) else "pyllars"
            }).encode('utf-8'))

            stream.write(("""
                constexpr cstring name = "%(pyname)s";
    
                //generated from %(file)s.generate_body_proper
                // FUNCTION %(name)s THROWS %(throws)s
                """ % {
                'name': self._element.name,
                'pyname': self.METHOD_NAMES.get(self._element.name) if
                   self._element.name in self.METHOD_NAMES else self._element.name if self._element.name != "operator=" else "assign_to",
                'throws': "" if self._element.throws is None else "void" if len(self._element.throws) == 0
                          else ",".join(self._element.throws),
                         'file': __file__,
            }).encode('utf-8'))
            argument_names = ','.join(["\"%s\"" % (arg.name if arg.name else "_%s" % (index + 1)) for index, arg in
                                            enumerate(self._element.params)]) + (',' if self._element.params else '')
            stream.write(self.decorate("""
                status_t %(name)s_init(PyObject * const global_mod){
                   static const char* const argumentNames[] = {%(argument_names)s nullptr};
                   status_t status = 0;
                   %(func_decl)s
                   return status;
                }
                
                """ % {
                'argument_names': argument_names,
                'func_decl': self._func_declaration(),
                'name': self._element.name,
            }).encode('utf-8'))
            stream.write(self.decorate("""
                status_t %(name)s_register(pyllars::Initializer*){
                    //do nothing, functions have no children
                    return 0;
                }
            """ % {
                'name': self._element.name,
            }).encode('utf-8'))
            stream.write(("""
               Initializer_%(name)s *Initializer_%(name)s::initializer = new Initializer_%(name)s();
    
    """ % {
                'name': self._element.name,
            }).encode('utf-8'))

