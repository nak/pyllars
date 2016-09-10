#!/usr/bin/env python
import sys, os.path
#####sys.path=
[os.path.join(os.path.abspath(os.path.dirname(os.path.dirname(__file__))),"..","build","lib.linux-x86_64-2.7")]+sys.path
print "### %s"%sys.path
import test_pyllars
import unittest

class Test_BasicC(unittest.TestCase):


    def testGlobalData(self):
        t = test_pyllars.TestStruct()
        t.new()
        return
        self.assertEqual( test_pyllars.data1(), 1)
        self.assertEqual( test_pyllars.data2(), 2)
        self.assertEqual( test_pyllars.data3(), 3)

        self.assertRaises( RuntimeError, test_pyllars.data1, value=2)
        self.assertRaises( RuntimeError, test_pyllars.data2, value=23)
        test_pyllars.data3( value = 43)
        self.assertEqual( test_pyllars.data3(), 43)

    #def testGlobalFunction(self):
    #    stringcopy = test_pyllars.copy_string("MAKE A COPY");
    #    self.assertEqual( stringcopy, "MAKE A COPY")

    def testTestStructMemberDouble(self):
        t = test_pyllars.TestStruct()
        self.assertEqual(t.str_member(), "Default constructed  TestStruct")
        self.assertAlmostEqual( t.double_member(), 1.23456789,delta=  0.000000000000001)
        t.double_member(set_to=9.87654321)
        self.assertAlmostEqual( t.double_member(), 9.87654321,delta=  0.000000000000001)

    def testTestStructMemberCString(self):
        t = test_pyllars.TestStruct("My_Message")
        self.assertEqual(type(t.str_member()), type("My_Message"))
        self.assertEqual(t.str_member(), "My_Message")
        self.assertRaises(RuntimeError, t.str_member,set_to="New value")

    def testTestStructAlloc(self):
        t = test_pyllars.TestStruct.new([(), ("Second Instance",)])
        self.assertEqual(t[0].str_member(), "Default constructed  TestStruct")
        self.assertEqual(t[1].str_member(), "Second Instance")
        self.assertEqual(t[-2].str_member(), "Default constructed  TestStruct")
        self.assertEqual(t[-1].str_member(), "Second Instance")
        try:
            t[2]
            self.assertTrue( false, "Failed to raise index error")
        except IndexError:
            pass
        try:
            t[-3]
            self.assertTrue( false, "Failed to raise index error")
        except IndexError:
            pass
        taddr = t.this()

        for i in range(100):
           self.assertEqual(taddr[0][0].str_member(), "Default constructed  TestStruct")
        return
        tptr = taddr[0]
        titem = tptr[0]
        del taddr
        self.assertEqual( tptr[0].str_member(), "Default constructed  TestStruct")
        del titem
        del t
        self.assertEqual( tptr[0].str_member(), "Default constructed  TestStruct")
        titem = tptr[0]
        del tptr
        self.assertEqual( titem.str_member(), "Default constructed  TestStruct")

    def testArrayElements( self):
        t = test_pyllars.TestStruct.new([(), ("Second Instance",)])
        titemcopy = t.at(1)
        titemref = t[1]
        for i in range(100):print titemcopy.double_member()==2.3456789

        self.assertAlmostEqual( titemref.double_member(), 2.3456789, delta=0.000000000000001)
        self.assertAlmostEqual( titemcopy.double_member(), 2.3456789, delta= 0.000000000000001)
        # set copy, and see that ref has not changed
        titemcopy.double_member(set_to=-5.4321)
        self.assertAlmostEqual( titemcopy.double_member(),-5.4321, delta= 0.000000000000001)
        self.assertAlmostEqual( titemref.double_member(),2.3456789,delta=  0.000000000000001)
        # set ref to new value and see that copy has not changed
        titemref.double_member( set_to=99.99999493)
        self.assertAlmostEqual( titemcopy.double_member(),-5.4321,delta=  0.000000000000001)
        self.assertAlmostEqual( titemref.double_member(),99.99999493, delta= 0.000000000000001)


    def testAddressAndDereference(self):
        t = test_pyllars.TestStruct("TakeMyAddress")
        taddr = t.this()
        # del t and see that C object is still arround since
        # taddr holds reference to t
        del(t)
        self.assertEqual(taddr[0].str_member(), "TakeMyAddress")

    def testBitFields(self):
        b = test_pyllars.BitFields()
        self.assertEqual(b.bitfield1_unsigned_size1(), 0)
        self.assertEqual(b.bitfield3_anon_union_size4(), 15)
        self.assertEqual(b.const_bitfield2_signed_size3(), -1)
        try:
            b.const_bitfield2_signed_size3(set_to=1)
            self.assertTrue(False)
        except SyntaxError:
            self.assertEqual(b.const_bitfield2_signed_size3(), -1)
        self.assertEqual(b.bitfield3_anon_union_size4(), 15)
        self.assertEqual(b.bitfield_deeep_inner_anonymous(), 0x7FFF)
        try:
            b.bitfield_deeep_inner_anonymous(set_to=0xFFFFFFFF00000000)
            self.assertTrue(False, "Value error should have been raised")
        except ValueError:
            self.assertEqual(b.bitfield_deeep_inner_anonymous(), 0x7FFF)
        self.assertEqual(b.entry()._field(), -22)
        self.assertEqual(b.entry()._field2(), 22)
        self.assertEqual(b._subfields().bitfield4_named_field_size7(), 0x7F)
        b._subfields(as_ref=True).bitfield4_named_field_size7(set_to=0x3A)
        self.assertEqual(b._subfields().bitfield4_named_field_size7(), 0x3A)

    def testMapOperator(self):
        t = test_pyllars.TestStruct("TestMapOperatorMethod")
        self.assertEqual(t[0.123], "0.123")

if __name__ == "__main__":
    unittest.main()
