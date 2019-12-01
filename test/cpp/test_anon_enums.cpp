#include "setup.h"
#include <stdlib.h>
#include "gtest/gtest.h"
#include "pyllars/pyllars.hpp"
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"
#include "pyllars/internal/pyllars_pointer.impl.hpp"
#include "pyllars/internal/pyllars_membersemantics.impl.hpp"
#include "pyllars/internal/pyllars_classmembersemantics.impl.hpp"
#include "pyllars/internal/pyllars_staticfunctionsemantics.impl.hpp"
#include <map>
#include "class_test_defns.h"
#include "anon_enum_defns.h"
#include "setup_anon_enums.hpp"

#include "pyllars/pyllars_enum.hpp"


TEST_F(SetupAnonEnums, TestPythonClassWrappperChar){
    auto c_char = PyObject_GetAttrString(pyllars::GlobalNS::module(), "c_char");
    ASSERT_NE(c_char, nullptr);
    ASSERT_TRUE(PyType_Check((PyTypeObject*)c_char));
    PyObject* args = PyTuple_New(0);
    PyObject* obj = PyObject_Call(c_char, args, nullptr);
    ASSERT_NE(obj, nullptr);
    ASSERT_NE(obj->ob_type->tp_as_number, nullptr);
    ASSERT_NE(obj->ob_type->tp_as_number->nb_add, nullptr);
    ASSERT_NE(obj->ob_type->tp_as_number->nb_subtract, nullptr);
    ASSERT_NE(obj->ob_type->tp_as_number->nb_multiply, nullptr);
    ASSERT_NE(obj->ob_type->tp_as_number->nb_remainder, nullptr);
    ASSERT_NE(obj->ob_type->tp_as_number->nb_divmod, nullptr);
    ASSERT_NE(obj->ob_type->tp_as_number->nb_power, nullptr);
    Py_DECREF(obj);

}

TEST(TypesName, TestTypeNameVariants){
    ASSERT_STREQ(pyllars_internal::Types<char>::type_name(), "c_char");
    ASSERT_STREQ(pyllars_internal::Types<int>::type_name(), "c_int");
    ASSERT_STREQ(pyllars_internal::Types<unsigned long long>::type_name(), "c_unsigned_long_long");
    ASSERT_STREQ(pyllars_internal::Types<double>::type_name(), "c_double");
    ASSERT_STREQ(pyllars_internal::Types<const double>::type_name(), "const_c_double");
}



TEST_F(SetupAnonEnums, TestClassWithEnum){
    auto PyClassWithEnum = PyObject_GetAttrString(pyllars::GlobalNS::module(), "ClassWithEnum");
    ASSERT_NE(PyClassWithEnum, nullptr);
    ASSERT_TRUE(PyType_Check((PyTypeObject*)PyClassWithEnum));
    PyObject* args = PyTuple_New(0);
    PyObject* obj = PyObject_Call(PyClassWithEnum, args, nullptr);
    ASSERT_NE(obj, nullptr);
    auto first = PyObject_GetAttrString(obj, "FIRST");
    ASSERT_NE(first, nullptr);
    auto value_f = PyObject_GetAttrString(first, "value");
    if (!value_f)PyErr_Clear();
    ASSERT_NE(value_f, nullptr);
    auto valargs = PyTuple_Pack(1, first);
    auto value = PyObject_Call(value_f, valargs, nullptr);
    ASSERT_EQ(PyInt_AsLong(value), ClassWithEnum::FIRST);
    auto second = PyObject_GetAttrString(obj, "SECOND");
    ASSERT_NE(second, nullptr);
    valargs = PyTuple_Pack(1, second);
    value = PyObject_Call(value_f, valargs, nullptr);
    ASSERT_EQ(PyInt_AsLong(value), ClassWithEnum::SECOND);
    Py_DECREF(obj);
}

TEST_F(SetupAnonEnums, TestEnums){
    auto PyEnum = PyObject_GetAttrString(pyllars::GlobalNS::module(), "Enum");
    ASSERT_NE(PyEnum, nullptr);
    ASSERT_TRUE(PyType_Check((PyTypeObject*)PyEnum));
    PyObject* ZERO_E = PyObject_GetAttrString(PyEnum, "ZERO");

    auto value_f = PyObject_GetAttrString(ZERO_E, "value");
    if (!value_f){
        PyErr_Clear();  //assertion below handles
    }
    ASSERT_NE(value_f, nullptr);
    ASSERT_NE(ZERO_E, nullptr);
    auto valargs = PyTuple_Pack(1, ZERO_E);
    auto value = PyObject_Call(value_f, valargs, nullptr);
    ASSERT_EQ(PyInt_AsLong(value), 0);
}


