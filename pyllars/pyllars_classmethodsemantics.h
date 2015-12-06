#ifndef __PYLLARS_INTERNAL__CLASSMETHODCALLSEMANTICS_H
#define __PYLLARS_INTERNAL__CLASSMETHODCALLSEMANTICS_H
#include "pyllars_utils.h"

/**
* This unit defines template classes needed to contain method pointers and
* define Python-to-C call semantics for invoking class instance methods
**/

namespace __pyllars_internal{

    ///////////
    // Helper conversion functions
    //////////
    template< typename T, bool is_complete, const ssize_t max = -1,  typename E = void>
    PyObject* toPyObject( T &var, const bool asArgument);

    template< typename T, bool is_complete, const ssize_t max = -1,  typename E = void>
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
    class ClassMethodCallSemantics{
    public:
        typedef typename extent_as_pointer<T>::type(*method_t)(Args...);
        static const char* const *kwlist;

        /**
         * Used for regular methods:
         */
        static PyObject* call( method_t method, PyObject* args, PyObject* kwds){
            try{
              return toPyObject<T, true> ( call_methodBase(method,  args, kwds, typename argGenerator<sizeof...(Args)>::type()), false);
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
        static typename extent_as_pointer<T>::type call_methodC( typename extent_as_pointer<T>::type  (*method)(Args...),
                                        PyObject *args, PyObject *kwds, PyO* ...pyargs){
            static char format[sizeof...(Args)+1] = {0};
            if (sizeof...(Args)>0)
                memset( format, 'O', sizeof...(Args));

            if(!PyArg_ParseTupleAndKeywords(args, kwds, format, (char**)kwlist, &pyargs...)){
                PyErr_Print();
                throw "Invalid arguments to method call";
            }
            T retval =  method(*toCObject<Args>(*pyargs)...);
            return retval;
        }

        /**
         * call that converts python given arguments to make C call:
         **/
        template<int ...S>
        static typename extent_as_pointer<T>::type  call_methodBase( typename extent_as_pointer<T>::type  (*method)(Args...),
                                  PyObject* args, PyObject* kwds, container<S...> s) {
            (void)s;
            PyObject pyobjs[sizeof...(Args)+1];
            return call_methodC(method, args, kwds, &pyobjs[S]...);
            (void)pyobjs;
        }

    };

    template< class CClass, typename ReturnType, typename ...Args>
    const char* const *
    ClassMethodCallSemantics<CClass, ReturnType, Args...>::kwlist;


    /**
     * specialize for void returns:
     **/
    template<typename CClass, typename ...Args>
    class ClassMethodCallSemantics<CClass,void, Args...>{
    public:
        typedef void(*method_t)(Args...);
        static const char* const * kwlist;

        static PyObject* call( method_t method, PyObject* args, PyObject* kwds){
            call_methodBase(method,  args, kwds, typename argGenerator< sizeof...(Args) >::type());
            return Py_None;
        }

    private:

        template< typename ...PyO>
        static void call_methodC( void (*method)(Args...),
                                        PyObject* args, PyObject* kwds,
                                        PyO* ...pyargs){

            char format[sizeof...(Args)+1]={0};
            if (sizeof...(Args) > 0)
                memset(format,'O',sizeof...(Args));
            if(!PyArg_ParseTupleAndKeywords(args, kwds, format, (char**)kwlist, &pyargs...)){
                PyErr_SetString( PyExc_RuntimeError, "Failed to parse argument on method call");
            } else {
                method(*toCObject<Args>(*pyargs)...);
            }
        }

        template<int ...S>
        static void call_methodBase( void (*method)(Args...),
                                     PyObject *args, PyObject *kwds,
				     container<S...> unused) {
	     (void)unused;
             PyObject pyobjs[sizeof...(Args)+1];
             call_methodC(method, args, kwds, &pyobjs[S]...);
             (void)pyobjs;
        }

     };


    template< class CClass, typename ...Args>
    const char* const *
    ClassMethodCallSemantics<CClass, void, Args...>::kwlist;


    /**
     * This class is needed to prevent ambiguities and compiler issues in add_method
     * It holds the method call and allows specialization based on
     * underlying CClass type
     **/
    template<  class CClass,  typename E = void>
    class ClassMethodContainer{
    public:

        template<const char* const name, typename ReturnType, typename ...Args>
        class Container{
            typedef typename extent_as_pointer<ReturnType>::type (*method_t)(Args...);

            typedef const char* const * kwlist_t;
            static constexpr  kwlist_t &kwlist = ClassMethodCallSemantics<CClass, ReturnType, Args...>::kwlist;

            static PyObject* call(PyObject* cls, PyObject* args, PyObject* kwds);

        };
    };




    /**
     * Specialization for non-const class types
     **/
    template<class CClass>
    class ClassMethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && !std::is_const<CClass>::value >::type>{
    public:

        template<const char* const name, typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef typename extent_as_pointer<ReturnType>::type(*method_t)(Args...);

            typedef const char* const * kwlist_t;
            static constexpr kwlist_t &kwlist = ClassMethodCallSemantics<CClass, ReturnType, Args...>::kwlist;
            static method_t method;

            static PyObject* call(PyObject* cls, PyObject* args, PyObject* kwds){
                 (void)cls;
                try{
                    return ClassMethodCallSemantics<CClass, ReturnType, Args...>::call(method, args, kwds);
                } catch(...){
                    PyErr_SetString(PyExc_RuntimeError, "Exception in class method");
                    return nullptr;
                }
                return nullptr;
            }

        };
    };


    /**
     * Specialization for const class types
     **/
    template<class CClass>
    class ClassMethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && std::is_const<CClass>::value >::type>{
    public:

        template<const char* const name, typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef typename extent_as_pointer<ReturnType>::type(*method_t)(Args...);

            typedef const char* const * kwlist_t;
            static constexpr kwlist_t &kwlist = ClassMethodCallSemantics<CClass, ReturnType, Args...>::kwlist;
            static method_t method;

            static PyObject* call(PyObject* cls, PyObject* args, PyObject* kwds){
                (void)cls;
                try{
                    return ClassMethodCallSemantics<CClass, ReturnType, Args...>::call(method, args, kwds);
                } catch(...){
                    PyErr_SetString(PyExc_RuntimeError,"Exception in class method");
                    return nullptr;
                }
                return nullptr;
            }

        };
    };

    template< class CClass>
    template< const char* const name, typename ReturnType, typename ...Args>
    typename ClassMethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && !std::is_const<CClass>::value >::type>::template Container<name, ReturnType, Args...>::method_t
    ClassMethodContainer< CClass, typename std::enable_if< std::is_class<CClass>::value && !std::is_const<CClass>::value >::type>::Container<name, ReturnType, Args...>::method;


    template< class CClass>
    template< const char* const name, typename ReturnType, typename ...Args>
    typename ClassMethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && std::is_const<CClass>::value >::type>::template Container<name, ReturnType, Args...>::method_t
    ClassMethodContainer< CClass, typename std::enable_if< std::is_class<CClass>::value && std::is_const<CClass>::value >::type>::Container<name, ReturnType, Args...>::method;




    /**
     * Class member container
     **/
    template<class CClass>
    class ClassMemberContainer{
    public:

        template<const char* const name, typename T>
        class Container{
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T * member_t;

            static member_t member;

            static PyObject* call(PyObject* cls, PyObject* args, PyObject* kwds){
                (void)cls;
                return toPyObject<T,true>(member, false);
            }

            static void setFromPyObject(PyObject* pyobj){
                 member = *toCObject<T>(*pyobj);
            }
        };

          template<const char* const name, size_t size, typename T>
        class Container<name, T[size]>{
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T * member_t[size];

            static member_t member;

            static PyObject* call(PyObject* cls, PyObject* args, PyObject* kwds){
                (void)cls;
                return toPyObject<T,true>(member, false);
            }

            static void setFromPyObject(PyObject* pyobj){
                 T val[] = *toCObject<T[size]>(*pyobj);
                 for(size_t i = 0; i < size; ++i)member[i] = val[i];
            }
        };
    };

    template< class CClass>
    template< const char* const name, typename T>
    typename ClassMemberContainer<CClass>::template Container< name, T>::member_t
    ClassMemberContainer<CClass>::Container< name, T>::member;






    /**
     * Class member container for const class members
     **/
    template<class CClass>
    class ConstClassMemberContainer{
    public:

        template<const char* const name, typename T>
        class Container{
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T const * member_t;

            static member_t member;

            static PyObject* call(PyObject* cls, PyObject* args, PyObject* kwds){
                (void)cls;
                return toPyObject<T,true>(member, false);
            }

        };


    };

    template< class CClass>
    template< const char* const name, typename T>
    typename ConstClassMemberContainer<CClass>::template Container< name, T>::member_t
    ConstClassMemberContainer<CClass>::Container< name, T>::member;





}

#endif // __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2

