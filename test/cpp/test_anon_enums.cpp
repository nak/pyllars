#include <stdlib.h>
#include "gtest/gtest.h"
#include "pyllars/pyllars.hpp"
#include "pyllars/pyllars_classwrapper.impl.hpp"
#include "pyllars/pyllars_pointer.impl.hpp"
#include "pyllars/pyllars_membersemantics.impl.hpp"
#include "pyllars/pyllars_classmembersemantics.impl.hpp"
#include "pyllars/pyllars_staticfunctionsemantics.impl.hpp"
#include <map>
#include "class_test_defns.h"
#include "anon_enum_defns.h"
#include "setup.h"
#include "setup_anon_enums.hpp"

#include "pyllars/pyllars_classenum.hpp"
#include "pyllars/pyllars_classenumclass.hpp"

template<>
const char* const __pyllars_internal::_Types<ClassWithEnum>::type_name = "ClassWithEnum";

template<>
const char* const __pyllars_internal::_Types<decltype(ClassWithEnum::FIRST)>::type_name = "anonymous enum";

template<>
const char* const __pyllars_internal::_Types<Enum>::type_name = "Enum";



TEST_F(SetupAnonEnums, TestPythonClassWrappperChar){
    ASSERT_EQ(PyType_Ready(__pyllars_internal::PythonClassWrapper<char>::getPyType()), 0);
    PyObject* args = PyTuple_New(0);
    PyObject* obj = PyObject_Call((PyObject*) __pyllars_internal::PythonClassWrapper<char>::getPyType(), args, nullptr);
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
    ASSERT_STREQ(__pyllars_internal::Types<char>::type_name(), "c_char");
    ASSERT_STREQ(__pyllars_internal::Types<int>::type_name(), "c_int");
    ASSERT_STREQ(__pyllars_internal::Types<unsigned long long>::type_name(), "c_unsigned_long_long");
    ASSERT_STREQ(__pyllars_internal::Types<double>::type_name(), "c_double");
    ASSERT_STREQ(__pyllars_internal::Types<const double>::type_name(), "const_c_double");
}



TEST_F(SetupAnonEnums, TestClassWithEnum){
    using namespace __pyllars_internal;
    PyObject* args = PyTuple_New(0);
    PyObject* obj = PyObject_Call((PyObject*) PythonClassWrapper<ClassWithEnum>::getPyType(),
                                  args, nullptr);
    ASSERT_NE(obj, nullptr);
    auto first = PyObject_GetAttrString(obj, "FIRST");
    ASSERT_NE(first, nullptr);
    ASSERT_EQ(PyInt_AsLong(first), ClassWithEnum::FIRST);
    auto second = PyObject_GetAttrString(obj, "SECOND");
    ASSERT_NE(second, nullptr);
    ASSERT_EQ(PyInt_AsLong(second), ClassWithEnum::SECOND);
    Py_DECREF(obj);
}

TEST_F(SetupAnonEnums, TestEnums){
    using namespace __pyllars_internal;
    typedef PythonClassWrapper<Enum> Class;

    PyObject* ZERO_E = PyObject_GetAttrString((PyObject*)Class::getType(), "ZERO");
    ASSERT_NE(ZERO_E, nullptr);
    ASSERT_EQ(PyInt_AsLong(ZERO_E), 0);
}


