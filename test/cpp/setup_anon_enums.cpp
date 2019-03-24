//
// Created by jrusnak on 3/24/19.
//

#include "setup_anon_enums.hpp"
#include "class_test_defns.h"
#include "anon_enum_defns.h"

#include "pyllars/pyllars.hpp"
#include "pyllars/pyllars_classwrapper.impl.hpp"
#include "pyllars/pyllars_pointer.impl.hpp"
#include "pyllars/pyllars_membersemantics.impl.hpp"
#include "pyllars/pyllars_classmembersemantics.impl.hpp"
#include "pyllars/pyllars_classmethodsemantics.impl.hpp"

typedef const char c_string[];

constexpr c_string bit_name = "bit";

void
SetupAnonEnums::SetUpTestSuite(){
    PythonBased::SetUpTestSuite();
    using namespace __pyllars_internal;
    static const char *const kwlist[] = {"value", nullptr};
    static const char *const empty_list[] = {nullptr};
    static const char *const kwlist_copy_constr[] = {"obj", nullptr};
    static const char *const kwlist_copy_constr2[] = {"float_val", "unused", nullptr};

    {
        typedef PythonClassWrapper<Enum> Class;
        Class::addConstructor<>(empty_list);
        Class::addConstructor<Enum>(kwlist);
        Class::addEnumValue("ZERO", ZERO);
        Class::initialize();
    }
    {
        static const char *const value[] = {"value", nullptr};
        PythonClassWrapper<decltype(ClassWithEnum::FIRST)>::addConstructor<decltype(ClassWithEnum::FIRST)>(
                value);
        PythonClassWrapper<ClassWithEnum>::addConstructor<>(empty_list);
        PythonClassWrapper<ClassWithEnum>::addEnumValue("FIRST", ClassWithEnum::FIRST);
        PythonClassWrapper<ClassWithEnum>::addEnumValue("SECOND", ClassWithEnum::SECOND);
        PythonClassWrapper<ClassWithEnum>::addEnumValue("THIRD", ClassWithEnum::THIRD);
        ASSERT_EQ(PythonClassWrapper<ClassWithEnum>::initialize(), 0);

    }
    ASSERT_FALSE(PyErr_Occurred());
}