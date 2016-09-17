//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__CLASSMETHODSEMANTICS_CPP__
#define __PYLLARS_INTERNAL__CLASSMETHODSEMANTICS_CPP__

#include "pyllars_classmethodsemantics.hpp"

namespace __pyllars_internal{

    /**
    * Used for regular methods:
    */
    template<typename CClass, typename T, typename ... Args>
    PyObject *ClassMethodCallSemantics<CClass, T,Args...>::
    call(method_t method, PyObject *args, PyObject *kwds) {
        try {
            const T &result = call_methodBase(method, args, kwds, typename argGenerator<sizeof...(Args)>::type());
            const ssize_t array_size = ArraySize<T>::size;//sizeof(result) / sizeof(T_base);
            return toPyObject<T>(result, false, array_size);
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }


    /**
     * call that invokes method a la C:
     **/
    template<typename CClass, typename T, typename ... Args>
    template<typename ...PyO>
    typename extent_as_pointer<T>::type
    ClassMethodCallSemantics<CClass, T,Args...>::
    call_methodC(typename extent_as_pointer<T>::type  (*method)(Args...),
                                                            PyObject *args, PyObject *kwds, PyO *...pyargs) {
        static char format[sizeof...(Args) + 1] = {0};
        if (sizeof...(Args) > 0) {
            memset(format, 'O', (size_t)
            sizeof...(Args));

            if (!PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...)) {
                throw "Invalid arguments to method call";
            }
        }
        T retval = method(*toCObject<Args, false, PythonClassWrapper<Args> >(*pyargs)...);
        return retval;
    }

    /**
     * call that converts python given arguments to make C call:
     **/
    template<typename CClass, typename T, typename ... Args>
    template<int ...S>
    typename extent_as_pointer<T>::type
    ClassMethodCallSemantics<CClass, T,Args...>::
    call_methodBase(
            typename extent_as_pointer<T>::type  (*method)(Args...),
            PyObject *args, PyObject *kwds, container<S...> s) {
        (void) s;
        PyObject pyobjs[sizeof...(Args) + 1];
        (void) pyobjs;
        return call_methodC(method, args, kwds, &pyobjs[S]...);
    }

    template<class CClass, typename ReturnType, typename ...Args>
    const char *const *ClassMethodCallSemantics<CClass, ReturnType, Args...>::kwlist;



    template<typename CClass, typename ...Args>
    PyObject *ClassMethodCallSemantics<CClass, void, Args...>::
    call(method_t method, PyObject *args, PyObject *kwds) {
        call_methodBase(method, args, kwds, typename argGenerator<sizeof...(Args)>::type());
        return Py_None;
    }


    template<typename CClass, typename ...Args>
    template<typename ...PyO>
    void ClassMethodCallSemantics<CClass, void, Args...>::
    call_methodC(void (*method)(Args...),
                             PyObject *args, PyObject *kwds,
                             PyO *...pyargs) {

        char format[sizeof...(Args) + 1] = {0};
        if (sizeof...(Args) > 0)
            memset(format, 'O', sizeof...(Args));
        if (!PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...)) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to parse argument on method call");
        } else {
            method(*toCObject<Args, false, PythonClassWrapper<Args> >(*pyargs)...);
        }
    }

    template<typename CClass, typename ...Args>
    template<int ...S>
    void ClassMethodCallSemantics<CClass, void, Args...>::
    call_methodBase(void (*method)(Args...),
                                PyObject *args, PyObject *kwds,
                                container<S...> unused) {
        (void) unused;
        PyObject pyobjs[sizeof...(Args) + 1];
        call_methodC(method, args, kwds, &pyobjs[S]...);
        (void) pyobjs;
    }

    template<class CClass, typename ...Args>
    const char *const *ClassMethodCallSemantics<CClass, void, Args...>::kwlist;



    template<class CClass>
    template<const char *const name, typename ReturnType, typename ...Args>
    PyObject *ClassMethodContainer<CClass, typename std::enable_if<
            std::is_class<CClass>::value && !std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::
    call(PyObject *cls, PyObject *args, PyObject *kwds) {
        (void) cls;
        try {
            return ClassMethodCallSemantics<CClass, ReturnType, Args...>::call(method, args, kwds);
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Exception in class method");
            return nullptr;
        }
        return nullptr;
    }


    template<class CClass>
    template<const char *const name, typename ReturnType, typename ...Args>
    PyObject *ClassMethodContainer<CClass, typename std::enable_if<
            std::is_class<CClass>::value && std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::
    call(PyObject *cls, PyObject *args, PyObject *kwds) {
        (void) cls;
        try {
            return ClassMethodCallSemantics<CClass, ReturnType, Args...>::call(method, args, kwds);
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Exception in class method");
            return nullptr;
        }
        return nullptr;
    }

    template<class CClass>
    template<const char *const name, typename ReturnType, typename ...Args>
    typename ClassMethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                                  !std::is_const<CClass>::value>::type>::template Container<name, ReturnType, Args...>::method_t
            ClassMethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                                 !std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::method;


    template<class CClass>
    template<const char *const name, typename ReturnType, typename ...Args>
    typename ClassMethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                                  std::is_const<CClass>::value>::type>::template Container<name, ReturnType, Args...>::method_t
            ClassMethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                                 std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::method;



    template<class CClass>
    template<const char *const name, typename T>
    PyObject *ClassMemberContainer<CClass>::Container<name, T>::
    call(PyObject *cls, PyObject *args, PyObject *kwds) {
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
            *member = *toCObject<T, std::is_array<T>::value, PythonClassWrapper<T> >(*pyarg);
            return Py_None;
        }
        return toPyObject<T>(*member, false, ArraySize<T>::size);
    }

    template<class CClass>
    template<const char *const name, typename T>
    void ClassMemberContainer<CClass>::Container<name, T>::
    setFromPyObject(PyObject *pyobj) {
        *member = *toCObject<T, false, PythonClassWrapper<T>>(*pyobj);
    }

    template<class CClass>
    template<const char *const name, typename T>
    typename ClassMemberContainer<CClass>::template Container<name, T>::member_t
            ClassMemberContainer<CClass>::Container<name, T>::member;



    template<class CClass>
    template<const char *const name, typename T>
    PyObject *ConstClassMemberContainer<CClass>::Container<name, T>::
    call(PyObject *cls, PyObject *args, PyObject *kwds) {
        (void) cls;
        if (PyTuple_Size(args) > 0 || kwds) {
            PyErr_SetString(PyExc_ValueError, "C++: const static members cannot change value");
            return nullptr;
        }
        return toPyObject<T>(*member, false, ArraySize<T>::size);
    }

    template<class CClass>
    template<const char *const name, typename T>
    typename ConstClassMemberContainer<CClass>::template Container<name, T>::member_t
            ConstClassMemberContainer<CClass>::Container<name, T>::member;

    template<class CClass>
    template<const char *const name, size_t size, typename T>
    void ClassMemberContainer<CClass>::Container<name, T[size]>::setFromPyObject(PyObject* pyobj){
        T val[] = *toCObject<T[size], false, PythonClassWrapper<T[size]>>(*pyobj);
        for (size_t i = 0; i < size; ++i)member[i] = val[i];
    }

    template<class CClass>
    template<const char *const name, size_t size, typename T>
    PyObject * ClassMemberContainer<CClass>::Container<name, T[size]>::call(PyObject *cls, PyObject *args, PyObject *kwds){
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
}

#endif