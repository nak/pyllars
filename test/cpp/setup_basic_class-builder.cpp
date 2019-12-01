//
// Created by jrusnak on 3/24/19.
//
#include <Python.h>

#include "class_test_defns.h"

#include "pyllars/pyllars_class.hpp"
#include "pyllars/internal/pyllars_classwrapper-type.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper-staticmethods.impl.hpp"

template struct pyllars_internal::PythonClassWrapper<BasicClass>;
template struct pyllars_internal::PythonClassWrapper<BasicClass2>;
template struct pyllars_internal::PythonClassWrapper<const BasicClass2>;
template struct pyllars_internal::PythonClassWrapper<volatile BasicClass2>;
template struct pyllars_internal::PythonClassWrapper<const volatile BasicClass2>;

template struct pyllars_internal::PythonClassWrapper<NonDestructible>;
template struct pyllars_internal::PythonClassWrapper<const NonDestructible>;
template struct pyllars_internal::PythonClassWrapper<volatile NonDestructible>;
template struct pyllars_internal::PythonClassWrapper<const volatile NonDestructible>;

template struct pyllars_internal::PythonClassWrapper<EnumClass>;
template struct pyllars_internal::PythonClassWrapper<const EnumClass>;
template struct pyllars_internal::PythonClassWrapper<volatile EnumClass>;
template struct pyllars_internal::PythonClassWrapper<const volatile EnumClass>;
