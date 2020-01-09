//
// Created by johnj on 11/29/2019.
//
#include "pyllars/internal/pyllars_classwrapper-type.impl.hpp"
#include "pyllars/internal/pyllars_pointer.impl.hpp"
#include "pyllars/internal/pyllars_reference.impl.hpp"
#include "pyllars/internal/pyllars_integer.hpp"
#include "pyllars/pyllars_class.hpp"
#include <pyllars/internal/pyllars_pointer-type.impl.hpp>
#include <pyllars/internal/pyllars_pointer.impl.hpp>

namespace pyllars_internal {



    template
    struct DLLEXPORT PythonClassWrapper<const volatile bool &>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile char &>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile signed char &>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile short &>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile int &>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile long &>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile long long &>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile unsigned char &>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile unsigned short &>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile unsigned int &>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile unsigned long &>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile unsigned long long &>;




    template
    struct DLLEXPORT PythonClassWrapper<const volatile bool &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile char &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile signed char &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile short &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile int &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile long &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile long long &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile unsigned char &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile unsigned short &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile unsigned int &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile unsigned long &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile unsigned long long &&>;

}
