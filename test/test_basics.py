import os, sys

import pytest


sizes = {'c_char': 8,
         'c_short': 16,
         'c_int': 32,
         'c_long': 64,
         'c_long_long': 64,
         'c_unsigned_char': 8,
         'c_unsigned_short': 16,
         'c_unsigned_int': 32,
         'c_unsigned_long': 64,
         'c_unsigned_long_long': 64,
         'const_c_unsigned_char': 8,
         'const_c_unsigned_short': 16,
         'const_c_unsigned_int': 32,
         'const_c_unsigned_long': 64,
         'const_c_unsigned_long_long': 64,
         'const_c_char': 8,
         'const_c_short': 16,
         'const_c_int': 32,
         'const_c_long': 64,
         'const_c_long_long': 64,
         'const_c_float': 32,
         'const_c_double': 64,
         'c_float': 32,
         'c_double': 64,

         }


@pytest.fixture(params=['c_char', 'c_short', 'c_int', 'c_long', 'c_long_long'])
def c_signed_type_and_size(testglobals, request):
    # CUATION: These are known to be good only for Linux 64-bit systems

    import pyllars
    return getattr(pyllars, request.param), sizes[request.param]


@pytest.fixture(params=['const_c_char', 'const_c_short', 'const_c_int', 'const_c_long', 'const_c_long_long'])
def const_c_signed_type_and_size(testglobals, request):
    # CUATION: These are known to be good only for Linux 64-bit systems
    import pyllars
    return getattr(pyllars, request.param), sizes[request.param]


@pytest.fixture(params=['c_char', 'c_short', 'c_int', 'c_long', 'c_long_long', 'const_c_char', 'const_c_short', 'const_c_int', 'const_c_long', 'const_c_long_long'])
def c_all_signed_type_and_size(testglobals, request):
    # CUATION: These are known to be good only for Linux 64-bit systems
    import pyllars
    return getattr(pyllars, request.param), sizes[request.param]


@pytest.fixture(params=['c_unsigned_char', 'c_unsigned_short', 'c_unsigned_int', 'c_unsigned_long', 'c_unsigned_long_long'])
def c_unsigned_type_and_size(testglobals, request):
    # CUATION: These are known to be good only for Linux 64-bit systems
    import pyllars
    return getattr(pyllars, request.param), sizes[request.param]


@pytest.fixture(params=['const_c_unsigned_char', 'const_c_unsigned_short', 'const_c_unsigned_int', 'const_c_unsigned_long', 'const_c_unsigned_long_long'])
def const_c_unsigned_type_and_size(testglobals, request):
    # CUATION: These are known to be good only for Linux 64-bit systems
    import pyllars
    return getattr(pyllars, request.param), sizes[request.param]


@pytest.fixture(params=['c_unsigned_char', 'c_unsigned_short', 'c_unsigned_int', 'c_unsigned_long', 'c_unsigned_long_long',
                        'const_c_unsigned_char', 'const_c_unsigned_short', 'const_c_unsigned_int', 'const_c_unsigned_long', 'const_c_unsigned_long_long'])
def c_all_unsigned_type_and_size(testglobals, request):
    # CUATION: These are known to be good only for Linux 64-bit systems
    import pyllars
    return getattr(pyllars, request.param), sizes[request.param]


@pytest.fixture(params=['const_c_float', 'const_c_double'])
def const_c_float_type_and_size(testglobals, request):
    # CUATION: These are known to be good only for Linux 64-bit systems
    import pyllars
    return getattr(pyllars, request.param), sizes[request.param]


@pytest.fixture(params=['c_float', 'c_double'])
def c_float_type_and_size(testglobals, request):
    # CUATION: These are known to be good only for Linux 64-bit systems
    import pyllars
    return getattr(pyllars, request.param), sizes[request.param]


class TestBasics:

    def test_basic_class(self, testglobals):
        testinstance = testglobals.scoped.TestClass()
        assert abs(4.2 - testinstance.method(1)) < 0.00001
        assert testinstance.InnerTestClass() is not None

    def test_basic_class_members(self, testglobals):
        testinstance = testglobals.scoped.TestClass()
        assert testinstance.long_long_member == 51
        assert testinstance.const_int_member == 123
        assert abs(testglobals.scoped.TestClass.static_const_float_member() - 42.1) < 0.00001
        testinstance.long_long_member = 21234;
        assert testinstance.long_long_member == 21234;
        with pytest.raises(ValueError):
            testinstance.const_int_member = 1
        assert isinstance(testinstance.inner_instance(), testglobals.scoped.TestClass.InnerTestClass)
        assert testglobals.null_long_ptr is not None
        assert testglobals.null_long_ptr2 is not None
        assert abs(testglobals.trial.double_value() - 2.33) < 0.00001

    def test_global_funcs(self, testglobals):
        instance = testglobals.scoped.TestClass.InnerTestClass()
        instance.value = 5.9
        assert testglobals.scoped.scoped_function(instance) == 5
        instance.value = 123983.2
        assert testglobals.scoped.scoped_function(instance) == 123983

    def test_global_vars(self, testglobals):
        assert testglobals.const_ptr_str() == "HELLO WORLD!"

    def test_float_add(self, c_float_type_and_size):
        c_type, size = c_float_type_and_size
        tolerance = 0.00001 if size == 32 else 0.00000001
        assert abs((c_type(1.2) + c_type(123.9)) - c_type(125.1)) < tolerance

    def test_signed_add(self, c_all_signed_type_and_size):
        c_type, size = c_all_signed_type_and_size
        max = (1<<(size-1)) - 1
        min = -(1<<(size-1))
        assert c_type(1) + c_type(max//2) == c_type((max//2) + 1)
        assert c_type(-12) + c_type(-(max//2)) == c_type(-(max//2) - 12)
        with pytest.raises(ValueError):
            c_type(max) + c_type(1)
        with pytest.raises(ValueError):
            c_type(min) + c_type(-2)
        import pyllars
        if size < 64:
            assert c_type(max) + max == 2*max  # convert to Python int: no bounds

    def test_unsigned_add(self, c_all_unsigned_type_and_size):
        c_type, size = c_all_unsigned_type_and_size
        max = (1<<size)-1
        assert c_type(1) + c_type(max//2) == c_type((max//2) + 1)
        assert c_type(12) + c_type(max//2) == c_type((max//2) + 12)
        with pytest.raises(ValueError):
            c_type(max) + c_type(1)
        import pyllars
        if size < 64:
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

    def test_unsigned_inplace_add(self, c_unsigned_type_and_size):
        c_type, size = c_unsigned_type_and_size
        max = c_type((1 << size) - 1)
        i = c_type(12)
        i += c_type(30)
        assert i == c_type(42)
        assert i == 42  # compare to normal Python int
        with pytest.raises(ValueError):
            j = c_type(121)
            j += max

    def test_signed_subtract(self, c_all_signed_type_and_size):
        c_type, size = c_all_signed_type_and_size
        max = c_type((1<<(size-1)) - 1)
        min = c_type(-(1<<(size-1)))
        assert c_type(1) - c_type(max//2) == c_type(-(max//2) + 1)
        assert c_type(-12) - c_type(-(max//2)) == c_type((max//2) - 12)
        with pytest.raises(ValueError):
            assert c_type(-max) - c_type(2)
        with pytest.raises(ValueError):
            assert c_type(min) - c_type(1)
        import pyllars
        if size < 64:
            assert c_type(max) - (1<<(size-1)) == max - (1<<(size-1))  # convert to Python int: no bounds

    def test_unsigned_subtract(self, c_all_unsigned_type_and_size):
        c_type, size = c_all_unsigned_type_and_size
        max = c_type((1<<size) - 1)
        assert c_type(max//2) -  c_type(1)  == c_type((max//2) - 1)
        with pytest.raises(ValueError):
            assert c_type(0) - c_type(1)
        import pyllars
        if size < 64:
            assert c_type(max) - (1<<size) == max - (1<<size)  # convert to Python int: no bounds

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

    def test_unsigned_inplace_subtract(self, c_unsigned_type_and_size):
        c_type, size = c_unsigned_type_and_size
        max = c_type(1<<size-1)
        i = c_type(30)
        i -= c_type(12)
        assert i == c_type(18)
        assert i == 18  # compare to normal Python int
        j = c_type(121)
        j -= 2
        assert j == 119
        with pytest.raises(ValueError):
            j = c_type(121)
            j -= max

    def test_signed_multiply(self, c_all_signed_type_and_size):
        c_type, size = c_all_signed_type_and_size
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

    def test_signed_divide(self, c_all_signed_type_and_size):
        c_type, size = c_all_signed_type_and_size
        assert c_type(12)/c_type(3) == 4.0
        assert c_type(13)/c_type(2) == 6.5
        assert c_type(15)/c_type(-2) == -7.5

    def test_signed_inplace_divide(self, c_signed_type_and_size):
        c_type, size = c_signed_type_and_size
        i = c_type(120)
        i /= 3
        assert i == 40.0

    def test_signed_floordiv(self, c_all_signed_type_and_size):
        c_type, size = c_all_signed_type_and_size
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

    def test_enums(self, testglobals):
        assert testglobals.FIRST == 1
        assert testglobals.SECOND == 2
        assert testglobals.THIRD == 3
        assert testglobals.UnsizedClassEnum.A == 0
        assert testglobals.UnsizedClassEnum.B == 1
        assert testglobals.UnsizedClassEnum.C == 2
        assert testglobals.SizedClassEnum.A == -1
        assert testglobals.SizedClassEnum.B == -2
        assert testglobals.SizedClassEnum.C == -3
        assert testglobals.Enumeration.ZERO == 0
        assert testglobals.Enumeration.ONE == 1
        assert testglobals.Enumeration.TWO == 2
        assert testglobals.Enumeration.THREE == 3

        assert testglobals.scoped.TestClass.INNER_ONE == 1
        assert testglobals.scoped.TestClass.INNER_TWO == 2
        assert testglobals.scoped.TestClass.INNER_THREE == 3