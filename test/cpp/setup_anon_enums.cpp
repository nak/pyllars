//
// Created by jrusnak on 3/24/19.
//

#include "setup_anon_enums.hpp"
#include "class_test_defns.h"
#include "anon_enum_defns.h"

#include "pyllars/pyllars.hpp"
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper-type.impl.hpp"
#include "pyllars/internal/pyllars_pointer.impl.hpp"
#include "pyllars/internal/pyllars_membersemantics.impl.hpp"
#include "pyllars/internal/pyllars_classmembersemantics.impl.hpp"
#include "pyllars/internal/pyllars_staticfunctionsemantics.impl.hpp"

#include "pyllars/pyllars_classmethod.hpp"
#include "pyllars/pyllars_classstaticmethod.hpp"
#include "pyllars/pyllars_classconstructor.hpp"
#include "pyllars/pyllars_enum.hpp"
#include "pyllars/pyllars_classoperator.hpp"
#include "pyllars/pyllars_classstaticmember.hpp"
#include "pyllars/pyllars_classmember.hpp"
#include "pyllars/pyllars_classbitfield.hpp"
#include "pyllars/pyllars_class.hpp"


typedef const char c_string[];

constexpr c_string bit_name = "bit";
constexpr const char EnumName[] = "Enum";
constexpr const char ClassWithEnumName[] = "ClassWithEnum";
constexpr const char * enum_names[] = {"ZERO", "ONE", "TWO"};
constexpr const char * cwenum_names[] = {"FIRST", "SECOND", "THIRD"};

template class pyllars::PyllarsClass<ClassWithEnum, pyllars::GlobalNS>;
template class pyllars::PyllarsClassConstructor<emptylist, ClassWithEnum>;
template class pyllars::PyllarsEnum<EnumName, Enum, pyllars::GlobalNS , enum_names, ZERO, ONE, TWO>;
template class pyllars::PyllarsClassConstructor<emptylist, Enum>;
template class pyllars::PyllarsEnum<ClassWithEnumName, decltype(ClassWithEnum::FIRST), ClassWithEnum, cwenum_names,
        ClassWithEnum::FIRST, ClassWithEnum::SECOND, ClassWithEnum::THIRD>;
template class pyllars::PyllarsClassConstructor<emptylist, decltype(ClassWithEnum::FIRST)>;

void
SetupAnonEnums::SetUpTestSuite(){
    PythonBased::SetUpTestSuite();
    using namespace __pyllars_internal;
    static const char *const kwlist[] = {"_CObject", nullptr};
    static const char *const empty_list[] = {nullptr};
    static const char *const kwlist_copy_constr[] = {"obj", nullptr};
    static const char *const kwlist_copy_constr2[] = {"float_val", "unused", nullptr};

    {
        typedef PythonClassWrapper<Enum> Class;
    }
    {
        static const char *const value[] = {"_CObject", nullptr};
        ASSERT_EQ(PythonClassWrapper<ClassWithEnum>::initialize(), 0);

    }
    ASSERT_FALSE(PyErr_Occurred());
}