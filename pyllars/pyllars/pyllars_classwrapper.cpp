#ifndef PYLLARS__CLASSWRAPPER_IMPL
#define PYLLARS__CLASSWRAPPER_IMPL

#include <limits>
#include <string.h>

#include "pyllars_classwrapper.hpp"

#include "pyllars_utils.hpp"
#include "pyllars_classmethodsemantics.hpp"
#include "pyllars_constmethodcallsemantics.cpp"
#include "pyllars_methodcallsemantics.hpp"
#include "pyllars_object_lifecycle.hpp"
#include "pyllars_conversions.cpp"

constexpr int UNKNOWN_SIZE = __pyllars_internal::UNKNOWN_SIZE;
namespace __pyllars_internal{
    template<typename T, typename E>
    class InitHelper {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
    };

    /**
     * Specialization for integers
     **/
    template<typename T>
    class InitHelper<T, typename std::enable_if<std::is_integral<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
    };

    /**
     * Specialization for floating point
     **/
    template<typename T>
    class InitHelper<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };

    //specialize for non-numeric fundamental types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            !std::is_void<T>::value && !std::is_arithmetic<T>::value && std::is_fundamental<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };

    //specialize for non-numeric fundamental types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<std::is_void<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };


    //specialize for pointer types:
    template<typename T>
    class InitHelper<T,  typename std::enable_if<!std::is_void<T>::value && !std::is_arithmetic<T>::value &&
                                                std::is_pointer<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };

    //specialize for copiable non-fundamental reference types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<!std::is_void<T>::value &&
                                                !std::is_integral<typename std::remove_reference<T>::type>::value &&
                                                !std::is_floating_point<typename std::remove_reference<T>::type>::value &&
                                                std::is_copy_constructible<typename std::remove_reference<T>::type>::value &&
                                                std::is_reference<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };

    //  specialize for non-copiable non-fundamental reference types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            !std::is_integral<typename std::remove_reference<T>::type>::value &&
            !std::is_floating_point<typename std::remove_reference<T>::type>::value &&
            !std::is_copy_constructible<typename std::remove_reference<T>::type>::value &&
            std::is_reference<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };

    //specialize for integral reference types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            std::is_integral<typename std::remove_reference<T>::type>::value &&
            std::is_reference<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };

    //specialize for floating point reference types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            std::is_floating_point<typename std::remove_reference<T>::type>::value &&
            std::is_reference<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
    };

    //specialize for other complex types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            !std::is_arithmetic<T>::value && !std::is_reference<T>::value &&
            !std::is_pointer<T>::value && !std::is_fundamental<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };

   template<typename T>
    PyObject *PythonClassWrapper<T,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            parent_module = nullptr;
    template<typename T>
    std::vector<PyMethodDef> PythonClassWrapper<T,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            _methodCollection = std::vector<PyMethodDef>(emptyMethods, emptyMethods + 1);
    template<typename T>
    std::vector<PyTypeObject *> PythonClassWrapper<T,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            _baseClasses = std::vector<PyTypeObject *>();
    template<typename T>
    std::map<std::string, typename MethodContainer<T>::setter_t>
            PythonClassWrapper<T,
                    typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            _memberSettersDict = std::map<std::string, typename MethodContainer<T>::setter_t>();
    template<typename T>
    std::vector<typename PythonClassWrapper<T, typename std::enable_if<
            !std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
    ConstructorContainer>
            PythonClassWrapper<T,
                    typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            _constructors;


    template<typename T>
    std::string PythonClassWrapper<T,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::_name;
    template<typename T>
    std::string PythonClassWrapper<T,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::_module_entry_name;
    template<typename T>
    std::string PythonClassWrapper<T,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::_full_name;
    template<typename T>
    PyTypeObject PythonClassWrapper<T,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            Type = {

                    PyObject_HEAD_INIT(nullptr)
                    0,                         /*ob_size*/
                    nullptr,             /*tp_name*/ /*filled on init*/
                    sizeof(PythonClassWrapper),             /*tp_basicsize*/
                    0,                         /*tp_itemsize*/
                    (destructor) PythonClassWrapper::_dealloc, /*tp_dealloc*/
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
                    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
                    "PythonClassWrapper object",           /* tp_doc */
                    nullptr,                       /* tp_traverse */
                    nullptr,                       /* tp_clear */
                    nullptr,                       /* tp_richcompare */
                    0,                               /* tp_weaklistoffset */
                    nullptr,                       /* tp_iter */
                    nullptr,                       /* tp_iternext */
                    PythonClassWrapper::_methodCollection.data(),             /* tp_methods */
                    nullptr,             /* tp_members */
                    nullptr,                         /* tp_getset */
                    Base::TypePtr,                         /* tp_base */
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

template<typename T>
int __pyllars_internal::InitHelper<T, typename std::enable_if<std::is_integral<T>::value>::type>::init
        (PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
    if (!self) {
        return -1;
    }
    static const char *kwlist[] = {"value", nullptr};
    if (std::is_unsigned<T>::value) {
        unsigned long long value;
        if (!PyArg_ParseTupleAndKeywords(args, kwds, "L", (char **) kwlist,
                                         &value))
            return -1;
        if (value < (unsigned long long) std::numeric_limits<T>::min() ||
            value > (unsigned long long) std::numeric_limits<T>::max()) {
            PyErr_SetString(PyExc_OverflowError, "Integer value out of range of int");
            return -1;
        }
        const T val = (T) value;
        self->_CObject = new ObjContainerProxy<T, T>(val);
    } else {
        long long value;
        fprintf(stderr, "%ld", PyLong_AsLong(PyTuple_GetItem(args, 0)));
        if (!PyArg_ParseTupleAndKeywords(args, kwds, "K", (char **) kwlist,
                                         &value))
            return -1;
        if (value < (long long) std::numeric_limits<T>::min() ||
            value > (long long) std::numeric_limits<T>::max()) {
            PyErr_SetString(PyExc_OverflowError, "Integer value out of range of int");
            return -1;
        }
        const T val = (T) value;
        self->_CObject = new ObjContainerProxy<T, T>(val);
    }
    return 0;
}

/**
 * Specialization for floating point
 **/
template<typename T>
int __pyllars_internal::InitHelper<T, typename std::enable_if<std::is_floating_point<T>::value>::type> ::init
            (PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
    static const char *kwlist[] = {"value", nullptr};
    if (!self) {
        return -1;
    }

    double value;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "d", (char **) kwlist,
                                     &value))
        return -1;
    if (value < std::numeric_limits<T>::min() || value > std::numeric_limits<T>::max()) {
        PyErr_SetString(PyExc_OverflowError, "Integer value out of range of int");
        return -1;
    }
    T val = (T) value;
    self->_CObject = new ObjContainerProxy<T, T>(val);


    return 0;
}

//specialize for non-numeric fundamental types:
template<typename T >
int  __pyllars_internal::InitHelper<T, typename std::enable_if<
                !std::is_void<T>::value && !std::is_arithmetic<T>::value && std::is_fundamental<T>::value>::type> ::init
            (PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
    if (!self) {
        return -1;
    }
    for (auto it = PythonClassWrapper<T>::_constructors.begin();
         it != PythonClassWrapper<T>::_constructors.end(); ++it) {
        try {
            if ((*it)(args, kwds, self->_CObject)) {
                break;
            }
        } catch (...) {
        }
        PyErr_Clear();
    }
    if (self->_CObject == nullptr) {
        static const char *kwdlist[] = {"value", nullptr};
        PyObject *pyobj;
        if (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char **) kwdlist, pyobj)) {
            if (!PythonClassWrapper<T>::checkType(pyobj)) {
                PyErr_Print();
                PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                return -1;
            }
            self->_CObject = new typename std::remove_reference<T>::type
                    (*(reinterpret_cast<PythonClassWrapper <T> *>(pyobj)->template get_CObject<T>()));
        } else if ((!kwds || PyDict_Size(kwds) == 0) && (!args || PyTuple_Size(args) == 0)) {
            self->_CObject = new typename std::remove_reference<T>::type();
            memset(self->_CObject, 0, sizeof(typename std::remove_reference<T>::type));
        }
    } else if ((!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
        if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
            PyErr_Clear();
        } else {
            PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
            return -1;
        }
    }
    if (self->_CObject == nullptr) {
        PyErr_SetString(PyExc_TypeError, "Invalid argment(s) to constructor");
        return -1;
    }
    return 0;
}

//specialize for non-numeric fundamental types:
template<typename T >
int __pyllars_internal::InitHelper<T, typename std::enable_if<std::is_void<T>::value>::type>::init
                (PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
    if (!self) {
        return -1;
    }
    for (auto it = PythonClassWrapper<T>::_constructors.begin();
         it != PythonClassWrapper<T>::_constructors.end(); ++it) {
        try {
            if ((*it)(args, kwds, self->_CObject)) {
                break;
            }
        } catch (...) {
        }
        PyErr_Clear();
    }
    if (self->_CObject == nullptr) {
        static const char *kwdlist[] = {"value", nullptr};
        PyObject *pyobj = Py_None;
        if (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char **) kwdlist, pyobj)) {
            if (!PythonClassWrapper<T>::checkType(pyobj)) {
                PyErr_Print();
                PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                return -1;
            }
            self->_CObject = nullptr;
        } else if ((!kwds || PyDict_Size(kwds) == 0) && (!args || PyTuple_Size(args) == 0)) {
            self->_CObject = nullptr;

        }
    } else if ((!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
        if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
            PyErr_Clear();
        } else {
            PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
            return -1;
        }
    }
    if (self->_CObject == nullptr) {
        PyErr_SetString(PyExc_TypeError, "Invalid argment(s) to constructor");
        return -1;
    }
    return 0;
}


//specialize for pointer types:
template<typename T >
int __pyllars_internal::InitHelper<T,  typename std::enable_if<!std::is_void<T>::value && !std::is_arithmetic<T>::value &&
                                                          std::is_pointer<T>::value>::type> ::init
        (PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
    if (!self) {
        return -1;
    }
    self->_CObject = nullptr;

    for (auto it = PythonClassWrapper<T>::_constructors.begin();
         it != PythonClassWrapper<T>::_constructors.end(); ++it) {
        try {
            if ((*it)(args, kwds, self->_CObject)) {
                break;
            }
        } catch (...) {
        }
        PyErr_Clear();
    }

    if (self->_CObject == nullptr) {
        static const char *kwdlist[] = {"value", nullptr};
        PyObject pyobj;
        PythonClassWrapper<T> *pyclass = reinterpret_cast<PythonClassWrapper<T> *>(&pyobj);
        if (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char **) kwdlist, &pyobj)) {
            if (!PythonClassWrapper<T>::checkType(&pyobj)) {
                PyErr_SetString(PyExc_TypeError, "Invalid type to constructor");
                return -1;
            }
            self->_CObject = pyclass->template get_CObject<T>();
        } else if ((!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
            if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
                PyErr_Clear();
            } else {
                PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                return -1;
            }
        } else {
            PyErr_Print();
            PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
            return -1;
        }
    }
    return 0;
}

//specialize for copiable non-fundamental reference types:
template<typename T >
int __pyllars_internal::InitHelper<T, typename std::enable_if<!std::is_void<T>::value &&
        !std::is_integral<typename std::remove_reference<T>::type>::value &&
        !std::is_floating_point<typename std::remove_reference<T>::type>::value &&
        std::is_copy_constructible<typename std::remove_reference<T>::type>::value &&
        std::is_reference<T>::value>::type>::init
        (PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
    if (!self) {
        return -1;
    }
    self->_CObject = nullptr;
    for (auto it = PythonClassWrapper<T>::_constructors.begin();
         it != PythonClassWrapper<T>::_constructors.end(); ++it) {
        try {
            if ((*it)(args, kwds, self->_CObject, false)) {
                break;
            }
        } catch (...) {
        }
        PyErr_Clear();
    }

    if (self->_CObject == nullptr) {
        if ((!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
            if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
                PyErr_Clear();
            } else {
                PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                return -1;
            }
        } else {
            PyErr_Print();
            PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
            return -1;
        }
    }
    return 0;
}

//  specialize for non-copiable non-fundamental reference types:
template<typename T >
int __pyllars_internal::InitHelper<T, typename std::enable_if<
                !std::is_integral<typename std::remove_reference<T>::type>::value &&
                !std::is_floating_point<typename std::remove_reference<T>::type>::value &&
                !std::is_copy_constructible<typename std::remove_reference<T>::type>::value &&
                std::is_reference<T>::value>::type> ::init
        (PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
    if (!self) {
        return -1;
    }
    self->_CObject = nullptr;
    for (auto it = PythonClassWrapper<T>::_constructors.begin();
         it != PythonClassWrapper<T>::_constructors.end(); ++it) {
        try {
            if ((*it)(args, kwds, self->_CObject)) {
                break;
            }
        } catch (...) {
        }
        PyErr_Clear();
    }

    if (self->_CObject == nullptr) {

        if ((!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
            if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
                PyErr_Clear();
            } else {
                PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                return -1;
            }
        } else {
            PyErr_Print();
            PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
            return -1;
        }
    }
    return 0;
}

//specialize for integral reference types:
template<typename T >
int  __pyllars_internal::InitHelper<T, typename std::enable_if<
                std::is_integral<typename std::remove_reference<T>::type>::value &&
                std::is_reference<T>::value>::type> ::init
        (PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
    typedef typename std::remove_reference<T>::type T_NoRef;
    if (!self) {
        return -1;
    }
    self->_CObject = nullptr;
    int status = 0;
    if (self->_CObject == nullptr) {
        static const char *kwdlist[] = {"value", nullptr};
        if (std::is_signed<T_NoRef>::value) {
            long long intval = 0;

            if (!PyArg_ParseTupleAndKeywords(args, kwds, "L", (char **) kwdlist, &intval)) {
                PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                status = -1;
                goto onerror;
            }
            if (intval < (long long) std::numeric_limits<T_NoRef>::min() ||
                intval > (long long) std::numeric_limits<T_NoRef>::max()) {
                PyErr_SetString(PyExc_TypeError, "Argument value out of range");
                status = -1;
                goto onerror;
            }
            T_NoRef trueval = (T_NoRef) intval;
            self->_CObject = new ObjContainerProxy<T_NoRef, T_NoRef>(trueval);

        } else {
            unsigned long long intval = 0;

            if (!PyArg_ParseTupleAndKeywords(args, kwds, "K", (char **) kwdlist, &intval)) {
                PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                status = -1;
                goto onerror;
            }
            if (intval < (unsigned long long) std::numeric_limits<T_NoRef>::min() ||
                intval > (unsigned long long) std::numeric_limits<T_NoRef>::max()) {
                PyErr_SetString(PyExc_TypeError, "Argument value out of range");
                status = -1;
                goto onerror;
            }
            T_NoRef trueval = (T_NoRef) intval;
            self->_CObject = new ObjContainerProxy<T_NoRef, T_NoRef>(trueval);

        }
    }
    onerror:
    if (status != 0 && (!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
        if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
            PyErr_Clear();
            status = 0;
        } else {
            if ((!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0))
                PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
            status = -1;
        }
    }
    return status;
}

//specialize for floating point reference types:
template<typename T >
int __pyllars_internal::InitHelper<T, typename std::enable_if<
                std::is_floating_point<typename std::remove_reference<T>::type>::value &&
                std::is_reference<T>::value>::type> ::init
        (PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
    typedef typename std::remove_reference<T>::type T_NoRef;
    if (!self) {
        return -1;
    }


    if (self->_CObject == nullptr) {
        static const char *kwdlist[] = {"value", nullptr};
        double intval = 0;

        if (!PyArg_ParseTupleAndKeywords(args, kwds, "d", (char **) kwdlist, &intval)) {
            PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
            return -1;
        }
        if (intval < (double) std::numeric_limits<T_NoRef>::min() ||
            intval > (double) std::numeric_limits<T_NoRef>::max()) {
            PyErr_SetString(PyExc_TypeError, "Argument value out of range");
            return -1;
        }
        self->_CObject = new ObjContainerProxy<T_NoRef, T_NoRef>((T_NoRef) intval);

    }
    return 0;
}

//specialize for other complex types:
template<typename T >
int __pyllars_internal::InitHelper<T, typename std::enable_if<
                !std::is_arithmetic<T>::value && !std::is_reference<T>::value &&
                !std::is_pointer<T>::value && !std::is_fundamental<T>::value>::type>::init
        (PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
    if (!self) {
        return -1;
    }
    self->_CObject = nullptr;
    for (auto it = PythonClassWrapper<T>::_constructors.begin();
         it != PythonClassWrapper<T>::_constructors.end(); ++it) {
        try {
            if ((*it)(args, kwds, self->_CObject, false)) {
                self->_allocated = (self->_CObject != nullptr);
                self->_inPlace = false;
                self->_arraySize = 0;
		self->_depth = 0;
                return 0;
            }
        } catch (...) {
        }
        PyErr_Clear();
    }
    if ((!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
        if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
            PyErr_Clear();
        } else {
            PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
            return -1;
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
        return -1;
    }

    return 0;
}


/**
     * Class to define Python wrapper to C class/type
     *
    template<typename T>
    struct
            : public CommonBaseWrapper {
*/
template<typename T>
template<typename Z>
typename std::remove_reference<T>::type *__pyllars_internal::PythonClassWrapper<T,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::get_CObject() {
    return _CObject ? _CObject->ptr() : nullptr;
}


template<typename T>
int __pyllars_internal::PythonClassWrapper<T, 
                typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::initialize
                (const char *const name, const char *const module_entry_name,
                 PyObject *module, const char *const fullname) {
    if (!name || strlen(name) == 0) return -1;
    if (!module_entry_name || strlen(module_entry_name) == 0) return -1;
    if (!fullname || strlen(fullname) == 0) return -1;
    int status = 0;
    _name = name;
    _module_entry_name = module_entry_name;
    _full_name = fullname;
    if (Type.tp_name) {/*already initialized*/ return status; }
    char *tp_name = new char[strlen(fullname ? fullname : name) + 1];
    strcpy(tp_name, fullname ? fullname : name);
    Type.tp_name = tp_name;

    PyMethodDef pyMeth = {
            address_name,
            addr,
            METH_KEYWORDS,
            "C address of object"
    };
    _methodCollection.insert(_methodCollection.begin(), pyMeth);
    PyMethodDef pyMethAlloc = {
            alloc_name_,
            (PyCFunction) alloc,
            METH_KEYWORDS | METH_CLASS,
            "allocate arry of single dynamic instance of this class"
    };
    _methodCollection.insert(_methodCollection.begin(), pyMethAlloc);
    Type.tp_methods = _methodCollection.data();
    if (!_baseClasses.empty()) {
        Type.tp_bases = PyTuple_New(_baseClasses.size());
        Py_ssize_t index = 0;
        for (auto it = _baseClasses.begin(); it != _baseClasses.end(); ++it) {
            PyTuple_SET_ITEM(Type.tp_bases, index++, (PyObject *) *it);
        }
    }
    if (PyType_Ready(&Type) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to ready type!");
        PyErr_Print();
        status = -1;
        goto onerror;
    }
    {
        PyObject *const type = reinterpret_cast<PyObject *>(&Type);
        Py_INCREF(type);
        if (module != nullptr) {
            if (PyModule_AddObject(module, module_entry_name, type) == 0) {
                parent_module = module;
            } else {
                PyErr_Print();
                PyErr_SetString(PyExc_RuntimeError, "Failed to add type to module!");
            }
        }
    }
    onerror:
    return status;
}


template<typename T>
__pyllars_internal::PythonClassWrapper<T, 
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>
                 *__pyllars_internal::PythonClassWrapper<T,
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
createPy
        (const ssize_t arraySize, ObjContainer<T_NoRef> *const cobj, const bool isAllocated,
         const bool inPlace, PyObject *referencing, const size_t depth) {
    static PyObject *kwds = PyDict_New();
    static PyObject *emptyargs = PyTuple_New(0);
    PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);
    typedef PythonClassWrapper<T> PyWrapper;
    PyTypeObject* type_ = &Type;

    if (!type_->tp_name){
      PyErr_SetString(PyExc_RuntimeError, "Uninitialized type when creating object");
      return nullptr;
    }
    PythonClassWrapper *pyobj = (PythonClassWrapper *) PyObject_Call((PyObject *) &Type, emptyargs, kwds);
    pyobj->_CObject = cobj;//new ObjContainerPtrProxy<T_NoRef>(cobj, isAllocated);
    pyobj->_allocated = isAllocated;
    pyobj->_inPlace = inPlace;
    pyobj->_arraySize = 0;
    pyobj->_depth = 0;
    if (referencing) pyobj->_referenced = referencing;
    return pyobj;
}

template<typename T>
void __pyllars_internal::PythonClassWrapper<T, 
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
addConstructor(const char *const kwlist[], constructor c) {
    _constructors.push_back(ConstructorContainer(kwlist, c));
}

template<typename T>
template<typename ...Args>
bool __pyllars_internal::PythonClassWrapper<T,
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
create(const char *const kwlist[], PyObject *args, PyObject *kwds, ObjContainer<T_NoRef> *&cobj, const bool inPlace) {
    try {
        return _createBase(cobj, args, kwds, kwlist, typename argGenerator<sizeof...(Args)>::type(),
                           (typename std::remove_reference<Args>::type *) nullptr...);

    } catch (const char *const msg) {
        PyErr_SetString(PyExc_RuntimeError, msg);
        PyErr_Print();
        return false;
    }
}

template<typename T>
PyObject *__pyllars_internal::PythonClassWrapper<T,
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
addr(PyObject *self, PyObject *args) {
    if ((args && PyTuple_Size(args) > 0)) {
        PyErr_BadArgument();
        return nullptr;
    }
    std::string ptr_name = std::string(Type.tp_name) + "*";
    std::string module_ptr_name = std::string(Type.tp_name) + "_ptr";
    PythonClassWrapper<T_NoRef *>::initialize();
    PythonClassWrapper *self_ = reinterpret_cast<PythonClassWrapper *>(self);
    PythonClassWrapper<T_NoRef *> * obj = (PythonClassWrapper<T_NoRef *> * ) (self_->_CObject ? toPyObject<T_NoRef *>(self_->_CObject->ptr(), AS_VARIABLE, 1) : Py_None);
     PyErr_Clear();
     obj->make_reference(self);
     return (PyObject*) obj;
}

template<typename T>
bool __pyllars_internal::PythonClassWrapper<T, 
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
_findMemberSetter(const char *const name) {
    return _memberSettersDict.find(name) != _memberSettersDict.end();
}

template<typename T>
template<const char *const name, typename ReturnType, typename ...Args>
void __pyllars_internal::PythonClassWrapper<T,
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
addClassMethod(ReturnType(*method)(Args...), const char *const kwlist[]) {
    static const char *const doc = "Call class method ";
    char *doc_string = new char[strlen(name) + strlen(doc) + 1];
    snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);

    PyMethodDef pyMeth = {
            name,
            (PyCFunction) ClassMethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::call,
            METH_KEYWORDS | METH_CLASS,
            doc_string
    };

    ClassMethodContainer<T>::template Container<name, ReturnType, Args...>::method = method;
    ClassMethodContainer<T>::template Container<name, ReturnType, Args...>::kwlist = kwlist;
    _addMethod(pyMeth);
}

template<typename T>
template<const char *const name, typename ReturnType, typename ...Args>
void __pyllars_internal::PythonClassWrapper<T, 
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
addMethod(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]) {
    static const char *const doc = "Call method ";
    char *doc_string = new char[strlen(name) + strlen(doc) + 1];
    snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);

    PyMethodDef pyMeth = {
            name,
            (PyCFunction) MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::call,
            METH_KEYWORDS,
            doc_string
    };

    MethodContainer<T>::template Container<name, ReturnType, Args...>::method = method;
    MethodContainer<T>::template Container<name, ReturnType, Args...>::kwlist = kwlist;
    _addMethod(pyMeth);
}

template<typename T>
template<const char *const name, typename ReturnType, typename ...Args>
void __pyllars_internal::PythonClassWrapper<T, 
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
addConstMethod( typename ConstMethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]) {
    static const char *const doc = "Call method ";
    char *doc_string = new char[strlen(name) + strlen(doc) + 1];
    snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);

    PyMethodDef pyMeth = {
            name,
            (PyCFunction) ConstMethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::call,
            METH_KEYWORDS,
            doc_string
    };

    ConstMethodContainer<T>::template Container<name, ReturnType, Args...>::method = method;
    ConstMethodContainer<T>::template Container<name, ReturnType, Args...>::kwlist = kwlist;
    _addMethod(pyMeth);
}

template<typename T>
void __pyllars_internal::PythonClassWrapper<T, 
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
addClassMember(const char *const name, PyObject *pyobj) {
    if (!Type.tp_dict) {
        Type.tp_dict = PyDict_New();
    }
    PyDict_SetItemString(Type.tp_dict, name, pyobj);
}

template<typename T>
void __pyllars_internal::PythonClassWrapper<T, 
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
addBaseClass(PyTypeObject *base) {
    if (!base) return;
    if (!Type.tp_base && _baseClasses.empty()) {
        Type.tp_base = base;
    } else {
        if (Type.tp_base) {
            _baseClasses.push_back(Type.tp_base);
            Type.tp_base = nullptr;
        }
        _baseClasses.insert(_baseClasses.begin(), base);
    }
}


template<typename T>
void __pyllars_internal::PythonClassWrapper<T,
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
set_contents(typename std::remove_reference<T>::type *ptr, const bool allocated, const bool inPlace) {
    _allocated = allocated;
    _inPlace = inPlace;
    _arraySize = 1;
    _depth = 0;
    _CObject = ptr;
}

template<typename T>
bool __pyllars_internal::PythonClassWrapper<T,
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
checkType(PyObject *const obj) {
    return PyObject_TypeCheck(obj, &Type);
}

template<typename T>
PyTypeObject *__pyllars_internal::PythonClassWrapper<T,
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
getType(const size_t unused_depth) {
            (void) unused_depth;
            return &Type;
}

template<typename T>
std::string __pyllars_internal::PythonClassWrapper<T,
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
get_name() { return _name; }

template<typename T>
std::string __pyllars_internal::PythonClassWrapper<T, 
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
get_module_entry_name() { return _module_entry_name; }

template<typename T>
std::string __pyllars_internal::PythonClassWrapper<T,
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
get_full_name() { return _full_name; }

template<typename T>   
PyObject *__pyllars_internal::PythonClassWrapper<T,
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
alloc(PyObject *cls, PyObject *args, PyObject *kwds) {
    return ObjectLifecycleHelpers::Alloc<T, PythonClassWrapper<T_NoRef *>, PythonClassWrapper>::allocbase
            (cls, args, kwds, _constructors);
}

template<typename T>
int __pyllars_internal::PythonClassWrapper<T,
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
_init(PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
    if (Type.tp_base && Base::TypePtr->tp_init) {
 	PyObject *empty = PyTuple_New(0);
        Base::TypePtr->tp_init((PyObject *) &self->baseClass, empty, nullptr);
        PyErr_Clear();
        Py_DECREF(empty);
    }
    self->_allocated = false;
    self->_inPlace = false;
    self->_arraySize = 0;
    self->_depth = 0;
    return InitHelper<T>::init(self, args, kwds);
}

template<typename T>
PyObject *__pyllars_internal::PythonClassWrapper<T, 
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    (void) args;
    (void) kwds;
    PythonClassWrapper *self;
    self = (PythonClassWrapper *) type->tp_alloc(type, 0);
    /*if( Type.tp_base && Base::TypePtr->tp_new){
      PyObject* empty = PyTuple_New(0);
      Base::TypePtr->tp_new( Type.tp_base, empty, nullptr);
      Py_DECREF(empty);
      }*/
    if (nullptr != self) {
        self->_CObject = nullptr;
    }
     return (PyObject *) self;
}

template<typename T>
void __pyllars_internal::PythonClassWrapper<T,
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
_free(void *self_) {
    PythonClassWrapper *self = (PythonClassWrapper *) self_;
    if (!self->_CObject) return;
    if (self->_allocated) {
        //Deallocator<T_NoRef>::dealloc(
        //        self->_CObject->ptr(), self->_inPlace, self->_arraySize);
        if(self->_allocated) delete self->_CObject;
    }
    if (self->_allocated) self->_CObject = nullptr;
    self->_allocated = false;
    self->_inPlace = false;
    self->_arraySize = 0;
    self->_depth = 0;
}

template<typename T>
void __pyllars_internal::PythonClassWrapper<T,
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
_dealloc(PythonClassWrapper *self) {
    if (!self) return;
    if (self->_referenced) {
        Py_XDECREF(self->_referenced);
        self->_referenced = nullptr;
    }
    _free(self);
}


template<typename T>
template<typename ...PyO>
bool __pyllars_internal::PythonClassWrapper<T,
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
_parsePyArgs(const char *const kwlist[], PyObject *args, PyObject *kwds, PyO *&...pyargs) {
    char format[sizeof...(PyO) + 1] = {0};
    if (sizeof...(PyO) > 0)
        memset(format, 'O', sizeof...(PyO));
    return sizeof...(PyO) == 0 || PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...);
}

template<typename T>
template<typename ...Args>
bool __pyllars_internal::PythonClassWrapper<T,
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
_createBaseBase(ObjContainer<T_NoRef> *&cobj,  Args &... args) {
    if (!cobj) {
        cobj = new ObjContainerProxy<T_NoRef, Args &...>(args...);
        return cobj != nullptr;
    }
    cobj->set_contents(args...);
      //new(cobj) ObjContainerProxy<T_NoRef, Args& ...>(args...);

    return true;
}

template<typename T>
template<typename ...Args,int ...S >
bool __pyllars_internal::PythonClassWrapper<T, 
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
_createBase( ObjContainer<T_NoRef> *&cobj, PyObject *args, PyObject *kwds,
             const char *const kwlist[], container<S...> unused1, Args *... unused2) {
    (void) unused1;
    void *unused[] = {(void *) unused2..., nullptr};
    (void) unused;
    if (args && PyTuple_Size(args) != sizeof...(Args)) {
        return false;
    }
    PyObject *pyobjs[sizeof...(Args) + 1];
    (void) pyobjs;
    if (!_parsePyArgs(kwlist, args, kwds, pyobjs[S]...)) {
        PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
        return false;
    }

    return _createBaseBase<Args...>(cobj,
                                    *toCObject<Args, false, PythonClassWrapper<Args> >(*pyobjs[S])...);
}

template<typename T>
void __pyllars_internal::PythonClassWrapper<T, 
        typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
_addMethod(PyMethodDef method) {
    //insert at beginning to keep null sentinel at end of list:
    _methodCollection.insert(_methodCollection.begin(), method);
    Type.tp_methods = _methodCollection.data();
}


#endif