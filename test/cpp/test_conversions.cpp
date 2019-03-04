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



class DisparateType{
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
const char* const __pyllars_internal::_Types<DisparateType>::type_name = "DisparateType";

template<>
const char* const __pyllars_internal::_Types<decltype(ONE)>::type_name = "anon_enum";

template<typename T>
struct Assertion {
    static void assert_equal(const T &v1, const T &v2) {
        ASSERT_EQ(v1, v2);
    }
};

template<>
struct Assertion<float>{
    static void assert_equal(const float &v1, const float &v2) {
        ASSERT_FLOAT_EQ(v1, v2);
    }
};


template<>
struct Assertion<double>{
    static void assert_equal(const double &v1, const double &v2) {
        ASSERT_DOUBLE_EQ(v1, v2);
    }
};

template<>
struct Assertion<const char*>{
    static void assert_equal(const char* const &v1, const char* const &v2) {
        ASSERT_STREQ(v1, v2);
    }
};


template<>
struct Assertion<char*>{
    static void assert_equal( char* const &v1,  char* const &v2) {
        ASSERT_STREQ(v1, v2);
    }
};

TEST_F(PythonSetup, test_basic_convert_pybasic){
    using namespace __pyllars_internal;
    static constexpr long const_long = 123445567678l;
    static constexpr unsigned int const_uint = 212;

    auto long_obj = PyLong_FromLong(const_long);
    auto uint_obj = PyLong_FromLong(const_uint);

    smart_ptr<long, false> long_value = toCArgument<long, false>(*long_obj);
    smart_ptr<unsigned int, false> uint_value = toCArgument<unsigned int, false>(*uint_obj);

    ASSERT_EQ(*long_value, const_long);
    ASSERT_EQ(*uint_value, const_uint);
    auto float_obj = PyFloat_FromDouble(1.234);
    typedef const char* cstring;
    ASSERT_THROW((toCArgument<long, false>(*float_obj)), cstring);
}

TEST_F(PythonSetup, test_pyfloat_to_cfloat){
    using namespace __pyllars_internal;
    static constexpr double const_double = 19823.34432;

    auto double_obj = PyFloat_FromDouble(const_double);

    ASSERT_DOUBLE_EQ((*toCArgument<double, false>(*double_obj)), const_double);
    ASSERT_FLOAT_EQ((*toCArgument<float, false>(*double_obj)), (float)const_double);
}


TEST_F(PythonSetup, test_pylong_to_c){
    using namespace __pyllars_internal;
    static constexpr long const_long = 19823;

    auto long_obj = PyLong_FromLong(const_long);

    ASSERT_EQ((*toCArgument<long, false>(*long_obj)), const_long);
    ASSERT_EQ((*toCArgument<unsigned long long, false>(*long_obj)), (unsigned long long)const_long);
}

template<typename T>
void test_basic(T val, PyObject* (*PyFrom)(T)){
    using namespace __pyllars_internal;
    PyObject* obj;
    obj = PyFrom(val);
    smart_ptr<T, false> value = toCArgument<T, false>(*obj);
    Assertion<T>::assert_equal(*value, val);
    typedef const char* cstring;
    ASSERT_THROW( (toCArgument<DisparateType, false>(*obj)), cstring);

}

template<typename T>
PyObject* PyLong_FromInt(T v){
    return PyLong_FromLong(v);
}

PyObject* PyFloat_FromFloat(float v){
    return PyFloat_FromDouble(v);
}

PyObject* PyClass_FromClass(A v){
    using namespace __pyllars_internal;
    static const char* const kwlist[]= {"obj", nullptr};
    PythonClassWrapper<A>::addConstructor<const A&>(kwlist);
    auto args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, toPyObject(v, true, 1));
    return PyObject_Call((PyObject*)PythonClassWrapper<A>::getPyType(), args, nullptr);
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


TEST_F(PythonSetup, convert_basic_class){
    test_basic(A(99), PyClass_FromClass);
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
    Assertion<T>::assert_equal((*avalue)[0], array[0]);
    Assertion<T>::assert_equal((*avalue)[1], array[1]);
    Assertion<T>::assert_equal((*avalue)[2], array[2]);
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
        ASSERT_NE(PyList_GetItem(obj, 0), nullptr);
        ASSERT_NE(PyList_GetItem(obj, 1), nullptr);
        ASSERT_NE(PyList_GetItem(obj, 2), nullptr);
        Assertion<T>::assert_equal(PyTo(PyList_GetItem(obj, 0)), toVals[0]);
        Assertion<T>::assert_equal(PyTo(PyList_GetItem(obj, 1)), vals[1]);
        Assertion<T>::assert_equal(PyTo(PyList_GetItem(obj, 2)), toVals[2]);
    }

    typedef const char* cstring;
    ASSERT_THROW((toCArgument<DisparateType, false>(*obj)), cstring);
}

PyObject* __PyLong_FromInt(int v){
    return PyLong_FromLong(v);
}

template<typename T>
T __PyLong_AsInt(PyObject* obj){
    long v = PyLong_AsLong(obj);
    return (T)v;
}

template<typename T>
PyObject* PyWrapper_FromValue(T v){
    using namespace __pyllars_internal;
    return toPyObject<T>(v, true, 1);
}

template<typename T>
T PyWrapper_AsValue(PyObject* obj){
    using namespace __pyllars_internal;
    assert(PyObject_TypeCheck(obj, PythonClassWrapper<T>::getPyType()));
    return *((PythonClassWrapper<T>*)obj)->get_CObject();
}

TEST_F(PythonSetup, convert_from_native_py_int) {
    int vals[3] = {1,2,3};
    int toVals[3] = {999, 341, -783};
    test_conversion_from_native_py<int, array_call<int>, __PyLong_FromInt, __PyLong_AsInt>(vals, toVals);
    test_conversion_from_native_py<int, array_call<int>, PyWrapper_FromValue<int>, PyWrapper_AsValue<int> >(vals, toVals);
}


TEST_F(PythonSetup, convert_from_native_py_long) {
    long vals[3] = {1,2,3};
    long toVals[3] = {999, 341, -783};
    test_conversion_from_native_py<long, array_call<long>, PyLong_FromLong, PyLong_AsLong>(vals, toVals);
    test_conversion_from_native_py<long, array_call<long>, PyWrapper_FromValue<long>, PyWrapper_AsValue<long> >(vals, toVals);
}


TEST_F(PythonSetup, convert_from_native_py_ulong) {
    unsigned long vals[3] = {1,2,3};
    unsigned long toVals[3] = {999, 341, 783};
    test_conversion_from_native_py<unsigned long, array_call<unsigned long>, PyLong_FromInt, __PyLong_AsInt>(vals, toVals);
    test_conversion_from_native_py<unsigned long, array_call<unsigned long>, PyWrapper_FromValue<unsigned long>,
            PyWrapper_AsValue<unsigned long> >(vals, toVals);
}

PyObject* _PyUnicode_FromString(const char* data){
    return PyUnicode_DecodeASCII(data, strlen(data), nullptr);
}

TEST_F(PythonSetup, convert_from_native_py_cstring) {
    const char*  vals[3] = {"abc", "def", "ghi"};
    const char*  toVals[3] = {"rst", "uvw", "xyz"};
    test_conversion_from_native_py<const char* , array_call<const char* >,
            _PyUnicode_FromString, PyUnicode_AsUTF8>(vals, toVals);
    test_conversion_from_native_py<const char*, array_call<const char*>,
            PyWrapper_FromValue<const char*>, PyWrapper_AsValue<const char*> >(vals, toVals);

}
const char* __PyBytes_ToString(PyObject* obj) {
    return (const char *) PyBytes_AsString(obj);
}

TEST_F(PythonSetup, convert_from_native_pybytes_cstring) {
    const char*  vals[3] = {"abc", "def", "ghi"};
    const char*  toVals[3] = {"rst", "uvw", "xyz"};
    test_conversion_from_native_py<const char* , array_call<const char* >,
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
