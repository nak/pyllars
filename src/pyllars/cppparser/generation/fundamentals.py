from io import TextIOBase
import logging
import os

from .base import Generator, qualified_name
from .. import parser



def template_decl(element: Generator):
    if element.parent:
        return template_decl(element.parent) + (("\n" + element.template_decl) if element.template_decl.strip() else "")
    else:
        return element.template_decl


class FunctionDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return True

    def _func_declaration(self):
        has_varargs = self.element.has_varargs
        altenative_code = self.element.full_name
        argslambda = ", ".join([arg.as_function_argument(index, typed=True) for
                                index, arg in enumerate(self.element.params)])
        if self.element.is_template:
            template_args = "< %s >" % ",".join([arg.type_and_var_name(index) for index, arg in
                                                 enumerate(self.element.template_args)])
        else:
            template_args = ""
        lambda_code = "[](%(argslambda)s)->%(return_type)s{%(return_cast)s %(func_name)s%(template_args)s(%(params)s);}" % {
            'return_type': self.element.return_type.full_name if self.element.return_type else "void",
            'func_name': self.element.full_name,
            'return_cast': "" if self.element.return_type is None or self.element.return_type.full_name == "void" else
                 "return (%s)" % self.element.return_type.full_name,
            'argslambda': argslambda,
            'template_args': template_args,
            'params': ", ".join(["%s" % arg.name if arg.name else
                                 "p%s" % index for index, arg in enumerate(self.element.params)]),
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
            'module_name': self.element.parent.pyllars_module_name,
            'return_type': self.element.return_type.full_name if self.element.return_type else "void",
            'arguments': (',' if len(self.element.params) > 0 else "") +
                          ', '.join([t.type_.full_name for t in self.element.params]),
            'lambdacode': lambda_code,
            'has_varargs': str(self.element.has_varargs).lower(),
            'throws': "" if self.element.throws is None else "void" if len(self.element.throws) == 0
            else ",".join(self.element.throws),
            'func_name': self.element.name,
        }

    def generate_body_proper(self, scoped: TextIOBase, as_top: bool = False) -> None:
        imports = set([])
        for elem in self.element.params:
            if elem and elem.type_.namespace_name != self.element.namespace_name and elem.type_.namespace_name != "::":
                imports.add(elem.namespace_name)
        if self.element.return_type and \
                        self.element.return_type.namespace_name != self.element.namespace_name and self.element.return_type.namespace_name != "::":
            imports.add(self.element.return_type.namespace_name)

        from .structs import CXXMethodDecl
        scoped.write(("""
            constexpr cstring name = "%(pyname)s";

            //generated from %(file)s.generate_initializer_code
            // FUNCTION %(name)s THROWS %(throws)s
            %(template_decl)s
            status_t %(pyllars_scope)s::%(name)s%(template_args)s::%(name)s_init(){
               static const char* const argumentNames[] = {%(argument_names)s nullptr};
               status_t status = 0;
               %(imports)s
               %(func_decl)s
               return status;
            }
            
            %(template_decl)s
            status_t %(pyllars_scope)s::%(name)s%(template_args)s::%(name)s_register(pyllars::Initializer*){
                //do nothing, functions have no children
                return 0;
            }

            %(template_decl)s
            %(pyllars_scope)s::%(basic_name)s%(template_args)s::Initializer_%(basic_name)s
            *%(pyllars_scope)s::%(basic_name)s%(template_args)s::Initializer_%(basic_name)s::initializer = 
            new %(pyllars_scope)s::%(basic_name)s%(template_args)s::Initializer_%(basic_name)s();
            
""" % {
            'file': __file__,
            'basic_name': self.element.basic_name,
            'pyllars_scope': self.element.pyllars_scope,
            'imports': "\n".join
                (["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in imports]),
            'module_name': self.element.parent.pyllars_module_name,
            'name': self.sanitize(self.element.name),
            'pyname': CXXMethodDecl.METHOD_NAMES.get(self.element.name).replace('addMethod', '') if
            self.element.name in CXXMethodDecl.METHOD_NAMES else self.element.name if self.element.name != "operator=" else "assign_to",
            'parent_name': qualified_name
                (self.element.parent.name if (self.element.parent.name and self.element.parent.name != "::")
                                          else "pyllars"),
            'parent': self.scope(),
            'template_decl': template_decl(self),
            'template_args': self.element.template_arguments_string(),
            'argument_names': ','.join(["\"%s\"" % (arg.name if arg.name else "_%s" % (index + 1)) for index, arg in
                                        enumerate(self.element.params)]) + (',' if self.element.params else ''),
            'has_varargs': str(self.element.has_varargs).lower(),
            'throws': "" if self.element.throws is None else "void" if len(self.element.throws) == 0
            else ",".join(self.element.throws),
            'func_decl': self._func_declaration() if not self.element.is_template else "",
            'return_type': self.element.return_type.full_name if self.element.return_type else "void",
            'arguments': (',' if len(self.element.params) > 0 else "") + ', '.join([t.type_.full_name for
                                                                               t in self.element.params]),
        }).encode('utf-8'))


class VarDecl(Generator):
    @classmethod
    def is_generatable(cls):
        return True

    def generate_body_proper(self, scoped: TextIOBase, as_top: bool = False) -> None:
        if self.element.parent and isinstance(self.element.parent.parent, parser.ClassTemplateDecl):
            raise Exception("NOT IMPL")
        scoped.write(("\n                    //generated rom: %(file)s:VarDecl.generate_body_proper\n" % {
            'file': os.path.basename(__file__),
        }).encode('utf-8'))
        imports = set([])
        if self.element.type_ and self.element.type_.namespace_name != self.element.parent.namespace_name:
            imports.add(self.element.namespace_name)
        if isinstance(self.element.parent, parser.CXXRecordDecl):
            # static class member var:
            scoped.write(("""
                constexpr cstring name = "%(name)s";
                status_t %(pyllars_scope)s::%(basic_name)s::%(basic_name)s_init(){
                    status_t status = 0;
                    %(imports)s
                    __pyllars_internal::PythonClassWrapper<%(parent_full_name)s>::addClassAttribute%(qual)s<name, %(full_type_name)s>
                      ( &%(parent_full_name)s::%(name)s);
                    return status;
                }

                status_t %(pyllars_scope)s::%(basic_name)s::%(basic_name)s_register( pyllars::Initializer* const){
                    status_t status = 0;
                    // do nothing
                    return status;
                }

                 %(template_decl)s
                 %(pyllars_scope)s::%(basic_name)s%(template_args)s::Initializer_%(basic_name)s
                 *%(pyllars_scope)s::%(basic_name)s%(template_args)s::Initializer_%(basic_name)s::initializer = 
                 new %(pyllars_scope)s::%(basic_name)s%(template_args)s::Initializer_%(basic_name)s();

""" % {
                'pyllars_scope': self.element.pyllars_scope,
                'basic_name' : self.element.basic_name,
                'name': self.element.name or "anonymous_%s" % self.element.tag,
                'parent': self.element.parent.full_name,
                'parent_name': qualified_name(self.element.parent.name if self.element.parent.name != '::' else ''),
                'parent_full_name': self.element.parent.full_name,
                'full_type_name': self.element.type_.full_name,
                'qual': 'Const' if self.element.type_.is_const else '',
                'imports': "\n".join(
                    ["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in
                     imports]),
                'template_decl': template_decl(self),
                'template_args': self.element.template_arguments_string()
            }).encode('utf-8'))
        elif isinstance(self.element.parent, parser.NamespaceDecl):
            # global or namespace var:
            scoped.write(("""
                constexpr cstring name = "%(name)s";
                
                %(template_decl)s
                status_t %(pyllars_scope)s::%(basic_name)s::%(basic_name)s_init(){
                    status_t status = 0;
                    PyObject* mod = %(module_name)s;
                    %(imports)s
                    if( !__pyllars_internal::GlobalVariable::createGlobalVariable<%(full_type_name)s>("%(name)s", "%(tp_name)s",
                        &%(parent)s::%(name)s, mod, %(array_size)s)){
                       status = -1;
                     }
                    return status;
                }
                

                status_t %(pyllars_scope)s::%(basic_name)s::%(basic_name)s_register( pyllars::Initializer* const){
                    status_t status = 0;
                    // do nothing
                    return status;
                }

                 %(template_decl)s
                 %(pyllars_scope)s::%(basic_name)s%(template_args)s::Initializer_%(basic_name)s
                 *%(pyllars_scope)s::%(basic_name)s%(template_args)s::Initializer_%(basic_name)s::initializer = 
                 new %(pyllars_scope)s::%(basic_name)s%(template_args)s::Initializer_%(basic_name)s();
""" % {
                'pyllars_scope': self.element.pyllars_scope,
                'basic_name': self.element.basic_name,
                'name': self.element.full_name or "anonymous_%s" % self.element.tag,
                'tp_name': self.element.type_.name,
                'parent': self.element.parent.full_name if self.element.parent.full_name != '::' else "",
                'parent_name': qualified_name(self.element.parent.name if self.element.parent.name else "pyllars"),
                'module_name': self.element.parent.pyllars_module_name,
                'parent_full_name': self.element.parent.full_name,
                'full_type_name': self.element.type_.full_name,
                'array_size': self.element.type_.array_size or 0,
                'qual': 'Const' if self.element.type_.is_const else 'cont',
                'imports': "\n".join(
                    ["if(!PyImport_ImportModule(\"pylllars%s\")){return -1;} " % n.replace("::", ".") for n in
                     imports]),
                'template_args': self.element.template_arguments_string(),
                'template_decl': template_decl(self)
            }).encode('utf-8'))
        else:
            logging.error("Unknown parent type for global var")