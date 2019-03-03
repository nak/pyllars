//
// Created by jrusnak on 3/1/19.
//

#include <stdlib.h>
#include <Python.h>
#include "gtest/gtest.h"
#include "pyllars/pyllars_conversions.impl.hpp"
#include "pyllars/pyllars_classwrapper.impl.hpp"
#include "pyllars/pyllars_pointer.impl.hpp"
#include "pyllars/pyllars_membersemantics.impl.hpp"
#include "pyllars/pyllars_classmembersemantics.impl.hpp"
#include "pyllars/pyllars_classmethodsemantics.impl.hpp"
#include "pyllars/pyllars.hpp"

struct A{
    int ivalue;

    A(int v = 100):ivalue(v){

    }

    bool operator==(const A& v) const {
        return ivalue == v.ivalue;
    }
};

template __pyllars_internal::smart_ptr<double, false>
                __pyllars_internal::toCArgument<double, false>(PyObject&);

class PythonSetup: public ::testing::Test{
protected:
    void SetUp() override{

    }

    void TearDown() override{

    }

    static void SetUpTestSuite() {
        Py_Initialize();
    }



    static void TearDownTestSuite(){
        PyErr_Clear();
    }

};

TEST_F(PythonSetup, convert_basic){
    using namespace __pyllars_internal;
    PyObject* obj;
    obj = PyLong_FromLong(123987);
    smart_ptr<long, false> value = toCArgument<long, false>(*obj);
    ASSERT_EQ(*value, 123987);

    obj = PyFloat_FromDouble(1.2345);
    smart_ptr<double, false> dvalue = toCArgument<double, false>(*obj);
    ASSERT_DOUBLE_EQ(1.2345, *dvalue);

}

template<typename T>
void array_call(T v[3], T to[3]){
    v[0] = to[0];
    // skip element 1
    v[2] = to[2];
}

template<typename T>
void test_conversion(T vals[3]){
    using namespace __pyllars_internal;
    T array[3] = {vals[0], vals[1], vals[2]};
    PythonClassWrapper<T[3]>::initialize();
    auto obj = toPyObject<T[3]>(array, true, 3);
    smart_ptr<T[3], false> avalue = toCArgument<T[3], false >(*obj);
    ASSERT_EQ((*avalue)[0], array[0]);
    ASSERT_EQ((*avalue)[1], array[1]);
    ASSERT_EQ((*avalue)[2], array[2]);
}

TEST_F(PythonSetup, convert_array_int) {
    int vals[3] = {1,2,3};
    test_conversion(vals);
}

TEST_F(PythonSetup, convert_array_float) {
    float vals[3] = {1,2,3};
    test_conversion(vals);
}


TEST_F(PythonSetup, convert_array_class) {
    A vals[3] = {A(1),A(12),A(-234)};
    test_conversion(vals);
}


template<typename T, void(*call)(T v[3], T y[3]), PyObject* (*PyFrom)(T), T (*PyTo)(PyObject*)>
void test_conversion_from_native_py(T vals[3], T toVals[3]) {
    using namespace __pyllars_internal;

    auto obj = PyList_New(3);
    PyList_SetItem(obj, 0, PyFrom(vals[0]));
    PyList_SetItem(obj, 1, PyFrom(vals[1]));
    PyList_SetItem(obj, 2, PyFrom(vals[2]));
    {
        call(*toCArgument<T[3], false>(*obj), toVals);
        ASSERT_EQ(PyTo(PyList_GetItem(obj, 0)), toVals[0]);
        ASSERT_EQ(PyTo(PyList_GetItem(obj, 1)), vals[1]);
        ASSERT_EQ(PyTo(PyList_GetItem(obj, 2)), toVals[2]);
    }

}


template<typename T, void(*call)(T v[3], T y[3]), PyObject* (*PyFrom)(T), T (*PyTo)(PyObject*)>
void test_conversion_from_native_py_strcmp(T vals[3], T toVals[3]) {
    using namespace __pyllars_internal;

    auto obj = PyList_New(3);
    PyList_SetItem(obj, 0, PyFrom(vals[0]));
    PyList_SetItem(obj, 1, PyFrom(vals[1]));
    PyList_SetItem(obj, 2, PyFrom(vals[2]));
    {
        call(*toCArgument<T[3], false>(*obj), toVals);
        ASSERT_STREQ(PyTo(PyList_GetItem(obj, 0)), toVals[0]);
        ASSERT_STREQ(PyTo(PyList_GetItem(obj, 1)), vals[1]);
        ASSERT_STREQ(PyTo(PyList_GetItem(obj, 2)), toVals[2]);
    }

}

PyObject* __PyLong_FromInt(int v){
    return PyLong_FromLong(v);
}

int __PyLong_AsInt(PyObject* obj){
    long v = PyLong_AsLong(obj);
    return (int)v;
}


TEST_F(PythonSetup, convert_from_native_py_int) {
    int vals[3] = {1,2,3};
    int toVals[3] = {999, 341, -783};
    test_conversion_from_native_py<int, array_call<int>, __PyLong_FromInt, __PyLong_AsInt>(vals, toVals);
}


TEST_F(PythonSetup, convert_from_native_py_long) {
    long vals[3] = {1,2,3};
    long toVals[3] = {999, 341, -783};
    test_conversion_from_native_py<long, array_call<long>, PyLong_FromLong, PyLong_AsLong>(vals, toVals);
}

const char* __PyUnicode_AsString(PyObject* obj){
   return PyUnicode_AsUTF8(obj);
}

PyObject* _PyUnicode_FromString(const char* data){
    return PyUnicode_DecodeASCII(data, strlen(data), nullptr);
}

TEST_F(PythonSetup, convert_from_native_py_cstring) {
    const char*  vals[3] = {"abc", "def", "ghi"};
    const char*  toVals[3] = {"rst", "uvw", "xyz"};
    test_conversion_from_native_py_strcmp<const char* , array_call<const char* >,
            _PyUnicode_FromString, __PyUnicode_AsString>(vals, toVals);
}

template<>
const char* const __pyllars_internal::_Types<A>::type_name = "A";


TEST_F(PythonSetup, convert_to_py) {
    using namespace __pyllars_internal;
    PythonClassWrapper<A>::initialize();
    A a;
    auto obj = toPyObject<A>(a, true, 1);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_EQ(&a, reinterpret_cast<PythonClassWrapper<A>*>(obj)->get_CObject());
}
