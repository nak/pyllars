//
// Created by jrusnak on 3/24/19.
//

#include "setup.h"
#include "setup_globals.h"
#include "pyllars/pyllars.hpp"
#include "pyllars/internal/pyllars_globalmembersemantics.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"
#include "globals.h"

TEST_F(SetupGlobals, TestGlobals) {
    using namespace __pyllars_internal;
    auto obj = reinterpret_cast<GlobalVariable::Container<const char* const> *>(PyObject_GetAttrString(SetupGlobals::pymod, "const_str_ptr"));
    ASSERT_NE(obj, nullptr);
    ASSERT_TRUE(PyCallable_Check((PyObject*)obj));
    PyObject* str = PyObject_Call((PyObject*)obj, PyTuple_New(0), nullptr);
    ASSERT_NE(str, nullptr);
    ASSERT_STREQ(*((PythonClassWrapper<const char* const>*)str)->get_CObject(), "global_string");
}