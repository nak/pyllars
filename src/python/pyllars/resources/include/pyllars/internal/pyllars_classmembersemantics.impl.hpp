//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__CLASSMEMBERSEMANTICS_CPP__
#define __PYLLARS_INTERNAL__CLASSMEMBERSEMANTICS_CPP__

#include "pyllars/internal/pyllars_classmembersemantics.hpp"

#include "pyllars_conversions.hpp"

namespace{
    const char* kwlist_call[] = {"_CObject", nullptr};
}

namespace pyllars_internal{

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
            PyObject *pyarg = nullptr;
            if (PyTuple_Size(args) > 0) {
                PyErr_SetString(PyExc_ValueError,
                                "Only one _CObject with explicit keyword '_CObject' allowed to this method");
                return nullptr;
            } else if (kwds && !PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist_call, &pyarg)) {
                PyErr_SetString(PyExc_ValueError, "Invalid argument keyword name or type to method call");
                return nullptr;
            } else if (kwds) {
                if (!pyarg){ // should be set in previous if conditional
                    PyErr_SetString(PyExc_ValueError, "Failed to parse kwds in call to method");
                    return nullptr;
                }
                auto value = toCArgument<AttrType>(*pyarg);
                Assignment<AttrType>::assign(*member, value.value());
                return Py_None;
            }
        }
        return toPyObject<AttrType>(*member, ArraySize<AttrType>::size);
    }

    template<const char *const name, typename CClass, typename AttrType>
    typename ClassMember<name, CClass, AttrType>::member_t
            ClassMember<name, CClass, AttrType>::member;


}

#endif
