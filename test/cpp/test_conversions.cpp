//
// Created by jrusnak on 3/1/19.
//

#include <stdlib.h>
#include <Python.h>
#include <src/resources/pyllars/pyllars.hpp>
#include "gtest/gtest.h"
#include "pyllars/pyllars.hpp"
#include "pyllars/pyllars_classwrapper.impl"
#include "pyllars/pyllars_pointer.impl"
#include "pyllars/pyllars_membersemantics.impl"
#include "pyllars/pyllars_classmembersemantics.impl"
#include "pyllars/pyllars_classmethodsemantics.impl"

struct A{
    int ivalue;
};


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

void int_array_call(int v[3]){
    v[0] = 999;
    v[2] = -764;
}

TEST_F(PythonSetup, convert_array){
    using namespace __pyllars_internal;

    int array[3] = {1,2,3};
    PythonClassWrapper<int[3]>::initialize();
    auto obj = toPyObject<int[3]>(array, true, 3);
    smart_ptr<int[3], false> avalue = toCArgument<int[3], false >(*obj);
    ASSERT_EQ((*avalue)[0], array[0]);
    ASSERT_EQ((*avalue)[1], array[1]);
    ASSERT_EQ((*avalue)[2], array[2]);

    obj = PyList_New(3);
    PyList_SetItem(obj, 0, PyLong_FromLong(1));
    PyList_SetItem(obj, 1, PyLong_FromLong(2));
    PyList_SetItem(obj, 2, PyLong_FromLong(3));
    {
        int_array_call(*toCArgument<int[3], false>(*obj));
        ASSERT_EQ(PyLong_AsLong(PyList_GetItem(obj, 0)), 999);
        ASSERT_EQ(PyLong_AsLong(PyList_GetItem(obj, 2)), -764);
    }

    PyList_SetItem(obj, 0, PyString_FromString("abc"));
    PyList_SetItem(obj, 1, PyString_FromString("def"));
    PyList_SetItem(obj, 2, PyString_FromString("xyz"));
    {
        const char**v = *toCArgument<const char* [3], false>(*obj);
    }
}