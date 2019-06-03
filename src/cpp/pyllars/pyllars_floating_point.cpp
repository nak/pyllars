//
// Created by jrusnak on 5/8/19.
//

#include <pyllars/pyllars_floating_point.hpp>
#include <pyllars/pyllars_integer.hpp>
#include <pyllars/pyllars_classwrapper.impl.hpp>
#include <pyllars/pyllars_pointer.impl.hpp>
#include <pyllars/pyllars_reference.impl.hpp>


namespace __pyllars_internal{

    PyTypeObject PyFloatingPtCustomBase::_Type = {
#if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(nullptr)
    0,                         /*ob_size*/
#endif
            "PyllarsFloatingPtBase", /*tp_name*/
            sizeof(PyFloatingPtCustomBase), /*tp_basicsize*/
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
    struct FloatingPointType {

        static PyNumberMethods *instance() {
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
            obj.nb_lshift = nullptr;
            obj.nb_rshift = nullptr;
            obj.nb_and = nullptr;
            obj.nb_or = nullptr;
            obj.nb_xor = nullptr;
            obj.nb_int = to_pyint;
            obj.nb_float = to_pyfloat;
            obj.nb_index = to_pyint;
            obj.nb_inplace_add = _baseInplaceBinaryFunc<inplace_add>;
            obj.nb_inplace_subtract = _baseInplaceBinaryFunc<inplace_subtract>;
            obj.nb_inplace_remainder = _baseInplaceBinaryFunc<inplace_remainder>;
            obj.nb_inplace_lshift = nullptr;
            obj.nb_inplace_rshift = nullptr;
            obj.nb_inplace_and = nullptr;
            obj.nb_inplace_or = nullptr;
            obj.nb_inplace_xor = nullptr;

            obj.nb_true_divide = _baseBinaryFunc<divide>;
            obj.nb_floor_divide = _baseBinaryFunc<floor_div>;
            obj.nb_inplace_floor_divide = _baseInplaceBinaryFunc<inplace_floor_div>;
            obj.nb_inplace_true_divide = _baseInplaceBinaryFunc<inplace_divide>;
#if PY_MAJOR_VERSION == 2

            obj.nb_nonzero = nonzero;
#endif
            obj.nb_invert = nullptr;

            return &obj;
        }

        static double toDouble(PyObject *obj) {
            if (PyFloat_Check(obj)) {
                return PyFloat_AsDouble(obj);
            } else if (PyObject_TypeCheck(obj, &PyFloatingPtCustomBase::_Type)) {
                return ((PyFloatingPtCustomObject<number_type> *) obj)->asDouble();
            } else {
                PyErr_SetString(PyExc_SystemError, "System error: invalid type encountered");
                return 0;
            }
        }

        static bool isFloatingPtObject(PyObject *obj) {
            return bool(PyFloat_Check(obj)) || bool(PyObject_TypeCheck(obj, &PyFloatingPtCustomBase::_Type));
        }

        typedef typename std::remove_reference<number_type>::type number_type_basic;

        static constexpr number_type_basic min(){return std::numeric_limits<number_type_basic>::min();}
        static constexpr number_type_basic max = std::numeric_limits<number_type_basic>::max();

        static bool is_out_of_bounds_add(double value1, double value2) {
            return ((value1 > 0.0 && value1 > max - value2) ||
                    (value1 < 0.0 && value1 < min()- value2));
        }

        static bool is_out_of_bounds_subtract(double value1, double value2) {
            return ((value1 > 0.0 && value1 > max + value2) ||
                    (value1 < 0.0 && value1 < min()+ value2));
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
            }
            auto *ret = (PyFloatingPtCustomObject<typename std::remove_const<number_type>::type > *) PyObject_Call(
                    (PyObject *) PyFloatingPtCustomObject<number_type>::getPyType(), emptyargs, nullptr);
            if (!ret) {
                return nullptr;
            }
            *const_cast<typename std::remove_const<number_type_basic>::type *>(ret->_CObject) = ret_value;
            return (PyObject *) ret;
        }

        template<void(*func)(double &, double)>
        static PyObject *_baseInplaceBinaryFunc(PyObject *v1, PyObject *v2) {
            if (!PyObject_TypeCheck(v1, PyFloatingPtCustomObject<number_type>::getPyType())) {
                PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
                Py_RETURN_NOTIMPLEMENTED;
            }
            if (!isFloatingPtObject(v1) || !isFloatingPtObject(v2)) {
                PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
                Py_RETURN_NOTIMPLEMENTED;
            }
            double ret_value = *((PyFloatingPtCustomObject<typename std::remove_const<number_type>::type > *) v1)->_CObject;
            func(ret_value, toDouble(v2));
            if (PyErr_Occurred()) {
                return nullptr;
            }
            *const_cast<typename std::remove_const<number_type_basic >::type*>(((PyFloatingPtCustomObject<number_type> *) v1)->_CObject) = ret_value;
            Py_INCREF(v1);
            return v1;
        }

        template<number_type_basic (*func)(double)>
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

            auto *ret = (PyFloatingPtCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) PyFloatingPtCustomObject<number_type>::getPyType(), emptyargs, nullptr);
            if (ret) {
                *const_cast<typename std::remove_const<number_type_basic>::type*>(ret->_CObject) = ret_value;
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
            const number_type_basic result = value1 * value2;
            if (check && value1 != 0 && result / value1 != value2) {
                PyErr_SetString(PyExc_ValueError, "multiplication of values is out of range");
            }
            return result;
        }

        static PyObject *power(PyObject *v1, PyObject *v2, PyObject *v3) {
            static PyObject *emptyargs = PyTuple_New(0);
            auto *ret = (PyFloatingPtCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) PyFloatingPtCustomObject<number_type>::getPyType(), emptyargs, nullptr);
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
            *const_cast<typename std::remove_const<number_type_basic >::type*>(ret->_CObject) = (number_type) result;
            return (PyObject *) ret;
        }

        static double remainder(double value1, double value2, const bool check) {
            double result;
            if (((value1 < 0.0 and value2 > 0.0) || (value1 > 0.0 && value2 < 0.0)) && fmod(value1, value2) != 0.0) {
                result = value1 - (floor_div(value1, value2, false) * value2);
            } else {
                result = fmod(value1, value2);
            }
            return result;
        }

        static PyObject *positive(PyObject *v1) {
            Py_INCREF(v1);
            return v1;
        }

        static number_type_basic absolute(double value1) {
            return fabs(value1);
        }

        static number_type_basic negative(double value) {
            return -value;
        }

        static PyObject *divmod(PyObject *v1, PyObject *v2) {
            static PyObject *emptyargs = PyTuple_New(0);
            auto *retq = (PyFloatingPtCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) PyFloatingPtCustomObject<number_type>::getPyType(), emptyargs, nullptr);
            auto *retr = (PyFloatingPtCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) PyFloatingPtCustomObject<number_type>::getPyType(), emptyargs, nullptr);
            if (!retq || !retr) {
                return nullptr;
            }

            if (!isFloatingPtObject(v1) || !isFloatingPtObject(v2)) {
                PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
                return nullptr;
            }
            const double value1 = toDouble(v1);
            const double value2 = toDouble(v2);
            const double quotient = (double) ((long long) value1 / (long long) value2);
            const double remainder = value1 - ((double) quotient) * value2;
            if (quotient < min()|| quotient > max || remainder < min()|| remainder > max) {
                static const char *const msg = "Invalid types for arguments";
                PyErr_SetString(PyExc_ValueError, msg);
                return nullptr;
            }
            PyObject *tuple = PyTuple_New(2);
            *const_cast<typename std::remove_const<number_type_basic >::type*>(retq->_CObject) = (number_type_basic) quotient;
            *const_cast<typename std::remove_const<number_type_basic >::type*>(retr->_CObject) = (number_type_basic) remainder;
            PyTuple_SetItem(tuple, 0, reinterpret_cast<PyObject*>(retq));
            PyTuple_SetItem(tuple, 1, reinterpret_cast<PyObject*>(retr));
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

        static void inplace_add(double &value1, double value2) {
            value1 += value2;
        }

        static void inplace_subtract(double &value1, double value2) {
            value1 -= value2;
        }

        static void inplace_multiply(double &value1, double value2) {
            value1 *= value2;
        }

        static void inplace_remainder(double &value1, double value2) {
            value1 = fmod(value1, value2);
        }

        static void inplace_floor_div(double &value1, double value2) {
            auto intv1 = (__int128_t) value1;
            auto intv2 = (__int128_t) value2;
            intv1 /= intv2;

            if (((intv1 < 0 and intv2 > 0) || (intv1 > 0 && intv2 < 0)) && (intv1 % intv2 != 0)) {
                intv1 -= 1;
            }
            value1 = (double) intv1;
        }

        static double floor_div(double v1, double v2, const bool check) {
            auto value1 = (__int128_t) v1;
            auto value2 = (__int128_t) v2;
            return (double) (value1 / value2);
        }

        static double divide(double value1, double value2, const bool check) {
            return value1 / value2;
        }

        static void inplace_divide(double &value1, double value2) {
            value1 /= value2;
        }
    };


    template<typename number_type>
    PyMethodDef PyFloatingPtCustomObject<number_type>::_methods[] = {
            {
                    alloc_name_,
                             (PyCFunction) alloc,
                    METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                    "allocate array of numbers"
            },
            {
                    "to_float",
                             (PyCFunction) to_float,
                    METH_KEYWORDS | METH_VARARGS,
                    "convert to Python float type"
            },
            {
                    nullptr, nullptr, 0, nullptr /**sentinel **/
            }
    };

    template<typename number_type>
    PyTypeObject PyFloatingPtCustomObject<number_type>::_Type = {
#if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(nullptr)
    0,                         /*ob_size*/
#endif
            __pyllars_internal::type_name<number_type>(), /*tp_name*/
            sizeof(PyFloatingPtCustomObject<number_type>), /*tp_basicsize*/
            0, /*tp_itemsize*/
            nullptr, /*tp_dealloc*/
            nullptr, /*tp_print*/
            nullptr, /*tp_getattr*/
            nullptr, /*tp_setattr*/
            nullptr, /*tp_as_sync*/
            PyFloatingPtCustomObject::repr, /*tp_repr*/

            FloatingPointType<number_type>::instance(), /*tp_as_number*/
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
            PyFloatingPtCustomObject::richcompare,                       /* tp_richcompare */
            0,                               /* tp_weaklistoffset */
            nullptr,                       /* tp_iter */
            nullptr,                       /* tp_iternext */
            _methods,             /* tp_methods */
            nullptr,             /* tp_members */
            nullptr,                         /* tp_getset */
            &PyFloatingPtCustomBase::_Type,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            (initproc)PyFloatingPtCustomObject::_init,  /* tp_init */
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
    PyObject *PyFloatingPtCustomObject<number_type>::repr(PyObject *o) {
        auto *obj = (PyFloatingPtCustomObject<number_type> *) o;
        std::string name = std::string("<pyllars.") + std::string(__pyllars_internal::type_name<number_type>()) +
                           std::string("> _CObject=") + std::to_string(*obj->_CObject);
        return PyString_FromString(name.c_str());
    }

    template<typename number_type>
    PythonClassWrapper<typename std::remove_reference<number_type>::type *> *
    PyFloatingPtCustomObject<number_type>::alloc(PyObject *, PyObject *args, PyObject *kwds) {
        if (kwds && PyDict_Size(kwds) > 0) {
            static const char *const msg = "Allocator does not accept keywords";
            PyErr_SetString(PyExc_TypeError, msg);
            return nullptr;
        }
        const ssize_t size = PyTuple_Size(args);
        if (size < 0){
            PyErr_SetString(PyExc_ValueError, "Negative tuple size encountered");
            return nullptr;
        }
        if (size > 2) {
            static const char *const msg = "Too many arguments to call to allocations";
            PyErr_SetString(PyExc_TypeError, msg);
            return nullptr;
        }
        typename std::remove_const<number_type_basic>::type value = 0;
        if (size >= 1) {
            PyObject *item = PyTuple_GetItem(args, 0);
            if (!item) {
                static const char *const msg = "Internal error getting tuple _CObject";
                PyErr_SetString(PyExc_SystemError, msg);
                return nullptr;
            }
            if (!FloatingPointType<number_type>::isFloatingPtObject(item)) {
                PyErr_SetString(PyExc_ValueError, "Argument must be of integral type");
                return nullptr;
            }
            const double fvalue = FloatingPointType<number_type>::toDouble(item);
            if (value < FloatingPointType<number_type>::min()|| value > FloatingPointType<number_type>::max) {
                PyErr_SetString(PyExc_ValueError, "Argument out of range");
                return nullptr;
            }
            value = (number_type) fvalue;
        }
        size_t count = 1;
        if (size == 2) {
            PyObject *item = PyTuple_GetItem(args, 1);
            if (!item) {
                PyErr_SetString(PyExc_SystemError, "Internal error getting tuple _CObject");
                return nullptr;
            }
            if (!bool(PyLong_Check(item)) || bool(PyObject_TypeCheck(item, &PyFloatingPtCustomBase::_Type))){
                PyErr_SetString(PyExc_ValueError, "Argument must be of integral type");
                return nullptr;
            }
            const __int128_t long_value = NumberType<number_type_basic>::toLongLong(item);
            if (value < NumberType<number_type>::min() || value > NumberType<number_type>::max) {
                PyErr_SetString(PyExc_ValueError, "Argument out of range");
                return nullptr;
            }
            count = (size_t) long_value;
            if (count <= 0) {
                PyErr_SetString(PyExc_ValueError, "Number of elements to allocate must be greater then 0");
                return nullptr;
            }
        }
        if (count <= 1){
            return PythonClassWrapper<number_type_basic *>::template allocateInstance<number_type_basic>(value);
        } else {
            return PythonClassWrapper<number_type_basic *>::template allocateArray<number_type_basic>(value, count);
        }
    }


    template<typename number_type>
    PyObject *
    PyFloatingPtCustomObject<number_type>::to_float(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!PyTuple_Check(args) || PyTuple_Size(args) + kwds?PyDict_Size(kwds):0 > 0){
            PyErr_SetString(PyExc_TypeError, "to_float takes no arguments");
            return nullptr;
        }
        if (!PyObject_TypeCheck(self, getPyType())){
            PyErr_SetString(PyExc_TypeError, "invalid type for self");
            return nullptr;
        }
        auto self_ = reinterpret_cast<PyFloatingPtCustomObject*>(self);
        return PyFloat_FromDouble(*self_->get_CObject());
    }


    template<typename number_type>
    int PyFloatingPtCustomObject<number_type>::_initialize(PyTypeObject &type) {
        static int rc = -1;
        static bool inited = false;
        if (inited){
            return rc;
        }
        rc = PyType_Ready(&PyFloatingPtCustomBase::_Type);
        rc |= PyType_Ready(&type);
        return rc;
    }

    template<typename number_type>
    PyObject *PyFloatingPtCustomObject<number_type>::richcompare(PyObject *a, PyObject *b, int op) {
        if (!FloatingPointType<number_type>::isFloatingPtObject(a) ||
            !FloatingPointType<number_type>::isFloatingPtObject(b)) {
            PyErr_SetString(PyExc_TypeError, "Invalid operands for comparison");
            return NULL;
        }
        double value1 = FloatingPointType<number_type>::toDouble(a);
        double value2 = FloatingPointType<number_type>::toDouble(b);
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
    __pyllars_internal::PythonClassWrapper<number_type> *PyFloatingPtCustomObject<number_type>::fromCObject
            ( number_type& cobj, PyObject *referencing) {
        static PyObject *kwds = PyDict_New();
        static PyObject *emptyargs = PyTuple_New(0);
        PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);
        PythonClassWrapper<number_type>::initialize();
        auto type = getPyType();
        if (!type){
            PyErr_SetString(PyExc_SystemError, "fault in pyllars to initialize an underlying Python type");
            return nullptr;
        }
        auto *pyobj = (__pyllars_internal::PythonClassWrapper<number_type> *) PyObject_Call((PyObject *) type, emptyargs, kwds);
        if (pyobj) {
            pyobj->_CObject = new number_type(cobj);//ObjectLifecycleHelpers::Copy<number_type_basic>::new_copy(cobj);
            pyobj->make_reference(referencing);
        }
        return pyobj;
    }

    template<typename number_type>
    int PyFloatingPtCustomObject<number_type>::_init(PyFloatingPtCustomObject *self, PyObject *args, PyObject *kw) {
        PyTypeObject * const coreTypePtr = PythonClassWrapper<typename core_type<number_type>::type>::getPyType();
        // self->template populate_type_info< number_type>(&checkType, coreTypePtr);
        if (self) {
            if (PyTuple_Size(args) == 0) {
                if constexpr(std::is_reference<number_type>::value){
                    if (kw && PyODict_GetItemString(kw, "__internal_allow_null")){
                        self->_CObject = nullptr;
                    } else {
                        PyErr_SetString(PyExc_TypeError, "Cannot initialize reference type without initial _CObject");
                        return -1;
                    }
                } else {
                    self->_CObject = new number_type_basic(0.0);
                }
            } else if (PyTuple_Size(args) == 1) {
                PyObject *value = PyTuple_GetItem(args, 0);
                if (!FloatingPointType<number_type>::isFloatingPtObject(value)) {
                    PyErr_SetString(PyExc_TypeError, "Argument must be a floating point object");
                    return -1;
                }
                double fvalue = FloatingPointType<number_type>::toDouble(value);
                if (fvalue < (double) std::numeric_limits<number_type_basic >::min() ||
                    fvalue > (double) std::numeric_limits<number_type_basic >::max()) {
                    PyErr_SetString(PyExc_ValueError, "Argument _CObject out of range");
                    return -1;
                }
                if constexpr (std::is_reference<number_type>::value){
                    if(!PyObject_TypeCheck(value, &PyFloatingPtCustomBase::_Type)){
                        PyErr_SetString(PyExc_TypeError, "Cannot initialize reference type _CObject from non-ltype");
                        return -1;
                    }
                    self->_CObject = reinterpret_cast<PyFloatingPtCustomObject<number_type>*>(value)->get_CObject();
                } else {
                    self->_CObject = new  number_type_basic(fvalue);
                }
            } else {
                PyErr_SetString(PyExc_TypeError, "Should only call with at most one arument");
                return -1;
            }
            self->asDouble = [self]() -> double { return (double) *self->_CObject; };
            PythonClassWrapper<number_type>::initialize();
            return 0;
        }
        PyErr_SetString(PyExc_TypeError, "Recevied null self !?#");
        return -1;
    }

    template<typename number_type>
    typename PyFloatingPtCustomObject<number_type>::Initializer *PyFloatingPtCustomObject<number_type>::Initializer::initializer = new PyFloatingPtCustomObject<number_type>::Initializer();


    template<typename number_type>
    PyFloatingPtCustomObject<number_type>::Initializer::Initializer() {
        pyllars_register(this);
    }

    template<typename number_type>
    status_t PyFloatingPtCustomObject<number_type>::Initializer::set_up() {
        static PyObject *module = PyImport_ImportModule("pyllars");
        PyType_Ready(&PyFloatingPtCustomBase::_Type);
        const int rc = PyType_Ready(&PyFloatingPtCustomObject::_Type);
        Py_INCREF(&PyFloatingPtCustomBase::_Type);
        Py_INCREF(&PyFloatingPtCustomObject::_Type);
        if (module && rc == 0) {
            PyModule_AddObject(module, __pyllars_internal::type_name<number_type>(),
                               (PyObject *) &PyFloatingPtCustomObject::_Type);
        }
        return rc;
    }

    template <typename number_type>
    typename std::remove_const<number_type>::type &
    __pyllars_internal::PyFloatingPtCustomObject<number_type>::toCArgument(){
        if constexpr (std::is_const<number_type>::value){
            throw "Cannot pass const value as non-const reference argument";
        } else {
            return *get_CObject();
        }
    }

    template <typename number_type>
    const number_type &
    __pyllars_internal::PyFloatingPtCustomObject<number_type>::toCArgument() const{
        return *const_cast<const number_type*>(get_CObject());
    }

    template
    class PyFloatingPtCustomObject<float>;

    template
    class PyFloatingPtCustomObject<double>;


    template
    class PyFloatingPtCustomObject<const float>;

    template
    class PyFloatingPtCustomObject<const double>;


    template
    class PyFloatingPtCustomObject<volatile float>;

    template
    class PyFloatingPtCustomObject<volatile double>;


    template
    class PyFloatingPtCustomObject<volatile const float>;

    template
    class PyFloatingPtCustomObject<volatile const double>;
}