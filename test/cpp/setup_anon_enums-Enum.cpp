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
    const char *const empty_list[] = {nullptr};

    const char EnumName[] = "Enum";
    const char *enum_names[] = {"ZERO", "ONE", "TWO"};
}

namespace pyllars_internal{

    template<> struct TypeInfo<Enum>{
        static  constexpr char type_name[] = "Enum";
    };
}

template class pyllars::PyllarsClassConstructor<emptylist, Enum>;
template class pyllars::PyllarsEnum<EnumName, Enum, pyllars::GlobalNS , enum_names, ZERO, ONE, TWO>;
