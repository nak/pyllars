//
// Created by jrusnak on 3/24/19.
//
#include <Python.h>

#include "pyllars/pyllars.hpp"
#include "pyllars/pyllars_classwrapper.impl.hpp"
#include "pyllars/pyllars_pointer.impl.hpp"
#include "pyllars/pyllars_membersemantics.impl.hpp"
#include "pyllars/pyllars_classmembersemantics.impl.hpp"
#include "pyllars/pyllars_classmethodsemantics.impl.hpp"
#include "pyllars/pyllars_conversions.impl.hpp"

#include "class_test_defns.h"
#include "setup_inheritance_test.h"

constexpr c_string new_method_name = "new_method";

template<>
const char* const __pyllars_internal::_Types<InheritanceClass>::type_name = "InheritanceClass";

template<>
const char* const __pyllars_internal::_Types<MultiInheritanceClass>::type_name ="MultiInheritanceClass";


void
SetupInheritanceTest::SetUpTestSuite() {
    SetupBasicClass::SetUpTestSuite();
    using namespace __pyllars_internal;
    static constexpr char *const empty_list[] = {nullptr};
    {
        static const char *const kwlist2[] = {"data", nullptr};
        typedef  PythonClassWrapper<InheritanceClass> Class;
        Class::addConstructor<>(empty_list);
        Class::addMethod<new_method_name, kwlist2, int(InheritanceClass::*)(const char* const) const,
                &InheritanceClass::new_method>();
        Class::addBaseClass<BasicClass>();
        ASSERT_EQ(Class::initialize(), 0);
    }

    {
        typedef PythonClassWrapper<MultiInheritanceClass> Class;
        Class::addConstructor<>(empty_list);
        static const char *const kwlist2[] = {"data", nullptr};
        Class::addMethod<create_bclass2_method_name, kwlist2,
          BasicClass2(MultiInheritanceClass::*)(),
          &MultiInheritanceClass::createBasicClass2>();

        Class::addBaseClass<BasicClass>();
        Class::addBaseClass<BasicClass2>();
        ASSERT_EQ(Class::initialize(), 0);
    }
}