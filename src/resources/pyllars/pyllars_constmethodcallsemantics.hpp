#ifndef __PYLLARS_INTERNAL__CONSTMETHODCALLSEMANTICS_H
#define __PYLLARS_INTERNAL__CONSTMETHODCALLSEMANTICS_H

#include <Python.h>

#include "pyllars_defns.hpp"
#include "pyllars_utils.hpp"
#include "pyllars_conversions.hpp"
/**
* This unit defines template classes needed to contain CONSTANT method  and member pointers and
* define Python-to-C call semantics for invoking class instance methods
**/

namespace __pyllars_internal {


    /////////////////////////////////////////////////////////

    /**
     * Class member container
     **/
    template<class CClass>
    class ConstMemberContainer {
    public:

        template<const char *const name, typename T>
        class Container {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T const CClass_NoRef::* member_t;

            static member_t member;

            static int set(PyObject *self, PyObject* value);

            static PyObject* get(PyObject* self);

        };
    };

    template<class CClass>
    template<const char *const name, typename T>
    typename ConstMemberContainer<CClass>::template Container<name, T>::member_t
            ConstMemberContainer<CClass>::Container<name, T>::member;


}

#endif // __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2

