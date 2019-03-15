//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__POINTER_CPP_
#define __PYLLARS_INTERNAL__POINTER_CPP_

#include "pyllars_pointer.hpp"

#include "pyllars_classwrapper.impl.hpp"

namespace __pyllars_internal {

    template<typename T>
    Py_ssize_t
    PythonPointerWrapperBase<T>::_size(PyObject *self) {
        const Py_ssize_t length = ((PythonPointerWrapperBase *) self)->_arraySize;
        return length > 0 ? length : 1;
    }

    template<typename T>
    PyObject *
    PythonPointerWrapperBase<T>::
    _concat(PyObject *self, PyObject *other) {
        PythonClassWrapper<T> *self__ = (PythonClassWrapper<T> *) self;
        if (PythonClassWrapper<T>::checkType(other) ||
            NonConstWrapper::checkType(other) ||
            ConstWrapper::checkType(other)) {
            PythonPointerWrapperBase *self_ = (PythonPointerWrapperBase *) self;
            PythonPointerWrapperBase *other_ = (PythonPointerWrapperBase *) other;
            if (self_->_arraySize <= 0 || other_->_arraySize <= 0) {
                PyErr_SetString(PyExc_TypeError, "Cannot concatenate array(s) of unknown size");
                return nullptr;
            }
            const ssize_t new_size = self_->_arraySize + other_->_arraySize;
            char *raw_storage = new char[new_size * Sizeof<T_base>::value];
            T_base *values = (T_base *) raw_storage;
            for (size_t i = 0; i < (size_t) self_->_arraySize; ++i) {
                T &cobj = *self__->get_CObject();
                ObjectLifecycleHelpers::Copy<T_base>::inplace_copy(
                        values, i, ObjectLifecycleHelpers::Copy<T_base>::new_copy2(
                                ObjectLifecycleHelpers::Array<T_base *>::at(cobj, i))->ptr());
            }
            self_->_arraySize = new_size;
        }
        return nullptr;
    }


    template<typename T>
    int
    PythonPointerWrapperBase<T>::
    _set_item(PyObject *self, Py_ssize_t index, PyObject *obj) {
        try {
            PythonPointerWrapperBase *self_ = (PythonPointerWrapperBase *) self;
            PythonClassWrapper<T> *self__ = (PythonClassWrapper<T> *) self;
            if (index < 0 && self_->_arraySize > 0) { index = self_->_arraySize + index + 1; };
            if (index < 0 || index > self_->_arraySize) { return -1; };
            if (!PythonClassWrapper<T_base>::checkType(obj) && !PythonClassWrapper<const T_base>::checkType(obj)) {
                PyErr_SetString(PyExc_TypeError, "Setting item from incompatible type");
                return -1;
            }
            PythonClassWrapper<const T_base> *obj_ = (PythonClassWrapper<const T_base> *) obj;
            ObjectLifecycleHelpers::Copy<T_base>::inplace_copy(*(self__->get_CObject()), index,
                                                               obj_->get_CObject()//,
                    //self_->_raw_storage != nullptr
            );
            return 0;
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return -1;
        }
    }

    template<typename T>
    int
    PythonPointerWrapperBase<T>::
    _contains(PyObject *self, PyObject *obj) {
        return -1;
    }


    template<typename T>
    PyObject *
    PythonPointerWrapperBase<T>::
    _get_item(PyObject *self, Py_ssize_t index) {
        typedef typename remove_all_pointers<T>::type T_bare;
        PythonPointerWrapperBase *self_ = reinterpret_cast<PythonPointerWrapperBase *>(self);
        if (!self_ || !self_->_CObject) {
            PyErr_SetString(PyExc_RuntimeError, "Null pointer dereference");
            return nullptr;
        }
        try {
            if (index < 0 && self_->_max >= 0) {
                index = self_->_max + index - 1;
            }
            if (index < 0 || (self_->_max >= 0 && index > self_->_max)) {
                PyErr_SetString(PyExc_IndexError, "Index out of range");
                return nullptr;
            }
            ssize_t element_array_size = std::extent<T_base>::value;
            if (element_array_size == 0) { element_array_size = UNKNOWN_SIZE; }
            if (self_->_referenced_elements) {
                PythonPointerWrapperBase *item = (PythonPointerWrapperBase *) PyList_GetItem(
                        self_->_referenced_elements,
                        index);
                element_array_size = item ? item->_arraySize : element_array_size;
            }

            PyObject *result;
            if (self_->_depth > 2) {
                T_bare **&var = ObjectLifecycleHelpers::Array<T_bare ***>::at((T_bare ***) *self_->_CObject->ptr(),
                                                                              index);
                PythonClassWrapper<T_bare **> *res = PythonClassWrapper<T_bare **>::createPy(element_array_size, var,
                                                                                             ContainmentKind::CONSTRUCTED);

                if (res) {
                    res->_depth = self_->_depth - 1;
                }
                result = reinterpret_cast<PyObject *>(res);
            } else if (self_->_depth == 2) {
                T_bare *&var = ObjectLifecycleHelpers::Array<T_bare **>::at((T_bare **) *self_->_CObject->ptr(), index);
                PythonClassWrapper<T_bare *> *res = PythonClassWrapper<T_bare *>::createPy(element_array_size, var,
                                                                                           ContainmentKind::CONSTRUCTED);
                if (res) {
                    ((PythonPointerWrapperBase *) res)->_depth = self_->_depth - 1;
                }
                result = reinterpret_cast<PyObject *>(res);
            } else {
                T_bare &var = ObjectLifecycleHelpers::Array<T_bare *>::at((T_bare *) *self_->_CObject->ptr(), index);
                PythonClassWrapper<T_bare> *res = (PythonClassWrapper<T_bare> *) PythonClassWrapper<T_bare>::createPy(
                        element_array_size, var,
                        ContainmentKind::BY_REFERENCE, self);

                result = reinterpret_cast<PyObject *>(res);
            }
            return result;
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }

    template<typename T>
    PyObject *
    PythonPointerWrapperBase<T>::
    _at(PyObject *self, PyObject *args, PyObject *kwds) {
        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
        try {
            PyObject *set_value = nullptr;
            if (kwds && PyDict_Size(kwds) == 1) {
                set_value = PyDict_GetItemString(kwds, "set_value");
                if (!set_value) {
                    PyErr_BadArgument();
                    return nullptr;
                }
                Py_INCREF(set_value);
                PyDict_DelItemString(kwds, "set_value");
            }
            if ((kwds && PyDict_Size(kwds) != 0) || !args || PyTuple_Size(args) != 1 ||
                !(PyLong_Check(PyTuple_GetItem(args, 0)) || PyInt_Check(PyTuple_GetItem(args, 0)))) {
                PyErr_BadArgument();
                if (set_value) { Py_DECREF(set_value); }
                return nullptr;
            }
            PyObject *pyindex = PyTuple_GetItem(args, 0);
            Py_ssize_t index = -1;
            if (PyLong_Check(pyindex)) {
                index = PyLong_AsLong(PyTuple_GetItem(args, 0));
            } else {
                index = PyInt_AsLong(PyTuple_GetItem(args, 0));
            }
            if (set_value) {
                _set_item(self, index, set_value);
                Py_DECREF(set_value);
            }
            return _get_item(self, index);
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }

    template<typename T>
    int PythonPointerWrapperBase<T>::
    _initialize(PyTypeObject &Type) {
        static bool initialized = false;
        if (initialized) return 0;
        initialized = true;
        if (PyType_Ready(&CommonBaseWrapper::_BaseType) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
            return -1;
        }
        Type.tp_base = &CommonBaseWrapper::_BaseType;

        if (PyType_Ready(&Type) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
            return -1;
        }
        _seqmethods.sq_length = _size;
        _seqmethods.sq_concat = _concat;
        _seqmethods.sq_repeat = _repeat;
        _seqmethods.sq_item = _get_item;
        _seqmethods.sq_ass_item = _set_item;
        _seqmethods.sq_contains = _contains;
        _seqmethods.sq_inplace_concat = _inplace_concat;
        _seqmethods.sq_inplace_repeat = _inplace_repeat;

        Type.tp_as_sequence = &_seqmethods;

        Py_INCREF(&Type);
        return 0;
    }


    template<typename T>
    PythonPointerWrapperBase <T> *
    PythonPointerWrapperBase<T>::
    _createPy(PyTypeObject &Type,
              const size_t arraySize,
              T &cobj,
              const ContainmentKind containmentKind,
              PyObject *referencing) {
        if (containmentKind == ContainmentKind::ALLOCATED && arraySize < 0) {
            PyErr_SetString(PyExc_SystemError, "System error: conflicting params to call to _createPy");
            return nullptr;
        }
        if (_initialize(Type) != 0) {
            PyErr_SetString(PyExc_SystemError, "System error: failed to initialize type");
            return nullptr;
        }
        static PyObject *args = PyTuple_New(0);
        static PyObject *kwds = PyDict_New();
        PythonPointerWrapperBase *pyobj = (PythonPointerWrapperBase *) PyObject_Call((PyObject *) &Type, args, kwds);
        if (!pyobj) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to create Python Object");
            return nullptr;
        }
        assert(pyobj->get_CObject() == nullptr);
        pyobj->_arraySize = 0;
        switch (containmentKind) {
            case ContainmentKind::ALLOCATED:
                throw "System error";
                //pyobj->_CObject = new ObjectContainerAllocated<T>(&cobj, arraySize > 0);
                //pyobj->_arraySize = arraySize;
                break;
            case ContainmentKind::CONSTRUCTED:
                pyobj->_CObject = new ObjectContainerReference<T>(cobj);
                break;
            case ContainmentKind::CONSTRUCTED_IN_PLACE:
                //pyobj->_CObject = new ObjectContainerInPlace<T, T>(cobj);
                throw "Cannot construct new object in place with no memory address";
                break;
            case ContainmentKind::BY_REFERENCE:
                pyobj->_CObject = new ObjectContainerReference<T>(cobj);
                break;
        }
        if (arraySize > 0) { pyobj->_max = arraySize - 1; }
        if (referencing) pyobj->make_reference(referencing);
        return pyobj;
    }

    template<typename T>
    int
    PythonPointerWrapperBase<T>::
    _initbase(PythonPointerWrapperBase *self, PyObject *args, PyObject *kwds, PyTypeObject *pytypeobj) {
        int status = -1;
        if (!self) return status;
        self->_referenced = nullptr;
        self->_max = last;
        if (((PyObject *) self)->ob_type->tp_base && Base::TypePtr->tp_init) {
            static PyObject *empty = PyTuple_New(0);
            Base::TypePtr->tp_init((PyObject *) &self->baseClass, empty, nullptr);
        }
        if (kwds && PyDict_Size(kwds) > 1) {
            PyErr_SetString(PyExc_TypeError, "Unexpected keyword argument(s) in Pointer cosntructor");
            status = -1;
            goto onerror;
        } else if (kwds && PyDict_Size(kwds) == 1) {
            PyObject *sizeItem;
            if ((sizeItem = PyDict_GetItemString(kwds, "size"))) {
                if (last > 0) {
                    PyErr_SetString(PyExc_RuntimeError, "Attempt to dynamically set size on fixed-size array");
                    status = -1;
                    goto onerror;
                }
                if (PyLong_Check(sizeItem)) {
                    //if size arg is truly an integer and is positive or zero, set the property here (and if not a fixed-size array)
                    self->_max = PyLong_AsLongLong(sizeItem) - 1;
                    if (self->_max < 0) {
                        PyErr_SetString(PyExc_TypeError, "Invalid negative size value in Pointer constructor");
                        status = -1;
                        goto onerror;
                    }
                } else {
                    PyErr_SetString(PyExc_TypeError,
                                    "Invalid type for size keyword argument in Pointer constructor");
                    status = -1;
                    goto onerror;
                }
            } else {
                PyErr_SetString(PyExc_TypeError, "Unexpected keyword argument in Pointer constructor");
                status = -1;
                goto onerror;
            }
        }
        //if have an argument, set pointer value, otherwise set to nullptr
        if (args && PyTuple_Size(args) > 1) {
            PyErr_SetString(PyExc_TypeError, "Excpect only one object in Pointer constructor");
            status = -1;
        } else if (args && PyTuple_Size(args) == 1) {
            // we are asked to make a new pointer from an existing object:
            PyObject *pyobj = PyTuple_GetItem(args, 0);
            if (!pyobj) {
                self->_CObject = nullptr;
                return 0;
            }
            if (PyObject_TypeCheck(pyobj, pytypeobj)) {
                // if this is an instance of a basic class:
                self->_CObject = reinterpret_cast<PythonPointerWrapperBase *>(pyobj)->_CObject;
                self->_referenced = pyobj;
                Py_INCREF(pyobj);
            } else if (PyUnicode_Check(pyobj) && (is_bytes_like<T>::value || is_c_string_like<T>::value)) {
                typedef typename std::remove_const<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::type T_base;
                /*auto inval =  PyUnicode_AsUTF8(pyobj);
                auto dup = new char[strlen(inval)+1];
                strcpy(dup, inval);*/
                self->_CObject = (ObjectContainerPyReference<T> *) new ObjectContainerPyReference<const char *>(pyobj,
                                                                                                                PyUnicode_AsUTF8);
            } else {
                PyErr_SetString(PyExc_TypeError, "Mismatched types when assigning pointer");
                status = ERROR_TYPE_MISMATCH;
                goto onerror;
            }

            status = self->_CObject ? 0 : -1;
        } else {
            self->_CObject = nullptr;
            status = 0;
        }
        onerror:
        return status;
    }

    template<typename T>
    T *PythonPointerWrapperBase<T>::
    _get_CObject() {
        return _CObject ? _CObject->ptr() : nullptr;
    }

    template<typename T>
    void
    PythonPointerWrapperBase<T>::
    _free(PythonPointerWrapperBase *self) {
        if (self->_referenced) {
            Py_XDECREF(self->_referenced);
            self->_referenced = nullptr;
        }
        delete self->_CObject;
        self->_CObject = nullptr;
    }

    template<typename T>
    void
    PythonPointerWrapperBase<T>::
    _dealloc(PyObject *self_) {

        PythonPointerWrapperBase *self = (PythonPointerWrapperBase *) self_;
        //TODO: track dynamically allocated content and free if so
        if (self != nullptr) {
            _free(self);
#if PY_MAJOR_VERSION == 3
            self->baseClass.ob_base.ob_type->tp_free((PyObject *) self);
#else
            self->baseClass.ob_type->tp_free((PyObject *) self);
#endif

        }
    }

    template<typename T>
    PyObject *
    PythonPointerWrapperBase<T>::
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        (void) args;
        (void) kwds;
        PythonPointerWrapperBase *self;
        self = (PythonPointerWrapperBase *) type->tp_alloc(type, 0);
        if (self != nullptr) {
            self->_CObject = nullptr;
        }
        return (PyObject *) self;

    }

    template<typename T>
    Py_ssize_t
    PythonPointerWrapperBase<T>::
    get_array_index(PythonPointerWrapperBase *const self, PyObject *args, PyObject *kwargs) {
        static const char *kwlist[] = {"index", nullptr};
        static constexpr Py_ssize_t INVALID_INDEX = -1;
        long index = -1;

        if (!PyArg_ParseTupleAndKeywords(args, kwargs, "l", (char **) kwlist, &index)) {
            PyErr_SetString(PyExc_TypeError, "Unable to parse index");
            return INVALID_INDEX;
        }

        index = (index < 0 && self->_max >= 0) ? self->_max + index + 1 : index;
        if ((self->_max >= 0 && index > self->_max) || index < 0) {
            PyErr_SetString(PyExc_IndexError, "Index out of bounds");
            return INVALID_INDEX;
        }
        return index;
    }


    template<typename T>
    PySequenceMethods PythonPointerWrapperBase<T>::_seqmethods;


    ///////////////////////////


    template<typename T>
    int
    PythonClassWrapper<T, typename std::enable_if<
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value > 1) &&
            sizeof(typename extent_as_pointer<T>::type) == sizeof(typename base_type<T>::type *)>::type>::
    _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
        if (!self) { return -1; }

        self->_arraySize = UNKNOWN_SIZE;
        self->_referenced_elements = nullptr;
        self->_referenced = nullptr;
        PyTypeObject *const coreTypePtr = PythonClassWrapper<typename core_type<T>::type>::getPyType();
        self->template populate_type_info<T>(&checkType, coreTypePtr);
        int result = Base::_initbase(self, args, kwds, &Type);

        if (result == ERROR_TYPE_MISMATCH && (ptr_depth<T>::value == 1) &&
            (PythonClassWrapper<const char *>::checkType((PyObject *) self) ||
             PythonClassWrapper<const char *const>::checkType((PyObject *) self)
            )) {
            PyObject *arg = PyTuple_GetItem(args, 0);
            const char *const s = PyString_AsString(arg);

            if (s) {
                Py_INCREF(arg);
                self->_referenced = arg;
                self->_arraySize = 0;
                self->_allocated = false;
                self->_CObject = (ObjectContainerReference<T> *) new ObjectContainerReference<const char *const>(s);
            }
            PyErr_Clear();
            return 0;
        }
        return result;
    }

    template<typename T>
    PythonClassWrapper<T, typename std::enable_if<!std::is_function<typename std::remove_pointer<T>::type>::value &&
                                                  (std::is_pointer<T>::value || std::is_array<T>::value) &&
                                                  (ptr_depth<T>::value > 1) &&
                                                  sizeof(typename extent_as_pointer<T>::type) ==
                                                  sizeof(typename base_type<T>::type *)>::type> *
    PythonClassWrapper<T, typename std::enable_if<!std::is_function<typename std::remove_pointer<T>::type>::value &&
                                                  (std::is_pointer<T>::value || std::is_array<T>::value) &&
                                                  (ptr_depth<T>::value > 1) &&
                                                  sizeof(typename extent_as_pointer<T>::type) ==
                                                  sizeof(typename base_type<T>::type *)>::type>::
    createPyUsingBytePool(const size_t size, std::function<void(void *, size_t)>& constructor) {
        if (PythonPointerWrapperBase<T>::_initialize(Type) != 0) {
            PyErr_SetString(PyExc_SystemError, "System error: failed to initialize type");
            return nullptr;
        }
        static PyObject *args = PyTuple_New(0);
        static PyObject *kwds = PyDict_New();
        PyTypeObject *type = getPyType();
        if (!type){
            PyErr_SetString(PyExc_RuntimeError, "Unable to initializer Python type to c-pointer-wrapper");
        return nullptr;
        }
        PythonClassWrapper *pyobj = (PythonClassWrapper *) PyObject_Call((PyObject *) getPyType(), args, kwds);
        if (!pyobj) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to create Python Object");
            return nullptr;
        }
        assert(pyobj->get_CObject() == nullptr);
        pyobj->_arraySize = size;
        pyobj->_CObject = new ObjectContainerBytePool<T>(size, constructor);
        return pyobj;
    }

    template<typename T>
    PythonClassWrapper<T, typename std::enable_if<
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value > 1) &&
            sizeof(typename extent_as_pointer<T>::type) == sizeof(typename base_type<T>::type *)>::type> *
    PythonClassWrapper<T, typename std::enable_if<
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value > 1) &&
            sizeof(typename extent_as_pointer<T>::type) == sizeof(typename base_type<T>::type *)>::type>::
    createPyReferenceToAddr() {
        auto *addrType = PythonClassWrapper<T>::getPyType();

        static PyObject *args = PyTuple_New(0);
        static PyObject *kwds = PyDict_New();
        PythonClassWrapper *pyobj = (PythonClassWrapper *) PyObject_Call((PyObject *) &addrType, args, kwds);
        if (!pyobj) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to create Python Object");
            return nullptr;
        }
        assert(pyobj->get_CObject() == nullptr);
        pyobj->_arraySize = 0;
        pyobj->_Cobject = ObjectContainerPyReference<T>((PyObject *) this,
                                                        [](PyObject *s) -> T * {
                                                            return (T * )((PythonClassWrapper *) s)->get_CObject()->ptr();
                                                        });
        return pyobj;
    }

    template<typename T>
    PyMethodDef
            PythonClassWrapper<T, typename std::enable_if<
                    !std::is_function<typename std::remove_pointer<T>::type>::value &&
                    (std::is_pointer<T>::value || std::is_array<T>::value) &&
                    (ptr_depth<T>::value > 1) &&
                    sizeof(typename extent_as_pointer<T>::type) == sizeof(typename base_type<T>::type *)>::type>::
            _methods[] =
            {{address_name, (PyCFunction) PythonClassWrapper::_addr, METH_KEYWORDS | METH_VARARGS, nullptr},
             {"at",         (PyCFunction) Base::_at,                 METH_KEYWORDS | METH_VARARGS, nullptr},
             {nullptr,      nullptr, 0, nullptr} /*sentinel*/
            };


    template<typename T>
    PyMethodDef
            PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
                    !std::is_function<typename std::remove_pointer<T>::type>::value &&
                    (std::is_pointer<T>::value || std::is_array<T>::value) &&
                    (ptr_depth<T>::value == 1)>::type>::
            _methods[] =
            {{address_name, (PyCFunction) PythonClassWrapper::_addr, METH_KEYWORDS | METH_VARARGS, nullptr},
             {"at",         (PyCFunction) Base::_at,                 METH_KEYWORDS | METH_VARARGS, nullptr},
             {nullptr,      nullptr, 0, nullptr} /*sentinel*/
            };

    template<typename T>
    bool
    PythonClassWrapper<T, typename std::enable_if<
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value > 1) &&
            sizeof(typename extent_as_pointer<T>::type) == sizeof(typename base_type<T>::type *)>::type>::
    checkType(PyObject *const obj) {
        if (!obj || !obj->ob_type || (obj->ob_type->tp_init != (initproc) _init)) { return false; }
        return PyObject_TypeCheck(obj, &Type);
    }

    template<typename T>
    PyTypeObject
            PythonClassWrapper<T, typename std::enable_if<
                    !std::is_function<typename std::remove_pointer<T>::type>::value &&
                    (std::is_pointer<T>::value || std::is_array<T>::value) &&
                    (ptr_depth<T>::value > 1) &&
                    sizeof(typename extent_as_pointer<T>::type) == sizeof(typename base_type<T>::type *)>::type>::Type = {
    #if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
    #else
    PyObject_HEAD_INIT(nullptr)
            0,                         /*ob_size*/
    #endif
            type_name<T>(),             /*tp_name*/  //set on call to initialize
            sizeof(PythonClassWrapper) + 8,             /*tp_basicsize*/
            0,                         /*tp_itemsize*/
            (destructor) &Base::_dealloc, /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            nullptr,                         /*tp_getattr*/
            nullptr,                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            nullptr,                         /*tp_as_number*/
            &Base::_seqmethods,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            nullptr,                         /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
            "PythonCPointerWrapper object",           /* tp_doc */
            nullptr,                               /* tp_traverse */
            nullptr,                               /* tp_clear */
            nullptr,                               /* tp_richcompare */
            0,                             /* tp_weaklistoffset */
            nullptr,                               /* tp_iter */
            nullptr,                               /* tp_iternext */
            _methods,             /* tp_methods */
            nullptr,             /* tp_members */
            nullptr,                         /* tp_getset */
            nullptr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            (initproc) _init,      /* tp_init */
            nullptr,                         /* tp_alloc */
            Base::_new,                 /* tp_new */
            nullptr,                         /*tp_free*/ // no freeing of fixed-array
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_base*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };

    template<typename T>
    PyObject *
    PythonClassWrapper<T, typename std::enable_if<!std::is_function<typename std::remove_pointer<T>::type>::value &&
                                                  (std::is_pointer<T>::value || std::is_array<T>::value) &&
                                                  (ptr_depth<T>::value > 1) &&
                                                  sizeof(typename extent_as_pointer<T>::type) ==
                                                  sizeof(typename base_type<T>::type *)>::type>::
    _addr(PyObject *self_, PyObject *args) {
        typedef typename remove_all_pointers<T>::type T_bare;
        PythonClassWrapper *self = reinterpret_cast<PythonClassWrapper *>(self_);
        if (!self->_CObject || !*self->_CObject) {
            PyErr_SetString(PyExc_RuntimeError, "Cannot take address of null pointer!");
            return nullptr;
        }
        try {
            T_bare **obj = (T_bare **) (self->_CObject->ptr());
            PythonClassWrapper < T_bare * * > *pyobj = reinterpret_cast<PythonClassWrapper < T_bare * * > * > (
                    PythonClassWrapper<T_bare **>::createPy(1, obj, ContainmentKind::BY_REFERENCE, (PyObject *) self));
            pyobj->_depth = self->_depth + 1;
            return reinterpret_cast<PyObject *>(pyobj);
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }

////////////////////////////////


    template<typename T>
    bool
    PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value == 1)>::type>::
    checkType(PyObject *const obj) {
        if (!obj || !obj->ob_type || (obj->ob_type->tp_init != (initproc) _init)) { return false; }
        return PyObject_TypeCheck(obj, &Type);
    }

    template<typename T>
    PyObject *
    PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value == 1)>::type>::
    _addr(PyObject *self_, PyObject *args) {
        PythonClassWrapper *self = (PythonClassWrapper *) self_;
        if (!self->get_CObject()) {
            PyErr_SetString(PyExc_RuntimeError, "Cannot take address of null pointer!");
            return nullptr;
        }
        try {
            auto *pyobj = self->createPyReferenceToAddr();//1, obj, ContainmentKind ::BY_REFERENCE, (PyObject *) self);
            pyobj->_depth = 2;
            return reinterpret_cast<PyObject *>(pyobj);
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }

    }


    template<typename T>
    int
    PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value == 1)>::type>::
    _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
        if (!self) { return -1; }

        self->_arraySize = UNKNOWN_SIZE;
        self->_referenced_elements = nullptr;
        self->_referenced = nullptr;
        PyTypeObject *const coreTypePtr = PythonClassWrapper<typename core_type<T>::type>::getPyType();
        self->template populate_type_info<T>(&checkType, coreTypePtr);

        int result = Base::_initbase(self, args, kwds, &Type);

        if (result == ERROR_TYPE_MISMATCH && (ptr_depth<T>::value == 1) &&
            (PythonClassWrapper<const char *>::checkType((PyObject *) self) ||
             PythonClassWrapper<const char *const>::checkType((PyObject *) self))) {
            PyObject *arg = PyTuple_GetItem(args, 0);
            const char *const s = PyString_AsString(arg);
            if (s) {
                Py_INCREF(arg);
                typedef typename extent_as_pointer<T>::type T_core;
                T *s2 = (T *) &s;
                self->_CObject = new ObjectContainerReference<T>(*s2);
                self->_arraySize = UNKNOWN_SIZE;
                self->_referenced = arg;
                PyErr_Clear();
                return 0;
            }
        }
        return result;
    }

    template<typename T>
    PythonClassWrapper<T, typename std::enable_if<!std::is_function<typename std::remove_pointer<T>::type>::value &&
                                                  (std::is_pointer<T>::value || std::is_array<T>::value) &&
                                                  (ptr_depth<T>::value == 1)>::type> *
    PythonClassWrapper<T, typename std::enable_if<!std::is_function<typename std::remove_pointer<T>::type>::value &&
                                                  (std::is_pointer<T>::value || std::is_array<T>::value) &&
                                                  (ptr_depth<T>::value == 1)>::type>::
    createPyUsingBytePool(const size_t size, std::function<void(void *, size_t)>& constructor){
        if (
            PythonPointerWrapperBase<T>::_initialize(Type) != 0) {
            PyErr_SetString(PyExc_SystemError, "System error: failed to initialize type");
            return nullptr;
        }
        static PyObject *args = PyTuple_New(0);
        static PyObject *kwds = PyDict_New();
        PyTypeObject *type = getPyType();
        if (!type){
            PyErr_SetString(PyExc_RuntimeError, "Unable to initializer Python type to c-pointer-wrapper");
            return nullptr;
        }
        PythonClassWrapper *pyobj = (PythonClassWrapper *) PyObject_Call((PyObject *) getPyType(), args, kwds);
        if (!pyobj) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to create Python Object");
            return nullptr;
        }
        assert(pyobj->get_CObject() == nullptr);
        pyobj->_arraySize = size;
        pyobj->_CObject = new ObjectContainerBytePool<T>(size, constructor);
        return pyobj;
    }


    template<typename T>
    PythonClassWrapper<T *> *
    PythonClassWrapper<T, typename std::enable_if<
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value == 1)>::type>::
    createPyReferenceToAddr() {
        auto *addrType = PythonClassWrapper<T *>::getPyType();

        static PyObject *args = PyTuple_New(0);
        static PyObject *kwds = PyDict_New();
        PythonClassWrapper < T * > *pyobj = (PythonClassWrapper < T * > *)
        PyObject_Call((PyObject *) &addrType, args, kwds);
        if (!pyobj) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to create Python Object");
            return nullptr;
        }
        assert(pyobj->get_CObject() == nullptr);
        pyobj->_arraySize = 0;
        pyobj->_CObject = new ObjectContainerPyReference<T *>((PyObject *) this,
                                                              [](PyObject *s) -> T * { return ((PythonClassWrapper *) s)->_CObject->ptr(); });
        return pyobj;
    }

    template<typename T>
    PyTypeObject
            PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
                    !std::is_function<typename std::remove_pointer<T>::type>::value &&
                    (std::is_pointer<T>::value || std::is_array<T>::value) &&
                    (ptr_depth<T>::value == 1)>::type>::Type = {
    #if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
    #else
    PyObject_HEAD_INIT(nullptr)
        0,                         /*ob_size*/
    #endif
            type_name<T>(),             /*tp_name*/  //set on call to initialize
            sizeof(PythonClassWrapper) + 8,             /*tp_basicsize*/
            0,                         /*tp_itemsize*/
            (destructor) &Base::_dealloc, /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            nullptr,                         /*tp_getattr*/
            nullptr,                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            nullptr,                         /*tp_as_number*/
            &Base::_seqmethods,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            nullptr,                         /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
            "PythonCPointerWrapper object",           /* tp_doc */
            nullptr,                               /* tp_traverse */
            nullptr,                               /* tp_clear */
            nullptr,                               /* tp_richcompare */
            0,                             /* tp_weaklistoffset */
            Iter::iter,                               /* tp_iter */
            Iter::iternext,                               /* tp_iternext */
            _methods,             /* tp_methods */
            nullptr,             /* tp_members */
            nullptr,                         /* tp_getset */
            nullptr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            (initproc) _init,      /* tp_init */
            nullptr,                         /* tp_alloc */
            Base::_new,                 /* tp_new */
            nullptr,                         /*tp_free*/ // no freeing of fixed-array
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_base*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };


    #if PY_MAJOR_VERSION >= 3
    #  define Py_TPFLAGS_HAVE_ITER 0
    #endif

    template<typename T>
    const std::string
            PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
                    !std::is_function<typename std::remove_pointer<T>::type>::value &&
                    (std::is_pointer<T>::value || std::is_array<T>::value) &&
                    (ptr_depth<T>::value == 1)>::type>::Iter::name = std::string(type_name<T>()) + std::string(" iterator");


    template<typename T>
    PyObject *
    PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value == 1)>::type>::Iter::iter(PyObject *self) {

        Iter *p;
        auto self_ = (PythonClassWrapper *) self;
        /* I don't need python callable __init__() method for this iterator,
           so I'll simply allocate it as PyObject and initialize it by hand. */

        p = PyObject_New(Iter, &Type);
        if (!p) return nullptr;

        if (!PyObject_Init((PyObject *) p, &Type)) {
            Py_DECREF(p);
            return nullptr;
        }
        if (!self_->get_CObject()) {
            Py_DECREF(p);
            return nullptr;
        }

        p->max = self_->_arraySize > 0 ? self_->_arraySize : 1;
        p->i = 0;
        return (PyObject *) p;

    }

    template<typename T>
    PyObject *
    PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value == 1)>::type>::Iter::iternext(PyObject *self) {
        Iter *p = (Iter *) self;
        if (p->i < p->max) {
            PyObject *tmp = PythonClassWrapper::_get_item((PyObject *) p->obj, p->i);
            (p->i)++;
            return tmp;
        } else {
            /* Raising of standard StopIteration exception with empty value. */
            PyErr_SetNone(PyExc_StopIteration);
            return nullptr;
        }
    }

    template<typename T>
    PyTypeObject
            PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
                    !std::is_function<typename std::remove_pointer<T>::type>::value &&
                    (std::is_pointer<T>::value || std::is_array<T>::value) &&
                    (ptr_depth<T>::value == 1)>::type>::Iter::Type = {
    #if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
    #else
    PyObject_HEAD_INIT(nullptr)
        0,                         /*ob_size*/
    #endif
            name.c_str(),             /*tp_name*/  //set on call to initialize
            sizeof(PythonClassWrapper::Iter) + 8,             /*tp_basicsize*/
            0,                         /*tp_itemsize*/
            nullptr, /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            nullptr,                         /*tp_getattr*/
            nullptr,                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            nullptr,                         /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            nullptr,                         /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_ITER, /*tp_flags*/
            "PythonCPointerWrapper iterator object",           /* tp_doc */
            nullptr,                               /* tp_traverse */
            nullptr,                               /* tp_clear */
            nullptr,                               /* tp_richcompare */
            0,                             /* tp_weaklistoffset */
            iter,                               /* tp_iter */
            iternext,                               /* tp_iternext */
            nullptr,             /* tp_methods */
            nullptr,             /* tp_members */
            nullptr,                         /* tp_getset */
            nullptr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            nullptr,      /* tp_init */
            nullptr,                         /* tp_alloc */
            PyType_GenericNew,                 /* tp_new */
            nullptr,                         /*tp_free*/ // no freeing of fixed-array
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_base*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };

}

#endif
