//
// Created by jrusnak on 3/24/19.
//

#include "setup_anon_enums.hpp"
#include "anon_enum_defns.h"

#include "pyllars/pyllars.hpp"
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper-type.impl.hpp"

#include "pyllars/pyllars_classmethod.hpp"
#include "pyllars/pyllars_classstaticmethod.hpp"
#include "pyllars/pyllars_classconstructor.hpp"
#include "pyllars/pyllars_enum.hpp"
#include "pyllars/pyllars_class.hpp"

namespace {
    const char *const kwlist[] = {"_CObject", nullptr};
    const char *const empty_list[] = {nullptr};

    const char ClassWithEnumName[] = "ClassWithEnum";
    const char *cwenum_names[] = {"FIRST", "SECOND", "THIRD"};
}

namespace pyllars_internal{

    template<> struct TypeInfo<ClassWithEnum>{
        static  constexpr char type_name[] = "ClassWithEnum";
    };

    template<> struct TypeInfo<decltype(ClassWithEnum::FIRST)>{
        static  constexpr char type_name[] = "ClassWithEnum::anon enum";
    };

}


//#ifndef MSVC
typedef decltype(ClassWithEnum::FIRST) ClassWithEnumType;
template class pyllars::PyllarsEnum<ClassWithEnumName, ClassWithEnumType, ClassWithEnum, cwenum_names,
        ClassWithEnum::FIRST, ClassWithEnum::SECOND, ClassWithEnum::THIRD>;
//#endif
template class pyllars::PyllarsClass<ClassWithEnum, pyllars::GlobalNS>;
template class pyllars::PyllarsClassConstructor<emptylist, ClassWithEnum>;
template class pyllars::PyllarsClassConstructor<emptylist, decltype(ClassWithEnum::FIRST)>;

void
SetupAnonEnums::SetUpTestSuite(){
    PythonBased::SetUpTestSuite();
}