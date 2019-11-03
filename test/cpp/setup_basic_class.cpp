//
// Created by jrusnak on 3/24/19.
//
#include <Python.h>
#include "pyllars/pyllars_class.hpp"
#include "pyllars/pyllars_classmethod.hpp"
#include "pyllars/pyllars_classmember.hpp"
#include "pyllars/pyllars_classenum.hpp"
#include "pyllars/pyllars_classenumclass.hpp"

#include "pyllars/pyllars.hpp"
#include "pyllars/internal/pyllars_reference.impl.hpp"
#include "pyllars/internal/pyllars_membersemantics.impl.hpp"
#include "pyllars/internal/pyllars_classmembersemantics.impl.hpp"
#include "pyllars/internal/pyllars_staticfunctionsemantics.impl.hpp"
#include "pyllars/internal/pyllars_conversions.impl.hpp"
#include "pyllars/pyllars_classmethod.hpp"
#include "pyllars/pyllars_classstaticmethod.hpp"
#include "pyllars/pyllars_classconstructor.hpp"
#include "pyllars/pyllars_classenum.hpp"
#include "pyllars/pyllars_classenumclass.hpp"
#include "pyllars/pyllars_classoperator.hpp"
#include "pyllars/pyllars_classstaticmember.hpp"
#include "pyllars/pyllars_classmember.hpp"
#include "pyllars/pyllars_classbitfield.hpp"
#include "pyllars/pyllars_class.hpp"
#include "pyllars/pyllars_classmapoperator.hpp"

#include "class_test_defns.h"
#include "setup_basic_class.h"

int BasicClass::class_member = 6234;
const int BasicClass::class_const_member;

template<>
const char* const __pyllars_internal::TypeInfo<BasicClass>::type_name = "BasicClass";

template<>
const char* const __pyllars_internal::TypeInfo<BasicClass2>::type_name = "BasicClass2";

template<>
const char* const __pyllars_internal::TypeInfo<NonDestructible>::type_name = "NonDestructible";

template<>
const char* const __pyllars_internal::TypeInfo<EnumClass>::type_name = "EnumClass";

long long convertEnum(const EnumClass &val){
    return (long long)val;
}

const char *const empty_list[] = {nullptr};

template
class pyllars::PyllarsClass<BasicClass const, pyllars::GlobalNamespace >;

template
class pyllars::PyllarsClassConstructor<empty_list, BasicClass>;

static const char *const kwlist_copy_constr2[] = {"float_val", "unused", nullptr};

template
class pyllars::PyllarsClassConstructor<kwlist_copy_constr2, BasicClass, const double, const char *const>;

const char *const kwlist_copy_constr[] = {"obj", nullptr};

template
class pyllars::PyllarsClassConstructor<kwlist_copy_constr, BasicClass, BasicClass&>;

template
class pyllars::PyllarsClassConstructor<kwlist_copy_constr, BasicClass, BasicClass&&>;

const char *const kwlist[2] = {"_CObject", nullptr};

template class pyllars::PyllarsClassConstructor<kwlist_copy_constr, BasicClass, const double>;

template class pyllars::PyllarsClassMapOperator<int& (BasicClass::*)(const char* const), &BasicClass::operator[]>;

template class pyllars::PyllarsClassMapOperator<const int& (BasicClass::*)(const char* const) const, &BasicClass::operator[]>;

template class pyllars::PyllarsClassMember<const_int_member_name, BasicClass, const int, &BasicClass::const_int_value>;

template class pyllars::PyllarsClassMethod<method_name, kwlist, int(BasicClass::*)(const double), &BasicClass::public_method>;

template class pyllars::PyllarsClassOperator<BasicClass(BasicClass::*)() const, &BasicClass::operator+, pyllars::OpUnaryEnum::POS>;

template class pyllars::PyllarsClassOperator<BasicClass(BasicClass::*)() const, &BasicClass::operator-, pyllars::OpUnaryEnum::NEG>;

template class pyllars::PyllarsClassOperator<BasicClass(BasicClass::*)() const, &BasicClass::operator~, pyllars::OpUnaryEnum::INV >;

template class pyllars::PyllarsClassOperator<BasicClass(BasicClass::*)(const double),&BasicClass::operator-, pyllars::OpBinaryEnum::SUB>;

template class pyllars::PyllarsClassOperator<double(BasicClass::*)(const BasicClass&) const, &BasicClass::operator+, pyllars::OpBinaryEnum::ADD>;

void
SetupBasicClass::SetUpTestSuite() {
    using namespace __pyllars_internal;
    PythonBased::SetUpTestSuite();
    static bool inited = false;
    if (inited) return;
    inited = true;
    {
        typedef PythonClassWrapper <BasicClass> Class;
        Class::addClassMethod<static_method_name, kwlist, const char *const(), &BasicClass::static_public_method>();
        Class::addClassAttribute<class_const_member_name, const int>(&BasicClass::class_const_member);
        Class::addClassAttribute<class_member_name, int>(&BasicClass::class_member);
        Class::addAttribute<int_array_member_name, int[3]>(&BasicClass::int_array);
        Class::addAttribute<dbl_ptr_member_name, const double *const>(&BasicClass::double_ptr_member);
        ASSERT_EQ(Class::initialize(), 0);
    }

    {
        typedef PythonClassWrapper<BasicClass2> Class;
        Class::addConstructor<>(empty_list);
        Class::addMethod<create_bclass_method_name, kwlist, BasicClass(BasicClass2::*)(),&BasicClass2::createBasicClass>();
        ASSERT_EQ(Class::initialize(), 0);
    }


    {
        static const char *const empty_list[] = {nullptr};
        typedef PythonClassWrapper<NonDestructible> Class;

        Class::addClassMethod<create_method_name, empty_list, NonDestructible *(), &NonDestructible::create>();
        Class::addClassMethod<create_const_method_name, empty_list, const NonDestructible *(), &NonDestructible::create_const>();
        Class::initialize();
    }

    {
        static const char *const empty_list[] = {nullptr};
        static const char *const kwlist[] = {"_CObject", nullptr};
        typedef PythonClassWrapper<EnumClass> Class;
        Class::addConstructor<>(empty_list);
        Class::addConstructor<EnumClass>(kwlist);
        typedef long long (*convert)(const EnumClass &);
        Class::addClassMethod<enum_convert_name, kwlist, long long(const EnumClass&), &convertEnum >();
        Class::addEnumClassValue("E_ONE", EnumClass::E_ONE);
        ASSERT_EQ(Class::initialize(), 0);

    }
    ASSERT_FALSE(PyErr_Occurred());
}

typedef int int_array[3];
template PyObject* __pyllars_internal::toPyObject<int[3]>(int_array var, const ssize_t array_size);
template PyObject* __pyllars_internal::toPyObject<int*>(int* var, const ssize_t array_size);
template PyObject* __pyllars_internal::toPyObject<int&>(int& var, const ssize_t array_size);
template PyObject* __pyllars_internal::toPyObject<const int&>(const int& var, const ssize_t array_size);
template PyObject* __pyllars_internal::toPyObject<BasicClass const&>(BasicClass const &, const ssize_t);
template PyObject* __pyllars_internal::toPyObject<double>(double, const ssize_t);

template PyObject* __pyllars_internal::toPyArgument<BasicClass const&>(BasicClass const &, const ssize_t);

template __pyllars_internal::argument_capture<BasicClass> __pyllars_internal::toCArgument(PyObject&);
