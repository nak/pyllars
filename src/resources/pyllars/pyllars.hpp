/**
 * holds top-level namespace fo Pyllars and common definitions
 */
#ifndef PYLLARS
#define PYLLARS

#include <vector>
#include <cstddef>
#include <Python.h>

#include "pyllars_classwrapper.hpp"
typedef int status_t;

namespace pyllars{

    /**
     * class for registering a hierarchy of initializers to execute on load of library
     */
    class Initializer{
    public:

        Initializer():_initializers(nullptr){
        }

        /**
         * Call all initializers, passing in the global pyllars module
         */
        virtual status_t init(PyObject* const global_module){
            int status = 0;
            if(!_initializers) return 0;
            for (auto it = _initializers->begin(); it != _initializers->end(); ++ it){
                status |= (*it)->init(global_module);
            }
            _initializers->clear();
            return status;
        }

        virtual status_t init_last(PyObject* const global_module){
            int status = 0;
            if(!_initializers_last) return 0;
            for (auto it = _initializers_last->begin(); it != _initializers_last->end(); ++ it){
                status |= (*it)->init(global_module);
            }
            _initializers_last->clear();
            return status;
        }

        int register_init( Initializer* const init){
	        if(!_initializers){
                // allocate here as this may be called before main
                // and do not want to depend on static initailization order of files which is
                // unpredictable in C++
	            _initializers = new std::vector<Initializer*>();
	        }
            _initializers->push_back(init);
            return 0;
        }

        int register_init_last( Initializer* const init){
	        if(!_initializers_last){
                // allocate here as this may be called before main
                // and do not want to depend on static initailization order of files which is
                // unpredictable in C++
	            _initializers_last = new std::vector<Initializer*>();
	        }
            _initializers_last->push_back(init);
            return 0;
        }

        // ths root (top-level) initializer
        static Initializer *root;

    private:

        std::vector<Initializer*> *_initializers;
        std::vector<Initializer*> *_initializers_last;

    };

    int pyllars_register( Initializer* const init);

    int init(PyObject* global_mod);

}  // namespace pyllars

namespace __pyllars_internal{


template<typename number_type>
static const char* const _type_name();


 template<>
 const char* const _type_name<char>(){
  static const char* const name = "c_char";
  return name;
}

template<typename number_type>
struct NumberType;

struct PyNumberCustomBase{
  static PyTypeObject Type;

  std::function<long long()> asLongLong;
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

    static PythonClassWrapper<number_type*>* alloc(PyObject *cls, PyObject *args, PyObject *kwds) {
       if(kwds && PyDict_Size(kwds) > 0){
            PyErr_SetString(PyExc_TypeError, "Allocator does not accept keywords");
            return nullptr;
       }
       const size_t size = PyTuple_Size(args);
       if (size > 2){
            PyErr_SetString(PyExc_TypeError, "Too many arguments to call to allocations");
            return nullptr;
       }
       number_type value = 0;
       if (size >= 1){
            PyObject* item = PyTuple_GetItem(args, 0);
            if(!item){
                PyErr_SetString(PyExc_SystemError, "Internal error getting tuple value");
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


     static PyMethodDef _methods[];


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
}

#if PY_MAJOR_VERSION == 3
PyObject*
#else
PyMODINIT_FUNC
int
#endif
PyllarsInit(const char* const name);

#endif
//PYLLARS
