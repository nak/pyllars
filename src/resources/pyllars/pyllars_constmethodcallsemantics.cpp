//
// Created by jrusnak on 7/22/16.
//
#ifndef __PYLLARS__INTERNAL__CONSTMETHODCALLSEMANTICS_CPP__
#define __PYLLARS__INTERNAL__CONSTMETHODCALLSEMANTICS_CPP__

#include "pyllars_constmethodcallsemantics.hpp"
#include "pyllars_conversions.cpp"

namespace __pyllars_internal {



    template<class CClass>
    template<const char *const name, typename T>
    PyObject *ConstMemberContainer<CClass>::Container<name, T>::
    get(PyObject *self) {

        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> ClassWrapper;
        auto _this = reinterpret_cast<ClassWrapper *>(self);
        const ssize_t array_size = ArraySize<T>::size;//sizeof(_this->template get_CObject<CClass>()->*member) / sizeof(T_base);
        if (_this->template get_CObject<CClass>()) {
            return toPyObject<T>((_this->template get_CObject<CClass>()->*member), false, array_size);
        }
        PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
        return nullptr;
    }


}

#endif