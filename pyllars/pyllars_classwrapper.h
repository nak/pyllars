//  PREDECLARATIONS
#include "pyllars_classmethodsemantics.h"
#include "pyllars_constmethodcallsemantics.h"
#include "pyllars_methodcallsemantics.h"
#include "pyllars_utils.h"
#include "pyllars_defns.h"

#ifndef __PYLLARS__INTERNAL__CLASS_WRAPPER_H
#define __PYLLARS__INTERNAL__CLASS_WRAPPER_H

#include <Python.h>
#include <structmember.h>
#include <limits.h>
#include <limits>
#include <vector>
#include <map>
#include <functional>

#include "pyllars_utils.h"
#include "pyllars_defns.h"

namespace __pyllars_internal{

  namespace{
    //basic constants (names and such)
    extern const char address_name[] = "PYL_addr";
    extern const char alloc_name[] = "PYL_alloc";
  }


  template<typename T>
    class ConstructorContainerTmpl{
  public:
    typedef T* (*constructor)( const char* const kwlist[], PyObject *args, PyObject *kwds);
    ConstructorContainerTmpl( const char* const kwlist[],
                              constructor c):_kwlist(kwlist),
      _constructor(c){
    }

      T* operator()(PyObject* args, PyObject*kwds){
        return _constructor(_kwlist, args, kwds);
      }

  private:
      const char* const * const _kwlist;
      const constructor _constructor;
  };

  
  template< typename T>
  class Setter{
    static void set_at(T const to, const T& from, const size_t index ){
      throw "Invalid call to dereference non-pointer type";
    }
  };

  template <typename T>
  class Setter<T*>{
  public:
    static void set_at( T* const to,  const T& from, const size_t index){
      to[index] = from;
    }
  };

  ///////////
    // Helper conversion functions
    //////////

    /**
     * Define conversion helper class, which allows easier mechanism
     * for necessary specializations
     **/
    template< typename T, bool is_complete,  const ssize_t max, typename E = void>
      class PyObjectConversionHelper;

    /**
     * specialize for non-copiable types
     **/
    template< typename T, bool is_complete, const ssize_t max>
      class PyObjectConversionHelper<T, is_complete, max, typename  std::enable_if<!std::is_array<T>::value && !std::is_copy_constructible<T>::value >::type >{
    public:


      static PyObject* toPyObject( T & var, const bool asArgument ){
        //must pass in keyword to inform Python it is ok to create a Python C Wrapper object
        //with a null content, which will be set here
        PyObject* kw = PyDict_New();
        PyDict_SetItemString(kw, "__internal_allow_null", PyBool_FromLong(asArgument));
        //create the object of the desired type and do some checks
        PyObject* emptyTuple = PyTuple_New(0);
        PyObject* pyobj = PyObject_Call(  (PyObject*)&PythonClassWrapper<T, is_complete>::Type, emptyTuple, kw);
        Py_DECREF(kw);
        Py_DECREF(emptyTuple);
        if ( !pyobj || !PyObject_TypeCheck(pyobj,(&PythonClassWrapper<T, is_complete>::Type))){
          PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
          goto onerror;
        }
        //here is where the pointer content is set:
        reinterpret_cast<PythonClassWrapper<T, is_complete>*>(pyobj)->set_contents(&var);
        return pyobj;
      onerror:
        return Py_None;
      }

    };


    /**
     * specialize for non-trivial copiable types (non-pointer too)
     **/
    template<typename T, bool is_complete, const ssize_t max>
      class PyObjectConversionHelper<T, is_complete, max, typename  std::enable_if< std::is_copy_constructible<T>::value && !std::is_integral<T>::value && !std::is_floating_point<T>::value &&
      !std::is_pointer<T>::value >::type >{
    public:
      typedef typename std::remove_reference<T>::type T_NoRef;

      static PyObject* toPyObject(  T & var, const bool asArgument ){
        PyObject* kw = PyDict_New();
        PyDict_SetItemString(kw, "__internal_allow_null", PyBool_FromLong(asArgument));
        //create the object of the desired type and do some checks
        PyObject* emptyTuple = PyTuple_New(0);
        PyObject* pyobj = PyObject_Call(  (PyObject*)&PythonClassWrapper<T, is_complete>::Type, emptyTuple, kw);
        Py_DECREF(kw);
        Py_DECREF(emptyTuple);
        if ( !pyobj || !PyObject_TypeCheck(pyobj, (&PythonClassWrapper<T, is_complete>::Type))){
          PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
          goto onerror;
        }

        reinterpret_cast<PythonClassWrapper<T, is_complete>*>(pyobj)->set_contents( asArgument?&var:new T_NoRef(var));
        return pyobj;
      onerror:
        PyErr_Print();
        return Py_None;

      }
    };


    /**
     * specialize for array types (non-pointer too and non-const-char too)
     **/
    template<typename T, bool is_complete, const size_t size, const ssize_t max>
      class PyObjectConversionHelper<T[size], is_complete, max,void>{
    public:
      typedef typename std::remove_reference<T>::type T_NoRef;
      typedef T T_array[size];
      static PyObject* toPyObject(  T_array& var, const bool asArgument ){
        PyObject* kw = PyDict_New();
        PyDict_SetItemString(kw, "__internal_allow_null", PyBool_FromLong(asArgument));
        //create the object of the desired type and do some checks
        PyObject* emptyTuple = PyTuple_New(0);
        PyObject* pyobj = PyObject_Call(  (PyObject*)&PythonClassWrapper<T[size], is_complete>::Type, emptyTuple, kw);
        Py_DECREF(kw);
        Py_DECREF(emptyTuple);

        if ( !pyobj || !PyObject_TypeCheck(pyobj, (&PythonClassWrapper<T[size], is_complete>::Type))){
          PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
          goto onerror;
        }
        {
          T_array * var_ = new T_array[1];
          for( size_t i = 0; i < size; ++i){
            (*var_)[i] = var[i];
          }
          reinterpret_cast<PythonClassWrapper<T[size], is_complete>*>(pyobj)->set_contents( (void**)(asArgument?&var:var_));
        }
        return pyobj;
      onerror:
        PyErr_Print();
        return Py_None;

      }
    };

    /**
     * specialize for c string
     **/
    template<bool is_complete, const size_t size, const ssize_t max>
      class PyObjectConversionHelper<const char[size], is_complete, max,void>{
    public:
      typedef const char T;
      typedef typename std::remove_reference<T>::type T_NoRef;
      typedef T T_array[size];

      static PyObject* toPyObject(  T_array& var, const bool asArgument ){
        if (!asArgument){
          return PyString_FromStringAndSize(var, var[size-1]==0? strlen(var):size);
        }
        PyObject* kw = PyDict_New();
        PyDict_SetItemString(kw, "__internal_allow_null", PyBool_FromLong(asArgument));
        //create the object of the desired type and do some checks
        PyObject* emptyTuple = PyTuple_New(0);
        PyObject* pyobj = PyObject_Call(  (PyObject*)&PythonClassWrapper<T[size], is_complete>::Type, emptyTuple, kw);
        Py_DECREF(kw);
        Py_DECREF(emptyTuple);

        if ( !pyobj || !PyObject_TypeCheck(pyobj, (&PythonClassWrapper<T[size], is_complete>::Type))){
          PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
          goto onerror;
        }
        {
          T_array * var_ = new T_array[1];
          for( size_t i = 0; i < size; ++i){
            (*var_)[i] = var[i];
          }
          reinterpret_cast<PythonClassWrapper<T[size], is_complete>*>(pyobj)->set_contents( (void**)(asArgument?&var:var_));
        }
        return pyobj;
      onerror:
        PyErr_Print();
        return Py_None;

      }
    };
    /**
     * specialize for integer types
     **/
    template<typename T, bool is_complete, const ssize_t max>
      class PyObjectConversionHelper<T, is_complete, max, typename  std::enable_if< std::is_integral<T>::value >::type >{
    public:
      static PyObject* toPyObject( const T & var, const bool asArgument ){
        (void)asArgument;
        return PyInt_FromLong( var);
      }
    };

    /**
     * specialize for floating point types
     **/
    template<typename T, bool is_complete, const ssize_t max>
      class PyObjectConversionHelper<T, is_complete, max,  typename  std::enable_if< std::is_floating_point<T>::value >::type >{
    public:
      static PyObject* toPyObject( const T & var, const bool asArgument ){
        (void)asArgument;
        return PyFloat_FromDouble( var);
      }
    };

    /**
     * specialize for cosnt integer reference types
     **/
    template<typename T, bool is_complete, const ssize_t max>
      class PyObjectConversionHelper<T, is_complete, max, typename  std::enable_if< std::is_const<T>::value && std::is_reference<T>::value && std::is_integral<typename std::remove_reference<T>::type>::value >::type >{
    public:
      static PyObject* toPyObject( const T & var, const bool asArgument ){
        if(!asArgument){
          PyErr_SetString( PyExc_RuntimeError, "cannot create const reference variable from basic type");
          return Py_None;
        }
        PyObject* args = PyTuple_New(1);
        PyTuple_SET_ITEM(args, 0, PyLong_FromLong(var));
        PyObject* retval =  PyObject_CallObject(&PythonClassWrapper<T, is_complete>::Type, args);
        Py_DECREF( PyTuple_GetItem(args,0));
        Py_DECREF( args );
        return retval;
      }
    };

    /**
     * specialize for const non-basic reference types
     **/
    template<typename T, bool is_complete, const ssize_t max>
      class PyObjectConversionHelper<T, is_complete, max, typename  std::enable_if< std::is_const<T>::value && std::is_reference<T>::value && !std::is_integral<typename std::remove_reference<T>::type>::value && std::is_floating_point<typename std::remove_reference<T>::type>::value >::type >{
    public:
      static PyObject* toPyObject( const T & var, const bool asArgument ){
        if(!asArgument){
          PyErr_SetString( PyExc_RuntimeError, "cannot create const reference variable from basic type");
          return Py_None;
        }
        PyObject* args = PyTuple_New(1);
        PyTuple_SET_ITEM(args, 0, toPyObject(var));
        PyObject* retval = PyObject_CallObject(&PythonClassWrapper<T, is_complete>::Type, args);
        Py_DECREF( PyTuple_GetItem(args,0));
        Py_DECREF( args );
        return retval;
      }
    };

    /**
     * specialize for const floating point reference types
     **/
    template<typename T, bool is_complete, const ssize_t max>
      class PyObjectConversionHelper<T, is_complete, max, typename  std::enable_if< std::is_const<T>::value && std::is_reference<T>::value &&
      !std::is_floating_point<typename std::remove_reference<T>::type>::value >::type >{
    public:
      static PyObject* toPyObject( const T & var, const bool asArgument ){
        if(!asArgument){
          PyErr_SetString( PyExc_RuntimeError, "cannot create const reference variable from basic type");
          return Py_None;
        }
        PyObject* args = PyTuple_New(1);
        PyTuple_SET_ITEM(args, 0, PyFloat_FromDouble(var));
        PyObject* retval = PyObject_CallObject(&PythonClassWrapper<T, is_complete>::Type, args);
        Py_DECREF( PyTuple_GetItem(args,0));
        Py_DECREF( args );
        return retval;
      }
    };

    /**
     * Specialized for non-const pointers:
     **/
    template< typename T,  bool is_complete, const ssize_t max >
      class PyObjectConversionHelper<T*, is_complete, max, void/*typename  std::enable_if< !is_function_ptr<typename std::remove_pointer<T>::type >::value >::type*/>{
    public:
      static PyObject* toPyObject( T*  const& var, const bool asArgument ){
        (void)asArgument;
        PyObject* pyobj =nullptr;

        typedef PythonClassWrapper<T*, is_complete, max> PtrWrapper;
        if (!PtrWrapper::Type.tp_name){
          PtrWrapper::initialize((PythonClassWrapper<T, is_complete>::get_name()+'*').c_str(),
                                 (PythonClassWrapper<T, is_complete>::get_name()+"_ptr").c_str(),
                                 PythonClassWrapper<T, is_complete>::parent_module,
                                 (std::string(PythonClassWrapper<T, is_complete>::Type.tp_name)+'*').c_str());
          

          if (PtrWrapper::parent_module){
            PyModule_AddObject( PtrWrapper::parent_module,
                                PtrWrapper::Type.tp_name,
                                (PyObject*) &PtrWrapper::Type );
          }
        }
        {
          PyObject* emptyTuple = PyTuple_New(0);
          pyobj = PyObject_CallObject( (PyObject*)&PtrWrapper::Type, emptyTuple);
          Py_DECREF(emptyTuple);
        }
        if ( !pyobj || !PyObject_TypeCheck(pyobj, &PtrWrapper::Type)){
          PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
          goto onerror;
        }
        reinterpret_cast<PtrWrapper*>(pyobj)->set_contents ( (void**)var);
        return pyobj;

      onerror:
        return Py_None;
      }
    };

    /**
     * Specialized for  pointers:
     **/
    template< typename T,  bool is_complete, const ssize_t max >
      class PyObjectConversionHelper<T* const, is_complete, max, void/*typename  std::enable_if< !is_function_ptr<typename std::remove_pointer<T>::type >::value>::type*/>{
    public:
      static PyObject* toPyObject( T* const& var, const bool asArgument ){
        (void)asArgument;
        PyObject* pyobj =nullptr;

        typedef PythonClassWrapper<T*, is_complete, max> PtrWrapper;
        if (!PtrWrapper::Type.tp_name){
          PtrWrapper::initialize((PythonClassWrapper<T, is_complete>::get_name()+'*').c_str(),
                                 (PythonClassWrapper<T, is_complete>::get_name()+"_ptr").c_str(),
                                 PythonClassWrapper<T, is_complete>::parent_module,
                                 (std::string(PythonClassWrapper<T, is_complete>::Type.tp_name)+'*').c_str());
        }
        if (PtrWrapper::parent_module){
          PyModule_AddObject( PtrWrapper::parent_module,
                              PtrWrapper::Type.tp_name,
                              (PyObject*) &PtrWrapper::Type );
        }

        {
          PyObject* emptyTuple = PyTuple_New(0);
          pyobj = PyObject_CallObject( (PyObject*)&PtrWrapper::Type, emptyTuple);
          Py_DECREF(emptyTuple);
        }
        if ( !pyobj || !PyObject_TypeCheck(pyobj, &PtrWrapper::Type)){
          PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
          goto onerror;
        }
        reinterpret_cast<PtrWrapper*>(pyobj)->set_contents ( (void**)var);
        return pyobj;

      onerror:
        return Py_None;
      }
    };

    /**
     *  Specialized for pointer-to-pointer-to-function:
     *
     template< typename T, bool is_complete, const ssize_t max >
     class PyObjectConversionHelper<T*,  is_complete, max, typename  std::enable_if< is_function_ptr<typename std::remove_pointer<T>::type >::value >::type>{
     public:
     static PyObject* toPyObject( const T & var, const bool asArgument ){
     (void)asArgument;
     //typedef typename std::remove_pointer<T>::type T_base;
     PyObject* pyobj =nullptr;
     if( std::is_pointer<T>::value ){
     typedef T T_base;
     if (!PythonClassWrapper<T*, is_complete, max>::Type.tp_name){
     PythonClassWrapper<T*, is_complete, max>::initialize((PythonClassWrapper<T_base, is_complete>::get_name()).c_str(), PythonClassWrapper<T_base, is_complete>::parent_module,
     PythonClassWrapper<T, is_complete>::Type.tp_name);
     }


     }


     typedef PythonClassWrapper<T*, is_complete, max> PtrWrapper;
     {
     PyObject* emptyTuple = PyTuple_New(0);
     pyobj = PyObject_CallObject( (PyObject*)&PtrWrapper::Type, emptyTuple);
     Py_DECREF(emptyTuple);
     }
     if ( !pyobj || !PyObject_TypeCheck(pyobj, &PtrWrapper::Type)){
     PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
     goto onerror;
     }
     reinterpret_cast<PtrWrapper*>(pyobj)-> set_contents( (void**)var);
     return pyobj;

     onerror:
     return Py_None;
     }
     };*/

    /**
     * Specialized for char*:
     **/
    template< const ssize_t max>
      class PyObjectConversionHelper<const char*, true, max, void >{
    public:
      static PyObject* toPyObject( const char* const & var, const bool asArgument ){
        (void)asArgument;
        if (!var){
          throw "NULL CHAR* encountered";
        }
        return PyString_FromString(var);

      }
    };

    template<const ssize_t max>
      class PyObjectConversionHelper< char*,  true, max, void >{
    public:
      static PyObject* toPyObject( char* const & var, const bool asArgument ){
        (void)asArgument;
        if (!var){
          throw "NULL CHAR* encountered";
        }
        return PyString_FromString(var);

      }

    };

    template<const ssize_t max>
      class PyObjectConversionHelper< const char* const,  true, max, void >{
    public:
      static PyObject* toPyObject( const char* const & var, const bool asArgument ){
        (void)asArgument;
        if (!var){
          throw "NULL CHAR* encountered";
        }
        return PyString_FromString(var);

      }

    };



    /**
     * convert C Object to python object
     * @param var: value to convert
     * @param asArgument: whether to be used as argument or not (can determine if copy is made or reference semantics used)
     **/
    template< typename T, bool is_complete, const ssize_t max = -1, typename E = void>
      PyObject* toPyObject(  T & var, const bool asArgument ){
      return PyObjectConversionHelper<T, is_complete, max>::toPyObject(var, asArgument);
    }

    template< typename T, bool is_complete,  const ssize_t max = -1, typename E = void>
      PyObject* toPyObject(  const T & var, const bool asArgument ){
      return PyObjectConversionHelper<const T, is_complete, max>::toPyObject(var, asArgument);
    }


    static PyMethodDef emptyMethods[] = {{nullptr, nullptr, 0, nullptr}};

    template< typename T, bool is_complete, typename E=void>
      class InitHelper{
      public:
      static int init(PythonClassWrapper<T, is_complete> *self, PyObject* args, PyObject*kwds);
    };

    /**
     * Specialization for integers
     **/
    template<typename T, bool is_complete>
      class InitHelper<T, is_complete, typename std::enable_if<std::is_integral<T>::value>::type >{
    public:
      static int init(PythonClassWrapper<T, is_complete> *self, PyObject* args, PyObject*kwds){
        if (!self) {
          return -1;
        }
        static const char* kwlist[] = {"value",nullptr};
        if (std::is_unsigned<T>::value){
          unsigned long long value;
          if (! PyArg_ParseTupleAndKeywords(args, kwds, "L", (char**)kwlist,
                                            &value))
            return -1;
          if (value < (unsigned long long)std::numeric_limits<T>::min() || value >(unsigned long long)std::numeric_limits<T>::max() ) {
            PyErr_SetString(PyExc_OverflowError,"Integer value out of range of int");
            return -1;
          }
          self->_CObject = new T((T)value);
        } else {
          long long value;
          fprintf(stderr, "%ld", PyLong_AsLong(PyTuple_GetItem(args,0)));
          if (! PyArg_ParseTupleAndKeywords(args, kwds, "K", (char**)kwlist,
                                            &value))
            return -1;
          if (value < (long long)std::numeric_limits<T>::min() ||  value > (long long)std::numeric_limits<T>::max() ) {
            PyErr_SetString(PyExc_OverflowError,"Integer value out of range of int");
            return -1;
          }
          self->_CObject = new T((T)value);
        }
        return 0;
      }
    };

    /**
     * Specialization for floating point
     **/
    template<typename T, bool is_complete>
      class InitHelper<T, is_complete, typename std::enable_if<std::is_floating_point<T>::value >::type>{
    public:
      static int init(PythonClassWrapper<T, is_complete> *self, PyObject* args, PyObject*kwds){
        static const char* kwlist[] = {"value",nullptr};
        if (!self) {
          return -1;
        }

        double value;
        if (! PyArg_ParseTupleAndKeywords(args, kwds, "d", (char**)kwlist,
                                          &value))
          return -1;
        if (value < std::numeric_limits<T>::min() || value >std::numeric_limits<T>::max() ) {
          PyErr_SetString(PyExc_OverflowError,"Integer value out of range of int");
          return -1;
        }
        self->_CObject = new T((T)value);


        return 0;
      }
    };

    //specialize for non-numeric fundamental types:
    template<typename T, bool is_complete>
      class InitHelper<T, is_complete, typename std::enable_if<!std::is_void<T>::value && !std::is_arithmetic<T>::value && std::is_fundamental<T>::value >::type>{
    public:
      static int init(PythonClassWrapper<T, is_complete> *self, PyObject* args, PyObject*kwds){
        if (!self) {
          return -1;
        }
        for (auto it = PythonClassWrapper<T, is_complete>::_constructors.begin(); it != PythonClassWrapper<T, is_complete>::_constructors.end(); ++it){
          try{
            if ((self->_CObject = (*it)(args, kwds)) != nullptr){
              break;
            }
          } catch(...){
          }
          PyErr_Clear();
        }
        if ( self->_CObject == nullptr){
          static const char * kwdlist[] = {"value", nullptr};
          PyObject* pyobj;
          if (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwdlist, pyobj) ) {
            if(!PyObject_TypeCheck(pyobj, (&PythonClassWrapper<T, is_complete>::Type))){
              PyErr_Print();
              PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
              return -1;
            }
            self->_CObject = new typename std::remove_reference<T>::type>(*(reinterpret_cast<PythonClassWrapper<T, is_complete>*>(pyobj)->get_CObject()));
          } else if ((!kwds || PyDict_Size(kwds)==0) && (!args || PyTuple_Size(args)==0)){
            self->_CObject = new typename std::remove_reference<T>::type();
            memset(self->_CObject, 0, sizeof(typename std::remove_reference<T>::type));
          }
        } else if((!args || PyTuple_Size(args)==0) && kwds && PyDict_Size(kwds)==1 ){
          if( PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True){
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
    };

    //specialize for non-numeric fundamental types:
    template<typename T, bool is_complete>
      class InitHelper<T, is_complete, typename std::enable_if<std::is_void<T>::value >::type>{
    public:
      static int init(PythonClassWrapper<T, is_complete> *self, PyObject* args, PyObject*kwds){
        if (!self) {
          return -1;
        }
        for (auto it = PythonClassWrapper<T, is_complete>::_constructors.begin(); it != PythonClassWrapper<T, is_complete>::_constructors.end(); ++it){
          try{
            if ((self->_CObject = (*it)(args, kwds)) != nullptr){
              break;
            }
          } catch(...){
          }
          PyErr_Clear();
        }
        if ( self->_CObject == nullptr){
          static const char * kwdlist[] = {"value", nullptr};
          PyObject* pyobj = Py_None;
          if (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwdlist, pyobj) ) {
            if(!PyObject_TypeCheck(pyobj, (&PythonClassWrapper<T, is_complete>::Type))){
              PyErr_Print();
              PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
              return -1;
            }
            self->_CObject = nullptr;
          } else if ((!kwds || PyDict_Size(kwds)==0) && (!args || PyTuple_Size(args)==0)){
            self->_CObject = nullptr;

          }
        } else if((!args || PyTuple_Size(args)==0) && kwds && PyDict_Size(kwds)==1 ){
          if( PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True){
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
    };


    //specialize for pointer types:
    template<typename T, bool is_complete>
      class InitHelper<T, is_complete, typename std::enable_if<!std::is_void<T>::value && !std::is_arithmetic<T>::value &&
      std::is_pointer<T>::value >::type>{
    public:
      static int init(PythonClassWrapper<T, is_complete> *self, PyObject* args, PyObject*kwds){
        if (!self) {
          return -1;
        }
        self->_CObject = nullptr;
        for (auto it = PythonClassWrapper<T, is_complete>::_constructors.begin(); it != PythonClassWrapper<T, is_complete>::_constructors.end(); ++it){
          try{
            if ((self->_CObject = (*it)(args, kwds)) != nullptr){
              break;
            }
          } catch(...){
          }
          PyErr_Clear();
        }

        if (self->_CObject == nullptr) {
          static const char * kwdlist[] = {"value", nullptr};
          PyObject pyobj;
          PythonClassWrapper<T, is_complete>* pyclass = reinterpret_cast<PythonClassWrapper<T, is_complete> *>(&pyobj);
          if (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwdlist, &pyobj)) {
            if(!PyObject_TypeCheck(&pyobj, (&PythonClassWrapper<T, is_complete>::Type))){
              PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
              return -1;
            }
            self->_CObject = pyclass->get_CObject();
          } else if((!args || PyTuple_Size(args)==0) && kwds && PyDict_Size(kwds)==1 ){
            if( PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True){
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
    };

    //specialize for copiable non-fundamental reference types:
    template<typename T, bool is_complete>
      class InitHelper<T , is_complete, typename std::enable_if<!std::is_void<T>::value &&
      !std::is_integral< typename std::remove_reference<T>::type >::value &&
      !std::is_floating_point< typename std::remove_reference<T>::type >::value &&
      std::is_copy_constructible<typename std::remove_reference<T>::type >::value &&
      std::is_reference<T>::value >::type>{
    public:
      static int init(PythonClassWrapper<T, is_complete> * self, PyObject *args, PyObject *kwds){
        if (!self) {
          return -1;
        }
        self->_CObject = nullptr;
        for (auto it = PythonClassWrapper<T, is_complete>::_constructors.begin(); it != PythonClassWrapper<T, is_complete>::_constructors.end(); ++it){
          try{
            if ((self->_CObject = (*it)(args, kwds)) != nullptr){
              break;
            }
          } catch(...){
          }
          PyErr_Clear();
        }

        if (self->_CObject == nullptr) {
          //static const char * kwdlist[] = {"value", nullptr};
          // PyObject *pyobj = nullptr;

          /*Iif (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwdlist, &pyobj)) {
            PythonCPointerWrapper<T_NoRef, is_complete> *pyptr = reinterpret_cast<PythonCPointerWrapper<T_NoRef, is_complete> *>(pyobj);
            if(PyObject_TypeCheck(&pyobj, (&PythonCPointerWrapper<T_NoRef, is_complete>::Type))){
            self->_CObject = (T_NoRef*) pyptr->ptr();
            }  else {
            PyObject_Print(pyobj, stderr, 0);
            PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
            return -1;
            }
            }else*/
          if((!args || PyTuple_Size(args)==0) && kwds && PyDict_Size(kwds)==1 ){
            if( PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True){
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
    };

    //specialize for non-copiable non-fundamental reference types:
    template<typename T, bool is_complete>
      class InitHelper<T, is_complete, typename std::enable_if<!std::is_integral< typename std::remove_reference<T>::type >::value &&
      !std::is_floating_point< typename std::remove_reference<T>::type >::value &&
      !std::is_copy_constructible<typename std::remove_reference<T>::type >::value &&
      std::is_reference<T>::value >::type>{
    public:
      static int init(PythonClassWrapper<T, is_complete> * self, PyObject *args, PyObject *kwds){
        if (!self) {
          return -1;
        }
        self->_CObject = nullptr;
        for (auto it = PythonClassWrapper<T, is_complete>::_constructors.begin(); it != PythonClassWrapper<T, is_complete>::_constructors.end(); ++it){
          try{
            if ((self->_CObject = (*it)(args, kwds)) != nullptr){
              break;
            }
          } catch(...){
          }
          PyErr_Clear();
        }

        if (self->_CObject == nullptr) {
          //static const char * kwdlist[] = {"value", nullptr};
          // PyObject *pyobj = nullptr;

          /*if (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwdlist, &pyobj)) {
            PythonCPointerWrapper<T_NoRef, is_complete> *pyptr = reinterpret_cast<PythonCPointerWrapper<T_NoRef, is_complete> *>(pyobj);
            if(PyObject_TypeCheck(&pyobj, (&PythonCPointerWrapper<T_NoRef, is_complete>::Type[ptr_depth<T>::value]))){
            self->_CObject = (T_NoRef*) pyptr->ptr();
            } else {
            PyErr_SetString(PyExc_TypeError, "Invalid type to construct from;  NOTE: type is not copy constructible");
            return -1;
            }
            }else */
          if((!args || PyTuple_Size(args)==0) && kwds && PyDict_Size(kwds)==1 ){
            if( PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True){
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
    };


    //specialize for integral reference types:
    template<typename T, bool is_complete>
      class InitHelper<T, is_complete, typename std::enable_if<std::is_integral< typename std::remove_reference<T>::type >::value &&
      std::is_reference<T>::value >::type>{
    public:
      static int init(PythonClassWrapper<T, is_complete> * self, PyObject *args, PyObject *kwds){
        typedef typename std::remove_reference<T>::type T_NoRef;
        if (!self) {
          return -1;
        }
        self->_CObject = nullptr;
        int status = 0;
        if (self->_CObject == nullptr) {
          static const char * kwdlist[] = {"value", nullptr};
          if (std::is_signed< T_NoRef>::value){
            long long intval  = 0;

            if (!PyArg_ParseTupleAndKeywords(args, kwds, "L", (char**)kwdlist, &intval)) {
              PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
              status =  -1;
              goto onerror;
            }
            if (intval < (long long) std::numeric_limits<T_NoRef>::min() || intval > (long long)std::numeric_limits<T_NoRef>::max()){
              PyErr_SetString(PyExc_TypeError, "Argument value out of range");
              status = -1;
              goto onerror;
            }
            self->_CObject = new T_NoRef((T_NoRef)intval);

          } else {
            unsigned long long intval  = 0;

            if (!PyArg_ParseTupleAndKeywords(args, kwds, "K", (char**)kwdlist, &intval)) {
              PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
              status = -1;
              goto onerror;
            }
            if (intval < (unsigned long long )std::numeric_limits<T_NoRef>::min() || intval > (unsigned long long )std::numeric_limits<T_NoRef>::max()){
              PyErr_SetString(PyExc_TypeError, "Argument value out of range");
              status =  -1;
              goto onerror;
            }
            self->_CObject = new T_NoRef((T_NoRef)intval);

          }
        }
      onerror:
        if(status != 0 && (!args || PyTuple_Size(args)==0) && kwds && PyDict_Size(kwds)==1 ){
          if( PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True){
            PyErr_Clear();
            status = 0;
          } else {
            if (( !args|| PyTuple_Size(args)==0)  && (!kwds || PyDict_Size(kwds)==0))
              PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
            status = -1;
          }
        }
        return status;
      }
    };

    //specialize for floating point reference types:
    template<typename T, bool is_complete>
      class InitHelper<T, is_complete, typename std::enable_if<std::is_floating_point< typename std::remove_reference<T>::type >::value && std::is_reference<T>::value >::type>{
    public:
      static int init(PythonClassWrapper<T, is_complete> * self, PyObject *args, PyObject *kwds){
        typedef typename std::remove_reference<T>::type T_NoRef;
        if (!self) {
          return -1;
        }


        if (self->_CObject == nullptr) {
          static const char * kwdlist[] = {"value", nullptr};
          double intval  = 0;

          if (!PyArg_ParseTupleAndKeywords(args, kwds, "d", (char**)kwdlist, &intval)) {
            PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
            return -1;
          }
          if (intval < (double) std::numeric_limits<T_NoRef>::min() || intval > (double)std::numeric_limits<T_NoRef>::max()){
            PyErr_SetString(PyExc_TypeError, "Argument value out of range");
            return -1;
          }
          self->_CObject = new T_NoRef((T_NoRef)intval);

        }
        return 0;
      }
    };

    //specialize for other complex types:
    template<typename T, bool is_complete>
      class InitHelper<T, is_complete, typename std::enable_if<!std::is_arithmetic<T>::value && !std::is_reference<T>::value &&
      !std::is_pointer<T>::value && !std::is_fundamental<T>::value >::type>{
    public:
      static int init(PythonClassWrapper<T, is_complete> *self, PyObject* args, PyObject*kwds){
        if (!self) {
          return -1;
        }
        self->_CObject = nullptr;
        for (auto it = PythonClassWrapper<T, is_complete>::_constructors.begin(); it != PythonClassWrapper<T, is_complete>::_constructors.end(); ++it){
          try{
            if ((self->_CObject = (*it)(args, kwds)) != nullptr){
              return 0;
            }
          } catch( ...) {
          }
          PyErr_Clear();
        }
        if((!args || PyTuple_Size(args)==0) && kwds && PyDict_Size(kwds)==1 ){
          if( PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True){
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
    };


    /**
     * Class to define Python wrapper to C class/type
     **/
    template<typename T, bool is_complete, typename Base>
    struct PythonClassWrapper < T, is_complete, -1, Base, typename std::enable_if< !std::is_array<T>::value && !std::is_pointer<T>::value >::type >: public CommonBaseWrapper{

        typedef typename std::remove_reference<T>::type T_NoRef;

        template< typename Z = T>
          typename std::remove_reference<T>::type* get_CObject() {
          return _CObject;
        }

          static PyTypeObject Type;
        typedef PyTypeObject* TypePtr_t;
        static TypePtr_t constexpr TypePtr = &Type;

        static int initialize(const char* const name, const char* const module_entry_name,
                              PyObject* module, const char* const fullname = nullptr){
          if(!name || strlen(name)==0) return -1;
          if(!module_entry_name || strlen(module_entry_name)==0) return -1;
          int status = 0;
          _name = name;
          if(Type.tp_name){/*already initialize*/ return status;}
          char* tp_name = new char[strlen(fullname?fullname:name)+1];
          strcpy(tp_name, fullname?fullname:name);
          Type.tp_name = tp_name;

          PyMethodDef pyMeth = {
            address_name,
            addr,
            METH_KEYWORDS,
            "C address of object"
          };
          _methodCollection.insert(_methodCollection.begin(), pyMeth);
          PyMethodDef pyMethAlloc = {
            alloc_name,
            (PyCFunction)Alloc<T, is_complete>::alloc,
            METH_KEYWORDS| METH_CLASS,
            "allocate arry of single dynamic instance of this class"
          };
          _methodCollection.insert(_methodCollection.begin(), pyMethAlloc);
          if(_memberSettersDict.size()!=0 && !std::is_const<T_NoRef>::value){
            PyMethodDef pyMeth = {
              "PYL_set_fields",
              (PyCFunction)set_fields,
              METH_KEYWORDS,
              nullptr
            };
            _methodCollection.insert(_methodCollection.begin(), pyMeth);
          }
          Type.tp_methods = _methodCollection.data();
          if (!_baseClasses.empty()){
            Type.tp_bases = PyTuple_New(_baseClasses.size());
            Py_ssize_t index = 0;
            for(auto it = _baseClasses.begin(); it != _baseClasses.end(); ++it){
              PyTuple_SET_ITEM( Type.tp_bases, index++, (PyObject*) *it);
            }
          }
          if (PyType_Ready(&Type) < 0){
            PyErr_SetString(PyExc_RuntimeError,"Failed to ready type!");
            PyErr_Print();
            status = -1 ;
            goto onerror;
          }
          {
            PyObject* const type = reinterpret_cast<PyObject*>(&Type);
            Py_INCREF(type);
            if (module != nullptr){
              if ( PyModule_AddObject(module, module_entry_name, type) == 0){
                parent_module = module;
              } else {
                PyErr_Print();
                PyErr_SetString(PyExc_RuntimeError,"Failed to add type to module!");
              }
            }
          }
        onerror:
          return status;
        }

        static std::string get_name(){ return _name;}

        /**
         * Add a constructor to the list contained
         **/
        typedef ConstructorContainerTmpl<T_NoRef> ConstructorContainer;
        typedef typename ConstructorContainerTmpl<T_NoRef>::constructor constructor;

        static void addConstructor( const char*  const kwlist[], constructor c){
          _constructors.push_back(ConstructorContainer(kwlist, c));
        }


        template <  typename ...Args >
        static T_NoRef* create( const char* const kwlist[], PyObject* args, PyObject* kwds){
          try{
            return _createBase( args, kwds, kwlist, typename argGenerator<sizeof...(Args)>::type(),(typename std::remove_reference<Args>::type*)nullptr...);
          } catch (const char* const msg){
            PyErr_SetString(PyExc_RuntimeError, msg);
            PyErr_Print();
            return nullptr;
          }
        }



        static PyObject* addr(PyObject* self, PyObject *args){
          if( (args&&PyTuple_Size(args)>0)){
            PyErr_BadArgument();
            return nullptr;
          }
          std::string ptr_name = std::string(Type.tp_name)+"*";
          std::string module_ptr_name = std::string(Type.tp_name)+"_ptr";
          PythonClassWrapper<T_NoRef*, is_complete >::initialize(ptr_name.c_str(), 
                                                                 module_ptr_name.c_str(),
                                                                 parent_module, 
                                                                 (_name + "*").c_str());
          PyObject* obj= toPyObject<T_NoRef*, is_complete>( reinterpret_cast<PythonClassWrapper*>(self)->_CObject, false);
          PyErr_Clear();
          ((PythonClassWrapper<T_NoRef*, is_complete>*)obj)->make_reference(self);
          return obj;
        }


        static PyObject* set_fields(PyObject* self, PyObject *args, PyObject* kwds){
          if( (args&&PyTuple_Size(args)!=0)){
            PyErr_BadArgument();
            return nullptr;
          }
          if(!kwds || PyDict_Size(kwds)){
            PyErr_BadArgument();
            return nullptr;
          }
          for( auto it = _memberSettersDict.begin(); it != _memberSettersDict.end(); ++it){
            PyObject* item = PyDict_GetItemString(kwds, it->first.c_str());
            if(item){
              PythonClassWrapper* self_ = reinterpret_cast<PythonClassWrapper*>(self);
              if (self_ && self_->_CObject)
                _memberSettersDict[it->first.c_str()](self_->_CObject, item);
            } else {
              PyErr_SetString(PyExc_SyntaxError, "Unrecognized field in call to set class instance member elements");
              return nullptr;
            }
          }
          //PyObject* obj= toPyObject<T, is_complete>( *reinterpret_cast<PythonClassWrapper*>(self)->_CObject, false);
          PyErr_Clear();
          return Py_None;
        }

        static bool findMemberSetter(const char* const name){
          return _memberSettersDict.find(name) != _memberSettersDict.end();
        }

        static void callMemberSetter(const char* const name, T_NoRef * this_, PyObject* pyval){
          if (findMemberSetter(name))
            _memberSettersDict[name](this_, pyval);
        }

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char* const name, typename ReturnType, typename ...Args>
          static void addClassMethod( ReturnType(*method)(Args...), const char * const kwlist[]) {
          static const char* const doc = "Call class method ";
          char *doc_string = new char[strlen(name) +strlen(doc)+1];
          snprintf(doc_string, strlen(name) +strlen(doc)+1, "%s%s",doc,name);

          PyMethodDef pyMeth = {
            name,
            (PyCFunction)ClassMethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::call,
            METH_KEYWORDS| METH_CLASS,
            doc_string
          };

          ClassMethodContainer<T>::template Container<name, ReturnType, Args...>::method = method;
          ClassMethodContainer<T>::template Container<name, ReturnType, Args...>::kwlist = kwlist;
          _addMethod(pyMeth);
        }

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char* const name, typename ReturnType, typename ...Args>
          static void addMethod( typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method, const char * const kwlist[]) {
          static const char* const doc = "Call method ";
          char *doc_string = new char[strlen(name) +strlen(doc)+1];
          snprintf(doc_string, strlen(name) +strlen(doc)+1, "%s%s",doc,name);

          PyMethodDef pyMeth = {
            name,
            (PyCFunction)MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::call,
            METH_KEYWORDS,
            doc_string
          };

          MethodContainer<T>::template Container<name, ReturnType, Args...>::method = method;
          MethodContainer<T>::template Container<name, ReturnType, Args...>::kwlist = kwlist;
          _addMethod(pyMeth);
        }

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char* const name, typename ReturnType, typename ...Args>
          static void addConstMethod( typename ConstMethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method, const char * const kwlist[]) {
          static const char* const doc = "Call method ";
          char *doc_string = new char[strlen(name) +strlen(doc)+1];
          snprintf(doc_string, strlen(name) +strlen(doc)+1, "%s%s",doc,name);

          PyMethodDef pyMeth = {
            name,
            (PyCFunction)ConstMethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::call,
            METH_KEYWORDS,
            doc_string
          };

          ConstMethodContainer<T>::template Container<name, ReturnType, Args...>::method = method;
          ConstMethodContainer<T>::template Container<name, ReturnType, Args...>::kwlist = kwlist;
          _addMethod(pyMeth);
        }


        static void addClassMember( const char* const name, PyObject* pyobj){
          if (!Type.tp_dict){
            Type.tp_dict = PyDict_New();
          }
          PyDict_SetItemString(Type.tp_dict, name , pyobj);
        }

        static void addExtraBaseClass( PyTypeObject* base){
          if(!base) return;
          if(!Type.tp_base && _baseClasses.empty()){
            Type.tp_base = base;
          } else {
            if (Type.tp_base){
              _baseClasses.push_back(Type.tp_base);
              Type.tp_base = nullptr;
            }
            _baseClasses.insert(_baseClasses.begin(),base);
          }
        }

        /**
         * add a getter method for the given compile-time-known named public class member
         **/
        template< const char* const name, size_t size, typename Type>
          static void addAttribute( typename MemberContainer<T_NoRef>::template Container<name, Type[size]>::member_t member){

          static const char* const doc = "Get attribute ";
          char *doc_string = new char[strlen(name) +strlen(doc)+1];
          snprintf(doc_string, strlen(name) +strlen(doc)+1, "%s%s",doc,name);
          static const char* const getter_prefix = "get_";
          //char *getter_name = new char[strlen(name) +strlen(getter_prefix)+1];
          //snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
          MemberContainer< T_NoRef>::template Container<name, Type[size]>::member = member;
          PyMethodDef pyMeth = {name,
                                (PyCFunction)MemberContainer<T_NoRef>::template Container<name,Type[size]>::call,
                                METH_KEYWORDS,
                                doc_string
          };
          _addMethod(pyMeth);
          _memberSettersDict[name]  = MemberContainer<T>::template Container<name, Type[size]>::setFromPyObject;
        }

        /**
         * add a getter method for the given compile-time-known named public class member
         **/
        template< const char* const name, typename Type>
          static void addAttribute( typename MemberContainer<T_NoRef>::template Container<name, Type>::member_t member){

          static const char* const doc = "Get attribute ";
          char *doc_string = new char[strlen(name) +strlen(doc)+1];
          snprintf(doc_string, strlen(name) +strlen(doc)+1, "%s%s",doc,name);
          //static const char* const getter_prefix = "get_";
          // char *getter_name = new char[strlen(name) +strlen(getter_prefix)+1];
          // snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
          MemberContainer< T_NoRef>::template Container<name, Type>::member = member;
          PyMethodDef pyMeth = {name,
                                (PyCFunction)MemberContainer<T_NoRef>::template Container<name,Type>::call,
                                METH_KEYWORDS,
                                doc_string
          };
          _addMethod(pyMeth);
          _memberSettersDict[name] = MemberContainer<T>::template Container<name, Type>::setFromPyObject;
        }


        /**
         * add a getter method for the given compile-time-known named public class member
         **/
        template< const char* const name, typename Type>
          static void addConstAttribute( typename ConstMemberContainer<T_NoRef>::template Container<name, Type>::member_t member){

          static const char* const doc = "Get attribute ";
          char *doc_string = new char[strlen(name) +strlen(doc)+1];
          snprintf(doc_string, strlen(name) +strlen(doc)+1, "%s%s",doc,name);
          //static const char* const getter_prefix = "get_";
          //char *getter_name = new char[strlen(name) +strlen(getter_prefix)+1];
          //snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
          ConstMemberContainer< T_NoRef>::template Container<name, Type>::member = member;
          PyMethodDef pyMeth = {name,
                                (PyCFunction)ConstMemberContainer<T_NoRef>::template Container<name,Type>::call,
                                METH_KEYWORDS,
                                doc_string
          };
          _addMethod(pyMeth);
        }


        /**
         * add a getter method for the given compile-time-known named public static class member
         **/
        template< const char* const name, typename Type>
          static void addClassAttribute( Type *member){

          static const char* const doc = "Get attribute ";
          char *doc_string = new char[strlen(name) +strlen(doc)+1];
          snprintf(doc_string, strlen(name) +strlen(doc)+1, "%s%s",doc,name);
          static const char* const getter_prefix = "get_";
          //char *getter_name = new char[strlen(name) +strlen(getter_prefix)+1];
          //snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
          static const char* const kwlist[] = {"value",nullptr};
          ClassMemberContainer< T_NoRef>::template Container<name, Type>::kwlist = kwlist;
          ClassMemberContainer< T_NoRef>::template Container<name, Type>::member = member;
          PyMethodDef pyMeth = {name,
                                (PyCFunction)ClassMemberContainer<T_NoRef>::template Container<name,Type>::call,
                                METH_KEYWORDS| METH_CLASS,
                                doc_string
          };
          _addMethod(pyMeth);
          _memberSettersDict[name] = ClassMemberContainer<T>::template Container<name, Type>::setFromPyObject;
        }

        /**
         * add a getter method for the given compile-time-known named public static class member
         **/
        template< const char* const name, typename Type>
          static void addConstClassAttribute( Type const *member){

          static const char* const doc = "Get attribute ";
          char *doc_string = new char[strlen(name) +strlen(doc)+1];
          snprintf(doc_string, strlen(name) +strlen(doc)+1, "%s%s",doc,name);
          static const char* const getter_prefix = "get_";
          //char *getter_name = new char[strlen(name) +strlen(getter_prefix)+1];
          //snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
          static const char* const kwlist[] = {"value",nullptr};
          ConstClassMemberContainer< T_NoRef>::template Container<name, Type>::kwlist = kwlist;
          ConstClassMemberContainer< T_NoRef>::template Container<name, Type>::member = member;
          PyMethodDef pyMeth = {name,
                                (PyCFunction)ConstClassMemberContainer<T_NoRef>::template Container<name,Type>::call,
                                METH_KEYWORDS| METH_CLASS,
                                doc_string
          };
          _addMethod(pyMeth);
        }

        void set_contents(typename std::remove_reference<T>::type * ptr){
          _CObject = ptr;
        }

        static PyObject * parent_module;

        template< typename C, bool is_complete2, const ssize_t size, typename depth>
          friend class PythonCPointerWrapper;

        template<typename C, bool is_complee2, typename E>
          friend  class InitHelper;
        friend class PythonClassWrapper<T, is_complete>;

        protected:

        T_NoRef* _CObject;

        private:

        template<typename Class, bool complete = is_complete, typename Z = void>
          struct  Alloc;

        template<typename Class>
        struct  Alloc<Class, true, typename std::enable_if< !is_function_ptr<Class>::value && !std::is_reference<Class>::value && std::is_constructible<Class>::value && std::is_destructible<Class>::value>::type>{

          static PyObject* alloc( PyObject* cls, PyObject *args, PyObject* kwds){
            (void)cls;
            PyObject* alloc_kwds  = PyDict_New();
            PyDict_SetItemString(alloc_kwds, "__internal_allow_null", Py_True);
            //Check if argument is list of tuples, and if so construct
            //an array of objects to store "behind the pointer"
            if( (!kwds || PyDict_Size(kwds) == 0) && args && PyTuple_Size(args) == 1 && PyList_Check(args)){
              PyObject* list = PyTuple_GetItem(args, 0);
              const Py_ssize_t size = PyList_Size(list);
              char * raw_storage = (char*)operator new[](size*sizeof(Class));
              Class* values = reinterpret_cast<Class*>(raw_storage);
              PyDict_SetItemString(alloc_kwds, "size", PyLong_FromSsize_t(size));
              PythonClassWrapper<Class*, is_complete, 0>* obj=
                (PythonClassWrapper<Class*, is_complete, 0>*)PyObject_Call((PyObject*)&PythonClassWrapper<Class*, is_complete, 0>::Type,
                                                                           args, alloc_kwds);
              Py_DECREF(alloc_kwds);
              for(Py_ssize_t i = 0; i < PyTuple_Size(args); ++i){
                PyObject* constructor_pyargs = PyList_GetItem(list, i);
                Class* cobj  = nullptr;
                if (!PyTuple_Check(constructor_pyargs) && !PyDict_Check(constructor_pyargs)){
                  PyErr_SetString(PyExc_TypeError, "Invalid element in list argument, expected tuple");
                  Py_DECREF(obj);
                  for (Py_ssize_t j = 0; j < i; ++j){
                    values[j].~Class();
                  }
                  delete[] raw_storage;
                  return nullptr;
                } else if (PyTuple_Check(constructor_pyargs)){
                  for (auto it = _constructors.begin(); it != _constructors.end(); ++it){
                    try{
                      static PyObject* emptylist = PyDict_New();
                      if ((cobj = (*it)(args, emptylist ))){ break;}
                    } catch(...){
                    }
                    PyErr_Clear();
                  }
                } else if (PyDict_Check(constructor_pyargs)){
                  Class* cobj  = nullptr;
                  for (auto it = _constructors.begin(); it != _constructors.end(); ++it){
                    try{
                      static PyObject* emptyargs = PyTuple_New(0);
                      if ((cobj = (*it)(emptyargs, constructor_pyargs ))){ break;}
                    } catch(...){
                    }
                    PyErr_Clear();
                  }
                }
                if (!cobj){
                  PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
                  Py_DECREF(obj);
                  for (Py_ssize_t j = 0; j < i; ++j){
                    values[j].~Class();
                  }
                  delete[] raw_storage;

                  return nullptr;
                }
                obj->set_contents_at( i, (void**)&values[i]);
              }
            }

            //otherwise, just have regular list of constructor arguments
            //for single object allocation
            Class *cobj = nullptr;
            for (auto it = _constructors.begin(); it != _constructors.end(); ++it){
              try{
                if ((cobj = (*it)(args, kwds ))){ break;}
              } catch(...){
              }
              PyErr_Clear();
            }
            if (!cobj){
              Py_DECREF(alloc_kwds);
              PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
              return nullptr;
            }
            PythonClassWrapper<Class*, is_complete, 0>* obj=
              (PythonClassWrapper<Class*, is_complete, 0>*)PyObject_Call((PyObject*)&PythonClassWrapper<Class*, is_complete, 0>::Type,
                                                                         args, alloc_kwds);
            Py_DECREF(alloc_kwds);
            obj->set_contents((void**)cobj);
            return (PyObject*)obj;
          }
        };

        template<typename Class>
        struct  Alloc<Class, true, typename std::enable_if< !is_function_ptr<Class>::value && !std::is_reference<Class>::value &&std::is_constructible<Class>::value && !std::is_destructible<Class>::value>::type>{
          static PyObject* alloc( PyObject* cls, PyObject *args, PyObject* kwds){
            (void)cls;
            PyObject* alloc_kwds  = PyDict_New();
            PyDict_SetItemString(alloc_kwds, "__internal_allow_null", Py_True);
            //Check if argument is list of tuples, and if so construct
            //an array of objects to store "behind the pointer"
            if( (!kwds || PyDict_Size(kwds) == 0) && args && PyTuple_Size(args) == 1 && PyList_Check(args)){
              PyObject* list = PyTuple_GetItem(args, 0);
              const Py_ssize_t size = PyList_Size(list);
              char * raw_storage = (char*)operator new[](size*sizeof(Class));
              Class* values = reinterpret_cast<Class*>(raw_storage);
              PyDict_SetItemString(alloc_kwds, "size", PyLong_FromSsize_t(size));
              PythonClassWrapper<Class*, is_complete, 0>* obj=
                (PythonClassWrapper<Class*, is_complete, 0>*)PyObject_Call((PyObject*)&PythonClassWrapper<Class*, is_complete, 0>::Type,
                                                                           args, alloc_kwds);
              Py_DECREF(alloc_kwds);
              for(Py_ssize_t i = 0; i < PyTuple_Size(args); ++i){
                PyObject* constructor_pyargs = PyList_GetItem(list, i);
                Class* cobj  = nullptr;
                if (!PyTuple_Check(constructor_pyargs) && !PyDict_Check(constructor_pyargs)){
                  PyErr_SetString(PyExc_TypeError, "Invalid element in list argument, expected tuple");
                  Py_DECREF(obj);
                  delete[] raw_storage;
                  PyErr_SetString(PyExc_RuntimeError, "NOTE: Freed memory, but no visible destructor available to call.");
                  return nullptr;
                } else if (PyTuple_Check(constructor_pyargs)){
                  for (auto it = _constructors.begin(); it != _constructors.end(); ++it){
                    try{
                      static PyObject* emptylist = PyDict_New();
                      if ((cobj = (*it)(args, emptylist ))){ break;}
                    } catch(...){
                    }
                    PyErr_Clear();
                  }
                } else if (PyDict_Check(constructor_pyargs)){
                  Class* cobj  = nullptr;
                  for (auto it = _constructors.begin(); it != _constructors.end(); ++it){
                    try{
                      static PyObject* emptyargs = PyTuple_New(0);
                      if ((cobj = (*it)(emptyargs, constructor_pyargs ))){ break;}
                    } catch(...){
                    }
                    PyErr_Clear();
                  }
                }
                if (!cobj){
                  PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation.  Objects not destructible by design! ");
                  Py_DECREF(obj);

                  delete[] raw_storage;

                  return nullptr;
                }
                obj->set_contents(values);
              }
            }

            //otherwise, just have regular list of constructor arguments
            //for single object allocation
            Class *cobj = nullptr;
            for (auto it = _constructors.begin(); it != _constructors.end(); ++it){
              try{
                if ((cobj = (*it)(args, kwds ))){ break;}
              } catch(...){
              }
              PyErr_Clear();
            }
            if (!cobj){
              Py_DECREF(alloc_kwds);
              PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
              return nullptr;
            }
            PythonClassWrapper<Class*, is_complete, 0>* obj=
              (PythonClassWrapper<Class*, is_complete, 0>*)PyObject_Call((PyObject*)&PythonClassWrapper<Class*, is_complete, 0>::Type,
                                                                         args, alloc_kwds);
            Py_DECREF(alloc_kwds);
            obj->template set_contents<Class>(cobj);
            return (PyObject*)obj;
          }
        };

        template<typename ReturnType, typename ...Args>
          struct  Alloc<ReturnType(*)(Args...), true, std::enable_if< std::is_constructible<int>::value>::type >{
          typedef ReturnType(*Class)(Args...);
          static PyObject* alloc( PyObject* cls, PyObject *args, PyObject* kwds){
            (void)cls;
            PyObject* alloc_kwds  = PyDict_New();
            PyDict_SetItemString(alloc_kwds, "__internal_allow_null", Py_True);
            //Check if argument is list of tuples, and if so construct
            //an array of objects to store "behind the pointer"
            if( (!kwds || PyDict_Size(kwds) == 0) && args && PyTuple_Size(args) == 1 && PyList_Check(args)){
              PyObject* list = PyTuple_GetItem(args, 0);
              const Py_ssize_t size = PyList_Size(list);
              char * raw_storage = (char*)operator new[](size*sizeof(Class));
              Class* values = reinterpret_cast<Class*>(raw_storage);
              PyDict_SetItemString(alloc_kwds, "size", PyLong_FromSsize_t(size));
              PythonClassWrapper<Class*, is_complete>* obj=
                (PythonClassWrapper<Class*, is_complete>*)PyObject_Call((PyObject*)&PythonClassWrapper<Class*, is_complete>::Type,
                                                                        args, alloc_kwds);
              Py_DECREF(alloc_kwds);
              for(Py_ssize_t i = 0; i < PyTuple_Size(args); ++i){
                PyObject* constructor_pyargs = PyList_GetItem(list, i);
                Class* cobj  = nullptr;
                if (!PyTuple_Check(constructor_pyargs) && !PyDict_Check(constructor_pyargs)){
                  PyErr_SetString(PyExc_TypeError, "Invalid element in list argument, expected tuple");
                  Py_DECREF(obj);
                  for (Py_ssize_t j = 0; j < i; ++j){
                    values[j].~Class();
                  }
                  delete[] raw_storage;
                  return nullptr;
                } else if (PyTuple_Check(constructor_pyargs)){
                  for (auto it = _constructors.begin(); it != _constructors.end(); ++it){
                    try{
                      static PyObject* emptylist = PyDict_New();
                      if ((cobj = (*it)(args, emptylist ))){ break;}
                    } catch(...){
                    }
                    PyErr_Clear();
                  }
                } else if (PyDict_Check(constructor_pyargs)){
                  Class* cobj  = nullptr;
                  for (auto it = _constructors.begin(); it != _constructors.end(); ++it){
                    try{
                      static PyObject* emptyargs = PyTuple_New(0);
                      if ((cobj = (*it)(emptyargs, constructor_pyargs ))){ break;}
                    } catch(...){
                    }
                    PyErr_Clear();
                  }
                }
                if (!cobj){
                  PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
                  Py_DECREF(obj);
                  for (Py_ssize_t j = 0; j < i; ++j){
                    values[j].~Class();
                  }
                  delete[] raw_storage;

                  return nullptr;
                }
                obj->set_contents_at(i, (void**)&values[i]);

              }
            }

            //otherwise, just have regular list of constructor arguments
            //for single object allocation
            Class *cobj = nullptr;
            for (auto it = _constructors.begin(); it != _constructors.end(); ++it){
              try{
                if ((cobj = (*it)(args, kwds ))){ break;}
              } catch(...){
              }
              PyErr_Clear();
            }
            if (!cobj){
              Py_DECREF(alloc_kwds);
              PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
              return nullptr;
            }
            PythonClassWrapper<Class*, is_complete>* obj=
              (PythonClassWrapper<Class*, is_complete>*)PyObject_Call((PyObject*)&PythonClassWrapper<Class*, is_complete>::Type,
                                                                      args, alloc_kwds);
            Py_DECREF(alloc_kwds);
            obj->set_contents((void**)cobj);
            return (PyObject*)obj;
          }
        };

        template<typename Class>
        struct  Alloc<Class, is_complete, typename std::enable_if< std::is_reference<Class>::value || !std::is_constructible<Class>::value>::type>{
          static PyObject* alloc( PyObject* cls, PyObject *args, PyObject* kwds){
            (void) args; (void) kwds; (void)cls;
            PyErr_SetString(PyExc_RuntimeError, "Type is not directly constructible");
            return nullptr;
          }
        };


        static int
        _init( PythonClassWrapper *self, PyObject *args, PyObject *kwds){
          if( Type.tp_base && Base::TypePtr->tp_init){
            PyObject* empty = PyTuple_New(0);
            Base::TypePtr->tp_init( (PyObject*)&self->baseClass, empty, nullptr);
            Py_DECREF(empty);
            PyErr_Clear();
          }
          return InitHelper<T, is_complete>::init(self, args, kwds);
        }


        static PyObject* _new( PyTypeObject* type, PyObject* args, PyObject*kwds) {
          (void)args;
          (void)kwds;
          PythonClassWrapper  *self;
          self = (PythonClassWrapper*)type ->tp_alloc(type, 0);
          /*if( Type.tp_base && Base::TypePtr->tp_new){
            PyObject* empty = PyTuple_New(0);
            Base::TypePtr->tp_new( Type.tp_base, empty, nullptr);
            Py_DECREF(empty);
            }*/
          if ( nullptr != self) {
            self->_CObject = nullptr;
          }
          return (PyObject*) self;
        }

        static void _dealloc( PythonClassWrapper* self) {
          if(!self) return;
          //TODO: Track when object is actually created under new 
          //and delete, but do so in tp_free!!
          //delete self->_CObject;
          self->_CObject = nullptr;
          self->baseClass.ob_type->tp_free((PyObject*)self);
          if (self->_referenced){
            Py_XDECREF( self->_referenced );
            self->_referenced = nullptr;
          }
        }


        template <typename ...PyO>
        static bool _parsePyArgs( const char* const kwlist[],  PyObject* args, PyObject* kwds, PyO*& ...pyargs){
          char format[sizeof...(PyO)+1] = {0};
          if(sizeof...(PyO)> 0)
            memset(format, 'O', sizeof...(PyO));
          return sizeof...(PyO)== 0 || PyArg_ParseTupleAndKeywords(args, kwds, format, (char**)kwlist, &pyargs... );
        }

        template< typename ...Args>
        static T_NoRef* _createBaseBase(Args... args){
          return new T_NoRef(args...);
        }

        template< typename ...Args, int ...S>
          static typename std::remove_reference<T>::type* _createBase( PyObject* args, PyObject* kwds, const char* const kwlist[], container<S...> unused1, Args*... unused2){
          (void)unused1;
          void* unused[] = {(void*)unused2..., nullptr}; (void)unused;
          if (args && PyTuple_Size(args)!= sizeof...(Args)){
            return nullptr;
          }
          PyObject* pyobjs[sizeof...(Args)+1];
          (void)pyobjs;
          if (!_parsePyArgs(kwlist, args, kwds, pyobjs[S]...)){
            PyErr_SetString(PyExc_TypeError, "Invalid constgructor arguments");
            PyErr_Print();//TODO: REMOVE:  only for debugging;  other constructors may work;  they are try/catched and may not be a real error here (error string is cleared on finding a good constructor!)
            return nullptr;
          }

          return _createBaseBase<Args...>( *toCObject<Args, false, is_complete>(*pyobjs[S])...);
        }

        static std::string _name;
        static std::vector<ConstructorContainer> _constructors;
        static std::map<std::string, typename MethodContainer<T>::setter_t> _memberSettersDict;
        static std::vector<PyMethodDef> _methodCollection;
        static std::vector<PyTypeObject*> _baseClasses;
        static void _addMethod( PyMethodDef method){
          //insert at beginning to keep null sentinel at end of list:
          _methodCollection.insert(_methodCollection.begin(), method);
          Type.tp_methods = _methodCollection.data();
        }



      };


    template< typename T, bool is_complete, typename Base>
      PyObject* PythonClassWrapper<T, is_complete, -1, Base, typename std::enable_if< !std::is_array<T>::value && !std::is_pointer<T>::value >::type>::parent_module = nullptr;

    template< typename T, bool is_complete, typename Base>
      std::vector<PyMethodDef> PythonClassWrapper<T, is_complete, -1, Base, typename std::enable_if< !std::is_array<T>::value && !std::is_pointer<T>::value >::type>::_methodCollection = std::vector<PyMethodDef>(emptyMethods, emptyMethods+1);

    template< typename T, bool is_complete, typename Base>
      std::vector< PyTypeObject* > PythonClassWrapper<T, is_complete, -1, Base,typename std::enable_if< !std::is_array<T>::value && !std::is_pointer<T>::value >::type>::_baseClasses = std::vector<PyTypeObject * >();

    template< typename T, bool is_complete, typename Base>
      std::map<std::string, typename MethodContainer<T>::setter_t>
      PythonClassWrapper<T, is_complete, -1, Base,typename std::enable_if< !std::is_array<T>::value && !std::is_pointer<T>::value >::type>::_memberSettersDict =
      std::map< std::string, typename MethodContainer<T>::setter_t>();

    template<typename T, bool is_complete, typename Base>
      std::vector<typename PythonClassWrapper<T, is_complete, -1, Base, typename std::enable_if< !std::is_array<T>::value && !std::is_pointer<T>::value >::type>::ConstructorContainer >
      PythonClassWrapper<T, is_complete, -1, Base, typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value >::type>::_constructors;

    template<typename T, bool is_complete, typename Base>
      std::string PythonClassWrapper<T, is_complete, -1, Base, typename std::enable_if< !std::is_array<T>::value &&  !std::is_pointer<T>::value >::type>::_name;

    template< typename T, bool is_complete, typename Base>
      PyTypeObject PythonClassWrapper<T, is_complete, -1, Base, typename std::enable_if< !std::is_array<T>::value &&  !std::is_pointer<T>::value >::type>::Type = {

      PyObject_HEAD_INIT(nullptr)
      0,                         /*ob_size*/
      nullptr,             /*tp_name*/ /*filled on init*/
      sizeof(PythonClassWrapper),             /*tp_basicsize*/
      0,                         /*tp_itemsize*/
      (destructor)PythonClassWrapper::_dealloc, /*tp_dealloc*/
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
      nullptr,		               /* tp_traverse */
      nullptr,		               /* tp_clear */
      nullptr,		               /* tp_richcompare */
      0,		                       /* tp_weaklistoffset */
      nullptr,		               /* tp_iter */
      nullptr,		               /* tp_iternext */
      PythonClassWrapper::_methodCollection.data(),             /* tp_methods */
      nullptr,             /* tp_members */
      nullptr,                         /* tp_getset */
      Base::TypePtr,                         /* tp_base */
      nullptr,                         /* tp_dict */
      nullptr,                         /* tp_descr_get */
      nullptr,                         /* tp_descr_set */
      0,                         /* tp_dictoffset */
      (initproc)PythonClassWrapper::_init,  /* tp_init */
      nullptr,                         /* tp_alloc */
      PythonClassWrapper::_new,             /* tp_new */
      nullptr,                         /*tp_free*/ //TODO: Implement a free??
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
#endif
