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
    using namespace pyllars_internal;
    auto obj = PyObject_GetAttrString(pyllars::GlobalNS::module(), "const_str_ptr");
    ASSERT_NE(obj, nullptr);
    ASSERT_TRUE(PyCallable_Check(obj));
    PyObject* str = PyObject_Call(obj, PyTuple_New(0), nullptr);
    ASSERT_NE(str, nullptr);
    ASSERT_STREQ(*((PythonClassWrapper<const char* const>*)str)->get_CObject(), "global_string");
}