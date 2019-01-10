import asyncio
import hashlib
import logging
import os
import shlex
import subprocess
import sysconfig
import tempfile
from contextlib import contextmanager
from dataclasses import dataclass
from io import TextIOBase
from typing import List, Type, Optional

import pkg_resources

from abc import ABC, abstractmethod

from pyllars.cppparser.parser import code_structure

pyllars_resources_dir = pkg_resources.resource_filename("pyllars", os.path.join("..", "resources"))

INDENT = b"    "


log = logging.getLogger(__name__)

@dataclass
class CompilationModel:
    sources: List[str]
    compile_flags: str
    linker_flags: str


class Compiler(object):

    CFLAGS = sysconfig.get_config_var('CFLAGS') or ""
    LDFLAGS = sysconfig.get_config_var('LDFLAGS') or ""
    LDCXXSHARED = sysconfig.get_config_var('LDCXXSHARED') or ""
    LDLIBRARY = sysconfig.get_config_var("LDLIBRARY")
    LIBDIR = sysconfig.get_config_var("LIBDIR")
    PYINCLUDE = sysconfig.get_config_var('INCLUDEPY')
    CXX = sysconfig.get_config_var('CXX')
    PYLIB = sysconfig.get_config_var('BLDLIBRARY')

    def __init__(self, compiler_flags: List[str]=[], output_dir="./objects", optimization_level="-O3", debug=False):
        self._output_dir = output_dir
        os.makedirs(output_dir, exist_ok=True)
        self._compiler_flags = compiler_flags
        self._optimization_level = optimization_level
        self._debug = debug

    @property
    def compiler_flags(self):
        return self._compiler_flags

    def compile(self, path: str):
        import uuid
        m = hashlib.md5()
        m.update(("pyllars" + path).encode('utf-8'))
        hex = m.hexdigest()
        uuid = uuid.UUID(hex)
        target =  os.path.join(self._output_dir, os.path.basename(path) + str(uuid) + ".o")
        cmd = "%(cxx)s -ftemplate-backtrace-limit=0 -g -O -std=c++14 %(cxxflags)s -c -fPIC %(includes)s -I%(python_include)s " \
              "-I%(pyllars_include)s -o \"%(target)s\" \"%(compilable)s\"" % {
                  'cxx': Compiler.CXX,
                  'cxxflags': Compiler.CFLAGS,
                  'includes': " ".join(self._compiler_flags),
                  'pyllars_include': pyllars_resources_dir,
                  'python_include': Compiler.PYINCLUDE,
                  'target': target,
                  'compilable': path,
              }
        cmd = cmd.replace("-O2", self._optimization_level)
        cmd = cmd.replace("-O3", self._optimization_level)
        if not self._debug:
            cmd = cmd.replace("-g", "")
        elif " -g" not in cmd:
            cmd += " -g"
        print(cmd)
        p = subprocess.run(shlex.split(cmd), stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding='utf-8')
        if p.returncode != 0:
            print(p.stdout)
            print(p.stderr)
        assert os.path.exists(target)
        return target

    async def compile_async(self, path: str, asynchronous=False):
        import uuid
        m = hashlib.md5()
        m.update(("pyllars" + path).encode('utf-8'))
        hex = m.hexdigest()
        uuid = uuid.UUID(hex)
        target =  os.path.join(self._output_dir, os.path.basename(path) + str(uuid) + ".o")
        cmd = "%(cxx)s -ftemplate-backtrace-limit=0 -g -O -std=c++14 %(cxxflags)s -c -fPIC %(includes)s -I%(python_include)s " \
              "-I%(pyllars_include)s -o \"%(target)s\" \"%(compilable)s\"" % {
                  'cxx': Compiler.CXX,
                  'cxxflags': Compiler.CFLAGS,
                  'includes': " ".join(self._compiler_flags),
                  'pyllars_include': pyllars_resources_dir,
                  'python_include': Compiler.PYINCLUDE,
                  'target': target,
                  'compilable': path,
              }
        cmd = cmd.replace("-O2", self._optimization_level)
        cmd = cmd.replace("-O3", self._optimization_level)
        if not self._debug:
            cmd = cmd.replace("-g", "")
        elif " -g" not in cmd:
            cmd += " -g"
        print(cmd)
        p = await asyncio.subprocess.create_subprocess_exec(*shlex.split(cmd),
                                                            stdout=subprocess.PIPE,
                                                            stderr=subprocess.PIPE, encoding='utf-8')
        returncode = await p.wait()
        if returncode != 0:
            print(p.stdout)
            print(p.stderr)
        assert os.path.exists(target)
        return target


class Linker:

    CODE = b"""
#include <pyllars/pyllars.hpp>
#include <string.h>
extern "C"{
#if PY_MAJOR_VERSION == 3

    PyObject* PyInit_%(name)s(){
        static const char* const name = "%(name)s";
        return PyllarsInit(name);
    }
#else
    PyMODINIT_FUNC
    init%(name)s{){
        static const char* const name = "%(name)s";
        return PyllarsInit(name);
    }
    
#endif
}
"""

    def __init__(self, compiler_flags: List[str], linker_options: List[str], debug=True):
        self._compiler_flags = compiler_flags
        self._link_flags = linker_options
        self._debug = debug
        self._optimization_level = "-O0"

    def link(self, objects, output_module_path, module_name: str, global_module_name: Optional[str] = None):
        code = self.CODE % {b'name': (module_name or "pyllars").encode('utf-8')}
        compiler = Compiler(compiler_flags=self._compiler_flags + ["-I%s" % pyllars_resources_dir],
                            output_dir=output_module_path, optimization_level=self._optimization_level,
                            debug=self._debug)
        pyllars_obj = compiler.compile(os.path.join(pyllars_resources_dir, "pyllars", "pyllars.cpp"))

        with tempfile.NamedTemporaryFile(mode='wb', suffix='.cpp') as f:
            f.write(code)
            f.flush()
            cmd = "%(cxx)s -shared -O -fPIC -std=c++14 %(cxxflags)s -I%(python_include)s -shared -o %(output_module_path)s -Wl,--no-undefined " \
                  "%(objs)s %(python_lib_name)s %(linker_flags)s -Wl,-R,'$ORIGIN' -lpthread -lffi %(codefile)s -I%(pyllars_include)s" % {
                      'cxx': Compiler.LDCXXSHARED,
                      'cxxflags': Compiler.CFLAGS + " " + " ".join(self._compiler_flags),
                      'linker_flags': " ".join(self._link_flags),
                      'output_module_path': os.path.join(output_module_path, "%s.so" % module_name),
                      'objs': "%s " % pyllars_obj + " ".join(["\"%s\"" % o for o in objects]),
                      'pyllars_include': pyllars_resources_dir,
                      'python_include': Compiler.PYINCLUDE,
                      'codefile': f.name,
                      'python_lib_name': " %s/%s" % (Compiler.LIBDIR, Compiler.LDLIBRARY),
                  }
            cmd = cmd.replace("-O2", self._optimization_level)
            cmd = cmd.replace("-O3", self._optimization_level)
            if not self._debug:
                cmd = cmd.replace("-g", "")
            elif " -g" not in cmd:
                cmd += " -g"
            print(cmd)
            p = subprocess.run(shlex.split(cmd), stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding='utf-8')
            if p.returncode != 0:
                print(p.stdout)
                print(p.stderr)
                raise Exception("Failed to link module")

    def link_bare(self, objects, output_lib_path):
            cmd = "%(cxx)s -shared -O -fPIC -std=c++14 %(cxxflags)s -shared -o %(output_lib_path)s -Wl,--no-undefined " \
                  "%(objs)s %(linker_flags)s -Wl,-R,'$ORIGIN' -lpthread -lffi" % {
                      'cxx': Compiler.LDCXXSHARED,
                      'cxxflags': Compiler.CFLAGS + " " + " ".join(self._compiler_flags),
                      'linker_flags': " ".join(self._link_flags),
                      'output_lib_path': output_lib_path,
                      'objs': " ".join(["\"%s\"" % o for o in objects]),
                  }
            cmd = cmd.replace("-O2", self._optimization_level)
            cmd = cmd.replace("-O3", self._optimization_level)
            if not self._debug:
                cmd = cmd.replace("-g", "")
            elif " -g" not in cmd:
                cmd += " -g"
            print(cmd)
            p = subprocess.run(shlex.split(cmd), stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding='utf-8')
            if p.returncode != 0:
                print(p.stdout)
                print(p.stderr)
                raise Exception("Failed to link module")


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


class BaseGenerator(ABC):


    def decorate(self, text):
        """
        Output text with decorations (as implemented by inherited subclasses potentially).  For example, for
        template object, would prefix with "template <...>"
        :param text: text to be decorated
        """
        if self._parent:
            return self._parent.decorate(text)
        return text  # default is no decoration

    @classmethod
    def is_generatable(cls):
        """
        :return: whether this class can generate code or not
        """
        return True

    @contextmanager
    def _scoped(self, stream, indent=INDENT):
        try:
            if self._parent:
                indent = indent + INDENT
                with self._parent._scoped(stream, indent=indent):
                    if self._element.is_scoping:
                        stream.write(b"%snamespace %s{\n" % (indent, self._element_name.encode('utf-8')))
                    yield stream
            else:
                stream.write(b"namespace pyllars{\n")
                indent = indent + INDENT
                if self._element.is_scoping:
                    stream.write(b"%snamespace %s{\n" % (indent, self._element_name.encode('utf-8')))
                yield stream
        finally:
            if self._element.is_scoping:
                stream.write(b"%s}\n" % indent)
            if not self._parent:
                stream.write(b"%s} // end pyllars\n" % indent)


class GeneratorBody(BaseGenerator):

    COMMON_BODY_INCLUDES = \
        b"""
        #include "%(my_header_file_name)s"
        #include <pyllars/pyllars.hpp>
        #include <pyllars/pyllars_globalmembersemantics.impl>
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
                        if (%(pyllars_scope)s::Initializer::root->init(%(basic_name)s_mod) != 0){
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
                    if (!pyllars_mod){
                        return -1;
                    }
                    PyModule_AddObject(pyllars_mod, "%(basic_name)s", %(basic_name)s_mod);
                    try{
                        return %(pyllars_scope)s::Initializer::root?pyllars::Initializer::root->init(%(basic_name)s_mod):0;
                    } catch (const char* msg ){
                        PyErr_SetString(PyExc_RuntimeError, msg);
                        return -2;
                    }
                }
            #endif
        }
        """

    INITIALIZER_CODE = """
                class Initializer_%(name)s: public pyllars::Initializer{
                public:
                    Initializer_%(name)s():pyllars::Initializer(){
                        %(parent_name)s_register(this);                          
                    }

                    virtual int init(PyObject * const global_mod){
                       int status = pyllars::Initializer::init(global_mod);
                       return status == 0?%(name)s_init(global_mod):status;
                    }
                    
                    static Initializer_%(name)s* initializer;
                    
                    static Initializer_%(name)s *singleton(){
                        static  Initializer_%(name)s _initializer;
                        return &_initializer;
                    }
                 };
                 
                """

    INITIALIZER_INSTANTIATION_CODE = """
                //ensure instance is created on global static initialization, otherwise this
                //element would never be reigstered and picked up
                Initializer_%(name)s * Initializer_%(name)s::initializer = singleton();
    """

    REGISTRATION_CODE = """
                status_t %(name)s_register( pyllars::Initializer* const init ){ 
                    // DO NOT RELY SOLEY ON global static initializatin as order is not guaranteed, so 
                    // initializer initializer var here:
                    
                    return Initializer_%(name)s::singleton()->register_init(init);
                }"""

    @staticmethod
    def _get_generator_class(element: code_structure.Element) -> Type["GeneratorBody"]:
        from . import _get_generator_body_class
        generator_class = _get_generator_body_class(element)
        return generator_class

    @staticmethod
    def generator(element: code_structure.Element, src_path: str, folder: Folder,
                 parent: Optional["GeneratorHeader"]):
        clazz = GeneratorBody._get_generator_class(element)
        return clazz(element, src_path, folder, parent)

    def __init__(self, element: code_structure.Element, src_path: str, folder: Folder, parent: "GeneratorBody"):
        self._parent = parent
        self._src_path = src_path
        self._element = element
        self._element_name = self._element.name or "anonymous_%s" % self._element.tag
        self._folder = folder.create_subfolder(self._element_name) if not parent else \
            parent._folder.create_subfolder(self._element_name)
        self._header_file_name = (element.name or "global").replace(" ", "_") + ".hpp"
        self._header_file_path = os.path.join(self._folder.path, self._header_file_name)
        self._body_file_path = self._header_file_path[:-4] + ".cpp"
        self._stream = None

    def __enter__(self):
        if self.is_generatable():
            self._stream = open(self._body_file_path, 'w+b')
            self.write_include_directives()
            if self._element.is_typename and self._element.name:
                self._stream.write(("""
                   template<>            
                   const char* const __pyllars_internal::Types<%(full_name)s>::type_name(){
                       static const char* const name = "%(full_name)s";
                       return name;
                   }
                """ % {
                    'full_name': self._element.full_name,
                }).encode('utf-8'))
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        if self._stream:
            self._stream.close()

    @property
    def body_file_path(self):
        return self._body_file_path

    def write_include_directives(self):
        self._stream.write(self.COMMON_BODY_INCLUDES % {b'my_header_file_name': self._header_file_path.encode('utf-8')})

    def generate(self):
        """
        Generate code to this element's stream for body implementation of spec
        """
        pass


class GeneratorHeader(BaseGenerator):

    MODE_FULL_HEADER = 0
    MODE_SEPARATE_HEADERS = 1
    MODE_EMPTY_HEADER = 2

    COMMON_HEADER_INCLUDES = \
        b"""
        #include <Python.h>
        #include <pyllars/pyllars.hpp>
        #include <pyllars/pyllars_function_wrapper.hpp>
        
        #include <vector>
        #include <cstddef>
        #include <%(src_path)s>
        %(parent_header_name)s

        #include <pyllars/pyllars_classwrapper.impl>
        """

    @staticmethod
    def _get_generator_class(element: code_structure.Element) -> Type["GeneratorHeader"]:
        from . import _get_generator_header_class
        generator_class = _get_generator_header_class(element)
        return generator_class

    @classmethod
    def sanitize(cls, text):
        map = {'-': '_minus_',
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

    def __init__(self, element: code_structure.Element, src_path: str, folder: Folder,
                 parent: Optional["GeneratorHeader"]):
        self._parent = parent
        self._src_path = src_path
        self._element = element
        self._element_name = self._element.name or "anonymous_%s" % self._element.tag
        self._folder = folder.create_subfolder(self._element_name) if not parent else \
            parent._folder.create_subfolder(self._element_name)
        self._header_file_name = (element.name or "global").replace(" ", "_") + ".hpp"
        self._header_file_path = os.path.join(self._folder.path, self._header_file_name)
        self._stream = None

    def __enter__(self):
        if self.is_generatable():
            self._stream = open(self._header_file_path, 'w+b')
            self._stream.write(("""
#ifndef __%(guard)s__
#define __%(guard)s__
    
                      """ % {'guard': self._element.guard}).encode('utf-8'))
            self.output_include_directives()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        if self._stream:
            self._stream.write(b"""
#endif
""")
            self._stream.close()

    @property
    def folder(self):
        return self._folder

    def output_include_directives(self):
        """
        Write include directives at top of file.  Method allows children to inherit and expand on this if needed
        """
        parent_header_name = "#include \"%s\"\n" % self._parent._header_file_path if self._parent else ""
        self._stream.write(self.COMMON_HEADER_INCLUDES % {b"parent_header_name": parent_header_name.encode('utf-8'),
                                                          b'src_path': self._src_path.encode('utf-8')})

    @staticmethod
    def generator(element: code_structure.Element, src_path: str, folder: Folder,
                 parent: Optional["GeneratorHeader"]):
        clazz = GeneratorHeader._get_generator_class(element)
        return clazz(element, src_path, folder, parent)

    def _output_function_spec(self, comment: str, spec: str, indent: bytes = INDENT):
        self._stream.write(b"%s/**\n" % indent)
        remainder = comment.strip()
        while remainder.strip():
            line = remainder[:80]
            try:
                leftover, line = line.split(maxsplit=1)
            except:
                leftover = ""
            self._stream.write(b"%s* %s\n" % (indent, line.encode('utf-8')))
            remainder = leftover.strip() + ' ' + remainder[80:].strip()
        self._stream.write(b"%s*/\n" % indent)
        self._stream.write(indent + self.decorate(spec).encode('utf-8'))
        if not spec.endswith(';'):
            self._stream.write(b';')
        self._stream.write(b'\n\n')

    def generate(self):
        with self._scoped(self._stream):
            self.generate_spec()

    def generate_spec(self):
        if self._element.is_anonymous_type:  # anonymous directly inaccessible type
            self._stream.write(b"")
            return
        self._output_function_spec(comment="static initializer method to register initialization routine for initialization "
                                   "on dynamic load of library",
                                   spec="status_t %s_register( pyllars::Initializer* const);" % self._element_name,
                                   indent=b"                ")
        self._output_function_spec(comment="called back on initialization to initialize Python wrapper for this C construct "
                                   "@param global_mod:  mod to which the wrapper Python object should belong",
                                   spec="status_t %s_init(PyObject * const global_mod);" % self._element_name,
                                   indent=b"                ")

