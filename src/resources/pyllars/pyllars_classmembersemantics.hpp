register#ifndef __PYLLARS_INTERNAL__CLASSMEMBERCALLSEMANTICS_H
#define __PYLLARS_INTERNAL__CLASSMEMBERCALLSEMANTICS_H

#include </usr/include/python2.7/Python.h>
#include </usr/include/string.h>
#include </usr/include/sys/types.h>

#include "pyllars_utils.hpp"
#include "pyllars_defns.hpp"
#include "pyllars_conversions.hpp"
#include "pyllars_varargs.hpp"

/**
* This unit defines template classes needed to contain member pointers and
* define Python-to-C  semantics for accessing public members
**/


namespace __pyllars_internal {

    /**
     * Class to hold pointer to a public member of a class, specialized for
     * various types of elements (arrays, pointer, etc...)
     * @param CClass class that holds the member to be accessed
     **/
    template<class CClass>
    class ClassMemberContainer {
    public:

        /**
         * Class to hold a named non-const flat (non-array, non-pointer) class member
         */
        template<const char *const name, typename T>
        class Container {
        public:

            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T *member_t;

            static member_t member;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

            static void setFromPyObject(PyObject *pyobj);

        };

        /**
          * Class to hold a named non-const explicit array class member
          */
        template<const char *const name, size_t size, typename T>
        class Container<name, T[size]> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T *member_t[size];

            static member_t member;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

            static void setFromPyObject(PyObject *pyobj) ;
        };
    };

   /**
     * Class member container for const class members
     **/
    template<class CClass>
    class ConstClassMemberContainer {
    public:

        /**
         * Class to hold a named const flat (non-array, non-pointer) class member
         */
        template<const char *const name, typename T>
        class Container {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T const *member_t;

            static member_t member;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

        };

        /**
        * Class to hold a named const explicit array class member
        */
        template<const char *const name, size_t size, typename T>
        class Container<name, T[size]> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T const *member_t[size];

            static member_t member;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

            static void setFromPyObject(PyObject *pyobj) ;
        };
    };

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

