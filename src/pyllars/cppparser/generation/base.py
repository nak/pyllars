import logging
import os
import shlex
import subprocess
import sysconfig
from abc import ABCMeta
from contextlib import contextmanager
from io import TextIOBase

from .. import parser
import pkg_resources

pyllars_resources_dir = pkg_resources.resource_filename("pyllars", os.path.join("..", "resources"))
INDENT = "    "


def qualified_name(name):
    return Generator.sanitize(name.replace("(", "_lparen_").replace(")", "_rparen_").replace(":", "_")).replace(" ", "_")


class Compiler(object):

    CFLAGS = sysconfig.get_config_var('CFLAGS') or ""
    LDFLAGS = sysconfig.get_config_var('LDFLAGS') or ""
    LDCXXSHARED = sysconfig.get_config_var('LDCXXSHARED') or ""
    PYINCLUDE = sysconfig.get_config_var('INCLUDEPY')
    CXX = sysconfig.get_config_var('CXX')
    PYLIB = sysconfig.get_config_var('BLDLIBRARY')

    def __init__(self, folder):
        self._compileables = []
        for root, dirs, files in os.walk(folder):
            for fil in [f for f in files if f.endswith(".cpp")]:
                self._compileables.append(os.path.join(root, fil))
        self._folder = folder

    def compile_all(self, src_path:str, output_module_path: str):
        objects = []
        body = src_path.replace(".hpp", ".cpp")
        for compilable in self._compileables:
            target = os.path.join("objects", compilable[10:]).replace(".cpp", ".o")
            if not os.path.exists(os.path.dirname(target)):
                os.makedirs(os.path.dirname(target))
            cmd = "%(cxx)s -O -std=c++14 %(cxxflags)s -c -fPIC -I%(local_include)s -I%(python_include)s " \
                  "-I%(pyllars_include)s -o \"%(target)s\" \"%(compilable)s\"" % {
                      'cxx': Compiler.CXX,
                      'cxxflags': Compiler.CFLAGS,
                      'local_include': self._folder,
                      'pyllars_include': pyllars_resources_dir,
                      'python_include': Compiler.PYINCLUDE,
                      'target': target,
                      'compilable': compilable,
                  }
            cmd = cmd.replace("-O2", "-O0")
            print(cmd)
            p = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE,
                                 stderr=subprocess.STDOUT)
            output = p.communicate()[0].decode('utf-8')
            if p.returncode != 0:
                return p.returncode, "Command \"%s\" failed:\n%s" % (cmd, output)
            objects.append("\"%s\"" % target)
        cmd = "%(cxx)s -O -fPIC -std=c++14 %(cxxflags)s -I%(python_include)s -shared -o objects/%(output_module_path)s -Wl,--no-undefined " \
              "%(src)s %(objs)s %(python_lib_name)s -lffi %(pyllars_include)s/pyllars/pyllars.cpp" % {
                  'cxx': Compiler.LDCXXSHARED,
                  'src': body,
                  'cxxflags': Compiler.CFLAGS,
                  'output_module_path': output_module_path,
                  'objs': " ".join(objects),
                  'pyllars_include': pyllars_resources_dir,
                  'python_include': Compiler.PYINCLUDE,
                  'python_lib_name': Compiler.PYLIB,
              }
        print(cmd)
        p = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        output = p.communicate()[0].decode('utf-8')
        return p.returncode, cmd + "\n" + output


class Folder(object):
    def __init__(self, path: str, parent: "Folder" = None):
        assert os.path.isdir(path)
        self._path = path
        self._parent = parent

    @property
    def path(self):
        return self._path

    def purge(self, file_name: str) -> None:
        full_path = os.path.join(self._path, file_name)
        if os.path.exists(full_path):
            os.remove(full_path)

    @contextmanager
    def open(self, file_name: str) -> TextIOBase:
        from . import FileWriter
        open_file = None
        try:
            full_path = os.path.join(self._path, file_name)
            if not os.path.exists(os.path.dirname(full_path)):
                os.makedirs(os.path.dirname(full_path))
            open_file = FileWriter.open(full_path, 'ab')
            yield open_file
        finally:
            if open_file:
                open_file.close()

    def create_subfolder(self, name: str)->"Folder":
        folder_name = os.path.join(self._path, name)
        assert not os.path.isfile(folder_name)
        if not os.path.exists(folder_name):
            os.makedirs(folder_name)
        return Folder(folder_name, parent=self)

    @property
    def base_name(self) -> str:
        return os.path.basename(self._path) if self._parent else "."

    @property
    def path(self) -> str:
        if self._parent:
            return os.path.join(self._parent.path, self.base_name)
        return "."


class Generator(metaclass=ABCMeta):

    MODE_FULL_HEADER = 0
    MODE_SEPARATE_HEADERS = 1
    MODE_EMPTY_HEADER = 2

    generator_mapping = {}

    def __init__(self, element: parser.Element, src_path: str, folder: Folder, parent_generator: "Generator"=None):
        self._mode = parent_generator.mode if parent_generator else Generator.MODE_SEPARATE_HEADERS
        if parent_generator and parent_generator._mode == Generator.MODE_FULL_HEADER:
            self._mode = Generator.MODE_EMPTY_HEADER
        elif element.is_template_macro and element.parent and element.parent.is_namespace:
            self._mode = Generator.MODE_FULL_HEADER
        self._src_path = src_path
        self._template_argument_lists = []
        self._template_type_params = []
        self._folder = folder
        self._subfolder = None
        self._element = element
        self._parent = parent_generator
        Generator.generator_mapping[element] = self
        for child in element.children():
            if element not in Generator.generator_mapping:
                child_generator_class = self.get_generator_class(child)
                # store for future reference:
                child_generator_class(child, src_path=src_path, folder=self.subfolder, parent_generator=self)


    @classmethod
    def is_generatable(cls):
        return False

    def header_file_path(self) -> str:
        header_name = self.header_file_name()
        return os.path.join(self.folder.path, header_name)

    @classmethod
    def sanitize(cls, text):
        map = {'-' : '_minus_',
         '=': '_eq_',
         '&': '_amp_',
         '<': '_lt_',
         '>': '_gt_',
         '*': '_star_',
         '|': '_or_',
         '!': '_not_',
         '^': '_xor_',
         '[': '_lparen_',
         ']': '_rparen',
         '/': '_div_'}
        for c in list(map.keys()):
            text = text.replace(c, map[c])
        return text

    def header_file_name(self)->str:
        return (self.element.basic_name or "global").replace(" ", "_") + ".hpp"

    def body_file_path(self) ->str:
        parent_path = self.folder.path
        if not os.path.exists(parent_path):
            os.makedirs(parent_path)
        return os.path.join(parent_path, self.to_path(ext=".cpp"))

    def body_file_name(self)->str:
        return (self.element.basic_name or "global") + ".cpp"

    @property
    def mode(self):
        return self._mode

    @property
    def element(self):
        return self._element

    @property
    def folder(self):
        return self._folder

    @property
    def subfolder(self):
        if self._subfolder is None:
            self._subfolder = self.folder.create_subfolder(self.element.name)
        return self._subfolder

    @property
    def parent(self):
        return self._parent

    def to_path(self, ext="") -> str:
        name = self.element.basic_name if self.element.name else "global"
        return name.replace(" ", "_") + ext

    def scope(self):
        return self.element.scope

    def basic_includes(self):
        return ("""
#include <Python.h>
#include <pyllars/pyllars.hpp>
#include <pyllars/pyllars_classwrapper.cpp>
#include <pyllars/pyllars_function_wrapper.hpp>

#include <vector>
#include <cstddef>
#include <%(src_path)s>
%(parent_header_name)s
"""  % {'src_path': self._src_path,
        'parent_header_name': "#include \"%s\"" % self.parent.header_file_path() if self.element.parent else "",
        }).encode('utf-8')

    @contextmanager
    def scoped(self, stream: TextIOBase):
        with self._ns_scope(stream) as stream:
            yield stream

    @contextmanager
    def _ns_scope(self, stream: TextIOBase):
        need_closure = False
        if self.parent:
            with self.parent._ns_scope(stream) as scoped:
                if not self.element.is_template_macro and not isinstance(self.element, (parser.FieldDecl, parser.CXXMethodDecl)):
                    need_closure = True
                    scoped.write(("\nnamespace %s{\n" % qualified_name(self.element.basic_name)).encode("utf-8"))
                yield stream
        else:
            need_closure = True
            stream.write(b"\nnamespace pyllars{\n")
            yield stream
        if need_closure:
            stream.write(b"\n}")

    @contextmanager
    def templated(self, stream: TextIOBase):
        yield stream

    @contextmanager
    def guarded(self, stream : TextIOBase):
        stream.write(("""
            #ifndef __%(guard)s__
            #define __%(guard)s__
    
            """ % {'guard': self.element.guard}).encode('utf-8'))
        yield stream
        stream.write(b"""
            #endif
        """)

    @property
    def template_decl(self):
        template_decl = "\n%s" % parser.Element.template_declaration(self.element.template_arguments)
        return template_decl

    @property
    def template_arguments(self):
        return "" if not self.element.template_arguments else "<%s>" % (", ".join([e.name for e in self.element.template_arguments]))

    def generate_header_code(self, stream: TextIOBase) -> None:
        with self.guarded(stream) as guarded:
            guarded.write(self.basic_includes())
            if self._mode == Generator.MODE_EMPTY_HEADER:
                return
            with self.scoped(guarded) as scoped:
                self.generate_header_core(scoped)
                if self._mode == Generator.MODE_FULL_HEADER:
                    with self.templated(scoped) as templated:
                        for child in self.element.children():
                            generator = Generator.generator_mapping.get(child) or \
                                       self.get_generator_class(child)(child,
                                                                       self._src_path,
                                                                       self.folder.create_subfolder(child.basic_name),
                                                                       parent_generator=self)
                            if generator.is_generatable():
                                generator.generate_header_core_full(templated)

    def generate_header_core_full(self, stream: TextIOBase):
        with self.templated(stream) as templated:
            self.generate_header_core(stream)
            for child in self.element.children():
                generator = Generator.generator_mapping.get(child) or \
                            self.get_generator_class(child)(child, self._src_path,
                                                            self.folder.create_subfolder(child.basic_name),
                                                            parent_generator=self)
                if generator.is_generatable():
                    generator.generate_header_core_full(templated)

    def generate_header_core(self, stream: TextIOBase, as_top=False):
        if not self.element.name or self.element.is_anonymous_type:  # anonymous directly inaccessible type
            stream.write(b"")
            return
        stream.write(("""
                %(template_decl)s
                status_t %(basic_name)s_register( pyllars::Initializer* const);
                
                %(template_decl)s
                status_t %(basic_name)s_init();
                
                class Initializer_%(basic_name)s: public pyllars::Initializer{
                public:
                    Initializer_%(basic_name)s():pyllars::Initializer(){
                       %(pyllars_scope)s::%(parent_basic_name)s_register(this);
                    }

                    virtual int init(){
                       int status = pyllars::Initializer::init();
                       return status | %(basic_name)s_init();
                    }

                    static Initializer_%(basic_name)s* initializer;
                 };
            """ % {
                'name': self.element.name,
                'basic_name': self.sanitize(self.element.basic_name),
                'parent_basic_name': self.element.parent.basic_name if self.element.parent.basic_name else "pyllars",
                 'pyllars_scope': self.element.pyllars_scope,
                'parent_name': self.element.parent.basic_name if self.element.parent else "pyllars",
                'template_decl': self.template_decl,
        }).encode('utf-8'))

    @staticmethod
    def generate_code(element: parser.Element, src_path: str, folder: Folder, module_name: str):
        Generator.generator_mapping = {}
        generator_class = Generator.get_generator_class(element)
        generator = generator_class(element, src_path, folder)
        generator.generate_body(as_top=module_name)

    def generate_spec(self):
        file_name = self.to_path(ext=".hpp")
        self.folder.purge(file_name)
        with self.folder.open(file_name) as stream:
            self.generate_header_code(stream)

    def generate_body(self, as_top=False):
        if not self.element.name and not as_top:
            return
        if self.element.is_anonymous_type:
            return
        file_name = self.to_path(ext=".cpp")
        self.folder.purge(file_name)
        self.generate_spec()
        with self.folder.open(file_name) as stream:
            stream.write(("""
                #include "%(my_header_name)s"
                #include <pyllars/pyllars.hpp>
                #include <pyllars/pyllars_globalmembersemantics.cpp>
                #include <cstddef>
                """ % {"my_header_name": self.header_file_name()}).encode('utf-8'))
            if self.element.is_implicit:
                return
            if not self.element.name:
                self.element._anonymous_types.add(self)
            else:
                self.generate_body_proper(stream, as_top)
        for child in self.element.children():
            child_generator_class = self.get_generator_class(child)
            if child_generator_class.is_generatable():
                child_generator = child_generator_class(child, self._src_path, self.folder.create_subfolder(child.basic_name),
                                                        parent_generator=self)
                child_generator.generate_body()
        if as_top:
            stream.write(("""

            extern "C"{
                static const char* const name = "%(basic_name)s";
                static const char* const doc = "%(basic_name)s top-level C++ interface module";
                #if PY_MAJOR_VERSION == 3
                PyMODINIT_FUNC
                PyInit_%(basic_name)s(void){
                    static PyModuleDef moduleDef;
                    memset(&moduleDef, 0, sizeof(moduleDef));
                    moduleDef.m_name = name;
                    moduleDef.m_doc = doc;
                    moduleDef.m_size = -1;
                    PyObject *pyllars_mod = PyImport_ImportModule("pyllars");
                    if (!pyllars_mod){
                        return nullptr;
                    }                    
                    PyObject *%(basic_name)s_mod = PyModule_Create(&moduleDef);
                    if (%(basic_name)s_mod){
                        PyModule_AddObject(pyllars_mod, "%(basic_name)s", %(basic_name)s_mod);
                    }
                    if (%(pyllars_scope)s::Initializer::root->init() != 0){
                        return nullptr;
                    }
                    return %(basic_name)s_mod;
                }
                #else
                int _init%(basic_name)s(){
                    PyObject *%(basic_name)s_mod = Py_InitModule3(name, nullptr, doc);
                    if(!%(basic_name)s_mod) { return -1;}
                    PyObject *pyllars_mod = PyImport_ImportModule("pyllars");
                    if (!pyllars_mod){
                        return -1;
                    }
                    PyModule_AddObject(pyllars_mod, "%(basic_name)s", %(basic_name)s_mod);
                    return %(pyllars_scope)s::Initializer::root?pyllars::Initializer::root->init():0;
                }
                #endif
            }
            """ % {
            'basic_name': as_top,
            'pyllars_scope': self.element.pyllars_scope}).encode('utf-8'))


    @staticmethod
    def get_generator_class(element: parser.Element) -> "Generator":
        from . import _get_generator_class
        generator_class = _get_generator_class(element)
        return generator_class

    def generate_body_proper(self, stream: TextIOBase, as_top: bool = False):
        pass


class ParmVarDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return False


class ClassTemplateDecl(Generator):

    @classmethod
    def is_generatable(cls):
        return True

    def generate_spec(self, as_top=False):
        generator_class = self.get_generator_class(self.element)
        file_name = self.to_path(ext=".hpp")
        if not generator_class.is_generatable():
            if as_top:
                with  self.folder.open(file_name=file_name) as stream:
                    with self.guarded(stream) as guarded:
                        with self.scoped(guarded) as scoped:
                            scoped.write(("""
                        #include <pyllars/pyllars_classwrapper.hpp>

                        namespace pyllars{

                        ///////////////////////////////
                            namespace{
                                status_t pyllars_top_init();
                                status_t pyllars_top_register();
                            }

                        //////////////////////////////
    
                        }
                        #endif
                            """ % {
                                'top_name': as_top
                            }).encode('utf-8'))
            return
        self.folder.purge(file_name)
        with self.folder.open(file_name=file_name) as stream:
            if not self.element.name:
                stream.write(b"")
                return
            with self.guarded(stream) as guarded:
                with self.scoped(guarded) as scoped:

                    if self.element.parent:
                        scoped.write(("""
        #include "%(target_file_name)s"
        #include "%(parent_header_name)s"
        """ % {
                            'parent_header_name': self.header_file_path(self.element.parent),
                            'target_file_name': self._src_path}).encode("utf-8"))

                    scoped.write(("""
        #include <pyllars/pyllars_classwrapper.hpp>
        
        namespace pyllars{
    
///////////////////////////////

        namespace %(qname)s{
            %(template_decl)s
            status_t %(qname)s_init();
            
            %(template_decl)s
            class Initializer: public pyllars::Initializer{
            public:
               
               Initializer():pyllars::Initializer(){
                   %(qname)s_register(this);
               }
               
               virtual int init(){
                   int status = pyllars::Initializer::init();
                   return status | %(qname)s_init();
               }
               
            };
        
            %(template_decl)s
            static Initializer init = Initializer();  
        }

//////////////////////////////
        
}
#endif
            """ % {
                'qname': qualified_name(self.element.parent.basic_name),
                'parent': self.element.parent.full_name if self.element.parent.full_name != "::" else "",
                'parent_name': self.element.parent.basic_name,
                'template_arg_vals': ",".join([arg.py_var_name(index) for index, arg in enumerate(self.element.template_args)]),
                'full_name': self.element.full_name,
                'template_arg_len': len(self.element.template_args),
                'template_decl': self.element.template_decl,
                # TODO: REMOTE 'template_args': self.element.parent.template_arguments_string() if self.element.parent else "",
                    }).encode('utf-8'))

    def generate_body_proper(self, stream: TextIOBase, as_top: bool = False):
        with self.scoped(stream) as scoped:
            scoped.write(("            //From: %(file)s:generate_body_proper\n" % {
                'file': __file__
            }).encode('utf-8'))

            full_class_name = self.element.full_name
            if isinstance(self.element.parent, parser.NamespaceDecl):
                add_dict_code = """
                    PyModule_AddObject(
                        %(module_name)s, "%(name)s", mapping);    
                """ % {
                    'module_name': self.element.parent.pyllars_scope,
                    'name': self.element.name
                }
            elif isinstance(self.element.parent, parser.RecordTypeDefn):
                add_dict_code = """
                     __pyllars_internal::PythonClassWrapper< %(parent_class_name)s >::addClassMember
                        ("%(name)s", mapping);
                """ % {
                        'parent_class_name': self.element.parent.full_name,
                        'name': self.element.name,
                    }
            else:
                raise Exception("Unknown parent type ro template class: %s" % self.element.parent.__class__)
            scoped.write(("""
                
                template<typename T>
                PyObject* keyFrom( const T& instance){
                    return PyBytes_FromStringAndSize( (const char*)&instance, sizeof(instance) );
                }
                
                static PyObject* mapping = PyDict_New();

                
                %(template_decl)s
                status_t %(qname)s_init(){
                   if (!mapping){
                       return -1;
                   }
                   %(add_dict_code)s
                   return 0;
                }
               
                %(template_decl)s
                int %(qname)s_register( pyllars::Initializer* const init){
                    return Initializer_%(qname)s::initializer.register_init(init);
                }
                
""" % {
                'qname': qualified_name(self.element.basic_name or "anonymous_%s" % self.element.tag),
                'full_class_name': full_class_name,
                'add_dict_code': add_dict_code,
                'full_name_space': self.element.parent.full_name if self.element.parent.full_name != "::" else "",
                'parent_name': qualified_name(
                    self.element.parent.name if (self.element.parent.name and self.element.parent.name != "::")
                    else ""),
                'parent': self.element.parent.name if self.element.parent else "pyllars",
                'template_decl': self.element.template_decl,
                'parent_template_decl': self.element.parent.template_decl if self.element.parent else ""
            }).encode('utf-8'))
