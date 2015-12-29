#ifndef __PYLLARS_INTERNAL_DEFNS
#define __PYLLARS_INTERNAL_DEFNS

#include <type_traits>
#include <sys/types.h>
#include <Python.h>

#include "pyllars_utils.hpp"

namespace __pyllars_internal {

    namespace {
        constexpr size_t MAX_PTR_DEPTH = 5;
        constexpr int ERROR_TYPE_MISMATCH = -2;
        constexpr ssize_t UNKNOWN_SIZE = -1;
         static constexpr Py_ssize_t INVALID_INDEX = LONG_LONG_MIN;
    }

    ///////////
    // Helper conversion functions
    //////////
    template<typename T, const ssize_t max = -1, typename E = void>
    PyObject *toPyObject(T &var, const bool asArgument, const ssize_t array_size, const size_t depth = ptr_depth<T>::value);

    template<typename T, const ssize_t max = -1, typename E = void>
    PyObject *toPyObject(const T &var, const bool asArgument, const ssize_t array_size, const size_t depth = ptr_depth<T>::value);

    template<typename T, bool is_array, typename ClassWrapper>
    smart_ptr<T, is_array> toCObject(PyObject &pyobj);

    struct ZERO_DEPTH;

    template<typename T>
    struct rm_ptr {
        typedef ZERO_DEPTH type;
    };

    template<typename T>
    struct rm_ptr<T *> {
        typedef T type;
    };


    template <typename T, const ssize_t last = UNKNOWN_SIZE, typename E = void>
    struct PythonClassWrapper;

    /**
     * Class common to all C++ wrapper classes
     **/
    struct CommonBaseWrapper {

        struct Base {
            PyObject_HEAD
            /*Per Python API docs*/

            typedef PyTypeObject *TypePtr_t;

            static TypePtr_t constexpr TypePtr = &PyBaseObject_Type;

        } baseClass;

        CommonBaseWrapper() : _referenced(nullptr) {
        }

        void make_reference(PyObject *obj) {
            if (_referenced) { Py_DECREF(_referenced); }
            if (obj) { Py_INCREF(obj); }
            _referenced = obj;
        }

        PyObject *getReferenced() {
            return _referenced;
        }

    protected:
        PyObject *_referenced;

    };


    struct PtrWrapperBaseBase : public CommonBaseWrapper {
        static PyObject *parent_module;

        static int initialize(const char *const name, const char *const module_entry_name,
                              PyObject *module, const char *const fullname,
                              PyCFunction addr_method, PyCFunctionWithKeywords element_method, PyTypeObject &Type) {
            if (!name)
                return -1;

            std::string ptr_name = std::string(fullname ? fullname : name);
            Type.tp_methods[0].ml_meth = addr_method;
            Type.tp_methods[1].ml_meth = (PyCFunction) element_method;

            char *new_name = new char[ptr_name.size() + 1];
            strcpy(new_name, ptr_name.c_str());
            Type.tp_name = new_name;

            if (PyType_Ready(&Type) < 0) {
                PyErr_Print();
                return -1;
            }
            //reinterpret to avoid compiler warning:
            PyObject *const type = reinterpret_cast<PyObject *>(&Type);
            Py_INCREF(type);
            if (module == nullptr)
                return 0;
            PyModule_AddObject(module, module_entry_name, (PyObject *) &Type);
            parent_module = module;

            return 0;
        }


        template<typename T>
        T *get_CObject() {
            return (T *) _all_content._untyped_content;//ObjectHelper<T>::get_CObject(this);
        }

        template<typename Tptr>
        static PyObject *_addr(PyObject *self, PyObject *args, Tptr &cobj) {
            (void) self;
            try {
                if ((args && PyTuple_Size(args) > 0)) {
                    PyErr_BadArgument();
                    return nullptr;
                }
                if (ptr_depth<Tptr>::value > MAX_PTR_DEPTH)
                    throw "Maximum pointer depth on dynamic pointer-type creation reached";
                PyObject *pyobj = nullptr;
                pyobj = toPyObject<typename PtrTo<Tptr>::type>((typename PtrTo<Tptr>::type) &cobj, false, 1);

                PyErr_Clear();
                return pyobj;
            } catch (const char *const msg) {
                PyErr_SetString(PyExc_RuntimeError, msg);
                return nullptr;
            }
        }

        template<typename Tptrtype>
        inline static PyObject *addr(PyObject *self, PyObject *args) {
            try {
                typedef typename extent_as_pointer<Tptrtype>::type Tptr;
                Tptr *obj = PtrTo<Tptr>::cast(
                        (Tptr *) &((reinterpret_cast<PtrWrapperBaseBase *>(self))->_all_content._untyped_content));
                return PtrWrapperBaseBase::_addr<Tptr>(self, args, *obj);
            } catch (const char *const msg) {
                PyErr_SetString(PyExc_RuntimeError, msg);
                return nullptr;
            }
        }


        template<typename Tptr, typename Deallocator>
        static void _dealloc(PtrWrapperBaseBase *self) {
            //TODO: track dynamically allocated content and free if so
            if (self != nullptr) {
                Deallocator::_free(self);
                if (self->_referenced)
                    Py_DECREF(self->_referenced);
                self->_referenced = nullptr;
                self->_all_content._untyped_content = nullptr;
                self->_all_content._func_content = nullptr;
                self->baseClass.ob_type->tp_free((PyObject *) self);
                if (self->_referenced) {
                    Py_XDECREF(self->_referenced);
                    self->_referenced = nullptr;
                }

            }
        }

        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
            (void) args;
            (void) kwds;
            PtrWrapperBaseBase *self;

            self = (PtrWrapperBaseBase *) type->tp_alloc(type, 0);
            if (self != nullptr) {
                self->_all_content._untyped_content = nullptr;
                self->_all_content._func_content = nullptr;
            }
            return (PyObject *) self;

        }

        static int _init(PtrWrapperBaseBase *self, PyObject *args, PyObject *kwds, PyTypeObject *pytypeobj,
                         const ssize_t max) {
            int status = -1;
            if (!self) return status;
            self->_referenced = nullptr;
            self->_max = max;
            self->_allocated = false;
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
                    if (max > 0) {
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
                    self->_all_content = reinterpret_cast<PtrWrapperBaseBase *>(pyobj)->_all_content;
                    status = self->_all_content._untyped_content ? 0 : -1;
                } else {
                    PyErr_SetString(PyExc_TypeError, "Mismatched types when assigning pointer");
                    status = ERROR_TYPE_MISMATCH;
                    goto onerror;
                }
            } else {
                self->_allocated = true;
                self->_all_content._untyped_content = nullptr;
                self->_all_content._func_content = nullptr;
                status = 0;
            }
            onerror:
            return status;
        }


        static Py_ssize_t get_array_index(PtrWrapperBaseBase *self, PyObject *args, PyObject *kwargs) {
            static const char *kwlist[] = {"index", nullptr};
            long index = -1;

            if (!PyArg_ParseTupleAndKeywords(args, kwargs, "l", (char **) kwlist, &index)) {
                PyErr_SetString(PyExc_TypeError, "Unable to parse index");
                return INVALID_INDEX;
            }

            index = (index < 0 && self->_max >= 0) ? self->_max - index + 1 : index;
            if ((self->_max >= 0 && index > self->_max) || index < 0) {
                PyErr_SetString(PyExc_IndexError, "Index out of bounds");
                return INVALID_INDEX;
            }
            return index;
        }

        char *_raw_storage;

    protected:

        template<typename T, typename E = void>
        struct PtrTo;

        //limit pointer depth:
        template<typename T>
        struct PtrTo<T, typename std::enable_if<
                (!std::is_function<T>::value) && (ptr_depth<T>::value < MAX_PTR_DEPTH)>::type> {
        public:
            typedef T *type;

            static T *cast(T const *t) { return const_cast<T *>(t); }
        };

        template<typename T>
        struct PtrTo<T, typename std::enable_if<(ptr_depth<T>::value >= MAX_PTR_DEPTH)>::type> {
        public:
            typedef void **type;

            static T *cast(T const *t) {
                (void) t;
                return nullptr;
            }
        };


        template<typename ReturnType, typename ...Args>
        class PtrTo<ReturnType(Args...), void> {
        public:
            typedef ReturnType(*functype)(Args...);

            typedef functype *type;

            static functype cast(functype t) { return t; }
        };


        typedef void(*generic_func_ptr)();

        typedef generic_func_ptr *gfuncptrptr;
        typedef void *voidptr;

    public:
        union {
            void *_untyped_content;
            gfuncptrptr _func_content;
        } _all_content;
        ssize_t _max;
        bool _allocated;
    };

    PyObject *PtrWrapperBaseBase::parent_module = nullptr;


    template<typename T, bool is_array, const ssize_t array_size, typename E = void>
    PyObject *set_array_values(T values, const ssize_t size, PyObject *fromTuple, PyObject *referenced);

    template<typename T, bool is_array, const ssize_t array_size, typename std::enable_if<std::is_assignable<T, T>::value>::type>
    PyObject *set_array_values(T *values, const ssize_t size, PyObject *from, PyObject *referenced) {
        if (!PyList_Check(referenced) || PyList_Size(referenced) != size) {
            PyErr_SetString(PyExc_RuntimeError, "Internal error setting array elements");
        }
        if (PyList_Check(from)) {

            //have python list to set from
            if (PyList_Size(from) != size) {
                PyErr_SetString(PyExc_TypeError, "Invalid array size");
                return nullptr;
            }
            for (int i = 0; i < size; ++i) {
                values[i] = *toCObject<T, is_array, true>(PyList_GetItem(from, i));
            }
        } else if (PythonClassWrapper<T *>::checkType(from)) {
            //TODO: check array size on from????
            for (int i = 0; i < size; ++i) {
                values[i] = ((T *) ((PythonClassWrapper < T * > *)
                from)->_all_content._untyped_content)[i];
            }
        } else if (PythonClassWrapper<T[]>::checkType(from)) {
            //TODO: check array size on from????
            for (int i = 0; i < size; ++i) {
                values[i] = ((T *) ((PythonClassWrapper < T[] > *)
                from)->_all_content._untyped_content)[i];
            }
        } else if (array_size > 0 && size > 0 &&
                PythonClassWrapper<T[array_size]>::checkType(from)) {
            if (array_size != size) {
                PyErr_SetString(PyExc_RuntimeError, "Mistmached array lengths");
                return nullptr;
            }
            for (int i = 0; i < size; ++i) {
                values[i] = ((T *) ((PythonClassWrapper < T[array_size] > *)
                from)->_all_content._untyped_content)[i];
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid argument type");
            return nullptr;
        }
        return Py_None;
    }

    template<>
    PyObject *set_array_values<const char **, false, -1, void>(const char **values, const ssize_t size, PyObject *from,
                                                               PyObject *referenced) {
        if (!PyList_Check(referenced) || PyList_Size(referenced) != size) {
            PyErr_SetString(PyExc_RuntimeError, "Internal error setting array elements");
        }
        if (PyList_Check(from)) {

            //have python list to set from
            if (PyList_Size(from) != size) {
                PyErr_SetString(PyExc_TypeError, "Invalid array size");
                return nullptr;
            }
            for (int i = 0; i < size; ++i) {
                PyObject *item = PyList_GetItem(from, i);
                if (!PyString_Check(item)) {
                    return nullptr;
                }
                char *asstr = PyString_AsString(item);

                if (!asstr) {
                    PyErr_SetString(PyExc_TypeError, "Not a string on array elements assignment");
                    return nullptr;
                } else {
                    //make copy and keep reference
                    PyObject *pystr = PyString_FromString(asstr);
                    PyList_SetItem(referenced, i, pystr);
                    values[i] = (const char *const) PyString_AsString(pystr);
                }
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid argument type");
            return nullptr;
        }
        return Py_None;
    }
}
#endif
