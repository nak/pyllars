//
// Created by jrusnak on 5/8/19.
//

#include <pyllars/internal/pyllars_integer.hpp>
#include <pyllars/internal/pyllars_classwrapper.impl.hpp>
#include <pyllars/internal/pyllars_pointer.impl.hpp>
#include <pyllars/internal/pyllars_reference.impl.hpp>

#include "pyllars_class.hpp"
#include "pyllars_namespacewrapper.hpp"

namespace __pyllars_internal {

    PyTypeObject PyNumberCustomBase::_Type = {
#if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(nullptr)
    0,                         /*ob_size*/
#endif
            "PyllarsNumberBase", /*tp_name*/
            sizeof(PyNumberCustomBase), /*tp_basicsize*/
            0, /*tp_itemsize*/
            nullptr, /*tp_dealloc*/
            nullptr, /*tp_print*/
            nullptr, /*tp_getattr*/
            nullptr, /*tp_setattr*/
            nullptr, /*tp_as_sync*/
            nullptr, /*tp_repr*/

            nullptr, /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            nullptr,                         /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES, /*tp_flags*/
            "Base umber type in pyllars",           /* tp_doc */
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
            nullptr,  /* tp_init */
            nullptr,                         /* tp_alloc */
            PyType_GenericNew,             /* tp_new */
            nullptr,                         /*tp_free*/
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bases*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };

    template<typename number_type>
    PyNumberMethods *
    NumberType<number_type>::instance() {
        static PyNumberMethods obj;
        obj.nb_add = _baseBinaryFunc<add>;
        obj.nb_subtract = _baseBinaryFunc<subtract>;
        obj.nb_multiply = _baseBinaryFunc<multiply>;
        obj.nb_remainder = _baseBinaryFunc<remainder>;

        obj.nb_power = (ternaryfunc) power;

        obj.nb_positive = (unaryfunc) positive;
        obj.nb_negative = _baseUnaryFunc<negative>;
        obj.nb_divmod = (binaryfunc) divmod;
        obj.nb_absolute = _baseUnaryFunc<absolute>;
        obj.nb_lshift = _baseBinaryFunc<lshift>;
        obj.nb_rshift = _baseBinaryFunc<rshift>;
        obj.nb_and = _baseBinaryFunc<and_>;
        obj.nb_or = _baseBinaryFunc<or_>;
        obj.nb_xor = _baseBinaryFunc<xor_>;
        obj.nb_int = to_pyint;
        obj.nb_float = to_pyfloat;
        obj.nb_index = to_pyint;
        obj.nb_inplace_add = _baseInplaceBinaryFunc<inplace_add>;
        obj.nb_inplace_subtract = _baseInplaceBinaryFunc<inplace_subtract>;
        obj.nb_inplace_remainder = _baseInplaceBinaryFunc<inplace_remainder>;
        obj.nb_inplace_lshift = _baseInplaceBinaryFunc<inplace_lshift>;
        obj.nb_inplace_rshift = _baseInplaceBinaryFunc<inplace_rshift>;
        obj.nb_inplace_and = _baseInplaceBinaryFunc<inplace_and>;
        obj.nb_inplace_or = _baseInplaceBinaryFunc<inplace_or>;
        obj.nb_inplace_xor = _baseInplaceBinaryFunc<inplace_xor>;

        obj.nb_true_divide = divide;
        obj.nb_floor_divide = _baseBinaryFunc<floor_div>;
        obj.nb_inplace_floor_divide = _baseInplaceBinaryFunc<inplace_floor_div>;
        obj.nb_inplace_true_divide = inplace_divide;
#if PY_MAJOR_VERSION == 2

        obj.nb_nonzero = nonzero;
#endif
        obj.nb_invert = _baseUnaryFunc<invert>;

        return &obj;
    }


    template<typename number_type>
    template<__int128_t(*func)(__int128_t, __int128_t, const bool check)>
    PyObject *
    NumberType<number_type>::_baseBinaryFunc(PyObject *v1, PyObject *v2) {
        if constexpr (std::is_reference<number_type>::value) {
            PyErr_SetString(PyExc_TypeError, "Cannot instantiate reference type for resulting expression");
            return nullptr;
        } else {
            static PyObject *emptyargs = PyTuple_New(0);
            const bool return_py = PyLong_Check(v1) || PyLong_Check(v2);

            if (!isIntegerObject(v1) || !isIntegerObject(v2)) {
                static const char *const msg = "Invalid types for arguments";
                PyErr_SetString(PyExc_TypeError, msg);
                return nullptr;
            }
            const __int128_t value1 = toLongLong(v1);
            const __int128_t value2 = toLongLong(v2);
            __int128_t ret_value;
            ret_value = func(value1, value2, !return_py);
            if (PyErr_Occurred()) {
                return nullptr;
            }
            if (return_py) {
                if (NumberType<number_type>::min() == 0) {
                    return PyLong_FromUnsignedLongLong(ret_value);
                }
                return PyLong_FromLongLong(ret_value);
            } else if (ret_value < min() || ret_value > max) {
                PyErr_SetString(PyExc_ValueError, "Result out of range");
                return nullptr;
            }
            auto *ret = (PyNumberCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) &PyNumberCustomObject<number_type>::_Type, emptyargs, nullptr);
            if (!ret) {
                return nullptr;
            }
            ret->_CObject = new number_type(ret_value);
            return (PyObject *) ret;
        }
    }

    template<typename number_type>
    template<void(*func)(__int128_t &, number_type)>
    PyObject *
    NumberType<number_type>::_baseInplaceBinaryFunc(PyObject *v1, PyObject *v2) {
        if (!PyObject_TypeCheck(v1, &PyNumberCustomObject<number_type>::_Type)) {
            PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
            Py_RETURN_NOTIMPLEMENTED;
        }
        if (!isIntegerObject(v1) || !isIntegerObject(v2)) {
            PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
            Py_RETURN_NOTIMPLEMENTED;
        }
        __int128_t ret_value = *((PyNumberCustomObject<number_type> *) v1)->_CObject;

        func(ret_value, toLongLong(v2));
        if (PyErr_Occurred()) {
            return nullptr;
        }
        if (ret_value < min() || ret_value > max) {
            PyErr_SetString(PyExc_ValueError, "Result out of range");
            return nullptr;
        }
        if constexpr (std::is_const<number_type>::value) {
            auto v1_ = (PyNumberCustomObject<number_type> *) v1;
            *const_cast<typename std::remove_const<number_type>::type *>(v1_->_CObject) = ret_value;
        } else {
            *((PyNumberCustomObject<number_type> *) v1)->_CObject = ret_value;
        }
        Py_INCREF(v1);
        return v1;
    }

    template<typename number_type>
    template<number_type (*func)(__int128_t)>
    PyObject *
    NumberType<number_type>::_baseUnaryFunc(PyObject *obj) {
        static PyObject *emptyargs = PyTuple_New(0);
        if (!isIntegerObject(obj)) {
            PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
            Py_RETURN_NOTIMPLEMENTED;
        }
        __int128_t ret_value = func(toLongLong(obj));
        if (PyErr_Occurred()) {
            return nullptr;
        }

        auto *ret = (PyNumberCustomObject<number_type> *) PyObject_Call(
                (PyObject *) &PyNumberCustomObject<number_type>::_Type, emptyargs, nullptr);
        if (ret) {
            if constexpr(std::is_const<number_type>::value) {
                *const_cast<typename std::remove_const<number_type>::type *>(ret->_CObject) = ret_value;
            } else {
                *ret->_CObject = ret_value;
            }
        }
        return (PyObject *) ret;
    }


    template<typename number_type>
    PyObject *
    NumberType<number_type>::power(PyObject *v1, PyObject *v2, PyObject *v3) {
        if constexpr (std::is_reference<number_type>::value) {
            PyErr_SetString(PyExc_TypeError, "Cannot instantiate reference type for resulting expression");
            return nullptr;
        } else {
            static PyObject *emptyargs = PyTuple_New(0);

            auto *ret = (PyNumberCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) &PyNumberCustomObject<number_type>::_Type, emptyargs, nullptr);
            if (!ret) {
                return nullptr;
            }
            if (v3 == Py_None) {
                v3 = nullptr;
            }
            if (!isIntegerObject(v1) || !isIntegerObject(v2) || (v3 && !isIntegerObject(v3))) {
                static const char *const msg = "Invalid types for arguments";
                PyErr_SetString(PyExc_TypeError, msg);
                return nullptr;
            }
            const __int128_t value1 = toLongLong(v1);
            const __int128_t value2 = toLongLong(v2);
            const __int128_t value3 = v3 ? toLongLong(v3) : 0;
            const __int128_t result = v3 ?
                                      pow(value1, value2) % value3 :
                                      pow(value1, value2);
            if (result < min() || result > max) {
                static const char *const msg = "Result is out of range";
                PyErr_SetString(PyExc_ValueError, msg);
                return nullptr;
            }
            ret->_CObject = new number_type(result);
            return (PyObject *) ret;
        }
    }


    template<typename number_type>
    PyObject *
    NumberType<number_type>::divmod(PyObject *v1, PyObject *v2) {
        if constexpr (std::is_reference<number_type>::value) {
            PyErr_SetString(PyExc_TypeError, "Cannot instantiate reference type for resulting expression");
            return nullptr;
        } else {
            static PyObject *emptyargs = PyTuple_New(0);
            auto *retq = (PyNumberCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) &PyNumberCustomObject<number_type>::_Type, emptyargs, nullptr);
            auto *retr = (PyNumberCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) &PyNumberCustomObject<number_type>::_Type, emptyargs, nullptr);
            if (!retq || !retr) {
                return nullptr;
            }

            if (!isIntegerObject(v1) || !isIntegerObject(v2)) {
                PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
                return nullptr;
            }
            const __int128_t value1 = toLongLong(v1);
            const __int128_t value2 = toLongLong(v2);
            const __int128_t quotient = value1 / value2;
            const __int128_t remainder = value1 % value2;
            if (quotient < min() || quotient > max || remainder < min() || remainder > max) {
                static const char *const msg = "Invalid types for arguments";
                PyErr_SetString(PyExc_ValueError, msg);
                return nullptr;
            }
            PyObject *tuple = PyTuple_New(2);
            retq->_CObject = new number_type(quotient);
            retr->_CObject = new number_type(remainder);
            PyTuple_SetItem(tuple, 0, (PyObject *) retq);
            PyTuple_SetItem(tuple, 1, (PyObject *) retr);
            return tuple;
        }
    }

#if PY_MAJOR_VERSION == 2

    static int nonzero(PyObject* v1){
        if(!isIntegerObject(v1)){
            return 0;
        }
        return toLongLong(v1) != 0;
     }
#endif

    template<typename number_type>
    PyObject *
    NumberType<number_type>::divide(PyObject *v1, PyObject *v2) {
        if (!isIntegerObject(v1) || !isIntegerObject(v2)) {
            static const char *const msg = "Invalid types for arguments";
            PyErr_SetString(PyExc_TypeError, msg);
            return nullptr;
        }
        const __int128_t value1 = toLongLong(v1);
        const __int128_t value2 = toLongLong(v2);
        double result = (double) value1 / (double) value2;
        return (PyObject *) PythonClassWrapper<double>::fromCObject(result);
    }

    template<typename number_type>
    PyObject *
    NumberType<number_type>::inplace_divide(PyObject *v1, PyObject *v2) {
        if (!isIntegerObject(v1) || !isIntegerObject(v2)) {
            static const char *const msg = "Invalid types for arguments";
            PyErr_SetString(PyExc_TypeError, msg);
            return nullptr;
        }
        const __int128_t value1 = toLongLong(v1);
        const __int128_t value2 = toLongLong(v2);
        double result = (double) value1 / (double) value2;
        return (PyObject *) PythonClassWrapper<double>::fromCObject(result);
    }

    template<typename number_type>
    PyMethodDef PyNumberCustomObject<number_type>::_methods[] = {
            {
                    alloc_name_,
                             (PyCFunction) alloc,
                    METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                    "allocate array of numbers"
            },
            {
                    "to_int",
                             (PyCFunction) to_int,
                    METH_KEYWORDS | METH_VARARGS,
                    "convert to Python int type"
            },
            {
                    nullptr, nullptr, 0, nullptr /**sentinel **/
            }
    };


    template<typename number_type>
    PyTypeObject PyNumberCustomObject<number_type>::_Type = {
#if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(nullptr)
    0,                         /*ob_size*/
#endif
            __pyllars_internal::type_name<number_type>(), /*tp_name*/
            sizeof(PyNumberCustomObject<number_type>), /*tp_basicsize*/
            0, /*tp_itemsize*/
            nullptr, /*tp_dealloc*/
            nullptr, /*tp_print*/
            nullptr, /*tp_getattr*/
            nullptr, /*tp_setattr*/
            nullptr, /*tp_as_sync*/
            PyNumberCustomObject::repr, /*tp_repr*/

            NumberType<number_type>::instance(), /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            nullptr,                         /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES, /*tp_flags*/
            "Number type in pyllars",           /* tp_doc */
            nullptr,                       /* tp_traverse */
            nullptr,                       /* tp_clear */
            PyNumberCustomObject::richcompare,                       /* tp_richcompare */
            0,                               /* tp_weaklistoffset */
            nullptr,                       /* tp_iter */
            nullptr,                       /* tp_iternext */
            _methods,             /* tp_methods */
            nullptr,             /* tp_members */
            nullptr,                         /* tp_getset */
            &PyNumberCustomBase::_Type,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            (initproc) PyNumberCustomObject::_init,  /* tp_init */
            nullptr,                         /* tp_alloc */
            nullptr,             /* tp_new */
            PyNumberCustomObject::free,                         /*tp_free*/
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bases*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };


    template<typename number_type>
    PyObject *PyNumberCustomObject<number_type>::repr(PyObject *o) {
        auto *obj = (PyNumberCustomObject<number_type> *) o;
        std::string name = std::string("<pyllars.") + std::string(__pyllars_internal::type_name<number_type>()) +
                           std::string("> _CObject=") + std::to_string(*obj->_CObject);
        return PyString_FromString(name.c_str());
    }


    template<typename number_type>
    PyObject *
    PyNumberCustomObject<number_type>::alloc(PyObject *, PyObject *args, PyObject *kwds) {
        if (kwds && PyDict_Size(kwds) > 0) {
            static const char *const msg = "Allocator does not accept keywords";
            PyErr_SetString(PyExc_TypeError, msg);
            return nullptr;
        }
        const ssize_t size = PyTuple_Size(args);
        if (size > 2) {
            static const char *const msg = "Too many arguments to call to allocations";
            PyErr_SetString(PyExc_TypeError, msg);
            return nullptr;
        }
        if constexpr (std::is_reference<number_type>::value) {
            PyErr_SetString(PyExc_TypeError, "Cannot allocate a reference type");
            return nullptr;
        } else {
            typename std::remove_const<number_type>::type value = 0;
            if (size >= 1) {
                PyObject *item = PyTuple_GetItem(args, 0);
                if (!item) {
                    static const char *const msg = "Internal error getting tuple _CObject";
                    PyErr_SetString(PyExc_SystemError, msg);
                    return nullptr;
                }
                if (!NumberType<number_type>::isIntegerObject(item)) {
                    PyErr_SetString(PyExc_ValueError, "Argument must be of integral type");
                    return nullptr;
                }
                const __int128_t long_value = NumberType<number_type>::toLongLong(item);
                if (long_value < NumberType<number_type>::min() || long_value > NumberType<number_type>::max) {
                    PyErr_SetString(PyExc_ValueError, "Argument out of range");
                    return nullptr;
                }
                value = (number_type) long_value;
            }
            size_t count = 1;
            if (size == 2) {
                PyObject *item = PyTuple_GetItem(args, 1);
                if (!item) {
                    PyErr_SetString(PyExc_SystemError, "Internal error getting tuple _CObject");
                    return nullptr;
                }
                if (!NumberType<number_type>::isIntegerObject(item)) {
                    PyErr_SetString(PyExc_ValueError, "Argument must be of integral type");
                    return nullptr;
                }
                const __int128_t long_value = NumberType<number_type>::toLongLong(item);
                if (long_value < NumberType<number_type>::min() || long_value > NumberType<number_type>::max) {
                    PyErr_SetString(PyExc_ValueError, "Argument out of range");
                    return nullptr;
                }
                count = (number_type) long_value;
                if (count <= 0) {
                    PyErr_SetString(PyExc_ValueError, "Number of elements to allocate must be greater then 0");
                    return nullptr;
                }
            }
            if (count == 1) {
                return (PyObject *) PythonClassWrapper<number_type *>::template allocateInstance<number_type>(value);
            } else {
                return (PyObject *) PythonClassWrapper<number_type *>::template allocateArray<number_type>(value,
                                                                                                           count);
            }
        }
    }

    template<typename number_type>
    status_t PyNumberCustomObject<number_type>::_initialize(PyTypeObject &type) {
        return 0;
    }



    template<typename number_type>
    PyObject *
    PyNumberCustomObject<number_type>::to_int(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!PyTuple_Check(args) || PyTuple_Size(args) + kwds ? PyDict_Size(kwds) : 0 > 0) {
            PyErr_SetString(PyExc_TypeError, "to_float takes no arguments");
            return nullptr;
        }
        if (!PyObject_TypeCheck(self, getPyType())) {
            PyErr_SetString(PyExc_TypeError, "invalid type for self");
            return nullptr;
        }
        auto self_ = reinterpret_cast<PyNumberCustomObject *>(self);
        return PyLong_FromLongLong(*self_->get_CObject());
    }

    template<typename number_type>
    PyObject *PyNumberCustomObject<number_type>::richcompare(PyObject *a, PyObject *b, int op) {
        if (!NumberType<number_type>::isIntegerObject(a) || !NumberType<number_type>::isIntegerObject(b)) {
            PyErr_SetString(PyExc_TypeError, "Invalid operands for comparison");
            return NULL;
        }
        __int128_t value1 = NumberType<number_type>::toLongLong(a);
        __int128_t value2 = NumberType<number_type>::toLongLong(b);
        switch (op) {
            case Py_LT:
                if (value1 < value2) {
                    Py_RETURN_TRUE;
                } else {
                    Py_RETURN_FALSE;
                }
            case Py_LE:
                if (value1 <= value2) {
                    Py_RETURN_TRUE;
                } else {
                    Py_RETURN_FALSE;
                }
            case Py_EQ:
                if (value1 == value2) {
                    Py_RETURN_TRUE;
                } else {
                    Py_RETURN_FALSE;
                }
            case Py_NE:
                if (value1 != value2) {
                    Py_RETURN_TRUE;
                } else {
                    Py_RETURN_FALSE;
                }
            case Py_GT:
                if (value1 > value2) {
                    Py_RETURN_TRUE;
                } else {
                    Py_RETURN_FALSE;
                }
            case Py_GE:
                if (value1 >= value2) {
                    Py_RETURN_TRUE;
                } else {
                    Py_RETURN_FALSE;
                }
            default:
                Py_RETURN_NOTIMPLEMENTED;
        }
    }

    template<typename number_type>
    PyObject *
    PyNumberCustomObject<number_type>::fromCObject(number_type &cobj, PyObject *) {
        static PyObject *kwds = PyDict_New();
        PyObject *args = PyTuple_New(0);
        PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);

        auto *pyobj = (__pyllars_internal::PythonClassWrapper<number_type> *) PyObject_Call(
                (PyObject *) getPyType(), args, kwds);
        if (pyobj) {
            pyobj->_CObject = new number_type(cobj);
        }
        Py_DECREF(args);
        return (PyObject *) pyobj;
    }


    template<typename number_type>
    typename std::remove_const<number_type>::type &
    __pyllars_internal::PyNumberCustomObject<number_type>::toCArgument() {
        if constexpr (std::is_const<number_type>::value) {
            throw PyllarsException(PyExc_TypeError, "Cannot pass const value as non-const reference argument");
        } else {
            return *get_CObject();
        }
    }

    template<typename number_type>
    const number_type &
    __pyllars_internal::PyNumberCustomObject<number_type>::toCArgument() const {
        return *const_cast<const number_type *>(get_CObject());
    }


    template<typename number_type>
    int PyNumberCustomObject<number_type>::_init(PyNumberCustomObject *self, PyObject *args, PyObject *) {
        self->compare = [](CommonBaseWrapper *self_, CommonBaseWrapper *other) -> bool {
            return PyObject_TypeCheck(other, getPyType()) &&
                   (*reinterpret_cast<PyNumberCustomObject *>(self_)->get_CObject() ==
                    *reinterpret_cast<PyNumberCustomObject *>(other)->get_CObject());
        };
        self->hash = [](CommonBaseWrapper *self) -> size_t {
            static std::hash<typename std::remove_cv<number_type>::type> hasher;
            return hasher(*((PyNumberCustomObject *) self)->get_CObject());
        };

        auto toInt = [](PyObject *obj) -> __int128_t {
            return *((PyNumberCustomObject *) obj)->_CObject;
        };
        self->toInt = toInt;
        if (args == NULL_ARGS()) {
            self->_CObject = nullptr;
            return 0;
        } else if (self) {


            if (PyTuple_Size(args) == 0) {
                if constexpr (std::is_reference<number_type>::value) {
                    PyErr_SetString(PyExc_TypeError, "Cannot instantiate a reference type without an acutal object");
                    return -1;
                } else {
                    self->_CObject = new number_type(0);
                }
            } else if (PyTuple_Size(args) == 1) {
                PyObject *value = PyTuple_GetItem(args, 0);
                if constexpr (std::is_reference<number_type>::value) {
                    if (PyObject_TypeCheck(value, PythonClassWrapper<number_type *>::getPyType())) {
                        self->_CObject = *reinterpret_cast<PythonClassWrapper<number_type *> *>(value)->get_CObject();
                    } else if (PyObject_TypeCheck(value, PyNumberCustomObject::getPyType())) {
                        self->_CObject = reinterpret_cast<PyNumberCustomObject *>(value)->_CObject;
                    } else {
                        PyErr_SetString(PyExc_TypeError, "instantiating reference-to-object from incompatible type");
                        return -1;
                    }
                } else {
                    if (!NumberType<number_type>::isIntegerObject(value)) {
                        PyErr_SetString(PyExc_TypeError, "Argument must be an integer");
                        return -1;
                    }
                    __int128_t longvalue = NumberType<number_type>::toLongLong(value);
                    if (longvalue < (__int128_t) std::numeric_limits<number_type>::min() ||
                        longvalue > (__int128_t) std::numeric_limits<number_type>::max()) {
                        PyErr_SetString(PyExc_ValueError, "Argument _CObject out of range");
                        return -1;
                    }
                    self->_CObject = new number_type(longvalue);
                }
            } else {
                PyErr_SetString(PyExc_TypeError, "Should only call with at most one arument");
                return -1;
            }
            self->asLongLong = [self]() -> __int128_t { return (__int128_t) *self->_CObject; };
            return 0;
        }
        PyErr_SetString(PyExc_TypeError, "Recevied null self !?#");
        return -1;
    }


    template
    class PyNumberCustomObject<const bool>;

    template
    class PyNumberCustomObject<const char>;

    template
    class PyNumberCustomObject<const short>;

    template
    class PyNumberCustomObject<const int>;

    template
    class PyNumberCustomObject<const long>;

    template
    class PyNumberCustomObject<const long long>;

    template
    class PyNumberCustomObject<const unsigned char>;

    template
    class PyNumberCustomObject<const signed char>;

    template
    class PyNumberCustomObject<const unsigned short>;

    template
    class PyNumberCustomObject<const unsigned int>;

    template
    class PyNumberCustomObject<const unsigned long>;

    template
    class PyNumberCustomObject<const unsigned long long>;


    template
    class PyNumberCustomObject<bool>;

    template
    class PyNumberCustomObject<char>;

    template
    class PyNumberCustomObject<signed char>;

    template
    class PyNumberCustomObject<short>;

    template
    class PyNumberCustomObject<int>;

    template
    class PyNumberCustomObject<long>;

    template
    class PyNumberCustomObject<long long>;

    template
    class PyNumberCustomObject<unsigned char>;

    template
    class PyNumberCustomObject<unsigned short>;

    template
    class PyNumberCustomObject<unsigned int>;

    template
    class PyNumberCustomObject<unsigned long>;

    template
    class PyNumberCustomObject<unsigned long long>;


    template
    class PyNumberCustomObject<volatile const bool>;

    template
    class PyNumberCustomObject<volatile const char>;

    template
    class PyNumberCustomObject<volatile const signed char>;

    template
    class PyNumberCustomObject<volatile const short>;

    template
    class PyNumberCustomObject<volatile const int>;

    template
    class PyNumberCustomObject<volatile const long>;

    template
    class PyNumberCustomObject<volatile const long long>;

    template
    class PyNumberCustomObject<volatile const unsigned char>;

    template
    class PyNumberCustomObject<volatile const unsigned short>;

    template
    class PyNumberCustomObject<volatile const unsigned int>;

    template
    class PyNumberCustomObject<volatile const unsigned long>;

    template
    class PyNumberCustomObject<volatile const unsigned long long>;


    template
    class PyNumberCustomObject<volatile bool>;

    template
    class PyNumberCustomObject<volatile char>;

    template
    class PyNumberCustomObject<volatile signed char>;

    template
    class PyNumberCustomObject<volatile short>;

    template
    class PyNumberCustomObject<volatile int>;

    template
    class PyNumberCustomObject<volatile long>;

    template
    class PyNumberCustomObject<volatile long long>;

    template
    class PyNumberCustomObject<volatile unsigned char>;

    template
    class PyNumberCustomObject<volatile unsigned short>;

    template
    class PyNumberCustomObject<volatile unsigned int>;

    template
    class PyNumberCustomObject<volatile unsigned long>;

    template
    class PyNumberCustomObject<volatile unsigned long long>;
}

namespace pyllars{


    template
    class PyllarsClass<bool, GlobalNS>;

    template
    class PyllarsClass<const bool, GlobalNS>;


    template
    class PyllarsClass<volatile bool, GlobalNS>;


    template
    class PyllarsClass<const volatile bool, GlobalNS>;


    template
    class PyllarsClass<char, GlobalNS>;


    template
    class PyllarsClass<unsigned char, GlobalNS >;


    template
    class PyllarsClass<signed char, GlobalNS >;


    template
    class PyllarsClass<short, GlobalNS>;


    template
    class PyllarsClass<unsigned short, GlobalNS>;


    template
    class PyllarsClass<int, GlobalNS >;


    template
    class PyllarsClass<unsigned int, GlobalNS >;


    template
    class PyllarsClass<long, GlobalNS >;


    template
    class PyllarsClass<unsigned long, GlobalNS >;


    template
    class PyllarsClass<long long, GlobalNS >;


    template
    class PyllarsClass<unsigned long long, GlobalNS >;

    ///////////////////

    template
    class PyllarsClass<const char, GlobalNS >;


    template
    class PyllarsClass<const unsigned char, GlobalNS >;


    template
    class PyllarsClass<const signed char, GlobalNS >;


    template
    class PyllarsClass<const short, GlobalNS >;


    template
    class PyllarsClass<const unsigned short, GlobalNS >;


    template
    class PyllarsClass<const int, GlobalNS >;


    template
    class PyllarsClass<const unsigned int, GlobalNS >;


    template
    class PyllarsClass<const long, GlobalNS >;


    template
    class PyllarsClass<const unsigned long, GlobalNS >;


    template
    class PyllarsClass<const long long, GlobalNS >;


    template
    class PyllarsClass<const unsigned long long, GlobalNS >;

   ////////////////////

    template
    class PyllarsClass<volatile char, GlobalNS >;


    template
    class PyllarsClass<volatile unsigned char, GlobalNS >;


    template
    class PyllarsClass<volatile signed char, GlobalNS >;


    template
    class PyllarsClass<volatile short, GlobalNS >;


    template
    class PyllarsClass<volatile unsigned short, GlobalNS >;


    template
    class PyllarsClass<volatile int, GlobalNS >;


    template
    class PyllarsClass<volatile unsigned int, GlobalNS >;


    template
    class PyllarsClass<volatile long, GlobalNS >;


    template
    class PyllarsClass<volatile unsigned long, GlobalNS >;


    template
    class PyllarsClass<volatile long long, GlobalNS >;


    template
    class PyllarsClass<volatile unsigned long long, GlobalNS >;

    //////////

    template
    class PyllarsClass<const volatile char, GlobalNS >;


    template
    class PyllarsClass<const volatile unsigned char, GlobalNS >;


    template
    class PyllarsClass<const volatile signed char, GlobalNS >;


    template
    class PyllarsClass<const volatile short, GlobalNS >;


    template
    class PyllarsClass<const volatile unsigned short, GlobalNS >;


    template
    class PyllarsClass<const volatile int, GlobalNS >;


    template
    class PyllarsClass<const volatile unsigned int, GlobalNS >;


    template
    class PyllarsClass<const volatile long, GlobalNS >;


    template
    class PyllarsClass<const volatile unsigned long, GlobalNS >;


    template
    class PyllarsClass<const volatile long long, GlobalNS >;


    template
    class PyllarsClass<const volatile unsigned long long, GlobalNS >;

}