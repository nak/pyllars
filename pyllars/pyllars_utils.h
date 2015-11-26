#include "pyllars/pyllars_pointer.h"

#ifndef __PYLLARS__INTERNAL__UTILS_H
#define __PYLLARS__INTERNAL__UTILS_H

#include <structmember.h>

/**
 * Some (perhaps odd) utility classes to aide in converting calls with python tuples
 * to full-blown argument lists used in C function calls
 **/
namespace __pyllars_internal{


    /**
     * container class for holding a tuple, to be expanded to an argument list
    **/
    template<int ...S>
    struct container {
    };

    /**
     * Counter class to expand arguments from a tuple object
     * to a set of arguments passed to a function
     **/
    template<int N, int ...S>
    struct argGenerator : argGenerator<N-1, N-1, S...> {

    };

    /**
     * 0th order specialization
    **/
    template<int ...S>
    struct argGenerator<0, S...> {
      typedef container<S...> type;

    };
}

#define CLASS_NAME(T) #T

#endif
