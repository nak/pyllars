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
     *
     * @param with ellipsis: whether method has ellipsis
     * @param ReturnType:  ReturnType of the method (can be "void")
     * @param Args: template variable arg list of method arguments (can be empty)
     **/
    template<bool with_ellipsis, typename CClass, const char* const kwlist[], typename ReturnType_, typename ... Args>
    class ClassMethodCallSemantics {
    public:

        typedef typename extent_as_pointer<ReturnType_>::type ReturnType;

        template<bool has_ellipsis, int no_args>
        struct FuncDef;

        template<int val>
        struct FuncDef<false, val> { // no ellipsis
            typedef ReturnType(*func_type)(Args...);
        };

        template<int val>
        struct FuncDef<true, val> { // with ellipsis
            typedef ReturnType(*func_type)(Args... ...);
        };

        /**
         * Type to hold a callable "function".
         */
        struct FunctType {
            typedef typename FuncDef<with_ellipsis, 0>::func_type func_type;

            static ReturnType call(func_type func, argument_capture<Args>... args, PyObject *extra_args);
        };

        /**
         * the 0-arg "based" case.  this uses somewhat of a trick to unroll
         * the method's arguments into a template definition throug var-args
         * based on the types of arguments defined for the method
         */
        typedef typename FuncDef<with_ellipsis, 0>::func_type method_t;

        /**
         * Used for regular methods:
         */
        static PyObject *call(method_t method, PyObject *args, PyObject *kwds) ;

    private:

        /**
         * call that invokes method a la C:
         **/
        template<typename ...PyO>
        static ReturnType call_methodC(typename FuncDef<with_ellipsis,0>::func_type method, PyObject *args,
                                       PyObject *kwds, PyO *...pyargs);

        static ReturnType methodForward(method_t method, argument_capture<Args> ...args){
            return method(args.value()...);
        }

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
    template<bool with_ellipsis, typename CClass, const char* const kwlist[], typename ...Args>
    class ClassMethodCallSemantics<with_ellipsis, CClass, kwlist, void, Args...> {
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

            static void call(func_type func, argument_capture<Args>... args, PyObject *extra_args);
        };


        typedef typename FuncDef<with_ellipsis, 0>::func_type method_t;

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

    template<class CClass>
    class ClassMethodContainer {
    public:

        template<bool with_ellipsis, const char *const name, const char* const kwlist[], typename ReturnType, typename ...Args>
        class Container {
        public:
            typedef typename ClassMethodCallSemantics<with_ellipsis, CClass, kwlist, ReturnType, Args...>::method_t method_t;

            static method_t method;

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds) ;

        };
    };

}

#endif // __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2

