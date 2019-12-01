//
// Created by jrusnak on 3/24/19.
//

#include "setup_functions.hpp"
#include "function_defns.hpp"

#include "pyllars/pyllars.hpp"
#include "pyllars/internal/pyllars_function_wrapper.hpp"


void
SetupFunctions::SetUpTestSuite(){
    PythonBased::SetUpTestSuite();
    using namespace pyllars_internal;

    PythonClassWrapper<void(double&)>::initialize();
    PythonClassWrapper<long long(const int i, double*)>::initialize();
    PythonClassWrapper<void(double&, int, ...)>::initialize();
    PythonClassWrapper<long long(const int i, double*, ...)>::initialize();

    ASSERT_FALSE(PyErr_Occurred());
}