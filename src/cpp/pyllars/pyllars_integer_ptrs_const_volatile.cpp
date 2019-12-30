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

    ////////////////
    // pointers
    ////////////////

    template
    struct DLLEXPORT PythonClassWrapper<const volatile bool *>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile char *>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile signed char *>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile short *>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile int *>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile long *>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile long long *>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile unsigned char *>;


    template
    struct DLLEXPORT PythonClassWrapper<const volatile unsigned short *>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile unsigned int *>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile unsigned long *>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile unsigned long long *>;

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

namespace pyllars{



    template
    class DLLEXPORT PyllarsClass<const volatile char, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const volatile unsigned char, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const volatile signed char, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const volatile short, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const volatile unsigned short, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const volatile int, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const volatile unsigned int, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const volatile long, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const volatile unsigned long, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const volatile long long, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const volatile unsigned long long, GlobalNS>;

}