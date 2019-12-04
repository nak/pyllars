//
// Created by jrusnak on 3/24/19.
//
#include <Python.h>

#include "pyllars/pyllars.hpp"

#include "class_test_defns.h"
#include "setup_inheritance_test.h"

#include "pyllars/pyllars_classmethod.hpp"
#include "pyllars/pyllars_classconstructor.hpp"
#include "pyllars/pyllars_class.hpp"

#include "pyllars/internal/pyllars_classwrapper-type.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper-staticmethods.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper-xtors.impl.hpp"

namespace {
    const char new_method_name[] = "new_method";
    const char *const empty_list[] = {nullptr};
    const char *const xtor_list[] = {"value", nullptr};
    const char *const kwlist2[] = {"data", nullptr};
}
namespace pyllars_internal{
    template<> struct DLLEXPORT TypeInfo<InheritanceClass>{
        static constexpr char type_name[] = "InheritanceClass";
    };
    template<> struct DLLEXPORT TypeInfo<MultiInheritanceClass>{
        static constexpr char type_name[] = "MultiInheritanceClass";
    };
}

template class DLLEXPORT pyllars::PyllarsClass<InheritanceClass, pyllars::GlobalNS , BasicClass>;
template class DLLEXPORT pyllars::PyllarsClassConstructor<empty_list, InheritanceClass>;
template class DLLEXPORT pyllars::PyllarsClassMethod<new_method_name, kwlist2, int(InheritanceClass::*)(const char* const) const,
        &InheritanceClass::new_method >;

template class DLLEXPORT pyllars::PyllarsClass<MultiInheritanceClass, pyllars::GlobalNS , BasicClass, BasicClass2>;
template class DLLEXPORT pyllars::PyllarsClassConstructor<empty_list, MultiInheritanceClass>;
template class DLLEXPORT pyllars::PyllarsClassMethod<create_bclass2_method_name, kwlist2, BasicClass2(MultiInheritanceClass::*)(),
        &MultiInheritanceClass::createBasicClass2>;


void
SetupInheritanceTest::SetUpTestSuite() {
    SetupBasicClass::SetUpTestSuite();
}
