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

PYLLARS_RESOURCES_DIR = pkg_resources.resource_filename("pyllars", "resources")
PYLLARS_LIBS_DIR = os.path.join(PYLLARS_RESOURCES_DIR, "lib")
PYLLARS_INCLUDE_DIR = os.path.join(PYLLARS_RESOURCES_DIR, "include")

INDENT = b"    "

logging.basicConfig(level=logging.DEBUG)

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
        includes = " ".join(self._compiler_flags)
        cmd = f"{Compiler.CXX} -ftemplate-backtrace-limit=0 -g -O -std=c++1z {Compiler.CFLAGS} -c -fPIC {includes} -I{PYLLARS_INCLUDE_DIR} " + \
              f"-I{Compiler.PYINCLUDE} -o \"{target}\" \"{path}\""
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
        includes = " ".join(self._compiler_flags)
        cmd = f"{Compiler.CXX} -ftemplate-backtrace-limit=0 -g -O -std=c++1z {Compiler.CFLAGS} -c -fPIC {includes} -I{PYLLARS_INCLUDE_DIR} " + \
              f"-I{Compiler.PYINCLUDE} -o \"{target}\" \"{path}\""
        cmd = cmd.replace("-O2", self._optimization_level)
        cmd = cmd.replace("-O3", self._optimization_level)
        if not self._debug:
            cmd = cmd.replace("-g", "")
        elif " -g" not in cmd:
            cmd += " -g"
        print(cmd)
        p = await asyncio.subprocess.create_subprocess_exec(*shlex.split(cmd),
                                                            stdout=asyncio.subprocess.PIPE,
                                                            stderr=asyncio.subprocess.PIPE,
                                                            bufsize=0)
        async def read(output):
            line = True
            while line:
                line = await output.readline()
                print(line.decode('utf-8').strip())

        await asyncio.gather(read(p.stdout), read(p.stderr))

        returncode = await p.wait()
        if returncode != 0:
            stdout = await p.stdout.read()
            stderr = await p.stderr.read()
            print(stdout.decode('utf-8'))
            print(stderr.decode('utf-8'))
        else:
            print("DONE %s" % cmd)
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

        with tempfile.NamedTemporaryFile(mode='wb', suffix='.cpp') as f:
            f.write(code)
            f.flush()
            cmd = "%(cxx)s -shared -O2 -fPIC -std=c++1z %(cxxflags)s -I%(python_include)s -shared -o %(objfile)s  %(codefile)s -I%(pyllars_include)s" % {
                      'cxx': Compiler.LDCXXSHARED,
                      'cxxflags': Compiler.CFLAGS + " " + " ".join(self._compiler_flags),
                      'pyllars_include': PYLLARS_INCLUDE_DIR,
                      'python_include': Compiler.PYINCLUDE,
                      'codefile': f.name,
                      'objfile': f.name + ".o"
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
                raise Exception("Failed to build common code file %s" % f.name)
            objs = " ".join(["\"%s\"" % o for o in objects])
            cxx = Compiler.LDCXXSHARED
            cxxflags = Compiler.CFLAGS + " " + " ".join(self._compiler_flags)
            linker_flags = " ".join(self._link_flags +["-L", PYLLARS_LIBS_DIR, "-Wl,-rpath", PYLLARS_LIBS_DIR, "-lpyllars"])
            python_lib_name = os.path.join(Compiler.LIBDIR, Compiler.LDLIBRARY)

            cmd2 = f"{cxx} -fPIC -std=c++1z {cxxflags} -I{Compiler.PYINCLUDE} -o {output_module_path}/{module_name}.so "\
                  f"{objs} {python_lib_name} {linker_flags} -Wl,--no-undefined -Wl,-R,'$ORIGIN' -lpthread -lffi -I{PYLLARS_INCLUDE_DIR} {f.name}"
            cmd2 = cmd2.replace("-O2", self._optimization_level)
            cmd2 = cmd2.replace("-O3", self._optimization_level)
            print(cmd2)
            p = subprocess.run(shlex.split(cmd2), stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding='utf-8')
            if p.returncode != 0:
                print(p.stdout)
                print(p.stderr)
                raise Exception("Failed to link module\n %s" % p.stderr)

    def link_bare(self, objects, output_lib_path):
        cmd = "%(cxx)s -shared -O -fPIC -std=c++14 %(cxxflags)s -shared -o %(output_lib_path)s -Wl,--no-undefined " \
                  "%(objs)s %(linker_flags)s -Wl,-R,'$ORIGIN' -lpthread -lffi" % {
                      'cxx': Compiler.LDCXXSHARED,
                      'cxxflags': Compiler.CFLAGS + " " + " ".join(self._compiler_flags),
                      'linker_flags': " ".join(self._link_flags +["-L", PYLLARS_LIBS_DIR, "-Wl,-rpath", PYLLARS_LIBS_DIR,
                                                                  "-lpyllars"]),
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
