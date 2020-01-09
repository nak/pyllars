#ifndef __PYLLARS__FLOATING_POINT_IMPL_H__
#define __PYLLARS__FLOATING_POINT_IMPL_H__

#include "pyllars_reference.hpp"

namespace pyllars_internal{



/*
    template <typename T>
    int PythonClassWrapper<T&, void>::_init(PythonClassWrapper *self, PyObject *args, PyObject *kwds){
        // Must have at least one arg to reference or we ar bootstrapping
        const bool allow_null = args == NULL_ARGS();
        PyObject *arg = nullptr;
        if (!allow_null && PyTuple_Size(args) != 1){
            PyErr_SetString(PyExc_TypeError, "Must supply exactly  one object to reference when constructing wrapper to "
                                             "C-style reference");
            return -1;
        } else if (!allow_null) {
            arg = PyTuple_GetItem(args, 0);
            if (!PythonClassWrapper<T>::checkType(arg) &&
                !PyObject_TypeCheck(arg, getPyType())) {
                PyErr_SetString(PyExc_ValueError, "Type mismatch create wrapper to reference to C object");
                return -1;
            }
        }
        if (arg) {
            self->set_CObject(((PythonClassWrapper *) arg)->get_CObject());
            if (!self->get_CObject()) {
                PyErr_SetString(PyExc_ValueError, "Cannot create C reference to null object");
                return -1;
            }
            self->make_reference(arg);
        } else {
            Base::_init(self, args, kwds);
            self->set_CObject(nullptr);
        }
        return 0;
    }


    template<typename T>
    int PythonClassWrapper<T&, void>::initialize() {
        static bool inited = false;
        if (inited) return 0;

        PyTypeObject* Type = PythonBaseWrapper<T&>::getRawType();
        // Type->tp_basicsize = sizeof(PythonClassWrapper);
      //  Type->tp_itemsize = 0;
    //    Type->tp_dealloc = (destructor) _dealloc;
   //     Type->tp_getattr = (getattrfunc) getter();
    //    Type->tp_setattr = (setattrfunc) setter();
     //   Type->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES;
     //   Type->tp_init = (initproc) _init;
     //   Type->tp_free = _free;
    //    Type->tp_base = PythonClassWrapper_Base<T>::getRawType();

        if (Base::_initialize(*Type) == 0) {
            inited = true;
            return PyType_Ready(Base::getRawType());
        } else {
            PyErr_SetString(PyExc_SystemError, "Failed to initializer Python type wrapper to C object reference");
            return -1;
        }
    }


    template <typename T>
    PythonClassWrapper<T&, void> *
    PythonClassWrapper<T&, void>::fromCPointer(T& cobj, const ssize_t size, PyObject *referencing){
        PyTypeObject *type_ = getPyType();

        if (!type_ || !type_->tp_name) {
            PyErr_SetString(PyExc_RuntimeError, "Uninitialized type when creating object");
            return nullptr;
        }
        auto pyobj = (PythonClassWrapper *) PyObject_Call(reinterpret_cast<PyObject *>(type_), NULL_ARGS(), nullptr);
        if (pyobj) {
            pyobj->set_CObject(&cobj);
           // pyobj->_max = size-1;
        }
        return pyobj;
    }

*/
////////////////////////////////////////////////////////////////////////////
/*

    template <typename T>
    int PythonClassWrapper<T&&, void>::_init(PythonClassWrapper *self, PyObject *args, PyObject *kwds){
        // Must have at least one arg to reference or we ar bootstrapping
        PyObject *arg = nullptr;
        if (args && PyTuple_Size(args) != 1){
            PyErr_SetString(PyExc_TypeError, "Must supply exactly  one object to reference when constructing wrapper to "
                                             "C-style reference");
            return -1;
        } else if (args) {
            PyObject *arg = PyTuple_GetItem(args, 0);
            if (!PythonClassWrapper<T>::checkType(arg) &&
                !PyObject_TypeCheck(arg, PythonBaseWrapper<T>::getRawType())) {
                PyErr_SetString(PyExc_ValueError, "Type mismatch create wrapper to reference to C object");
                return -1;
            }
        }
        //call Base with no args, we will do the work here
        Base::_init(self, NULL_ARGS(), nullptr);
        if (arg) {
            self->set_CObject(((PythonClassWrapper *) arg)->get_CObject());
            if (!self->get_CObject()) {
                PyErr_SetString(PyExc_ValueError, "Cannot create C reference to null object");
                return -1;
            }
            self->make_reference(arg);
        }
        return 0;
    }


    template <typename T>
    PythonClassWrapper<T&&, void> *
    PythonClassWrapper<T&&, void>::fromCObject(T&& cobj, PyObject *referencing){
        PyTypeObject *type_ = PythonClassWrapper::getPyType();

        if (!type_ || !type_->tp_name) {
            PyErr_SetString(PyExc_RuntimeError, "Uninitialized type when creating object");
            return nullptr;
        }
        auto pyobj = (PythonClassWrapper *) PyObject_Call(reinterpret_cast<PyObject *>(type_), NULL_ARGS(), nullptr);
        if (pyobj) {
            pyobj->set_CObject(&cobj);
        }
        return pyobj;
    }


    template <typename T>
    typename PythonClassWrapper<T&&, void>::Initializer
            PythonClassWrapper<T&&, void>::initializer;

    template <typename T>
    typename PythonClassWrapper<T&, void>::Initializer
            PythonClassWrapper<T&, void>::initializer;
            */

}
#endif
