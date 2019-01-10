class TestOpaqueTypes:
    
    def test_opaque_creation(self, testglobals):
        obj = testglobals.opaque_factory()
        assert testglobals.opaque_get_value(obj) == 0
        assert testglobals.opaque_get_float_value(obj.at(0)) == 0.0
    
    def test_opaque_setters(self, testglobals):
        obj = testglobals.opaque_factory()
        testglobals.opaque_set_value(obj, 2383)
        assert testglobals.opaque_get_value(obj) == 2383
        testglobals.opaque_set_float_value(obj.at(0), 983.2122)
        assert abs(testglobals.opaque_get_float_value(obj.at(0)) - 983.2122) < 0.0001
