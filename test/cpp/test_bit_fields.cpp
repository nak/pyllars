//
// Created by jrusnak on 3/24/19.
//


#include "setup_bitfields.h"
#include "pyllars/pyllars.hpp"
#include "class_test_defns.h"

TEST_F(SetupBitfields, TestBitFields) {
    using namespace pyllars_internal;
    PyObject *args = PyTuple_New(0);
    PyObject *obj = PyObject_Call((PyObject *) PythonClassWrapper<BitFieldContainerClass>::getPyType(),
                                  args, nullptr);
    ASSERT_NE(obj, nullptr);
    PyObject *bit = PyObject_GetAttrString(obj, "bit");
    ASSERT_NE(bit, nullptr);
    ASSERT_EQ(PyLong_AsLong(bit), 1);
    PyObject_SetAttrString(obj, "bit", PyLong_FromLong(0));
    bit = PyObject_GetAttrString(obj, "bit");
    ASSERT_EQ(PyLong_AsLong(bit), 0);
}