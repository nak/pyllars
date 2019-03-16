#ifndef PYLLARS__CLASSWRAPPER_IMPL
#define PYLLARS__CLASSWRAPPER_IMPL

#include <limits>
#include <string.h>
#include <utility>

#include "pyllars_classwrapper.hpp"
#include "pyllars_utils.hpp"

#include "pyllars_classmethodsemantics.impl.hpp"
#include "pyllars_classmembersemantics.impl.hpp"
#include "pyllars_methodcallsemantics.impl.hpp"
#include "pyllars_object_lifecycle.impl.hpp"
#include "pyllars_conversions.impl.hpp"


namespace __pyllars_internal {

    //static PyMethodDef emptyMethods[] = {{nullptr, nullptr, 0, nullptr}};

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
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
    };

    //specialize for non-numeric fundamental types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            !std::is_void<T>::value && !std::is_arithmetic<T>::value && std::is_fundamental<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
    };

    //specialize for non-numeric fundamental types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<std::is_void<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
    };


    //specialize for pointer types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<!std::is_void<T>::value && !std::is_arithmetic<T>::value &&
                                                std::is_pointer<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
    };

    //specialize for copiable non-fundamental reference types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<!std::is_void<T>::value &&
                                                !std::is_integral<typename std::remove_reference<T>::type>::value &&
                                                !std::is_floating_point<typename std::remove_reference<T>::type>::value &&
                                                std::is_copy_constructible<typename std::remove_reference<T>::type>::value &&
                                                std::is_reference<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
    };

    //  specialize for non-copiable non-fundamental reference types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            !std::is_integral<typename std::remove_reference<T>::type>::value &&
            !std::is_floating_point<typename std::remove_reference<T>::type>::value &&
            !std::is_copy_constructible<typename std::remove_reference<T>::type>::value &&
            std::is_reference<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
    };

    //specialize for integral reference types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            std::is_integral<typename std::remove_reference<T>::type>::value &&
            std::is_reference<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
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
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
    };


    template<typename T>
    std::map<std::string, PyMethodDef> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _methodCollection = std::map<std::string, PyMethodDef>();

    template<typename T>
    std::vector<PyTypeObject *> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _baseClasses = std::vector<PyTypeObject *>();

    template<typename T>
    std::map<std::string, const typename std::remove_reference<T>::type *> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _classEnumValues = std::map<std::string, const T_NoRef *>();

    template<typename T>
    std::map<std::string, unaryfunc> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _unaryOperators = std::map<std::string, unaryfunc>();

    template<typename T>
    std::map<std::string, binaryfunc> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _binaryOperators = std::map<std::string, binaryfunc>();

    template<typename T>
    std::vector<typename PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::ConstructorContainer>
            PythonClassWrapper<T,
                    typename std::enable_if<is_rich_class<T>::value>::type>::
            _constructors;

    template<typename T>
    std::map<std::string, _getattrfunc>
            PythonClassWrapper<T,
                    typename std::enable_if<is_rich_class<T>::value>::type>::
            _member_getters;
    template<typename T>
    std::map<std::string, _setattrfunc>
            PythonClassWrapper<T,
                    typename std::enable_if<is_rich_class<T>::value>::type>::
            _member_setters;
    template<typename T>
    std::vector<_setattrfunc>
            PythonClassWrapper<T,
                    typename std::enable_if<is_rich_class<T>::value>::type>::
            _assigners;

    template<typename T>
    bool PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::_isInitialized = false;

    template<typename T>
    std::map<std::string, std::pair<std::function<PyObject *(PyObject *, PyObject *)>,
            std::function<int(PyObject *, PyObject *, PyObject *)>
    >
    >
            PythonClassWrapper<T,
                    typename std::enable_if<is_rich_class<T>::value>::type>::_mapMethodCollection;

    template<typename T>
    PyTypeObject PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _Type = {
#if PY_MAJOR_VERSION == 3
                    PyVarObject_HEAD_INIT(NULL, 0)
#else
            PyObject_HEAD_INIT(nullptr)
            0,                         /*ob_size*/
#endif
                    nullptr,             /*tp_name*/ /*filled on init*/
                    sizeof(PythonClassWrapper),             /*tp_basicsize*/
                    0,                         /*tp_itemsize*/
                    (destructor) PythonClassWrapper::_dealloc, /*tp_dealloc*/
                    nullptr,                         /*tp_print*/
                    _pyGetAttr,                         /*tp_getattr*/
                    _pySetAttr,                         /*tp_setattr*/
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
                    nullptr,                         /* tp_base */
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


    template<typename T>
    int InitHelper<T, typename std::enable_if<std::is_integral<T>::value>::type>::init
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
            self->_CObject = new ObjectContainerConstructed<T, T>((T) value);
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
            self->_CObject = new ObjectContainerConstructed<T, T>((T) value);
        }
        return 0;
    }

    /**
     * Specialization for floating point
     **/
    template<typename T>
    int InitHelper<T, typename std::enable_if<std::is_floating_point<T>::value>::type>::init
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
        self->_CObject = new ObjectContainerConstructed<T, T>((T) value);


        return 0;
    }

    //specialize for non-numeric fundamental types:
    template<typename T>
    int InitHelper<T, typename std::enable_if<
            !std::is_void<T>::value && !std::is_arithmetic<T>::value && std::is_fundamental<T>::value>::type>::init
            (PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
        if (!self) {
            return -1;
        }
        for (auto const&[kwlist, constructor] : PythonClassWrapper<T>::_constructors) {
            try {
                if (constructor(args, kwds, self->_CObject, false)) {
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
                        (*(reinterpret_cast<PythonClassWrapper<T> *>(pyobj)->get_CObject()));
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
    template<typename T>
    int InitHelper<T, typename std::enable_if<std::is_void<T>::value>::type>::init
            (PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
        if (!self) {
            return -1;
        }
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
    template<typename T>
    int InitHelper<T, typename std::enable_if<!std::is_void<T>::value && !std::is_arithmetic<T>::value &&
                                              std::is_pointer<T>::value>::type>::init
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
            static const char *kwdlist[] = {"value", nullptr};
            PyObject pyobj;
            PythonClassWrapper<T> *pyclass = reinterpret_cast<PythonClassWrapper<T> *>(&pyobj);
            if (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char **) kwdlist, &pyobj)) {
                if (!PythonClassWrapper<T>::checkType(&pyobj)) {
                    PyErr_SetString(PyExc_TypeError, "Invalid type to constructor");
                    return -1;
                }
                self->_CObject = pyclass->get_CObject();
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
    template<typename T>
    int InitHelper<T, typename std::enable_if<!std::is_void<T>::value &&
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
    template<typename T>
    int InitHelper<T, typename std::enable_if<
            !std::is_integral<typename std::remove_reference<T>::type>::value &&
            !std::is_floating_point<typename std::remove_reference<T>::type>::value &&
            !std::is_copy_constructible<typename std::remove_reference<T>::type>::value &&
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

    //specialize for integral reference types:
    template<typename T>
    int InitHelper<T, typename std::enable_if<
            std::is_integral<typename std::remove_reference<T>::type>::value &&
            std::is_reference<T>::value>::type>::init
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
                self->_CObject = new ObjectContainerConstructed<T_NoRef, T_NoRef>((T_NoRef) intval);

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
                self->_CObject = new ObjectContainerConstructed<T_NoRef, T_NoRef>((T_NoRef) intval);

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
    template<typename T>
    int InitHelper<T, typename std::enable_if<
            std::is_floating_point<typename std::remove_reference<T>::type>::value &&
            std::is_reference<T>::value>::type>::init
            (PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
        typedef typename std::remove_reference<T>::type T_NoRef;
        if (!self) {
            return -1;
        }


        if (self->_CObject == nullptr) {
            static const char *kwdlist[] = {"value", nullptr};
            double val = 0;

            if (!PyArg_ParseTupleAndKeywords(args, kwds, "d", (char **) kwdlist, &val)) {
                PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                return -1;
            }
            if (val < (double) std::numeric_limits<T_NoRef>::min() ||
                val > (double) std::numeric_limits<T_NoRef>::max()) {
                PyErr_SetString(PyExc_TypeError, "Argument value out of range");
                return -1;
            }
            self->value = val;//new ObjContainerProxy<T_NoRef, T_NoRef>((T_NoRef) intval);

        }
        return 0;
    }

    //specialize for other complex types:
    template<typename T>
    int InitHelper<T, typename std::enable_if<
            !std::is_arithmetic<T>::value && !std::is_reference<T>::value &&
            !std::is_pointer<T>::value && !std::is_fundamental<T>::value>::type>::init
            (PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
        if (!self) {
            return -1;
        }
        self->_CObject = nullptr;
        for (auto const &[kwlist, constructor] : PythonClassWrapper<T>::_constructors) {
            try {
                if ((self->_CObject = constructor(kwlist, args, kwds, ContainmentKind::CONSTRUCTED, nullptr))) {
                    self->_isInitialized = true;
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
    typename std::remove_reference<T>::type *PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::get_CObject() {
        return _CObject ? _CObject->ptr() : nullptr;
    }

    template<typename T>
    int PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    initialize() {
        typedef typename std::remove_volatile<
                typename std::remove_const<
                        typename std::remove_reference<T>::type
                >::type
        >::type basic_type;
        typedef PythonClassWrapper<basic_type> Basic;

        static bool inited = false;
        if (inited) return 0;
        inited = true;
        const char *const name = type_name<T>();
        if (!name || strlen(name) == 0) return -1;
        int status = 0;
        if (_Type.tp_name) {/*already initialized*/ return status; }
        char *tp_name = new char[strlen(name) + 1 + tp_name_prefix_len];
        strcpy(tp_name, tp_name_prefix);
        strcpy(tp_name + strlen(tp_name_prefix), name);
        _Type.tp_name = tp_name;

        PyMethodDef pyMethAlloc = {
                alloc_name_,
                (PyCFunction) alloc,
                METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                "allocate array of single dynamic instance of this class"
        };
        Basic::_methodCollection[alloc_name_] = pyMethAlloc;
        if (!Basic::_baseClasses.empty()) {
            _Type.tp_bases = PyTuple_New(Basic::_baseClasses.size());
            Py_ssize_t index = 0;
            for (auto const &baseClass: Basic::_baseClasses) {
                PyTuple_SetItem(_Type.tp_bases, index, (PyObject *) baseClass);
                // tp_bases not usable for inheritance of methods/membser as it doesn't really do the right thing and
                // causes problems on lookup of base classes,
                // so do this manually...
                {
                    PyMethodDef *def = baseClass->tp_methods;
                    if (Basic::_methodCollection.count(def->ml_name) == 0) {
                        while (def->ml_name != nullptr) {
                            Basic::_methodCollection[def->ml_name] = *def;
                            ++def;
                        }
                    }
                }
                {
                    auto *def = baseClass->tp_getset;
                    while (def->name != nullptr) {
                        if (Basic::_member_getters.count(def->name) == 0) {
                            Basic::_member_setters[def->name] = def->set;
                            Basic::_member_getters[def->name] = def->get;
                        }
                        ++def;
                    }
                }
            }
        }
        _Type.tp_methods = new PyMethodDef[_methodCollection.size() + 1];
        _Type.tp_methods[_methodCollection.size()] = {nullptr};
        size_t index = 0;
        for (auto const&[key, methodDef]: Basic::_methodCollection) {
            (void) key;
            _Type.tp_methods[index] = methodDef;
            ++index;
        }
        _Type.tp_getset = new PyGetSetDef[Basic::_member_getters.size() + 1];
        _Type.tp_getset[Basic::_member_getters.size()] = {0};
        index = 0;
        for (auto const&[key, getter]: Basic::_member_getters) {
            auto it = Basic::_member_setters.find(key);
            _setattrfunc setter = nullptr;
            if (it != Basic::_member_setters.end()) {
                setter = it->second;
            }
            _Type.tp_getset[index].name = key.c_str();
            _Type.tp_getset[index].get = getter;
            _Type.tp_getset[index].set = setter;
            _Type.tp_getset[index].doc = "get/set attribute";
            _Type.tp_getset[index].closure = nullptr;
            index++;
        }

        for (auto const&[name, func]: Basic::_unaryOperators) {
            static std::map<std::string, unaryfunc *> unary_mapping =
                    {{std::string(OP_UNARY_INV), &_Type.tp_as_number->nb_invert},
                     {std::string(OP_UNARY_NEG), &_Type.tp_as_number->nb_negative},
                     {std::string(OP_UNARY_POS), &_Type.tp_as_number->nb_positive}};

            if (unary_mapping.count(name) == 0) {
                status = -1;
                goto onerror;
            }
            *unary_mapping[name] = func;
        }

        for (auto const&[name, func]: Basic::_binaryOperators) {
            static std::map<std::string, binaryfunc *> binary_mapping =
                    {{OP_BINARY_ADD,     &_Type.tp_as_number->nb_add},
                     {OP_BINARY_AND,     &_Type.tp_as_number->nb_and},
                     {OP_BINARY_OR,      &_Type.tp_as_number->nb_or},
                     {OP_BINARY_XOR,     &_Type.tp_as_number->nb_xor},
                     {OP_BINARY_DIV,     &_Type.tp_as_number->nb_true_divide},
                     {OP_BINARY_MOD,     &_Type.tp_as_number->nb_remainder},
                     {OP_BINARY_MUL,     &_Type.tp_as_number->nb_multiply},
                     {OP_BINARY_LSHIFT,  &_Type.tp_as_number->nb_lshift},
                     {OP_BINARY_RSHIFT,  &_Type.tp_as_number->nb_rshift},
                     {OP_BINARY_SUB,     &_Type.tp_as_number->nb_subtract},
                     {OP_BINARY_IADD,    &_Type.tp_as_number->nb_inplace_add},
                     {OP_BINARY_IAND,    &_Type.tp_as_number->nb_inplace_and},
                     {OP_BINARY_IOR,     &_Type.tp_as_number->nb_inplace_or},
                     {OP_BINARY_IXOR,    &_Type.tp_as_number->nb_inplace_xor},
                     {OP_BINARY_IDIV,    &_Type.tp_as_number->nb_inplace_true_divide},
                     {OP_BINARY_IMOD,    &_Type.tp_as_number->nb_inplace_remainder},
                     {OP_BINARY_IMUL,    &_Type.tp_as_number->nb_inplace_multiply},
                     {OP_BINARY_ILSHIFT, &_Type.tp_as_number->nb_inplace_lshift},
                     {OP_BINARY_IRSHIFT, &_Type.tp_as_number->nb_inplace_rshift},
                     {OP_BINARY_ISUB,    &_Type.tp_as_number->nb_inplace_subtract},

                    };
            if (binary_mapping.count(name) == 0) {
                throw "Undefined operator name (internal error)";
            }
            *binary_mapping[name] = func;
        }


        if (!_Type.tp_base && _baseClasses.size() == 0) {
            if (PyType_Ready(&CommonBaseWrapper::_BaseType) < 0) {
                PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
                PyErr_Print();
                status = -1;
                goto onerror;
            }
            _Type.tp_base = &CommonBaseWrapper::_BaseType;
        }
        if (PyType_Ready(&_Type) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
            PyErr_Print();
            status = -1;
            goto onerror;
        }

        for (auto const&[name, value]: Basic::_classEnumValues) {
            // can only be called after ready of _Type:
            PyObject *pyval = toPyObject<T>(*const_cast<T *>(value), false, 1);
            if (pyval) {
                PyDict_SetItemString(_Type.tp_dict, name.c_str(), pyval);
            } else {
                status = -1;
                goto onerror;
            }
        }

        {
            PyObject *const type = reinterpret_cast<PyObject *>(&_Type);
            Py_INCREF(type);
        }
        onerror:
        _isInitialized = (status == 0);
        return status;
    }


    template<typename T>
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type> *
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    createPy(const ssize_t arraySize, T_NoRef &cobj, const ContainmentKind containmentKind, PyObject *referencing) {
        static PyObject *kwds = PyDict_New();
        static PyObject *emptyargs = PyTuple_New(0);
        if (containmentKind != ContainmentKind::BY_REFERENCE) {
            PyErr_SetString(PyExc_SystemError, "Invalid container type when create Python Wrapper to C object");
            return nullptr;
        }
        PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);
        PyTypeObject *type_ = getPyType();

        if (!type_->tp_name) {
            PyErr_SetString(PyExc_RuntimeError, "Uninitialized type when creating object");
            return nullptr;
        }
        PythonClassWrapper *pyobj = (PythonClassWrapper *) PyObject_Call((PyObject *) type_, emptyargs, kwds);
        if (pyobj) {
            pyobj->_CObject = new ObjectContainerReference<T>(cobj);
            if (referencing) pyobj->_referenced = referencing;
        }
        return pyobj;
    }

    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addConstructorBase(const char *const kwlist[], constructor_t c) {
        _constructors.push_back(ConstructorContainer(kwlist, c));
    }


    template<typename T>
    template<typename ...Args>
    ObjectContainer<T> *
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    create(const char *const kwlist[], PyObject *args, PyObject *kwds, const ContainmentKind containmentKind,
           unsigned char *location) {
        try {
            return _createBase<Args...>(args, kwds, kwlist, typename argGenerator<sizeof...(Args)>::type(),
                                        (_____fake<Args> *) nullptr...);

        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }

    template<typename T>
    PyObject *PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addr(PyObject *self, PyObject *args) {
        if ((args && PyTuple_Size(args) > 0)) {
            PyErr_BadArgument();
            return nullptr;
        }
        PythonClassWrapper<T_NoRef *>::initialize();
        PythonClassWrapper *self_ = reinterpret_cast<PythonClassWrapper *>(self);
        PyObject*obj;
        if(self_->_CObject){
            obj = toPyObject<T_NoRef *>(self_->_CObject->ptr(), AS_REFERNCE, 1);
            PyErr_Clear();
            Py_INCREF(self);
            ((PythonClassWrapper<T_NoRef *> *)obj)->_referenced = self;
        } else {
            obj = Py_None;
        }
        return (PyObject *) obj;
    }

    template<typename T>
    template<const char *const name, const char *const kwlist[], typename ReturnType, typename ...Args>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addClassMethod(ReturnType(*method)(Args...)) {
        static const char *const doc = "Call class method ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);

        PyMethodDef pyMeth = {
                name,
                (PyCFunction) ClassMethodContainer<T_NoRef>::template Container<false, name, kwlist, ReturnType, Args...>::call,
                METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                doc_string
        };

        ClassMethodContainer<T>::template Container<false, name, kwlist, ReturnType, Args...>::method = method;
        _addMethod(pyMeth);
    }

    template<typename T>
    template<const char *const name, const char *const kwlist[], typename ReturnType, typename ...Args>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addClassMethodVarargs(ReturnType(*method)(Args... ...)) {
        static const char *const doc = "Call class method ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);

        PyMethodDef pyMeth = {
                name,
                (PyCFunction) ClassMethodContainer<T_NoRef>::template Container<true, name, kwlist, ReturnType, Args...>::call,
                METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                doc_string
        };

        ClassMethodContainer<T>::template Container<true, name, ReturnType, Args...>::method = method;
        _addMethod(pyMeth);
    }


    template<typename T>
    template<typename _Container, bool is_const, const char *const kwlist[], typename ReturnType, typename ...Args>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addMethodTempl(typename _Container::template Container<is_const, kwlist, ReturnType, Args...>::method_t method) {
        static const char *const doc = "Call method ";
        char *doc_string = new char[strlen(_Container::name_) + strlen(doc) + 1];
        snprintf(doc_string, strlen(_Container::name_) + strlen(doc) + 1, "%s%s", doc, _Container::name_);

        PyMethodDef pyMeth = {
                _Container::name_,
                (PyCFunction) _Container::call,
                METH_KEYWORDS | METH_VARARGS,
                doc_string
        };

        _Container::template Container<is_const, kwlist, ReturnType, Args...>::setMethod(method);
        _addMethod(pyMeth);
    }


    template<typename T>
    template<const char *const name, bool is_const, typename Arg>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addUnaryOperator(
            typename MethodContainer<T_NoRef, name>::template Container<is_const, emptylist, Arg>::method_t method) {
        MethodContainer<T_NoRef, name>::template Container<is_const, emptylist, Arg>::setMethod(method);
        _unaryOperators[name] = (unaryfunc) MethodContainer<T_NoRef, name>::callAsUnaryFunc;
    }


    template<typename T>
    template<const char *const name, bool is_const, const char *const kwlist[2], typename ReturnType, typename Arg>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addBinaryOperator(
            typename MethodContainer<T_NoRef, name>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method) {
        MethodContainer<T_NoRef, name>::template Container<is_const, kwlist, ReturnType, Arg>::setMethod(method);
        _binaryOperators[name] = (binaryfunc) MethodContainer<T_NoRef, name>::callAsBinaryFunc;
    }

    template<typename T>
    PyObject *
    PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _mapGet(PyObject *self, PyObject *key) {
        PyObject *value = nullptr;
        for (auto const &[_, method]: _mapMethodCollection) {
            if ((value = method.first(self, key))) {
                PyErr_Clear();
                break;
            }
        }
        return value;
    }


    template<typename T>
    int PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _mapSet(PyObject *self, PyObject *key, PyObject *value) {
        int status = -1;
        for (auto const &[_, method]: _mapMethodCollection) {
            if ((status = method.second(self, key, value)) == 0) {
                PyErr_Clear();
                break;
            }
        }
        return status;
    }

    namespace {


        template<typename K, typename V>
        class Name {
        public:
            static const std::string name;
        };

        template<typename K, typename V>
        const std::string
                Name<K, V>::name = std::string(typeid(K).name()) + std::string(typeid(V).name());
    }


#include <functional>

    template<typename T>
    template<const char *const kwlist[], typename KeyType, typename ValueType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addMapOperatorMethod(
            typename MethodContainer<T_NoRef, operatormapname>::template Container<false, kwlist, ValueType, KeyType>::method_t method) {
        std::function<PyObject *(PyObject *, PyObject *)> getter = [method](PyObject *self,
                                                                            PyObject *item) -> PyObject * {
            PythonClassWrapper *self_ = (PythonClassWrapper *) self;
            try {
                auto c_key = toCArgument<KeyType, defaults_to_string<KeyType>::value>(*item);
                return toPyObject((self_->get_CObject()->*method)(*c_key), false, 1);
            } catch (const char *const msg) {
                PyErr_SetString(PyExc_TypeError, msg);
                return nullptr;
            }
        };
        std::function<int(PyObject *, PyObject *, PyObject *)> setter = [method](PyObject *self, PyObject *item,
                                                                                 PyObject *value) -> int {
            PythonClassWrapper *self_ = (PythonClassWrapper *) self;
            try {
                auto c_value = toCArgument<ValueType, defaults_to_string<ValueType>::value>(*value);
                auto c_key = toCArgument<KeyType, defaults_to_string<KeyType>::value>(*item);
                AssignValue<ValueType>::assign((self_->get_CObject()->*method)(*c_key), *c_value);
            } catch (const char *const msg) {
                PyErr_SetString(PyExc_TypeError, "Cannot assign to value of unrelated type.");
                return -1;
            }
            return 0;
        };

        const std::string name = Name<ValueType, KeyType>::name;
        _mapMethodCollection[name] = std::pair<std::function<PyObject *(PyObject *, PyObject *)>,
                std::function<int(PyObject *, PyObject *, PyObject *)>
        >(getter, setter);

        static PyMappingMethods methods = {nullptr, _mapGet, _mapSet};
        _Type.tp_as_mapping = &methods;
    }

    template<typename T>
    template<const char *const kwlist[], typename KeyType, typename ValueType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addMapOperatorMethodConst(
            typename MethodContainer<T_NoRef, operatormapname>::template Container<true, kwlist, ValueType, KeyType>::method_t method) {
        std::function<PyObject *(PyObject *, PyObject *)> getter = [method](PyObject *self,
                                                                            PyObject *item) -> PyObject * {
            PythonClassWrapper *self_ = (PythonClassWrapper *) self;
            try {
                auto c_key = toCArgument<KeyType, defaults_to_string<KeyType>::value>(*item);
                return toPyObject((self_->get_CObject()->*method)(*c_key), false, 1);
            } catch (const char *const msg) {
                PyErr_SetString(PyExc_TypeError, msg);
                return nullptr;
            }
        };

        std::function<int(PyObject *, PyObject *, PyObject *)> setter = [method](PyObject *self, PyObject *item,
                                                                                 PyObject *value) -> int {
            PyErr_SetString(PyExc_TypeError, "Unable to set value of const mapping");
            return 1;
        };

        const std::string name = Name<ValueType, KeyType>::name;
        //do not override a non-const with const version of operator[]
        if (!_mapMethodCollection.count(name)) {
            _mapMethodCollection[name] = std::pair<std::function<PyObject *(PyObject *, PyObject *)>,
                    std::function<int(PyObject *, PyObject *, PyObject *)>
            >(getter, setter);
        }
        static PyMappingMethods methods = {nullptr, _mapGet, _mapSet};
        _Type.tp_as_mapping = &methods;
    }


    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addClassMember(const char *const name, PyObject *pyobj) {
        if (!_Type.tp_dict) {
            _Type.tp_dict = PyDict_New();
        }
        PyDict_SetItemString(_Type.tp_dict, name, pyobj);
    }

    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addBaseClass(PyTypeObject *base) {
        if (!base) return;
        if (!_Type.tp_base && _baseClasses.empty()) {
            _Type.tp_base = base;
        } else {
            if (_Type.tp_base) {
                _baseClasses.push_back(_Type.tp_base);
                _Type.tp_base = nullptr;
            }
            _baseClasses.insert(_baseClasses.begin(), base);
        }
    }

    template<typename T>
    bool PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    checkType(PyObject *const obj) {
        return PyObject_TypeCheck(obj, &_Type);
    }

    template<typename T>
    PyObject *PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    alloc(PyObject *cls, PyObject *args, PyObject *kwds) {
        /**
         * Takes no kwds
         * args is of length 1
         * if arg is int value, allocate array of that size based on default constructor (or raise exc if no such constructor)
         * If arg is a tuple, use that as the arguments to a constructor for allocation of single item
         * If arg is a list of tuples, cosntrcut an array of that size of objects using each tuple as cosntructor args
         *  (allocation will be through generic byte buffer and then in-place-memory allocation)
         */
        if (kwds && PyDict_Size(kwds) != 0) {
            PyErr_SetString(PyExc_TypeError, "new takes not key words");
        }
        if (PyTuple_Size(args) != 1) {
            PyErr_SetString(PyExc_TypeError, "new takes only one arg (and int, a tuple, or list of tuples)");
        }
        auto arg = PyList_GetItem(args, 0);
        if (PyLong_Check(arg)) {
            const size_t size = PyLong_AsLong(arg);
            T *values = Constructor<T>::allocate_array(size);
            return (PyObject *) PythonClassWrapper<T *>::createPy(size, values, ContainmentKind::ALLOCATED);
        } else if (PyTuple_Check(arg)) {
            auto list = PyList_New(1);
            PyList_SetItem(list, 0, arg);
            return alloc(cls, list, kwds);
        } else if (PyList_Check(arg)) {
            const ssize_t size = PyList_Size(arg);
            std::function<void(void*, size_t)> constructor = [args, kwds](void * location, size_t index){
                ObjectContainer<T> *cobj = nullptr;
                PyObject* args2 = PyTuple_GetItem(args, index);
                if (!args2 || !PyTuple_Check(args2)){
                    throw "Invalid constructor arguments: not a tuple as expected, or index out of range";
                }
                for (auto const &[kwlist, constructor] : PythonClassWrapper<T>::_constructors) {
                    try {
                        cobj = constructor(kwlist, args, nullptr, ContainmentKind::CONSTRUCTED_IN_PLACE,
                                           (unsigned char*)location);
                        if (cobj) break;
                    } catch (...) {
                    }
                    PyErr_Clear();
                }
                if (!cobj) {
                    throw  "No matching constructor";
                }
            };
            return (PyObject *) PythonClassWrapper<T_NoRef *>::createPyUsingBytePool(size, constructor);
        }
        throw "Invalid constructor arguments";
    }

    template<typename T>
    int PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
        if (_Type.tp_base && Base::TypePtr->tp_init) {
            PyObject *empty = PyTuple_New(0);
            Base::TypePtr->tp_init((PyObject *) &self->baseClass, empty, nullptr);
            PyErr_Clear();
            Py_DECREF(empty);
        }
        self->_referenced = nullptr;
        PyTypeObject *const coreTypePtr = PythonClassWrapper<typename core_type<T>::type>::getPyType();
        self->populate_type_info<T>(&checkType, coreTypePtr);
        if (!_member_getters.count("this")) {
            _member_getters["this"] = getThis;
        }
        return InitHelper<T>::init(self, args, kwds);
    }

    template<typename T>
    PyObject *PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        (void) args;
        (void) kwds;
        PythonClassWrapper *self;
        self = (PythonClassWrapper *) type->tp_alloc(type, 0);
        if (nullptr != self) {
            self->_CObject = nullptr;
        }
        return (PyObject *) self;
    }

    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _free(void *self_) {
        PythonClassWrapper *self = (PythonClassWrapper *) self_;
        if (!self->_CObject) return;
        delete self->_CObject;
        self->_CObject = nullptr;
    }

    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
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
    bool PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _parsePyArgs(const char *const kwlist[], PyObject *args, PyObject *kwds, PyO *&...pyargs) {
        char format[sizeof...(PyO) + 1] = {0};
        if (sizeof...(PyO) > 0)
            memset(format, 'O', sizeof...(PyO));
        return sizeof...(PyO) == 0 || PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...);
    }

    template<typename T>
    template<typename ...Args>
    ObjectContainer<T> *
    PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _createBaseBase(Args ... args) {
        return new ObjectContainerConstructed<T, Args...>(
                std::forward<typename extent_as_pointer<Args>::type>(args)...);
    }


    template<typename T>
    template<typename ...Args, int ...S>
    ObjectContainer<T> *
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    _createBase(PyObject *args, PyObject *kwds,
                const char *const kwlist[], container<S...>, _____fake<Args> *...) {
        if (args && PyTuple_Size(args) != sizeof...(Args)) {
            return nullptr;
        }
        PyObject *pyobjs[sizeof...(Args) + 1];
        (void) pyobjs;
        if (!_parsePyArgs(kwlist, args, kwds, pyobjs[S]...)) {
            PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
            return nullptr;
        }

        return _createBaseBase<Args...>
                (std::forward<typename extent_as_pointer<Args>::type>(
                        *toCArgument<Args, defaults_to_string<Args>::value>(*pyobjs[S]))...);
    }


    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _addMethod(PyMethodDef method) {
        //insert at beginning to keep null sentinel at end of list:
        _methodCollection[method.ml_name] = method;
    }

    template<typename T>
    template<const char *const name, typename FieldType, const size_t bits>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addBitField(
            typename BitFieldContainer<typename std::remove_reference<T>::type>::template Container<name, FieldType, bits>::getter_t &getter,
            typename BitFieldContainer<typename std::remove_reference<T>::type>::template Container<name, FieldType, bits>::setter_t &setter) {
        static const char *const doc = "Get bit-field attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::_getter = getter;
        BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::_setter = setter;
        _member_getters[name] = BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::get;
        _member_setters[name] = BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::set;

    }

    template<typename T>
    template<const char *const name, typename FieldType, const size_t bits>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addBitFieldConst(
            typename BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::getter_t &getter) {
        static const char *const doc = "Get bit-field attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        BitFieldContainer<T_NoRef>::template ConstContainer<name, FieldType, bits>::_getter = getter;
        _member_getters[name] = BitFieldContainer<T_NoRef>::template ConstContainer<name, FieldType, bits>::get;
    }


    template<typename T>
    template<const char *const name, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addAttribute(typename MemberContainer<T_NoRef>::template Container<name, FieldType>::member_t member) {

        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        const ssize_t array_size = ArraySize<FieldType>::size;
        MemberContainer<T_NoRef>::template Container<name, FieldType>::member = member;
        MemberContainer<T_NoRef>::template Container<name, FieldType>::array_size = array_size;
        _member_getters[name] = MemberContainer<T_NoRef>::template Container<name, FieldType>::get;
        _member_setters[name] = MemberContainer<T_NoRef>::template Container<name, FieldType>::set;
    }

    template<typename T>
    template<const char *const name, ssize_t size, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addConstAttribute(
            typename MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::member_t member,
            const ssize_t array_size) {
        assert(array_size == size);
        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::member = member;
        _member_getters[name] = MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::get;
    }

    template<typename T>
    template<const char *const name, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addAttributeConst(
            typename ConstMemberContainer<T_NoRef>::template Container<name, FieldType>::member_t member) {

        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        ConstMemberContainer<T_NoRef>::template Container<name, FieldType>::member = member;
        _member_getters[name] = ConstMemberContainer<T_NoRef>::template Container<name, FieldType>::get;
    }

    template<typename T>
    template<const char *const name, ssize_t size, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addArrayAttribute(
            typename MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::member_t member,
            const ssize_t array_size) {
        assert(array_size == size);
        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::member = member;
        _member_getters[name] = MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::get;
        _member_setters[name] = MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::set;
    }

    template<typename T>
    template<const char *const name, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addClassAttribute(FieldType *member) {

        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        ClassMember<T_NoRef>::template Container<name, FieldType>::member = member;
        PyMethodDef pyMeth = {name,
                              (PyCFunction) ClassMember<T_NoRef>::template Container<name, FieldType>::call,
                              METH_VARARGS | METH_KEYWORDS | METH_CLASS,
                              doc_string
        };
        _addMethod(pyMeth);
    }


    template<typename T>
    template<const char *const name, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addClassAttributeConst(FieldType const *member) {

        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        ConstClassMember<T_NoRef>::template Container<name, FieldType>::member = member;
        PyMethodDef pyMeth = {name,
                              (PyCFunction) ConstClassMember<T_NoRef>::template Container<name, FieldType>::call,
                              METH_VARARGS | METH_KEYWORDS | METH_CLASS,
                              doc_string
        };
        _addMethod(pyMeth);
    }
}

#endif
