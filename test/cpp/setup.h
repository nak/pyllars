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
        static bool inited = false;
        if(!inited) {
            Py_Initialize();
            PyErr_Clear();
            inited = true;
        }
    }

    static void TearDownTestSuite(){
        static bool invoked = false;
        if(!invoked) {
            ASSERT_FALSE(PyErr_Occurred());
            ASSERT_TRUE(Py_FinalizeEx() == 0);
            invoked = true;
        }
    }

    void TearDown(){
        ASSERT_FALSE(PyErr_Occurred());
    }

};

#endif //PYLLARS_SETUP_H
