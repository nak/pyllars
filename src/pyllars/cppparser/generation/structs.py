import os

from pyllars.cppparser.generation.base2 import GeneratorBody, GeneratorHeader
from pyllars.cppparser.parser import code_structure
from .base import qualified_name


class GeneratorBodyCXXRecordDecl(GeneratorBody):

    def has_default_constructor(self):
        return not [child for child in self._element.children() + self._element._inaccessible_children if isinstance(child, code_structure.CXXConstructorDecl)]

    def generate(self):
        if not self._element.is_definition or self._element.is_implicit:
            return

        self._stream.write(("                //From: %(file)s: GeneratorBodyCXXRecordDecl.generate\n" % {
            'file': os.path.basename(__file__),
        }).encode('utf-8'))

        if not self._element.is_union:
            typename = 'typename'
        else:
            typename = ""
        with self._scoped(self._stream) as stream:
            self._stream.write(self.decorate("""
                    class Initializer_%(basic_name)s: public pyllars::Initializer{
                    public:
                        Initializer_%(basic_name)s():pyllars::Initializer(){
                            %(parent_name)s_register(this);                          
                        }

                        virtual int init(PyObject * const global_mod){
                           int status = pyllars::Initializer::init(global_mod);
                           return status == 0?%(basic_name)s_init(global_mod):status;
                        }
                        static Initializer_%(basic_name)s *initializer;
                     };

                            """ % {
                    'basic_name': self._element.name,
                    'parent_name': self._element.parent.name or "pyllars"
                }).encode('utf-8'))
            stream.write(self.decorate("""
                status_t %(name)s_init(PyObject * const global_mod){
                    using namespace __pyllars_internal;
                    typedef %(typename)s ::%(scope)s::%(name)s  main_type;
                    static status_t _status = -1;
                    static bool inited = false;
                    if (inited){
                        return _status;
                    }
                    inited = true;
                    status_t status = 0;
    """ % {
                'qname': qualified_name(self._element.name or "anonymous_%s" % self._element.tag),
                'name': self._element_name or "anonymous_%s" % self._element.tag,
                'scope': self._element.scope,
                'typename': typename,
            }).encode('utf-8'))

            for base in self._element.base_classes or []:
                stream.write(("""
                    status |= pyllars%(base_class_name)s::%(base_class_bare_name)s_init(PyObject* const global_mod);
                     __pyllars_internal::PythonClassWrapper< %(typename)s %(scope)s::%(class_name)s >::addBaseClass
                        (&PythonClassWrapper< %(typename)s %(base_class_name)s >::Type); /*1*/
    """ % {
                    'class_name': self._element.name,
                    'base_class_name': base.full_name,
                    'base_class_bare_name': base.name,
                    'scope': self._element.scope,
                    'typename': typename
                }).encode('utf-8'))
            if self.has_default_constructor():
                stream.write(("""
                    static const char* const argumentNames[] = {nullptr};
                    __pyllars_internal::PythonClassWrapper<::%(scope)s::%(name)s>::template addConstructor<>(argumentNames);
                """ %{
                    'scope': self._element.scope,
                    'name': self._element.name or "_anonymous%s" % self._element.tag
                }).encode('utf-8'))

            if self._element.name:
                if self._element.parent and self._element.parent.is_namespace:
                    stream.write(("""
                    status |=  __pyllars_internal::PythonClassWrapper< main_type >::initialize(
                                 "%(name)s",
                                 "%(name)s",
                                 %(module_name)s,
                                 "%(full_name)s");  //classes
        """ % {
                        'name': self._element.name or "_anonymous%s" % self._element.tag,
                        'module_name': self._element.parent.python_cpp_module_name,
                        'full_name': self._element.full_name,
                    }).encode('utf-8'))
                elif isinstance(self._element.parent, code_structure.TranslationUnitDecl):
                    stream.write(("""
                            status |=  __pyllars_internal::PythonClassWrapper< main_type >::initialize(
                                         "%(name)s",
                                         "%(name)s",
                                         "pyllars",
                                         "%(full_name)s");  //classes
                """ % {
                        'name': self._element.name or "_anonymous%s" % self._element.tag,
                        'full_name': self._element.full_name,
                    }).encode('utf-8'))

                elif self._element.parent:
                    stream.write(("""
                    status |=  __pyllars_internal::PythonClassWrapper< main_type >::initialize(
                                 "%(name)s",
                                 "%(name)s",
                                 nullptr,
                                 "%(full_name)s");  //classes
                                 
                    __pyllars_internal::PythonClassWrapper< %(typename)s ::%(parent_class_scope)s::%(parent_class_name)s >::addClassMember
                        ("%(class_name)s",
                         (PyObject*) & __pyllars_internal::PythonClassWrapper< %(typename)s ::%(scope)s::%(class_name)s >::Type);
    """ % {
                        'parent_class_name': self._element.parent.name,
                        'parent_class_scope': self._element.parent.scope,
                        'class_name': self._element.name,
                        'full_name': self._element.full_name,
                        'name': self._element.name,
                        'scope': self._element.scope,
                        'typename': 'typename' if not self._element.is_union else ""
                    }).encode('utf-8'))
            stream.write(b"""
                    _status = status;
                    return status;
                }
            """)
            stream.write(self.decorate("""
    
                static Initializer_%(name)s* _init(){
                    static %(name)s::Initializer_%(name)s *_initializer = 
                      new %(name)s::Initializer_%(name)s();
                    %(name)s::Initializer_%(name)s::initializer = _initializer;
                    return _initializer;
                }
                """ % {
                'name': self._element.name if self._element.name else "pyllars",
                'parent_name': qualified_name
                (self._element.parent.name if (
                            self._element.parent and self._element.parent.name and self._element.parent.name != "::")
                 else "pyllars"),
            }).encode('utf-8'))

            stream.write(self.decorate("""
                status_t %(qname)s_register( pyllars::Initializer* const init ){ 
                    static Initializer_%(basic_name)s *_initializer = _init();
                    Initializer_%(basic_name)s::initializer = _initializer;
                    return Initializer_%(basic_name)s::initializer->register_init(init);
                 }
    
    
                """ % {
                'name': self._element.name if self._element.name else "pyllars",
                'basic_name': self._element.name,
                'qname': self._element.name if self._element.name else "pyllars",
                'parent_name': qualified_name
                (self._element.parent.name if (
                            self._element.parent and self._element.parent.name and self._element.parent.name != "::")
                 else "pyllars"),
            }).encode('utf-8'))

            stream.write(self.decorate("""
                %(typename)s %(name)s::Initializer_%(name)s 
                *%(name)s::Initializer_%(name)s::initializer = _init();
            """ % {
                'name': self._element.name,
                'typename': 'typename' if not self._element.is_union else "",
            }).encode('utf-8'))


class GeneratorHeaderCXXRecordDecl(GeneratorHeader):

    def output_include_directives(self):
        super().output_include_directives()
        for subelement in self._element.base_classes:
            child_generator = GeneratorHeader._get_generator_class(subelement)
            self._stream.write(b"#include \"%s\"" % child_generator._header_file_path.encode('utf-8'))


