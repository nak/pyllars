
#include <stdio.h>
#include <limits>
#include <math.h>

#include "pyllars.hpp"
#include "pyllars_pointer.impl.hpp"
#include "pyllars_classwrapper.impl.hpp"

pyllars::Initializer *pyllars::Initializer::root = nullptr;


static void _pyllars_import_to_top(PyObject* pyllars_mod, PyObject* module){
  if(!pyllars_mod || !module){
    return;
  }
  PyObject *dir = PyObject_Dir(module);
  if(!dir){
    return;
  }
  PyObject *iterator = PyObject_GetIter(dir);
  PyObject *name;
  while((name = PyIter_Next(iterator))){
    PyObject * obj = PyObject_GetAttr(module, name);
    if (obj){
      PyObject_SetAttr(pyllars_mod, name, obj);
    }
    Py_DECREF(name);
  }
  Py_XDECREF(dir);
}

#if PY_MAJOR_VERSION == 3

PyObject*
PyllarsInit(const char* const name){
  static PyObject *const pyllars_mod = PyImport_ImportModule("pyllars");
  static const char* const doc = "Pyllars top-level module";
  static PyModuleDef moduleDef = {
    PyModuleDef_HEAD_INIT,
    name,
    doc,
    -1,
    nullptr
  };

  PyObject* mod = PyModule_Create(&moduleDef);
  if(mod){
    Py_INCREF(mod);
    PyObject_SetAttrString(pyllars_mod, name, mod);
    int rc = pyllars::Initializer::root? pyllars::Initializer::root->set_up():0;
    if (0 == rc) {
       rc = pyllars::Initializer::root?pyllars::Initializer::root->ready(mod):0;
    } 
    if (rc != 0){
      printf("Failed to initialize some components of %s", name);
    }
    _pyllars_import_to_top(pyllars_mod, mod);
  }
  return mod;
}
#else
PyMODINIT_FUNC
int PyllarsInit(const char* const name){
  static PyObject *const pyllars_mod = PyImport_ImportModule("pyllars");
  PyObject *mod = Py_InitModule3(name, nullptr, "Pyllars top-level module");
  if(!mod) { return -1;}
  PyObject_SetAttr(pyllars_mod, name, mod);
  int rc = pyllars::Initializer::root?pyllars::Initializer::root->init(mod):0;
  if (0 == rc) {
    rc = pyllars::Initializer::root?pyllars::Initializer::root->init_last(mod):0;
  }
  if (rc != 0){
    printf("Failed to initializer some components of %s", name);
  }
  _pyllars_import_to_top(pyllar_mod, mod);
  return rc;
}
#endif

int pyllars::pyllars_register( Initializer* const init){
    // ensure root is "clean" and no static initizlied as this function
    // may be called during static initialization before root has been assigend
    // a static value
    static Initializer _root;
    if(!Initializer::root)
      Initializer::root = &_root;

    return Initializer::root->register_init(init);
}


namespace __pyllars_internal {


    PyTypeObject PyNumberCustomBase::Type = {
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
    typename PyNumberCustomObject<number_type>::Initializer *PyNumberCustomObject<number_type>::Initializer::initializer = new PyNumberCustomObject<number_type>::Initializer();


    template<typename number_type>
    struct NumberType {

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

        static __int128_t toLongLong(PyObject *obj) {
            if (PyLong_Check(obj)) {
                if (min == 0) {
                    return PyLong_AsUnsignedLongLong(obj);
                }
                return PyLong_AsLongLong(obj);
            } else if (PyObject_TypeCheck(obj, &PyNumberCustomBase::Type)) {
                return ((PyNumberCustomObject<number_type> *) obj)->asLongLong();
            } else {
                PyErr_SetString(PyExc_SystemError, "System error: invalid type encountered");
                return 0;
            }
        }

        static bool isIntegerObject(PyObject *obj) {
            return bool(PyLong_Check(obj)) || bool(PyObject_TypeCheck(obj, &PyNumberCustomBase::Type));
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
                if (NumberType<number_type>::min == 0) {
                    return PyLong_FromUnsignedLongLong(ret_value);
                }
                return PyLong_FromLongLong(ret_value);
            } else if (ret_value < min || ret_value > max) {
                PyErr_SetString(PyExc_ValueError, "Result out of range");
                return nullptr;
            }
            PyNumberCustomObject<number_type> *ret = (PyNumberCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
            if (!ret) {
                return nullptr;
            }
            *const_cast<typename std::remove_const<number_type>::type *>(&ret->value) = ret_value;
            return (PyObject *) ret;
        }

        template<void(*func)(__int128_t &, number_type)>
        static PyObject *_baseInplaceBinaryFunc(PyObject *v1, PyObject *v2) {
            if (!PyObject_TypeCheck(v1, &PyNumberCustomObject<number_type>::Type)) {
                PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
                Py_RETURN_NOTIMPLEMENTED;
            }
            if (!isIntegerObject(v1) || !isIntegerObject(v2)) {
                PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
                Py_RETURN_NOTIMPLEMENTED;
            }
            __int128_t ret_value = ((PyNumberCustomObject<number_type> *) v1)->value;
            func(ret_value, toLongLong(v2));
            if (PyErr_Occurred()) {
                return nullptr;
            }
            if (ret_value < min || ret_value > max) {
                PyErr_SetString(PyExc_ValueError, "Result out of range");
                return nullptr;
            }
            *const_cast<typename std::remove_const<number_type>::type *>(&((PyNumberCustomObject<number_type> *) v1)->value) = ret_value;
            Py_INCREF(v1);
            return v1;
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

            PyNumberCustomObject<number_type> *ret = (PyNumberCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
            if (ret) {
                ret->value = ret_value;
            }
            return (PyObject *) ret;
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
            PyNumberCustomObject<number_type> *ret = (PyNumberCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
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
            ret->value = (number_type) result;
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
                PyErr_SetString(PyExc_ValueError, "Result is out of range");
            }
            return result;
        }

        static PyObject *divmod(PyObject *v1, PyObject *v2) {
            static PyObject *emptyargs = PyTuple_New(0);
            PyNumberCustomObject<number_type> *retq = (PyNumberCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
            PyNumberCustomObject<number_type> *retr = (PyNumberCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
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
            retq->value = (number_type) quotient;
            retr->value = (number_type) remainder;
            PyTuple_SetItem(tuple, 0, (PyObject *) retq);
            PyTuple_SetItem(tuple, 1, (PyObject *) retr);
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
            return PyLong_FromLong(toLongLong(value));
        }

        static PyObject *to_pyfloat(PyObject *value) {
            return PyFloat_FromDouble((double) toLongLong(value));
        }

        static void inplace_add(__int128_t &value1, number_type value2) {
            if (is_out_of_bounds_add(value1, value2)) {
                PyErr_SetString(PyExc_ValueError, "Values out of range for in place addition");
            }
            value1 += value2;
        }

        static void inplace_subtract(__int128_t &value1, number_type value2) {
            if (is_out_of_bounds_subtract(value1, value2)) {
                PyErr_SetString(PyExc_ValueError, "Values out of range for in place subtraction");
            }
            value1 -= value2;
        }

        static void inplace_multiply(__int128_t &value1, number_type value2) {
            const number_type orig = value1;
            value1 *= value2;
            if (value2 != 0 && value1 / value2 != orig) {
                PyErr_SetString(PyExc_ValueError, "multiplication of values is out of range");
            }
        }

        static void inplace_remainder(__int128_t &value1, number_type value2) {
            value1 %= value2;
        }

        static void inplace_lshift(__int128_t &value1, number_type value2) {
            value1 <<= value2;
        }

        static void inplace_rshift(__int128_t &value1, number_type value2) {
            value1 >>= value2;
        }

        static void inplace_and(__int128_t &value1, number_type value2) {
            value1 &= value2;
        }

        static void inplace_or(__int128_t &value1, number_type value2) {
            value1 |= value2;
        }

        static void inplace_xor(__int128_t &value1, number_type value2) {
            value1 ^= value2;
        }

        static void inplace_floor_div(__int128_t &value1, number_type value2) {
            value1 /= value2;
            if (((value1 < 0 and value2 > 0) || (value1 > 0 && value2 < 0)) && (value1 % value2 != 0)) {
                value1 -= 1;
            }
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

        static PyObject *inplace_divide(PyObject *v1, PyObject *v2) {
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
                    METH_KEYWORDS  | METH_VARARGS,
                    "convert to Python int type"
            },
            {
                    nullptr, nullptr, 0, nullptr /**sentinel **/
            }
    };

    template<typename number_type>
    PyTypeObject PyNumberCustomObject<number_type>::Type = {
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
            &PyNumberCustomBase::Type,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            PyNumberCustomObject::create,  /* tp_init */
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
    PyObject *PyNumberCustomObject<number_type>::repr(PyObject *o) {
        PyNumberCustomObject<number_type> *obj = (PyNumberCustomObject<number_type> *) o;
        std::string name = std::string("<pyllars.") + std::string(__pyllars_internal::type_name<number_type>()) +
                           std::string("> value=") + std::to_string(obj->value);
        return PyString_FromString(name.c_str());
    }


    template<typename number_type>
    PythonClassWrapper<number_type *> *
    PyNumberCustomObject<number_type>::alloc(PyObject *cls, PyObject *args, PyObject *kwds) {
        if (kwds && PyDict_Size(kwds) > 0) {
            static const char *const msg = "Allocator does not accept keywords";
            PyErr_SetString(PyExc_TypeError, msg);
            return nullptr;
        }
        const size_t size = PyTuple_Size(args);
        if (size > 2) {
            static const char *const msg = "Too many arguments to call to allocations";
            PyErr_SetString(PyExc_TypeError, msg);
            return nullptr;
        }
        number_type value = 0;
        if (size >= 1) {
            PyObject *item = PyTuple_GetItem(args, 0);
            if (!item) {
                static const char *const msg = "Internal error getting tuple value";
                PyErr_SetString(PyExc_SystemError, msg);
                return nullptr;
            }
            if (!NumberType<number_type>::isIntegerObject(item)) {
                PyErr_SetString(PyExc_ValueError, "Argument must be of integral type");
                return nullptr;
            }
            const __int128_t long_value = NumberType<number_type>::toLongLong(item);
            if (long_value < NumberType<number_type>::min || long_value > NumberType<number_type>::max) {
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
            if (!NumberType<number_type>::isIntegerObject(item)) {
                PyErr_SetString(PyExc_ValueError, "Argument must be of integral type");
                return nullptr;
            }
            const __int128_t long_value = NumberType<number_type>::toLongLong(item);
            if (long_value < NumberType<number_type>::min || long_value > NumberType<number_type>::max) {
                PyErr_SetString(PyExc_ValueError, "Argument out of range");
                return nullptr;
            }
            count = (number_type) long_value;
            if (count <= 0) {
                PyErr_SetString(PyExc_ValueError, "Number of elements to allocate must be greater then 0");
                return nullptr;
            }
        }
        number_type *alloced = new number_type(value);
        return (PythonClassWrapper<number_type *> *) PythonClassWrapper<number_type *>::createPy2(count, &alloced, true,
                                                                                                  false, nullptr);
    }

    template<typename number_type>
    int PyNumberCustomObject<number_type>::initialize() {
        static bool inited = false;
        static int rc = -1;
        if(inited) return rc;
        inited = true;
        rc = PyType_Ready(&CommonBaseWrapper::_BaseType);
        rc |= PyType_Ready(&PyNumberCustomBase::Type);
        rc |= PyType_Ready(&PyNumberCustomObject::Type);
        return rc;
    }


    template<typename number_type>
    PyObject *
    PyNumberCustomObject<number_type>::to_int(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!PyTuple_Check(args) || PyTuple_Size(args) + kwds?PyDict_Size(kwds):0 > 0){
            PyErr_SetString(PyExc_TypeError, "to_float takes no arguments");
            return nullptr;
        }
        if (!PyObject_TypeCheck(self, getPyType())){
            PyErr_SetString(PyExc_TypeError, "invalid type for self");
            return nullptr;
        }
        auto self_ = reinterpret_cast<PyNumberCustomObject*>(self);
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
    __pyllars_internal::PythonClassWrapper<number_type> *PyNumberCustomObject<number_type>::createPy
            (const ssize_t arraySize,
             __pyllars_internal::ObjContainer<ntype> *const cobj,
             const bool isAllocated,
             const bool inPlace, PyObject *referencing,
             const size_t depth) {
        static PyObject *kwds = PyDict_New();
        static PyObject *emptyargs = PyTuple_New(0);
        PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);

        __pyllars_internal::PythonClassWrapper<number_type> *pyobj = (__pyllars_internal::PythonClassWrapper<number_type> *) PyObject_Call(
                (PyObject *) &Type, emptyargs, kwds);
        pyobj->_depth = 0;
        return pyobj;
    }

    template<typename number_type>
    void PyNumberCustomObject<number_type>::make_reference(PyObject *obj) {
        if (_referenced) { Py_DECREF(_referenced); }
        if (obj) { Py_INCREF(obj); }
        _referenced = obj;
    }

    template<typename number_type>
    int PyNumberCustomObject<number_type>::create(PyObject *self_, PyObject *args, PyObject *kwds) {
        PyNumberCustomObject *self = (PyNumberCustomObject *) self_;
        if (self) {
            PyTypeObject* const coreTypePtr = PythonClassWrapper<typename core_type<number_type>::type>::getPyType();
            self->template populate_type_info< number_type>(&checkType, coreTypePtr);
            if (PyTuple_Size(args) == 0) {
                memset(const_cast<typename std::remove_const<number_type>::type *>(&self->value), 0,
                       sizeof(self->value));
            } else if (PyTuple_Size(args) == 1) {
                PyObject *value = PyTuple_GetItem(args, 0);
                if (!NumberType<number_type>::isIntegerObject(value)) {
                    PyErr_SetString(PyExc_TypeError, "Argument must be an integer");
                    return -1;
                }
                __int128_t longvalue = NumberType<number_type>::toLongLong(value);
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
    PyNumberCustomObject<number_type>::Initializer::Initializer() {
        pyllars_register(this);
    }

    template<typename number_type>
    status_t PyNumberCustomObject<number_type>::Initializer::set_up() {
        static PyObject *module = PyImport_ImportModule("pyllars");
        int rc = PyType_Ready(&CommonBaseWrapper::_BaseType);
        rc |= PyType_Ready(&PyNumberCustomBase::Type);
        rc |= PyType_Ready(&PyNumberCustomObject::Type);
        Py_INCREF(&PyNumberCustomBase::Type);
        Py_INCREF(&PyNumberCustomObject::Type);
        if (module && rc == 0) {
            PyModule_AddObject(module, __pyllars_internal::type_name<number_type>(),
                               (PyObject *) &PyNumberCustomObject::Type);
        }
        return rc;
    }

    template
    class PyNumberCustomObject<char>;

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


//////////////////////////////

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
            } else if (PyObject_TypeCheck(obj, &PyFloatingPtCustomBase::Type)) {
                return ((PyFloatingPtCustomObject<number_type> *) obj)->asDouble();
            } else {
                PyErr_SetString(PyExc_SystemError, "System error: invalid type encountered");
                return 0;
            }
        }

        static bool isFloatingPtObject(PyObject *obj) {
            return bool(PyFloat_Check(obj)) || bool(PyObject_TypeCheck(obj, &PyFloatingPtCustomBase::Type));
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
            }
            PyFloatingPtCustomObject<number_type> *ret = (PyFloatingPtCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) PyFloatingPtCustomObject<number_type>::getPyType(), emptyargs, nullptr);
            if (!ret) {
                return nullptr;
            }
            *const_cast<typename std::remove_const<number_type>::type *>(&ret->value) = ret_value;
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
            double ret_value = ((PyFloatingPtCustomObject<number_type> *) v1)->value;
            func(ret_value, toDouble(v2));
            if (PyErr_Occurred()) {
                return nullptr;
            }
            *const_cast<typename std::remove_const<number_type>::type *>(&((PyFloatingPtCustomObject<number_type> *) v1)->value) = ret_value;
            Py_INCREF(v1);
            return v1;
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

            PyFloatingPtCustomObject<number_type> *ret = (PyFloatingPtCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) PyFloatingPtCustomObject<number_type>::getPyType(), emptyargs, nullptr);
            if (ret) {
                ret->value = ret_value;
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
            PyFloatingPtCustomObject<number_type> *ret = (PyFloatingPtCustomObject<number_type> *) PyObject_Call(
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
            ret->value = (number_type) result;
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

        static number_type absolute(double value1) {
            return fabs(value1);
        }

        static number_type negative(double value) {
            return -value;
        }

        static PyObject *divmod(PyObject *v1, PyObject *v2) {
            static PyObject *emptyargs = PyTuple_New(0);
            PyFloatingPtCustomObject<number_type> *retq = (PyFloatingPtCustomObject<number_type> *) PyObject_Call(
                    (PyObject *) PyFloatingPtCustomObject<number_type>::getPyType(), emptyargs, nullptr);
            PyFloatingPtCustomObject<number_type> *retr = (PyFloatingPtCustomObject<number_type> *) PyObject_Call(
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
            if (quotient < min || quotient > max || remainder < min || remainder > max) {
                static const char *const msg = "Invalid types for arguments";
                PyErr_SetString(PyExc_ValueError, msg);
                return nullptr;
            }
            PyObject *tuple = PyTuple_New(2);
            retq->value = (number_type) quotient;
            retr->value = (number_type) remainder;
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
            return PyFloat_FromDouble((double) toDouble(value));
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
            __int128_t intv1 = (__int128_t) value1;
            __int128_t intv2 = (__int128_t) value2;
            intv1 /= intv2;

            if (((intv1 < 0 and intv2 > 0) || (intv1 > 0 && intv2 < 0)) && (intv1 % intv2 != 0)) {
                intv1 -= 1;
            }
            value1 = (double) intv1;
        }

        static double floor_div(double v1, double v2, const bool check) {
            __int128_t value1 = (__int128_t) v1;
            __int128_t value2 = (__int128_t) v2;
            return (double) (value1 / value2);
        }

        static double divide(double value1, double value2, const bool check) {
            return value1 / value2;
        }

        static void inplace_divide(double &value1, double value2) {
            value1 /= value2;
        }
    };


    PyTypeObject PyFloatingPtCustomBase::Type = {
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
    PyTypeObject PyFloatingPtCustomObject<number_type>::Type = {
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
            &PyFloatingPtCustomBase::Type,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            PyFloatingPtCustomObject::create,  /* tp_init */
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
        PyFloatingPtCustomObject<number_type> *obj = (PyFloatingPtCustomObject<number_type> *) o;
        std::string name = std::string("<pyllars.") + std::string(__pyllars_internal::type_name<number_type>()) +
                           std::string("> value=") + std::to_string(obj->value);
        return PyString_FromString(name.c_str());
    }

    template<typename number_type>
    PythonClassWrapper<number_type *> *
    PyFloatingPtCustomObject<number_type>::alloc(PyObject *cls, PyObject *args, PyObject *kwds) {
        if (kwds && PyDict_Size(kwds) > 0) {
            static const char *const msg = "Allocator does not accept keywords";
            PyErr_SetString(PyExc_TypeError, msg);
            return nullptr;
        }
        const size_t size = PyTuple_Size(args);
        if (size > 2) {
            static const char *const msg = "Too many arguments to call to allocations";
            PyErr_SetString(PyExc_TypeError, msg);
            return nullptr;
        }
        number_type value = 0;
        if (size >= 1) {
            PyObject *item = PyTuple_GetItem(args, 0);
            if (!item) {
                static const char *const msg = "Internal error getting tuple value";
                PyErr_SetString(PyExc_SystemError, msg);
                return nullptr;
            }
            if (!FloatingPointType<number_type>::isFloatingPtObject(item)) {
                PyErr_SetString(PyExc_ValueError, "Argument must be of integral type");
                return nullptr;
            }
            const double fvalue = FloatingPointType<number_type>::toDouble(item);
            if (value < FloatingPointType<number_type>::min || value > FloatingPointType<number_type>::max) {
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
            if (!NumberType<number_type>::isIntegerObject(item)) {
                PyErr_SetString(PyExc_ValueError, "Argument must be of integral type");
                return nullptr;
            }
            const __int128_t long_value = NumberType<number_type>::toLongLong(item);
            if (value < NumberType<number_type>::min || value > NumberType<number_type>::max) {
                PyErr_SetString(PyExc_ValueError, "Argument out of range");
                return nullptr;
            }
            count = long_value;
            if (count <= 0) {
                PyErr_SetString(PyExc_ValueError, "Number of elements to allocate must be greater then 0");
                return nullptr;
            }
        }
        number_type *alloced = new number_type(value);
        return (PythonClassWrapper<number_type *> *) PythonClassWrapper<number_type *>::createPy2(count, &alloced, true,
                                                                                                  false, nullptr);
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
    int PyFloatingPtCustomObject<number_type>::initialize() {
        int rc = PyType_Ready(&CommonBaseWrapper::_BaseType);
        rc |= PyType_Ready(&PyFloatingPtCustomBase::Type);
        rc |= PyType_Ready(&PyFloatingPtCustomObject::Type);
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
    __pyllars_internal::PythonClassWrapper<number_type> *PyFloatingPtCustomObject<number_type>::createPy
            (const ssize_t arraySize,
             __pyllars_internal::ObjContainer<ntype> *const cobj,
             const bool isAllocated,
             const bool inPlace, PyObject *referencing,
             const size_t depth) {
        static PyObject *kwds = PyDict_New();
        static PyObject *emptyargs = PyTuple_New(0);
        PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);

        __pyllars_internal::PythonClassWrapper<number_type> *pyobj = (__pyllars_internal::PythonClassWrapper<number_type> *) PyObject_Call(
                (PyObject *) &Type, emptyargs, kwds);
        pyobj->_depth = 0;
        return pyobj;
    }


    template<typename number_type>
    void PyFloatingPtCustomObject<number_type>::make_reference(PyObject *obj) {
        if (_referenced) { Py_DECREF(_referenced); }
        if (obj) { Py_INCREF(obj); }
        _referenced = obj;
    }

    template<typename number_type>
    int PyFloatingPtCustomObject<number_type>::create(PyObject *self_, PyObject *args, PyObject *kwds) {
        PyFloatingPtCustomObject *self = (PyFloatingPtCustomObject *) self_;
        PyTypeObject * const coreTypePtr = PythonClassWrapper<typename core_type<number_type>::type>::getPyType();
        self->template populate_type_info< number_type>(&checkType, coreTypePtr);
        if (self) {
            if (PyTuple_Size(args) == 0) {
                memset(const_cast<typename std::remove_const<number_type>::type *>(&self->value), 0,
                       sizeof(self->value));
            } else if (PyTuple_Size(args) == 1) {
                PyObject *value = PyTuple_GetItem(args, 0);
                if (!FloatingPointType<number_type>::isFloatingPtObject(value)) {
                    PyErr_SetString(PyExc_TypeError, "Argument must be an integer");
                    return -1;
                }
                double fvalue = FloatingPointType<number_type>::toDouble(value);
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
    typename PyFloatingPtCustomObject<number_type>::Initializer *PyFloatingPtCustomObject<number_type>::Initializer::initializer = new PyFloatingPtCustomObject<number_type>::Initializer();


    template<typename number_type>
    PyFloatingPtCustomObject<number_type>::Initializer::Initializer() {
        pyllars_register(this);
    }

    template<typename number_type>
    status_t PyFloatingPtCustomObject<number_type>::Initializer::set_up() {
        static PyObject *module = PyImport_ImportModule("pyllars");
        PyType_Ready(&PyFloatingPtCustomBase::Type);
        const int rc = PyType_Ready(&PyFloatingPtCustomObject::Type);
        Py_INCREF(&PyFloatingPtCustomBase::Type);
        Py_INCREF(&PyFloatingPtCustomObject::Type);
        if (module && rc == 0) {
            PyModule_AddObject(module, __pyllars_internal::type_name<number_type>(),
                               (PyObject *) &PyFloatingPtCustomObject::Type);
        }
        return rc;
    }

    template
    class PyFloatingPtCustomObject<float>;

    template
    class PyFloatingPtCustomObject<double>;

    ///////////////////////////////////////


    int getType(PyObject *obj, ffi_type *&type) {
        int subtype = 0;
        if (PyInt_Check(obj)) {
            type = &ffi_type_sint32;
        } else if (PyLong_Check(obj)) {
            type = &ffi_type_sint64;
        } else if (PyFloat_Check(obj)) {
            type = &ffi_type_double;
        } else if (PyBool_Check(obj)) {
            type = &ffi_type_uint8;
        } else if (PyString_Check(obj)) {
            type = &ffi_type_pointer;
            subtype = STRING_TYPE;
#if PY_MAJOR_VERSION == 3
        } else if (PyBytes_Check(obj)) {
            type = &ffi_type_pointer;
            subtype = STRING_TYPE;
#endif
        } else if (CommonBaseWrapper::IsClassType(obj)) {
            type = &ffi_type_pointer;
            subtype = COBJ_TYPE;
        } else if (CommonBaseWrapper::IsCFunctionType(obj)) {
            subtype = FUNC_TYPE;
            type = &ffi_type_pointer;
        } else {
            throw "Cannot conver Python object to C Object";
        }/*else if (PyList_Check(obj)){
            const C_TYPE subtype = PyTuple_Size(obj)>0?getType(PyList_GetItem(obj,0)):C_TYPE::INT;
            for(int i = 1; i < PyTuple_Size(obj); ++i){
                if(getType(PyList_GetItem(obj, i)) != subtype){
                    throw "Cannot convert mixed type list to C array";
                }
            }
            type = C_TYPE::ARRAY;
        }*/
        return subtype;
    }

    PyTypeObject CommonBaseWrapper::_BaseType ={
#if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(nullptr)
            0,                         /*ob_size*/
#endif
            "BasePythonFromCWrapper",             /*tp_name*/ /*filled on init*/
            sizeof(CommonBaseWrapper),             /*tp_basicsize*/
            0,                         /*tp_itemsize*/
            nullptr, //(destructor) CommonBaseWrapper::_dealloc, /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            nullptr,                         /*tp_getattr*/
            nullptr,                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            nullptr,          /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            nullptr,                         /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES, /*tp_flags*/
            "BasePythonFromCWrapper object",           /* tp_doc */
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
            CommonBaseWrapper::__init,  /* tp_init */
            nullptr,                         /* tp_alloc */
            CommonBaseWrapper::_new,             /* tp_new */
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
}
#include "pyllars_const.cpp"
