#ifndef __PYLLARS_INTERNAL__CLASSMETHODCALLSEMANTICS_H
#define __PYLLARS_INTERNAL__CLASSMETHODCALLSEMANTICS_H

#include </usr/include/python2.7/Python.h>
#include </usr/include/string.h>
#include </usr/include/sys/types.h>

#include "pyllars_utils.hpp"
#include "pyllars_defns.hpp"
#include "pyllars_conversions.hpp"
#include "pyllars_varargs.hpp"
#include <ffi.h>
/**
* This unit defines template classes needed to contain method pointers and
* define Python-to-C call semantics for invoking class instance methods
**/


namespace __pyllars_internal {

    /**
     * class to hold reference to a class method and define
     * method call semantics
     **/
    template<bool with_ellipsis, typename CClass, typename ReturnType_, typename ... Args>
    class ClassMethodCallSemantics {
    public:

        typedef typename extent_as_pointer<ReturnType_>::type ReturnType;

        template<bool, int>
        struct FuncDef;
        template<int val>
        struct FuncDef<false, val> {
            typedef ReturnType(*func_type)(Args...);
        };
        template<int val>
        struct FuncDef<true, val> {
            typedef ReturnType(*func_type)(Args... ...);
        };

        struct FunctType {
            typedef typename FuncDef<with_ellipsis, 0>::func_type func_type;

            static ReturnType call(func_type func, Args... args, PyObject *extra_args);
        };


        typedef typename FuncDef<with_ellipsis, 0>::func_type method_t;

        static const char *const *kwlist;

        /**
         * Used for regular methods:
         */
        static PyObject *call(method_t method, PyObject *args, PyObject *kwds) ;

    private:

        /**
         * call that invokes method a la C:
         **/
        template<typename ...PyO>
        static ReturnType call_methodC(typename FuncDef<with_ellipsis,0>::func_type method, PyObject *args, PyObject *kwds, PyO *...pyargs);
        /**
         * call that converts python given arguments to make C call:
         **/
        template<int ...S>
        static ReturnType call_methodBase(typename FuncDef<with_ellipsis,0>::func_type ,
                                          PyObject *args, PyObject *kwds, container<S...> s) ;

    };


    /**
     * specialize for void returns:
     **/
    template<bool with_ellipsis, typename CClass, typename ...Args>
    class ClassMethodCallSemantics<with_ellipsis, CClass, void, Args...> {
    public:

        template<bool, int>
        struct FuncDef;

        template<int val>
        struct FuncDef<false, val> {
            typedef void(*func_type)(Args...);
        };
        template<int val>
        struct FuncDef<true, val> {
            typedef void(*func_type)(Args... ...);
        };

        struct FunctType {
            typedef typename FuncDef<with_ellipsis, 0>::func_type func_type;

            static void call(func_type func, Args... args, PyObject *extra_args);
        };


        typedef typename FuncDef<with_ellipsis, 0>::func_type method_t;


        static const char *const *kwlist;

        static PyObject *call(method_t method, PyObject *args, PyObject *kwds) ;

    private:

        template<typename ...PyO>
        static void call_methodC(typename FuncDef<with_ellipsis,0>::func_type method,
                                 PyObject *args, PyObject *kwds,
                                 PyO *...pyargs) ;

        template<int ...S>
        static void call_methodBase(typename FuncDef<with_ellipsis,0>::func_type ,
                                    PyObject *args, PyObject *kwds,
                                    container<S...> unused) ;

    };


     /**
     * This class is needed to prevent ambiguities and compiler issues in add_method
     * It holds the method call and allows specialization based on
     * underlying CClass type
     **/
    template<class CClass, typename E = void>
    class ClassMethodContainer {
    public:

        template<bool with_ellipsis, const char *const name, typename ReturnType, typename ...Args>
        class Container {
            typedef typename extent_as_pointer<ReturnType>::type (*method_t)(Args...);

            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = ClassMethodCallSemantics<with_ellipsis, CClass, ReturnType, Args...>::kwlist;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

        };
    };


    /**
     * Specialization for non-const class types
     **/
    template<class CClass>
    class ClassMethodContainer<CClass, typename std::enable_if<
            std::is_class<CClass>::value && !std::is_const<CClass>::value>::type> {
    public:

        template<bool with_ellipsis, const char *const name, typename ReturnType, typename ...Args>
        class Container {
        public:
            typedef typename ClassMethodCallSemantics<with_ellipsis, CClass, ReturnType, Args...>::method_t method_t;


            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = ClassMethodCallSemantics<with_ellipsis, CClass, ReturnType, Args...>::kwlist;
            static method_t method;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds) ;

        };
    };


    /**
     * Specialization for const class types
     **/
    template<class CClass>
    class ClassMethodContainer<CClass, typename std::enable_if<
            std::is_class<CClass>::value && std::is_const<CClass>::value>::type> {
    public:

        template<bool with_ellipsis, const char *const name, typename ReturnType, typename ...Args>
        class Container {
        public:
            typedef typename ClassMethodCallSemantics<with_ellipsis, CClass, ReturnType, Args...>::method_t method_t;

            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = ClassMethodCallSemantics<with_ellipsis, CClass, ReturnType, Args...>::kwlist;
            static method_t method;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

        };

    };


    /**
     * Class member container
     **/
    template<class CClass>
    class ClassMemberContainer {
    public:

        template<const char *const name, typename T>
        class Container {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T *member_t;

            static member_t member;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

            static void setFromPyObject(PyObject *pyobj);

        };

        template<const char *const name, size_t size, typename T>
        class Container<name, T[size]> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T *member_t[size];

            static member_t member;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

            static void setFromPyObject(PyObject *pyobj) ;
        };
    };

   /**
     * Class member container for const class members
     **/
    template<class CClass>
    class ConstClassMemberContainer {
    public:

        template<const char *const name, typename T>
        class Container {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T const *member_t;

            static member_t member;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

        };

    };

}

#endif // __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2

