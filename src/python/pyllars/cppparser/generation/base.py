import asyncio
import collections
import filecmp
import logging
import multiprocessing
import os
import shlex
import shutil
import subprocess
import sysconfig
from abc import ABCMeta, ABC
from contextlib import contextmanager
from dataclasses import dataclass
from io import TextIOBase
from typing import List, Optional

from pyllars.cppparser.generation.base2 import GeneratorBody, Linker, Compiler
from .. import parser
import pkg_resources
from ..parser import code_structure
from contextlib import suppress

pyllars_resources_dir = pkg_resources.resource_filename("pyllars", os.path.join("..", "resources"))

INDENT = "    "


log = logging.getLogger(__name__)

@dataclass
class CompilationModel:
    sources: List[str]
    compile_flags: str
    linker_flags: str


def qualified_name(name):
    if name is None:
        return "pyllars"
    return Generator.sanitize(name.replace("(", "_lparen_").replace(")", "_rparen_").replace(":", "_")).replace(" ", "_")


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
        return self._path


class Generator(metaclass=ABCMeta):

    MODE_FULL_HEADER = 0
    MODE_SEPARATE_HEADERS = 1
    MODE_EMPTY_HEADER = 2

    COMMON_HEADER_INCLUDES = \
"""
#include <Python.h>
#include <pyllars/pyllars.hpp>
#include <pyllars/pyllars_classwrapper.cpp>
#include <pyllars/pyllars_function_wrapper.hpp>

#include <vector>
#include <cstddef>
#include <%(src_path)s>
%(parent_header_name)s
"""

    COMMON_BODY_INCLUDES = \
"""
#include "%(my_header_name)s"
#include <pyllars/pyllars.hpp>
#include <pyllars/pyllars_globalmembersemantics.cpp>
#include <cstddef>
"""
    INIT_IMPL_CODE = \
"""

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
        PyObject *%(basic_name)s_mod = PyModule_Create(&moduleDef);
        try{
            if (%(pyllars_scope)s::Initializer::root->set_up%() != 0){
                return nullptr;
            } else if (%(pyllars_scope)s::Initializer::root->ready(%(basic_name)s_mod) != 0){
                return nullptr;
            }
           return %(basic_name)s_mod;
        } catch (const char* msg){
           PyErr_SetString(PyExc_RuntimeError, msg);
           return nullptr;
        }
    }

    #else

    PyMODINIT_FUNC
    init%(basic_name)s(){
        PyObject *%(basic_name)s_mod = Py_InitModule3(name, nullptr, doc);
        if(!%(basic_name)s_mod) { return -1;}
        PyObject *pyllars_mod = PyImport_ImportModule("pyllars");
        if (!pyllars_mod){
            return -1;
        }
        PyModule_AddObject(pyllars_mod, "%(basic_name)s", %(basic_name)s_mod);
        try{
            status_t status = %(pyllars_scope)s::Initializer::root?pyllars::Initializer::root->set_up():0;
            status |= %(pyllars_scope)s::Initializer::root?pyllars::Initializer::root->ready(%(basic_name)s_mod):0;
            return status;
        } catch (const char* msg ){
            PyErr_SetString(PyExc_RuntimeError, msg);
            return -2;
        }
    }
    #endif
}
"""

    generator_mapping = {}

    def __init__(self, element: code_structure.Element, src_path: str, folder: Folder, parent: "Generator"):
        self._parent = parent
        self._src_path = src_path
        self._element = element
        folder_name = element.name or "anonymous-%s" % element.tag
        self._folder = folder.create_subfolder(folder_name)
        Generator.generator_mapping[element] = self
        self._children = {}
        for child in element.children():
            child_generator_class = self.get_generator_class(child)
            # store for future reference:
            self._children[child.tag] = child_generator_class(child, src_path=src_path, folder=self.folder, parent=self)

    @classmethod
    def is_generatable(cls):
        return False

    def header_file_path(self) -> str:
        return os.path.join(self.folder.path, self.header_file_name())

    def header_file_name(self)->str:
        return (self.element.name or "global").replace(" ", "_") + ".hpp"

    def body_file_path(self) -> str:
        return os.path.join(self.folder.path, self.body_file_name())

    def body_file_name(self)->str:
        return (self.element.name or "global").replace(" ", "_") + ".cpp"

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
    def parent(self):
        return self._parent

    # def to_path(self, ext="") -> str:
    #     name = self.element.basic_name if self.element.name else "global"
    #     return name.replace(" ", "_") + ext

    # def scope(self):
    #     return self.element.scope

    def basic_includes(self):
        return (self.COMMON_HEADER_INCLUDES % {
            'src_path': self._src_path,
            'parent_header_name': "#include \"%s\"" % self.parent.header_file_path() if self.parent else "",
        }).encode('utf-8')

    @contextmanager
    def scoped(self, stream: TextIOBase):
        with self._ns_scope(stream) as stream:
            yield stream

    @contextmanager
    def _ns_scope(self, stream: TextIOBase):
        need_closure = False
        if self.parent:
            # recurse upward
            with self.parent._ns_scope(stream) as scoped:
                if not self.element.is_template_macro and not isinstance(self.element, (code_structure.FieldDecl,
                                                                                        code_structure.CXXMethodDecl,
                                                                                        code_structure.BuiltinType,
                                                                                        code_structure.TypedefDecl)):
                    need_closure = True
                    scoped.write(("\nnamespace %s{\n" % qualified_name(self.element.name)).encode("utf-8"))
                yield stream
        else:
            # at top level
            stream.write(b"\nnamespace pyllars{\n")
            if not self.element.is_template_macro and not isinstance(self.element, (code_structure.FieldDecl,
                                                                                    code_structure.CXXMethodDecl,
                                                                                    code_structure.BuiltinType,
                                                                                    code_structure.TypedefDecl)):
                need_closure = True
                stream.write(("\nnamespace %s{\n" % qualified_name(self.element.name)).encode("utf-8"))
            yield stream
            stream.write(b"\n}")

        if need_closure:
            stream.write(b"\n}")

    # @contextmanager
    # def templated(self, stream: TextIOBase):
    #     yield stream

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

    def generate_header_code(self, stream: TextIOBase) -> None:
        if self.element.is_implicit:
            return
        with self.guarded(stream) as guarded:
            guarded.write(self.basic_includes())
            with self.scoped(guarded) as scoped:
                self.generate_header_core(scoped)

    def generate_header_core_full(self, stream: TextIOBase):
        if self.element.is_implicit:
            return
        self.generate_header_core(stream)
        for child in self.element.children():
            generator = self._children[child.tag]
            if generator.is_generatable():
                generator.generate_header_core_full(stream)

    def generate_header_core(self, stream: TextIOBase, as_top=False):
        if self.element.is_anonymous_type:  # anonymous directly inaccessible type
            stream.write(b"")
            return
        stream.write(("""
                status_t %(basic_name)s_register( pyllars::Initializer* const);
                
                status_t %(basic_name)s_ready(PyObject * const global_mod);

                status_t %(basic_name)s_set_up();
              
            """ % {
                'name': self.element.name,
                'basic_name': self.sanitize(self.element.name or "anonymous-%s" % self.element.tag),
                'parent_basic_name': self.element.parent.name if (self.element.parent and self.element.parent.name) else "pyllars",
                'pyllars_scope': self.element.pyllars_scope,
                'parent_name': self.element.parent.name if self.element.parent else "pyllars",
        }).encode('utf-8'))

    @staticmethod
    def generate_code(node_tree: code_structure.TranslationUnitDecl,
                      src_paths: List[str],
                      output_dir: str,
                      compiler: Compiler):
        from .base2 import GeneratorHeader
        cmakelists = open("CMakeLists.txt", 'w')
        cmakelists.write("""                
cmake_minimum_required(VERSION 3.10)
project(test_objects)
set(GCC_COVERAGE_COMPILE_FLAGS "-g -O0")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)

include_directories(./sources
        /usr/local/include/python3.7m
        ../../src/python/pyllars/resources/include
        ../../src/python/pyllars/resources/include/pyllars)

add_library(objects SHARED 

                """)
        objects = []
        for src_path in src_paths:
            folder = Folder(output_dir)
            Generator.generator_mapping = {}

            queue = collections.deque()
            for element in node_tree.children():
                queue.append((element, folder, None))

            def pop():
                try:
                    return queue.pop()
                except IndexError:
                    return None, None, None

            async def generate_elements():
                nonlocal objects, queue
                element, folder, parent = pop()

                while element is not None:
                    if os.path.abspath(element.location) != os.path.abspath(src_path) or element.is_implicit:
                        element, folder, parent = pop()
                        continue

                    with GeneratorHeader.generator(element=element, src_path=src_path, folder=folder, parent=parent) as header_generator:
                        try:
                            header_generator.generate()

                        except:
                            log.exception("Failed to generate for element %s and its children" % element.name)
                            return
                    from_file = header_generator._header_file_path
                    to_file = os.path.join("sources", from_file)
                    with suppress(Exception):
                        os.makedirs(os.path.join("sources", os.path.dirname(from_file)) )
                    if not os.path.exists(to_file) or not filecmp.cmp(from_file, to_file):
                        shutil.copy(from_file, to_file)

                    with GeneratorBody.generator(element=element, src_path=src_path, folder=folder, parent=parent) as body_generator:
                        try:
                            body_generator.generate()
                        except:
                            import traceback
                            traceback.print_exc()
                            log.exception("Failed to generate body for element %s" % element.name)
                            return
                    from_file = body_generator.body_file_path
                    to_file = os.path.join("sources", from_file)
                    with suppress(Exception):
                        os.makedirs(os.path.join("sources", os.path.dirname(from_file)) )
                    if not os.path.exists(to_file) or not filecmp.cmp(from_file, to_file):
                        shutil.copy(from_file, to_file)
                    cmakelists.write(to_file+ "\n")
                    objects.append(os.path.join("CMakeFiles", "objects.dir", to_file + ".o"))

                    try:
                        pass
                        #obj = await compiler.compile_async(body_generator.body_file_path)
                        #objects.append(obj)

                        for child in element.children():
                            queue.append((child, header_generator.folder, header_generator))
                    except:
                        log.exception("Failed to compile %s" % body_generator.body_file_path)
                    element, folder, parent = pop()
                GeneratorBody.common_stream().flush()

            async def main():
                thread_count = multiprocessing.cpu_count()
                await asyncio.gather(*[generate_elements() for _ in range(thread_count)])
                obj = await compiler.compile_async(GeneratorBody.common_stream().name)
                objects.append(obj)
            asyncio.run(main())
        cmakelists.write(GeneratorBody.common_stream().name + "\n)\n")
        cmakelists.write("""
        
        set_target_properties(objects PROPERTIES LIBRARY_OUTPUT_DIRECTORY ./libs)
        target_link_libraries(objects PUBLIC -Wl,-rpath '$ORIGIN')
        
                        """)
        cmakelists.close()
        p = subprocess.Popen(["cmake", "."])
        if p.wait() != 0:
            raise Exception("Failed to build cmake system!")
        p = subprocess.Popen(["make", "objects", "-k"])
        if p.wait() != 0:
            print("Failed to compile some files: %s" % [o for o in objects if not os.path.exists(o)])

        objects = [o for o in objects if os.path.exists(o)]
        return objects

    @staticmethod
    def link(objects: List[str],
             linker: Linker,
             module_name: str,
             globals_module_name: Optional[str] = None,
             module_location: str = ".",
            ):
        linker.link(set(objects), output_module_path=module_location, module_name=module_name, global_module_name=globals_module_name or "%s_globals" % module_name)


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
                                status_t pyllars_top_set_up();
                                status_t pyllars_top_ready(PyObject* const global_mod);
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
                            'parent_header_name': self.header_file_path(),
                            'target_file_name': self._src_path}).encode("utf-8"))

                    scoped.write(("""
        #include <pyllars/pyllars_classwrapper.hpp>
        
        namespace pyllars{
    
///////////////////////////////

        namespace %(qname)s{
            %(template_decl)s
            status_t %(qname)s_set_up();
           
            %(template_decl)s
            status_t %(qname)s_ready(PyObject * const global_mod);
            
            %(template_decl)s
            class Initializer: public pyllars::Initializer{
            public:
               
               Initializer():pyllars::Initializer(){
                   %(qname)s_register(this);
               }
               
               virtual int set_up(){
                   int status = pyllars::Initializer::set_up();
                   return status | %(qname)s_set_up();               
               }
               
               virtual int ready(PyObject * const global_mod) orverride{
                   int status = pyllars::Initializer::ready(global_mod);
                   return status | %(qname)s_ready(global_mod);
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
                status_t %(qname)s_set_up(){
                   if (!mapping){
                       return -1;
                   }
                   %(add_dict_code)s
                   return 0;
                }
                
                
                %(template_decl)s
                status_t %(qname)s_ready(PyObject* global_mod){
                    return 0;
                }
               
                %(template_decl)s
                static Initializer_%(qname)s *_init(){
                    static Initializer_%(qname)s * _initializer = new Initializer_%(qname)s();
                    Initializer_%(qname)s::initializer = _initializer;
                    return _initializer;
                }
                
                %(template_decl)s
                int %(qname)s_register( pyllars::Initializer* const init){
                    static Initializer_%(qname)s * _initializer = _init();
                    Initializer_%(qname)s::initializer = _initializer;
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

class BuiltInType(Generator):

    def generate_body(self,  src_path: str):
        pass


class TranslationUnitDecl(Generator):

    def generate_body(self,  src_path: str):
        pass
