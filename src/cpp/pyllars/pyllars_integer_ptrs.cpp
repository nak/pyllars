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
#include <pyllars/internal/pyllars_pointer-createAllocatedInstance.impl.hpp>

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
    struct DLLEXPORT Pointers<bool *>;

    template
    struct DLLEXPORT Pointers<char *>;

    template
    struct DLLEXPORT Pointers<signed char *>;

    template
    struct DLLEXPORT Pointers<short *>;

    template
    struct DLLEXPORT Pointers<int *>;

    template
    struct DLLEXPORT Pointers<long *>;

    template
    struct DLLEXPORT Pointers<long long *>;

    template
    struct DLLEXPORT Pointers<unsigned char *>;


    template
    struct DLLEXPORT Pointers<unsigned short *>;

    template
    struct DLLEXPORT Pointers<unsigned int *>;

    template
    struct DLLEXPORT Pointers<unsigned long *>;


    template
    struct DLLEXPORT Pointers<unsigned long long *>;


}
