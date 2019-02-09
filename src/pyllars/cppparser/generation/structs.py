import os
import logging

from pyllars.cppparser.generation.base2 import GeneratorBody, GeneratorHeader
from pyllars.cppparser.parser import code_structure
from .base import qualified_name


log = logging.getLogger(__name__)


class GeneratorBodyCXXRecordDecl(GeneratorBody):

    def has_default_constructor(self):
        return self._element.is_definition and \
               not [child for child in self._element.children() + self._element._inaccessible_children if isinstance(child, code_structure.CXXConstructorDecl)]

    def generate(self):
        if not self._element.name and self._element.children():
            self._stream.write(("""
                    namespace __pyllars_internal{
                        template<>
                        const char* const
                        type_name<decltype(%(name)s)>(){
                            static const char* const name = "anonymous enum";
                            return name;
                        }

                         template<>
                        const char* const
                        type_name<decltype(%(name)s) const>(){
                            static const char* const name = "const anonymous enum";
                            return name;
                        }

                        template<>
                        const char* const
                        Types<decltype(%(name)s)>::type_name(){
                            static const char* const name = "anonymous enum";
                            return name;
                        }
                    }
            """ % {'name': self._element.children()[0].full_name}).encode('utf-8'))
        if not self._element.is_definition or self._element.is_implicit or not self._element.name:
            return

        self._stream.write(("                //From: %(file)s: GeneratorBodyCXXRecordDecl.generate\n" % {
            'file': os.path.basename(__file__),
        }).encode('utf-8'))

        if not self._element.is_union:
            typename = 'typename'
        else:
            typename = ""
        with self._scoped(self._stream) as stream:
            self._stream.write(self.decorate(self.INITIALIZER_CODE% {
                'name': self._element.name or "anonymous_%s" % self._element.tag,
                'parent_name': self._element.parent.name or "pyllars"
            }).encode('utf-8'))

            self._stream.write(self.decorate(self.INITIALIZER_INSTANTIATION_CODE % {
                'name': self._element.name or "anonymous_%s" % self._element.tag,
            }).encode('utf-8'))

            stream.write(self.decorate("""
                status_t %(name)s_init(PyObject * const global_mod){
                    using namespace __pyllars_internal;
                    typedef %(typename)s %(scope)s::%(name)s  main_type;
                    static status_t _status = -1;
                    static bool inited = false;
                    if (inited){
                        return _status;
                    }
                    inited = true;
                    status_t status = 0;
    """ % {
                'name': self._element_name or "anonymous_%s" % self._element.tag,
                'scope': '::' + self._element.scope if self._element.scope else "",
                'typename': typename,
            }).encode('utf-8'))

            for base in self._element.base_classes or []:
                stream.write(("""
                    status |= pyllars%(base_class_name)s::%(base_class_bare_name)s_init(PyObject* const global_mod);
                     __pyllars_internal::PythonClassWrapper< %(typename)s %(scope)s::%(class_name)s >::addBaseClass
                        (&PythonClassWrapper< %(typename)s %(base_class_name)s >::getPyType()); /*1*/
    """ % {
                    'class_name': self._element.name,
                    'base_class_name': base.full_name,
                    'base_class_bare_name': base.name,
                    'scope': '::' + self._element.scope if self._element.scope else "",
                    'typename': typename
                }).encode('utf-8'))
            if self.has_default_constructor():
                stream.write(("""
                    static const char* const argumentNames[] = {nullptr};
                    __pyllars_internal::PythonClassWrapper<%(scope)s::%(name)s>::template addConstructor<>(argumentNames);
                """ %{
                    'scope': '::' + self._element.scope if self._element.scope else "",
                    'name': self._element.name or "_anonymous%s" % self._element.tag
                }).encode('utf-8'))

            if self._element.name:
                if self._element.parent and self._element.parent.is_namespace:
                    stream.write(("""
                    status |=  __pyllars_internal::PythonClassWrapper< main_type >::initialize("%(name)s");  //classes
        """ % {
                        'name': self._element.name or "_anonymous%s" % self._element.tag,
                    }).encode('utf-8'))
                elif isinstance(self._element.parent, code_structure.TranslationUnitDecl):
                    stream.write(("""
                            PyObject* pyllars_mod = PyImport_ImportModule("pyllars");
                            status |=  __pyllars_internal::PythonClassWrapper< main_type >::initialize("%(name)s");
                            PyObject *module = global_mod?global_mod : pyllars_mod;
                            if(module){
                                PyModule_AddObject(module, "%(name)s", (PyObject*) __pyllars_internal::PythonClassWrapper< main_type >::getPyType());
                            }
                """ % {
                        'name': self._element.name or "_anonymous%s" % self._element.tag,
                    }).encode('utf-8'))

                elif self._element.parent:
                    stream.write(("""
                    status |=  __pyllars_internal::PythonClassWrapper< main_type >::initialize("%(name)s");
                                 
                    __pyllars_internal::PythonClassWrapper< %(typename)s ::%(parent_class_scope)s::%(parent_class_name)s >::addClassMember
                        ("%(class_name)s",
                         (PyObject*) & __pyllars_internal::PythonClassWrapper< %(typename)s ::%(scope)s::%(class_name)s >::getPyType());
    """ % {
                        'parent_class_name': self._element.parent.name,
                        'parent_class_scope': self._element.parent.scope,
                        'class_name': self._element.name,
                        'name': self._element.name,
                        'scope': self._element.scope,
                        'typename': 'typename' if not self._element.is_union else ""
                    }).encode('utf-8'))
            stream.write(b"""
                    _status = status;
                    return status;
                }
            """)

            stream.write(self.decorate(self.REGISTRATION_CODE % {
                'name': self._element.name if self._element.name else "pyllars",
            }).encode('utf-8'))


class GeneratorHeaderCXXRecordDecl(GeneratorHeader):

    def output_include_directives(self):
        super().output_include_directives()
        for subelement in self._element.base_classes:
            child_generator = GeneratorHeader._get_generator_class(subelement)
            self._stream.write(b"#include \"%s\"" % child_generator._header_file_path.encode('utf-8'))


class GeneratorHeaderEnumDecl(GeneratorHeaderCXXRecordDecl):
    pass


class GeneratorBodyEnumDecl(GeneratorBodyCXXRecordDecl):
    pass


class GeneratorBodyEnumConstantDecl(GeneratorBody):
    
    def generate(self):
        self._stream.write(("                //From: %(file)s: GeneratorBodyEnumConstantDecl.generate\n" % {
            'file': os.path.basename(__file__),
        }).encode('utf-8'))
        with self._scoped(self._stream) as stream:
            self._stream.write(self.decorate(self.INITIALIZER_CODE % {
                'name': self._element.name,
                'parent_name': self._element.parent.name or "pyllars"
            }).encode('utf-8'))

            self._stream.write(self.decorate(self.INITIALIZER_INSTANTIATION_CODE % {
                'name': self._element.name
            }).encode('utf-8'))

            parent = self._element.parent
            while parent and not parent.name:
                parent = parent.parent
            if parent and parent.full_name:
                parent_full_name = parent.full_name
            else:
                parent_full_name = "decltype(%s)" % self._element.full_name
            if isinstance(parent, (code_structure.EnumDecl, code_structure.RecordTypeDefn)):
                # static class member var:
                stream.write(("""
                    constexpr cstring name = "%(name)s";
                    constexpr cstring type_name = "%(parent_name)s";
                    """ % {
                    'name': self._element.full_name,
                    'parent_name': self._element.parent.name if self._element.parent.name else 'decltype(%s)' % self._element.full_name,
                }).encode('utf-8'))
                stream.write(self.decorate("""
                    status_t %(name)s_init(PyObject * const global_mod){
                        PyObject* pyllars_mod = PyImport_ImportModule("pyllars");
                        __pyllars_internal::PythonClassWrapper<%(parent_full_name)s>::initialize("%(parent_name)s",
                                                                      "%(parent_name)s",
                                                                      global_mod?global_mod:pyllars_mod);
                        return __pyllars_internal::PythonClassWrapper<%(parent_full_name)s>::template addEnum%(suffix)s("%(name)s", %(full_name)s);
                    }
                """ % {
                    'name': self._element.name or "anonymous_%s" % self._element.tag,
                    'full_name': self._element.full_name,
                    'parent_full_name': parent_full_name,
                    'parent_name': self._element.parent.name if self._element.parent.name else 'decltype(%s)' % self._element.full_name,
                    'suffix': "Value" if not isinstance(parent, code_structure.CXXRecordDecl) else "ClassValue<type_name, decltype(%s)>" % self._element.full_name,
                }).encode('utf-8'))
                stream.write(self.decorate("""
                    
                    status_t %(name)s_register( pyllars::Initializer* const){
                        status_t status = 0;
                        // do nothing
                        return status;
                    }
                 """ % {
                    'name': self._element.name or "anonymous_%s" % self._element.tag,
                }).encode('utf-8'))

            elif not parent or isinstance(parent, (code_structure.NamespaceDecl, code_structure.TranslationUnitDecl)):
                # global or namespace var:y
                stream.write(b"""
                    constexpr cstring name = "%s";
                """ % self._element.name.encode('utf-8'))
                stream.write(self.decorate("""
                    status_t %(name)s_init(PyObject * const global_mod){
                        status_t status = 0;
                        PyObject* mod = %(module_name)s;
                        PyObject* value = PyInt_FromLong(static_cast<long int>(%(parent_full_name)s::%(name)s));
                        if (!mod || !value){
                            status = -1;
                        } else {
                            status = PyModule_AddObject(mod, "%(name)s", value);
                        }
                        return status;
                    }""" % {
                    'name': self._element.name or "anonymous_%s" % self._element.tag,
                    'parent_full_name': parent_full_name,
                    'module_name': self._element.parent.python_cpp_module_name if parent else "global_mod?global_mod:PyImport_ImportModule(\"pyllars\")",
                }).encode('utf-8'))
                stream.write(self.decorate("""
                    status_t %(name)s_register( pyllars::Initializer* const){
                        status_t status = 0;
                        // do nothing
                        return status;
                    }
                    """ % {
                    'name': self._element.name or "anonymous_%s" % self._element.tag,

                }).encode('utf-8'))

            else:
                log.error("Unknown parent type for enum var")
