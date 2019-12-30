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


const char *const bc_empty_list[] = {nullptr};
static const char *const kwlist_copy_constr2[] = {"float_val", "unused", nullptr};
const char *const kwlist_copy_constr[] = {"obj", nullptr};
const char *const kwlist2[2] = {"_CObject", nullptr};

template class DLLEXPORT pyllars::PyllarsClass<BasicClass, pyllars::GlobalNS >;

template class DLLEXPORT pyllars::PyllarsClassConstructor<bc_empty_list, BasicClass>;

template class DLLEXPORT pyllars::PyllarsClassConstructor<kwlist_copy_constr2, BasicClass, const double, const char *const>;

template class DLLEXPORT pyllars::PyllarsClassConstructor<kwlist_copy_constr, BasicClass, BasicClass&>;

template class DLLEXPORT pyllars::PyllarsClassConstructor<kwlist_copy_constr, BasicClass, BasicClass&&>;

template class DLLEXPORT pyllars::PyllarsClassConstructor<kwlist_copy_constr, BasicClass, const double>;

template class DLLEXPORT pyllars::PyllarsClassMapOperator<int& (BasicClass::*)(const char* const), &BasicClass::operator[]>;

template class DLLEXPORT pyllars::PyllarsClassMapOperator<const int& (BasicClass::*)(const char* const) const, &BasicClass::operator[]>;

template class DLLEXPORT pyllars::PyllarsClassMember<const_int_member_name, BasicClass, const int, &BasicClass::const_int_value>;

template class DLLEXPORT pyllars::PyllarsClassMethod<method_name, kwlist2, int(BasicClass::*)(const double), &BasicClass::public_method>;

template class DLLEXPORT pyllars::PyllarsClassOperator<BasicClass(BasicClass::*)() const, &BasicClass::operator+, pyllars::OpUnaryEnum::POS>;

template class DLLEXPORT pyllars::PyllarsClassOperator<BasicClass(BasicClass::*)() const, &BasicClass::operator-, pyllars::OpUnaryEnum::NEG>;

template class DLLEXPORT pyllars::PyllarsClassOperator<BasicClass(BasicClass::*)() const, &BasicClass::operator~, pyllars::OpUnaryEnum::INV >;

template class DLLEXPORT pyllars::PyllarsClassOperator<BasicClass(BasicClass::*)(const double),&BasicClass::operator-, pyllars::OpBinaryEnum::SUB>;

template class DLLEXPORT pyllars::PyllarsClassOperator<double(BasicClass::*)(const BasicClass&) const, &BasicClass::operator+, pyllars::OpBinaryEnum::ADD>;

template class DLLEXPORT pyllars::PyllarsClassStaticMethod<static_method_name, kwlist2, BasicClass,  const char* const(*)(), &BasicClass::static_public_method>;

template class DLLEXPORT pyllars::PyllarsClassStaticMember<class_const_member_name, BasicClass, const int, &BasicClass::class_const_member>;

template class DLLEXPORT pyllars::PyllarsClassStaticMember<class_member_name, BasicClass, int, &BasicClass::class_member>;

template class DLLEXPORT pyllars::PyllarsClassMember<int_array_member_name, BasicClass, int[3], &BasicClass::int_array>;

template class DLLEXPORT pyllars::PyllarsClassMember<dbl_ptr_member_name, BasicClass, const double *const, &BasicClass::double_ptr_member>;
