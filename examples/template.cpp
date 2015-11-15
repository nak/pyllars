
#include <functional>
#include <Python.h>

namespace __pyllars_internal{
  
  /**
   * template fnction to convert python to C object
   **/
  template< typename return_type, typename py_type>
  return_type toCObject( PyObject& pyobj){
    return dynamic_cast<py_type&>(pyobj)._Cobject;
  }
  

  /// explicit instantiations for fundamental types
  template<>
  long int toCObject< long int, PyObject>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    } 
    return PyLong_AsLong( &pyobj );
  }

  template<>
  int toCObject< int, PyObject>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    } 
    return PyLong_AsLong( &pyobj );
  }


  template<>
  short int toCObject< short int, PyObject>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    } 
    return PyLong_AsLong( &pyobj );
  }

  template<>
  char toCObject< char, PyObject>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    } 
    return PyLong_AsLong( &pyobj );
  }

  template<>
  unsigned long int toCObject< unsigned long int, PyObject>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    } 
    return PyLong_AsLong( &pyobj );
  }

  template<>
  unsigned int toCObject< unsigned int, PyObject>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    } 
    return PyLong_AsLong( &pyobj );
  }

  template<>
  unsigned short int toCObject< unsigned short int, PyObject>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    } 
    return PyLong_AsLong( &pyobj );
  }


  template<>
  unsigned char toCObject< unsigned char, PyObject>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    } 
    return PyLong_AsLong( &pyobj );
  }

  template<>
  float toCObject< float, PyObject>( PyObject& pyobj){
    return PyFloat_AsDouble( &pyobj );
  }

  template<>
  double toCObject< double, PyObject>( PyObject& pyobj){
    return PyFloat_AsDouble( &pyobj );
  }



  /**
   * template to convert c object to python
   **/
  template< typename c_type>
  PyObject* toPyObject( const c_type & var){
    return Py_None;//XSLT HERE
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

  template<>
  PyObject* toPyObject<unsigned char>( const unsigned char & var){
    return PyInt_FromLong( var);
  }

  template<>
  PyObject* toPyObject<unsigned short>( const unsigned short & var){
    return PyInt_FromLong( var);
  }
  
  template<>
  PyObject* toPyObject<unsigned int>( const unsigned int & var){
    return PyInt_FromLong( var);
  }

  template<>
  PyObject* toPyObject<unsigned long>( const unsigned long & var){
    return PyInt_FromLong( var);
  }

  template<>
  PyObject* toPyObject<float>( const float & var){
    return PyFloat_FromDouble( var);
  }

  template<>
  PyObject* toPyObject<double>( const double & var){
    return PyFloat_FromDouble( var);
  }

  template
  typedef < typename return_type, typename... Args> return_type(*cb_type)(Args...);

  template< typename return_type, typename... Args>
  PyObject* toPyObject< return_type(*)(Args...)>( const cb_type< return_type, Args...> var){
    return Py_None;
  }


template <  const size_t count, 
	    typename return_type,
	    typename pyclass,
	    typename... Args>
class CallbackPool{
public:

  typedef return_type(*callback_t)( Args...);
  
  static PyObject *pycallbacks[count];
  static callback_t c_callbacks[count];


  template< const size_t index>
  static return_type cb( Args...  args){
    PyObject pyargs[] = { *toPyObject(args)...};
    PyObject* result = PyObject_CallObject( pycallbacks[index],
					    pyargs);
    if (!result){ throw "Invalid argument";}
    return toCObject<return_type, pyclass>(*result);

  }  

  
};

template < const size_t count,
	   typename return_type,
	   typename pyclass,
	   typename... Args>
PyObject* CallbackPool< count, return_type,pyclass, Args...>::pycallbacks[count];

template < const size_t count,
	   typename return_type,
	   typename pyclass,
	   typename... Args>
typename CallbackPool< count, return_type,pyclass, Args...>::callback_t
CallbackPool< count, return_type,pyclass, Args...>::c_callbacks[count];


template < const size_t count,
	   const size_t index,
	   typename return_type,
	    typename pyclass,
	    typename... Args>
static return_type __cb( Args...  args){
  typedef CallbackPool<count, return_type, pyclass, Args...> Pool;
  PyObject pyargs[] = { *toPyObject(args)...};
  PyObject* result = PyObject_CallObject( Pool::pycallbacks[index],
					  pyargs);
  if (!result){ throw "Invalid argument";}
  return toCObject<return_type, pyclass>(*result);
  
}  



template <  const size_t count, 
	    const size_t index,
	    typename return_type,
	    typename pyclass,
	    typename... Args
	   >
class CBPoolInitializer{
public:

  CBPoolInitializer(){
    typedef CallbackPool<count, return_type, pyclass, Args...> Pool;
    Pool::c_callbacks[index-1] = &__cb<count, index, return_type, pyclass, Args...>;
    CBPoolInitializer<count, index-1, return_type, pyclass, Args...>();
  }

};

template <  const size_t count, 
	    typename return_type,
	    typename pyclass,
	    typename... Args
	   >
class CBPoolInitializer<count, 0, return_type, pyclass, Args...>{
public:
  typedef CallbackPool<count, return_type, pyclass, Args...> Pool;

  CBPoolInitializer(){
  }

};

#define MAX_CB_POOL_DEPTH 898
CBPoolInitializer< MAX_CB_POOL_DEPTH, MAX_CB_POOL_DEPTH,
		   int, PyObject, int , double> initializer;



template< typename pyclass, 
	  typename return_type, typename... Args>
class PyCallbackWrapper{
public:

  typedef CallbackPool< MAX_CB_POOL_DEPTH, 
			return_type, 
			pyclass, 
			Args...> Pool;

  PyCallbackWrapper( PyObject* pycb){
    if (cb_index >= MAX_CB_POOL_DEPTH){
      throw "Callbaks exhausted";
    }
    Pool::pycallbacks[cb_index] = pycb;
    Py_INCREF(pycb);
    _my_cb_index = cb_index++;    
  }


  typename Pool::callback_t get_C_callback(){
    return Pool::c_callbacks[_my_cb_index];
  }
  

private:
  static size_t cb_index ;
  size_t _my_cb_index;
  
};



template< typename pyclass, 
	  typename return_type, typename... Args>
size_t PyCallbackWrapper<pyclass, return_type, Args...>::cb_index = 0;


}
using namespace __pyllars_internal;

typedef int(*dummy)(int i, double d);

void add_cb( dummy f){
  f(1, 1.0);
}
 

int main(){
  PyCallbackWrapper< PyObject, int, int, double > wr(new PyObject() );
  add_cb(wr.get_C_callback());
}

