from .base import Folder, Generator, qualified_name
from . import parser, TextIOBase


class NamespaceDecl(Generator):
    @classmethod
    def is_generatable(cls):
        return True

    def generate_header_core(self, element: Element, stream: TextIOBase, as_top=False) -> None:
        if element.name:
            stream.write(("""
        extern PyModuleObject *%s_mod;
    """ % element.name).encode('utf-8'))

    @classmethod
    def generate_spec(cls, element: parser.Element, folder: Folder, src_path: str):
        file_name = cls.to_path(element.name or "global", ext=".hpp")
        if element.name == "::" or not element.name:
            with folder.open(file_name=file_name) as stream:
                stream.write("")
                return
        generator = cls.get_generator(type(element), src_path, "")
        if not generator:
            return
        folder.purge(file_name)
        with folder.open(file_name=file_name) as stream:
            namespace_text, namespace_closure = generator.namespaces(element)
            stream.write(("""
            #ifndef __%(guard)s__
            #define __%(guard)s__

""" % {'guard': element.guard}).encode('utf-8'))
            if element.parent:
                stream.write(("""
            #include "%(parent_header_name)s"
            #include "%(target_file_name)s"
""" % {
                    'parent_header_name': cls.header_file_path(element.parent),
                    'target_file_name': src_path}).encode("utf-8"))
            stream.write(("""
                namespace pyllars{
                    %(namespaces)s
                        namespace %(qname)s{
                            int %(qname)s_register( pyllars::Initializer* const);
                            extern PyObject* %(name)s_mod;
                        }
                    %(closure)s
                }
            #endif
            """ % {'name': cls.sanitize(element.name),
                   'qname': qualified_name(element.name),
                   'namespaces': namespace_text,
                   'closure': namespace_closure,
                   }).encode('utf-8'))

    def generate_body_proper(self, element: parser.Element, stream: TextIOBase, src_path, as_top: bool = False) -> None:
        if not element.parent:
            return

        with self.scoped(element, stream) as scoped:
            scoped.write(("""
                status_t init_me(){
                    if (%(name)s_mod) return 0;// if already initialized
                    int status = 0;
                    #if PY_MAJOR_VERSION==3
                    static PyModuleDef %(name)s_moddef = {
                        PyModuleDef_HEAD_INIT,
                        "noddy",
                        "Example module that creates an extension type.",
                        -1,
                        NULL, NULL, NULL, NULL, NULL
                    };
                    %(name)s_mod = PyModule_Create(&%(name)s_moddef);
                    #else
                    %(name)s_mod = Py_InitModule3("%(name)s", nullptr,
                                                  "Module corresponding to C++ namespace %(fullname)s");
                    #endif
                    if(! %(name)s_mod ){
                        status = -1;
                    }
                    return status;
                } // end init

                int %(qname)s_register( pyllars::Initializer* const init ){
                    static pyllars::Initializer _initializer = pyllars::Initializer();
                    static int status = pyllars%(parent_name)s::%(parent)s_register(&_initializer);
                    return status==0?_initializer.register_init(init):status;
                 }


                 PyObject *%(name)s_mod = nullptr;

                int %(name)s_init(){return %(name)s::init_me();}
    """ % {
                'indent': self._indent,
                'name': self.sanitize(element.name),
                'qname': qualified_name(element.name),
                'fullname': element.full_name,
                'parent': qualified_name(
                    element.parent.name if element.parent.name and element.parent.name != "::" else "pyllars"),
                'parent_name': (element.scope if element.scope != '::' else ""),
            }).encode('utf-8'))
