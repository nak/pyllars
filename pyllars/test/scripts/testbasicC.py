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

    def testTestStructMemberDouble(self):
        t = test_pyllars.TestStruct()
        assert(t.str_member() == "Default constructed  TestStruct")
        assert( abs(t.double_member() - 1.23456789) <=  0.000000000000001)
        t.double_member(set_to=9.87654321)
        assert( abs(t.double_member() - 9.87654321) <=  0.000000000000001)

    def testTestStructMemberCString(self):
        t = test_pyllars.TestStruct("My_Message")
        assert(type(t.str_member())== type("My_Message"))
        assert(t.str_member() == "My_Message")
        with pytest.raises(RuntimeError):
            t.str_member(set_to="New value")

    def testTestStructAlloc(self):
        t = test_pyllars.TestStruct.new([(), ("Second Instance",)])
        assert(t[0].str_member() == "Default constructed  TestStruct")
        assert(t[1].str_member() =="Second Instance")
        assert(t[-2].str_member() =="Default constructed  TestStruct")
        assert(t[-1].str_member() == "Second Instance")
        with pytest.raises(IndexError):
            t[2]

        with pytest.raises(IndexError):
            t[-3]

        taddr = t.this()

        for i in range(100):
           assert(taddr[0][0].str_member() == "Default constructed  TestStruct")
        return
        tptr = taddr[0]
        titem = tptr[0]
        del taddr
        assert( tptr[0].str_member() == "Default constructed  TestStruct")
        del titem
        del t
        assert( tptr[0].str_member() == "Default constructed  TestStruct")
        titem = tptr[0]
        del tptr
        assert( titem.str_member() == "Default constructed  TestStruct")

    def testArrayElements( self):
        t = test_pyllars.TestStruct.new([(), ("Second Instance",)])
        titemcopy = t.at(1)
        titemref = t[1]
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

    def testMapOperator(self):
        t = test_pyllars.TestStruct("TestMapOperatorMethod")
        assert(t[0.123] == "0.123000")

    def testVarArgFunction(self):
        s = test_pyllars.function_var_args(2.3, 1, int(2), long(12345), 2.1234892, "last", test_pyllars.var_arg_param_func)
        assert(s == "2 12345 2.123489 last 1902")

    def testVarArgFunctionVoidReturn(self):
        var = test_pyllars.TestStruct()
        test_pyllars.function_var_args_void_return(2.3, 1, int(2), long(12345), 2.1234892, "last", var);

if __name__ == "__main__":
    unittest.main()
