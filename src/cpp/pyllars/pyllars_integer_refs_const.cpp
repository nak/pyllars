//
// Created by johnj on 11/29/2019.
//
#include "pyllars/internal/pyllars_integer.hpp"
#include "pyllars/internal/pyllars_classwrapper-type.impl.hpp"
#include "pyllars/internal/pyllars_reference.impl.hpp"
#include <pyllars/internal/pyllars_pointer-type.impl.hpp>
#include <pyllars/internal/pyllars_pointer.impl.hpp>

#include "pyllars/pyllars_class.hpp"


namespace pyllars_internal {

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
