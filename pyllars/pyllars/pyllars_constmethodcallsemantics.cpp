//
// Created by jrusnak on 7/22/16.
//
#ifndef __PYLLARS__INTERNAL__CONSTMETHODCALLSEMANTICS_CPP__
#define __PYLLARS__INTERNAL__CONSTMETHODCALLSEMANTICS_CPP__

#include "pyllars_constmethodcallsemantics.hpp"
#include "pyllars_conversions.cpp"

namespace __pyllars_internal {

    template<typename CClass, typename T, typename ... Args>
    PyObject *ConstMethodCallSemantics<CClass, T, Args...>::call(method_t method, CClass &self, PyObject *args, PyObject *kwds) {
        try {
            const T &result = call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type());
            const ssize_t array_size = ArraySize<T>::size;//sizeof(result) / sizeof(T_base);
            return toPyObject<T>(result, false, array_size);
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }

    template<typename CClass, typename T, typename ... Args>
    template<typename ...PyO>
    typename extent_as_pointer<T>::type
    ConstMethodCallSemantics<CClass, T, Args...>::call_methodC(
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
    template<typename CClass, typename T, typename ... Args>
    template<int ...S>
    typename extent_as_pointer<T>::type
    ConstMethodCallSemantics<CClass, T, Args...>::call_methodBase(
            typename extent_as_pointer<T>::type  (CClass::*method)(Args...) const,
            typename std::remove_reference<CClass>::type &self,
            PyObject *args, PyObject *kwds, container<S...> s) {
        (void) s;
        PyObject pyobjs[sizeof...(Args) + 1];
        (void) pyobjs;
        return call_methodC(method, self, args, kwds, &pyobjs[S]...);
    }


    template<typename CClass, typename ...Args>
    PyObject *ConstMethodCallSemantics<CClass, void, Args...>::
    toPyObj(CClass &self) {
        (void) self;
        return Py_None;
    }

    template<typename CClass, typename ...Args>
    PyObject *ConstMethodCallSemantics<CClass, void, Args...>::
    call(method_t method, CClass &self, PyObject *args, PyObject *kwds) {
        call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type());
        return Py_None;
    }

    template<typename CClass, typename ...Args>
    template<typename ...PyO>
    void ConstMethodCallSemantics<CClass, void, Args...>::
    call_methodC(void (CClass::*method)(Args...) const,
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

    template<typename CClass, typename ...Args>
    template<int ...S>
    void ConstMethodCallSemantics<CClass, void, Args...>::
    call_methodBase(void (CClass::*method)(Args...) const,
                    typename std::remove_reference<CClass>::type &self,
                    PyObject *args, PyObject *kwds,
                    container<S...> unused) {
        (void) unused;
        PyObject pyobjs[sizeof...(Args) + 1];
        call_methodC(method, self, args, kwds, &pyobjs[S]...);
        (void) pyobjs;
    }


    template<class CClass, typename ReturnType, typename ...Args>
    const char *const * ConstMethodCallSemantics<CClass, ReturnType, Args...>::kwlist;


    template<class CClass,  typename ...Args>
    const char *const *
            ConstMethodCallSemantics<CClass, void, Args...>::kwlist;


    template<class CClass>
    template<const char *const name, typename ReturnType, typename ...Args>
    PyObject *ConstMethodContainer<CClass>::Container<name,  ReturnType, Args...>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
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

    template<class CClass>
    template<const char *const name, typename T>
    PyObject *ConstMemberContainer<CClass>::Container<name, T>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if ((args && PyTuple_Size(args) != 0) || (kwds && PyDict_Size(kwds) != 0)) {
            PyErr_SetString(PyExc_RuntimeError, "Expected no arguments");
            return nullptr;
        }
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> ClassWrapper;
        auto _this = reinterpret_cast<ClassWrapper *>(self);
        const ssize_t array_size = ArraySize<T>::size;//sizeof(_this->template get_CObject<CClass>()->*member) / sizeof(T_base);
        if (_this->template get_CObject<CClass>()) {
            return toPyObject<T>((_this->template get_CObject<CClass>()->*member), false, array_size);
        }
        PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
        return nullptr;
    }


}

#endif