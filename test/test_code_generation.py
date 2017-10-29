import glob
import os.path
import shlex
import shutil
import subprocess

import pytest
from pyllars.cppparser import parser
from pyllars.cppparser.generation import Generator, Folder, Compiler

RESOURCES_DIR = os.path.join(os.path.dirname(__file__), "resources")
hpp_input_files = glob.glob(os.path.join(RESOURCES_DIR, "*.hpp"))


class TestCodeGen(object):

    @pytest.mark.parametrize("input_file",  hpp_input_files,
                             ids=[os.path.basename(f).replace(".hpp", "") for f in hpp_input_files])
    def test_generation(self, input_file):
        parser.init()
        top = parser.parse_file(input_file)
        gen_path = os.path.join("generated", os.path.basename(input_file))
        gen_path = os.path.splitext(gen_path)[0]
        if os.path.exists(gen_path):
            shutil.rmtree(gen_path)
        os.makedirs(gen_path)
        generator = Generator.get_generator(type(top.top), src_path=input_file, indent="")
        generator.generate_body(top.top, Folder(gen_path), as_top=True)
        rc, output = Compiler(gen_path).compile_all(src_path=input_file)
        assert rc == 0, "Failed to compile/link generated code: \n%s" % output

    def test_template_class_generation(self):
        input_file = os.path.join(RESOURCES_DIR, "template_classes.hpp")
        top = parser.parse_file(input_file)
        gen_path = os.path.join("generated", os.path.basename(input_file))
        gen_path = os.path.splitext(gen_path)[0]
        if os.path.exists(gen_path):
            shutil.rmtree(gen_path)
        os.makedirs(gen_path)
        generator = Generator.get_generator(type(top.top), src_path=input_file, indent="")
        generator.generate_body(top.top, Folder(gen_path), as_top=True)
        rc, output = Compiler(gen_path).compile_all(src_path=input_file)
        assert rc == 0, "Failed to compile/link generated code: \n%s" % output