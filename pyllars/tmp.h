#ifndef __PYLLARS_INTERNAL__CALLBACKS_H
#define __PYLLARS_INTERNAL__CALLBACKS_H

#ifndef MAX_CB_POOL_DEPTH 
#define MAX_CB_POOL_DEPTH 898
#endif


namespace __pyllars_internal{

  /**
   * template class for a pool of C-style callbacks
   * mapped to python-function-callbacks.  There is
   * probably some magic way through ffi to do this,
   * but didn't want to go that route just yet
   **/
  template <  const size_t count, 
              typename ReturnType,
              typename... Args>
  class CallbackPool{
  public:
    
    typedef ReturnType(*callback_t)( Args...);
    
    static PyObject *pycallbacks[count];
    static callback_t c_callbacks[count];


    /*   template< const size_t index>
    static ReturnType cb( Args...  args){
      PyObject pyargs[] = { *toPyObject(args)...};
      if(!pycallbacks[index] || PyCallable_check(pycallbacks[index]){
	   PyErr_SetString( PyExc_RuntimeError,
                            "Python callback is not callable!");
	   throw "Python callback is not callable!");
      }
      PyObject* result = PyObject_CallObject( pycallbacks[index],
                                            pyargs);
      if (!result){ throw "Invalid argument";}
      return toCObject<ReturnType>(*result);

      }  */

  
  };




  template < const size_t count,
             typename ReturnType,
             typename... Args>
  PyObject* CallbackPool< count, ReturnType, Args...>::pycallbacks[count];
  
  template < const size_t count,
             typename ReturnType,
             typename... Args>
  typename CallbackPool< count, ReturnType, Args...>::callback_t
  CallbackPool< count, ReturnType, Args...>::c_callbacks[count];
  
  
  template < const size_t count,
             const size_t index,
             typename ReturnType,
             typename... Args>
  static ReturnType __cb( Args...  args){
    typedef CallbackPool<count, ReturnType, Args...> Pool;
    if(!pycallbacks[index] || PyCallable_check(pycallbacks[index]){
	   PyErr_SetString( PyExc_RuntimeError,
                            "Python callback is not callable!");
	   throw "Python callback is not callable!");
    }
    PyObject pyargs[] = { *toPyObject(args)...};
    PyObject pytuple = PyTuple_New(sizeof...(args));
    for (int i = 0; i < sizeof...(args); ++ i){
      PyTuple_SetItem( pytuple, i, pyargs[i]);
    }
    PyObject* result = PyObject_CallObject
      ( Pool::pycallbacks[index],
	pytuple);
    if (!result){ 
      throw "Invalid arguments or error in callback";
    } else if( std::is_pointer<ReturnType>::value && 
	       result == Py_None){
      ReturnType retval;
      memset(retval, 0, sizeof(retval)); //null pointer, but this generic type, so....
      return retval;
    } else if( result == Py_None){
      throw "Invalid return type from callback";
    }
    for (int i = 0; i < sizeof...(args); ++ i){
      Py_DECREF(pyargs[i]);
    }
    Py_DECREF(pytuple);
    return toCObject<ReturnType>(*result);
    
  }  
  
  /**
   * specialize for void return
   **/
   template < const size_t count,
             const size_t index,
             typename... Args>
  static ReturnType __cb( Args...  args){
    typedef CallbackPool<count, void, Args...> Pool;
    if(!pycallbacks[index] || PyCallable_check(pycallbacks[index]){
	   PyErr_SetString( PyExc_RuntimeError,
                            "Python callback is not callable!");
	   throw "Python callback is not callable!");
    }
    PyObject pyargs[] = { *toPyObject(args)...};
    PyObject pytuple = PyTuple_New(sizeof...(args));
    for (int i = 0; i < sizeof...(args); ++ i){
      PyTuple_SetItem( pytuple, i, pyargs[i]);
    }
    PyObject_CallObject( Pool::pycallbacks[index],
                         pytuple);
    for (int i = 0; i < sizeof...(args); ++ i){
      Py_DECREF(pyargs[i]);
    }
    Py_DECREF(pytuple);
  }  
  

  /*****************************
   ** This code initializes all the callback function 
   ** pointers recursively before main hits
   **/
  template <  const size_t count, 
              const size_t index,
              typename ReturnType,
              typename... Args
              >
  class CBPoolInitializer{
  public:

    CBPoolInitializer(){
      typedef CallbackPool<count, ReturnType, Args...> Pool;
      Pool::c_callbacks[index-1] = &__cb<count, index, ReturnType, Args...>;
      CBPoolInitializer<count, index-1, ReturnType, Args...>();
    }
   
  };
  
  template <  const size_t count, 
            typename ReturnType,
            typename... Args
           >
  class CBPoolInitializer<count, 0, ReturnType, Args...>{
  public:
    typedef CallbackPool<count, ReturnType, Args...> Pool;
    
    CBPoolInitializer(){
    }
    
  };
  
  
  CBPoolInitializer< MAX_CB_POOL_DEPTH, MAX_CB_POOL_DEPTH,
                   int, PyObject, int , double> initializer;



  /***************
   * wrapper to python callback, mapping to C-style callback
   **/
  template<typename ReturnType, typename... Args>
  class PyCallbackWrapper{
  public:
    
    typedef CallbackPool< MAX_CB_POOL_DEPTH, 
                          ReturnType, 
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

}
#endif
