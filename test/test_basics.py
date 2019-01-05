import os, sys

import pytest

from pyllars.cppparser.generation import Generator
from pyllars.cppparser.generation.base2 import Compiler, Linker
from pyllars.cppparser.parser.clang_filter import ClangFilter

TEST_RESOURCES_DIR = os.path.join(os.path.dirname(__file__), "resources")
TEST_LIBS_DIR = os.path.join(os.path.dirname(__file__), "libs")


sys.path.insert(0, TEST_LIBS_DIR)


class TestBasics:

    def test_globals(self, linker_flags):
        compiler = Compiler(compiler_flags=["-I%s" % TEST_RESOURCES_DIR, "-I."],
                            optimization_level="-O0",
                            debug=True,
                            output_dir="generated")
        src_path = os.path.join(TEST_RESOURCES_DIR, "globals.hpp")
        nodes = ClangFilter.parse(src_path=src_path, flags=compiler.compiler_flags)

        linker = Linker(linker_options=linker_flags, compiler_flags=compiler.compiler_flags,
                        debug=True)

        Generator.generate_code(nodes, src_path=src_path, module_name="testglobals", output_dir="generated",
                                compiler=compiler, linker=linker, module_location=TEST_LIBS_DIR)
        sys.stderr.write("HERE1\n")
        import testglobals
        sys.stderr.write("HERE2\n")
        testinstance = testglobals.scoped.TestClass()
        sys.stderr.write("HERE3\n")
        assert abs(4.2 - testinstance.method(1)) < 0.00001

        testinstance.InnerTestClass()
        from pyllars import c_char, c_short, c_int, c_long, c_long_long
        sys.stderr.write("HERE4\n")
        i = c_char(11)
        sys.stderr.write("HERE5\n")
        i += c_char(1)
        sys.stderr.write("HERE6\n")
        assert i == c_char(12)
        sys.stderr.write("HERE7\n")
        assert i == 12
        sys.stderr.write("HERE8\n")
        j = i + c_char(22)
        sys.stderr.write("HERE9\n")
        assert j == c_char(34)
        sys.stderr.write("HERE10\n")
        with pytest.raises(ValueError):
            i + c_char(120)
