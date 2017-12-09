import os
import shlex
import subprocess
import sysconfig
from abc import abstractmethod
from enum import IntEnum
from typing import List
from ..generation import *

import pkg_resources

_pyllars_resources_dir = pkg_resources.resource_filename("pyllars", os.path.join("..", "resources"))

class CodeBase(object):

    def __init__(self, element_type, base_dir, dependencies: "List[(str)]"):
        self._type = element_type
        self._base_dir = base_dir
        self._dependencies = dependencies

    @property
    def element_type(self):
        return self._type

    @property
    def base_dir(self):
        return self._base_dir


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

    def __init__self(self, code_base: List[CodeBase], objs_dir: str, output_dir: str, globals_module_name:str):
        self._globals_module = os.path.join(output_dir, globals_module_name + ".so")
        self._objs_dir = objs_dir
        self._code_dir = code_base
        self._objects = set([])
        self._failed = set([])
        self._compiled_modules = set([])

    @property
    def globals_module(self):
        return self._globals_module

    @property
    def objs_dir(self):
        return self._objs_dir

    @property
    def code_dir(self):
        return self._code_dir

    @abstractmethod
    def compile_modules(self)->List[str]:
        pass

    def compile(self):
        for root, dirs, files in os.walk(self.code_dir):
           for base_name in [f for f in files if f.endswith('.cpp')]:
               file_name = os.path.join(root, base_name)
               self._compile_file(root, file_name)

    def _link(self, module_file_name: str, linker_flags: List[str], addl_sources: List[str]):
        if module_file_name in self._compiled_modules:
            raise Exception("Attempted to compile module %s twice" % module_file_name)
        fmt = "%(cxx)s -O -fPIC -std=c++14 %(cxxflags)s -I%(python_include)s -shared " +\
              "-o %(output_module_path)s -Wl,--no-undefined %(src)s %(objs)s %(python_lib_name)s " +\
              "-L%(output_module_path)s -lffi %(linker_flags) %(pyllars_include)s/pyllars/pyllars.cpp"

        cmd = fmt % {'cxx': CompilationModel.LDCXXSHARED,
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
        p = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE,
                             stderr = subprocess.STDOUT)
        p.communicate()[0].decode('utf-8')
        if p.returncode != 0:
            raise Exception("failed to link %s" % module_file_name)
        self._compiled_modules.add(module_file_name)
        self._objects = set({})

    def _compile_file(self, folder: str, file_name: str):
        obj_file_name = os.path.join(self.objs_dir, os.path.basename(file_name).replace(".cpp", ".o"))
        cmd = "%(cxx)s -ftemplate-backtrace-limit=0 -O -std=c++14 %(cxxflags)s -c -fPIC -I%(local_include)s -I%(python_include)s " \
              "-I%(pyllars_include)s -o \"%(target)s\" \"%(compilable)s\"" % {
                  'cxx': CompilationModel.CXX,
                  'cxxflags': CompilationModel.CFLAGS,
                  'local_include': folder,
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
            self._failed.add((p.returncode, cmd))
        self._objects.add("\"%s\"" % obj_file_name)

    @staticmethod
    def create(policy: "CompilationModel.Policy", code_dir: str, objs_dir: str,
               output_dir: str, globals_module_name: str) -> "CompilationModel":
        if policy == CompilationModel.Policy.SINGLE_MODULE:
            return SingleModuleCompilationModel(code_dir, objs_dir, output_dir, globals_module_name)
        else:
            return ByNamespaceCompilationModel(code_dir, objs_dir, output_dir, globals_module_name)


class SingleModuleCompilationModel(CompilationModel):
    pass

class ByNamespaceCompilationModel(CompilationModel):
    pass


