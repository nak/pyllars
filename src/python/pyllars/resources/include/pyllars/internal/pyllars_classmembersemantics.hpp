#ifndef __PYLLARS_INTERNAL__CLASSMEMBERCALLSEMANTICS_H
#define __PYLLARS_INTERNAL__CLASSMEMBERCALLSEMANTICS_H

#include <Python.h>
#include <string.h>


/**
* This unit defines template classes needed to contain member pointers and
* define Python-to-C  semantics for accessing public members
**/


namespace pyllars_internal {

    /**
     * Class to hold pointer to a public static member of a class
     *
     * @tparam name: the name of the field (null-terminated char* string)
     * @tparam CClass: class that holds the member to be accessed
     * @tparam FieldType: type of field within class to contain
     **/
    template<const char *const name, typename CClass, typename FieldType>
    class DLLEXPORT ClassMember {
    public:

            typedef FieldType *member_t;

            static member_t member;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

            static void setFrom(PyObject *pyobj);

    };


}

#endif

