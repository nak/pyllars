//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__GLOBALMEMBERSEMANTICS_CPP__
#define __PYLLARS_INTERNAL__GLOBALMEMBERSEMANTICS_CPP__

#include "pyllars_globalmembersemantics.hpp"
#include "pyllars_pointer.impl.hpp"


namespace __pyllars_internal{

    template<typename T>
    PyObject *
    GlobalVariable::
    createGlobalVariable(const char *const name,
                            T *variable,
                            PyObject *module,
                            const size_t size) {
        if (!name || !module) {
            PyErr_SetString(PyExc_RuntimeError, "Null name or module on global variable creation");
            return nullptr;
        }
        static bool inited = false;
        PyTypeObject *type = &Container<T>::Type;
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
            PyModule_AddObject(module, name, reinterpret_cast<PyObject *>(callable));
            return reinterpret_cast<PyObject *>(callable);
        }
    }


    template<typename T>
    int
    GlobalVariable::
    Container<T>::
    _init(PyObject *self, PyObject *args, PyObject *kwds) {
        //avoid compiler warnings
        (void) self;
        (void) args;
        (void) kwds;

        return 0;
    }

    template<typename T>
    PyObject *
    GlobalVariable::Container<T>::
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        (void) args;
        (void) kwds;
        return type->tp_alloc(type, 0);
    }

    template<typename T>
    PyObject *GlobalVariable::Container<T>::
    call(Container *callable, PyObject *args, PyObject *kwds) {
        if constexpr(!std::is_array<T>::value && !std::is_const<T>::value) {
            try {
                typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
                const ssize_t type_size = Sizeof<T_base>::value;
                const ssize_t array_size = type_size > 0 ? sizeof(*(callable->member)) / type_size : 1;
                if (((kwds ? PyDict_Size(kwds) : 0) + PyTuple_Size(args)) > 1) {
                    PyErr_SetString(PyExc_TypeError, "Invalid parameters when getting/setting global variable");
                    return nullptr;
                } else if (kwds && PyDict_Size(kwds) == 1) {
                    PyObject *const v = PyDict_GetItemString(kwds, "value");
                    if (v) {
                        *(callable->member) = toCArgument<T>(*v).value();
                    } else {
                        PyErr_SetString(PyExc_TypeError, "Invalid parameters when getting/setting global variable");
                        return nullptr;
                    }

                } else if (PyTuple_Size(args) == 1) {
                    *(callable->member) = toCArgument<T>(*PyTuple_GetItem(args, 0)).value();
                }
                return toPyObject<T&>(*callable->member, array_size);
            } catch (const char *msg) {
                PyErr_SetString(PyExc_TypeError, msg);
                return nullptr;
            }
        } else if constexpr (std::is_array<T>::value && ArraySize<T>::size > 0){
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
            if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "value")) {
                T_base val[ArraySize<T>::size];
                auto cval = *toCArgument<T_base, true, PythonClassWrapper<T> >(*PyDict_GetItemString(kwds, "value"));
                //for(unsigned int i = 0; i < size; ++i)
                //    val[i] = cval[i];
                for (size_t i = 0; i < ArraySize<T>::size; ++i) (*callable->member)[i] = cval[i];
            } else if (kwds) {
                PyErr_SetString(PyExc_RuntimeError, "Invalid parameters when getting/setting global variable");
                return nullptr;
            }
            return toPyObject<T>(*callable->member, ArraySize<T>::size);
        } else if constexpr (std::is_array<T>::value){
            typedef typename extent_as_pointer<T>::type T_ptr;
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
            if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "value")) {
                if (callable->array_size < 0) {
                    PyErr_SetString(PyExc_RuntimeError, "Attempt to set whole array of unknown size");
                    return nullptr;
                }
                T_ptr val = *toCArgument<T_ptr, true, PythonClassWrapper<T_ptr> >(*PyDict_GetItemString(kwds, "value"));
                for (size_t i = 0; i < callable->array_size; ++i) (*callable->member)[i] = val[i];
            } else if (kwds && PyDict_Size(kwds) == 2 && PyDict_GetItemString(kwds, "value") &&
                       PyDict_GetItemString(kwds, "index") &&
                       PyLong_Check(PyDict_GetItemString(kwds, "index"))) {
                long i = PyLong_AsLong(PyDict_GetItemString(kwds, "index"));
                T_ptr val = *toCArgument<T *, false, PythonClassWrapper<T_ptr> >(*PyDict_GetItemString(kwds, "value"));
                (*callable->member)[i] = val[i];
            } else if (kwds && PyDict_Size(kwds) != 0) {
                PyErr_SetString(PyExc_RuntimeError, "Invalid parameters when getting/setting global variable");
                return nullptr;
            }
            const ssize_t type_size = Sizeof<T_base>::value;
            const ssize_t array_size = type_size > 0 ? sizeof(*(callable->member)) / type_size : 1;
            return toPyObject<T&>(*callable->member, array_size);
        } else {
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
            return toPyObject<T&>(*callable->member, array_size);
        }
    }

    //Python definition of Type for this function wrapper
    template<typename T>
    PyTypeObject GlobalVariable::Container<T>::Type = {
        #if PY_MAJOR_VERSION == 3
                PyVarObject_HEAD_INIT(nullptr, 0)
        #else
                PyObject_HEAD_INIT(nullptr)
                0,                               /*ob_size*/
        #endif
            type_name<T>(),                         /*tp_name*/
            sizeof(GlobalVariable::Container<T>),   /*tp_basicsize*/
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
