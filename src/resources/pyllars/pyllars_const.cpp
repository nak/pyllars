
#include <cstdio>
#include <limits>
#include <cmath>

#include "pyllars_const.hpp"
#include "pyllars_pointer.impl.hpp"
#include "pyllars_classwrapper.impl.hpp"

namespace __pyllars_internal {


    PyTypeObject PyConstNumberCustomBase::Type = {
#if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(nullptr)
    0,                         /*ob_size*/
#endif
            "PyllarsConstNumberBase", /*tp_name*/
            sizeof(PyConstNumberCustomBase), /*tp_basicsize*/
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
            "Base const number type in pyllars",           /* tp_doc */
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
    typename PyConstNumberCustomObject<number_type>::Initializer *PyConstNumberCustomObject<number_type>::
            Initializer::initializer =
            new PyConstNumberCustomObject<number_type>::Initializer();


    template<typename number_type>
    struct ConstNumberType {

        static PyNumberMethods *instance() {
            static PyNumberMethods obj;
            memset(&obj, 0, sizeof(obj));
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

            obj.nb_true_divide = divide;
            obj.nb_floor_divide = _baseBinaryFunc<floor_div>;
#if PY_MAJOR_VERSION == 2

            obj.nb_nonzero = nonzero;
#endif
            obj.nb_invert = _baseUnaryFunc<invert>;

            return &obj;
        }

        static __int128_t toLongLong(PyObject *obj) {
            if (PyLong_Check(obj)) {
                if (min == 0) {
                    return PyLong_AsUnsignedLongLong(obj);
                }
                return PyLong_AsLongLong(obj);
            } else if (PyObject_TypeCheck(obj, &PyConstNumberCustomBase::Type)) {
                return ((PyConstNumberCustomObject<number_type> *) obj)->asLongLong();
            } else {
                PyErr_SetString(PyExc_SystemError, "System error: invalid type encountered");
                return 0;
            }
        }

        static bool isIntegerObject(PyObject *obj) {
            return bool(PyLong_Check(obj)) || bool(PyObject_TypeCheck(obj, &PyConstNumberCustomBase::Type));
        }

        static constexpr number_type min = std::numeric_limits<number_type>::min();
        static constexpr number_type max = std::numeric_limits<number_type>::max();

        static bool is_out_of_bounds_add(__int128_t value1, __int128_t value2) {
            return ((value1 > 0 && value1 > max - value2) ||
                    (value1 < 0 && value1 < min - value2));
        }

        static bool is_out_of_bounds_subtract(__int128_t value1, __int128_t value2) {
            return ((value1 > 0 && value1 > max + value2) ||
                    (value1 < 0 && value1 < min + value2));
        }

        template<__int128_t(*func)(__int128_t, __int128_t, const bool check)>
        static PyObject *_baseBinaryFunc(PyObject *v1, PyObject *v2) {
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
                if (ConstNumberType<number_type>::min == 0) {
                    return PyLong_FromUnsignedLongLong(ret_value);
                }
                return PyLong_FromLongLong(ret_value);
            } else if (ret_value < min || ret_value > max) {
                PyErr_SetString(PyExc_ValueError, "Result out of range");
                return nullptr;
            }
            auto *ret = (PyConstNumberCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) PyConstNumberCustomObject<number_type>::getPyType(), emptyargs, nullptr);
            if (!ret) {
                return nullptr;
            }
            *const_cast<typename std::remove_const<number_type>::type *>(&ret->value) = ret_value;
            return (PyObject *) ret;
        }


        template<number_type (*func)(__int128_t)>
        static PyObject *_baseUnaryFunc(PyObject *obj) {
            static PyObject *emptyargs = PyTuple_New(0);
            if (!isIntegerObject(obj)) {
                PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
                Py_RETURN_NOTIMPLEMENTED;
            }
            __int128_t ret_value = func(toLongLong(obj));
            if (PyErr_Occurred()) {
                return nullptr;
            }

            auto *ret = (PyConstNumberCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) PyConstNumberCustomObject<number_type>::getPyType(), emptyargs, nullptr);
            if (ret) {
                *const_cast<typename std::remove_const<number_type>::type *>(&ret->value) = ret_value;
            }
            return reinterpret_cast<PyObject*>(ret);
        }

        static __int128_t add(__int128_t value1, __int128_t value2, const bool check) {

            return value1 + value2;
        }

        static __int128_t subtract(__int128_t value1, __int128_t value2, const bool check) {

            return value1 - value2;
        }

        static __int128_t multiply(__int128_t value1, __int128_t value2, const bool check) {
            const number_type result = value1 * value2;
            if (check && value1 != 0 && result / value1 != value2) {
                PyErr_SetString(PyExc_ValueError, "multiplication of values is out of range");
            }
            return result;
        }

        static __int128_t pow(__int128_t base, __int128_t exp) {
            __int128_t result = 1;
            for (;;) {
                if (exp & 1)
                    result *= base;
                exp >>= 1;
                if (!exp)
                    break;
                base *= base;
            }
            return result;
        }

        static PyObject *power(PyObject *v1, PyObject *v2, PyObject *v3) {
            static PyObject *emptyargs = PyTuple_New(0);
            auto *ret = (PyConstNumberCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) PyConstNumberCustomObject<number_type>::getPyType(), emptyargs, nullptr);
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
            if (result < min || result > max) {
                static const char *const msg = "Result is out of range";
                PyErr_SetString(PyExc_ValueError, msg);
                return nullptr;
            }
            *const_cast<typename std::remove_const<number_type>::type *>(&ret->value) = (number_type) result;
            return (PyObject *) ret;
        }

        static __int128_t remainder(__int128_t value1, __int128_t value2, const bool check) {
            __int128_t result = value1 % value2;
            if (((value1 < 0 and value2 > 0) || (value1 > 0 && value2 < 0)) && (value1 % value2 != 0)) {
                result = value1 - (floor_div(value1, value2, false) * value2);
            }
            if (check && (result > max || result < min)) {
                PyErr_SetString(PyExc_ValueError, "Result is out of range");
            }
            return result;
        }

        static PyObject *positive(PyObject *v1) {
            Py_INCREF(v1);
            return v1;
        }

        static number_type absolute(__int128_t value1) {
            if (value1 == min) {
                PyErr_SetString(PyExc_ValueError, "Result is out of bounds");
            }
            return value1 > 0 ? value1 : -value1;
        }

        static number_type negative(__int128_t value) {
            const __int128_t result = -value;
            if (result < min || result > max) {
                PyErr_SetString(PyExc_ValueError, "result is out of range");
            }
            return result;
        }

        static PyObject *divmod(PyObject *v1, PyObject *v2) {
            static PyObject *emptyargs = PyTuple_New(0);
            auto *retq = (PyConstNumberCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) PyConstNumberCustomObject<number_type>::getPyType(), emptyargs, nullptr);
            auto *retr = (PyConstNumberCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) PyConstNumberCustomObject<number_type>::getPyType(), emptyargs, nullptr);
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
            if (quotient < min || quotient > max || remainder < min || remainder > max) {
                static const char *const msg = "Invalid types for arguments";
                PyErr_SetString(PyExc_ValueError, msg);
                return nullptr;
            }
            PyObject *tuple = PyTuple_New(2);
            *const_cast<typename std::remove_const<number_type>::type*>(&retq->value) = (number_type) quotient;
            *const_cast<typename std::remove_const<number_type>::type*>(&retr->value) = (number_type) remainder;
            PyTuple_SetItem(tuple, 0, reinterpret_cast<PyObject*>(retq));
            PyTuple_SetItem(tuple, 1, reinterpret_cast<PyObject*>(retr));
            return tuple;
        }

#if PY_MAJOR_VERSION == 2

        static int nonzero(PyObject* v1){
            if(!isIntegerObject(v1)){
                return 0;
            }
            return toLongLong(v1) != 0;
         }
#endif

        static number_type invert(__int128_t value) {
            return ~(number_type) value;
        }

        static __int128_t lshift(__int128_t value1, __int128_t value2, const bool) {
            return ((number_type) value1) << ((number_type) value2);
        }

        static __int128_t rshift(__int128_t value1, __int128_t value2, const bool) {
            return ((number_type) value1) >> ((number_type) value2);
        }

        static __int128_t and_(__int128_t value1, __int128_t value2, const bool) {
            return (number_type) value1 & (number_type) value2;
        }

        static __int128_t or_(__int128_t value1, __int128_t value2, const bool) {
            return ((number_type) value1) | ((number_type) value2);
        }

        static __int128_t xor_(__int128_t value1, __int128_t value2, const bool) {
            return ((number_type) value1) ^ ((number_type) value2);
        }

        static PyObject *to_pyint(PyObject *value) {
            return PyLong_FromLongLong(toLongLong(value));
        }

        static PyObject *to_pyfloat(PyObject *value) {
            return PyFloat_FromDouble((double) toLongLong(value));
        }


        static __int128_t floor_div(__int128_t value1, __int128_t value2, const bool check) {
            if (((value1 < 0 and value2 > 0) || (value1 > 0 && value2 < 0)) && (value1 % value2 != 0)) {
                return value1 / value2 - 1;
            }
            return value1 / value2;
        }

        static PyObject *divide(PyObject *v1, PyObject *v2) {
            if (!isIntegerObject(v1) || !isIntegerObject(v2)) {
                static const char *const msg = "Invalid types for arguments";
                PyErr_SetString(PyExc_TypeError, msg);
                return nullptr;
            }
            const __int128_t value1 = toLongLong(v1);
            const __int128_t value2 = toLongLong(v2);
            return PyFloat_FromDouble((double) value1 / (double) value2);
        }

    };

    template<typename number_type>
    PyMethodDef PyConstNumberCustomObject<number_type>::_methods[] = {
            {
                alloc_name_,
                (PyCFunction) alloc,
                METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                "allocate array of numbers"
            },
            {
                nullptr, nullptr, 0, nullptr /**sentinel **/
            }
    };

    template<typename number_type>
    PyTypeObject PyConstNumberCustomObject<number_type>::Type = {
#if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(nullptr)
    0,                         /*ob_size*/
#endif
            __pyllars_internal::type_name<number_type>(), /*tp_name*/
            sizeof(PyConstNumberCustomObject<number_type>), /*tp_basicsize*/
            0, /*tp_itemsize*/
            nullptr, /*tp_dealloc*/
            nullptr, /*tp_print*/
            nullptr, /*tp_getattr*/
            nullptr, /*tp_setattr*/
            nullptr, /*tp_as_sync*/
            PyConstNumberCustomObject::repr, /*tp_repr*/

            ConstNumberType<number_type>::instance(), /*tp_as_number*/
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
            PyConstNumberCustomObject::richcompare,                       /* tp_richcompare */
            0,                               /* tp_weaklistoffset */
            nullptr,                       /* tp_iter */
            nullptr,                       /* tp_iternext */
            _methods,             /* tp_methods */
            nullptr,             /* tp_members */
            nullptr,                         /* tp_getset */
            &PyConstNumberCustomBase::Type,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            PyConstNumberCustomObject::create,  /* tp_init */
            nullptr,                         /* tp_alloc */
            nullptr,             /* tp_new */
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
    PyObject *PyConstNumberCustomObject<number_type>::repr(PyObject *o) {
        auto *obj = (PyConstNumberCustomObject<number_type> *) o;
        std::string name = std::string("<pyllars.") + std::string(__pyllars_internal::type_name<number_type>()) +
                           std::string("> value=") + std::to_string(obj->value);
        return PyString_FromString(name.c_str());
    }


    template<typename number_type>
    PythonClassWrapper<number_type *> *
    PyConstNumberCustomObject<number_type>::alloc(PyObject *cls, PyObject *args, PyObject *kwds) {
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
        typename std::remove_const<number_type>::type value = 0;
        if (size >= 1) {
            PyObject *item = PyTuple_GetItem(args, 0);
            if (!item) {
                static const char *const msg = "Internal error getting tuple value";
                PyErr_SetString(PyExc_SystemError, msg);
                return nullptr;
            }
            if (!ConstNumberType<number_type>::isIntegerObject(item)) {
                PyErr_SetString(PyExc_ValueError, "Argument must be of integral type");
                return nullptr;
            }
            const __int128_t long_value = ConstNumberType<number_type>::toLongLong(item);
            if (long_value < ConstNumberType<number_type>::min || long_value > ConstNumberType<number_type>::max) {
                PyErr_SetString(PyExc_ValueError, "Argument out of range");
                return nullptr;
            }
            value = (number_type) long_value;
        }
        size_t count = 1;
        if (size == 2) {
            PyObject *item = PyTuple_GetItem(args, 1);
            if (!item) {
                PyErr_SetString(PyExc_SystemError, "Internal error getting tuple value");
                return nullptr;
            }
            if (!ConstNumberType<number_type>::isIntegerObject(item)) {
                PyErr_SetString(PyExc_ValueError, "Argument must be of integral type");
                return nullptr;
            }
            const __int128_t long_value = ConstNumberType<number_type>::toLongLong(item);
            if (long_value < ConstNumberType<number_type>::min || long_value > ConstNumberType<number_type>::max) {
                PyErr_SetString(PyExc_ValueError, "Argument out of range");
                return nullptr;
            }
            count = (number_type) long_value;
            if (count <= 0) {
                PyErr_SetString(PyExc_ValueError, "Number of elements to allocate must be greater then 0");
                return nullptr;
            }
        }
        auto *alloced = new number_type(value);
        return PythonClassWrapper<number_type *>::createPy(count, alloced, ContainmentKind ::ALLOCATED);
    }

    template<typename number_type>
    int PyConstNumberCustomObject<number_type>::initialize() {
        int rc = PyType_Ready(&CommonBaseWrapper::_BaseType);
        rc |= PyType_Ready(&PyConstNumberCustomBase::Type);
        rc |= PyType_Ready(&PyConstNumberCustomObject::Type);
        return rc;
    }


    template<typename number_type>
    PyObject *PyConstNumberCustomObject<number_type>::richcompare(PyObject *a, PyObject *b, int op) {
        if (!ConstNumberType<number_type>::isIntegerObject(a) || !ConstNumberType<number_type>::isIntegerObject(b)) {
            PyErr_SetString(PyExc_TypeError, "Invalid operands for comparison");
            return NULL;
        }
        __int128_t value1 = ConstNumberType<number_type>::toLongLong(a);
        __int128_t value2 = ConstNumberType<number_type>::toLongLong(b);
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
    __pyllars_internal::PythonClassWrapper<number_type> *PyConstNumberCustomObject<number_type>::createPy
            (const ssize_t arraySize,
             ntype & cobj, const ContainmentKind  containmentKind,
             PyObject *referencing) {
        static PyObject *kwds = PyDict_New();
        static PyObject *emptyargs = PyTuple_New(0);
        PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);

        auto *pyobj = (__pyllars_internal::PythonClassWrapper<number_type> *) PyObject_Call(
                (PyObject *) getPyType(), emptyargs, kwds);
        pyobj->_depth = 0;
        return pyobj;
    }

    template<typename number_type>
    int PyConstNumberCustomObject<number_type>::create(PyObject *self_, PyObject *args, PyObject *kwds) {
        auto *self = (PyConstNumberCustomObject *) self_;
        if (self) {
            PyTypeObject* const coreTypePtr = PythonClassWrapper<typename core_type<number_type>::type>::getPyType();
            self->template populate_type_info< number_type>(&checkType, coreTypePtr);
            if (PyTuple_Size(args) == 0) {
                memset(const_cast<typename std::remove_const<number_type>::type *>(&self->value), 0,
                       sizeof(self->value));
            } else if (PyTuple_Size(args) == 1) {
                PyObject *value = PyTuple_GetItem(args, 0);
                if (!ConstNumberType<number_type>::isIntegerObject(value)) {
                    PyErr_SetString(PyExc_TypeError, "Argument must be an integer");
                    return -1;
                }
                __int128_t longvalue = ConstNumberType<number_type>::toLongLong(value);
                if (longvalue < (__int128_t) std::numeric_limits<number_type>::min() ||
                    longvalue > (__int128_t) std::numeric_limits<number_type>::max()) {
                    PyErr_SetString(PyExc_ValueError, "Argument value out of range");
                    return -1;
                }
                *(const_cast<typename std::remove_const<number_type>::type *>(&self->value)) = (number_type) longvalue;
            } else {
                PyErr_SetString(PyExc_TypeError, "Should only call with at most one arument");
                return -1;
            }
            self->asLongLong = [self]() -> __int128_t { return (__int128_t) self->value; };
            return 0;
        }
        PyErr_SetString(PyExc_TypeError, "Recevied null self !?#");
        return -1;
    }


    template<typename number_type>
    PyConstNumberCustomObject<number_type>::Initializer::Initializer() {
        pyllars_register(this);
    }

    template<typename number_type>
    status_t PyConstNumberCustomObject<number_type>::Initializer::init(PyObject *const global_mod) {
        static PyObject *module = PyImport_ImportModule("pyllars");
        int rc = PyType_Ready(&CommonBaseWrapper::_BaseType);
        rc |= PyType_Ready(&PyConstNumberCustomBase::Type);
        rc |= PyType_Ready(&PyConstNumberCustomObject::Type);
        Py_INCREF(&PyConstNumberCustomBase::Type);
        Py_INCREF(&PyConstNumberCustomObject::Type);
        if (module && rc == 0) {
            PyModule_AddObject(module, __pyllars_internal::type_name<number_type>(),
                               (PyObject *) &PyConstNumberCustomObject::Type);
        }
        return rc;
    }

    template
    class PyConstNumberCustomObject<const char>;

    template
    class PyConstNumberCustomObject<const short>;

    template
    class PyConstNumberCustomObject<const int>;

    template
    class PyConstNumberCustomObject<const long>;

    template
    class PyConstNumberCustomObject<const long long>;

    template
    class PyConstNumberCustomObject<const unsigned char>;

    template
    class PyConstNumberCustomObject<const unsigned short>;

    template
    class PyConstNumberCustomObject<const unsigned int>;

    template
    class PyConstNumberCustomObject<const unsigned long>;

    template
    class PyConstNumberCustomObject<const unsigned long long>;


//////////////////////////////

    template<typename number_type>
    struct ConstFloatingPointType {

        static PyNumberMethods *instance() {
            static PyNumberMethods obj;
            memset(&obj, 0, sizeof(obj));
            obj.nb_add = _baseBinaryFunc<add>;
            obj.nb_subtract = _baseBinaryFunc<subtract>;
            obj.nb_multiply = _baseBinaryFunc<multiply>;
            obj.nb_remainder = _baseBinaryFunc<remainder>;

            obj.nb_power = (ternaryfunc) power;

            obj.nb_positive = (unaryfunc) positive;
            obj.nb_negative = _baseUnaryFunc<negative>;
            obj.nb_divmod = (binaryfunc) divmod;
            obj.nb_absolute = _baseUnaryFunc<absolute>;
            obj.nb_int = to_pyint;
            obj.nb_float = to_pyfloat;
            obj.nb_index = to_pyint;

            obj.nb_true_divide = _baseBinaryFunc<divide>;
            obj.nb_floor_divide = _baseBinaryFunc<floor_div>;
#if PY_MAJOR_VERSION == 2

            obj.nb_nonzero = nonzero;
#endif
            obj.nb_invert = nullptr;

            return &obj;
        }

        static double toDouble(PyObject *obj) {
            if (PyFloat_Check(obj)) {
                return PyFloat_AsDouble(obj);
            } else if (PyObject_TypeCheck(obj, &PyConstFloatingPtCustomBase::Type)) {
                return ((PyConstFloatingPtCustomObject<number_type> *) obj)->asDouble();
            } else {
                PyErr_SetString(PyExc_SystemError, "System error: invalid type encountered");
                return 0;
            }
        }

        static bool isFloatingPtObject(PyObject *obj) {
            return bool(PyFloat_Check(obj)) || bool(PyObject_TypeCheck(obj, &PyConstFloatingPtCustomBase::Type));
        }

        static constexpr number_type min = std::numeric_limits<number_type>::min();
        static constexpr number_type max = std::numeric_limits<number_type>::max();

        static bool is_out_of_bounds_add(double value1, double value2) {
            return ((value1 > 0.0 && value1 > max - value2) ||
                    (value1 < 0.0 && value1 < min - value2));
        }

        static bool is_out_of_bounds_subtract(double value1, double value2) {
            return ((value1 > 0.0 && value1 > max + value2) ||
                    (value1 < 0.0 && value1 < min + value2));
        }

        template<double(*func)(double, double, const bool check)>
        static PyObject *_baseBinaryFunc(PyObject *v1, PyObject *v2) {
            static PyObject *emptyargs = PyTuple_New(0);
            const bool return_py = PyFloat_Check(v1) || PyFloat_Check(v2);

            if (!isFloatingPtObject(v1) || !isFloatingPtObject(v2)) {
                static const char *const msg = "Invalid types for arguments";
                PyErr_SetString(PyExc_TypeError, msg);
                return nullptr;
            }
            const double value1 = toDouble(v1);
            const double value2 = toDouble(v2);
            double ret_value;
            ret_value = func(value1, value2, !return_py);
            if (PyErr_Occurred()) {
                return nullptr;
            }
            if (return_py) {
                return PyFloat_FromDouble(ret_value);
            } else if (ret_value < min || ret_value > max) {
                PyErr_SetString(PyExc_ValueError, "Result out of range");
                return nullptr;
            }
            auto *ret = (PyConstFloatingPtCustomObject<number_type> *)
                    PyObject_Call((PyObject *) PyConstFloatingPtCustomObject<number_type>::getPyType(), emptyargs,
                                  nullptr);
            if (!ret) {
                return nullptr;
            }
            *const_cast<typename std::remove_const<number_type>::type *>(&ret->value) = ret_value;
            return (PyObject *) ret;
        }

        template<number_type (*func)(double)>
        static PyObject *_baseUnaryFunc(PyObject *obj) {
            static PyObject *emptyargs = PyTuple_New(0);
            if (!isFloatingPtObject(obj)) {
                PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
                Py_RETURN_NOTIMPLEMENTED;
            }
            double ret_value = func(toDouble(obj));
            if (PyErr_Occurred()) {
                return nullptr;
            }

            auto *ret = (PyConstFloatingPtCustomObject<number_type> *)
                    PyObject_Call((PyObject *) PyConstFloatingPtCustomObject<number_type>::getPyType(), emptyargs,
                                  nullptr);
            if (ret) {
                *const_cast<typename std::remove_const<number_type>::type *>(&ret->value) = ret_value;
            }
            return (PyObject *) ret;
        }

        static double add(double value1, double value2, const bool check) {
            if (check && is_out_of_bounds_add(value1, value2)) {
                PyErr_SetString(PyExc_ValueError, "sum of values out of range");
            }
            return value1 + value2;
        }

        static double subtract(double value1, double value2, const bool check) {
            if (check && is_out_of_bounds_subtract(value1, value2)) {
                PyErr_SetString(PyExc_ValueError, "difference of values out of range");
            }
            return value1 - value2;
        }

        static double multiply(double value1, double value2, const bool check) {
            const number_type result = value1 * value2;
            if (check && value1 != 0 && result / value1 != value2) {
                PyErr_SetString(PyExc_ValueError, "multiplication of values is out of range");
            }
            return result;
        }

        static PyObject *power(PyObject *v1, PyObject *v2, PyObject *v3) {
            static PyObject *emptyargs = PyTuple_New(0);
            auto *ret = (PyConstFloatingPtCustomObject<number_type> *)
                    PyObject_Call((PyObject *) PyConstFloatingPtCustomObject<number_type>::getPyType(), emptyargs,
                                  nullptr);
            if (!ret) {
                return nullptr;
            }
            if (v3 && v3 != Py_None) {
                PyErr_SetString(PyExc_TypeError, "3rd argument not allowed for floating point pow()");
                return nullptr;
            }
            if (!isFloatingPtObject(v1) || !isFloatingPtObject(v2)) {
                static const char *const msg = "Invalid types for arguments";
                PyErr_SetString(PyExc_TypeError, msg);
                return nullptr;
            }
            const double value1 = toDouble(v1);
            const double value2 = toDouble(v2);
            const double result = ::pow(value1, value2);
            if (result < min || result > max) {
                static const char *const msg = "Result is out of range";
                PyErr_SetString(PyExc_ValueError, msg);
                return nullptr;
            }
            *&ret->value = (number_type) result;
            return (PyObject *) ret;
        }

        static double remainder(double value1, double value2, const bool check) {
            double result;
            if (((value1 < 0.0 and value2 > 0.0) || (value1 > 0.0 && value2 < 0.0)) && fmod(value1, value2) != 0.0) {
                result = value1 - (floor_div(value1, value2, false) * value2);
            } else {
                result = fmod(value1, value2);
            }
            if (check && (result > max || result < min)) {
                PyErr_SetString(PyExc_ValueError, "Result is out of range");
            }
            return result;
        }

        static PyObject *positive(PyObject *v1) {
            Py_INCREF(v1);
            return v1;
        }

        static number_type absolute(double value1) {
            return fabs(value1);
        }

        static number_type negative(double value) {
            return -value;
        }

        static PyObject *divmod(PyObject *v1, PyObject *v2) {
            static PyObject *emptyargs = PyTuple_New(0);
            auto *retq = (PyConstFloatingPtCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) PyConstFloatingPtCustomObject<number_type>::getPyType(), emptyargs, nullptr);
            auto *retr = (PyConstFloatingPtCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) PyConstFloatingPtCustomObject<number_type>::getPyType(), emptyargs, nullptr);
            if (!retq || !retr) {
                return nullptr;
            }

            if (!isFloatingPtObject(v1) || !isFloatingPtObject(v2)) {
                PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
                return nullptr;
            }
            const double value1 = toDouble(v1);
            const double value2 = toDouble(v2);
            const auto quotient = (double) ((__int128_t) value1 / (__int128_t) value2);
            const double remainder = value1 - ((double) quotient) * value2;
            if (quotient < min || quotient > max || remainder < min || remainder > max) {
                static const char *const msg = "Invalid types for arguments";
                PyErr_SetString(PyExc_ValueError, msg);
                return nullptr;
            }
            PyObject *tuple = PyTuple_New(2);
            *&retq->value = (number_type) quotient;
            *&retr->value = (number_type) remainder;
            PyTuple_SetItem(tuple, 0, (PyObject *) retq);
            PyTuple_SetItem(tuple, 1, (PyObject *) retr);
            return tuple;
        }

#if PY_MAJOR_VERSION == 2

        static int nonzero(PyObject* v1){
            if(!isFloatingPtObject(v1)){
                return 0;
            }
            return toDouble(v1) != 0.0;
         }
#endif


        static PyObject *to_pyint(PyObject *value) {
            return PyLong_FromLong((long long) toDouble(value));
        }

        static PyObject *to_pyfloat(PyObject *value) {
            return PyFloat_FromDouble(toDouble(value));
        }

        static double floor_div(double v1, double v2, const bool check) {
            auto value1 = (__int128_t) v1;
            auto value2 = (__int128_t) v2;
            if (((value1 < 0 and value2 > 0) || (value1 > 0 && value2 < 0)) && (value1 % value2) != 0.0) {
                return (double) (value1 / value2 - 1);
            }
            return (double) (value1 / value2);
        }

        static double divide(double value1, double value2, const bool check) {
            return value1 / value2;
        }

    };


    PyTypeObject PyConstFloatingPtCustomBase::Type = {
#if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(nullptr)
    0,                         /*ob_size*/
#endif
            "PyllarsConstFloatingPtBase", /*tp_name*/
            sizeof(PyConstFloatingPtCustomBase), /*tp_basicsize*/
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
            "Base floating point type in pyllars",           /* tp_doc */
            nullptr,                       /* tp_traverse */
            nullptr,                       /* tp_clear */
            nullptr,                       /* tp_richcompare */
            0,                               /* tp_weaklistoffset */
            nullptr,                       /* tp_iter */
            nullptr,                       /* tp_iternext */
            nullptr,             /* tp_methods */
            nullptr,             /* tp_members */
            nullptr,                         /* tp_getset */
            &CommonBaseWrapper::_BaseType,                         /* tp_base */
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
    PyMethodDef PyConstFloatingPtCustomObject<number_type>::_methods[] = {
            {
                    alloc_name_,
                    (PyCFunction) alloc,
                    METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                    "allocate array of numbers"
            },
            {
                    nullptr, nullptr, 0, nullptr /**sentinel **/
            }
    };

    template<typename number_type>
    PyTypeObject PyConstFloatingPtCustomObject<number_type>::Type = {
#if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(nullptr)
    0,                         /*ob_size*/
#endif
            __pyllars_internal::type_name<number_type>(), /*tp_name*/
            sizeof(PyConstFloatingPtCustomObject<number_type>), /*tp_basicsize*/
            0, /*tp_itemsize*/
            nullptr, /*tp_dealloc*/
            nullptr, /*tp_print*/
            nullptr, /*tp_getattr*/
            nullptr, /*tp_setattr*/
            nullptr, /*tp_as_sync*/
            PyConstFloatingPtCustomObject::repr, /*tp_repr*/

            ConstFloatingPointType<number_type>::instance(), /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            nullptr,                         /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES, /*tp_flags*/
            "C Floating point type in pyllars",           /* tp_doc */
            nullptr,                       /* tp_traverse */
            nullptr,                       /* tp_clear */
            PyConstFloatingPtCustomObject::richcompare,                       /* tp_richcompare */
            0,                               /* tp_weaklistoffset */
            nullptr,                       /* tp_iter */
            nullptr,                       /* tp_iternext */
            _methods,             /* tp_methods */
            nullptr,             /* tp_members */
            nullptr,                         /* tp_getset */
            &PyConstFloatingPtCustomBase::Type,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            PyConstFloatingPtCustomObject::create,  /* tp_init */
            nullptr,                         /* tp_alloc */
            nullptr,             /* tp_new */
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
    PyObject *PyConstFloatingPtCustomObject<number_type>::repr(PyObject *o) {
        auto *obj = (PyConstFloatingPtCustomObject<number_type> *) o;
        std::string name = std::string("<pyllars.") + std::string(__pyllars_internal::type_name<number_type>()) +
                           std::string("> value=") + std::to_string(obj->value);
        return PyString_FromString(name.c_str());
    }

    template<typename number_type>
    PythonClassWrapper<number_type *> *
    PyConstFloatingPtCustomObject<number_type>::alloc(PyObject *cls, PyObject *args, PyObject *kwds) {
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
        typename std::remove_const<number_type>::type value = 0;
        if (size >= 1) {
            PyObject *item = PyTuple_GetItem(args, 0);
            if (!item) {
                static const char *const msg = "Internal error getting tuple value";
                PyErr_SetString(PyExc_SystemError, msg);
                return nullptr;
            }
            if (!ConstFloatingPointType<number_type>::isFloatingPtObject(item)) {
                PyErr_SetString(PyExc_ValueError, "Argument must be of integral type");
                return nullptr;
            }
            const double fvalue = ConstFloatingPointType<number_type>::toDouble(item);
            if (value < ConstFloatingPointType<number_type>::min || value > ConstFloatingPointType<number_type>::max) {
                PyErr_SetString(PyExc_ValueError, "Argument out of range");
                return nullptr;
            }
            value = (number_type) fvalue;
        }
        size_t count = 1;
        if (size == 2) {
            PyObject *item = PyTuple_GetItem(args, 1);
            if (!item) {
                PyErr_SetString(PyExc_SystemError, "Internal error getting tuple value");
                return nullptr;
            }
            if (!ConstNumberType<number_type>::isIntegerObject(item)) {
                PyErr_SetString(PyExc_ValueError, "Argument must be of integral type");
                return nullptr;
            }
            const __int128_t long_value = ConstNumberType<number_type>::toLongLong(item);
            if (value < ConstNumberType<number_type>::min || value > ConstNumberType<number_type>::max) {
                PyErr_SetString(PyExc_ValueError, "Argument out of range");
                return nullptr;
            }
            count = long_value;
            if (count <= 0) {
                PyErr_SetString(PyExc_ValueError, "Number of elements to allocate must be greater then 0");
                return nullptr;
            }
        }
        auto *alloced = new number_type(value);
        return PythonClassWrapper<number_type *>::createPy(count, alloced, ContainmentKind::ALLOCATED);
    }


    template<typename number_type>
    int PyConstFloatingPtCustomObject<number_type>::initialize() {
        int rc = PyType_Ready(&CommonBaseWrapper::_BaseType);
        rc |= PyType_Ready(&PyConstFloatingPtCustomBase::Type);
        rc |= PyType_Ready(&PyConstFloatingPtCustomObject::Type);
        return rc;
        //return PyConstFloatingPtCustomObject<number_type>::Initializer::initializer->set_up();
    }

    template<typename number_type>
    PyObject *PyConstFloatingPtCustomObject<number_type>::richcompare(PyObject *a, PyObject *b, int op) {
        if (!ConstFloatingPointType<number_type>::isFloatingPtObject(a) ||
            !ConstFloatingPointType<number_type>::isFloatingPtObject(b)) {
            PyErr_SetString(PyExc_TypeError, "Invalid operands for comparison");
            return NULL;
        }
        double value1 = ConstFloatingPointType<number_type>::toDouble(a);
        double value2 = ConstFloatingPointType<number_type>::toDouble(b);
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
    __pyllars_internal::PythonClassWrapper<number_type> *PyConstFloatingPtCustomObject<number_type>::createPy
    (const ssize_t arraySize,
            ntype& cobj,
            const ContainmentKind  containmentKind,
            PyObject *referencing) {
        static PyObject *kwds = PyDict_New();
        static PyObject *emptyargs = PyTuple_New(0);
        PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);

        auto *pyobj = (PythonClassWrapper<number_type> *) PyObject_Call(
                (PyObject *) getPyType(), emptyargs, kwds);
        pyobj->value = cobj;
        pyobj->_depth = 0;
        return pyobj;
    }

    template<typename number_type>
    int PyConstFloatingPtCustomObject<number_type>::create(PyObject *self_, PyObject *args, PyObject *) {
        auto *self = reinterpret_cast<PyConstFloatingPtCustomObject*>(self_);
        if (self) {
            PyTypeObject* const coreTypePtr = PythonClassWrapper<typename core_type<number_type>::type>::getPyType();
            self->template populate_type_info<number_type>(&checkType, coreTypePtr);
            if (PyTuple_Size(args) == 0) {
                memset(const_cast<typename std::remove_const<number_type>::type *>(&self->value), 0,
                       sizeof(self->value));
            } else if (PyTuple_Size(args) == 1) {
                PyObject *value = PyTuple_GetItem(args, 0);
                if (!ConstFloatingPointType<number_type>::isFloatingPtObject(value)) {
                    PyErr_SetString(PyExc_TypeError, "Argument must be an integer");
                    return -1;
                }
                double fvalue = ConstFloatingPointType<number_type>::toDouble(value);
                if (fvalue < (double) std::numeric_limits<number_type>::min() ||
                    fvalue > (double) std::numeric_limits<number_type>::max()) {
                    PyErr_SetString(PyExc_ValueError, "Argument value out of range");
                    return -1;
                }
                *(const_cast<typename std::remove_const<number_type>::type *>(&self->value)) = (number_type) fvalue;
            } else {
                PyErr_SetString(PyExc_TypeError, "Should only call with at most one arument");
                return -1;
            }
            self->asDouble = [self]() -> double { return (double) self->value; };
            return 0;
        }
        PyErr_SetString(PyExc_TypeError, "Recevied null self !?#");
        return -1;
    }

    template<typename number_type>
    typename PyConstFloatingPtCustomObject<number_type>::Initializer *PyConstFloatingPtCustomObject<number_type>::Initializer::initializer = new PyConstFloatingPtCustomObject<number_type>::Initializer();


    template<typename number_type>
    PyConstFloatingPtCustomObject<number_type>::Initializer::Initializer() {
        pyllars_register(this);
    }

    template<typename number_type>
    status_t PyConstFloatingPtCustomObject<number_type>::Initializer::init(PyObject *const global_mod) {
        static PyObject *module = PyImport_ImportModule("pyllars");
        static bool inited = false;
        if (inited) return 0;
        inited = true;
        int rc = PyType_Ready(&CommonBaseWrapper::_BaseType);
        rc |= PyType_Ready(&PyConstFloatingPtCustomBase::Type);
        rc |= PyType_Ready(&PyConstFloatingPtCustomObject::Type);
        Py_INCREF(&PyConstFloatingPtCustomBase::Type);
        Py_INCREF(&PyConstFloatingPtCustomObject::Type);
        if (module && rc == 0) {
            PyModule_AddObject(module, __pyllars_internal::type_name<number_type>(),
                               (PyObject *) &PyConstFloatingPtCustomObject::Type);
        }
        return rc;
    }

    template
    class PyConstFloatingPtCustomObject<const float>;

    template
    class PyConstFloatingPtCustomObject<const double>;

    ///////////////////////////////////////


    template
    class PythonClassWrapper<const double>;

    template
    class PythonClassWrapper<const float>;


    template
    class PythonClassWrapper<const char>;

    template
    class PythonClassWrapper<const short>;

    template
    class PythonClassWrapper<const int>;

    template
    class PythonClassWrapper<const long>;

    template
    class PythonClassWrapper<const long long>;


    template
    class PythonClassWrapper<const unsigned char>;

    template
    class PythonClassWrapper<const unsigned short>;

    template
    class PythonClassWrapper<const unsigned int>;

    template
    class PythonClassWrapper<const unsigned long>;

    template
    class PythonClassWrapper<const unsigned long long>;



}
