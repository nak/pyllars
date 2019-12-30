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
    struct DLLEXPORT PythonClassWrapper<const bool *>;

    template
    struct DLLEXPORT PythonClassWrapper<const char *>;

    template
    struct DLLEXPORT PythonClassWrapper<const signed char *>;

    template
    struct DLLEXPORT PythonClassWrapper<const signed short *>;

    template
    struct DLLEXPORT PythonClassWrapper<const signed int *>;

    template
    struct DLLEXPORT PythonClassWrapper<const signed long *>;

    template
    struct DLLEXPORT PythonClassWrapper<const signed long long *>;

    template
    struct DLLEXPORT PythonClassWrapper<const unsigned char *>;

    template
    struct DLLEXPORT PythonClassWrapper<const unsigned short *>;

    template
    struct DLLEXPORT PythonClassWrapper<const unsigned int *>;

    template
    struct DLLEXPORT PythonClassWrapper<const unsigned long *>;

    template
    struct DLLEXPORT PythonClassWrapper<const unsigned long long *>;

    ////////////////
    // refs
    ////////////////


    template
    struct DLLEXPORT PythonClassWrapper<const bool &>;

    template
    struct DLLEXPORT PythonClassWrapper<const char &>;

    template
    struct DLLEXPORT PythonClassWrapper<const signed char &>;

    template
    struct DLLEXPORT PythonClassWrapper<const signed short &>;

    template
    struct DLLEXPORT PythonClassWrapper<const signed int &>;

    template
    struct DLLEXPORT PythonClassWrapper<const signed long &>;

    template
    struct DLLEXPORT PythonClassWrapper<const signed long long &>;

    template
    struct DLLEXPORT PythonClassWrapper<const unsigned char &>;

    template
    struct DLLEXPORT PythonClassWrapper<const unsigned short &>;

    template
    struct DLLEXPORT PythonClassWrapper<const unsigned int &>;

    template
    struct DLLEXPORT PythonClassWrapper<const unsigned long &>;

    template
    struct DLLEXPORT PythonClassWrapper<const unsigned long long &>;


    ////////////////
    // ref declarators
    ////////////////

    template
    struct DLLEXPORT PythonClassWrapper<const bool &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const char &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const signed char &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const signed short &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const signed int &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const signed long &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const signed long long &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const unsigned char &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const unsigned short &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const unsigned int &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const unsigned long &&>;

    template
    struct DLLEXPORT PythonClassWrapper<const unsigned long long &&>;

}

namespace pyllars{

    ///////////////////

    template
    class DLLEXPORT PyllarsClass<const char, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const unsigned char, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const signed char, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const short, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const unsigned short, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const int, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const unsigned int, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const long, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const unsigned long, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const long long, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const unsigned long long, GlobalNS>;

    ////////////////////

}