//
// Created by jrusnak on 3/1/19.
//

#include <stdlib.h>
#include <Python.h>
#include "gtest/gtest.h"
#include "setup.h"
#include "pyllars/pyllars.hpp"

#include "pyllars/internal/pyllars_conversions.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"
#include "pyllars/internal/pyllars_pointer.impl.hpp"
#include "pyllars/internal/pyllars_membersemantics.impl.hpp"
#include "pyllars/internal/pyllars_classmembersemantics.impl.hpp"
#include "pyllars/internal/pyllars_staticfunctionsemantics.impl.hpp"

#include "pyllars/pyllars_classmethod.hpp"
#include "pyllars/pyllars_classstaticmethod.hpp"
#include "pyllars/pyllars_classconstructor.hpp"
#include "pyllars/pyllars_enum.hpp"
#include "pyllars/pyllars_classoperator.hpp"
#include "pyllars/pyllars_classstaticmember.hpp"
#include "pyllars/pyllars_classmember.hpp"
#include "pyllars/pyllars_classbitfield.hpp"
#include "pyllars/pyllars_class.hpp"
#include "pyllars/internal/pyllars_classwrapper-type.impl.hpp"
namespace {
    const char* const empty[] = {nullptr};
    const char* const kwlist[]= {"obj", nullptr};
}
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
public:
    static void SetUpTestCase(){
        SetUpTestSuite();
    }

    static void TearDownTestCase(){
        TearDownTestSuite();
    }
protected:
    void SetUp() override{
        PyErr_Clear();
    }

    void TearDown() override{
        ASSERT_FALSE(PyErr_Occurred());
    }

    static void SetUpTestSuite() {
        using namespace pyllars_internal;

        Py_Initialize();
        PythonClassWrapper<A>::addConstructor<kwlist, const A&>();
        PythonClassWrapper<A>::addConstructor<empty>();
    }



    static void TearDownTestSuite(){
        PyErr_Clear();
    }

};

enum {ONE, TWO, THREE};

namespace pyllars_internal {
    template<>
    struct DLLEXPORT TypeInfo<A> {
        static constexpr char type_name[] = "A";
    };
    template<>
    struct DLLEXPORT TypeInfo<DisparateType> {
        static constexpr char type_name[] = "DisparateType";
    };
    template<>
    struct DLLEXPORT TypeInfo<decltype(ONE)> {
        static constexpr char type_name[] = "anon_enum";
    };
}

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

TEST_F(PythonSetup, test_convert_basic_convert_pybasic){
    using namespace pyllars_internal;
    static constexpr long const_long = 123445567678l;
    static constexpr unsigned int const_uint = 212;

    auto long_obj = PyLong_FromLong(const_long);
    auto uint_obj = PyLong_FromLong(const_uint);

    argument_capture<long> long_value = toCArgument<long>(*long_obj);
    argument_capture<unsigned int> uint_value = toCArgument<unsigned int>(*uint_obj);

    ASSERT_EQ(long_value.value(), const_long);
    ASSERT_EQ(uint_value.value(), const_uint);
    auto float_obj = PyFloat_FromDouble(1.234);
    typedef const char* cstring;
    ASSERT_THROW((toCArgument<long>(*float_obj)), PyllarsException);
}

TEST_F(PythonSetup, test_convert_pyfloat_to_cfloat){
    using namespace pyllars_internal;
    static constexpr double const_double = 19823.34432;

    auto double_obj = PyFloat_FromDouble(const_double);

    ASSERT_DOUBLE_EQ((toCArgument<double>(*double_obj).value()), const_double);
    ASSERT_FLOAT_EQ((toCArgument<float>(*double_obj).value()), (float)const_double);
}


TEST_F(PythonSetup, test_convert_pylong_to_c){
    using namespace pyllars_internal;
    static constexpr long const_long = 19823;

    auto long_obj = PyLong_FromLong(const_long);

    ASSERT_EQ((toCArgument<long>(*long_obj).value()), const_long);
    ASSERT_EQ((toCArgument<unsigned long long>(*long_obj).value()), (unsigned long long)const_long);
}

template<typename T>
void test_convert_basic(T val, PyObject* (*PyFrom)(T)){
    using namespace pyllars_internal;
    PyObject* obj;
    obj = PyFrom(val);
    ASSERT_NE(obj, nullptr);
    argument_capture<T> value = toCArgument<T>(*obj);
    Assertion<T>::assert_equal(value.value(), val);
    ASSERT_THROW( (toCArgument<DisparateType>(*obj)), PyllarsException);

}

template<typename T>
PyObject* PyLong_FromInt(T v){
    return PyLong_FromLong((long)v);
}

PyObject* PyFloat_FromFloat(float v){
    return PyFloat_FromDouble(v);
}

PyObject* PyClass_FromClass(A v){
    using namespace pyllars_internal;
    auto args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, toPyObject<A>(v, 1));
    return PyObject_Call((PyObject*)PythonClassWrapper<A>::getPyType(), args, nullptr);
}

TEST_F(PythonSetup, convert_basic_char){
    test_convert_basic((char)123, PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_short){
    test_convert_basic((short)123, PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_int){
    test_convert_basic((int)123, PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_long){
    test_convert_basic((long)123,  PyLong_FromLong);
}

TEST_F(PythonSetup, convert_basic_long_long){
    test_convert_basic((long long)123,  PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_uchar){
    test_convert_basic((unsigned char)123, PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_ushort){
    test_convert_basic((unsigned short)123, PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_uint){
    test_convert_basic((unsigned int)123, PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_ulong){
    test_convert_basic((unsigned long)123,  PyLong_FromInt);
}

TEST_F(PythonSetup, convert_basic_ulong_long){
    test_convert_basic((unsigned long long)123,  PyLong_FromInt);
}


TEST_F(PythonSetup, convert_basic_double){
    test_convert_basic(1.2349, PyFloat_FromDouble);
}

TEST_F(PythonSetup, convert_basic_float){
    test_convert_basic(1.2349f, PyFloat_FromFloat);
}


TEST_F(PythonSetup, convert_basic_class){
    test_convert_basic(A(99), PyClass_FromClass);
}


template<typename T>
void array_call(T v[3], T to[3]){
    v[0] = to[0];
    // skip element 1
    v[2] = to[2];
}

template<typename T>
void test_conversion(T vals[3]){
    using namespace pyllars_internal;
    T array[3] = {vals[0], vals[1], vals[2]};
    PythonClassWrapper<T[3]>::initialize();
    auto obj = toPyObject<T[3]>(array, 3);
    argument_capture<T[3]> avalue = toCArgument<T[3] >(*obj);
    Assertion<T>::assert_equal((avalue.value())[0], array[0]);
    Assertion<T>::assert_equal((avalue.value())[1], array[1]);
    Assertion<T>::assert_equal((avalue.value())[2], array[2]);
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
    using namespace pyllars_internal;

    auto obj = PyList_New(3);
    PythonClassWrapper<T>* o= (PythonClassWrapper<T>*)PyFrom(vals[0]);
    Py_INCREF(o);
    PyList_SetItem(obj, 0, (PyObject*)o);//PyFrom(vals[0]));
    PyList_SetItem(obj, 1, PyFrom(vals[1]));
    PyList_SetItem(obj, 2, PyFrom(vals[2]));
    {
        auto val = toCArgument<T[3]>(*obj);
        call(val.value(), toVals);
    }
    ASSERT_NE(PyList_GetItem(obj, 0), nullptr);
    ASSERT_NE(PyList_GetItem(obj, 1), nullptr);
    ASSERT_NE(PyList_GetItem(obj, 2), nullptr);
    Assertion<T>::assert_equal(PyTo(PyList_GetItem(obj, 0)), toVals[0]);
    Assertion<T>::assert_equal(PyTo(PyList_GetItem(obj, 1)), vals[1]);
    Assertion<T>::assert_equal(PyTo(PyList_GetItem(obj, 2)), toVals[2]);

    typedef const char* cstring;
    ASSERT_THROW((toCArgument<DisparateType>(*obj)), PyllarsException);
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
    using namespace pyllars_internal;
    return toPyObject<T>(v, 1);
}

template<typename T>
T PyWrapper_AsValue(PyObject* obj){
    using namespace pyllars_internal;
    assert(PyObject_TypeCheck(obj, PythonClassWrapper<T>::getPyType()));
    assert(((PythonClassWrapper<T>*)obj)->get_CObject());
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

const char* toCString(PyObject* obj){
    return (const char*)PyUnicode_AsUTF8(obj);
}

TEST_F(PythonSetup, convert_from_native_py_cstring) {

try {

    const char *vals[3] = {"abc", "def", "ghi"};
    const char *toVals[3] = {"rst", "uvw", "xyz"};
    typedef const char *(*func_t)(PyObject*);

    test_conversion_from_native_py<const char *, array_call<const char *>,
             PyUnicode_FromString, toCString>(vals, toVals);
    test_conversion_from_native_py<const char *, array_call<const char *>,
            PyWrapper_FromValue<const char *>, PyWrapper_AsValue<const char *> >(vals, toVals);
} catch (const char* msg){
    throw msg;
}
}

const char* __PyUnicode_ToString(PyObject* obj) {
    return (const char *) PyUnicode_AsUTF8(obj);
}

TEST_F(PythonSetup, convert_from_native_pybytes_cstring) {
    const char*  vals[3] = {"abc", "def", "ghi"};
    const char*  toVals[3] = {"rst", "uvw", "xyz"};
    test_conversion_from_native_py<const char* , array_call<const char* >,
            PyUnicode_FromString, __PyUnicode_ToString>(vals, toVals);
}

template <typename T>
void convert_to_py(){

}

template<typename T>
class ToPyTest: public PythonSetup{
public:

    using Type = T;

};

using TypeList = testing::Types<A, const A, A&, const A&>;

#ifndef TYPED_TEST_SUITE
#define TYPED_TEST_SUITE TYPED_TEST_CASE
#endif

TYPED_TEST_SUITE(ToPyTest, TypeList);

TYPED_TEST(ToPyTest, convert_to_py ) {
    using T =typename TestFixture::Type;

    using namespace pyllars_internal;
    PythonClassWrapper<T>::initialize();
    typedef typename std::remove_reference<T>::type T_NoRef;
    T_NoRef *a = new T_NoRef();
    {
        auto obj = toPyArgument<T>(*a, 1);
        ASSERT_NE(obj, nullptr);
        ASSERT_FALSE(PyErr_Occurred());
        ASSERT_TRUE(PyObject_TypeCheck(obj, PythonClassWrapper<T&>::getPyType()));
        ASSERT_EQ(a, reinterpret_cast<PythonClassWrapper<T&> *>(obj)->get_CObject());
        Py_DECREF(obj);
    }
    {
        auto obj = toPyObject<T&>(*a, 1);
        ASSERT_NE(obj, nullptr);
        ASSERT_FALSE(PyErr_Occurred());
        ASSERT_TRUE(PyObject_TypeCheck(obj, PythonClassWrapper<T&>::getPyType()));
        ASSERT_EQ(a, reinterpret_cast<PythonClassWrapper<T&> *>(obj)->get_CObject());
        Py_DECREF(obj);
    }
}


template<typename T>
class ToPyTest2: public PythonSetup{
public:

    using Type = T;

};

using TypeList2 = testing::Types<A, const A>;
TYPED_TEST_SUITE(ToPyTest2, TypeList2);

TYPED_TEST(ToPyTest2, convert_to_py_array ) {
    PyErr_Clear();
    constexpr size_t size = 23;
    using T =typename TestFixture::Type[size];
    using T_element = typename TestFixture::Type;

    using namespace pyllars_internal;
    PythonClassWrapper<T>::initialize();
    typedef typename std::remove_reference<T>::type T_NoRef;
    T values = {T_element()};
    {
        auto *a = new(values) FixedArrayHelper<T>;
        auto obj = toPyArgument<T>(a->values(), 1);
        ASSERT_NE(obj, nullptr);
        ASSERT_FALSE(PyErr_Occurred());
        ASSERT_TRUE(PyObject_TypeCheck(obj, PythonClassWrapper<T&>::getPyType()));
        auto pywrapper = reinterpret_cast<PythonClassWrapper<T&> *>(obj);
        ASSERT_EQ((T *) a, pywrapper->get_CObject());
        Py_DECREF(obj);
    }
    {
        auto *a = new(values) FixedArrayHelper<T>;
        auto obj = toPyObject<T&>(a->values(), 1);
        ASSERT_NE(obj, nullptr);
        ASSERT_FALSE(PyErr_Occurred());
        ASSERT_TRUE(PyObject_TypeCheck(obj, PythonClassWrapper<T&>::getPyType()));
        ASSERT_EQ((T *) a, reinterpret_cast<PythonClassWrapper<T&> *>(obj)->get_CObject());
        Py_DECREF(obj);
    }
}
