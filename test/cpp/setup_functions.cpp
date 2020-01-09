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

    PythonFunctionWrapper<void(double&)>::initialize();
    PythonFunctionWrapper<long long(const int i, double*)>::initialize();
    PythonFunctionWrapper<void(double&, int, ...)>::initialize();
    PythonFunctionWrapper<long long(const int i, double*, ...)>::initialize();

    ASSERT_FALSE(PyErr_Occurred());
}