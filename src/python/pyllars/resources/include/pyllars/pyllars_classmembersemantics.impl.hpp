//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__CLASSMEMBERSEMANTICS_CPP__
#define __PYLLARS_INTERNAL__CLASSMEMBERSEMANTICS_CPP__

#include "pyllars_classmembersemantics.hpp"

namespace __pyllars_internal{

    template<const char *const name, typename CClass, typename AttrType>
    PyObject *ClassMember<name, CClass, AttrType>::
    call(PyObject *cls, PyObject *args, PyObject *kwds) {
        (void) cls;
        const ssize_t arg_size = kwds?PyDict_Size(kwds):0 + PyTuple_Size(args);
        if constexpr (std::is_const<AttrType>::value){
            if (arg_size > 0) {
                PyErr_SetString(PyExc_ValueError, "C++: const static members cannot change _CObject");
                return nullptr;
            }
        } else if (arg_size > 0){
            static char format[2] = {'O', 0};
            static const char *kwlist[] = {"_CObject", nullptr};
            PyObject *pyarg = nullptr;
            if (PyTuple_Size(args) > 0) {
                PyErr_SetString(PyExc_ValueError,
                                "Only one _CObject with explicit keyword '_CObject' allowed to this method");
                return nullptr;
            } else if (kwds && !PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyarg)) {
                PyErr_SetString(PyExc_ValueError, "Invalid argument keyword name or type to method call");
                return nullptr;
            } else if (kwds) {
                auto value = toCArgument<AttrType>(*pyarg);
                Assignment<AttrType>::assign(*member, value.value());
                return Py_None;
            }
        }
        return toPyObject<AttrType>(*member, ArraySize<AttrType>::size);
    }

    template<const char *const name, typename CClass, typename AttrType>
    void ClassMember<name, CClass, AttrType>::
    setFrom(PyObject *pyobj) {
        if constexpr (std::is_const<AttrType>::value){
             throw PyllarsException(PyExc_TypeError, "Cannot set const class-member");
        } else if constexpr (std::is_array<AttrType>::value && ArraySize<AttrType>::size > 0){
            AttrType val[] = *toCArgument<AttrType, false, PythonClassWrapper<AttrType>>(*pyobj);
            for (size_t i = 0; i < ArraySize<AttrType>::size; ++i)member[i] = val[i];
        } else {
            Assignment<AttrType>::assign(*member, *toCArgument<AttrType, false>(*pyobj));
        }
    }

    template<const char *const name, typename CClass, typename AttrType>
    typename ClassMember<name, CClass, AttrType>::member_t
            ClassMember<name, CClass, AttrType>::member;


}

#endif
