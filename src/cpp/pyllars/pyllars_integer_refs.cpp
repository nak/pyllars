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


}
