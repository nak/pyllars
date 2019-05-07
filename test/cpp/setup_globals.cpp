//
// Created by jrusnak on 3/24/19.
//

#include "setup_globals.h"

#include "pyllars/pyllars.hpp"
#include "pyllars/pyllars_globalmembersemantics.impl.hpp"
#include "pyllars/pyllars_conversions.impl.hpp"

#include "globals.h"

static PyMethodDef SpamMethods[] = {
        {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef spammodule = {
        PyModuleDef_HEAD_INIT,
        "pyllars",   /* name of module */
        "not really anything", /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
        SpamMethods
};

void
SetupGlobals::SetUpTestSuite() {
    PythonBased::SetUpTestSuite();
    using namespace __pyllars_internal;
    static const char *const empty_list[] = {nullptr};
    {
        SetupGlobals::pymod = PyModule_Create(&spammodule);
        GlobalVariable::createGlobalVariable("const_str_ptr", &const_ptr_str, pymod, 1);
        ASSERT_FALSE(PyErr_Occurred());
    }
}

PyObject* SetupGlobals::pymod;