#ifndef __PYLLARS__FLOATING_POINT_IMPL_H__
#define __PYLLARS__FLOATING_POINT_IMPL_H__

#include "pyllars_reference.hpp"

namespace __pyllars_internal{

    template <typename T>
    int PythonClassWrapper<T&, void>::_init(PythonClassWrapper *self, PyObject *args, PyObject *kwds){
        // Must have at least one arg to reference or we ar bootstrapping

        const bool allow_no_arguments = kwds && PyDict_GetItemString(kwds, "__internal_allow_null");
        if (PyTuple_Size(args) != allow_no_arguments?0:1){
            PyErr_SetString(PyExc_TypeError, "Must supply exactly  one object to reference when constructing wrapper to "
                                             "C-style reference");
            return -1;
        }
        PyObject *arg = nullptr;
        if (!allow_no_arguments) {
            PyObject *arg = PyTuple_GetItem(args, 0);
            if (!PythonClassWrapper<T>::checkType(arg) &&
                !PyObject_TypeCheck(arg, &_Type)) {
                PyErr_SetString(PyExc_ValueError, "Type mismatch create wrapper to reference to C object");
                return -1;
            }
        }
        static PyObject* noargs = PyTuple_New(0);
        //call Base with no args, we will do the work here
	static PyObject *_kwds = PyDict_New();
        PyDict_SetItemString(_kwds, "__internal_allow_null", Py_True);
        Base::_init(self, noargs, _kwds);
        if (arg) {
            self->_CObject = ((PythonClassWrapper *) arg)->get_CObject();
            if (!self->_CObject) {
                PyErr_SetString(PyExc_ValueError, "Cannot create C reference to null object");
                return -1;
            }
            self->make_reference(arg);
        }
        return 0;
    }

    template <typename T>
    T *PythonClassWrapper<T&, void>::get_CObject() const{
        return PythonClassWrapper<T>::get_CObject();
    }

    template<typename T>
    int PythonClassWrapper<T&, void>::initialize() {
        if (Base::initialize() == 0) {
            return PyType_Ready(&_Type);
        } else {
            PyErr_SetString(PyExc_SystemError, "Failed to initializer Python type wrapper to C object reference");
            return -1;
        }
    }

    template<typename T>
    bool PythonClassWrapper<T&, void>::checkType(PyObject *obj){
        return PyObject_TypeCheck(obj, &_Type);
    }

    template <typename T>
    PythonClassWrapper<T&, void> *
    PythonClassWrapper<T&, void>::fromCObject(T& cobj, PyObject *referencing){
        static PyObject *kwds = PyDict_New();
        static PyObject *emptyargs = PyTuple_New(0);
        PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);
        PyTypeObject *type_ = getPyType();

        if (!type_ || !type_->tp_name) {
            PyErr_SetString(PyExc_RuntimeError, "Uninitialized type when creating object");
            return nullptr;
        }
        auto pyobj = (PythonClassWrapper *) PyObject_Call(reinterpret_cast<PyObject *>(type_), emptyargs, kwds);
        if (pyobj) {
            pyobj->_CObject = &cobj;
        }
        return pyobj;
    }


    template <typename T>
    PythonClassWrapper<T&, void> *
    PythonClassWrapper<T&, void>::fromCPointer(T& cobj, const ssize_t size, PyObject *referencing){
        static PyObject *kwds = PyDict_New();
        static PyObject *emptyargs = PyTuple_New(0);
        PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);
        PyTypeObject *type_ = getPyType();

        if (!type_ || !type_->tp_name) {
            PyErr_SetString(PyExc_RuntimeError, "Uninitialized type when creating object");
            return nullptr;
        }
        auto pyobj = (PythonClassWrapper *) PyObject_Call(reinterpret_cast<PyObject *>(type_), emptyargs, kwds);
        if (pyobj) {
            pyobj->_CObject = &cobj;
            pyobj->_max = size-1;
        }
        return pyobj;
    }
    template<typename T>
    PyTypeObject PythonClassWrapper<T&, void>::_Type = {
        #if PY_MAJOR_VERSION == 3
        PyVarObject_HEAD_INIT(NULL, 0)
        #else
        PyObject_HEAD_INIT(nullptr)
                    0,                         /*ob_size*/
        #endif
        __pyllars_internal::type_name<T&>(),             /*tp_name*/ /*filled on init*/
        sizeof(PythonClassWrapper),             /*tp_basicsize*/
        0,                         /*tp_itemsize*/
        (destructor) PythonClassWrapper::_dealloc, /*tp_dealloc*/
        nullptr,                         /*tp_print*/
        getter(),                         /*tp_getattr*/
        setter(),                         /*tp_setattr*/
        nullptr,                         /*tp_compare*/
        nullptr,                         /*tp_repr*/
        new PyNumberMethods{0},          /*tp_as_number*/
        nullptr,                         /*tp_as_sequence*/
        nullptr,                         /*tp_as_mapping*/
        nullptr,                         /*tp_hash */
        nullptr,                         /*tp_call*/
        nullptr,                         /*tp_str*/
        nullptr,                         /*tp_getattro*/
        nullptr,                         /*tp_setattro*/
        nullptr,                         /*tp_as_buffer*/
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES, /*tp_flags*/
        "PythonClassWrapper object",           /* tp_doc */
        nullptr,                       /* tp_traverse */
        nullptr,                       /* tp_clear */
        nullptr,                       /* tp_richcompare */
        0,                               /* tp_weaklistoffset */
        nullptr,                       /* tp_iter */
        nullptr,                       /* tp_iternext */
        nullptr,             /* tp_methods */
        nullptr,             /* tp_members */
        nullptr,                         /* tp_getset */
        &PythonClassWrapper<T>::_Type,                         /* tp_base */
        nullptr,                         /* tp_dict */
        nullptr,                         /* tp_descr_get */
        nullptr,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        (initproc) PythonClassWrapper::_init,  /* tp_init */
        nullptr,                         /* tp_alloc */
        PythonClassWrapper::_new,             /* tp_new */
        _free,                         /*tp_free*/
        nullptr,                         /*tp_is_gc*/
        nullptr,                         /*tp_bases*/
        nullptr,                         /*tp_mro*/
        nullptr,                         /*tp_cache*/
        nullptr,                         /*tp_subclasses*/
        nullptr,                          /*tp_weaklist*/
        nullptr,                          /*tp_del*/
        0,                          /*tp_version_tag*/
        };

////////////////////////////////////////////////////////////////////////////

    template<typename T>
    bool PythonClassWrapper<T&&, void>::checkType(PyObject *obj){
        return PyObject_TypeCheck(obj, &_Type);
    }

    template <typename T>
    int PythonClassWrapper<T&&, void>::_init(PythonClassWrapper *self, PyObject *args, PyObject *kwds){
        // Must have at least one arg to reference or we ar bootstrapping

        const bool allow_no_arguments = kwds && PyDict_GetItemString(kwds, "__internal_allow_null");
        if (PyTuple_Size(args) != allow_no_arguments?0:1){
            PyErr_SetString(PyExc_TypeError, "Must supply exactly  one object to reference when constructing wrapper to "
                                             "C-style reference");
            return -1;
        }
        PyObject *arg = nullptr;
        if (!allow_no_arguments) {
            PyObject *arg = PyTuple_GetItem(args, 0);
            if (!PythonClassWrapper<T>::checkType(arg) &&
                !PyObject_TypeCheck(arg, &_Type)) {
                PyErr_SetString(PyExc_ValueError, "Type mismatch create wrapper to reference to C object");
                return -1;
            }
        }
        static PyObject* noargs = PyTuple_New(0);
        //call Base with no args, we will do the work here
	static PyObject *_kwds = PyDict_New();
        PyDict_SetItemString(_kwds, "__internal_allow_null", Py_True);
        Base::_init(self, noargs, _kwds);
        if (arg) {
            self->_CObject = ((PythonClassWrapper *) arg)->get_CObject();
            if (!self->_CObject) {
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
        static PyObject *kwds = PyDict_New();
        static PyObject *emptyargs = PyTuple_New(0);
        PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);
        PyTypeObject *type_ = getPyType();

        if (!type_ || !type_->tp_name) {
            PyErr_SetString(PyExc_RuntimeError, "Uninitialized type when creating object");
            return nullptr;
        }
        auto pyobj = (PythonClassWrapper *) PyObject_Call(reinterpret_cast<PyObject *>(type_), emptyargs, kwds);
        if (pyobj) {
            pyobj->_CObject = &cobj;
        }
        return pyobj;
    }


    template<typename T>
    PyTypeObject PythonClassWrapper<T&&, void>::_Type = {
#if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(nullptr)
                    0,                         /*ob_size*/
#endif
            __pyllars_internal::type_name<T&&>(),             /*tp_name*/ /*filled on init*/
            sizeof(PythonClassWrapper),             /*tp_basicsize*/
            0,                         /*tp_itemsize*/
            (destructor) PythonClassWrapper::_dealloc, /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            (getattrfunc) getter(),                         /*tp_getattr*/
            (setattrfunc) setter(),                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            new PyNumberMethods{0},          /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            nullptr,                         /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES, /*tp_flags*/
            "PythonClassWrapper object",           /* tp_doc */
            nullptr,                       /* tp_traverse */
            nullptr,                       /* tp_clear */
            nullptr,                       /* tp_richcompare */
            0,                               /* tp_weaklistoffset */
            nullptr,                       /* tp_iter */
            nullptr,                       /* tp_iternext */
            nullptr,             /* tp_methods */
            nullptr,             /* tp_members */
            nullptr,                         /* tp_getset */
            &PythonClassWrapper<T>::_Type,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            (initproc) PythonClassWrapper::_init,  /* tp_init */
            nullptr,                         /* tp_alloc */
            PythonClassWrapper::_new,             /* tp_new */
            _free,                         /*tp_free*/
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bases*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };


}
#endif
