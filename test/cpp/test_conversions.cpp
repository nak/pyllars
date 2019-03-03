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


class PythonSetup: public ::testing::Test{
protected:
    void SetUp() override{
        PyErr_Clear();
    }

    void TearDown() override{
        ASSERT_FALSE(PyErr_Occurred());
    }

    static void SetUpTestSuite() {
        Py_Initialize();
    }



    static void TearDownTestSuite(){
        PyErr_Clear();
    }

};

enum {ONE, TWO, THREE};


template<>
const char* const __pyllars_internal::_Types<A>::type_name = "A";

template<>
const char* const __pyllars_internal::_Types<decltype(ONE)>::type_name = "anon_enum";

template<typename T>
void test_basic(T val, PyObject* (*PyFrom)(T)){
    using namespace __pyllars_internal;
    PyObject* obj;
    obj = PyFrom(val);
    smart_ptr<T, false> value = toCArgument<T, false>(*obj);
    if (std::is_floating_point<T>::value){
        if(sizeof(T) == 4) {
            ASSERT_FLOAT_EQ(*value, val);
        } else {
            ASSERT_DOUBLE_EQ(*value, val);
        }
    } else {
        ASSERT_EQ(*value, val);
    }
}

template<typename T>
PyObject* PyLong_FromInt(T v){
    return PyLong_FromLong(v);
}

PyObject* PyFloat_FromFloat(float v){
    return PyFloat_FromDouble(v);
}

TEST_F(PythonSetup, convert_basic_char){
    test_basic((char)123, PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_short){
    test_basic((short)123, PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_int){
    test_basic((int)123, PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_long){
    test_basic((long)123,  PyLong_FromLong);
}

TEST_F(PythonSetup, convert_basic_long_long){
    test_basic((long long)123,  PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_uchar){
    test_basic((unsigned char)123, PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_ushort){
    test_basic((unsigned short)123, PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_uint){
    test_basic((unsigned int)123, PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_ulong){
    test_basic((unsigned long)123,  PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_ulong_long){
    test_basic((unsigned long long)123,  PyLong_FromInt);
}


TEST_F(PythonSetup, convert_basic_double){
    test_basic(1.2349, PyFloat_FromDouble);
}

TEST_F(PythonSetup, convert_basic_float){
    test_basic(1.2349f, PyFloat_FromFloat);
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


TEST_F(PythonSetup, convert_array_double) {
    double vals[3] = {1,2,3};
    test_conversion(vals);
}

TEST_F(PythonSetup, convert_array_enum) {
    decltype(ONE) vals[3] = {ONE, TWO, THREE};
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

template<typename T>
T __PyLong_AsInt(PyObject* obj){
    long v = PyLong_AsLong(obj);
    return (T)v;
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


TEST_F(PythonSetup, convert_from_native_py_ulong) {
    unsigned long vals[3] = {1,2,3};
    unsigned long toVals[3] = {999, 341, 783};
    test_conversion_from_native_py<unsigned long, array_call<unsigned long>, PyLong_FromInt, __PyLong_AsInt>(vals, toVals);
}

PyObject* _PyUnicode_FromString(const char* data){
    return PyUnicode_DecodeASCII(data, strlen(data), nullptr);
}

TEST_F(PythonSetup, convert_from_native_py_cstring) {
    const char*  vals[3] = {"abc", "def", "ghi"};
    const char*  toVals[3] = {"rst", "uvw", "xyz"};
    test_conversion_from_native_py_strcmp<const char* , array_call<const char* >,
            _PyUnicode_FromString, PyUnicode_AsUTF8>(vals, toVals);
}
const char* __PyBytes_ToString(PyObject* obj) {
    return (const char *) PyBytes_AsString(obj);
}

TEST_F(PythonSetup, convert_from_native_pybytes_cstring) {
    const char*  vals[3] = {"abc", "def", "ghi"};
    const char*  toVals[3] = {"rst", "uvw", "xyz"};
    test_conversion_from_native_py_strcmp<const char* , array_call<const char* >,
            PyBytes_FromString, __PyBytes_ToString>(vals, toVals);
}


TEST_F(PythonSetup, convert_to_py) {
    using namespace __pyllars_internal;
    PythonClassWrapper<A>::initialize();
    A a;
    auto obj = toPyObject<A>(a, true, 1);
    ASSERT_NE(obj, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_EQ(&a, reinterpret_cast<PythonClassWrapper<A>*>(obj)->get_CObject());
}
