#ifndef __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2
#define __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2
#include "pyllars_utils.h"

/**
* This unit defines template classes needed to contain method pointers and
* define Python-to-C call semantics for invoking class instance methods
**/

namespace __pyllars_internal{

    ///////////
    // Helper conversion functions
    //////////
    template< typename T, const ssize_t max = -1,  typename E = void>
    PyObject* toPyObject( T &var, const bool asArgument);

    template< typename T, const ssize_t max = -1,  typename E = void>
    PyObject* toPyObject( const T &var, const bool asArgument);

    template< typename T>
    smart_ptr<T> toCObject( PyObject& pyobj );

    struct PythonBase;

}

namespace __pyllars_internal{



    /**
     * class to hold referecne to a class method and define
     * method call semantics
     **/
    template<typename CClass, typename T, typename ... Args>
    class MethodCallSemantics{
    public:
        typedef typename extent_as_pointer<T>::type(CClass::*method_t)(Args...);
        typedef T CClass::* member_t;
        static member_t member;
        static const char* const *kwlist;

        /**
         * if call is a "get_" member element call:
         **/
        static PyObject* toPyObj(CClass &self){
            return toPyObject<T>(self.*member, false);
        }

        /**
         * Used for regular methods:
         */
        static PyObject* call( method_t method, CClass & self, PyObject* args, PyObject* kwds){
            try{
              return toPyObject( call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type()), false);
            } catch( const char* const msg){
                PyErr_SetString( PyExc_RuntimeError, msg);
                return  nullptr;
            }
        }

    private:

        /**
         * call that invokes method a la C:
         **/
        template< typename ...PyO>
        static typename extent_as_pointer<T>::type call_methodC( typename extent_as_pointer<T>::type  (CClass::*method)(Args...),
                                        typename std::remove_reference<CClass>::type &self,
                                        PyObject *args, PyObject *kwds, PyO* ...pyargs){
            static char format[sizeof...(Args)+1] = {0};
            if (sizeof...(Args)>0)
                memset( format, 'O', sizeof...(Args));

            if(!PyArg_ParseTupleAndKeywords(args, kwds, format, (char**)kwlist, &pyargs...)){
                PyErr_Print();
                throw "Invalid arguments to method call";
            }
            T retval =  (self.*method)(*toCObject<Args>(*pyargs)...);
            return retval;
        }

        /**
         * call that converts python given arguments to make C call:
         **/
        template<int ...S>
        static typename extent_as_pointer<T>::type  call_methodBase( typename extent_as_pointer<T>::type  (CClass::*method)(Args...),
                                  typename std::remove_reference<CClass>::type &self,
                                  PyObject* args, PyObject* kwds, container<S...> s) {
            (void)s;
            PyObject pyobjs[sizeof...(Args)+1];
            return call_methodC(method, self, args, kwds, &pyobjs[S]...);
            (void)pyobjs;
        }

    };

    template< class CClass, typename ReturnType, typename ...Args>
    const char* const *
    MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;


    /**
     * specialize for void returns:
     **/
    template<typename CClass, typename ...Args>
    class MethodCallSemantics<CClass,void, Args...>{
    public:
        typedef void(CClass::*method_t)(Args...);
        typedef void* member_t;
        static member_t member;
        static const char* const * kwlist;

        static PyObject* toPyObj(CClass & self){
            (void)self;
            return Py_None;
        }
        static PyObject* call( method_t method, CClass & self, PyObject* args, PyObject* kwds){
	  call_methodBase(method, self, args, kwds, typename argGenerator< sizeof...(Args) >::type());
            return Py_None;
        }

    private:

        template< typename ...PyO>
        static void call_methodC( void (CClass::*method)(Args...),
                                        typename std::remove_reference<CClass>::type &self,
                                        PyObject* args, PyObject* kwds,
                                        PyO* ...pyargs){

            char format[sizeof...(Args)+1]={0};
            if (sizeof...(Args) > 0)
                memset(format,'O',sizeof...(Args));
            if(!PyArg_ParseTupleAndKeywords(args, kwds, format, (char**)kwlist, &pyargs...)){
                PyErr_SetString( PyExc_RuntimeError, "Failed to parse argument on method call");
            } else {
                (self.*method)(*toCObject<Args>(*pyargs)...);
            }
        }

        template<int ...S>
        static void call_methodBase( void (CClass::*method)(Args...),
                                     typename std::remove_reference<CClass>::type &self,
                                     PyObject *args, PyObject *kwds,
				     container<S...> unused) {
	     (void)unused;
             PyObject pyobjs[sizeof...(Args)+1];
             call_methodC(method, self, args, kwds, &pyobjs[S]...);
             (void)pyobjs;
        }

     };

    template< class CClass, typename T, typename ...Args>
    typename MethodCallSemantics<CClass, T, Args...>::member_t
    MethodCallSemantics<CClass, T, Args...>::member;

    template< class CClass, typename ...Args>
    typename MethodCallSemantics<CClass, void, Args...>::member_t
    MethodCallSemantics<CClass, void, Args...>::member;

    template< class CClass, typename ...Args>
    const char* const *
    MethodCallSemantics<CClass, void, Args...>::kwlist;


    /**
     * This class is needed to prevent ambiguities and compiler issues in add_method
     * It holds the method call/member getter method and allows specialization based on
     * underlying CClass type
     **/
    template<  class CClass,  typename E = void>
    class MethodContainer{
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type * , PyObject*);

        template<const char* const name, typename ReturnType, typename ...Args>
        class Container{
	  typedef typename extent_as_pointer<ReturnType>::type TrueReturnType;
	  typedef TrueReturnType(CClass::*method_t)(Args...);
           typedef ReturnType CClass::* member_t;

            typedef const char* const * kwlist_t;
           static constexpr  kwlist_t &kwlist = MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;

           static PyObject* call(PyObject* self, PyObject* args, PyObject* kwds);

        };
    };




    /**
     * Specialization for non-const class types
     **/
    template<class CClass>
    class MethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && !std::is_const<CClass>::value >::type>{
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type * , PyObject*);

        template<const char* const name, typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef typename extent_as_pointer<ReturnType>::type(CClass::*method_t)(Args...);
            typedef typename MethodCallSemantics<CClass, ReturnType>::member_t member_t;

            typedef const char* const * kwlist_t;
            static constexpr kwlist_t &kwlist = MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;
            static method_t method;
            static member_t constexpr &member = MethodCallSemantics<CClass, ReturnType, Args...>::member;

            static PyObject* call(PyObject* self, PyObject* args, PyObject* kwds){
                if(!self) return nullptr;
                PythonClassWrapper<CClass>* _this = (PythonClassWrapper<CClass>*)self;
                if(_this->get_CObject()){
                    if (method){
                        try{
                            return MethodCallSemantics<CClass, ReturnType, Args...>::call(method, *_this->get_CObject(),args, kwds);
                        } catch(...){
                            return nullptr;
                        }
                    } else if(member){
                        return MethodCallSemantics<CClass, ReturnType, Args...>::toPyObj(*_this->get_CObject());
                    }
                }
                return nullptr;
            }
            static void setFromPyObject( typename std::remove_reference<CClass>::type * self, PyObject* pyobj){
                 self->*member = *toCObject<CClass>(*pyobj);
            }
        };
    };


    /**
     * Specialization for const class types
     **/
    template<class CClass>
    class MethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && std::is_const<CClass>::value >::type>{
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type * , PyObject*);

        template<const char* const name, typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef typename extent_as_pointer<ReturnType>::type(CClass::*method_t)(Args...);
            typedef typename MethodCallSemantics<CClass, ReturnType>::member_t member_t;

            typedef const char* const * kwlist_t;
            static constexpr kwlist_t &kwlist = MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;
            static method_t method;
            static constexpr member_t &member = MethodCallSemantics<CClass, ReturnType, Args...>::member;

            static PyObject* call(PyObject* self, PyObject* args, PyObject* kwds){
                if(!self) return nullptr;
                PythonClassWrapper<CClass>* _this = (PythonClassWrapper<CClass>*)self;
                if(_this->get_CObject()){
                    if (method){
                        try{
                            return MethodCallSemantics<CClass, ReturnType, Args...>::call(method, *_this->get_CObject(),args, kwds);
                        } catch(...){
                            return nullptr;
                        }
                    } else if(member){
                        return MethodCallSemantics<CClass, ReturnType, Args...>::toPyObj(*_this->get_CObject());
                    }
                }
                return nullptr;
            }
            static void setFromPyObject( typename std::remove_reference<CClass>::type * self, PyObject* pyobj){
               PyErr_SetString(PyExc_TypeError, "Attempt to set const field in C class object");
               PyErr_Print();
            }
        };
    };

    template< class CClass>
    template< const char* const name, typename ReturnType, typename ...Args>
    typename MethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && !std::is_const<CClass>::value >::type>::template Container<name, ReturnType, Args...>::method_t
     MethodContainer< CClass, typename std::enable_if< std::is_class<CClass>::value && !std::is_const<CClass>::value >::type>::Container<name, ReturnType, Args...>::method;


    template< class CClass>
    template< const char* const name, typename ReturnType, typename ...Args>
    typename MethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && std::is_const<CClass>::value >::type>::template Container<name, ReturnType, Args...>::method_t
     MethodContainer< CClass, typename std::enable_if< std::is_class<CClass>::value && std::is_const<CClass>::value >::type>::Container<name, ReturnType, Args...>::method;


    /**
     * Specialization for integral types
     **/
    template<typename CClass>
    class MethodContainer< CClass, typename std::enable_if< std::is_integral<CClass>::value>::type>{
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type * , PyObject*);

        template< const char* const name, typename ReturnType, typename ...Args>
        class Container{
           typedef int member_t;
           static PyObject* call(PyObject*, PyObject*, PyObject*){
                return nullptr;
           }
        };
    };

    /**
     * Specialization for floating point types
     **/
   template< typename CClass>
    class MethodContainer<  CClass, typename std::enable_if< std::is_floating_point<CClass>::value>::type>{
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type * , PyObject*);

        template< const char* const, typename ReturnType, typename ...Args>
        class Container{
           typedef int member_t;
           static PyObject* call(PyObject*, PyObject*, PyObject*){
                return nullptr;
           }
        };
    };



}

#endif // __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2

