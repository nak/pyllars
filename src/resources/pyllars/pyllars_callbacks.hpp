#ifndef __PYLLARS_INTERNAL__CALLBACKS_H
#define __PYLLARS_INTERNAL__CALLBACKS_H

#ifndef MAX_CB_POOL_DEPTH
#define MAX_CB_POOL_DEPTH 800
#endif

#include <Python.h>
#include <map>
#include <vector>

//#include "pyllars_conversions.hpp"
#include "pyllars_classwrapper.hpp"

namespace __pyllars_internal{

      /**
       * Non-template base class
       */
      class CallbackBase{
      public:

          __attribute__((noinline)) static PyObject* __pycb( PyObject *pycallback, PyObject* pyargs[], const Py_ssize_t size) {
              if(!pycallback || !PyCallable_Check(pycallback)){
                  PyErr_SetString( PyExc_RuntimeError,
                                   "Python callback is not callable!");
                  PyErr_Print();
                  throw "Python callback is not callable!";
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
                  PyErr_Print();
                  throw "Invalid arguments or error in callback";
              }
              return result;
          }

          static std::vector<PyObject*> pyCallbackList;
      };


      template< typename ReturnType,
                typename Args...>
      class CBHelper: protected  CallbackBase{
      public:
          typedef ReturnType(*callback_t)(Args...);

          callback_t get_C_callback(){
              return _cb;
          }

          static CBHelper* create();

      private:

          __attribute__((noinline)) static ReturnType __cbBase( PyObject* const  pycallback,  Args...  args) {
              PyObject *pyargs[] = { toPyObject<Args>(args, true, std::extent<Args>::value)...};
              PyObject* result =__pycb( pycallback, pyargs, sizeof...(args));
              if( std::is_pointer<ReturnType>::value && (result == (PyObject*)Py_None)){
                  ReturnType retval;
                  memset(&retval, 0, sizeof(retval)); //null pointer, but this is generic type, so....
                  return retval;
              } else if( result == (PyObject*)Py_None){
                  throw "Invalid return type from callback";
              }
              return *toCObject<ReturnType, false, PythonClassWrapper<ReturnType> >(*result);
          }

          callback_t _cb;
          static CBHelper* _cbContainer;
      };


    template<typename ReturnType, typename Args...>
    CBHelper<ReturnType, Args...>* CBHelper<ReturnType, Args...>::_cbContainer = std::nullptr;


    template< typename Args...>
    class CBHelper<void, Args...>: protected  CallbackBase{
    public:
        typedef void(*callback_t)(Args...);

        callback_t get_C_callback(){
            return _cb;
        }

        static CBHelper* create();

    private:
        __attribute__((noinline))  static void __cbBase( PyObject* const  pycallback,  Args...  args) {
            PyObject *pyargs[] = { toPyObject<Args>(args, true,  std::extent<Args>::value)...};
            __pycb( pycallback, pyargs, sizeof...(args));
        }

        callback_t  _cb;

        static CBHelper* _cbContainer;
    };

    template<typename Args...>
    CBHelper<Args...>* CBHelper<Args...>::_cbContainer = std::nullptr;

      template < const size_t index,
                 typename ReturnType,
                 typename... Args>
      class CBContainer: protected CBHelper<index, ReturnType, Args...>{
      public:


          CBContainer(PyObject* pyCallbackObject) {
              pyCallbackList[index] = pyCallbackObject;
              _cb = [](Args... args)->ReturnType{
                  return __cbBase(pyCallbackList[index], args...);
              };
           }

          static  CBContainer<index-1, ReturnType, Args...>* const next;
      };

    template< int index,
              typename ReturnType,
              typename... Args>
    CBContainer<index-1, ReturnType, Args...>* const CBContainer<index, ReturnType, Args...>::next =
    new CBContainer<index-1, ReturnType, Args...>();

    template < typename ReturnType,
               typename... Args>
    class CBContainer<0, ReturnType, Args...>: protected CBHelper<0, ReturnType, Args...>{
    public:


        CBContainer(PyObject* pyCallbackObject) {
            pyCallbackList[index] = pyCallbackObject;
            _cb = [](Args... args)->ReturnType{
                return __cbBase(pyCallbackList[0], args...);
            };
        }
        static  CBContainer<index-1, ReturnType, Args...>* const next;
    };

    template< typename ReturnType,
              typename... Args>
    CBContainer<0, ReturnType, Args...>* const CBContainer<index, ReturnType, Args...>::next = std::nullptr;

    /**
       * Specialize for void returns
       */
      template < cosnt size_t index,
                 typename... Args>
      class CBContainer<index, void, Args...>: protected CBHelper<void, Args...>{
      public:
          typedef void(*callback_t)(Args...);

          CBContainer(PyObject * pyCallbackObject){
              _cb = [](Args... args){
                  __cbBase(pyCallbackList[index], args...)
              };
          }

          static const CBContainer<index-1, void, Args...>* const next;
      };


    template< size_t index,
              typename... Args>
    CBContainer<index-1, void, Args...>* const CBContainer<index, void, Args...>::next =
    new CBContainer<index-1, void, Args...>;

    template < typename... Args>
    class CBContainer<0, void, Args...>: protected CBHelper<void, Args...>{
    public:
        typedef void(*callback_t)(Args...);

        CBContainer(PyObject * pyCallbackObject){
            _cb = [](Args... args){
                __cbBase(pyCallbackList[0], args...)
            };
        }
        static const CBContainer<index-1, void, Args...>* next;

    };

    template< size_t index,
            typename... Args>
    CBContainer<index-1, void, Args...>* const CBContainer<index, void, Args...>::next = std::nullptr;

    template<typename ReturnType, typename Args...>
    CBHelper<ReturnType, Args...>* CBHelper<ReturnType, Args...>::create(){
        if( !CBHelper::_cbContainer){
            CBHelper::_cbContainer = new CBContainer<MAX_CB_POOL_DEPTH, ReturnType, Args...>();
            return CBHelper::_cbContainer;
        }
        if (!CBHelper::_cbContainer->next){
            throw "Maximum depth of callbacks reached";
        }
        auto value = CBHelper::_cbContainer->next;
        CBHelper::_cbContainer->next = CBHelper::_cbContainer->next->next;
        return value;
    };

    template<typename Args...>
    CBHelper<void, Args...>* CBHelper<ReturnType, Args...>::create(){
        if( !CBHelper::_cbContainer){
            CBHelper::_cbContainer = new CBContainer<MAX_CB_POOL_DEPTH, void, Args...>();
            return CBHelper::_cbContainer;
        }
        if (!CBHelper::_cbContainer->next){
            throw "Maximum depth of callbacks reached";
        }
        auto value = CBHelper::_cbContainer->next;
        CBHelper::_cbContainer->next = CBHelper::_cbContainer->next->next;
        return value;
    };

    /**
     * wrapper to python callback, called from C-style callback
     **/
    template<typename ReturnType, typename... Args>
    class PyCallbackWrapper{
    public:

        PyCallbackWrapper( PyObject* const pycb){
            _cbContainer = CBHelper<ReturnType, Args...>::create();
            Py_INCREF(pycb);
        }


        typename CBHelper<ReturnType, Args...>::callback_t get_C_callback(){
            return _cbContainer->get_C_callback();
        }

    private:
        CBContainer<ReturnType, Args...>* _cbContainer;
    };

}
#endif
