//
// Created by jrusnak on 3/24/19.
//

#include "setup_inheritance_test.h"
#include "pyllars/pyllars.hpp"
#include "class_test_defns.h"
#include "pyllars/pyllars_classwrapper.hpp"

TEST_F(SetupInheritanceTest, TestInheritance){
    using namespace __pyllars_internal;

    PyObject* args = PyTuple_New(0);
    PyObject* obj = PyObject_Call((PyObject*) PythonClassWrapper<InheritanceClass>::getPyType(),
                                  args, nullptr);
    ASSERT_NE(obj, nullptr);
    auto dbl_ptr = (PythonClassWrapper<const double* const>*) PyObject_GetAttrString(obj, dbl_ptr_member_name);
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
    PyObject* dbl = PyObject_Call((PyObject*)PythonClassWrapper<double>::getPyType(), dargs, nullptr);
    ASSERT_NE(dbl, nullptr);
    PyTuple_SetItem(dargs, 0, dbl);
    PyObject* intValue = PyObject_Call(public_method, dargs, nullptr);
    ASSERT_NE(intValue, nullptr);
}



TEST_F(SetupInheritanceTest, TestMultipleInheritance){
    using namespace __pyllars_internal;
    typedef PythonClassWrapper<BasicClass> Class;

    PyObject* args = PyTuple_New(0);
    auto* obj = PyObject_Call((PyObject*) PythonClassWrapper<MultiInheritanceClass>::getPyType(),
                              args, nullptr);
    ASSERT_NE(obj, nullptr);
    auto dbl_ptr = (PythonClassWrapper<const double* const>*) PyObject_GetAttrString(obj, dbl_ptr_member_name);
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
    PyObject* dbl = PyObject_Call((PyObject*)PythonClassWrapper<double>::getPyType(), dargs, nullptr);
    ASSERT_NE(dbl, nullptr);
    PyTuple_SetItem(dargs, 0, dbl);
    PyObject* intValue = PyObject_Call(public_method, dargs, nullptr);
    ASSERT_NE(intValue, nullptr);

    PyObject* createBaseClass2_method = PyObject_GetAttrString(obj, create_bclass2_method_name);
    ASSERT_NE(createBaseClass2_method, nullptr);
    PyObject* bclass2 = PyObject_Call(createBaseClass2_method, PyTuple_New(0), nullptr);
    ASSERT_NE(bclass2, nullptr);
    ASSERT_TRUE(PyObject_TypeCheck(bclass2, PythonClassWrapper<BasicClass2>::getPyType()));
}
