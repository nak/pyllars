//
// Created by johnj on 11/29/2019.
//
#include "pyllars/internal/pyllars_classwrapper-type.impl.hpp"
#include "pyllars/internal/pyllars_pointer.impl.hpp"
#include "pyllars/internal/pyllars_reference.impl.hpp"
#include "pyllars/internal/pyllars_integer.hpp"
#include "pyllars/pyllars_class.hpp"
#include <pyllars/internal/pyllars_pointer-type.impl.hpp>

namespace pyllars_internal {

    ////////////////
    // pointers
    ////////////////

    template
    struct DLLEXPORT PythonClassWrapper<volatile bool *>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile char *>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile signed char *>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile short *>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile int *>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile long *>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile long long *>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile unsigned char *>;


    template
    struct DLLEXPORT PythonClassWrapper<volatile unsigned short *>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile unsigned int *>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile unsigned long *>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile unsigned long long *>;



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

namespace pyllars{



    template
    class DLLEXPORT PyllarsClass<volatile bool, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const volatile bool, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<volatile char, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<volatile unsigned char, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<volatile signed char, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<volatile short, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<volatile unsigned short, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<volatile int, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<volatile unsigned int, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<volatile long, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<volatile unsigned long, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<volatile long long, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<volatile unsigned long long, GlobalNS>;



}