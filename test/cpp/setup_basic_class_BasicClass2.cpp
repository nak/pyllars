//
// Created by jrusnak on 3/24/19.
//
#include <Python.h>

#include "pyllars/pyllars.hpp"
#include "pyllars/pyllars_class.hpp"
#include "pyllars/pyllars_classmethod.hpp"
#include "pyllars/pyllars_classstaticmethod.hpp"
#include "pyllars/pyllars_classmember.hpp"
#include "pyllars/pyllars_classstaticmember.hpp"
#include "pyllars/pyllars_enum.hpp"
#include "pyllars/pyllars_classconstructor.hpp"
#include "pyllars/pyllars_classoperator.hpp"
#include "pyllars/pyllars_classmember.hpp"
#include "pyllars/pyllars_classbitfield.hpp"
#include "pyllars/pyllars_classmapoperator.hpp"
#include "class_test_defns.h"
#include "setup_basic_class.h"
const int BasicClass::class_const_member;

long long convertEnum(const EnumClass &val){
    return (long long)val;
}
const char *const empty_list[] = {nullptr};
const char *const kwlist[2] = {"_CObject", nullptr};

template class DLLEXPORT pyllars::PyllarsClass<BasicClass2, pyllars::GlobalNS >;

template class DLLEXPORT pyllars::PyllarsClassMethod<create_bclass_method_name, kwlist, BasicClass(BasicClass2::*)(), &BasicClass2::createBasicClass>;

template class DLLEXPORT pyllars::PyllarsClassStaticMethod<create_method_name, empty_list, NonDestructible, NonDestructible *(*)(), &NonDestructible::create>;

template class DLLEXPORT pyllars::PyllarsClass<NonDestructible, pyllars::GlobalNS >;

template class DLLEXPORT pyllars::PyllarsClassStaticMethod<create_const_method_name, empty_list, NonDestructible, const NonDestructible *(*)(), &NonDestructible::create_const>;

template class DLLEXPORT pyllars::PyllarsClass<EnumClass, pyllars::GlobalNS >;

template class DLLEXPORT pyllars::PyllarsClassConstructor<empty_list, EnumClass>;

namespace {
    static const char *const kwlist2[] = {"_CObject", nullptr};
}
template class DLLEXPORT pyllars::PyllarsClassConstructor<kwlist2, EnumClass, EnumClass>;

template class DLLEXPORT pyllars::PyllarsClassStaticMethod<enum_convert_name, kwlist2, EnumClass, long long (*)(const EnumClass&), &convertEnum>;


constexpr const char * enum_names[] = {"E_ONE", "E_TWO"};
namespace {
    static const char ENUM_CLASS_NAME[] = "EnumClass";
}
template class DLLEXPORT pyllars::PyllarsEnum<ENUM_CLASS_NAME, EnumClass, pyllars::GlobalNS, enum_names, EnumClass::E_ONE, EnumClass::E_TWO>;
