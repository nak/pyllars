//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__POINTER_CPP_
#define __PYLLARS_INTERNAL__POINTER_CPP_

#include "pyllars_pointer.hpp"
#include "pyllars_classwrapper.cpp"

namespace __pyllars_internal {

    template<typename T>
    Py_ssize_t PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    _size(PyObject *self) {
        Py_ssize_t length = ((PythonClassWrapper *) self)->_arraySize;
        return length > 0 ? length : 1;
    }

    template<typename T>
    PyObject * PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    _concat(PyObject *self, PyObject *other) {

        if (checkType(other) || NonConstWrapper::checkType(other) || ConstWrapper::checkType(other)) {
            PythonClassWrapper *self_ = (PythonClassWrapper *) self;
            PythonClassWrapper *other_ = (PythonClassWrapper *) other;
            if (self_->_depth != 1 || other_->_depth != 1) {
                return nullptr;
            }
            if (self_->_arraySize <= 0 || other_->_arraySize <= 0) {
                PyErr_SetString(PyExc_TypeError, "Cannot concatenate array(s) of unknown size");
                return nullptr;
            }
            const ssize_t new_size = self_->_arraySize + other_->_arraySize;
            char *raw_storage = new char[new_size * Sizeof<T_base>::value];
            T_base *values = (T_base *) raw_storage;
            for (size_t i = 0; i < (size_t) self_->_arraySize; ++i) {
                T &cobj = *self_->template get_CObject<T>();
                ObjectLifecycleHelpers::Copy<T_base>::inplace_copy(
                        values, i, ObjectLifecycleHelpers::Copy<T_base>::new_copy(
                                ObjectLifecycleHelpers::Array<T_base *>::at(cobj, i) )->ptr(), true);
            }
        }
        return nullptr;
    }

    template<typename T>
    PyObject * PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    _inplace_repeat(PyObject *self, Py_ssize_t count) {
        return nullptr;
    }

    template<typename T>
    PyObject * PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    _repeat(PyObject *self, Py_ssize_t count) {
        return nullptr;
    }


    template<typename T>
    int  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    _set_item(PyObject *self, Py_ssize_t index, PyObject *obj) {
        PythonClassWrapper *self_ = (PythonClassWrapper *) self;
        if (self_->_depth != 1) { return -1; }
        if (index < 0 && self_->_arraySize > 0) { index = self_->_arraySize + index + 1; };
        if (index < 0 || index > self_->_arraySize) { return -1; };
        if (!PythonClassWrapper<T_base>::checkType(obj) || !PythonClassWrapper<const T_base>::checkType(obj)) {
            PyErr_SetString(PyExc_TypeError, "Setting item from incompatible type");
            return -1;
        }
        PythonClassWrapper<const T_base> *obj_ = (PythonClassWrapper<const T_base> *) obj;
        ObjectLifecycleHelpers::Copy<T_base>::inplace_copy(*(self_->template get_CObject<T>()), index,
                                                           obj_->template get_CObject<T_base>(),
                                                           self_->_raw_storage != nullptr);
        return 0;
    }

    template<typename T>
    int  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    _contains(PyObject *self, PyObject *obj) {
        return -1;
    }

    template<typename T>
    PyObject * PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    _at(PyObject *self, PyObject *args, PyObject *kwds) {
        if ((kwds && PyDict_Size(kwds) != 0) || !args || PyTuple_Size(args) != 1 ||
            !(PyLong_Check(PyTuple_GetItem(args, 0)) || PyInt_Check(PyTuple_GetItem(args, 0)))) {
            PyErr_BadArgument();
            return nullptr;
        }
        PyObject *pyindex = PyTuple_GetItem(args, 0);
        Py_ssize_t index = -1;
        if (PyLong_Check(pyindex)) {
            index = PyLong_AsLong(PyTuple_GetItem(args, 0));
        } else {
            index = PyInt_AsLong(PyTuple_GetItem(args, 0));
        }

        return _get_item2(self, index, true);
    }

    template<typename T>
    PyObject * PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    _get_item(PyObject *self, Py_ssize_t index) {
        return _get_item2(self, index, false);
    }

    template<typename T>
    PyObject * PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
             (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    _get_item2(PyObject *self, Py_ssize_t index, const bool make_copy) {
        PythonClassWrapper *self_ = (PythonClassWrapper *) self;
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
            ssize_t element_array_size = (self_->_depth == 1 ? std::extent<T_base>::value : UNKNOWN_SIZE);
            if (element_array_size == 0) { element_array_size = UNKNOWN_SIZE; }
            if (self_->_referenced_elements) {
                PythonClassWrapper *item = (PythonClassWrapper *) PyList_GetItem(self_->_referenced_elements,
                                                                                 index);
                element_array_size = item ? item->_arraySize : element_array_size;
            }

            PyObject *result;
            if (make_copy) {
                typename ObjectLifecycleHelpers::Array<T>::T_base& var = ObjectLifecycleHelpers::Array<T>::at(*self_->_CObject->ptr(), index);
                ObjContainer<T_base> *new_copy = ObjectLifecycleHelpers::Copy<T_base>::new_copy(var);
                PythonClassWrapper<T_base> *res = PythonClassWrapper<T_base>::createPy(element_array_size, new_copy,
                                                                                       true, false, nullptr);
                result = (PyObject *) res;
            } else {
                typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
                PythonClassWrapper< T_base> *res = (PythonClassWrapper<T_base > *)
                        (ObjectContent<T>::getObjectAt(*self_->_CObject->ptr(), index, element_array_size, self_->_depth));
                if(!res) return nullptr;
                if (self_->_depth != 1) {
                    res->_depth = self_->_depth - 1;
                }
                res->make_reference(self);
                result = (PyObject *) res;
            }
            return (PyObject *) result;
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }

    template<typename T>
    ssize_t  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    getArraySize() {
        return _arraySize;
    }

    template<typename T>
    void  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    setArraySize(const ssize_t size) {
        _arraySize = size;
        _max = size < 0 ? -1 : size - 1;
    }


    template<typename T>
    std::string  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    get_name(const size_t depth ) {
        char extension[depth + 1];// = {0};
        extension[depth] = 0;
        memset(extension, '*', depth);
        return PythonClassWrapper<T_base>::get_name() + extension;
    }

    template<typename T>
    std::string  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    get_module_entry_name(const size_t depth) {
        char extension[depth + 1];// = {0};
        extension[depth] = 0;
        memset(extension, '*', depth);
        return PythonClassWrapper<T_base>::get_module_entry_name() + extension;
    }

    template<typename T>
    int  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    initialize(const size_t depth) {
        if (PythonClassWrapper<T_base>::get_name().size() == 0) {
            if (depth > 1){ initialize(depth-1);} else {
                return -1;
            }
        }
        int status = 0;// PythonClassWrapper<T_base>::initialize();
        static std::string name = PythonClassWrapper<T_base>::get_name() + "*";
        static std::string module_name = PythonClassWrapper<T_base>::get_module_entry_name() + "*";
        static std::string full_name = PythonClassWrapper<T_base>::get_full_name() + "*";

        return status != 0 ? status : initialize(name.c_str(),
                                                 module_name.c_str(),
                                                 PythonClassWrapper<T_base>::parent_module,
                                                 full_name.c_str(),
                                                 depth);
    }

    template<typename T>
    int  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    initialize(const char *const name, const char *const module_entry_name,
                          PyObject *module, const char *const fullname,
                          const size_t depth ) {
        static bool initialized = false;
        if (initialized) return 0;
        initialized = true;
        if (!name && PythonClassWrapper<T_base>::initialize(name, module_entry_name,
                                                            module, fullname) != 0) {
            return -1;
        }
        static std::vector<int> status;
        if (status.size() >= depth && status[depth] > -2) {
            return status[depth];
        }
        while (status.size() < depth + 1) {
            status.push_back(-2);
        }
	_full_name = fullname?fullname:(name?name:"NONAME");
        PyTypeObject *type = getType(depth, fullname);
        if (!type) {
            return -1;
        }
        type->tp_init = (initproc) _init;
        type->tp_methods[0].ml_meth = (PyCFunction) _addr;
        type->tp_methods[1].ml_meth = (PyCFunction) _at;

        _seqmethods.sq_length = _size;
        _seqmethods.sq_concat = _concat;
        _seqmethods.sq_repeat = _repeat;
        _seqmethods.sq_item = _get_item;
        _seqmethods.sq_ass_item = _set_item;
        _seqmethods.sq_contains = _contains;
        _seqmethods.sq_inplace_concat = _inplace_concat;
        _seqmethods.sq_inplace_repeat = _inplace_repeat;

        type->tp_as_sequence = &_seqmethods;

        Py_INCREF(type);
        if (module == nullptr)
            return 0;
        PyModule_AddObject(module, module_entry_name ? module_entry_name : get_module_entry_name(depth).c_str(),
                           (PyObject *) type);
        parent_module = module;

        status[depth] = 0;
        return 0;
    }


    template<typename T>
    PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type> *
    PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    createPy2(const ssize_t arraySize,
              T *const cobj,
              const bool isAllocated,
              const bool inPlace,
              PyObject *referencing,
              const size_t depth ) {

      return createPy( arraySize, new ObjContainerProxy<T, T>(*cobj), isAllocated, inPlace, referencing, depth);
    }

    template<typename T>
    PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type> *
    PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    createPy(const ssize_t arraySize,
             ObjContainer<T> *const cobj,
             const bool isAllocated,
             const bool inPlace,
             PyObject *referencing,
             const size_t depth) {
        if (cobj == nullptr) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid null pointer on object creation!");
            return nullptr;
        }
        if (depth > 1000) {
            PyErr_SetString(PyExc_RuntimeError,
                            "Pointer depth requested greater than 1000 -- possible run time error");
            return nullptr;
        }
        if (initialize(depth) != 0) {
            return nullptr;
        }
        static PyObject *args = PyTuple_New(0);
        static PyObject *kwds = PyDict_New();
        PyDict_SetItemString(kwds, "__internal_depth", PyLong_FromLong(depth));
        PythonClassWrapper::initialize(depth);
        PyTypeObject *type = getType(depth);
        if (!type) {
            PyErr_SetString(PyExc_RuntimeError, "Cannot get type for pointer wrapper object");
            return nullptr;
        }
        if (!type->tp_name) {
            PyErr_SetString(PyExc_RuntimeError, "Uninitialized type when creating object");
            return nullptr;
        }

        PythonClassWrapper *pyobj = (PythonClassWrapper *) PyObject_Call((PyObject *) type, args, kwds);
        if (!pyobj) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to create Python Object");
            return nullptr;
        }
        pyobj->_CObject = cobj;
        pyobj->_allocated = isAllocated;
        pyobj->_inPlace = inPlace;
        pyobj->_arraySize = arraySize;
        if (arraySize > 0) { pyobj->_max = arraySize - 1; }
        pyobj->_depth = depth;
        if (referencing) pyobj->make_reference(referencing);
        return pyobj;
    }


    template<typename T>
    int  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
        if (!kwds || !PyDict_GetItemString(kwds, "__internal_depth")) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid arguments on pointer-wrapper creation");
            return -1;
        }
        if (!self) { return -1; }

        self->_arraySize = UNKNOWN_SIZE;
        self->_referenced_elements = nullptr;
        self->_referenced = nullptr;
        PyObject *pydepth = PyDict_GetItemString(kwds, "__internal_depth");
        if (!PyLong_Check(pydepth)) {
            PyErr_SetString(PyExc_TypeError, "Invalid argument type for depth on pointer-wrapper creation");
            return -1;
        }
        self->_depth = PyLong_AsLong(pydepth);
        PyDict_DelItemString(kwds, "__internal_depth");

        int result = _initbase(self, args, kwds, getType(self->_depth));

        if (result == ERROR_TYPE_MISMATCH && (self->_depth == 1) &&
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
                self->_depth = 1;
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
    int  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    _initbase(PythonClassWrapper *self, PyObject *args, PyObject *kwds, PyTypeObject *pytypeobj) {
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
                self->_CObject = reinterpret_cast<PythonClassWrapper *>(pyobj)->_CObject;
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
    template<typename T2>
    T2 * PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    get_CObject() {
        return (T2 *) _CObject;
    }

    template<typename T>
    PyObject * PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    _addr(PyObject* self_, PyObject *args) {
      PythonClassWrapper * self = (PythonClassWrapper*)self_;
        if (!self->_CObject || !*self->_CObject) {
            PyErr_SetString(PyExc_RuntimeError, "Found null object when taking address!");
            return nullptr;
        }
        try {
            T **obj = self->_CObject ? self->_CObject->ptrptr() : nullptr;
            PythonClassWrapper *pyobj = createPy2(1, (T *) obj, false, false, (PyObject *) self, self->_depth + 1);
            return (PyObject *) pyobj;
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }

    template<typename T>
    void  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    _dealloc(PyObject *self_) {
        PythonClassWrapper *self = (PythonClassWrapper *) self_;
        //TODO: track dynamically allocated content and free if so
        if (self != nullptr) {
            ObjectLifecycleHelpers::Deallocation<T, PythonClassWrapper>::_free(self);
            if (self->_allocated) self->_CObject = nullptr;
            self->baseClass.ob_type->tp_free((PyObject *) self);
            if (self->_referenced) {
                Py_XDECREF(self->_referenced);
                self->_referenced = nullptr;
            }

        }
    }

    template<typename T>
    PyObject * PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        (void) args;
        (void) kwds;
        PythonClassWrapper *self;
        self = (PythonClassWrapper *) type->tp_alloc(type, 0);
        if (self != nullptr) {
            self->_CObject = nullptr;
        }
        return (PyObject *) self;

    }

    template<typename T>
    bool  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    checkType(PyObject *const obj) {
        if (!obj || !obj->ob_type || (obj->ob_type->tp_init != (initproc) _init)) { return false; }
        PythonClassWrapper *obj_ = (PythonClassWrapper *) obj;
        return PyObject_TypeCheck(obj, getType(obj_->_depth));
    }

    template<typename T>
    bool  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    checkTypeDereferenced(PyObject *const obj) {
        if (!obj || !obj->ob_type || (obj->ob_type->tp_init != (initproc) _init)) { return false; }
        PythonClassWrapper *obj_ = (PythonClassWrapper *) obj;
        return PyObject_TypeCheck(obj, getType(obj_->_depth - 1));
    }

    template<typename T>
    PyTypeObject * PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    getType(const size_t depth,
            const char *const fullname) {
        static std::vector<PyTypeObject *> typeList;
        static PyTypeObject Type = {
                PyObject_HEAD_INIT(nullptr)
                0,                         /*ob_size*/
                nullptr,             /*tp_name*/  //set on call to initialize
                sizeof(PythonClassWrapper) + 8,             /*tp_basicsize*/
                0,                         /*tp_itemsize*/
                (destructor) &_dealloc, /*tp_dealloc*/
                nullptr,                         /*tp_print*/
                nullptr,                         /*tp_getattr*/
                nullptr,                         /*tp_setattr*/
                nullptr,                         /*tp_compare*/
                nullptr,                         /*tp_repr*/
                nullptr,                         /*tp_as_number*/
                &_seqmethods,                         /*tp_as_sequence*/
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
                _new,                 /* tp_new */
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
        while (typeList.size() < depth + 1) {
            typeList.push_back(nullptr);
        }
        if (!typeList.at(depth)) {
            PyTypeObject *type = &Type;
            typeList[depth] = new PyTypeObject(*type);

            std::string strname = get_name(depth);
            char *new_name = fullname ? new char[strlen(fullname) + 1]:new char[strname.length() + 1];
            if (!fullname) {
                strcpy(new_name, strname.c_str());
            } else {
                strcpy(new_name, fullname);
            }
            typeList[depth]->tp_name = new_name;
            if (PyType_Ready(typeList[depth]) < 0) {
                PyErr_SetString(PyExc_RuntimeError, "Error initializing type");
                return nullptr;
            }
        }

        return typeList.at(depth);
    }

    template<typename T>
    void  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    set_raw_storage(T_base *const storage, const size_t size) {
        _raw_storage = storage;
        _raw_size = size;
        if (storage) {
            _CObject = new ObjContainerProxy<T, T>(_raw_storage);
            _allocated = false;
        }
    }

    template<typename T>
    void  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
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
    Py_ssize_t  PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::
    get_array_index(PythonClassWrapper *const self, PyObject *args, PyObject *kwargs) {
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
    PyObject *PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::parent_module = nullptr;

    template<typename T>
    PyMethodDef PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::_methods[] =
            {{address_name,  (PyCFunction)_addr, METH_KEYWORDS, nullptr},
             {"at",         nullptr, METH_KEYWORDS, nullptr},
             {nullptr,      nullptr, 0,             nullptr} /*sentinel*/
            };

    template<typename T>
    PySequenceMethods PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::_seqmethods;
    template<typename T>
    std::string PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type>::_full_name;

}

#endif
