//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__GLOBALMEMBERSEMANTICS_CPP__
#define __PYLLARS_INTERNAL__GLOBALMEMBERSEMANTICS_CPP__

#include "pyllars_globalmembersemantics.hpp"

namespace __pyllars_internal{

    template<typename T>
    PyObject *GlobalVariable::
    createGlobalVariable(const char *const name, const char *const tp_name,
                            typename extent_as_pointer<T>::type *variable, PyObject *module,
                            const size_t size) {
        if (!tp_name || !name || !module) {
            PyErr_SetString(PyExc_RuntimeError, "Null name or module on global variable creation");
            return nullptr;
        }
        static bool inited = false;
        PyTypeObject *type = &Container<T>::Type;
        char *new_name = new char[strlen(tp_name)];
        strcpy(new_name, tp_name);
        Container<T>::Type.tp_name = new_name;
        if (!inited && (PyType_Ready(type) < 0)) {
            throw "Unable to initialize python object for c function global variable wrapper";
        } else {
            Py_INCREF(type);
            static PyObject *emptyargs = PyTuple_New(0);
            inited = true;
            auto callable = (Container <T> *) PyObject_CallObject((PyObject *) type, emptyargs);
            if (!PyCallable_Check((PyObject *) callable)) {
                PyErr_SetString(PyExc_RuntimeError, "Python object is not callbable as expected!");
                return nullptr;
            }
            callable->member = (typename Container<T>::member_t) variable;
            callable->array_size = size;
            PyModule_AddObject(module, name, (PyObject *) callable);
            return (PyObject *) callable;
        }
    }


    template<typename T>
    int GlobalVariable::Container<T, typename std::enable_if<
            !std::is_array<T>::value && !std::is_const<T>::value>::type>::
    _init(PyObject *self, PyObject *args, PyObject *kwds) {
        //avoid compiler warnings (including reinterpret cast to avoid type-punned warning)
        (void) self;
        (void) args;
        (void) kwds;

        return 0;
    }

    template<typename T>
    PyObject *
    GlobalVariable::Container<T, typename std::enable_if<
            !std::is_array<T>::value && !std::is_const<T>::value>::type>::
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        (void) args;
        (void) kwds;
        return type->tp_alloc(type, 0);
    }

    template<typename T>
    PyObject *GlobalVariable::Container<T, typename std::enable_if<
            !std::is_array<T>::value && !std::is_const<T>::value>::type>::
    call(Container *callable, PyObject *args, PyObject *kwds) {
        (void) callable;
        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
        const ssize_t type_size = Sizeof<T_base>::value;
        const ssize_t array_size = type_size > 0 ? sizeof(*(callable->member)) / type_size : 1;
        if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "value")) {
            *(callable->member) = *toCObject<T, false, PythonClassWrapper<T> >(
                    *PyDict_GetItemString(kwds, "value"));
        } else if (kwds) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid parameters when getting/setting global variable");
            return nullptr;
        }
        return toPyObject<T>(*callable->member, false, array_size);
    }


    template<size_t size, typename T>
    int GlobalVariable::Container<T[size]>::
    _init(PyObject *self, PyObject *args, PyObject *kwds) {
        //avoid compiler warnings (including reinterpret cast to avoid type-punned warning)
        (void) self;
        (void) args;
        (void) kwds;

        return 0;
    }


    template<size_t size, typename T>
    PyObject *GlobalVariable::Container<T[size]>::
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        (void) args;
        (void) kwds;
        return type->tp_alloc(type, 0);
    }


    template<size_t size, typename T>
    PyObject *GlobalVariable::Container<T[size]>::
    call(Container *callable, PyObject *args, PyObject *kwds) {
        (void) callable;
        if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "value")) {
            T val[size] = *toCObject<T[size], true>(*PyDict_GetItemString(kwds, "value"));
            for (size_t i = 0; i < size; ++i) (*callable->member)[i] = val[i];
        } else if (kwds) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid parameters when getting/setting global variable");
            return nullptr;
        }
        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
        const ssize_t type_size = Sizeof<T_base>::value;
        const ssize_t array_size = type_size > 0 ? sizeof(*(callable->member)) / type_size : 1;
        return toPyObject<T[size], size>(callable->member, false, array_size);
    }


    template<size_t size, typename T>
    void GlobalVariable::Container<T[size]>::
    setFromPyObject(Container *callable, PyObject *pyobj) {
    }

    template<typename T>
    int GlobalVariable::Container<T[], typename std::enable_if<!std::is_const<T>::value>::type>::
    _init(PyObject *self, PyObject *args, PyObject *kwds) {
        //avoid compiler warnings (including reinterpret cast to avoid type-punned warning)
        (void) self;
        (void) args;
        (void) kwds;

        return 0;
    }

    template<typename T>
    PyObject *GlobalVariable::Container<T[], typename std::enable_if<!std::is_const<T>::value>::type>::
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        (void) args;
        (void) kwds;
        return type->tp_alloc(type, 0);
    }

    template<typename T>
    PyObject *GlobalVariable::Container<T[], typename std::enable_if<!std::is_const<T>::value>::type>::
    call(Container *callable, PyObject *args, PyObject *kwds) {
        (void) callable;
        if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "value")) {
            if (callable->array_size < 0) {
                PyErr_SetString(PyExc_RuntimeError, "Attempt to set whole array of unknown size");
                return nullptr;
            }
            T *val = *toCObject<T *, true, PythonClassWrapper<T *> >(
                    *PyDict_GetItemString(kwds, "value"));
            for (size_t i = 0; i < callable->array_size; ++i) (*callable->member)[i] = val[i];
        } else if (kwds && PyDict_Size(kwds) == 2 && PyDict_GetItemString(kwds, "value") &&
                   PyDict_GetItemString(kwds, "index") &&
                   PyLong_Check(PyDict_GetItemString(kwds, "index"))) {
            long i = PyLong_AsLong(PyDict_GetItemString(kwds, "index"));
            T *val = *toCObject<T *, false, PythonClassWrapper<T *> >(
                    *PyDict_GetItemString(kwds, "value"));
            (*callable->member)[i] = val[i];
        } else if (kwds && PyDict_Size(kwds) != 0) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid parameters when getting/setting global variable");
            return nullptr;
        }
        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
        const ssize_t type_size = Sizeof<T_base>::value;
        const ssize_t array_size = type_size > 0 ? sizeof(*(callable->member)) / type_size : 1;
        return toPyObject<T *>(*callable->member, false, array_size);
    }



    template<typename T>
    int GlobalVariable::Container<const T, typename std::enable_if<!std::is_array<T>::value>::type>::
    _init(PyObject *self, PyObject *args, PyObject *kwds) {
        //avoid compiler warnings (including reinterpret cast to avoid type-punned warning)
        (void) self;
        (void) args;
        (void) kwds;

        return 0;
    }

    template<typename T>
    PyObject *GlobalVariable::Container<const T, typename std::enable_if<!std::is_array<T>::value>::type>::
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        (void) args;
        (void) kwds;
        return type->tp_alloc(type, 0);
    }

    template<typename T>
    PyObject *GlobalVariable::Container<const T, typename std::enable_if<!std::is_array<T>::value>::type>::
    call(Container *callable, PyObject *args, PyObject *kwds) {
        (void) callable;
        if (kwds && PyDict_GetItemString(kwds, "value")) {
            PyErr_SetString(PyExc_RuntimeError, "Cannot set const global variable!");
            return nullptr;
        } else if (kwds && PyDict_Size(kwds) != 0) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid parameters when getting/setting global variable");
            return nullptr;
        }
        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
        const ssize_t type_size = Sizeof<T_base>::value;
        const ssize_t array_size = type_size > 0 ? sizeof(*(callable->member)) / type_size : 1;
        return toPyObject<T>(*callable->member, false, array_size);
    }



    template<size_t size, typename T>
    int GlobalVariable::Container<const T[size], void>::
    _init(PyObject *self, PyObject *args, PyObject *kwds) {
        //avoid compiler warnings (including reinterpret cast to avoid type-punned warning)
        (void) self;
        (void) args;
        (void) kwds;

        return 0;
    }

    template<size_t size, typename T>
    PyObject *GlobalVariable::Container<const T[size], void>::
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        (void) args;
        (void) kwds;
        return type->tp_alloc(type, 0);
    }

    template<size_t size, typename T>
    PyObject *GlobalVariable::Container<const T[size], void>::
    call(Container *callable, PyObject *args, PyObject *kwds) {
        (void) callable;
        if (kwds && PyDict_GetItemString(kwds, "value")) {
            PyErr_SetString(PyExc_RuntimeError, "Cannot set const global variable!");
            return nullptr;
        } else if (kwds && PyDict_Size(kwds) != 0) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid parameters when getting/setting global variable");
            return nullptr;
        }
        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
        const ssize_t type_size = Sizeof<T_base>::value;
        const ssize_t array_size = type_size > 0 ? sizeof(*(callable->member)) / type_size : 1;
        return toPyObject<T>(callable->member, false, array_size);
    }



    template<typename T>
    int GlobalVariable::Container<const T[], void>::
    _init(PyObject *self, PyObject *args, PyObject *kwds) {
        //avoid compiler warnings (including reinterpret cast to avoid type-punned warning)
        (void) self;
        (void) args;
        (void) kwds;

        return 0;
    }

    template<typename T>
    PyObject *GlobalVariable::Container<const T[], void>::
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        (void) args;
        (void) kwds;
        return type->tp_alloc(type, 0);
    }

    template<typename T>
    PyObject *GlobalVariable::Container<const T[], void>::
    call(Container *callable, PyObject *args, PyObject *kwds) {
        (void) callable;
        if (kwds && PyDict_GetItemString(kwds, "value")) {
            PyErr_SetString(PyExc_RuntimeError, "Cannot set const global variable!");
            return nullptr;
        } else if (kwds && PyDict_Size(kwds) != 0) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid parameters when getting/setting global variable");
            return nullptr;
        }
        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
        const ssize_t type_size = Sizeof<T_base>::value;
        const ssize_t array_size = type_size > 0 ? sizeof(*(callable->member)) / type_size : 1;
        return toPyObject<const T *>(*callable->member, false, array_size);
    }



    //Python definition of Type for this function wrapper
    template<typename T>
    PyTypeObject GlobalVariable::Container<T, typename std::enable_if<
            !std::is_array<T>::value && !std::is_const<T>::value>::type>::Type = {
            PyObject_HEAD_INIT(nullptr)
            0,                               /*ob_size*/
            nullptr,                         /*tp_name*/
            sizeof(GlobalVariable::Container<T, typename std::enable_if<
                    !std::is_array<T>::value && !std::is_const<T>::value>::type>),   /*tp_basicsize*/
            0,                               /*tp_itemsize*/
            nullptr,                         /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            nullptr,                         /*tp_getattr*/
            nullptr,                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            nullptr,                         /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            (ternaryfunc) call,                           /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT, /*tp_flags*/
            "Global variable object",  /* tp_doc */
            nullptr,                         /* tp_traverse */
            nullptr,                         /* tp_clear */
            nullptr,                         /* tp_richcompare */
            0,                                 /* tp_weaklistoffset */
            nullptr,                         /* tp_iter */
            nullptr,                         /* tp_iternext */
            nullptr,                         /* tp_methods */
            nullptr,                         /* tp_members */
            nullptr,                         /* tp_getset */
            nullptr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                               /* tp_dictoffset */
            _init,  /* tp_init */
            nullptr,                         /* tp_alloc */
            _new,     /* tp_new */
            nullptr,                         /*tp_free*/ //TODO: Implement a free??
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bass*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };


    //Python definition of Type for this function wrapper
    template<size_t size, typename T>
    PyTypeObject GlobalVariable::Container<T[size], void>::Type = {
            PyObject_HEAD_INIT(nullptr)
            0,                               /*ob_size*/
            nullptr,                         /*tp_name*/
            sizeof(GlobalVariable::Container<T[size], void>),   /*tp_basicsize*/
            0,                               /*tp_itemsize*/
            nullptr,                         /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            nullptr,                         /*tp_getattr*/
            nullptr,                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            nullptr,                         /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            (ternaryfunc) call,                           /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT, /*tp_flags*/
            "Global variable object",  /* tp_doc */
            nullptr,                         /* tp_traverse */
            nullptr,                         /* tp_clear */
            nullptr,                         /* tp_richcompare */
            0,                                 /* tp_weaklistoffset */
            nullptr,                         /* tp_iter */
            nullptr,                         /* tp_iternext */
            nullptr,                         /* tp_methods */
            nullptr,                         /* tp_members */
            nullptr,                         /* tp_getset */
            nullptr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                               /* tp_dictoffset */
            _init,  /* tp_init */
            nullptr,                         /* tp_alloc */
            _new,     /* tp_new */
            nullptr,                         /*tp_free*/ //TODO: Implement a free??
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bass*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };

    //Python definition of Type for this function wrapper
    template<typename T>
    PyTypeObject GlobalVariable::Container<const T, typename std::enable_if<!std::is_array<T>::value>::type>::Type = {
            PyObject_HEAD_INIT(nullptr)
            0,                               /*ob_size*/
            nullptr,                         /*tp_name*/
            sizeof(GlobalVariable::Container<const T, typename std::enable_if<!std::is_array<T>::value>::type>),   /*tp_basicsize*/
            0,                               /*tp_itemsize*/
            nullptr,                         /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            nullptr,                         /*tp_getattr*/
            nullptr,                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            nullptr,                         /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            (ternaryfunc) call,                           /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT, /*tp_flags*/
            "Global variable object",  /* tp_doc */
            nullptr,                         /* tp_traverse */
            nullptr,                         /* tp_clear */
            nullptr,                         /* tp_richcompare */
            0,                                 /* tp_weaklistoffset */
            nullptr,                         /* tp_iter */
            nullptr,                         /* tp_iternext */
            nullptr,                         /* tp_methods */
            nullptr,                         /* tp_members */
            nullptr,                         /* tp_getset */
            nullptr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                               /* tp_dictoffset */
            _init,  /* tp_init */
            nullptr,                         /* tp_alloc */
            _new,     /* tp_new */
            nullptr,                         /*tp_free*/ //TODO: Implement a free??
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bass*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };

    //Python definition of Type for this function wrapper
    template<size_t size, typename T>
    PyTypeObject GlobalVariable::Container<const T[size], void>::Type = {
            PyObject_HEAD_INIT(nullptr)
            0,                               /*ob_size*/
            nullptr,                         /*tp_name*/
            sizeof(GlobalVariable::Container<const T[size], void>),   /*tp_basicsize*/
            0,                               /*tp_itemsize*/
            nullptr,                         /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            nullptr,                         /*tp_getattr*/
            nullptr,                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            nullptr,                         /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            (ternaryfunc) call,                           /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT, /*tp_flags*/
            "Global variable object",  /* tp_doc */
            nullptr,                         /* tp_traverse */
            nullptr,                         /* tp_clear */
            nullptr,                         /* tp_richcompare */
            0,                                 /* tp_weaklistoffset */
            nullptr,                         /* tp_iter */
            nullptr,                         /* tp_iternext */
            nullptr,                         /* tp_methods */
            nullptr,                         /* tp_members */
            nullptr,                         /* tp_getset */
            nullptr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                               /* tp_dictoffset */
            _init,  /* tp_init */
            nullptr,                         /* tp_alloc */
            _new,     /* tp_new */
            nullptr,                         /*tp_free*/ //TODO: Implement a free??
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bass*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };


    //Python definition of Type for this function wrapper
    template<typename T>
    PyTypeObject GlobalVariable::Container<T[], typename std::enable_if<!std::is_const<T>::value>::type>::Type = {
            PyObject_HEAD_INIT(nullptr)
            0,                               /*ob_size*/
            nullptr,                         /*tp_name*/
            sizeof(GlobalVariable::Container<T[], void>),   /*tp_basicsize*/
            0,                               /*tp_itemsize*/
            nullptr,                         /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            nullptr,                         /*tp_getattr*/
            nullptr,                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            nullptr,                         /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            (ternaryfunc) call,                           /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT, /*tp_flags*/
            "Global variable object",  /* tp_doc */
            nullptr,                         /* tp_traverse */
            nullptr,                         /* tp_clear */
            nullptr,                         /* tp_richcompare */
            0,                                 /* tp_weaklistoffset */
            nullptr,                         /* tp_iter */
            nullptr,                         /* tp_iternext */
            nullptr,                         /* tp_methods */
            nullptr,                         /* tp_members */
            nullptr,                         /* tp_getset */
            nullptr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                               /* tp_dictoffset */
            _init,  /* tp_init */
            nullptr,                         /* tp_alloc */
            _new,     /* tp_new */
            nullptr,                         /*tp_free*/ //TODO: Implement a free??
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bass*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };

    //Python definition of Type for this function wrapper
    template<typename T>
    PyTypeObject GlobalVariable::Container<const T[], void>::Type = {
            PyObject_HEAD_INIT(nullptr)
            0,                               /*ob_size*/
            nullptr,                         /*tp_name*/
            sizeof(GlobalVariable::Container<const T[], void>),   /*tp_basicsize*/
            0,                               /*tp_itemsize*/
            nullptr,                         /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            nullptr,                         /*tp_getattr*/
            nullptr,                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            nullptr,                         /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            (ternaryfunc) call,                           /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT, /*tp_flags*/
            "Global variable object",  /* tp_doc */
            nullptr,                         /* tp_traverse */
            nullptr,                         /* tp_clear */
            nullptr,                         /* tp_richcompare */
            0,                                 /* tp_weaklistoffset */
            nullptr,                         /* tp_iter */
            nullptr,                         /* tp_iternext */
            nullptr,                         /* tp_methods */
            nullptr,                         /* tp_members */
            nullptr,                         /* tp_getset */
            nullptr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                               /* tp_dictoffset */
            _init,  /* tp_init */
            nullptr,                         /* tp_alloc */
            _new,     /* tp_new */
            nullptr,                         /*tp_free*/ //TODO: Implement a free??
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bass*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };

}

#endif