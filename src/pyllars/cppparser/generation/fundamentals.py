from io import TextIOBase
import logging
import os

from .base import Generator, qualified_name
from .. import parser


class FunctionDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return True

    def _func_declaration(self, element: parser.FunctionElement):
        has_varargs = element.has_varargs
        altenative_code = element.full_name
        argslambda = ", ".join([arg.as_function_argument(index, typed=True) for
                                index, arg in enumerate(element.params)])
        if element.is_template:
            template_args = "< %s >" % ",".join([arg.type_and_var_name(index) for index, arg in
                                                 enumerate(element.template_args)])
        else:
            template_args = ""
        lambda_code = "[](%(argslambda)s)->%(return_type)s{%(return_cast)s %(func_name)s%(template_args)s(%(params)s);}" % {
            'return_type': element.return_type.full_name if element.return_type else "void",
            'func_name': element.full_name,
            'return_cast': "" if element.return_type is None or element.return_type.name == "void" else
            "return (%s)" % element.return_type.name,
            'argslambda': argslambda,
            'template_args': template_args,
            'params': ", ".join(["%s" % arg.name if arg.name else
                                 "p%s" % index for index, arg in enumerate(element.params)]),
        } if not has_varargs else altenative_code

        return """
            __pyllars_internal::FuncContainer<%(has_varargs)s,
                                             %(return_type)s %(arguments)s>::Type<0,
                                             %(throws)s> func_container;
            func_container._cfunc = %(lambdacode)s;
            return  PyModule_AddObject(
                        %(module_name)s, "%(func_name)s",
                        (PyObject*)__pyllars_internal::PythonFunctionWrapper<__pyllars_internal::
                        is_complete< %(return_type)s >::value, %(has_varargs)s, %(return_type)s %(arguments)s>::
                        template Wrapper<%(throws)s>::create("%(func_name)s", func_container, argumentNames));
""" % {
            'indent': self._indent,
            'module_name': element.parent.pyllars_module_name,
            'return_type': element.return_type.full_name if element.return_type else "void",
            'arguments': (',' if len(element.params) > 0 else "") + ', '.join([t.type_.full_name for
                                                                               t in element.params]),
            'lambdacode': lambda_code,
            'has_varargs': str(element.has_varargs).lower(),
            'throws': "" if element.throws is None else "void" if len(element.throws) == 0
            else ",".join(element.throws),
            'func_name': element.name,
        }

    def generate_body_proper(self, element: parser.FunctionElement, stream: TextIOBase, src_path, as_top: bool = False) -> None:
        with self.scoped(element, stream) as scoped:
            imports = set([])
            for elem in element.params:
                if elem and elem.type_.namespace_name != element.namespace_name and elem.type_.namespace_name != "::":
                    imports.add(elem.namespace_name)
            if element.return_type and \
                            element.return_type.namespace_name != element.namespace_name and element.return_type.namespace_name != "::":
                imports.add(element.return_type.namespace_name)

            if element.is_template:
                template_init_code = """
                    template< %(template_args)s >
                    class Template_Instance: public pyllars::Initializer{
                    public:
                        virtual status_t init(){
                            %(func_decl)s
                        }

                        class Init{
                        public:
                            Init(){ %(qname)s_register( new Template_Instance();}
                        }

                        static Init _init;
                    };

                    template< %(template_args)s >
                    Template_Instance: public pyllars::Initializer::Init _init;
""" % {
                    'qname': qualified_name(element.name),
                    'template_args': ",".join([arg.type_and_var_name(index) for index, arg in
                                               enumerate(element.template_args)]),
                    'template_arg_names': ",".join([arg.var_name(index) for index, arg in
                                                    enumerate(element.template_args)]),
                    'func_decl': self._func_declaration(element),
                }
            else:
                from .structs import CXXMethodDecl
                template_init_code = ""
            scoped.write(("""
                constexpr cstring name = "%(pyname)s";

                //generated from %(file)s.generate_initializer_code
                // FUNCTION %(name)s THROWS %(throws)s
                %(template_decl)s
                static int init_me(){
                   static const char* const argumentNames[] = {%(argument_names)s nullptr};
                   status_t status = 0;
                   %(imports)s
                   %(func_decl)s
                   return status;
                }

                %(template_decl)s
                int %(qname)s_init(){return init_me%(template_args)s();}

                namespace{

                    %(template_init_code)s

                    class Initializer: public pyllars::Initializer{
                    public:
                        typedef pyllars::Initializer super;

                        Initializer():pyllars::Initializer(){
                            pyllars%(parent)s::%(parent_name)s_register(this);
                        }

                        virtual int init(){
                            int status = super::init();
                            return status | init_me();
                        }


                    };

                    static Initializer init = Initializer();
                }
""" % {
                'indent': self._indent,
                'file': __file__,
                'imports': "\n".join
                    (["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in imports]),
                'module_name': element.parent.pyllars_module_name,
                'name': self.sanitize(element.name),
                'qname': qualified_name(element.name),
                'pyname': CXXMethodDecl.METHOD_NAMES.get(element.name).replace('addMethod', '') if
                element.name in CXXMethodDecl.METHOD_NAMES else element.name if element.name != "operator=" else "assign_to",
                'parent_name': qualified_name
                    (element.parent.name if (element.parent.name and element.parent.name != "::")
                                              else "pyllars"),
                'parent': self.scope(element),
                'template_decl': element.template_decl(),
                'template_args': element.template_arguments(),
                'argument_names': ','.join(["\"%s\"" % (arg.name if arg.name else "_%s" % (index + 1)) for index, arg in
                                            enumerate(element.params)]) + (',' if element.params else ''),
                'has_varargs': str(element.has_varargs).lower(),
                'throws': "" if element.throws is None else "void" if len(element.throws) == 0
                else ",".join(element.throws),
                'func_decl': self._func_declaration(element) if not element.is_template else "",
                'return_type': element.return_type.full_name if element.return_type else "void",
                'arguments': (',' if len(element.params) > 0 else "") + ', '.join([t.type_.full_name for
                                                                                   t in element.params]),
                'template_init_code': template_init_code,
            }).encode('utf-8'))


class VarDecl(Generator):
    @classmethod
    def is_generatable(cls):
        return True

    def generate_body_proper(self, element: parser.VarDecl, stream: TextIOBase, src_path, as_top: bool = False) -> None:
        if element.parent and isinstance(element.parent.parent, parser.ClassTemplateDecl):
            raise Exception("NOT IMPL")
        if element.name == 'cinit':
            return
        with self.scoped(element, stream) as scoped:
            scoped.write(("\n                    //generated rom: %(file)s:VarDecl.generate_body_proper\n" % {
                'file': os.path.basename(__file__),
            }).encode('utf-8'))
            imports = set([])
            if element.type_ and element.type_.namespace_name != element.parent.namespace_name:
                imports.add(element.namespace_name)
            if isinstance(element.parent, parser.CXXRecordDecl):
                # static class member var:
                scoped.write(("""
                    constexpr cstring name = "%(name)s";
                    static status_t init_me(){
                        status_t status = 0;
                        %(imports)s
                        __pyllars_internal::PythonClassWrapper<%(parent_full_name)s>::addClassAttribute%(qual)s<name, %(full_type_name)s>
                          ( &%(parent_full_name)s::%(name)s);
                        return status;
                    }

                    int %(qname)s_init(){return init_me();}

                    namespace{
                        class Initializer: public pyllars::Initializer{
                        public:
                            Initializer():pyllars::Initializer(){
                               pyllars%(parent)s::%(parent_name)s_register(this);
                            }
                            virtual int init(){
                               int status = pyllars::Initializer::init();
                               return status | init_me();
                            }
                        };

                       static Initializer init = Initializer();
                    }

    """ % {
                    'qname': qualified_name(element.name or "anonymous_%s" % element.tag),
                    'name': element.name or "anonymous_%s" % element.tag,
                    'indent': self._indent,
                    'parent': element.parent.full_name,
                    'parent_name': qualified_name(element.parent.name if element.parent.name != '::' else ''),
                    'parent_full_name': element.parent.full_name,
                    'full_type_name': element.type_.full_name,
                    'qual': 'Const' if element.type_.is_const else '',
                    'imports': "\n".join(
                        ["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in
                         imports]),
                }).encode('utf-8'))
            elif isinstance(element.parent, parser.NamespaceDecl):
                # global or namespace var:
                scoped.write(("""
                    constexpr cstring name = "%(name)s";
                    static status_t init_me(){
                        status_t status = 0;
                        PyObject* mod = %(mod_name)s;
                        %(imports)s
                        if( !__pyllars_internal::GlobalVariable::createGlobalVariable<%(full_type_name)s>("%(name)s", "%(tp_name)s",
                            &%(parent)s::%(name)s, mod, %(array_size)s)){
                           status = -1;
                         }
                        return status;
                    }
                    int %(qname)s_init(){return init_me();}

                    namespace{
                        class Initializer: public pyllars::Initializer{
                        public:
                           Initializer():pyllars::Initializer(){
                               pyllars%(parent)s::%(parent_name)s_register(this);
                           }
                           virtual int init(){
                               int status = pyllars::Initializer::init();
                               return status | init_me();
                           }
                        };
                        static Initializer init = Initializer();
                    }
    """ % {
                    'qname': qualified_name(element.name or "anonymous_%s" % element.tag),
                    'name': element.name or "anonymous_%s" % element.tag,
                    'tp_name': element.type_.name,
                    'indent': self._indent,
                    'parent': element.parent.full_name if element.parent.full_name != '::' else "",
                    'parent_name': qualified_name(element.parent.name if element.parent.name else "pyllars"),
                    'mod_name': element.parent.pyllars_module_name,
                    'parent_full_name': element.parent.full_name,
                    'full_type_name': element.type_.full_name,
                    'array_size': element.type_.array_size or 0,
                    'qual': 'Const' if element.type_.is_const else 'cont',
                    'imports': "\n".join(
                        ["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in
                         imports]),
                }).encode('utf-8'))
            else:
                logging.error("Unknown parent type for global var")