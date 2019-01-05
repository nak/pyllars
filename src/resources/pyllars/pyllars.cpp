
#include <stdio.h>
#include <limits>

#include "pyllars.hpp"
#include "pyllars_varargs.impl"
#include "pyllars_templates.impl"
#include "pyllars_pointer.impl"
#include "pyllars_classwrapper.impl"

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
    int rc = pyllars::Initializer::root?pyllars::Initializer::root->init(mod):0;
    if (0 == rc) {
       rc = pyllars::Initializer::root?pyllars::Initializer::root->init_last(mod):0;
    } 
    if (rc != 0){
      printf("Failed to initializer some components of %s", name);
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

int pyllars::pyllars_register_last( Initializer* const init){
    // ensure root is "clean" and no static initizlied as this function
    // may be called during static initialization before root has been assigend
    // a static value
    static Initializer _root;
    if(!Initializer::root)
      Initializer::root = &_root;

    return Initializer::root->register_init_last(init);
}

namespace __pyllars_internal{


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
typename PyNumberCustomObject<number_type>::Initializer* PyNumberCustomObject<number_type>::Initializer::initializer = new  PyNumberCustomObject<number_type>::Initializer();


template<typename number_type>
struct NumberType{

    static PyNumberMethods *instance(){
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

    static long long toLongLong(PyObject* obj){
        if(PyLong_Check(obj)){
            return PyLong_AsLong(obj);
        } else if(PyObject_TypeCheck(obj, &PyNumberCustomBase::Type)){
            return ((PyNumberCustomObject<number_type>*) obj)->asLongLong();
        } else {
            PyErr_SetString(PyExc_SystemError, "System error: invalid type encountered");
        }
    }

    static bool isIntegerObject(PyObject* obj){
        return bool(PyLong_Check(obj)) || bool(PyObject_TypeCheck(obj, &PyNumberCustomBase::Type));
    }

    static constexpr number_type min = std::numeric_limits<number_type>::min();
    static constexpr number_type max = std::numeric_limits<number_type>::max();

    static bool is_out_of_bounds_add(__int128_t value1, __int128_t value2){
      return  ((value1 > 0 && value1 > max - value2) ||
	       (value1 < 0 && value1 < min - value2));
    }

    static bool is_out_of_bounds_subtract(__int128_t value1, __int128_t value2){
      return ((value1 > 0 && value1 > max + value2) ||
	      (value1 < 0 && value1 < min +value2));
    }

    template<__int128_t(*func)(__int128_t, __int128_t, const bool check)>
    static PyObject* _baseBinaryFunc(PyObject* v1, PyObject* v2){
        static PyObject *emptyargs = PyTuple_New(0);
        const bool return_py = PyLong_Check(v1) || PyLong_Check(v2);
	
        if(!isIntegerObject(v1) || !isIntegerObject(v2)){
            static const char* const msg = "Invalid types for arguments";
            PyErr_SetString(PyExc_TypeError, msg);
            return nullptr;
        }
        const __int128_t value1 = toLongLong(v1);
        const __int128_t value2 = toLongLong(v2);
        __int128_t ret_value;
        ret_value = func(value1, value2, !return_py);
        if(PyErr_Occurred()){
            return nullptr;
        }
        if (return_py){
	    if(ret_value > NumberType<long long>::max || ret_value < NumberType<long long>::min){
	        PyErr_SetString(PyExc_ValueError, "Result out of range");
            return nullptr;
	    }
	    if (NumberType<number_type>::min == 0){
	      return PyLong_FromUnsignedLong(ret_value);
	    }
            return PyLong_FromLong(ret_value);
        } else if ( ret_value < min || ret_value > max){
            PyErr_SetString(PyExc_ValueError, "Result out of range");
            return nullptr;
        }
        PyNumberCustomObject<number_type>* ret = (PyNumberCustomObject<number_type>*) PyObject_Call((PyObject*) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
        if (!ret){
            return nullptr;
        }
        *const_cast<typename std::remove_const<number_type>::type*>(&ret->value) = ret_value;
        return (PyObject*)ret;
    }

    template<void(*func)(__int128_t&, number_type)>
    static PyObject* _baseInplaceBinaryFunc(PyObject* v1, PyObject* v2){
        if(!PyObject_TypeCheck(v1, &PyNumberCustomObject<number_type>::Type)){
            PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
            Py_RETURN_NOTIMPLEMENTED;
        }
        if(!isIntegerObject(v1) || !isIntegerObject(v2)){
            PyErr_SetString(PyExc_TypeError,  "Invalid types for arguments");
            Py_RETURN_NOTIMPLEMENTED;
        }
        __int128_t ret_value = ((PyNumberCustomObject<number_type>*)v1)->value;
        func(ret_value, toLongLong(v2));
        if(PyErr_Occurred()){
            return nullptr;
        }
        if (ret_value < min || ret_value > max){
            PyErr_SetString(PyExc_ValueError,  "Result out of bounds");
            return nullptr;
        }
        *const_cast<typename std::remove_const<number_type>::type*>(&((PyNumberCustomObject<number_type>*)v1)->value) = ret_value;
	Py_INCREF(v1);
        return v1;
    }

    template<number_type (*func)(__int128_t)>
    static PyObject* _baseUnaryFunc(PyObject* obj){
        static PyObject *emptyargs = PyTuple_New(0);
        if(!isIntegerObject(obj)){
            PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
            Py_RETURN_NOTIMPLEMENTED;
        }
        __int128_t ret_value = func(toLongLong(obj));
        if(PyErr_Occurred()){
            return nullptr;
        }

        PyNumberCustomObject<number_type>* ret = (PyNumberCustomObject<number_type>*) PyObject_Call((PyObject*) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
        if (ret){
            ret->value = ret_value;
        }
        return (PyObject*) ret;
    }

  static __int128_t add(__int128_t value1, __int128_t value2, const bool check){
        if(check && is_out_of_bounds_add(value1, value2)){
            PyErr_SetString(PyExc_ValueError, "sum of values out of range");
        }
        return value1 + value2;
    }

    static __int128_t subtract(__int128_t value1, __int128_t value2, const bool check){
        if(check && is_out_of_bounds_subtract(value1, value2)){
            PyErr_SetString(PyExc_ValueError, "difference of values out of range");
        }
       return value1 - value2;
    }

    static __int128_t multiply(__int128_t value1, __int128_t value2, const bool check){
        const number_type result = value1 * value2;
        if (check && value1 != 0 && result/value1 != value2){
           PyErr_SetString(PyExc_ValueError, "multiplication of values is out of range");
        }
        return result;
    }

    static __int128_t pow(__int128_t base, __int128_t exp){
        __int128_t result = 1;
        for (;;)
        {
            if (exp & 1)
                result *= base;
            exp >>= 1;
            if (!exp)
                break;
            base *= base;
        }
        return result;
    }

    static PyObject* power(PyObject* v1, PyObject* v2, PyObject* v3){
        static PyObject *emptyargs = PyTuple_New(0);
        PyNumberCustomObject<number_type>* ret = (PyNumberCustomObject<number_type>*) PyObject_Call((PyObject*) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
        if (!ret){
            return nullptr;
        }
        if(v3 == Py_None){
            v3 = nullptr;
        }
        if(!isIntegerObject(v1) || !isIntegerObject(v2) || (v3 && !isIntegerObject(v3))){
            static const char* const msg = "Invalid types for arguments";
            PyErr_SetString(PyExc_TypeError, msg);
            return nullptr;
        }
        const __int128_t value1 = toLongLong(v1);
        const __int128_t value2 = toLongLong(v2);
        const __int128_t value3 = v3? toLongLong(v3): 0;
        const __int128_t result = v3?
            pow(value1, value2) % value3:
            pow(value1, value2);
        if (result < min || result > max){
            static const char* const msg = "Result is out of range";
            PyErr_SetString(PyExc_ValueError, msg);
            return nullptr;
        }
        ret->value = (number_type) result;
        return  (PyObject*) ret;
    }

    static __int128_t remainder(__int128_t value1, __int128_t value2, const bool check){
        __int128_t result = value1 % value2;
        if(((value1 < 0 and value2 > 0) || (value1 > 0 && value2 < 0)) && (value1 % value2 != 0)){
	  result = value1 - (floor_div(value1, value2, false) * value2);
        }
        if(check && (result > max || result < min)){
            PyErr_SetString(PyExc_ValueError, "Result is out of range");
        }
        return result;
    }

    static PyObject* positive(PyObject* v1){
        Py_INCREF(v1);
        return v1;
    }

    static number_type absolute(__int128_t value1){
        if(value1 == min){
            PyErr_SetString(PyExc_ValueError, "Result is out of bounds");
        }
        return value1 > 0?value1:-value1;
    }

    static number_type negative(__int128_t value){
        const __int128_t result = -value;
        if (result < min || result > max){
            PyErr_SetString(PyExc_ValueError, "result is out of range");
        }
        return result;
     }

    static PyObject* divmod(PyObject* v1, PyObject* v2){
        static PyObject *emptyargs = PyTuple_New(0);
        PyNumberCustomObject<number_type>* retq = (PyNumberCustomObject<number_type>*) PyObject_Call((PyObject*) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
        PyNumberCustomObject<number_type>* retr = (PyNumberCustomObject<number_type>*) PyObject_Call((PyObject*) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
        if (!retq || !retr){
            return nullptr;
        }

        if(!isIntegerObject(v1) || !isIntegerObject(v2)){
            PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
            return nullptr;
        }
        const __int128_t value1 = toLongLong(v1);
        const __int128_t value2 = toLongLong(v2);
        const __int128_t quotient = value1/value2;
        const __int128_t remainder = value1 % value2;
        if (quotient < min || quotient > max || remainder < min || remainder> max){
            static const char* const msg = "Invalid types for arguments";
            PyErr_SetString(PyExc_ValueError, msg);
            return nullptr;
        }
        PyObject* tuple = PyTuple_New(2);
        retq->value = (number_type) quotient;
        retr->value = (number_type) remainder;
        PyTuple_SetItem(tuple, 0, (PyObject*)retq);
        PyTuple_SetItem(tuple, 1, (PyObject*)retr);
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

    static number_type invert(__int128_t value){
        return ~(number_type)value;
    }

    static __int128_t lshift(__int128_t value1, __int128_t value2, const bool){
        return ((number_type)value1) << ((number_type)value2);
    }

    static __int128_t rshift(__int128_t value1, __int128_t value2, const bool){
        return ((number_type) value1) >> ((number_type)value2);
    }

    static __int128_t and_(__int128_t value1, __int128_t value2, const bool){
        return (number_type)value1 & (number_type) value2;
    }

    static __int128_t or_(__int128_t value1, __int128_t value2, const bool){
        return ((number_type)value1) | ((number_type) value2);
    }

    static __int128_t xor_(__int128_t value1, __int128_t value2, const bool){
        return ((number_type)value1) ^ ((number_type) value2);
    }

    static PyObject* to_pyint(PyObject* value){
	    if (NumberType<number_type>::min == 0){
	      return PyLong_FromUnsignedLong(toLongLong(value));
	    }
        return PyLong_FromLong(toLongLong(value));
    }

    static PyObject* to_pyfloat(PyObject* value){
        return PyFloat_FromDouble((double)toLongLong(value));
    }

    static void inplace_add(__int128_t &value1, number_type value2){
        if (is_out_of_bounds_add(value1, value2)){
            PyErr_SetString(PyExc_ValueError, "Values out of range for in place addition");
        }
        value1 += value2;
    }

    static void inplace_subtract(__int128_t &value1, number_type value2){
        if (is_out_of_bounds_subtract(value1, value2)){
            PyErr_SetString(PyExc_ValueError, "Values out of range for in place subtraction");
        }
        value1 -= value2;
    }

    static void inplace_multiply(__int128_t &value1, number_type value2){
        const number_type orig = value1;
        value1 *= value2;
         if (value2 != 0 && value1/value2 != orig){
           PyErr_SetString(PyExc_ValueError, "multiplication of values is out of range");
        }
    }

    static void inplace_remainder(__int128_t &value1, number_type value2){
        value1 %= value2;
    }

    static void inplace_lshift(__int128_t &value1, number_type value2){
        value1 <<= value2;
    }

    static void inplace_rshift(__int128_t &value1, number_type value2){
        value1 >>= value2;
    }

    static void inplace_and(__int128_t &value1, number_type value2){
        value1 &= value2;
    }

    static void inplace_or(__int128_t &value1, number_type value2){
        value1 |= value2;
    }

    static void inplace_xor(__int128_t &value1, number_type value2){
        value1 ^= value2;
    }
    static void inplace_floor_div(__int128_t &value1, number_type value2){
        value1 /= value2;
	if(((value1 < 0 and value2 > 0) || (value1 > 0 && value2 < 0)) && (value1 % value2 != 0)){
	  value1 -= 1;
	}
    }

    static __int128_t floor_div(__int128_t value1, __int128_t value2, const bool check){
      if(((value1 < 0 and value2 > 0) || (value1 > 0 && value2 < 0)) && (value1 % value2 != 0)){
	return value1/value2 -1;
      }
        return value1/value2;
    }

    static PyObject* divide(PyObject* v1, PyObject* v2){
        if(!isIntegerObject(v1) || !isIntegerObject(v2)){
            static const char* const msg = "Invalid types for arguments";
            PyErr_SetString(PyExc_TypeError, msg);
            return nullptr;
        }
        const __int128_t value1 = toLongLong(v1);
        const __int128_t value2 = toLongLong(v2);
        return PyFloat_FromDouble((double)value1/(double)value2);
    }

    static PyObject* inplace_divide(PyObject* v1, PyObject* v2){
         if(!isIntegerObject(v1) || !isIntegerObject(v2)){
            static const char* const msg = "Invalid types for arguments";
            PyErr_SetString(PyExc_TypeError, msg);
            return nullptr;
        }
        const __int128_t value1 = toLongLong(v1);
        const __int128_t value2 = toLongLong(v2);
        return PyFloat_FromDouble((double)value1/(double)value2);
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
  _type_name<number_type>(), /*tp_name*/
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
 


template<>
const char* const _type_name<short>(){
  static const char* const name = "c_short";
  return name;
}

template<>
const char* const _type_name<int>(){
  static const char* const name = "c_int";
  return name;
}

template<>
const char* const _type_name<long>(){
  static const char* const name = "c_long";
  return name;
}

template<>
const char* const _type_name<long long>(){
  static const char* const name = "c_long_long";
  return name;
}

template<>
const char* const _type_name<unsigned char>(){
  static const char* const name = "c_unsigned_char";
  return name;
}

template<>
const char* const _type_name<unsigned short>(){
  static const char* const name = "c_unsigned_short";
  return name;
}

template<>
const char* const _type_name<unsigned int>(){
  static const char* const name = "c_unsigned_int";
  return name;
}

template<>
const char* const _type_name<unsigned long>(){
  static const char* const name = "c_unsigned_long";
  return name;
}

template<>
const char* const _type_name<unsigned long long>(){
  static const char* const name = "c_unsigned_long_long";
  return name;
}

template<typename number_type>
PyObject* PyNumberCustomObject<number_type>::repr(PyObject* o){
    PyNumberCustomObject<number_type>* obj = (PyNumberCustomObject<number_type>*)o;
    std::string name = std::string("<pyllars." ) + std::string(_type_name<number_type>()) + std::string("> value=") + std::to_string(obj->value);
    return PyString_FromString(name.c_str());
}


template<typename number_type>
PythonClassWrapper<number_type*>* PyNumberCustomObject<number_type>::alloc(PyObject *cls, PyObject *args, PyObject *kwds) {
   if(kwds && PyDict_Size(kwds) > 0){
        static const char* const msg = "Allocator does not accept keywords";
        PyErr_SetString(PyExc_TypeError, msg);
        return nullptr;
   }
   const size_t size = PyTuple_Size(args);
   if (size > 2){
        static const char* const msg = "Too many arguments to call to allocations";
        PyErr_SetString(PyExc_TypeError, msg);
        return nullptr;
   }
   number_type value = 0;
   if (size >= 1){
        PyObject* item = PyTuple_GetItem(args, 0);
        if(!item){
            static const char* const msg =  "Internal error getting tuple value";
            PyErr_SetString(PyExc_SystemError, msg);
            return nullptr;
        }
        if(! NumberType<number_type>::isIntegerObject(item)){
            PyErr_SetString(PyExc_ValueError, "Argument must be of integral type");
            return nullptr;
        }
        const __int128_t long_value =  NumberType<number_type>::toLongLong(item);
        if(long_value < NumberType<number_type>::min || long_value > NumberType<number_type>::max){
            PyErr_SetString(PyExc_ValueError, "Argument out of range");
            return nullptr;
        }
        value = (number_type) long_value;
   }
   size_t count = 1;
   if (size == 2){
        PyObject* item = PyTuple_GetItem(args, 1);
        if(!item){
            PyErr_SetString(PyExc_SystemError, "Internal error getting tuple value");
            return nullptr;
        }
        if(! NumberType<number_type>::isIntegerObject(item)){
            PyErr_SetString(PyExc_ValueError, "Argument must be of integral type");
            return nullptr;
        }
        const __int128_t long_value =  NumberType<number_type>::toLongLong(item);
        if(long_value <  NumberType<number_type>::min || long_value >  NumberType<number_type>::max){
            PyErr_SetString(PyExc_ValueError, "Argument out of range");
            return nullptr;
        }
        count = (number_type) long_value;
        if (count <= 0){
            PyErr_SetString(PyExc_ValueError, "Number of elements to allocate must be greater then 0");
            return nullptr;
        }
   }
   number_type* alloced = new number_type(value);
   return PythonClassWrapper<number_type*>::createPy2(count, &alloced, true, false, nullptr);
}

template<typename number_type>
int PyNumberCustomObject<number_type>::initialize(const char *const name, const char *const module_entry_name,
			              PyObject *module, const char *const fullname){
    PyType_Ready(&PyNumberCustomBase::Type);
    const int rc = PyType_Ready(&PyNumberCustomObject::Type);
    if(module && rc == 0){
        PyModule_AddObject(module, _type_name<ntype>(), (PyObject*) &PyNumberCustomObject::Type);
    }
    return rc;
}


template<typename number_type>
PyObject* PyNumberCustomObject<number_type>::richcompare(PyObject* a, PyObject* b, int op){
    if(!NumberType<number_type>::isIntegerObject(a) || ! NumberType<number_type>::isIntegerObject(b)){
        PyErr_SetString(PyExc_TypeError, "Invalid operands for comparison");
        return NULL;
    }
    __int128_t value1 = NumberType<number_type>::toLongLong(a);
    __int128_t value2 = NumberType<number_type>::toLongLong(b);
    switch(op){
    case Py_LT:
        if (value1 < value2){
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
    case Py_LE:
        if(value1 <= value2){
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
    case Py_EQ:
        if (value1 == value2){
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
    case Py_NE:
        if (value1 != value2){
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
    case Py_GT:
        if(value1 > value2){
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
    case Py_GE:
        if(value1 >= value2){
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
    default:
        Py_RETURN_NOTIMPLEMENTED;
    }
}

template<typename number_type>
 __pyllars_internal::PythonClassWrapper<number_type> * PyNumberCustomObject<number_type>::createPy
        (const ssize_t arraySize,
	     __pyllars_internal::ObjContainer<ntype> *const cobj,
        const bool isAllocated,
        const bool inPlace, PyObject *referencing,
        const size_t depth) {
    static PyObject *kwds = PyDict_New();
    static PyObject *emptyargs = PyTuple_New(0);
    PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);

    __pyllars_internal::PythonClassWrapper<number_type> *pyobj = (__pyllars_internal::PythonClassWrapper<number_type> *) PyObject_Call((PyObject *) &Type, emptyargs, kwds);
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
int PyNumberCustomObject<number_type>::create(PyObject* self_, PyObject* args, PyObject*kwds){
    PyNumberCustomObject* self = (PyNumberCustomObject*)self_;
    if(self){
        if(PyTuple_Size(args) == 0){
            memset(const_cast<typename std::remove_const<number_type>::type*>(&self->value), 0, sizeof(self->value));
        } else if (PyTuple_Size(args) == 1){
            PyObject* value = PyTuple_GetItem(args, 0);
            if (!NumberType<number_type>::isIntegerObject(value)){
                PyErr_SetString(PyExc_TypeError, "Argument must be an integer");
                return -1;
            }
            __int128_t longvalue = NumberType<number_type>::toLongLong(value);
            if( longvalue < (__int128_t)std::numeric_limits<number_type>::min() ||
                longvalue > (__int128_t)std::numeric_limits<number_type>::max()){
                PyErr_SetString(PyExc_ValueError, "Argument value out of range");
                return -1;
            }
            *(const_cast<typename std::remove_const<number_type>::type *>(&self->value)) = (number_type) longvalue;
        } else {
            PyErr_SetString(PyExc_TypeError, "Should only call with at most one arument");
            return -1;
        }
	self->asLongLong = [self]()->__int128_t{return (__int128_t) self->value;};
	return 0;
    }
    PyErr_SetString(PyExc_TypeError, "Recevied null self !?#");
    return -1;
}


template<typename number_type>
PyNumberCustomObject<number_type>::Initializer::Initializer(){
  pyllars_register(this);
}

template<typename number_type>
status_t PyNumberCustomObject<number_type>::Initializer::init(PyObject * const global_mod){
    static PyObject* module = PyImport_ImportModule("pyllars");
    PyType_Ready(&PyNumberCustomBase::Type);
    const int rc = PyType_Ready(&PyNumberCustomObject::Type);
    Py_INCREF(&PyNumberCustomBase::Type);
    Py_INCREF(&PyNumberCustomObject::Type);
    if(module && rc == 0){
        PyModule_AddObject(module, _type_name<number_type>(), (PyObject*) &PyNumberCustomObject::Type);
    }
}

template class PyNumberCustomObject<char>;
template class PyNumberCustomObject<short>;
template class PyNumberCustomObject<int>;
template class PyNumberCustomObject<long>;
template class PyNumberCustomObject<long long>;
template class PyNumberCustomObject<unsigned char>;
template class PyNumberCustomObject<unsigned short>;
template class PyNumberCustomObject<unsigned int>;
template class PyNumberCustomObject<unsigned long>;
template class PyNumberCustomObject<unsigned long long>;

}
