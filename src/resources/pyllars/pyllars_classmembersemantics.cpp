//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__CLASSMEMBERSEMANTICS_CPP__
#define __PYLLARS_INTERNAL__CLASSMEMBERSEMANTICS_CPP__

#include "pyllars_classmembersemantics.hpp"
#include "pyllars_function_wrapper.hpp"

namespace __pyllars_internal{

    template<class CClass>
    template<const char *const name, typename T>
    PyObject *ClassMemberContainer<CClass>::Container<name, T>::
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
            Assign<T>::assign(*member,*toCObject<T, std::is_array<T>::value, PythonClassWrapper<T> >(*pyarg));
            return Py_None;
        }
        return toPyObject<T>(*member, false, ArraySize<T>::size);
    }

    template<class CClass>
    template<const char *const name, typename T>
    void ClassMemberContainer<CClass>::Container<name, T>::
    setFromPyObject(PyObject *pyobj) {
        Assign<T>::assign(*member ,*toCObject<T, false, PythonClassWrapper<T>>(*pyobj));
    }

    template<class CClass>
    template<const char *const name, typename T>
    typename ClassMemberContainer<CClass>::template Container<name, T>::member_t
            ClassMemberContainer<CClass>::Container<name, T>::member;



    template<class CClass>
    template<const char *const name, typename T>
    PyObject *ConstClassMemberContainer<CClass>::Container<name, T>::
    call(PyObject *cls, PyObject *args, PyObject *kwds) {
        (void) cls;
        if (PyTuple_Size(args) > 0 || kwds) {
            PyErr_SetString(PyExc_ValueError, "C++: const static members cannot change value");
            return nullptr;
        }
        return toPyObject<T>(*member, false, ArraySize<T>::size);
    }

    template<class CClass>
    template<const char *const name, typename T>
    typename ConstClassMemberContainer<CClass>::template Container<name, T>::member_t
            ConstClassMemberContainer<CClass>::Container<name, T>::member;

    template<class CClass>
    template<const char *const name, size_t size, typename T>
    void ClassMemberContainer<CClass>::Container<name, T[size]>::setFromPyObject(PyObject* pyobj){
        T val[] = *toCObject<T[size], false, PythonClassWrapper<T[size]>>(*pyobj);
        for (size_t i = 0; i < size; ++i)member[i] = val[i];
    }

    template<class CClass>
    template<const char *const name, size_t size, typename T>
    PyObject * ClassMemberContainer<CClass>::Container<name, T[size]>::call(PyObject *cls, PyObject *args, PyObject *kwds){
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
        return toPyObject<T>(member, false);
    }

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