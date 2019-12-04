//
// Created by jrusnak on 12/3/19.
//


#include "pyllars_integer.impl.hpp"

namespace pyllars_internal {


    template
    struct DLLEXPORT PyNumberCustomObject<bool>;

    template
    struct DLLEXPORT PyNumberCustomObject<char>;

    template
    struct DLLEXPORT PyNumberCustomObject<signed char>;

    template
    struct DLLEXPORT PyNumberCustomObject<short>;

    template
    struct DLLEXPORT PyNumberCustomObject<int>;

    template
    struct DLLEXPORT PyNumberCustomObject<long>;

    template
    struct DLLEXPORT PyNumberCustomObject<long long>;

    template
    struct DLLEXPORT PyNumberCustomObject<unsigned char>;

    template
    struct DLLEXPORT PyNumberCustomObject<unsigned short>;

    template
    struct DLLEXPORT PyNumberCustomObject<unsigned int>;

    template
    struct DLLEXPORT PyNumberCustomObject<unsigned long>;

    template
    struct DLLEXPORT PyNumberCustomObject<unsigned long long>;




}