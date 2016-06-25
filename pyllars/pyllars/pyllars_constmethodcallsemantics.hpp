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
        static PyObject *call(method_t method, CClass &self, PyObject *args, PyObject *kwds) {
            try {
                typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
                const T &result = call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type());
                const ssize_t array_size = ArraySize<T>::size;//sizeof(result) / sizeof(T_base);
                return toPyObject<T>(result, false, array_size);
            } catch (const char *const msg) {
                PyErr_SetString(PyExc_RuntimeError, msg);
                return nullptr;
            }
        }

    private:

        /**
         * call that invokes method a la C:
         **/
        template<typename ...PyO>
        static typename extent_as_pointer<T>::type call_methodC(
                typename extent_as_pointer<T>::type  (CClass::*method)(Args...) const,
                typename std::remove_reference<CClass>::type &self,
                PyObject *args, PyObject *kwds, PyO *...pyargs) {
            static char format[sizeof...(Args) + 1] = {0};
            if (sizeof...(Args) > 0)
                memset(format, 'O', (size_t) sizeof...(Args));

            if (sizeof...(Args) > 0 && !PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...)) {
                PyErr_Print();
                throw "Invalid arguments to method call";
            }
            T retval = (self.*method)(*toCObject<Args, false, PythonClassWrapper<Args> >(*pyargs)...);
            return retval;
        }

        /**
         * call that converts python given arguments to make C call:
         **/
        template<int ...S>
        static typename extent_as_pointer<T>::type call_methodBase(
                typename extent_as_pointer<T>::type  (CClass::*method)(Args...) const,
                typename std::remove_reference<CClass>::type &self,
                PyObject *args, PyObject *kwds, container<S...> s) {
            (void) s;
            PyObject pyobjs[sizeof...(Args) + 1];
            (void) pyobjs;
            return call_methodC(method, self, args, kwds, &pyobjs[S]...);
        }

    };

    template<class CClass, typename ReturnType, typename ...Args>
    const char *const *
            ConstMethodCallSemantics<CClass, ReturnType, Args...>::kwlist;


    /**
     * specialize for void returns:
     **/
    template<typename CClass, typename ...Args>
    class ConstMethodCallSemantics<CClass, void, Args...> {
    public:
        typedef void(CClass::*method_t)(Args...) const;

        static const char *const *kwlist;

        static PyObject *toPyObj(CClass &self) {
            (void) self;
            return Py_None;
        }

        static PyObject *call(method_t method, CClass &self, PyObject *args, PyObject *kwds) {
            call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type());
            return Py_None;
        }

    private:

        template<typename ...PyO>
        static void call_methodC(void (CClass::*method)(Args...) const,
                                 typename std::remove_reference<CClass>::type &self,
                                 PyObject *args, PyObject *kwds,
                                 PyO *...pyargs) {

            char format[sizeof...(Args) + 1] = {0};
            if (sizeof...(Args) > 0)
                memset(format, 'O', (size_t) sizeof...(Args));
            if (!PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...)) {
                PyErr_SetString(PyExc_RuntimeError, "Failed to parse argument on method call");
            } else {
                (self.*method)(*toCObject<Args, false, PythonClassWrapper<Args> >(*pyargs)...);
            }
        }

        template<int ...S>
        static void call_methodBase(void (CClass::*method)(Args...) const,
                                    typename std::remove_reference<CClass>::type &self,
                                    PyObject *args, PyObject *kwds,
                                    container<S...> unused) {
            (void) unused;
            PyObject pyobjs[sizeof...(Args) + 1];
            call_methodC(method, self, args, kwds, &pyobjs[S]...);
            (void) pyobjs;
        }

    };
    template<class CClass,  typename ...Args>
    const char *const *
    ConstMethodCallSemantics<CClass, void, Args...>::kwlist;


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

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds) {
                if (!self) return nullptr;
                typedef PythonClassWrapper<CClass> ClassWrapper;
                auto _this = reinterpret_cast<ClassWrapper *>(self);
                if (_this->template get_CObject<CClass>()) {

                    try {
                        return ConstMethodCallSemantics<CClass, ReturnType, Args...>::call(method,
                                                                                           *_this->template get_CObject<CClass>(),
                                                                                           args, kwds);
                    } catch (...) {
                        PyErr_SetString(PyExc_RuntimeError, "Exception calling class instance method");
                        return nullptr;
                    }

                }
                return nullptr;
            }

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

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds) {
                if ((args && PyTuple_Size(args) != 0) || (kwds && PyDict_Size(kwds) != 0)) {
                    PyErr_SetString(PyExc_RuntimeError, "Expected no arguments");
                    return nullptr;
                }
                if (!self) return nullptr;
                typedef PythonClassWrapper<CClass> ClassWrapper;
                auto _this = reinterpret_cast<ClassWrapper *>(self);
                typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
                const ssize_t array_size = ArraySize<T>::size;//sizeof(_this->template get_CObject<CClass>()->*member) / sizeof(T_base);
                if (_this->template get_CObject<CClass>()) {
                    return toPyObject<T>((_this->template get_CObject<CClass>()->*member), false, array_size);
                }
                PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
                return nullptr;
            }

        };
    };

    template<class CClass>
    template<const char *const name, typename T>
    typename ConstMemberContainer<CClass>::template Container<name, T>::member_t
            ConstMemberContainer<CClass>::Container<name, T>::member;


}

#endif // __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2
