#ifndef __PYLLARS_INTERNAL__CLASSMETHODCALLSEMANTICS_H
#define __PYLLARS_INTERNAL__CLASSMETHODCALLSEMANTICS_H

#include </usr/include/python2.7/Python.h>
#include </usr/include/string.h>
#include </usr/include/sys/types.h>

#include "pyllars_utils.hpp"
#include "pyllars_defns.hpp"
#include "pyllars_conversions.hpp"

/**
* This unit defines template classes needed to contain method pointers and
* define Python-to-C call semantics for invoking class instance methods
**/


namespace __pyllars_internal {

    /**
     * class to hold reference to a class method and define
     * method call semantics
     **/
    template<typename CClass, typename T, typename ... Args>
    class ClassMethodCallSemantics {
    public:
        typedef typename extent_as_pointer<T>::type(*method_t)(Args...);

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
        static typename extent_as_pointer<T>::type call_methodC(typename extent_as_pointer<T>::type  (*method)(Args...),
                                                                PyObject *args, PyObject *kwds, PyO *...pyargs);
        /**
         * call that converts python given arguments to make C call:
         **/
        template<int ...S>
        static typename extent_as_pointer<T>::type call_methodBase(
                typename extent_as_pointer<T>::type  (*method)(Args...),
                PyObject *args, PyObject *kwds, container<S...> s) ;

    };


    /**
     * specialize for void returns:
     **/
    template<typename CClass, typename ...Args>
    class ClassMethodCallSemantics<CClass, void, Args...> {
    public:
        typedef void(*method_t)(Args...);

        static const char *const *kwlist;

        static PyObject *call(method_t method, PyObject *args, PyObject *kwds) ;

    private:

        template<typename ...PyO>
        static void call_methodC(void (*method)(Args...),
                                 PyObject *args, PyObject *kwds,
                                 PyO *...pyargs) ;

        template<int ...S>
        static void call_methodBase(void (*method)(Args...),
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

        template<const char *const name, typename ReturnType, typename ...Args>
        class Container {
            typedef typename extent_as_pointer<ReturnType>::type (*method_t)(Args...);

            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = ClassMethodCallSemantics<CClass, ReturnType, Args...>::kwlist;

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

        template<const char *const name, typename ReturnType, typename ...Args>
        class Container {
        public:
            typedef typename extent_as_pointer<ReturnType>::type(*method_t)(Args...);

            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = ClassMethodCallSemantics<CClass, ReturnType, Args...>::kwlist;
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

        template<const char *const name, typename ReturnType, typename ...Args>
        class Container {
        public:
            typedef typename extent_as_pointer<ReturnType>::type(*method_t)(Args...);

            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = ClassMethodCallSemantics<CClass, ReturnType, Args...>::kwlist;
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

            static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds) {
                (void) cls;
                static const char *kwlist[] = {"value", nullptr};
                static char format[2] = {'O', 0};
                PyObject *pyarg = nullptr;
                if (PyTuple_Size(args) > 0) {
                    PyErr_SetString(PyExc_ValueError,
                                    "Only one value with explicit keyword 'value' allowed to this method");
                    return nullptr;
                } else if (kwds && !PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyarg)) {
                    PyErr_SetString(PyExc_ValueError, "Invalid argument keyword name or type to method call");
                    return nullptr;
                } else if (kwds) {
                    setFromPyObject(pyarg);
                    return Py_None;
                }
                return toPyObject<T>(member, false);
            }

            static void setFromPyObject(PyObject *pyobj) {
                T val[] = *toCObject<T[size], false, PythonClassWrapper<T[size]>>(*pyobj);
                for (size_t i = 0; i < size; ++i)member[i] = val[i];
            }
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

