//
// Created by jrusnak on 3/24/19.
//
#include <Python.h>

#include "pyllars/pyllars.hpp"
#include "pyllars/internal/pyllars_reference.impl.hpp"
#include "pyllars/internal/pyllars_membersemantics.impl.hpp"
#include "pyllars/internal/pyllars_classmembersemantics.impl.hpp"
#include "pyllars/internal/pyllars_staticfunctionsemantics.impl.hpp"
#include "pyllars/internal/pyllars_templates.hpp"

#include "setup_template_class.h"
#include <pyllars/pyllars_classmethod.hpp>
#include <pyllars/pyllars_class.hpp>
#include <pyllars/pyllars_classconstructor.hpp>
#include "pyllars/internal/pyllars_classwrapper-type.impl.hpp"


template<>
const char* const __pyllars_internal::TypeInfo<SingleTypeTemplateClassInt >::type_name = "SingleTypeTemplateClass<int>";
template<>
const char* const __pyllars_internal::TypeInfo<SingleTypeTemplateClassCharPtr >::type_name = "SingleTypeTemplateClass<const char*>";


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

        Class::addConstructor<kwlist, const int&>();
        auto status = TemplateClassInstantiation<single_type_template_name, SingleTypeTemplateClassInt >::
                template Params< BareArgBundle<int> >::instantiate();
        ASSERT_EQ(status, 0);
    }
    {
        typedef PythonClassWrapper <SingleTypeTemplateClassCharPtr> Class;
        static const char *const kwlist[] = {"val", nullptr};

        Class::addConstructor<kwlist, const char*&>();
        auto status = TemplateClassInstantiation<single_type_template_name, SingleTypeTemplateClassCharPtr >::
            template Params< BareArgBundle<const char*> >::instantiate();
        ASSERT_EQ(status, 0);
    }

    ASSERT_FALSE(PyErr_Occurred());
}