//
// Created by jrusnak on 3/24/19.
//

#ifndef PYLLARS_SETUP_H
#define PYLLARS_SETUP_H

#include <Python.h>


#include "gtest/gtest.h"
#include "pyllars_namespacewrapper.hpp"


#include <pyllars/internal/pyllars_integer.hpp>
#include <pyllars/internal/pyllars_floating_point.hpp>



class PythonBased: public ::testing::Test{
public:


    static void SetUpTestCase() {
        SetUpTestSuite();
    }
    static void TearDownTestCase(){
        TearDownTestSuite();
    }

protected:
    void SetUp() override{
        PyErr_Clear();
    }

    static void SetUpTestSuite() {
        Py_Initialize();
        PyErr_Clear();
        PyllarsInit("pyllars");
    }

    static void TearDownTestSuite(){
        ASSERT_FALSE(PyErr_Occurred());
       // ASSERT_TRUE(Py_FinalizeEx() == 0);
    }

    void TearDown() override{
        ASSERT_FALSE(PyErr_Occurred());
        PyErr_Clear();
    }

};

#endif //PYLLARS_SETUP_H
