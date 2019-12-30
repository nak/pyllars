//
// Created by jrusnak on 3/24/19.
//
#include <Python.h>

#include "pyllars/pyllars.hpp"
#include "pyllars/pyllars_class.hpp"
#include "class_test_defns.h"
#include "setup_basic_class.h"

int BasicClass::class_member = 6234;
//

void
SetupBasicClass::SetUpTestSuite() {
    using namespace pyllars_internal;
    PythonBased::SetUpTestSuite();
    static bool inited = false;
    if (inited) return;
    inited = true;
    ASSERT_EQ(PythonClassWrapper <BasicClass>::initialize(), 0);
    ASSERT_EQ(PythonClassWrapper<BasicClass2>::initialize(), 0);
    ASSERT_EQ(PythonClassWrapper<NonDestructible>::initialize(), 0);
    ASSERT_EQ(PythonClassWrapper<EnumClass> ::initialize(), 0);
    ASSERT_FALSE(PyErr_Occurred());
}

