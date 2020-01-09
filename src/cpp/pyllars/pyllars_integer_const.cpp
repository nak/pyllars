//
// Created by jrusnak on 12/3/19.
//


#include "pyllars_integer.impl.hpp"
#include <pyllars/internal/pyllars_pointer-createAllocatedInstance.impl.hpp>

namespace pyllars_internal {

    template
    struct DLLEXPORT PyNumberCustomObject<const bool>;

    template
    struct DLLEXPORT PyNumberCustomObject<const char>;

    template
    struct DLLEXPORT PyNumberCustomObject<const short>;

    template
    struct DLLEXPORT PyNumberCustomObject<const int>;

    template
    struct DLLEXPORT PyNumberCustomObject<const long>;

    template
    struct DLLEXPORT PyNumberCustomObject<const long long>;

    template
    struct DLLEXPORT PyNumberCustomObject<const unsigned char>;

    template
    struct DLLEXPORT PyNumberCustomObject<const signed char>;

    template
    struct DLLEXPORT PyNumberCustomObject<const unsigned short>;

    template
    struct DLLEXPORT PyNumberCustomObject<const unsigned int>;

    template
    struct DLLEXPORT PyNumberCustomObject<const unsigned long>;

    template
    struct DLLEXPORT PyNumberCustomObject<const unsigned long long>;


}