//
// Created by jrusnak on 3/24/19.
//

#ifndef PYLLARS_SETUP_H
#define PYLLARS_SETUP_H
#include "gtest/gtest.h"
#include <Python.h>

class PythonBased: public ::testing::Test{
public:


protected:
    void SetUp() override{
        PyErr_Clear();
    }

    static void SetUpTestSuite() {
        Py_Initialize();
        PyErr_Clear();
    }

    static void TearDownTestSuite(){
        ASSERT_FALSE(PyErr_Occurred());
        ASSERT_TRUE(Py_FinalizeEx() == 0);
    }

    void TearDown(){
        ASSERT_FALSE(PyErr_Occurred());
        PyErr_Clear();
    }

};

#endif //PYLLARS_SETUP_H
