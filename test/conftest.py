import glob
import os
import subprocess

import pytest

from pyllars.cppparser.generation.base2 import Compiler, Linker

TEST_RESOURCES_DIR=os.path.join(os.path.dirname(__file__), "resources")
TEST_LIBS_DIR = os.path.join(os.path.dirname(__file__), "libs")
os.makedirs(TEST_LIBS_DIR, exist_ok=True)


@pytest.fixture(scope='session')
def linker_flags():
    files = glob.glob(os.path.join(TEST_RESOURCES_DIR, "*.cpp"))
    compiler = Compiler(compiler_flags=["-I%s" % TEST_RESOURCES_DIR, "-I."])
    objects = []
    for file in files:
        objects.append(compiler.compile(file))
    linker = Linker(compiler_flags=compiler.compiler_flags, linker_options=[], debug=True)
    linker.link_bare(objects, output_lib_path=os.path.join(TEST_LIBS_DIR, "libtestresources.so"))
    linker_flags = ["-L", TEST_LIBS_DIR, "-Wl,-rpath", TEST_LIBS_DIR,  "-ltestresources"]
    return linker_flags


