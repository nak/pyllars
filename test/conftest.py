import glob
import os, sys
import shutil
import subprocess

from pyllars.cppparser.generation import Generator
from pyllars.cppparser.generation.base2 import Compiler, Linker
from pyllars.cppparser.parser.clang_filter import ClangFilter
import pytest


TEST_RESOURCES_DIR=os.path.join(os.path.dirname(__file__), "resources")
TEST_LIBS_DIR = os.path.join(os.path.dirname(__file__), "libs")
os.makedirs(TEST_LIBS_DIR, exist_ok=True)

sys.path.insert(0, TEST_LIBS_DIR)


@pytest.fixture(scope='session')
def libpyllars():
    home_path = os.path.dirname(os.path.dirname(__file__))
    cmd = ["cmake", "--build", "."]
    p = subprocess.Popen(cmd, stderr=subprocess.PIPE, stdout=subprocess.PIPE, cwd=home_path)
    rc = p.wait()
    if rc != 0:
        out, err = p.communicate()
        raise Exception("Failed to build: \n%s\n%s" % (out, err))
    lib_path = os.path.join(home_path, "src", "resources", "libs")
    gen_path = os.path.join(home_path, "cmake-build-release", "src", "resources", "libpyllars.so")
    if not os.path.exists(gen_path):
        raise Exception("Failed to generate release libpyllars.so in %s" % gen_path)
    os.makedirs(lib_path, exist_ok=True)
    shutil.copy(gen_path, lib_path)


@pytest.fixture(scope='session')
def linker_flags(libpyllars):
    files = glob.glob(os.path.join(TEST_RESOURCES_DIR, "*.cpp"))
    compiler = Compiler(compiler_flags=["-I%s" % TEST_RESOURCES_DIR, "-I."], debug=True)
    objects = []
    for file in files:
        objects.append(compiler.compile(file))
    linker = Linker(compiler_flags=compiler.compiler_flags, linker_options=[], debug=True)
    linker.link_bare(objects, output_lib_path=os.path.join(TEST_LIBS_DIR, "libtestresources.so"))
    linker_flags = ["-L", TEST_LIBS_DIR, "-Wl,-rpath", TEST_LIBS_DIR,  "-ltestresources"]
    return linker_flags


@pytest.fixture(scope='session')
def testglobals(linker_flags):
    compiler = Compiler(compiler_flags=["-I%s" % TEST_RESOURCES_DIR, "-I."],
                        optimization_level="-O0",
                        debug=True,
                        output_dir="generated")
    src_paths = [os.path.join(TEST_RESOURCES_DIR, filename) for filename in ("globals.hpp", "opaque_types.hpp")]
    objects = []
    for src_path in src_paths:
        nodes = ClangFilter.parse(src_path=src_path, flags=compiler.compiler_flags)
        objects += Generator.generate_code(nodes, src_paths=[src_path], output_dir="generated", compiler=compiler)

    linker = Linker(linker_options=linker_flags, compiler_flags=compiler.compiler_flags,
                    debug=True)
    Generator.link(objects, module_name="testglobals", linker=linker, module_location=TEST_LIBS_DIR)

    import testglobals
    return testglobals
