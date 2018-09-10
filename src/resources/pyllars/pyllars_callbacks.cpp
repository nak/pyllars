//
// Created by jrusnak on 7/22/16.
//
#ifndef __PYLLARS__INTERNAL__CALLBACKS_CPP__
#define __PYLLARS__INTERNAL__CALLBACKS_CPP__

#include "pyllars_callbacks.hpp"

namespace __pyllars_internal {

    std::vector<PyObject*> CallbackBase::pyCallbackList(MAX_CB_POOL_DEPTH);
}

#endif