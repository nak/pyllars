#ifndef __PYLLARS_INTERNAL__CALLBACKS_H
#define __PYLLARS_INTERNAL__CALLBACKS_H

#ifndef MAX_CB_POOL_DEPTH
#define MAX_CB_POOL_DEPTH 898
#endif

#include "pyllars/pyllars_conversions.h"
#include "pyllars/pyllars_classwrapper.h"

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

        static PyObject *pycallbacks[count+1];
        static callback_t c_callbacks[count+1];

      };




      template < const size_t count,
                 typename ReturnType,
                 typename... Args>
      PyObject* CallbackPool< count, ReturnType, Args...>::pycallbacks[count+1];

      template < const size_t count,
                 typename ReturnType,
                 typename... Args>
      typename CallbackPool< count, ReturnType, Args...>::callback_t
      CallbackPool< count, ReturnType, Args...>::c_callbacks[count+1];


      __attribute__((noinline)) static PyObject* __pycb( PyObject *pycallback, PyObject* pyargs[], const Py_ssize_t size) {
        if(!pycallback || !PyCallable_Check(pycallback)){
           PyErr_SetString( PyExc_RuntimeError,
                                "Python callback is not callable!");
           throw "Python callback is not callable!";
        }
        PyObject *pytuple = PyTuple_New(size);
        for (Py_ssize_t i = 0; i < size; ++ i){
          PyTuple_SetItem( pytuple, i, pyargs[i]);
        }
        PyObject* result = PyObject_CallObject( pycallback, pytuple);
                   for (Py_ssize_t i = 0; i < size; ++i){
              Py_DECREF(pyargs[i]);
        }
        Py_DECREF(pytuple);
        return result;
      }


      template < typename ReturnType,
                 typename... Args>
      class CBHelper{
      public:
          __attribute__((noinline)) static ReturnType __cbBase( PyObject* const  pycallback,  Args...  args) {
            PyObject *pyargs[] = { toPyObject(args, true)...};
            PyObject* result =__pycb( pycallback, pyargs, sizeof...(args));
            if (!result){
              throw "Invalid arguments or error in callback";
            } else if( std::is_pointer<ReturnType>::value && result == Py_None){
              ReturnType retval;
              memset(&retval, 0, sizeof(retval)); //null pointer, but this generic type, so....
              return retval;
            } else if( result == Py_None){
              throw "Invalid return type from callback";
            }
             return toCObject<ReturnType>(*result);
        }
     };

      template < typename... Args>
      class CBHelper<void, Args...>{
      public:
           __attribute__((noinline))  static void __cbBase( PyObject* const  pycallback,  Args...  args) {
            PyObject *pyargs[] = { toPyObject(args, true)...};
            PyObject* result =__pycb( pycallback, pyargs, sizeof...(args));
            if (!result){
              throw "Invalid arguments or error in callback";
            }
        }
     };




   template < const size_t count,
                 const size_t index,
                 typename ReturnType,
                 typename... Args>
    class CBContainer: public CBHelper<ReturnType, Args...>{
    public:

        static ReturnType __cb( Args...  args){
            //only chain to a lower level class with fewer instanitaions
            return CBHelper<ReturnType, Args...>::__cbBase(CallbackPool<count, ReturnType, Args...>::pycallbacks[index], args...);
        }
    };

    /**
    * specialize for void return
    **/
    template < const size_t count,
               const size_t index ,
               typename... Args >
    class CBContainer<count, index, void, Args...>:public CBHelper<void, Args...>{
    public:
        static void __cb( Args...  args){
            return CBHelper<void, Args...>::__cbBase(CallbackPool<count, void, Args...>::pycallbacks[index], args...);
        }
    };

    /*****************************
    ** This code initializes all the callback function
    ** pointers recursively before main hits
    **/
    template < const size_t count,
               const size_t index,
               typename ReturnType,
               typename... Args >
    class CBPoolInitializer{
    public:

        CBPoolInitializer(){
            typedef CallbackPool<count, ReturnType, Args...> Pool;
            Pool::c_callbacks[index-1] = &CBContainer<count, index, ReturnType, Args...>::__cb;
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
                   void, PyObject, int , double> initializer;




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
