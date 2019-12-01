//
// Created by johnj on 11/29/2019.
//
#include "pyllars/internal/pyllars_classwrapper-type.impl.hpp"
#include "pyllars/internal/pyllars_pointer.impl.hpp"
#include "pyllars/internal/pyllars_reference.impl.hpp"
#include "pyllars/internal/pyllars_integer.hpp"
#include "pyllars/pyllars_class.hpp"

namespace pyllars_internal {

    ////////////////
    // pointers
    ////////////////

    template
    struct DLLEXPORT PythonClassWrapper<bool *>;

    template
    struct DLLEXPORT PythonClassWrapper<char *>;

    template
    struct DLLEXPORT PythonClassWrapper<signed char *>;

    template
    struct DLLEXPORT PythonClassWrapper<short *>;

    template
    struct DLLEXPORT PythonClassWrapper<int *>;

    template
    struct DLLEXPORT PythonClassWrapper<long *>;

    template
    struct DLLEXPORT PythonClassWrapper<long long *>;

    template
    struct DLLEXPORT PythonClassWrapper<unsigned char *>;


    template
    struct DLLEXPORT PythonClassWrapper<unsigned short *>;

    template
    struct DLLEXPORT PythonClassWrapper<unsigned int *>;

    template
    struct DLLEXPORT PythonClassWrapper<unsigned long *>;


    template
    struct DLLEXPORT PythonClassWrapper<unsigned long long *>;

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


    ////////////////
    // refs
    ////////////////

    template
    struct DLLEXPORT PythonClassWrapper<bool &>;

    template
    struct DLLEXPORT PythonClassWrapper<char &>;

    template
    struct DLLEXPORT PythonClassWrapper<signed char &>;

    template
    struct DLLEXPORT PythonClassWrapper<short &>;

    template
    struct DLLEXPORT PythonClassWrapper<int &>;

    template
    struct DLLEXPORT PythonClassWrapper<long &>;

    template
    struct DLLEXPORT PythonClassWrapper<long long &>;

    template
    struct DLLEXPORT PythonClassWrapper<unsigned char &>;


    template
    struct DLLEXPORT PythonClassWrapper<unsigned short &>;

    template
    struct DLLEXPORT PythonClassWrapper<unsigned int &>;

    template
    struct DLLEXPORT PythonClassWrapper<unsigned long &>;


    template
    struct DLLEXPORT PythonClassWrapper<unsigned long long &>;

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


    ////////////////
    // ref declarators
    ////////////////

    template
    struct DLLEXPORT PythonClassWrapper<bool &&>;

    template
    struct DLLEXPORT PythonClassWrapper<char &&>;

    template
    struct DLLEXPORT PythonClassWrapper<signed char &&>;

    template
    struct DLLEXPORT PythonClassWrapper<short &&>;

    template
    struct DLLEXPORT PythonClassWrapper<int &&>;

    template
    struct DLLEXPORT PythonClassWrapper<long &&>;

    template
    struct DLLEXPORT PythonClassWrapper<long long &&>;

    template
    struct DLLEXPORT PythonClassWrapper<unsigned char &&>;


    template
    struct DLLEXPORT PythonClassWrapper<unsigned short &&>;

    template
    struct DLLEXPORT PythonClassWrapper<unsigned int &&>;

    template
    struct DLLEXPORT PythonClassWrapper<unsigned long &&>;


    template
    struct DLLEXPORT PythonClassWrapper<unsigned long long &&>;

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
    class DLLEXPORT PyllarsClass<bool, GlobalNS>;

    template
    class DLLEXPORT PyllarsClass<const bool, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<volatile bool, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<const volatile bool, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<char, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<unsigned char, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<signed char, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<short, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<unsigned short, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<int, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<unsigned int, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<long, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<unsigned long, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<long long, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<unsigned long long, GlobalNS>;

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

    //////////

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