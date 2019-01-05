import os, sys

import pytest

from pyllars.cppparser.generation import Generator
from pyllars.cppparser.generation.base2 import Compiler, Linker
from pyllars.cppparser.parser.clang_filter import ClangFilter

TEST_RESOURCES_DIR = os.path.join(os.path.dirname(__file__), "resources")
TEST_LIBS_DIR = os.path.join(os.path.dirname(__file__), "libs")


sys.path.insert(0, TEST_LIBS_DIR)


@pytest.fixture(scope='session')
def testglobals(linker_flags):
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
    import testglobals
    return testglobals


@pytest.fixture(params=['c_char', 'c_short', 'c_int', 'c_long', 'c_long_long'])
def c_signed_type_and_size(testglobals, request):
    # CUATION: These are known to be good only for Linux 64-bit systems
    sizes = {'c_char': 8,
             'c_short': 16,
             'c_int': 32,
             'c_long': 64,
             'c_long_long': 64,
             }
    import pyllars
    return getattr(pyllars, request.param), sizes[request.param]


class TestBasics:

    def test_basic_class(self, testglobals):
        testinstance = testglobals.scoped.TestClass()
        assert abs(4.2 - testinstance.method(1)) < 0.00001
        assert testinstance.InnerTestClass() is not None

    def test_signed_add(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        max = (1<<(size-1)) - 1
        min = -(1<<(size-1))
        assert c_type(1) + c_type(max//2) == c_type((max//2) + 1)
        assert c_type(-12) + c_type(-(max//2)) == c_type(-(max//2) - 12)
        with pytest.raises(ValueError):
            c_type(max) + c_type(1)
        with pytest.raises(ValueError):
            c_type(min) + c_type(-2)
        import pyllars
        if c_type not in [pyllars.c_long, pyllars.c_long_long]:
            assert c_type(max) + max == 2*max  # convert to Python int: no bounds

    def test_signed_inplace_add(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        min = c_type(-(1<<(size-1)))
        i = c_type(12)
        i += c_type(30)
        assert i == c_type(42)
        assert i == 42  # compare to normal Python int
        j = c_type(-121)
        j += -2
        assert j == -123
        with pytest.raises(ValueError):
            j = c_type(-121)
            j += min

    def test_signed_subtract(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        max = c_type((1<<(size-1)) - 1)
        min = c_type(-(1<<(size-1)))
        assert c_type(1) - c_type(max//2) == c_type(-(max//2) + 1)
        assert c_type(-12) - c_type(-(max//2)) == c_type((max//2) - 12)
        with pytest.raises(ValueError):
            assert c_type(-max) - c_type(2)
        with pytest.raises(ValueError):
            assert c_type(min) - c_type(1)
        import pyllars
        if c_type not in [pyllars.c_long, pyllars.c_long_long]:
            assert c_type(max) - (1<<(size-1)) == max - (1<<(size-1))  # convert to Python int: no bounds

    def test_signed_inplace_subtract(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        max = c_type((1<<(size-1)) - 1)
        i = c_type(-12)
        i -= c_type(30)
        assert i == c_type(-42)
        assert i == -42  # compare to normal Python int
        j = c_type(121)
        j -= -2
        assert j == 123
        with pytest.raises(ValueError):
            j = c_type(-121)
            j -= max

    def test_signed_multiply(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        max = c_type((1 << (size - 1)) - 1)
        min = c_type(-(1 << (size - 1)))
        assert c_type(3) * c_type(12) == c_type(36)
        assert c_type(-3) * c_type(max//3) == -((max//3))*3
        with pytest.raises(ValueError):
            c_type(32) * c_type(max//2)
        with pytest.raises(ValueError):
            c_type(32) * c_type(min//2)

    def test_signed_inplace_multiply(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        min = c_type(-(1 << (size - 1)))
        i = c_type(12)
        i *= c_type(10)
        assert i == 120
        j = c_type(min)
        with pytest.raises(ValueError):
            j *= i
        assert j == c_type(min)

    def test_signed_divide(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        assert c_type(12)/c_type(3) == 4.0
        assert c_type(13)/c_type(2) == 6.5
        assert c_type(15)/c_type(-2) == -7.5

    def test_signed_inplace_divide(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        i = c_type(120)
        i /= 3
        assert i == 40.0

    def test_signed_floordiv(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        assert c_type(121)//c_type(2) == c_type(60)
        assert c_type(-121)//c_type(3) == c_type(-41)
        assert c_type(-120)//c_type(3) == c_type(-40)

    def test_signed_inplace_floordiv(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        i = c_type(121)
        i //= c_type(2)
        assert i == c_type(60)
        i //= c_type(7)
        assert i == 8

    def test_signed_remainder(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        max = c_type((1 << (size - 1)) - 1)
        assert max % c_type(2) == c_type(1)
        assert c_type(8) % 3 == 2
        assert c_type(-8) % 3 == 1

    def test_signed_inplace_remainder(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        max = c_type((1 << (size - 1)) - 1)
        i = max
        i %= 2
        assert i == c_type(1)

    def test_signed_power(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        assert pow(c_type(6), c_type(2)) == c_type(36)
        assert pow(c_type(12), c_type(3), c_type(5)) == c_type(3)

    def test_signed_negative(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        max = c_type((1 << (size - 1)) - 1)
        min = c_type(-(1 << (size - 1)))
        assert -max == -(1<<(size-1)) + 1
        with pytest.raises(ValueError):
            -min  # cannot represent positive of min within bounds of c_type
        assert -c_type(12) == c_type(-12)

    def test_signed_negate(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        assert ~c_type(8) == -9

    def test_signed_positive(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        assert +c_type(121) == 121
        i = c_type(12)
        assert +i is i

