//
// Created by jrusnak on 7/22/16.
//
#ifndef __PYLLARS__INTERNAL__CALLBACKS_CPP__
#define __PYLLARS__INTERNAL__CALLBACKS_CPP__

#include "pyllars_callbacks.hpp"

namespace __pyllars_internal {

    template < typename ReturnType,
            typename... Args>
    PyObject* CallbackPool< ReturnType, Args...>::pycallbacks[MAX_CB_POOL_DEPTH+1];

    template < typename ReturnType,
            typename... Args>
    typename CallbackPool< ReturnType, Args...>::callback_t
            CallbackPool< ReturnType, Args...>::c_callbacks[MAX_CB_POOL_DEPTH+1];

    template < typename ReturnType,
            typename... Args>
    typename CallbackPool< ReturnType, Args...>::callbackvar_t
            CallbackPool< ReturnType, Args...>::c_callbacksvar[MAX_CB_POOL_DEPTH+1];


}

#endif