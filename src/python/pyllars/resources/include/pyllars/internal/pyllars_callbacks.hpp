#ifndef __PYLLARS_INTERNAL__CALLBACKS_H
#define __PYLLARS_INTERNAL__CALLBACKS_H

#ifndef MAX_CB_POOL_DEPTH
#define MAX_CB_POOL_DEPTH 800
#endif

#include <Python.h>
#include <functional>

#include "pyllars_classwrapper.hpp"

namespace __pyllars_internal{

        template<typename T>
        T toCObject(PyObject* pyobj);

        template <typename ReturnType, typename ...Args>
        struct CallbackContainer;

        template <typename ReturnType, typename ...Args>
        struct CallbackContainer<ReturnType(Args...)>{

            template<std::function<ReturnType(Args...)> **func>
            struct StaticInstantiation {

                static ReturnType callback(Args ...args) {
                    if (!func) {
                        throw PyllarsException(PyExc_SystemError, "Internal error: No callback defn defined");
                    }
                    return (*func)(args...);
                }


                static std::function<ReturnType(Args...)> *initialize(PyObject *callable) {
                    auto cfunc = new std::function<ReturnType(Args...)>(
                            [callable](Args ...args) -> ReturnType {
                                PyObject pyargs_array[] = {toPyArgument(args)...};
                                auto pyargs = PyTuple_New(sizeof...(args));
                                for (size_t i = 0; i < sizeof...(args); ++i) {
                                    PyTuple_SetItem(pyargs, i, pyargs_array[i]);
                                }
                                PyObject *ret = PyObject_Call(callable, pyargs, nullptr);
                                if (!ret) {
                                    throw PyllarsException(PyExc_SystemError, "Error in call to python callback-wrapper");
                                }
                                if constexpr (std::is_void<ReturnType>::value) {
                                    return;
                                } else {
                                    return toCObject<ReturnType>(ret);
                                }
                            }
                    );
                    *func = cfunc;
                    return *func;
                }
            };
        };

      /**
       * template class for a pool of C-style callbacks
       * mapped to python-function-callbacks.  There is
       * probably some magic way through ffi to do this,
       * but didn't want to go that route just yet.  so a
       * pool of callbacks are available when requested.
       **/
      template <  typename ReturnType,
                  typename... Args >
      class CallbackPool{
      public:

        typedef ReturnType(*callback_t)( Args...);
	    typedef ReturnType(*callbackvar_t)( Args...,...);

        static PyObject *pycallbacks[MAX_CB_POOL_DEPTH+1];
        static callback_t c_callbacks[MAX_CB_POOL_DEPTH+1];
        static callbackvar_t c_callbacksvar[MAX_CB_POOL_DEPTH+1];

      };

      __attribute__((noinline)) static PyObject* __pycb( PyObject *pycallback, PyObject* pyargs[], const Py_ssize_t size) {
        if(!pycallback || !PyCallable_Check(pycallback)){
            PyErr_SetString( PyExc_RuntimeError,
                                "Python callback is not callable!");
            PyErr_Print();
            throw PyllarsException(PyExc_TypeError, "Python callback is not callable!");
        }
        PyObject *pytuple = PyTuple_New(size);
        for (Py_ssize_t i = 0; i < size; ++ i){
          PyTuple_SET_ITEM( pytuple, i, pyargs[i]);
        }
        PyObject* result = PyObject_Call( pycallback, pytuple, nullptr);
        for (Py_ssize_t i = 0; i < size; ++i){
              Py_DECREF(pyargs[i]);
        }
        Py_DECREF(pytuple);

        if (!result){
            throw PyllarsException(PyExc_TypeError, "Invalid arguments or error in callback");
        }
        return result;
      }


      template < typename ReturnType,
                 typename... Args>
      class CBHelper{
      public:
          __attribute__((noinline)) static ReturnType __cbBase( PyObject* const  pycallback,  Args...  args) {
          (void)__pycb;
            PyObject *pyargs[] = { toPyObject<Args>(args, true, std::extent<Args>::value)...};
            PyObject* result =__pycb( pycallback, pyargs, sizeof...(args));
            if( std::is_pointer<ReturnType>::value && (result == Py_None)){
              ReturnType retval;
              memset(&retval, 0, sizeof(retval)); //null pointer, but this is generic type, so....
              return retval;
            } else if( result == Py_None){
              throw PyllarsException(PyExc_TypeError, "Invalid return type from callback");
            }
             return *toCArgument<ReturnType, false, PythonClassWrapper<ReturnType> >(*result);
        }
     };

      template < typename... Args>
      class CBHelper<void, Args...>{
      public:
           __attribute__((noinline))  static void __cbBase( PyObject* const  pycallback,  Args...  args) {
            PyObject *pyargs[] = { toPyObject<Args>(args, true,  std::extent<Args>::value)...};
            __pycb( pycallback, pyargs, sizeof...(args));
        }
     };

   template <  const size_t index,
               typename ReturnType,
               typename... Args
              >
    class CBContainer: public CBHelper<ReturnType, Args...>{
    public:

        static ReturnType __cb( Args...  args){
            //only chain to a lower level class with fewer instanitaions
            return CBHelper<ReturnType, Args...>::__cbBase(CallbackPool<ReturnType, Args...>::pycallbacks[index], args...);
        }

      static ReturnType __cbvar( Args...  args,...){
            //only chain to a lower level class with fewer instanitaions
            return CBHelper<ReturnType, Args...>::__cbBase(CallbackPool<ReturnType, Args...>::pycallbacks[index], args...);
        }
    };

    /**
    * specialize for void return
    **/
    template < const size_t index ,
               typename... Args >
    class CBContainer<index, void, Args...>:public CBHelper<void, Args...>{
    public:
        static void __cb( Args...  args){
            return CBHelper<void, Args...>::__cbBase(CallbackPool< void, Args...>::pycallbacks[index], args...);
        }
        static void __cbvar( Args...  args,...){
            return CBHelper<void, Args...>::__cbBase(CallbackPool< void, Args...>::pycallbacks[index], args...);
        }
    };

    /*****************************
    ** This code initializes all the callback function
    ** pointers recursively before main hits
    **/
    template < const size_t index,
               typename ReturnType,
               typename... Args >
    class CBPoolInitializer{
    public:

        CBPoolInitializer(){
            typedef CallbackPool<ReturnType, Args...> Pool;
            Pool::c_callbacks[index] = &CBContainer<index, ReturnType, Args...>::__cb;
            Pool::c_callbacksvar[index] = &CBContainer<index, ReturnType, Args...>::__cbvar;
            CBPoolInitializer<index-1, ReturnType, Args...>();
        }

        };

    template <  typename ReturnType, typename... Args >
    class CBPoolInitializer<0, ReturnType, Args...>{
    public:
        typedef CallbackPool<ReturnType, Args...> Pool;

        CBPoolInitializer(){
            Pool::c_callbacks[0] = &CBContainer< 0, ReturnType, Args...>::__cb;
            Pool::c_callbacksvar[0] = &CBContainer< 0, ReturnType, Args...>::__cbvar;
        }

    };


    /***************
    * wrapper to python callback, mapping to C-style callback
    **/
    template<typename ReturnType, typename... Args>
    class PyCallbackWrapper{
    public:

        typedef CallbackPool< ReturnType, Args...> Pool;

        explicit PyCallbackWrapper( PyObject* const pycb){
            static CBPoolInitializer<MAX_CB_POOL_DEPTH, ReturnType, Args...> initializer;
            for (size_t index = 0 ; index < cb_index; ++index){
                if (pycb == Pool::pycallbacks[index])
                    _my_cb_index = index;
            }
            if (cb_index >= MAX_CB_POOL_DEPTH){
                throw PyllarsException(PyExc_ValueError, "Callbaks exhausted");
            }
            Pool::pycallbacks[cb_index] = pycb;
            Py_INCREF(pycb);
            _my_cb_index = cb_index++;
        }


        typename Pool::callback_t get_C_callback(){
            return Pool::c_callbacks[_my_cb_index];
        }



    private:
        //static volatile CBPoolInitializer<MAX_CB_POOL_DEPTH, ReturnType, Args...> initializer;
        static size_t cb_index ;
        size_t _my_cb_index;

    };
    template<typename ReturnType, typename... Args>
    size_t PyCallbackWrapper<ReturnType, Args...>::cb_index = 0;


    /***************
    * wrapper to python callback, mapping to C-style callback
    **/
    template<typename ReturnType, typename... Args>
    class PyCallbackWrapperVar{
    public:
        typedef CallbackPool< ReturnType, Args...> Pool;

        explicit  PyCallbackWrapperVar( PyObject* const pycb){
            static CBPoolInitializer<MAX_CB_POOL_DEPTH, ReturnType, Args...> initializer;
            for (size_t index = 0 ; index < cb_index; ++index){
                if (pycb == Pool::pycallbacks[index])
                    _my_cb_index = index;
            }
            if (cb_index >= MAX_CB_POOL_DEPTH){
                throw PyllarsException(PyExc_ValueError, "Callbaks exhausted");
            }
            Pool::pycallbacks[cb_index] = pycb;
            Py_INCREF(pycb);
            _my_cb_index = cb_index++;
        }


        typename Pool::callbackvar_t get_C_callback(){
            return Pool::c_callbacksvar[_my_cb_index];
        }

    private:
        static size_t cb_index ;
        size_t _my_cb_index;

    };
    template<typename ReturnType, typename... Args>
    size_t PyCallbackWrapperVar<ReturnType, Args...>::cb_index = 0;



}
#endif
