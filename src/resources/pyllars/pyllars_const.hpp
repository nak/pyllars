/**
 * holds top-level namespace fo Pyllars and common definitions
 */
#ifndef PYLLARS_CONST
#define PYLLARS_CONST

#include <vector>
#include <cstddef>
#include <Python.h>

#include "pyllars.hpp"
#include "pyllars_classwrapper.hpp"
typedef int status_t;


namespace __pyllars_internal{


template<typename number_type>
static const char* const _type_name();



template<typename number_type>
struct ConstNumberType;

template<typename number_type>
struct ConstFloatingPointType;

struct PyConstNumberCustomBase{
  static PyTypeObject Type;

  std::function<long long()> asLongLong;
};

template<typename number_type>
struct PyConstNumberCustomObject{
public:
    PyObject_HEAD
    typedef number_type ntype;
    typedef PythonClassWrapper<number_type const,   void> ConstWrapper;
    typedef PythonClassWrapper<typename std::remove_const<number_type>::type> NonConstWrapper;
    typedef PythonClassWrapper<typename std::remove_reference<number_type>::type> NoRefWrapper;

    static PyTypeObject Type;

    inline static std::string get_name(){
        return std::string(_type_name<ntype>());
    }

    inline static std::string get_full_name(){
      return std::string(_type_name<ntype>());
    }
    
    inline static std::string get_module_entry_name(){
      return std::string(_type_name<ntype>());
    }

    static PythonClassWrapper<number_type*>* alloc(PyObject *cls, PyObject *args, PyObject *kwds);
       
    static int initialize(const char *const name, const char *const module_entry_name,
			              PyObject *module, const char *const fullname = nullptr);

    static PyObject* richcompare(PyObject* a, PyObject* b, int op);

    static __pyllars_internal::PythonClassWrapper<number_type> *createPy
        (const ssize_t arraySize, 
	     __pyllars_internal::ObjContainer<ntype> *const cobj,
	     const bool isAllocated,
         const bool inPlace, PyObject *referencing, const size_t depth = 0);


    void make_reference(PyObject *obj);

    inline static bool checkType(PyObject *const obj) {
        return PyObject_TypeCheck(obj, &Type);
    }
    
    static constexpr PyObject * const parent_module = nullptr;

    static PyObject* repr(PyObject* o);

    static int create(PyObject* subtype, PyObject* args, PyObject*kwds);

    PyConstNumberCustomObject():_referenced(nullptr),_depth(0), value(0){
    }

    template<typename t=number_type>
    inline const t* get_CObject(){
        return &value;
    }


     static PyMethodDef _methods[];

    std::function<__int128_t()> asLongLong;

    PyObject* _referenced;
    size_t _depth;
    number_type value;

    class Initializer: public pyllars::Initializer{
    public:
            Initializer();

            status_t init(PyObject* const global_module);

            static Initializer* initializer;
    };
};



  template<>
  struct PythonClassWrapper<const char>: public PyConstNumberCustomObject<const char>{
  };

  template<>
  struct PythonClassWrapper<const short>: public PyConstNumberCustomObject<const short>{
  };

  template<>
  struct PythonClassWrapper<const int>: public PyConstNumberCustomObject<const int>{
  };

  template<>
  struct PythonClassWrapper<const long>: public PyConstNumberCustomObject<const long>{
  };

  template<>
  struct PythonClassWrapper<const long long>: public PyConstNumberCustomObject<const long long>{
  };

  template<>
  struct PythonClassWrapper<const unsigned char>: public PyConstNumberCustomObject<const unsigned char>{
  };

  template<>
  struct PythonClassWrapper<const unsigned short>: public PyConstNumberCustomObject<const unsigned short>{
  };

  template<>
  struct PythonClassWrapper<const unsigned int>: public PyConstNumberCustomObject<const unsigned int>{
  };

  template<>
  struct PythonClassWrapper<const unsigned long>: public PyConstNumberCustomObject<const unsigned long>{
  };

  template<>
  struct PythonClassWrapper<const unsigned long long>: public PyConstNumberCustomObject<const unsigned long long>{
  };




struct PyConstFloatingPtCustomBase{
  static PyTypeObject Type;

  std::function<double()> asDouble;
};

template<typename number_type>
struct PyConstFloatingPtCustomObject{
public:
    PyObject_HEAD
    typedef number_type ntype;
    typedef PythonClassWrapper<number_type const,   void> ConstWrapper;
    typedef PythonClassWrapper<typename std::remove_const<number_type>::type> NonConstWrapper;
    typedef PythonClassWrapper<typename std::remove_reference<number_type>::type> NoRefWrapper;

    static PyTypeObject Type;

    inline static std::string get_name(){
        return std::string(_type_name<ntype>());
    }

    inline static std::string get_full_name(){
      return std::string(_type_name<ntype>());
    }

    inline static std::string get_module_entry_name(){
      return std::string(_type_name<ntype>());
    }

    static PythonClassWrapper<number_type*>* alloc(PyObject *cls, PyObject *args, PyObject *kwds);

    static int initialize(const char *const name, const char *const module_entry_name,
			              PyObject *module, const char *const fullname = nullptr);

    static PyObject* richcompare(PyObject* a, PyObject* b, int op);

    static __pyllars_internal::PythonClassWrapper<number_type> *createPy
        (const ssize_t arraySize,
	     __pyllars_internal::ObjContainer<ntype> *const cobj,
	     const bool isAllocated,
         const bool inPlace, PyObject *referencing, const size_t depth = 0);


    void make_reference(PyObject *obj);

    inline static bool checkType(PyObject *const obj) {
        return PyObject_TypeCheck(obj, &Type);
    }

    static constexpr PyObject * const parent_module = nullptr;

    static PyObject* repr(PyObject* o);

    static int create(PyObject* subtype, PyObject* args, PyObject*kwds);

    PyConstFloatingPtCustomObject():_referenced(nullptr),_depth(0), value(0.0){
    }

    template<typename t=number_type>
    inline const t* get_CObject(){
        return &value;
    }


     static PyMethodDef _methods[];

    std::function<__int128_t()> asDouble;

    PyObject* _referenced;
    size_t _depth;
    number_type value;

    class Initializer: public pyllars::Initializer{
    public:
            Initializer();

            status_t init(PyObject* const global_module);

            static Initializer* initializer;
    };
};


  template<>
  struct PythonClassWrapper<const float>;

  template<>
  struct PythonClassWrapper<const double>;

  template<>
  struct PythonClassWrapper<const double>: public PyConstFloatingPtCustomObject<const double>{
  };


  template<>
  struct PythonClassWrapper<const float>: public PyConstFloatingPtCustomObject<const float>{
  };

 template<>
 const char* const _type_name<const char>();


 template<>
 const char* const _type_name<const short>();

 template<>
 const char* const _type_name<const int>();

 template<>
 const char* const _type_name<const long>();


 template<>
 const char* const _type_name<const long long>();


 template<>
 const char* const _type_name<const unsigned char>();

 template<>
 const char* const _type_name<const unsigned int>();

 template<>
 const char* const _type_name<const unsigned short>();

 template<>
 const char* const _type_name<const unsigned long>();


 template<>
 const char* const _type_name<const unsigned long long>();

 template<>
 const char* const _type_name<const float>();

 template<>
 const char* const _type_name<const double>();

}
#endif
//PYLLARS
