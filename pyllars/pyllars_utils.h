#include "pyllars/pyllars_pointer.h"

#ifndef __PYLLARS__INTERNAL__UTILS_H
#define __PYLLARS__INTERNAL__UTILS_H

#include <structmember.h>

#ifndef __PYLLARS__INTERNAL__POINTER_H
namespace __pyllars_internal{
#endif

template<int ...S>
struct seq {
    seq( PyObject* const objs):pyobjs(objs){}
    PyObject* const pyobjs;
};

template<int N, int ...S>
struct gens : gens<N-1, N-1, S...> {

};

template<int ...S>
struct gens<0, S...> {
  typedef seq<S...> type;

};


/////////////////////////////////////////////////////////////////////////////////

  /**
   * template to convert c object to python
   **/
  template< typename c_type>
  PyObject* toPyObject( const c_type & var){
    PyObject* pyobj = PyObject_CallObject(  (PyObject*)&PythonClassWrapper<c_type>::Type, nullptr);
    if ( !pyobj || !PyObject_TypeCheck(pyobj,&PythonClassWrapper<c_type>::Type)){
        throw "Unable to create pyobject from C object";
    }
    reinterpret_cast<PythonClassWrapper<c_type>*>(pyobj)->_CObject= new typename std::remove_reference<c_type>::type(var);
    return pyobj;
  }

  template< typename c_type>
  PyObject* toPyObject( const c_type * var){
    PyObject* pyobj = PyObject_CallObject(  (PyObject*)PythonCPointerWrapper<c_type>::Type, nullptr);
    if ( !pyobj || !PyObject_TypeCheck(pyobj, PythonCPointerWrapper<c_type>::Type)){
        throw "Unable to create pyobject from C object";
    }
    dynamic_cast<PythonCPointerWrapper<c_type> >(pyobj)->_CObject= new c_type(var);
    return pyobj;
  }

  template<>
  PyObject* toPyObject<char>( const char & var){
    return PyInt_FromLong( var);
  }

  template<>
  PyObject* toPyObject<short>( const short & var){
    return PyInt_FromLong( var);
  }

  template<>
  PyObject* toPyObject<int>( const int & var){
    return PyInt_FromLong( var);
  }

  template<>
  PyObject* toPyObject<long>( const long & var){
    return PyInt_FromLong( var);
  }

#ifndef __PYLLARS__INTERNAL__POINTER_H
}
#endif // __PYLLARS__INTERNAL__POINTER_H
#endif
