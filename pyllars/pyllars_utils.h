#include "pyllars/pyllars_pointer.h"

#ifndef __PYLLARS__INTERNAL__UTILS_H
#define __PYLLARS__INTERNAL__UTILS_H

#include <structmember.h>

namespace __pyllars_internal{


    template<int ...S>
    struct container {
        container( PyObject* const objs):pyobjs(objs){}
        PyObject* const pyobjs;
    };

    template<int N, int ...S>
    struct argGenerator : argGenerator<N-1, N-1, S...> {

    };

    template<int ...S>
    struct argGenerator<0, S...> {
      typedef container<S...> type;

    };
}

#define CLASS_NAME(T) #T

#endif
