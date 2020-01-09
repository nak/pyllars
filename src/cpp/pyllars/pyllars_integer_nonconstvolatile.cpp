//
// Created by jrusnak on 12/3/19.
//


#include "pyllars_integer.impl.hpp"
#include <pyllars/internal/pyllars_pointer-createAllocatedInstance.impl.hpp>

namespace pyllars_internal {


    template
    struct DLLEXPORT PyNumberCustomObject<volatile bool>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile char>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile signed char>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile short>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile int>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile long>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile long long>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile unsigned char>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile unsigned short>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile unsigned int>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile unsigned long>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile unsigned long long>;

}