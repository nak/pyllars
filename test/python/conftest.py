import asyncio
import glob
import os, sys
import platform
import shutil
import subprocess

import pkg_resources

import pytest


TEST_RESOURCES_DIR=os.path.join(os.path.dirname(__file__), "resources")
TEST_LIBS_DIR = os.path.join(os.path.dirname(__file__), "libs")
os.makedirs(TEST_LIBS_DIR, exist_ok=True)

sys.path.insert(0, TEST_LIBS_DIR)

PYLLARS_RESOURCE_DIR=pkg_resources.resource_filename("pyllars", "resources")
PYLLAR_LIBS_DIR=os.path.join(PYLLARS_RESOURCE_DIR, "lib")


@pytest.fixture(scope='session')
def libpyllars():
    home_path = os.path.join(os.path.dirname(__file__), "..", "..")
    cmd = ["cmake", "."]
    p = subprocess.Popen(cmd, stderr=subprocess.PIPE, stdout=subprocess.PIPE, cwd=home_path)
    rc = p.wait()
    if rc != 0:
        out, err = p.communicate()
        raise Exception("Failed to cmake-configure: \n%s\n%s" % (out.decode('utf-8'), err.decode('utf-8')))

    async def build_pyllars():
        p = await asyncio.subprocess.create_subprocess_exec("make", "pyllars", stdout=subprocess.PIPE,
                                                            stderr=subprocess.PIPE, cwd=home_path)
        line = True
        while line:
            line = await p.stdout.readline()
            print(line.strip().decode('utf-8'))
        if await p.wait() != 0:
            out, err = await p.communicate()
            raise Exception("Failed to build pyllars library: \n%s\n%s" % (out.decode('utf-8'), err.decode('utf-8')))

    asyncio.get_event_loop().run_until_complete(build_pyllars())
    if platform.system() == 'Windows':
        lib_path = os.path.join(PYLLAR_LIBS_DIR, "libpyllars.dll")
    elif platform.system() == 'Darwin':
        lib_path = os.path.join(PYLLAR_LIBS_DIR, "libpyllars.dylib")
    else:
        lib_path = os.path.join(PYLLAR_LIBS_DIR, "libpyllars.so")
    if not os.path.exists(lib_path):
        raise Exception("Failed to generate release libpyllars.so in %s" % lib_path)


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

