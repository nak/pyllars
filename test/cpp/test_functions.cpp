#include <stdlib.h>
#include "gtest/gtest.h"
#include "setup.h"
#include "pyllars/pyllars.hpp"
#include "pyllars/internal/pyllars_function_wrapper.hpp"
#include "pyllars/internal/pyllars_staticfunctionsemantics.impl.hpp"
#include <map>
#include "class_test_defns.h"
#include "setup.h"
#include "setup_functions.hpp"
#include "function_defns.hpp"
#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_integer.hpp"

namespace{
    const char* const kwlist_void_return[] = {"_CObject", nullptr};
    const char* const kwlist_void_return_va[] = {"_CObject", "spacer", nullptr};
    const char* const kwlist_long_long_return[] = {"i", "d", nullptr};
}

TEST_F(SetupFunctions, TestVoidReturn){
    using namespace pyllars_internal;
    typedef PythonFunctionWrapper<void(double&)> Wrapper;
    PyObject* obj = (PyObject*) Wrapper::template createPy<kwlist_void_return, void_return>("void_return");
    double dvalue = 98.6;
    auto args = PyTuple_New(1);
    auto value =  toPyObject<double&>(dvalue, 1);
    PyTuple_SetItem(args, 0, value);
    auto ret = PyObject_Call(obj, args, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_TRUE(ret == Py_None);
    double retval = PyFloat_AsDouble(value);
    ASSERT_FLOAT_EQ(retval, 2.5*98.6); // as implemented in function we are wrapping
    Py_DECREF(obj);
}


TEST_F(SetupFunctions, TestLongLongReturn){
    using namespace pyllars_internal;
    typedef PythonFunctionWrapper<long long(const int, double*)> Wrapper;
    PyObject* obj = (PyObject*) Wrapper::template createPy<kwlist_long_long_return, long_long_return>("long_long_return");
    double dv = 98.6;
    int iv = 12;
    auto args = PyTuple_New(2);
    auto ivalue = toPyObject(iv,1);
    auto dvalue =  toPyObject<double*>(&dv, 1);
    PyTuple_SetItem(args, 0, ivalue);
    PyTuple_SetItem(args, 1, dvalue);
    auto ret = PyObject_Call(obj, args, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_TRUE(PyObject_TypeCheck(ret, PythonClassWrapper<long long>::getPyType()));
    long long retval = PyLong_AsLongLong(ret);
    ASSERT_EQ(retval, 42); // as implemented in function we are wrapping
    Py_DECREF(obj);
}


TEST_F(SetupFunctions, TestVoidReturnVarArgs){
    using namespace pyllars_internal;
    typedef PythonFunctionWrapper<void(double&, int, ...)> Wrapper;
    PyObject* obj = (PyObject*) Wrapper::template createPy<kwlist_void_return_va, void_return_varargs>("void_return_varargs");
    double dvalue = 98.6;
    double dvalue2 = 12335.43;
    auto args = PyTuple_New(3);
    auto value =  toPyObject<double&>(dvalue, 1);
    auto value2 =  toPyObject<double>(dvalue2, 1);
    PyTuple_SetItem(args, 0, value);
    PyTuple_SetItem(args, 1, PyLong_FromLong(12));
    PyTuple_SetItem(args, 2, value2);
    auto ret = PyObject_Call(obj, args, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_TRUE(ret == Py_None);
    double retval = PyFloat_AsDouble(value);
    ASSERT_FLOAT_EQ(retval, dvalue2); // as implemented in function we are wrapping
    Py_DECREF(obj);
}


TEST_F(SetupFunctions, TestLongLongReturnVarArgs){
    using namespace pyllars_internal;
    typedef PythonFunctionWrapper<long long(const int, double*, ...)> Wrapper;
    PyObject* obj = (PyObject*) Wrapper::template createPy<kwlist_long_long_return, long_long_return_varargs>("long_long_return_varargs");
    double dv = 98.6;
    double dv2 = -98821.82;
    int iv = 12;
    auto args = PyTuple_New(3);
    auto ivalue = toPyObject(iv,1);
    auto dvalue =  toPyObject<double*>(&dv, 1);
    auto dvalue2 =  toPyObject<double>(dv2, 1);
    PyTuple_SetItem(args, 0, ivalue);
    PyTuple_SetItem(args, 1, dvalue);
    PyTuple_SetItem(args, 2, dvalue2);
    auto ret = PyObject_Call(obj, args, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_TRUE(PyObject_TypeCheck(ret, PythonClassWrapper<long long>::getPyType()));
    long long retval = PyLong_AsLongLong(ret);
    ASSERT_EQ(retval, -iv); // as implemented in function we are wrapping
    ASSERT_FLOAT_EQ(dv, dv2); // as implemented in function we are wrapping
    Py_DECREF(obj);
}
