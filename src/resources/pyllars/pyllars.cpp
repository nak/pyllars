#include <stdio.h>
#include <limits>

#include "pyllars.hpp"
#include "pyllars_varargs.cpp"
#include "pyllars_templates.cpp"
#include "pyllars_pointer.cpp"
#include "pyllars_classwrapper.cpp"

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
  static PyModuleDef moduleDef;
  memset(&moduleDef, 0, sizeof(moduleDef));
  moduleDef.m_name = name;
  moduleDef.m_doc = doc;
  moduleDef.m_size = -1;

  PyObject* mod = PyModule_Create(&moduleDef);
  if(mod){
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
    Initializer::root = &_root;

    return Initializer::root->register_init(init);
}

template<typename number_type>
struct NumberType;


template<typename number_type>
static const char* const _type_name();

template<>
const char* const _type_name<char>(){
  static const char* const name = "c_char";
  return name;
}

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
struct PyNumberCustomObject222{
public:
  PyObject_HEAD
  number_type value;
  number_type * get_CObject(){
    return &value;
  }
};

struct PyNumberCustomBase{
  static PyTypeObject Type;

  std::function<long long()> asLongLong;
};

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
struct PyNumberCustomObject{
public:
    PyObject_HEAD
    typedef number_type ntype;

    static PyTypeObject Type;

    static std::string get_name(){
        return std::string(_type_name<ntype>());
    }

    static std::string get_full_name(){
      return std::string(_type_name<ntype>());
    }
    
    static std::string get_module_entry_name(){
      return std::string(_type_name<ntype>());
    }
       
    static int initialize(const char *const name, const char *const module_entry_name,
			              PyObject *module, const char *const fullname = nullptr){
        PyType_Ready(&PyNumberCustomBase::Type);
        const int rc = PyType_Ready(&PyNumberCustomObject::Type);
        if(module && rc == 0){
	        PyModule_AddObject(module, _type_name<ntype>(), (PyObject*) &PyNumberCustomObject::Type);
        }
        return rc;
    }

    static __pyllars_internal::PythonClassWrapper<number_type> *createPy
        (const ssize_t arraySize, 
	     __pyllars_internal::ObjContainer<ntype> *const cobj,
	     const bool isAllocated,
         const bool inPlace, PyObject *referencing, const size_t depth = 0) {
        static PyObject *kwds = PyDict_New();
        static PyObject *emptyargs = PyTuple_New(0);
        PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);
      
        __pyllars_internal::PythonClassWrapper<number_type> *pyobj = (__pyllars_internal::PythonClassWrapper<number_type> *) PyObject_Call((PyObject *) &Type, emptyargs, kwds);
        pyobj->_depth = 0;
        return pyobj;
    }


    void make_reference(PyObject *obj) {
        if (_referenced) { Py_DECREF(_referenced); }
        if (obj) { Py_INCREF(obj); }
        _referenced = obj;
    }

    static bool checkType(PyObject *const obj) {
        return PyObject_TypeCheck(obj, &Type);
    }
    
    static constexpr PyObject * const parent_module = nullptr;

    static PyObject* repr(PyObject* o){
        PyNumberCustomObject<number_type>* obj = (PyNumberCustomObject<number_type>*)o;
        std::string name = std::string("<pyllars." ) + std::string(_type_name<number_type>()) + std::string("> value=") + std::to_string(obj->value);
        return PyString_FromString(name.c_str());
    }

    static PyObject* create(PyTypeObject* subtype, PyObject* args, PyObject*kwds){
        PyNumberCustomObject<number_type> * self = (PyNumberCustomObject<number_type>*) subtype->tp_alloc(subtype, 0);
        if(self){
            if(PyTuple_Size(args) == 0){
	            memset(const_cast<typename std::remove_const<number_type>::type*>(&self->value), 0, sizeof(self->value));
        } else if (PyTuple_Size(args) == 1){
	        PyObject* value = PyTuple_GetItem(args, 0);
	        if (!NumberType<number_type>::isIntegerObject(value)){
	            PyErr_SetString(PyExc_TypeError, "Argument must be an integer");
	            return nullptr;
	            }
	            __int128_t longvalue = NumberType<number_type>::toLongLong(value);
	            if( longvalue < (__int128_t)std::numeric_limits<number_type>::min() ||
	                longvalue > (__int128_t)std::numeric_limits<number_type>::max()){
	                PyErr_SetString(PyExc_ValueError, "Argument value out of range");
	                return nullptr;
	            }
	            *(const_cast<typename std::remove_const<number_type>::type *>(&self->value)) = (number_type) longvalue;
            } else {
	            PyErr_SetString(PyExc_TypeError, "Should only call with at most one arument");
	            return nullptr;
            }
        }
        self->asLongLong = [self]()->__int128_t{return (__int128_t) self->value;};
        return (PyObject*) self;
    }

    PyNumberCustomObject():_referenced(nullptr),_depth(0){
    }

    number_type * get_CObject(){
        return &value;
    }

    std::function<__int128_t()> asLongLong;

    PyObject* _referenced;
    size_t _depth;
    number_type value;

    class Initializer{
    public:
        Initializer(){
            static PyObject* module = PyImport_ImportModule("pyllars");
            PyType_Ready(&PyNumberCustomBase::Type);
            const int rc = PyType_Ready(&PyNumberCustomObject::Type);
            if(module && rc == 0){
	            PyModule_AddObject(module, _type_name<number_type>(), (PyObject*) &PyNumberCustomObject::Type);
            }
        }

        static Initializer* initializer;
    };
};

template<typename number_type>
typename PyNumberCustomObject<number_type>::Initializer* PyNumberCustomObject<number_type>::Initializer::initializer = new  PyNumberCustomObject<number_type>::Initializer();

template<typename number_type>
struct NumberType{

    static PyNumberMethods *instance(){
        static PyNumberMethods obj;
        obj.nb_add = (binaryfunc) add;
        obj.nb_subtract = (binaryfunc) subtract;
        obj.nb_multiply = (binaryfunc) multiply;
        obj.nb_remainder = (binaryfunc) remainder;
    
        obj.nb_power = (ternaryfunc) power;
    
        obj.nb_positive = (unaryfunc) positive;
        obj.nb_negative = (unaryfunc) negative;
        obj.nb_divmod = (binaryfunc) divmod;
        obj.nb_absolute = (unaryfunc)absolute;

        return &obj;
    }

    static long long toLongLong(PyObject* obj){
        if(PyLong_Check(obj)){
            return PyLong_AsLong(obj);
        } else if(PyObject_TypeCheck(obj, &PyNumberCustomBase::Type)){
            return ((PyNumberCustomObject<number_type>*) obj)->asLongLong();
        } else {
            throw "System error: invalid type encountered";
        }
    }

    static bool isIntegerObject(PyObject* obj){
        return bool(PyLong_Check(obj)) || bool(PyObject_TypeCheck(obj, &PyNumberCustomBase::Type));
    }

    static constexpr number_type min = std::numeric_limits<number_type>::min();
    static constexpr number_type max = std::numeric_limits<number_type>::max();

    static bool is_out_of_bounds_add(__int128_t value1, __int128_t value2){
        return (value1 > max || value2 > max || value1 < min || value2 < min ||
           (value1 > 0 && value1 > max - value2) ||
           (value1 < 0 && value1 < min - value2));
    }

    static bool is_out_of_bounds_subtract(__int128_t value1, __int128_t value2){
        return (value1 > max || value2 > max || value1 < min || value2 < min ||
           (value1 > 0 && value1 > max + value2) ||
           (value1 < 0 && value1 < min +value2));
    }

    static PyObject* add(PyObject* v1, PyObject* v2){
        static PyObject *emptyargs = PyTuple_New(0);
        PyNumberCustomObject<number_type>* ret = (PyNumberCustomObject<number_type>*) PyObject_Call((PyObject*) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
        if (!ret){
            return nullptr;
        }
        if(!isIntegerObject(v1) || !isIntegerObject(v2)){
            PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
            return nullptr;
        }
        const __int128_t value1 = toLongLong(v1);
        const __int128_t value2 = toLongLong(v2);
        if(is_out_of_bounds_add(value1, value2)){
            PyErr_SetString(PyExc_ValueError, "sum of values out of range");
            return nullptr;
        }
        *const_cast<typename std::remove_const<number_type>::type*>(&ret->value) =
            (number_type)(value1 + value2);
        //Py_INCREF(ret);
        return (PyObject*) ret;
    }

    static PyObject* subtract(PyObject* v1,PyObject* v2){
        static PyObject *emptyargs = PyTuple_New(0);
        PyNumberCustomObject<number_type>* ret = (PyNumberCustomObject<number_type>*) PyObject_Call((PyObject*) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
        if (!ret){
            return nullptr;
        }
        if(!isIntegerObject(v1) || !isIntegerObject(v2)){
            PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
            return nullptr;
        }
        const __int128_t value1 = toLongLong(v1);
        const __int128_t value2 = toLongLong(v2);
        if(is_out_of_bounds_subtract(value1, value2)){
            PyErr_SetString(PyExc_ValueError, "difference of values out of range");
            return nullptr;
        }
       ret->value = (number_type)(value1 - value2);
       //Py_INCREF(ret);
        return (PyObject*) ret;
    }

    static PyObject* multiply(PyObject* v1,PyObject* v2){
         static PyObject *emptyargs = PyTuple_New(0);
        PyNumberCustomObject<number_type>* ret = (PyNumberCustomObject<number_type>*) PyObject_Call((PyObject*) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
        if (!ret){
            return nullptr;
        }
        if(!isIntegerObject(v1) || !isIntegerObject(v2)){
            PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
            return nullptr;
        }
        const number_type value1 = toLongLong(v1) ;
        const number_type value2 = toLongLong(v2);
        const number_type result = value1 * value2;
        if (value1 != 0 && result/value1 != value2){
            PyErr_SetString(PyExc_ValueError, "multiplication of values is out of range");
            return nullptr;
        }
        ret->value = result;
        //Py_INCREF(ret);
        return (PyObject*) ret;
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
            PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
            return nullptr;
        }
        const __int128_t value1 = toLongLong(v1);
        const __int128_t value2 = toLongLong(v2);
        const __int128_t value3 = v3? toLongLong(v3): 0;
        const __int128_t result = v3?
            pow(value1, value2) % value3:
            pow(value1, value2);
        if (result < min || result > max){
            PyErr_SetString(PyExc_ValueError, "result is out of range");
            return nullptr;
        }
        ret->value = (number_type) result;
        //Py_INCREF(ret);
        return  (PyObject*) ret;
    }

    static PyObject* remainder(PyObject* v1, PyObject* v2){
        static PyObject *emptyargs = PyTuple_New(0);
        PyNumberCustomObject<number_type>* ret = (PyNumberCustomObject<number_type>*) PyObject_Call((PyObject*) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
        if (!ret){
            return nullptr;
        }

        if(!isIntegerObject(v1) || !isIntegerObject(v2)){
            PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
            return nullptr;
        }
        const __int128_t value1 = toLongLong(v1);
        const __int128_t value2 = toLongLong(v2);
        const __int128_t result = value1 % value2;
        if(result > max || result < min){
            PyErr_SetString(PyExc_ValueError, "Result is out of range");
            return nullptr;
        }
        ret->value = (number_type)result;
        //Py_INCREF(ret);
        return  (PyObject*) ret;
    }


     static PyObject* positive(PyObject* v1){
        return v1;
     }

     static PyObject* absolute(PyObject* v1){
        static PyObject *emptyargs = PyTuple_New(0);
        PyNumberCustomObject<number_type>* ret = (PyNumberCustomObject<number_type>*) PyObject_Call((PyObject*) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
        if (!ret){
            return nullptr;
        }

        if(!isIntegerObject(v1)){
            PyErr_SetString(PyExc_TypeError, "Invalid types for arguments");
            return nullptr;
        }
        __int128_t result = toLongLong(v1);
        if(result == min){
            PyErr_SetString(PyExc_ValueError, "Result is out of bounds");
            return nullptr;
        }
        result = result > 0?result:-result;
        ret->value = (number_type)result;
        //Py_INCREF(ret);
        return (PyObject*) ret;
     }

     static PyObject* negative(PyObject* v1){
        static PyObject *emptyargs = PyTuple_New(0);
        PyNumberCustomObject<number_type>* ret = (PyNumberCustomObject<number_type>*) PyObject_Call((PyObject*) &PyNumberCustomObject<number_type>::Type, emptyargs, nullptr);
        if (!ret){
            return nullptr;
        }
        __int128_t result = -toLongLong(v1);
        if (result < min || result > max){
            PyErr_SetString(PyExc_ValueError, "result is out of range");
            return nullptr;
        }
        ret->value = (number_type)result;
        //Py_INCREF(ret);
        return  (PyObject*) ret;
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
            PyErr_SetString(PyExc_ValueError, "Results are out of range");
            return nullptr;
        }
        PyObject* tuple = PyTuple_New(2);
        retq->value = (number_type) quotient;
        retr->value = (number_type) remainder;
        PyTuple_SetItem(tuple, 0, (PyObject*)retq);
        PyTuple_SetItem(tuple, 1, (PyObject*)retr);
        return tuple;
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
  nullptr,                       /* tp_richcompare */
  0,                               /* tp_weaklistoffset */
  nullptr,                       /* tp_iter */
  nullptr,                       /* tp_iternext */
  nullptr,             /* tp_methods */
  nullptr,             /* tp_members */
  nullptr,                         /* tp_getset */
  &PyNumberCustomBase::Type,                         /* tp_base */
  nullptr,                         /* tp_dict */
  nullptr,                         /* tp_descr_get */
  nullptr,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  nullptr,  /* tp_init */
  nullptr,                         /* tp_alloc */
  PyNumberCustomObject::create,             /* tp_new */
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


namespace __pyllars_internal{

  template<>
  class PythonClassWrapper<short>: public PyNumberCustomObject<short>{
  public:
    

  };

  template<>
  class PythonClassWrapper<char>: public PyNumberCustomObject<char>{
  };


  template<>
  class PythonClassWrapper<int>: public PyNumberCustomObject<int>{
  };

  template<>
  class PythonClassWrapper<long>: public PyNumberCustomObject<long>{
  };

  template<>
  class PythonClassWrapper<long long>: public PyNumberCustomObject<long long>{
  };

  template<>
  class PythonClassWrapper<unsigned char>: public PyNumberCustomObject<unsigned char>{
  };

  template<>
  class PythonClassWrapper<unsigned short>: public PyNumberCustomObject<unsigned short>{
  };

  template<>
  class PythonClassWrapper<unsigned int>: public PyNumberCustomObject<unsigned int>{
  };

  template<>
  class PythonClassWrapper<unsigned long>: public PyNumberCustomObject<unsigned long>{
  };

  template<>
  class PythonClassWrapper<unsigned long long>: public PyNumberCustomObject<unsigned long long>{
  };

  template
  class PythonClassWrapper<short*>;
}
