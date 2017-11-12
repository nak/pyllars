from .base import Folder, Generator, qualified_name
from . import parser, TextIOBase


class NamespaceDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return True

    def generate_header_core(self, stream: TextIOBase, as_top=False) -> None:
        if self.element.name:
            stream.write(("""
        extern PyModuleObject *%s_mod;
    """ % self.element.name).encode('utf-8'))

    def generate_spec(self):
        file_name = self.to_path(ext=".hpp")
        if self.element.name == "::" or not self.element.name:
            with self.folder.open(file_name) as stream:
                stream.write(b"")
                return
        self.folder.purge(file_name)
        with self.folder.open(file_name=file_name) as stream:
            with self.guarded(stream) as guarded:
                with self.scoped(guarded) as scoped:
                    if self.element.parent:
                        scoped.write(self.basic_includes())
                        scoped.write(("""
                    #include "%(parent_header_name)s"
                    #include "%(target_file_name)s"
        """ % {
                            'parent_header_name': self.parent.header_file_path(),
                            'target_file_name': self._src_path}).encode("utf-8"))
                    scoped.write(("""
                        namespace %(qname)s{
                            int %(qname)s_register( pyllars::Initializer* const);
                            extern PyObject* %(name)s_mod;
                        }
                    #endif
                    """ % {'name': self.sanitize(self.element.name),
                           'qname': qualified_name(self.element.name),
                           }).encode('utf-8'))

    def generate_body_proper(self, scoped: TextIOBase, as_top: bool = False) -> None:
        if not self.element.parent:
            return

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
            'name': self.sanitize(self.element.name),
            'qname': qualified_name(self.element.name),
            'fullname': self.element.full_name,
            'parent': qualified_name(
                self.element.parent.name if self.element.parent.name and self.element.parent.name != "::" else "pyllars"),
            'parent_name': (self.element.scope if self.element.scope != '::' else ""),
        }).encode('utf-8'))
