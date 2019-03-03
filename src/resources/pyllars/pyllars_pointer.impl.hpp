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
        PythonClassWrapper<T> * self__ = (PythonClassWrapper<T>*) self;
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
                                ObjectLifecycleHelpers::Array<T_base *>::at(cobj, i))->ptr(), true);
            }
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
                                                               obj_->get_CObject(),
                                                               self_->_raw_storage != nullptr);
            return 0;
        } catch (const char* const msg) {
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
        return _get_item2(self, index, false);
    }

    template<typename T>
    PyObject *
    PythonPointerWrapperBase<T>::
    _get_item2(PyObject *self, Py_ssize_t index, const bool make_copy) {
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
                PythonPointerWrapperBase *item = (PythonPointerWrapperBase *) PyList_GetItem(self_->_referenced_elements,
                                                                                 index);
                element_array_size = item ? item->_arraySize : element_array_size;
            }

            PyObject *result;
            if (make_copy) {
                if (self_->_depth > 2) {
                     T_bare** &var = ObjectLifecycleHelpers::Array<T_bare***>::at((T_bare***)*self_->_CObject->ptr(), index);
                    ObjContainer<T_bare**> *new_copy = ObjectLifecycleHelpers::Copy<T_bare**>::new_copy2(var);
                    PythonClassWrapper<T_bare**> *res = PythonClassWrapper<T_bare**>::createPy(element_array_size, new_copy,
                                                                                           true, false, nullptr);

                    if (res ) {
                        res->_depth = self_->_depth - 1;
                    }
                    result = reinterpret_cast<PyObject *>(res);
                } else if (self_->_depth == 2){
                    T_bare* &var = ObjectLifecycleHelpers::Array<T_bare**>::at( (T_bare**)*self_->_CObject->ptr(), index);
                    ObjContainer<T_bare*> *new_copy = ObjectLifecycleHelpers::Copy<T_bare*>::new_copy2(var);
                    PythonClassWrapper<T_bare*> *res = PythonClassWrapper<T_bare*>::createPy(element_array_size, new_copy,
                                                                                           true, false, nullptr);

                    if (res ) {
                        ((PythonPointerWrapperBase*)res)->_depth = self_->_depth - 1;
                    }
                    result = reinterpret_cast<PyObject *>(res);
                } else {
                    T_bare & var = ObjectLifecycleHelpers::Array<T_bare*>::at( (T_bare*)*self_->_CObject->ptr(), index);
                    ObjContainer<T_bare> *new_copy = ObjectLifecycleHelpers::Copy<T_bare>::new_copy2(var);
                    PythonClassWrapper<T_bare> *res = (PythonClassWrapper<T_bare> *) PythonClassWrapper<T_bare>::createPy(element_array_size, new_copy,
                                                                                           true, false, nullptr);

                    result = reinterpret_cast<PyObject *>(res);
                }
            } else {
                if (self_->_depth > 2){
                    PythonClassWrapper<T_bare**> *res = (PythonClassWrapper<T_bare**> *)
                            (ObjectContent<T_bare**>::getObjectAt((T_bare**)*self_->_CObject->ptr(), index, element_array_size));
                    if(res) {
                        res->make_reference(self);
                        res->_depth = self_->_depth-1;
                    }
                    result = reinterpret_cast<PyObject *>(res);
                } else if(self_->_depth == 2) {
                    PythonClassWrapper<T_bare*> *res = (PythonClassWrapper<T_bare*> *)
                            (ObjectContent<T>::getObjectAt(*self_->_CObject->ptr(), index, element_array_size));
                    if(res){
                        ((PythonPointerWrapperBase*)res)->_depth = 1;
                        res->make_reference(self);
                    }
                    result = reinterpret_cast<PyObject *>(res);
                } else {
                    PythonClassWrapper<T_bare> *res = (PythonClassWrapper<T_bare> *)
                            (ObjectContent<T>::getObjectAt(*self_->_CObject->ptr(), index, element_array_size));
                    if(res && PyObject_TypeCheck(res, PythonClassWrapper<T_bare>::getPyType())) {
                        res->make_reference(self);
                    }
                    result = reinterpret_cast<PyObject *>(res);
                }
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
            PyObject * set_value = nullptr;
            if (kwds && PyDict_Size(kwds) == 1){
                set_value = PyDict_GetItemString(kwds, "set_value");
                if(!set_value){
                    PyErr_BadArgument();
                    return nullptr;
                }
                Py_INCREF(set_value);
                PyDict_DelItemString(kwds, "set_value");
            }
            if ((kwds && PyDict_Size(kwds) != 0) || !args || PyTuple_Size(args) != 1 ||
                !(PyLong_Check(PyTuple_GetItem(args, 0)) || PyInt_Check(PyTuple_GetItem(args, 0)))) {
                PyErr_BadArgument();
                if(set_value){Py_DECREF(set_value);}
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
            return _get_item2(self, index, false);
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }

    template<typename T>
    int PythonPointerWrapperBase<T>::
    _initialize(PyTypeObject & Type) {
        static bool initialized = false;
        if (initialized) return 0;
        initialized = true;
        if(PyType_Ready(&CommonBaseWrapper::_BaseType)< 0){
            PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
            return -1;
        }
        Type.tp_base = &CommonBaseWrapper::_BaseType;

	    if (PyType_Ready(&Type) < 0){
            PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
	        return -1;
	    }
        _seqmethods.sq_length = _size;
        _seqmethods.sq_concat = _concat;
        _seqmethods.sq_repeat =  _repeat;
        _seqmethods.sq_item =  _get_item;
        _seqmethods.sq_ass_item = _set_item;
        _seqmethods.sq_contains =  _contains;
        _seqmethods.sq_inplace_concat =  _inplace_concat;
        _seqmethods.sq_inplace_repeat =  _inplace_repeat;

        Type.tp_as_sequence = & _seqmethods;

        Py_INCREF(&Type);
        return 0;
    }


    template<typename T>
    PythonPointerWrapperBase<T> *
    PythonPointerWrapperBase<T>::
    _createPy2(PyTypeObject & Type,
               const ssize_t arraySize,
               T *const cobj,
               const bool isAllocated,
               const bool inPlace,
               PyObject *referencing) {

        return _createPy(Type, arraySize, new ObjContainerProxy<T, T>(*cobj), isAllocated, inPlace, referencing);
    }

    template<typename T>
    PythonPointerWrapperBase<T> *
    PythonPointerWrapperBase<T>::
    _createPy(PyTypeObject & Type,
              const ssize_t arraySize,
              ObjContainer<T> *const cobj,
              const bool isAllocated,
              const bool inPlace,
              PyObject *referencing) {
        if (cobj == nullptr) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid null pointer on object creation!");
            return nullptr;
        }
        if (_initialize(Type) != 0) {
            return nullptr;
        }
        static PyObject *args = PyTuple_New(0);
        static PyObject *kwds = PyDict_New();
        PythonPointerWrapperBase *pyobj = (PythonPointerWrapperBase *) PyObject_Call((PyObject*) &Type, args, kwds);
        if (!pyobj) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to create Python Object");
            return nullptr;
        }
        pyobj->_CObject = cobj;
        pyobj->_allocated = isAllocated;
        pyobj->_inPlace = inPlace;
        pyobj->_arraySize = arraySize;
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
        self->_allocated = false;
        self->_inPlace = false;
        self->_raw_storage = nullptr;
        if (((PyObject *) self)->ob_type->tp_base && Base::TypePtr->tp_init) {
            PyObject *empty = PyTuple_New(0);
            Base::TypePtr->tp_init((PyObject *) &self->baseClass, empty, nullptr);
            Py_DECREF(empty);
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

            if (PyObject_TypeCheck(pyobj, pytypeobj)) {
                // if this is an instance of a basic class:
                self->make_reference(pyobj);
                self->_CObject = reinterpret_cast<PythonPointerWrapperBase *>(pyobj)->_CObject;
                status = self->_CObject ? 0 : -1;
            } else {
                PyErr_SetString(PyExc_TypeError, "Mismatched types when assigning pointer");
                status = ERROR_TYPE_MISMATCH;
                goto onerror;
            }
        } else {
            self->_allocated = true;
            self->_inPlace = false;
            self->_CObject = nullptr;
            status = 0;
        }
        onerror:
        return status;
    }

    template<typename T>
    T *PythonPointerWrapperBase<T>::
    _get_CObject() {
        if (!_CObject) {return nullptr;}
        return _CObject->ptr();
    }


    template<typename T>
    void
    PythonPointerWrapperBase<T>::
    _dealloc(PyObject *self_) {
        PythonPointerWrapperBase *self = (PythonPointerWrapperBase *) self_;
        //TODO: track dynamically allocated content and free if so
        if (self != nullptr) {
            ObjectLifecycleHelpers::Deallocation<T, PythonPointerWrapperBase>::_free(self);
            if (self->_allocated) self->_CObject = nullptr;
#if PY_MAJOR_VERSION == 3
            self->baseClass.ob_base.ob_type->tp_free((PyObject *) self);
#else
            self->baseClass.ob_type->tp_free((PyObject *) self);
#endif
            if (self->_referenced) {
                Py_XDECREF(self->_referenced);
                self->_referenced = nullptr;
            }

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
    void
    PythonPointerWrapperBase<T>::
    set_raw_storage(T_base *const storage, const size_t size) {
        _raw_storage = storage;
        _raw_size = size;
        if (storage) {
            _CObject = new ObjContainerProxy<T, T>(_raw_storage);
            _allocated = false;
        }
    }

    template<typename T>
    void
    PythonPointerWrapperBase<T>::
    delete_raw_storage() {
        if (_raw_storage) {
            for (size_t i = 0; i < _raw_size; ++i) {
                _raw_storage[i].~T_base();
            }
            char *storage = (char *) _raw_storage;
            delete[] storage;
            _raw_storage = nullptr;
            _allocated = false;
            _arraySize = UNKNOWN_SIZE;
        }
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
            (ptr_depth<T>::value > 1)  &&
            sizeof(typename extent_as_pointer<T>::type) == sizeof(typename base_type<T>::type*) >::type>::
    _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
        if (!self) { return -1; }

        self->_arraySize = UNKNOWN_SIZE;
        self->_referenced_elements = nullptr;
        self->_referenced = nullptr;
        PyTypeObject* const coreTypePtr = PythonClassWrapper<typename core_type<T>::type>::getPyType();
        self->template populate_type_info<T>(&checkType, coreTypePtr);
        int result = Base::_initbase(self, args, kwds, &Type);

        if (result == ERROR_TYPE_MISMATCH && (ptr_depth<T>::value == 1) &&
            (PythonClassWrapper<const char *>::checkType((PyObject *) self) ||
             PythonClassWrapper<const char *const>::checkType((PyObject *) self) ||
             PythonClassWrapper<char *const>::checkType((PyObject *) self) ||
             PythonClassWrapper<char *>::checkType((PyObject *) self))
                ) {
            PyObject *arg = PyTuple_GetItem(args, 0);
            const char *const s = PyString_AsString(arg);
            if (s) {
                char *new_s = new char[strlen(s) + 1];
                strcpy(new_s, s);
                T *val = const_cast<T *>(reinterpret_cast<const T *>(&new_s));
                self->_CObject = new ObjContainerProxy<T, T>(*val);
                self->_arraySize = UNKNOWN_SIZE;
                self->_referenced = nullptr;
                self->_allocated = true;
                self->_inPlace = false;
                PyErr_Clear();
                return 0;
            }
        }
        return result;
    }

    template<typename T>
    PyMethodDef
    PythonClassWrapper<T,  typename std::enable_if<
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value > 1) &&
            sizeof(typename extent_as_pointer<T>::type) == sizeof(typename base_type<T>::type*)>::type>::
     _methods[] =
            {{address_name, (PyCFunction) PythonClassWrapper::_addr, METH_KEYWORDS | METH_VARARGS, nullptr},
             {"at", (PyCFunction)  Base::_at,             METH_KEYWORDS | METH_VARARGS, nullptr},
             {nullptr,      nullptr, 0, nullptr} /*sentinel*/
            };


    template<typename T>
    PyMethodDef
            PythonClassWrapper<T,  typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value == 1) >::type>::
            _methods[] =
            {{address_name, (PyCFunction) PythonClassWrapper::_addr, METH_KEYWORDS | METH_VARARGS, nullptr},
             {"at",      (PyCFunction) Base::_at,             METH_KEYWORDS | METH_VARARGS, nullptr},
             {nullptr,      nullptr, 0, nullptr} /*sentinel*/
            };

    template<typename T>
    bool
    PythonClassWrapper<T, typename std::enable_if<
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value > 1) &&
            sizeof(typename extent_as_pointer<T>::type) == sizeof(typename base_type<T>::type*)>::type>::
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
            sizeof(typename extent_as_pointer<T>::type) == sizeof(typename base_type<T>::type*)>::type>::Type = {
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

    template <typename T>
    PyObject *
    PythonClassWrapper<T,  typename std::enable_if<
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value > 1)  &&
            sizeof(typename extent_as_pointer<T>::type) == sizeof(typename base_type<T>::type*) >::type>::_addr(PyObject *self_, PyObject *args) {
        typedef typename remove_all_pointers<T>::type T_bare;
        PythonClassWrapper *self = reinterpret_cast<PythonClassWrapper *>(self_);
        if (!self->_CObject || !*self->_CObject) {
            PyErr_SetString(PyExc_RuntimeError, "Cannot take address of null pointer!");
            return nullptr;
        }
        try {
            T_bare **obj = (T_bare **)(self->_CObject->ptr());
            PythonClassWrapper<T_bare**> *pyobj = reinterpret_cast<PythonClassWrapper<T_bare**> *>(
                    PythonClassWrapper<T_bare**>::createPy2(1, (T_bare***) &obj, false, false, (PyObject *) self));
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
    PythonClassWrapper<T,  typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value == 1) >::type>::
    checkType(PyObject *const obj) {
        if (!obj || !obj->ob_type || (obj->ob_type->tp_init != (initproc) _init)) { return false; }
        return PyObject_TypeCheck(obj, &Type);
    }

    template <typename T>
    PyObject *
    PythonClassWrapper<T,typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value == 1) >::type>::_addr(PyObject *self_, PyObject *args) {
        PythonClassWrapper* self = (PythonClassWrapper*)self_;
        if (!self->_CObject || !*self->_CObject) {
            PyErr_SetString(PyExc_RuntimeError, "Cannot take address of null pointer!");
            return nullptr;
        }
        try {
            T *obj = self->_CObject->ptr();
            PythonClassWrapper< T*>  *pyobj = PythonClassWrapper< T*>::createPy2(1, &obj, false, false, (PyObject *) self);
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
            (ptr_depth<T>::value == 1) >::type>::
    _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
        if (!self) { return -1; }

        self->_arraySize = UNKNOWN_SIZE;
        self->_referenced_elements = nullptr;
        self->_referenced = nullptr;
        PyTypeObject* const coreTypePtr = PythonClassWrapper<typename core_type<T>::type>::getPyType();
        self->template populate_type_info<T>(&checkType, coreTypePtr);

        int result = Base::_initbase(self, args, kwds, &Type);

        if (result == ERROR_TYPE_MISMATCH && (ptr_depth<T>::value == 1) &&
            (PythonClassWrapper<const char *>::checkType((PyObject *) self) ||
             PythonClassWrapper<const char *const>::checkType((PyObject *) self) ||
             PythonClassWrapper<char *const>::checkType((PyObject *) self) ||
             PythonClassWrapper<char *>::checkType((PyObject *) self))
                ) {
            PyObject *arg = PyTuple_GetItem(args, 0);
            const char *const s = PyString_AsString(arg);
            if (s) {
                char *new_s = new char[strlen(s) + 1];
                strcpy(new_s, s);
                T *val = const_cast<T *>(reinterpret_cast<const T *>(&new_s));
                self->_CObject = new ObjContainerProxy<T, T>(*val);
                self->_arraySize = UNKNOWN_SIZE;
                self->_referenced = nullptr;
                self->_allocated = true;
                self->_inPlace = false;
                PyErr_Clear();
                return 0;
            }
        }
        return result;
    }

    template<typename T>
    PyTypeObject
    PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
        !std::is_function<typename std::remove_pointer<T>::type>::value &&
        (std::is_pointer<T>::value || std::is_array<T>::value) &&
        (ptr_depth<T>::value == 1) >::type>::Type = {
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
}

#endif
