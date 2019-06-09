//
// Created by jrusnak on 3/24/19.
//
#include <Python.h>

#include "pyllars/pyllars.hpp"
#include "pyllars/pyllars_reference.impl.hpp"
#include "pyllars/pyllars_membersemantics.impl.hpp"
#include "pyllars/pyllars_classmembersemantics.impl.hpp"
#include "pyllars/pyllars_staticfunctionsemantics.impl.hpp"
#include "pyllars/pyllars_templates.hpp"

#include "setup_template_class.h"


template<>
const char* const __pyllars_internal::_Types<SingleTypeTemplateClassInt >::type_name = "SingleTypeTemplateClass<int>";
template<>
const char* const __pyllars_internal::_Types<SingleTypeTemplateClassCharPtr >::type_name = "SingleTypeTemplateClass<const char*>";


template
struct __pyllars_internal::PythonClassWrapper<SingleTypeTemplateClassInt , void>;
template
struct __pyllars_internal::PythonClassWrapper<SingleTypeTemplateClassCharPtr , void>;

void
SetupTemplateClass::SetUpTestSuite() {
    using namespace __pyllars_internal;
    PythonBased::SetUpTestSuite();
    static bool inited = false;
    if (inited) return;
    inited = true;
    static const char *const empty_list[] = {nullptr};
    {
        typedef PythonClassWrapper <SingleTypeTemplateClassInt> Class;
        static const char *const kwlist[] = {"val", nullptr};

        Class::addConstructor<const int&>(kwlist);
        auto status = TemplateClassInstantiation<single_type_template_name, SingleTypeTemplateClassInt >::
                template Params< BareArgBundle<int> >::instantiate();
        ASSERT_EQ(status, 0);
    }
    {
        typedef PythonClassWrapper <SingleTypeTemplateClassCharPtr> Class;
        static const char *const kwlist[] = {"val", nullptr};

        Class::addConstructor<const char*&>(kwlist);
        auto status = TemplateClassInstantiation<single_type_template_name, SingleTypeTemplateClassCharPtr >::
            template Params< BareArgBundle<const char*> >::instantiate();
        ASSERT_EQ(status, 0);
    }

    ASSERT_FALSE(PyErr_Occurred());
}