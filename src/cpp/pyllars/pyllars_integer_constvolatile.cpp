//
// Created by jrusnak on 12/3/19.
//


#include "pyllars_integer.impl.hpp"

namespace pyllars_internal {

    template
    struct DLLEXPORT PyNumberCustomObject<volatile const bool>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile const char>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile const signed char>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile const short>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile const int>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile const long>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile const long long>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile const unsigned char>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile const unsigned short>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile const unsigned int>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile const unsigned long>;

    template
    struct DLLEXPORT PyNumberCustomObject<volatile const unsigned long long>;

}