//
// Created by jrusnak on 3/24/19.
//
#include "setup.h"
#include "setup_inheritance_test.h"
#include "class_test_defns.h"
#include "pyllars/pyllars.hpp"
#include "pyllars/pyllars_class.hpp"

TEST_F(SetupInheritanceTest, TestInheritance){

    PyObject* args = PyTuple_New(0);
    PyObject* PyInheritanceClass = PyObject_GetAttrString(pyllars::GlobalNS::module(), "InheritanceClass");
    auto obj = PyObject_Call(PyInheritanceClass, args, nullptr);
    ASSERT_NE(obj, nullptr);
    auto dbl_ptr = PyObject_GetAttrString(obj, dbl_ptr_member_name);
    ASSERT_NE(dbl_ptr, nullptr);
    PyObject* at = PyObject_GetAttrString(dbl_ptr, "at");
    ASSERT_NE(at, nullptr);
    PyObject* at_args = PyTuple_Pack(1, PyLong_FromLong(0));
    PyObject* dbl_value = PyObject_Call(at, at_args, nullptr);
    ASSERT_NE(dbl_value, nullptr);
    ASSERT_NEAR(PyFloat_AsDouble(dbl_value), 2.3, 0.000001);

    PyObject* public_method = PyObject_GetAttrString(obj, method_name);
    ASSERT_NE(public_method, nullptr);
    PyObject* dargs = PyTuple_New(1);
    PyTuple_SetItem(dargs, 0, PyFloat_FromDouble(12.3));
    auto c_double = PyObject_GetAttrString(pyllars::GlobalNS::module(), "c_double");
    PyObject* dbl = PyObject_Call(c_double, dargs, nullptr);
    ASSERT_NE(dbl, nullptr);
    PyTuple_SetItem(dargs, 0, dbl);
    PyObject* intValue = PyObject_Call(public_method, dargs, nullptr);
    ASSERT_NE(intValue, nullptr);
}



TEST_F(SetupInheritanceTest, TestMultipleInheritance){
    auto PyBasicClass = PyObject_GetAttrString(pyllars::GlobalNS::module(), "MultiInheritanceClass");
    ASSERT_NE(PyBasicClass, nullptr);
    ASSERT_TRUE(PyType_Check(PyBasicClass));
    PyObject* args = PyTuple_New(0);
    auto* obj = PyObject_Call(PyBasicClass, args, nullptr);
    ASSERT_NE(obj, nullptr);
    auto dbl_ptr = PyObject_GetAttrString(obj, dbl_ptr_member_name);
    ASSERT_NE(dbl_ptr, nullptr);
    PyObject* at = PyObject_GetAttrString((PyObject*)dbl_ptr, "at");
    ASSERT_NE(at, nullptr);
    PyObject* at_args = PyTuple_New(1);
    PyTuple_SetItem(at_args, 0, PyLong_FromLong(0));
    PyObject* dbl_value = PyObject_Call(at, at_args, nullptr);
    ASSERT_NE(dbl_value, nullptr);
    ASSERT_NEAR(PyFloat_AsDouble(dbl_value), 2.3, 0.000001);

    PyObject* public_method = PyObject_GetAttrString(obj, method_name);
    ASSERT_NE(public_method, nullptr);
    PyObject* dargs = PyTuple_New(1);
    PyTuple_SetItem(dargs, 0, PyFloat_FromDouble(12.3));
    auto c_dbl = PyObject_GetAttrString(pyllars::GlobalNS::module(), "c_double");
    ASSERT_NE(c_dbl, nullptr);
    ASSERT_TRUE(PyType_Check(c_dbl));
    PyObject* dbl = PyObject_Call(c_dbl, dargs, nullptr);
    ASSERT_NE(dbl, nullptr);
    PyTuple_SetItem(dargs, 0, dbl);
    PyObject* intValue = PyObject_Call(public_method, dargs, nullptr);
    ASSERT_NE(intValue, nullptr);

    PyObject* createBaseClass2_method = PyObject_GetAttrString(obj, create_bclass2_method_name);
    ASSERT_NE(createBaseClass2_method, nullptr);
    PyObject* bclass2 = PyObject_Call(createBaseClass2_method, PyTuple_New(0), nullptr);
    ASSERT_NE(bclass2, nullptr);
    auto PyBasicClass2 = PyObject_GetAttrString(pyllars::GlobalNS::module(), "BasicClass2");
    ASSERT_NE(PyBasicClass2, nullptr);
    ASSERT_TRUE(PyType_Check(PyBasicClass2));
    ASSERT_TRUE(PyObject_TypeCheck(bclass2, (PyTypeObject*) PyBasicClass2));
}
