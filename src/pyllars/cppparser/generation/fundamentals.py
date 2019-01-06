from io import TextIOBase
import logging
import os

from pyllars.cppparser.parser import code_structure
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
        argslambda = ", ".join([arg.as_function_argument(index) for
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
            'module_name': self.element.parent.python_cpp_module_name if self.parent else "pyllars_mod",
            'return_type': self.element.return_type.full_name if self.element.return_type else "void",
            'arguments': (',' if len(self.element.params) > 0 else "") +
                          ', '.join([t.target_type.full_name for t in self.element.params]),
            'lambdacode': lambda_code,
            'has_varargs': str(self.element.has_varargs).lower(),
            'throws': "" if self.element.throws is None else "void" if len(self.element.throws) == 0
            else ",".join(self.element.throws),
            'func_name': self.element.name,
        }

    def generate_header_core(self, stream: TextIOBase, as_top=False):
        if 'operator delete' in self.element.name or 'operator new' in self.element.name:
            return
        super(FunctionDecl, self).generate_header_core(stream, as_top=as_top)

    def generate_body_proper(self, scoped: TextIOBase, as_top: bool = False) -> None:
        if 'operator delete' in self.element.name or 'operator new' in self.element.name:
            return
        imports = set([])
        for elem in self.element.params:
            if elem and elem.target_type.scope != self.element.scope and elem.target_type.scope != "::":
                imports.add(elem.scope)
        if self.element.return_type \
                and self.element.return_type.scope != self.element.scope \
                and self.element.return_type.scope != "::":
            imports.add(self.element.return_type.namespace.name)

        from .structs import CXXMethodDecl
        scoped.write(("""
            constexpr cstring name = "%(pyname)s";

            //generated from %(file)s.generate_body_proper
            // FUNCTION %(name)s THROWS %(throws)s
            status_t %(pyllars_scope)s::%(name)s::%(name)s_init(PyObject * const global_mod){
               static const char* const argumentNames[] = {%(argument_names)s nullptr};
               status_t status = 0;
               %(imports)s
               %(func_decl)s
               return status;
            }
            status_t %(pyllars_scope)s::%(name)s::%(name)s_register(pyllars::Initializer*){
                //do nothing, functions have no children
                return 0;
            }

            %(pyllars_scope)s::%(basic_name)s::Initializer_%(basic_name)s
            *%(pyllars_scope)s::%(basic_name)s::Initializer_%(basic_name)s::initializer =
            new %(pyllars_scope)s::%(basic_name)s::Initializer_%(basic_name)s();
            
""" % {
            'file': __file__,
            'basic_name': self.element.name,
            'pyllars_scope': self.element.pyllars_scope,
            'imports': "\n".join(
                ["if(!PyImport_ImportModule(\"pylllars.%s\")){PyErr_Clear();} " % n.replace("::", ".") for n in
                 imports if n]),
            'module_name': self.element.parent.python_cpp_module_name if self.parent else "pyllars_mod",
            'name': self.sanitize(self.element.name),
            'pyname': CXXMethodDecl.METHOD_NAMES.get(self.element.name).replace('addMethod', '') if
            self.element.name in CXXMethodDecl.METHOD_NAMES else self.element.name if self.element.name != "operator=" else "assign_to",
            'parent_name': qualified_name
                (self.element.parent.name if (self.element.parent.name and self.element.parent.name != "::")
                                          else "pyllars"),
            'parent': self.element.scope,
            # 'template_decl': template_decl(self),
            # 'template_args': self.element.template_arguments_string(),
            'argument_names': ','.join(["\"%s\"" % (arg.name if arg.name else "_%s" % (index + 1)) for index, arg in
                                        enumerate(self.element.params)]) + (',' if self.element.params else ''),
            'has_varargs': str(self.element.has_varargs).lower(),
            'throws': "" if self.element.throws is None else "void" if len(self.element.throws) == 0
            else ",".join(self.element.throws),
            'func_decl': self._func_declaration() if not self.element.is_template else "",
            'return_type': self.element.return_type_spec,
            'arguments': (',' if len(self.element.params) > 0 else "") + ', '.join([t.target_type.full_name for
                                                                               t in self.element.params]),
        }).encode('utf-8'))

