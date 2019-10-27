//
// Created by jrusnak on 3/24/19.
//
#include "setup.h"
#include "setup_template_class.h"
#include "template_class_defns.h"
#include "pyllars/pyllars.hpp"
#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_reference.impl.hpp"
#include "pyllars/internal/pyllars_conversions.impl.hpp"
#include <pyllars/internal/pyllars_templates.hpp>


template<typename T>
class TemplateClassTest : public SetupTemplateClass,
                           public testing::WithParamInterface<PyObject*>{
public:
    typedef T type;
    static T value;
};

template<>
int TemplateClassTest<int>::value = 96231;

template<>
const char* TemplateClassTest<const char*>::value = "char value";


typedef ::testing::Types<int, const char*> SingleTypeImplementations;

TYPED_TEST_CASE(TemplateClassTest, SingleTypeImplementations);

TYPED_TEST(TemplateClassTest, TestTemplateInstantiation){
    using namespace __pyllars_internal;
    PyObject* classTemplate = TemplateClassInstantiation<single_type_template_name, SingleTypeParamTemplateClass<TypeParam> >::addDefinition(nullptr);
    ASSERT_NE(classTemplate, nullptr);
    PyObject* clazz = PyObject_GetItem(classTemplate, (PyObject*)PythonClassWrapper<TypeParam>::getPyType());
    ASSERT_NE(clazz, nullptr);
    ASSERT_TRUE(PyType_Check(clazz) && clazz == (PyObject*)PythonClassWrapper<SingleTypeParamTemplateClass<TypeParam>>::getPyType());
    auto args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, toPyObject<TypeParam&>(TemplateClassTest<TypeParam>::value, 1));
    PyObject* instance = PyObject_Call(clazz, args, nullptr);
    ASSERT_NE(instance, nullptr);
    ASSERT_EQ(((PythonClassWrapper<SingleTypeParamTemplateClass<TypeParam> >*)instance)->get_CObject()->val(),
           TemplateClassTest<TypeParam>::value);
}

template
PyObject* __pyllars_internal::toPyObject<const char*>(const char*, ssize_t);