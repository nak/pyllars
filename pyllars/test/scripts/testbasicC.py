#!/usr/bin/env python
import sys, os.path
[os.path.join(os.path.abspath(os.path.dirname(os.path.dirname(__file__))),"..","build","lib.linux-x86_64-2.7")]+sys.path
print "### %s"%sys.path
import test_pyllars
import pytest


class Test_BasicC:

    def testGlobalData(self):
        t = test_pyllars.TestStruct()
        t.new()
        return
        assert( test_pyllars.data1() == 1)
        assert( test_pyllars.data2() == 2)
        assert( test_pyllars.data3() == 3)

        self.assertRaises( RuntimeError, test_pyllars.data1, value=2)
        self.assertRaises( RuntimeError, test_pyllars.data2, value=23)
        test_pyllars.data3( value = 43)
        assert( test_pyllars.data3() == 43)

    #def testGlobalFunction(self):
    #    stringcopy = test_pyllars.copy_string("MAKE A COPY");
    #    assert( stringcopy == "MAKE A COPY")

    @staticmethod
    @pytest.mark.parameterized("test_struct, msg", [(test_pyllars.TestStruct(), None),
                                                    (test_pyllars.TestStruct("My_Message"), "My Message"),
                                                    (test_pyllars.InheritedStruct(), None)])
    def testTestStructMembers(test_struct, msg):
        assert( abs(test_struct.double_member() - 1.23456789) <=  0.000000000000001)
        t.double_member(set_to=9.87654321)
        assert( abs(test_struct.double_member() - 9.87654321) <=  0.000000000000001)
        assert(type(test_struct.str_member())== type(msg if msg else ""))
        assert(test_struct.str_member() == (msg or "Default constructed  TestStruct"))
        with pytest.raises(RuntimeError):
            test_struct.str_member(set_to="New value")

    @staticmethod
    @pytest.mark.parameterized("test_struct_array", [test_pyllars.TestStruct.new([(), ("Second Instance",)]),
                                                     test_pyllars.InheritedStruct.new([(), ()])])
    def testTestStructAlloc(test_struct_array):
        assert(test_struct_array[0].str_member() == "Default constructed  TestStruct")
        assert(test_struct_array[1].str_member() =="Second Instance")
        assert(test_struct_array[-2].str_member() =="Default constructed  TestStruct")
        assert(test_struct_array[-1].str_member() == "Second Instance")
        with pytest.raises(IndexError):
            t[2]
        with pytest.raises(IndexError):
            t[-3]
        taddr = test_struct_array.this()

        for i in range(100):
           assert(taddr[0][0].str_member() == "Default constructed  TestStruct")
        return
        tptr = taddr[0]
        titem = tptr[0]
        del taddr
        assert( tptr[0].str_member() == "Default constructed  TestStruct")
        del titem
        del test_struct_array
        assert( tptr[0].str_member() == "Default constructed  TestStruct")
        titem = tptr[0]
        del tptr
        assert( titem.str_member() == "Default constructed  TestStruct")

    @staticmethod
    @pytest.mark.parameterized("test_struct_array", [test_pyllars.TestStruct.new([(), ("Second Instance",)]),
                                                 test_pyllars.InheritedStruct.new([(), ()])])
    def testArrayElements(self, test_struct_array):
        titemcopy = test_struct_array.at(1)
        titemref = test_struct_array[1]
        for i in range(100):print titemcopy.double_member()==2.3456789

        assert( abs(titemref.double_member() - 2.3456789) <= 0.000000000000001)
        assert( abs(titemcopy.double_member()- 2.3456789) <= 0.000000000000001)
        # set copy, and see that ref has not changed
        titemcopy.double_member(set_to=-5.4321)
        assert( abs(titemcopy.double_member() - (-5.4321)) <= 0.000000000000001)
        assert( abs(titemref.double_member() - 2.3456789) <=  0.000000000000001)
        # set ref to new value and see that copy has not changed
        titemref.double_member( set_to=99.99999493)
        assert( abs(titemcopy.double_member() - (-5.4321)) <= 0.000000000000001)
        assert( abs(titemref.double_member() - 99.99999493) <= 0.000000000000001)


    def testAddressAndDereference(self):
        t = test_pyllars.TestStruct("TakeMyAddress")
        taddr = t.this()
        # del t and see that C object is still arround since
        # taddr holds reference to t
        del(t)
        assert(taddr[0].str_member() ==  "TakeMyAddress")

    def testBitFields(self):
        b = test_pyllars.BitFields()
        assert(b.bitfield1_unsigned_size1() == 0)
        assert(b.bitfield3_anon_union_size4() == 15)
        assert(b.const_bitfield2_signed_size3() == -1)
        with pytest.raises(ValueError):
            b.const_bitfield2_signed_size3(set_to=1)
        assert(b.bitfield3_anon_union_size4() == 15)
        assert(b.bitfield_deeep_inner_anonymous() == 0x7FFF)
        with pytest.raises(ValueError):
            b.bitfield_deeep_inner_anonymous(set_to=0xFFFFFFFF00000000)
        assert(b.entry()._field() == -22)
        assert(b.entry()._field2() == 22)
        assert(b._subfields().bitfield4_named_field_size7() == 0x7F)
        b._subfields(as_ref=True).bitfield4_named_field_size7(set_to=0x3A)
        assert(b._subfields().bitfield4_named_field_size7() == 0x3A)

    @staticmethod
    @pytest.mark.parameterized("test_struct, msg", [(test_pyllars.TestStruct(), None),
                                                    (test_pyllars.TestStruct("My_Message"), "My Message"),
                                                    (test_pyllars.InheritedStruct(), None)])
    def testMapOperator(self, test_struct, msg):
        assert(test_struct[0.123] == "0.123000")

    def testVarArgFunction(self):
        s = test_pyllars.function_var_args(2.3, 1, int(2), long(12345), 2.1234892, "last", test_pyllars.var_arg_param_func)
        assert(s == "2 12345 2.123489 last 1902")

    def testVarArgFunctionVoidReturn(self):
        var = test_pyllars.TestStruct()
        test_pyllars.function_var_args_void_return(2.3, 1, int(2), long(12345), 2.1234892, "last", var);

    def testClassMethodVarArg(self):
        inst = test_pyllars.InheritedStruct()
        res = inst.method_with_varargs(22, 96)
        assert(res == 22*96)

    def testClassMethodVarArgVoidReturn(self):
        test_pyllars.InheritedStruct.method_with_varargs_with_void_return(1, int(2), long(12345), 2.1234892, "last")

    def testInheritance(self):
        assert(test_pyllars.InheritedStruct().inherited_value() == 214)

    def testTemplates(self):
        IntClass = test_pyllars.TemplatedClass((int, 'int'), 641)
        assert(IntClass.templated_type_element() == 641)