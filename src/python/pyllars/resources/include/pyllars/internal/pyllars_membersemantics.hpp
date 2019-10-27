#ifndef __PYLLARS_INTERNAL__MEMBERSEMANTICS_H
#define __PYLLARS_INTERNAL__MEMBERSEMANTICS_H

#include <Python.h>

#include "pyllars_utils.hpp"
#include "pyllars/internal/pyllars_defns.hpp"

/**
* This unit defines template classes needed to contain method pointers and
* define Python-to-C call semantics for invoking class instance methods
**/

namespace __pyllars_internal {

    template<const char* const name, class CClass, typename T>
    class MemberContainer {
    public:
        typedef typename std::remove_reference<CClass>::type CClass_NoRef;
        typedef T CClass_NoRef::* member_t;

        static member_t member;
        static size_t array_size;

        static int set(PyObject* o, PyObject* value, void* v=nullptr);
        static PyObject* get(PyObject* o, void* v=nullptr);

        static void setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj);
    };


    template<typename CClass>
    class BitFieldContainer {
    public:

        template<const char *const name, typename T, const size_t bits>
        class Container {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;

            typedef std::function<T(const CClass_NoRef &)> getter_t;
            typedef std::function<T(CClass_NoRef &, const T &)> setter_t;

            static getter_t _getter;
            static setter_t _setter;

            static int set(PyObject* o, PyObject* value, void* v=nullptr);
            static PyObject* get(PyObject* o, void* v=nullptr);


            static void setFromPyObject(CClass_NoRef *self, PyObject *pyobj);

        };


        template<const char *const name, typename T, const size_t bits>
        class ConstContainer {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;

            typedef std::function<T(const CClass_NoRef &)> getter_t;

            static getter_t _getter;

            static int set(PyObject* o, PyObject* value, void* v=nullptr);
            static PyObject* get(PyObject* o, void* v=nullptr);

            static void setFromPyObject(CClass_NoRef *self, PyObject *pyobj);

        };

    };

}
#endif