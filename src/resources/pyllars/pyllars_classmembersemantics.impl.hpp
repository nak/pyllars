//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__CLASSMEMBERSEMANTICS_CPP__
#define __PYLLARS_INTERNAL__CLASSMEMBERSEMANTICS_CPP__

#include "pyllars_classmembersemantics.hpp"
#include "pyllars_function_wrapper.hpp"

namespace __pyllars_internal{

    template<typename CClass>
    template<const char *const name, typename AttrType>
    PyObject *ClassMember<CClass>::Container<name, AttrType>::
    call(PyObject *cls, PyObject *args, PyObject *kwds) {
        (void) cls;
        static const char *kwlist[] = {"value", nullptr};
        static char format[2] = {'O', 0};
        PyObject *pyarg = nullptr;
        if (PyTuple_Size(args) > 0) {
            PyErr_SetString(PyExc_ValueError,
                            "Only one value with explicit keyword 'value' allowed to this method");
            return nullptr;
        } else if (kwds && !PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyarg)) {
            PyErr_SetString(PyExc_ValueError, "Invalid argument keyword name or type to method call");
            return nullptr;
        } else if (kwds) {
            auto value = toCArgument<AttrType>(*pyarg);
            Assign<AttrType>::assign(*member,value.value());
            return Py_None;
        }
        return toPyObject<AttrType>(*member, false, ArraySize<AttrType>::size);
    }

    template<class CClass>
    template<const char *const name, typename AttrType>
    void ClassMember<CClass>::Container<name, AttrType>::
    setFromPyObject(PyObject *pyobj) {
        Assign<AttrType>::assign(*member ,*toCArgument<AttrType, false>(*pyobj));
    }

    template<class CClass>
    template<const char *const name, typename AttrType>
    typename ClassMember<CClass>::template Container<name, AttrType>::member_t
            ClassMember<CClass>::Container<name, AttrType>::member;



    template<class CClass>
    template<const char *const name, typename AttrType>
    PyObject *ConstClassMember<CClass>::Container<name, AttrType>::
    call(PyObject *cls, PyObject *args, PyObject *kwds) {
        (void) cls;
        if (PyTuple_Size(args) > 0 || kwds) {
            PyErr_SetString(PyExc_ValueError, "C++: const static members cannot change value");
            return nullptr;
        }
        return toPyObject<AttrType>(*member, false, ArraySize<AttrType>::size);
    }

    template<class CClass>
    template<const char *const name, typename AttrType>
    typename ConstClassMember<CClass>::template Container<name, AttrType>::member_t
            ConstClassMember<CClass>::Container<name, AttrType>::member;

    template<class CClass>
    template<const char *const name, size_t size, typename AttrType>
    void ClassMember<CClass>::Container<name, AttrType[size]>::setFromPyObject(PyObject* pyobj){
        AttrType val[] = *toCArgument<AttrType[size], false, PythonClassWrapper<AttrType[size]>>(*pyobj);
        for (size_t i = 0; i < size; ++i)member[i] = val[i];
    }

    template<class CClass>
    template<const char *const name, size_t size, typename AttrType>
    PyObject * ClassMember<CClass>::Container<name, AttrType[size]>::call(PyObject *cls, PyObject *args, PyObject *kwds){
        (void) cls;
        static const char *kwlist[] = {"value", nullptr};
        static char format[2] = {'O', 0};
        PyObject *pyarg = nullptr;
        if (PyTuple_Size(args) > 0) {
            PyErr_SetString(PyExc_ValueError,
                            "Only one value with explicit keyword 'value' allowed to this method");
            return nullptr;
        } else if (kwds && !PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyarg)) {
            PyErr_SetString(PyExc_ValueError, "Invalid argument keyword name or type to method call");
            return nullptr;
        } else if (kwds) {
            setFromPyObject(pyarg);
            return Py_None;
        }
        return toPyObject<AttrType>(member, false);
    }

    template<class CClass>
    template<const char *const name, typename AttrType>
    PyObject *ConstMemberContainer<CClass>::Container<name, AttrType>::
    get(PyObject *self) {

        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> ClassWrapper;
        auto _this = reinterpret_cast<ClassWrapper *>(self);
        const ssize_t array_size = ArraySize<AttrType>::size;
        if (_this->get_CObject()) {
            return toPyObject<AttrType>((_this->get_CObject()->*member), false, array_size);
        }
        PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
        return nullptr;
    }


}

#endif
