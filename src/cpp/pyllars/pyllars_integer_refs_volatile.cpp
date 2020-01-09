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
    struct DLLEXPORT PythonClassWrapper<volatile bool &>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile char &>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile signed char &>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile short &>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile int &>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile long &>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile long long &>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile unsigned char &>;


    template
    struct DLLEXPORT PythonClassWrapper<volatile unsigned short &>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile unsigned int &>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile unsigned long &>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile unsigned long long &>;


    template
    struct DLLEXPORT PythonClassWrapper<volatile bool &&>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile char &&>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile signed char &&>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile short &&>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile int &&>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile long &&>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile long long &&>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile unsigned char &&>;


    template
    struct DLLEXPORT PythonClassWrapper<volatile unsigned short &&>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile unsigned int &&>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile unsigned long &&>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile unsigned long long &&>;


}