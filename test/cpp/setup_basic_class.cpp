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

int BasicClass::class_member = 6234;
//const int BasicClass::class_const_member;

long long convertEnum(const EnumClass &val){
    return (long long)val;
}
const char *const empty_list[] = {nullptr};
static const char *const kwlist_copy_constr2[] = {"float_val", "unused", nullptr};
const char *const kwlist_copy_constr[] = {"obj", nullptr};
const char *const kwlist[2] = {"_CObject", nullptr};

template class DLLEXPORT pyllars::PyllarsClass<BasicClass, pyllars::GlobalNS >;

template class DLLEXPORT pyllars::PyllarsClassConstructor<empty_list, BasicClass>;

template class DLLEXPORT pyllars::PyllarsClassConstructor<kwlist_copy_constr2, BasicClass, const double, const char *const>;

template class DLLEXPORT pyllars::PyllarsClassConstructor<kwlist_copy_constr, BasicClass, BasicClass&>;

template class DLLEXPORT pyllars::PyllarsClassConstructor<kwlist_copy_constr, BasicClass, BasicClass&&>;

template class DLLEXPORT pyllars::PyllarsClassConstructor<kwlist_copy_constr, BasicClass, const double>;

template class DLLEXPORT pyllars::PyllarsClassMapOperator<int& (BasicClass::*)(const char* const), &BasicClass::operator[]>;

template class DLLEXPORT pyllars::PyllarsClassMapOperator<const int& (BasicClass::*)(const char* const) const, &BasicClass::operator[]>;

template class DLLEXPORT pyllars::PyllarsClassMember<const_int_member_name, BasicClass, const int, &BasicClass::const_int_value>;

template class DLLEXPORT pyllars::PyllarsClassMethod<method_name, kwlist, int(BasicClass::*)(const double), &BasicClass::public_method>;

template class DLLEXPORT pyllars::PyllarsClassOperator<BasicClass(BasicClass::*)() const, &BasicClass::operator+, pyllars::OpUnaryEnum::POS>;

template class DLLEXPORT pyllars::PyllarsClassOperator<BasicClass(BasicClass::*)() const, &BasicClass::operator-, pyllars::OpUnaryEnum::NEG>;

template class DLLEXPORT pyllars::PyllarsClassOperator<BasicClass(BasicClass::*)() const, &BasicClass::operator~, pyllars::OpUnaryEnum::INV >;

template class DLLEXPORT pyllars::PyllarsClassOperator<BasicClass(BasicClass::*)(const double),&BasicClass::operator-, pyllars::OpBinaryEnum::SUB>;

template class DLLEXPORT pyllars::PyllarsClassOperator<double(BasicClass::*)(const BasicClass&) const, &BasicClass::operator+, pyllars::OpBinaryEnum::ADD>;

template class DLLEXPORT pyllars::PyllarsClassStaticMethod<static_method_name, kwlist, BasicClass,  const char* const(*)(), &BasicClass::static_public_method>;

template class DLLEXPORT pyllars::PyllarsClassStaticMember<class_const_member_name, BasicClass, const int, &BasicClass::class_const_member>;

template class DLLEXPORT pyllars::PyllarsClassStaticMember<class_member_name, BasicClass, int, &BasicClass::class_member>;

template class DLLEXPORT pyllars::PyllarsClassMember<int_array_member_name, BasicClass, int[3], &BasicClass::int_array>;

template class DLLEXPORT pyllars::PyllarsClassMember<dbl_ptr_member_name, BasicClass, const double *const, &BasicClass::double_ptr_member>;

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

void
SetupBasicClass::SetUpTestSuite() {
    using namespace pyllars_internal;
    PythonBased::SetUpTestSuite();
    static bool inited = false;
    if (inited) return;
    inited = true;
    ASSERT_EQ(PythonClassWrapper <BasicClass>::initialize(), 0);
    ASSERT_EQ(PythonClassWrapper<BasicClass2>::initialize(), 0);
    ASSERT_EQ(PythonClassWrapper<NonDestructible>::initialize(), 0);
    ASSERT_EQ(PythonClassWrapper<EnumClass> ::initialize(), 0);
    ASSERT_FALSE(PyErr_Occurred());
}

