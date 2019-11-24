//
// Created by jrusnak on 3/24/19.
//
#include <Python.h>

#include "class_test_defns.h"
#include "setup_basic_class.h"

#include "pyllars/pyllars_class.hpp"
#include "pyllars/internal/pyllars_classwrapper-type.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper-staticmethods.impl.hpp"

template<>
const char* const __pyllars_internal::TypeInfo<BasicClass>::type_name = "BasicClass";

template class __pyllars_internal::PythonClassWrapper<BasicClass>;


template<>
const char* const __pyllars_internal::TypeInfo<BasicClass2>::type_name = "BasicClass2";

template class __pyllars_internal::PythonClassWrapper<BasicClass2>;
template class __pyllars_internal::PythonClassWrapper<const BasicClass2>;
template class __pyllars_internal::PythonClassWrapper<volatile BasicClass2>;
template class __pyllars_internal::PythonClassWrapper<const volatile BasicClass2>;

template<>
const char* const __pyllars_internal::TypeInfo<NonDestructible>::type_name = "NonDestructible";

template class __pyllars_internal::PythonClassWrapper<NonDestructible>;

template<>
const char* const __pyllars_internal::TypeInfo<EnumClass>::type_name = "EnumClass";

template class __pyllars_internal::PythonClassWrapper<EnumClass>;
