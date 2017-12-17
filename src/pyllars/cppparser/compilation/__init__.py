import os
import shlex
import subprocess
import sysconfig
from abc import abstractmethod
from enum import IntEnum
from typing import List
from ..generation import *

import pkg_resources
import pyllars
import tempfile


_pyllars_resources_dir = pkg_resources.resource_filename("pyllars", os.path.join("..", "resources"))


class CodeBase(object):

    def __init__(self, element_type, base_dir: str, linker_flags: List[str]=[], addl_sources: List[str]=[]):
        self._type = element_type
        self._base_dir = base_dir
        self._linker_flags = linker_flags
        self._addl_sources = addl_sources

    @property
    def element_type(self):
        return self._type

    @property
    def base_dir(self):
        return self._base_dir

    @property
    def linker_flags(self):
        return self._linker_flags

    @property
    def addl_sources(self):
        return self._addl_sources


class CompilationModel(object):

    CFLAGS = sysconfig.get_config_var('CFLAGS') or ""
    LDFLAGS = sysconfig.get_config_var('LDFLAGS') or ""
    LDCXXSHARED = sysconfig.get_config_var('LDCXXSHARED') or ""
    PYINCLUDE = sysconfig.get_config_var('INCLUDEPY')
    CXX = sysconfig.get_config_var('CXX')
    PYLIB = sysconfig.get_config_var('BLDLIBRARY')

    class Policy(IntEnum):
        SINGLE_MODULE = 0
        BY_NAEMESPACE = 1

    def __init__(self, code_bases: List[CodeBase], globals_module_name: str):
        output_dir = pkg_resources.resource_filename("pyllars", ".")
        self._globals_module_name = globals_module_name
        self._globals_module = os.path.join(output_dir, globals_module_name + ".so")
        self._code_bases = code_bases
        self._objects = set([])
        self._failed = set([])
        self._compiled_modules = set([])
        self._objs_dir = tempfile.mkdtemp()

    @property
    def globals_module(self):
        return self._globals_module

    @abstractmethod
    def compile_modules(self, 
                        compiler_flags: List[str],
                        linker_flags: List[str],
                        addl_source: List[str]):
        pass

    def compile(self, code_base: CodeBase):
        for root, dirs, files in os.walk(code_base.base_dir):
           for base_name in [f for f in files if f.endswith('.cpp')]:
               file_name = os.path.join(root, base_name)
               self._compile_file(root, file_name, self._objs_dir)

    def _link(self, module_file_name: str, linker_flags: List[str], addl_sources: List[str]):
        if module_file_name in self._compiled_modules:
            raise Exception("Attempted to compile module %s twice" % module_file_name)
        cmd = "%(cxx)s -O -fPIC -std=c++14 %(cxxflags)s -I%(python_include)s -shared " \
              "-o %(output_module_path)s -Wl,--no-undefined %(src)s %(objs)s %(python_lib_name)s " \
              "-Wl,-R,'$ORIGIN' " \
              "-L%(output_module_path)s -lffi %(linker_flags)s %(pyllars_include)s/pyllars/pyllars.cpp" % {
            'cxx': CompilationModel.LDCXXSHARED,
            'src': " ".join(addl_sources),
            'cxxflags': CompilationModel.CFLAGS,
            'output_module_path': module_file_name,
            'objs': " ".join(self._objects),
            'pyllars_include': _pyllars_resources_dir,
            'python_include': CompilationModel.PYINCLUDE,
            'python_lib_name': CompilationModel.PYLIB,
            'linker_flags': " ".join(linker_flags)
        }
        print(cmd)
        p = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        out = p.communicate()[0].decode('utf-8')
        if p.returncode != 0:
            raise Exception("failed to link %s: %s" % (module_file_name, out))
        self._compiled_modules.add(module_file_name)
        self._objects = set({})
        return module_file_name

    def _compile_file(self, folder: str, file_name: str, objs_dir: str):
        obj_file_name = os.path.join(objs_dir, os.path.basename(file_name).replace(".cpp", ".o"))
        cmd = "%(cxx)s -ftemplate-backtrace-limit=0 -O -std=c++14 %(cxxflags)s " \
              "-c -fPIC -I%(local_include)s -I%(python_include)s " \
              "-I%(pyllars_include)s -o \"%(target)s\" \"%(compilable)s\" " % {
                  'cxx': CompilationModel.CXX,
                  'cxxflags': CompilationModel.CFLAGS,
                  'local_include': os.path.join(objs_dir, folder),
                  'pyllars_include': _pyllars_resources_dir,
                  'python_include': CompilationModel.PYINCLUDE,
                  'target': obj_file_name,
                  'compilable': file_name,
              }
        cmd = cmd.replace("-O2", "-O0")
        print(cmd)
        p = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        output = p.communicate()[0].decode('utf-8')
        if p.returncode != 0:
            self._failed.add((p.returncode, cmd + "\n\t%s" % output))
        else:
            self._objects.add("\"%s\"" % obj_file_name)

    @staticmethod
    def create(policy: "CompilationModel.Policy", code_bases: List[CodeBase],
               globals_module_name: str) -> "CompilationModel":
        if policy == CompilationModel.Policy.SINGLE_MODULE:
            return SingleModuleCompilationModel(code_bases, globals_module_name)
        else:
            return ByNamespaceCompilationModel(code_bases, globals_module_name)


class SingleModuleCompilationModel(CompilationModel):

    def compile_modules(self,
                        compiler_flags: List[str],
                        linker_flags: List[str],
                        addl_source: List[str]):
        linker_flags = linker_flags or []
        for code_base in self._code_bases:
            self.compile(code_base)
            linker_flags += code_base.linker_flags
        self._link(module_file_name=self._global_module,
                   linker_flags=linker_flags,
                   addl_sources=addl_source)


class ByNamespaceCompilationModel(CompilationModel):

    def compile_modules(self, 
                        compiler_flags: List[str],
                        linker_flags: List[str],
                        addl_sources: List[str]):
        global_sources = set([])
        linker_flags = linker_flags or []
        for code_base in self._code_bases:
            if isinstance(code_base.element_type, NamespaceDecl):
                self.compile(code_base)
                self._link(module_file_name=code_base.element_type.name,
                           linker_flags=code_base.linker_flags + linker_flags,
                           addl_sources=code_base.addl_sources)
                linker_flags += code_base.linker_flags
            else:
                global_sources.add(code_base)
        for code_base in global_sources:
            self.compile(code_base)
        self._link(module_file_name=self._globals_module_name,
                   linker_flags=linker_flags,
                   addl_sources=addl_sources)



