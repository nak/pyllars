#ifndef __PYLLARS_INTERNAL__CLASSMEMBERCALLSEMANTICS_H
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
     * Class to hold pointer to a public non-const member of a class, specialized for
     * various types of elements (arrays, pointer, etc...)
     *
     * @param CClass class that holds the member to be accessed
     **/
    template<typename CClass>
    class ClassMember {
    public:

        /**
         * Class to hold a named non-const flat (non-array, non-pointer) class member
         */
        template<const char *const name, typename AttrType>
        class Container {
        public:

            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef AttrType *member_t;

            static member_t member;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

            static void setFromPyObject(PyObject *pyobj);

        };

        /**
          * Class to hold a named non-const explicit array class member
          */
        template<const char *const name, size_t size, typename AttrType>
        class Container<name, AttrType[size]> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef AttrType *member_t[size];

            static member_t member;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

            static void setFromPyObject(PyObject *pyobj) ;
        };
    };

   /**
     * Class member container for const class members
     **/
    template<class CClass>
    class ConstClassMember {
    public:

        /**
         * Class to hold a named const flat (non-array, non-pointer) class member
         */
        template<const char *const name, typename AttrType>
        class Container {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef AttrType const *member_t;

            static member_t member;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

        };

        /**
        * Class to hold a named const explicit array class member
        */
        template<const char *const name, size_t size, typename AttrType>
        class Container<name, AttrType[size]> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef AttrType const *member_t[size];

            static member_t member;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

        };
    };

    /**
     * Class member container
     **/
    template<class CClass>
    class ConstMemberContainer {
    public:

        template<const char *const name, typename AttrType>
        class Container {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef AttrType const CClass_NoRef::* member_t;

            static member_t member;

            static int set(PyObject *self, PyObject* value){
                PyErr_SetString(PyExc_TypeError, "Cannot change a const member value");
                return -1;
            }

            static PyObject* get(PyObject* self);

        };
    };

    template<class CClass>
    template<const char *const name, typename AttrType>
    typename ConstMemberContainer<CClass>::template Container<name, AttrType>::member_t
            ConstMemberContainer<CClass>::Container<name, AttrType>::member;

}

#endif // __PYLLARS_INTERNAL__METHODCALLSEMANAttrTypeICS_H2

