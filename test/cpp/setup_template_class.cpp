//
// Created by jrusnak on 3/24/19.
//
#include <Python.h>
#include "setup_template_class.h"

#include "pyllars/pyllars.hpp"
#include "pyllars/internal/pyllars_templates.hpp"

#include <pyllars/pyllars_classmethod.hpp>
#include <pyllars/pyllars_class.hpp>
#include <pyllars/pyllars_classconstructor.hpp>
#include <pyllars/pyllars_namespacewrapper.hpp>

/*
namespace{
     const char *const kwlist[] = {"val", nullptr};
     const char *const kwlist2[] = {"val", nullptr};
}

template<>
const char pyllars_internal::TypeInfo<SingleTypeTemplateClassInt >::type_name[] = "SingleTypeTemplateClass<int>";
template<>
const char pyllars_internal::TypeInfo<SingleTypeTemplateClassCharPtr >::type_name[] = "SingleTypeTemplateClass<const char*>";


void
SetupTemplateClass::SetUpTestSuite() {
    PythonBased::SetUpTestSuite();
    static bool inited = false;
    if (inited) return;
    inited = true;
    static const char *const empty_list[] = {nullptr};
    auto PySingleTypeTemplateClass = PyObject_GetAttrString(pyllars::GlobalNS::module(), "SingleTypeTemplateClass");
    {

        auto status = pyllars::TemplateClassInstantiation<single_type_template_name, SingleTypeTemplateClassInt >::
                template Params<pyllars::BareArgBundle<int> >::instantiate();
        ASSERT_EQ(status, 0);
    }
    {

        auto status = pyllars::TemplateClassInstantiation<single_type_template_name, SingleTypeTemplateClassCharPtr >::
            template Params< pyllars::BareArgBundle<const char*> >::instantiate();
        ASSERT_EQ(status, 0);
    }

    ASSERT_FALSE(PyErr_Occurred());
}
*/