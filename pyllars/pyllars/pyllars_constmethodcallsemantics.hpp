#ifndef __PYLLARS_INTERNAL__CONSTMETHODCALLSEMANTICS_H
#define __PYLLARS_INTERNAL__CONSTMETHODCALLSEMANTICS_H

#include <Python.h>

#include "pyllars_defns.hpp"
#include "pyllars_utils.hpp"
/**
* This unit defines template classes needed to contain CONSTANT method  and member pointers and
* define Python-to-C call semantics for invoking class instance methods
**/

namespace __pyllars_internal {

    /////
    // The call semantics are separated into another class to perform specialiation at the proper
    // (lowest) level.  There may be a cleaner solution here
    /////

    /**
     * class to hold reference to a class method and define
     * method call semantics
     **/
    template<typename CClass, typename T, typename ... Args>
    /*private*/
    class ConstMethodCallSemantics {
    public:
        typedef typename extent_as_pointer<T>::type(CClass::*method_t)(Args...) const;

        static const char *const *kwlist;

        /**
         * Used for regular methods:
         */
        static PyObject *call(method_t method, CClass &self, PyObject *args, PyObject *kwds);

    private:

        /**
         * call that invokes method a la C:
         **/
        template<typename ...PyO>
        static typename extent_as_pointer<T>::type call_methodC(
                typename extent_as_pointer<T>::type  (CClass::*method)(Args...) const,
                typename std::remove_reference<CClass>::type &self,
                PyObject *args, PyObject *kwds, PyO *...pyargs);

        /**
         * call that converts python given arguments to make C call:
         **/
        template<int ...S>
        static typename extent_as_pointer<T>::type call_methodBase(
                typename extent_as_pointer<T>::type  (CClass::*method)(Args...) const,
                typename std::remove_reference<CClass>::type &self,
                PyObject *args, PyObject *kwds, container<S...> s);

    };


    /**
     * specialize for void returns:
     **/
    template<typename CClass, typename ...Args>
    class ConstMethodCallSemantics<CClass, void, Args...> {
    public:
        typedef void(CClass::*method_t)(Args...) const;

        static const char *const *kwlist;

        static PyObject *toPyObj(CClass &self);

        static PyObject *call(method_t method, CClass &self, PyObject *args, PyObject *kwds);

    private:

        template<typename ...PyO>
        static void call_methodC(void (CClass::*method)(Args...) const,
                                 typename std::remove_reference<CClass>::type &self,
                                 PyObject *args, PyObject *kwds,
                                 PyO *...pyargs);

        template<int ...S>
        static void call_methodBase(void (CClass::*method)(Args...) const,
                                    typename std::remove_reference<CClass>::type &self,
                                    PyObject *args, PyObject *kwds,
                                    container<S...> unused);

    };


    template<class CClass>
    class ConstMethodContainer {
    public:

        template<const char *const name, typename ReturnType, typename ...Args>
        class Container {
        public:
            typedef typename extent_as_pointer<ReturnType>::type(CClass::*method_t)(Args...) const;

            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = ConstMethodCallSemantics<CClass, ReturnType, Args...>::kwlist;
            static method_t method;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds) ;

        };
    };


    template<class CClass>
    template<const char *const name, typename ReturnType, typename ...Args>
    typename ConstMethodContainer<CClass>::template Container<name, ReturnType, Args...>::method_t
            ConstMethodContainer<CClass>::Container<name, ReturnType, Args...>::method;


    /////////////////////////////////////////////////////////

    /**
     * Class member container
     **/
    template<class CClass>
    class ConstMemberContainer {
    public:

        template<const char *const name, typename T>
        class Container {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T const CClass_NoRef::* member_t;

            static member_t member;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds);

        };
    };

    template<class CClass>
    template<const char *const name, typename T>
    typename ConstMemberContainer<CClass>::template Container<name, T>::member_t
            ConstMemberContainer<CClass>::Container<name, T>::member;


}

#endif // __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2

