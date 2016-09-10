//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__METHODCALLSEMANTICS_CPP_
#define __PYLLARS_INTERNAL__METHODCALLSEMANTICS_CPP_

#include "pyllars_methodcallsemantics.hpp"
#include "pyllars_classwrapper.cpp"

namespace __pyllars_internal {


    template<typename CClass, typename T, typename ... Args>
    PyObject *MethodCallSemantics<CClass, T, Args...>::
    call(method_t method, CClass &self, PyObject *args, PyObject *kwds) {
        try {
            T result = call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type());
            // const ssize_t type_size = Sizeof<T_base>::value;
            const ssize_t array_size = ArraySize<T>::size;//type_size > 0 ? sizeof(result) / type_size : 1;
            return toPyObject<T>(result, false, array_size);
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }

    template<typename CClass, typename T, typename ... Args>
    template<typename ...PyO>
    typename extent_as_pointer<T>::type
    MethodCallSemantics<CClass, T, Args...>::
    call_methodC(
            typename extent_as_pointer<T>::type  (CClass::*method)(Args...),
            typename std::remove_reference<CClass>::type &self,
            PyObject *args, PyObject *kwds, PyO *...pyargs) {
        static char format[sizeof...(Args) + 1] = {0};
        if (sizeof...(Args) > 0)
            memset(format, 'O', sizeof...(Args));

        if (!PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...)) {
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
    MethodCallSemantics<CClass, T, Args...>::
    call_methodBase(
            typename extent_as_pointer<T>::type  (CClass::*method)(Args...),
            typename std::remove_reference<CClass>::type &self,
            PyObject *args, PyObject *kwds, container<S...> s) {
        (void) s;
        PyObject pyobjs[sizeof...(Args) + 1];
        (void) pyobjs;
        return call_methodC(method, self, args, kwds, &pyobjs[S]...);

    }

    template<class CClass, typename ReturnType, typename ...Args>
    const char *const *
            MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;


    template<typename CClass, typename ...Args>
    PyObject *MethodCallSemantics<CClass, void, Args...>::
    toPyObj(CClass &self) {
        (void) self;
        return Py_None;
    }

    template<typename CClass, typename ...Args>
    PyObject *MethodCallSemantics<CClass, void, Args...>::
    call(method_t method, CClass &self, PyObject *args, PyObject *kwds) {
        call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type());
        return Py_None;
    }

    template<typename CClass, typename ...Args>
    template<typename ...PyO>
    void MethodCallSemantics<CClass, void, Args...>::
    call_methodC(void (CClass::*method)(Args...),
                 typename std::remove_reference<CClass>::type &self,
                 PyObject *args, PyObject *kwds,
                 PyO *...pyargs) {

        char format[sizeof...(Args) + 1] = {0};
        if (sizeof...(Args) > 0)
            memset(format, 'O', sizeof...(Args));
        if (!PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...)) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to parse argument on method call");
        } else {
            (self.*method)(*toCObject<Args, false, PythonClassWrapper<Args> >(*pyargs)...);
        }
    }

    template<typename CClass, typename ...Args>
    template<int ...S>
    void MethodCallSemantics<CClass, void, Args...>::
    call_methodBase(void (CClass::*method)(Args...),
                    typename std::remove_reference<CClass>::type &self,
                    PyObject *args, PyObject *kwds,
                    container<S...> unused) {
        (void) unused;
        PyObject pyobjs[sizeof...(Args) + 1];
        call_methodC(method, self, args, kwds, &pyobjs[S]...);
        (void) pyobjs;
    }


    template<class CClass, typename ...Args>
    const char *const *MethodCallSemantics<CClass, void, Args...>::kwlist;


    template<class CClass>
    template<const char *const name, typename ReturnType, typename ...Args>
    PyObject *MethodContainer<CClass, typename std::enable_if<
            std::is_class<CClass>::value &&
            !std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        if (_this->template get_CObject<CClass>()) {
            try {
                return MethodCallSemantics<CClass, ReturnType, Args...>::call(method,
                                                                              *_this->template get_CObject<CClass>(),
                                                                              args, kwds);
            } catch (...) {
                return nullptr;
            }
        }
        return nullptr;
    }

    template<typename CClass>
    template<const char *const name, typename ReturnType, typename ...Args>
    typename MethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                             !std::is_const<CClass>::value>::type>::template Container<name, ReturnType, Args...>::method_t
            MethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                            !std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::method;


    template<class CClass>
    template<const char *const name, typename ReturnType, typename ...Args>
    PyObject *MethodContainer<CClass, typename std::enable_if<
            std::is_class<CClass>::value && std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;
        if (_this->template get_CObject<CClass>()) {
            try {
                return MethodCallSemantics<CClass, ReturnType, Args...>::call(method,
                                                                              *_this->template get_CObject<CClass>(),
                                                                              args, kwds);
            } catch (...) {
                return nullptr;
            }
        }
        return nullptr;
    }

    template<class CClass>
    template<const char *const name, typename ReturnType, typename ...Args>
    typename MethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                             std::is_const<CClass>::value>::type>::template Container<name, ReturnType, Args...>::method_t
            MethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                            std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::method;


    template<class CClass>
    template<const char *const name, typename T>
    PyObject *MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;

        if (!std::is_reference<T>::value &&(!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
            const ssize_t base_size = ArrayHelper<T>::base_sizeof();
            const ssize_t array_size =
                    base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                                  : UNKNOWN_SIZE;
            if (_this->template get_CObject<CClass>()) {
                return toPyObject<T>(_this->template get_CObject<CClass>()->*member, false, array_size);
            }
            PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
            return nullptr;
        } else if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "set_to")) {
            PyObject *pyVal = PyDict_GetItemString(kwds, "set_to");
            if (pyVal == Py_None) {
                PyErr_SetString(PyExc_SyntaxError, "Unexpcted None value in member setter");
                return nullptr;
            }
            (_this->template get_CObject<CClass_NoRef>()->*member) =
                    *toCObject<T, false, PythonClassWrapper<T> >(*pyVal);
        } else if ((std::is_reference<T>::value && (!kwds || PyDict_Size(kwds)==0))||
                   (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "as_ref"))) {
            //handle case if oject is to be returned by reference
            bool asRef = std::is_reference<T>::value ;
            if (PyDict_Size(kwds)==1) {
                PyObject *pyVal = PyDict_GetItemString(kwds, "as_ref");
                if (pyVal == Py_None) {
                    PyErr_SetString(PyExc_SyntaxError, "Unexpected None value in member setter");
                    return nullptr;
                }
                if (PyBool_Check(pyVal)) {
                    asRef = (pyVal == Py_True);
                } else {
                    PyErr_SetString(PyExc_TypeError, "Expected boolean value for 'as_ref'");
                    return nullptr;
                }
                if (_this->template get_CObject<CClass>()) {
                    asRef = asRef && !std::is_fundamental<T>::value;
                    PyObject *obj = toPyObject<T>(_this->template get_CObject<CClass>()->*member, asRef, array_size);;
                    if (asRef) {
                        ((PythonClassWrapper<T> *) obj)->make_reference(self);
                    }
		    return obj;
                }
                PyErr_SetString(PyExc_RuntimeError, "Internal error: No C Object found to get member attribute value!");
                return nullptr;
            }
        } else {
            PyErr_SetString(PyExc_SyntaxError, "Invalid argsuments to set class instance member variable in C");
            return nullptr;
        }
        return Py_None;
    }

    template<class CClass>
    template<const char *const name, typename T>
    void MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::
    setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
        self->*member = *toCObject<T, false, PythonClassWrapper<T> >(*pyobj);
    }

    template<class CClass>
    template<const char *const name, typename T>
    PyObject *MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0>::type>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;

        if (!std::is_reference<T>::value && (!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
            const ssize_t base_size = ArrayHelper<T>::base_sizeof();
            const ssize_t array_size =
                    base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                                  : UNKNOWN_SIZE;
            if (_this->template get_CObject<CClass>()) {
                return toPyObject<T>(_this->template get_CObject<CClass>()->*member, false, array_size);
            }
            PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
            return nullptr;
        } else if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "set_to")) {
            PyObject *pyVal = PyDict_GetItemString(kwds, "set_to");
            if (pyVal == Py_None) {
                PyErr_SetString(PyExc_SyntaxError, "Unexpcted None value in member setter");
                return nullptr;
            }

        }  else if ((std::is_reference<T>::value && (!kwds || PyDict_Size(kwds)==0))||
                    (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "as_ref"))) {
            //handle case if oject is to be returned by reference
            bool asRef = std::is_reference<T>::value ;
            if (PyDict_Size(kwds)==1) {
                PyObject *pyVal = PyDict_GetItemString(kwds, "as_ref");
                if (pyVal == Py_None) {
                    PyErr_SetString(PyExc_SyntaxError, "Unexpected None value in member setter");
                    return nullptr;
                }
                if (PyBool_Check(pyVal)) {
                    asRef = (pyVal == Py_True);
                } else {
                    PyErr_SetString(PyExc_TypeError, "Expected boolean value for 'as_ref'");
                    return nullptr;
                }
                if (_this->template get_CObject<CClass>()) {
                    asRef = asRef && !std::is_fundamental<T>::value;
                    PyObject* obj = toPyObject<T>(*(_this->template get_CObject<CClass>()), asRef, 1);
                    if (asRef) {
                        ((PythonClassWrapper<T>*)obj)->make_reference(self);
                    }
		    return obj;
                }
                PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
                return nullptr;
            }
        } else {
            PyErr_SetString(PyExc_SyntaxError, "Invalid argsuments to set class instance member variable in C");
            return nullptr;
        }
        return Py_None;
    }

    template<class CClass>
    template<const char *const name, typename T>
    void MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0>::type>::
    setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
        (void) self;
        (void) pyobj;
    }

    template<class CClass>
    template<const char *const name, typename T>
    PyObject *MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        if (!std::is_reference<T>::value && (!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
            if (_this->template get_CObject<CClass_NoRef>()) {
                const ssize_t base_size = ArrayHelper<T>::base_sizeof();
                const ssize_t array_size =
                        base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                                      : UNKNOWN_SIZE;
                return toPyObject<T>(_this->template get_CObject<CClass_NoRef>()->*member, false, array_size);
            }
            PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
            return nullptr;
        } else if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "set_to")) {
            PyErr_SetString(PyExc_RuntimeError, "Attempt to set constant attribute!");
            return nullptr;
        }else if ((std::is_reference<T>::value && (!kwds || PyDict_Size(kwds)==0))||
                  (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "as_ref"))) {
            //handle case if oject is to be returned by reference
            bool asRef = std::is_reference<T>::value ;
            if (PyDict_Size(kwds)==1) {
                PyObject *pyVal = PyDict_GetItemString(kwds, "as_ref");
                if (pyVal == Py_None) {
                    PyErr_SetString(PyExc_SyntaxError, "Unexpected None value in member setter");
                    return nullptr;
                }
                if (PyBool_Check(pyVal)) {
                    asRef = (pyVal == Py_True);
                } else {
                    PyErr_SetString(PyExc_TypeError, "Expected boolean value for 'as_ref'");
                    return nullptr;
                }
                if (_this->template get_CObject<CClass>()) {
                    asRef = asRef && !std::is_fundamental<T>::value;
                    PyObject* obj = toPyObject<T>(*(_this->template get_CObject<CClass>()), asRef, 1);
                    if (asRef) {
                        ((PythonClassWrapper<T>*)obj)->make_reference(self);
                    }
		    return obj;
                }
                PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
                return nullptr;
            }
        }  else {
            PyErr_SetString(PyExc_SyntaxError, "Invalid argsuments to set class instance member variable in C");
            return nullptr;
        }
    }

    template<class CClass>
    template<const char *const name, typename T>
    void MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::
    setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
        PyErr_SetString(PyExc_RuntimeError, "Attempt to set constant field");
    }

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    PyObject *MemberContainer<CClass>::Container<name, T[size], void>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        try {
            if (!self) return nullptr;
            PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;

            if (!std::is_reference<T>::value &&(!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
                if (array_size != size) {
                    static char msg[250];
                    snprintf(msg, 250, "Mismatched array sizes (static)%lld!=%lld", (long long) array_size,
                             (long long) size);
                    PyErr_SetString(PyExc_TypeError, msg);
                    return nullptr;
                }
                if (_this->template get_CObject<CClass>()) {
                    const ssize_t base_size = ArrayHelper<T_array>::base_sizeof();
                    const ssize_t array_size =
                            base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                                          : UNKNOWN_SIZE;
                    return toPyObject<T_array>(_this->template get_CObject<CClass>()->*member, false,
                                               array_size);
                }
                PyErr_SetString(PyExc_RuntimeError, "Internal Error:No C Object found to get member attribute value!");
                return nullptr;
            } else if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "set_to")) {
                PyObject *pyVal = PyDict_GetItemString(kwds, "set_to");
                if (pyVal == Py_None) {
                    PyErr_SetString(PyExc_SyntaxError, "Unexpcted None value in member setter");
                    return nullptr;
                }
                if (PyTuple_Check(pyVal)) {
                    if (PyTuple_Size(pyVal) == size) {
                        for (size_t i = 0; i < size; ++i)
                            (_this->template get_CObject<CClass_NoRef>()->*
                             member)[i] = *toCObject<T, false, PythonClassWrapper<T> >(
                                    *PyTuple_GetItem(pyVal, i));
                    } else {
                        static char msg[250];
                        snprintf(msg, 250, "Mismatched array sizes (tuple)%lld!=%lld",
                                 (long long) PyTuple_Size(pyVal),
                                 (long long) size);
                        PyErr_SetString(PyExc_IndexError, msg);
                        return nullptr;
                    }
                } else if (PythonClassWrapper<T_array>::checkType(pyVal)) {
                    T_array *val = ((PythonClassWrapper<T_array> *) pyVal)->template get_CObject<T_array>();
                    for (size_t i = 0; i < size; ++i)
                        (_this->template get_CObject<T_array>()->*member)[i] = (*val)[i];

                } else if ((std::is_reference<T>::value && (!kwds || PyDict_Size(kwds)==0))||
                           (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "as_ref"))) {
                    //handle case if oject is to be returned by reference
                    bool asRef = std::is_reference<T>::value ;
                    if (PyDict_Size(kwds)==1) {
                        PyObject *pyVal = PyDict_GetItemString(kwds, "as_ref");
                        if (pyVal == Py_None) {
                            PyErr_SetString(PyExc_SyntaxError, "Unexpected None value in member setter");
                            return nullptr;
                        }
                        if (PyBool_Check(pyVal)) {
                            asRef = (pyVal == Py_True);
                        } else {
                            PyErr_SetString(PyExc_TypeError, "Expected boolean value for 'as_ref'");
                            return nullptr;
                        }
                        if (_this->template get_CObject<CClass>()) {
                            asRef = asRef && !std::is_fundamental<T>::value;
                            PyObject* obj = toPyObject<T_array>(_this->template get_CObject<CClass>()->*member, asRef,
                                                               array_size);
                            if (asRef) {
                                ((PythonClassWrapper<T>*)obj)->make_reference(self);
                            }
			    return obj;
                        }
                        PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
                        return nullptr;
                    }
                }else {
                    PyErr_SetString(PyExc_TypeError, "Invalid argument type when setting array attribute");
                    return nullptr;
                }
            } else {
                PyErr_SetString(PyExc_SyntaxError,
                                "Invalid arguments to set class instance member variable in C");
                return nullptr;
            }
            return Py_None;
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_SystemError, msg);
            return nullptr;
        }
    }

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    void MemberContainer<CClass>::Container<name, T[size], void>::
    setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
        smart_ptr<T[size], false> val = toCObject<T[size], false, PythonClassWrapper<T[size]> >(
                *pyobj);
        for (size_t i = 0; i < size; ++i) {
            (self->*member)[i] = (*val)[i];
        }
    }


    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    PyObject *MemberContainer<CClass>::Container<name, const T[size], void>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;

        if ((!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
            const ssize_t base_size = ArrayHelper<T_array>::base_sizeof();
            const ssize_t array_size =
                    base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                                  : UNKNOWN_SIZE;
            if (array_size != size) {
                PyErr_SetString(PyExc_TypeError, "Mismatched array sizes");
                return nullptr;
            }
            if (_this->template get_CObject<CClass>()) {
                return toPyObject<T_array, size>(*(_this->template get_CObject<CClass>()->*member), false,
                                                 array_size);
            }
            PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
            return nullptr;
        } else {
            PyErr_SetString(PyExc_SyntaxError, "Cannot set const array elements!");
            return nullptr;
        }
        return Py_None;
    }

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    void MemberContainer<CClass>::Container<name, const T[size], void>::
    setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
        smart_ptr<T[size], false> val = toCObject<T[size], false, PythonClassWrapper<T[size]> >(
                *pyobj);
        for (size_t i = 0; i < size; ++i) {
            (self->*member)[i] = (*val)[i];
        }
    }


    template<class CClass>
    template<const char *const name, typename T>
    PyObject *MemberContainer<CClass>::Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;

        if (!std::is_reference<T>::value && (!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
            if (_this->template get_CObject<CClass>()) {
                const ssize_t base_size = ArrayHelper<T[]>::base_sizeof();
                const ssize_t array_size =
                        base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                                      : UNKNOWN_SIZE;
                return toPyObject<T[]>(*(_this->template get_CObject<CClass>()->*member), false, array_size);
            }
            PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
            return nullptr;
        } else if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "set_to")) {
            PyObject *pyVal = PyDict_GetItemString(kwds, "set_to");
            if (pyVal == Py_None) {
                PyErr_SetString(PyExc_RuntimeError, "Unexpcted None value in member setter");
                return nullptr;
            }
            if (array_size == 0) {
                PyErr_SetString(PyExc_RuntimeError, "Attempt to set array elements on array of unknown size.");
                return nullptr;
            }
            if (PyTuple_Check(pyVal)) {
                if (PyTuple_Size(pyVal) == array_size) {
                    for (size_t i = 0; i < array_size; ++i)
                        (_this->template get_CObject<CClass_NoRef>()->*
                         member)[i] = *toCObject<T, false, PythonClassWrapper<T> >(
                                *PyTuple_GetItem(pyVal, i));
                } else {
                    static char msg[250];
                    snprintf(msg, 250, "Mismatched array sizes (tuple)%lld!=%lld",
                             (long long) PyTuple_Size(pyVal),
                             (long long) array_size);
                    PyErr_SetString(PyExc_IndexError, msg);
                    return nullptr;
                }
            } else if (PythonClassWrapper<T_array>::checkType(pyVal)) {
                T_array *val = ((PythonClassWrapper<T_array> *) pyVal)->template get_CObject<T_array>();
                //TODO: check size????
                for (size_t i = 0; i < array_size; ++i)
                    (_this->template get_CObject<T_array>()->*member)[i] = (*val)[i];

            } else if ((std::is_reference<T>::value && (!kwds || PyDict_Size(kwds)==0))||
                      (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "as_ref"))) {
                //handle case if oject is to be returned by reference
                bool asRef = std::is_reference<T>::value ;
                if (PyDict_Size(kwds)==1) {
                    PyObject *pyVal = PyDict_GetItemString(kwds, "as_ref");
                    if (pyVal == Py_None) {
                        PyErr_SetString(PyExc_SyntaxError, "Unexpected None value in member setter");
                        return nullptr;
                    }
                    if (PyBool_Check(pyVal)) {
                        asRef = (pyVal == Py_True);
                    } else {
                        PyErr_SetString(PyExc_TypeError, "Expected boolean value for 'as_ref'");
                        return nullptr;
                    }
                    if (_this->template get_CObject<CClass>()) {
                        asRef = asRef && !std::is_fundamental<T>::value;
                        PyObject* obj = toPyObject<T[]>(*(_this->template get_CObject<CClass>()->*member), asRef, array_size);
                        if (asRef) {
                            ((PythonClassWrapper<T>*)obj)->make_reference(self);
                        }
			return obj;
                    }
                    PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
                    return nullptr;
                }
            }  else {
                PyErr_SetString(PyExc_TypeError, "Invalid argument type when setting array attribute");
                return nullptr;
            }
        } else {
            PyErr_SetString(PyExc_SyntaxError, "Invalid arguments to set class instance member variable in C");
            return nullptr;
        }
        return Py_None;
    }

    template<class CClass>
    template<const char *const name, typename T>
    void MemberContainer<CClass>::Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type>::
    setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
        smart_ptr<T[], false> val = toCObject<T[], false, PythonClassWrapper<T[]> >(*pyobj);
        for (size_t i = 0; i < array_size; ++i) {
            (self->*member)[i] = (*val)[i];
        }
    }


    template<class CClass>
    template<const char *const name, typename T>
    typename MemberContainer<CClass>::template Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0>::type>::member_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0>::type>::member;

    template<class CClass>
    template<const char *const name, typename T>
    typename MemberContainer<CClass>::template Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::member_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::member;

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    typename MemberContainer<CClass>::template Container<name, T[size], void>::member_t
            MemberContainer<CClass>::Container<name, T[size], void>::member;

    template<class CClass>
    template<const char *const name, typename T>
    typename MemberContainer<CClass>::template Container<name, T, typename std::enable_if<
            std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::member_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::member;

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    typename MemberContainer<CClass>::template Container<name, const T[size], void>::member_t
            MemberContainer<CClass>::Container<name, const T[size], void>::member;

    template<class CClass>
    template<const char *const name, typename T>
    typename MemberContainer<CClass>::template Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type>::member_t
            MemberContainer<CClass>::Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type>::member;

    template<class CClass>
    template<const char *const name, typename T>
    typename MemberContainer<CClass>::template Container<name, T[], typename std::enable_if<std::is_const<T>::value>::type>::member_t
            MemberContainer<CClass>::Container<name, T[], typename std::enable_if<std::is_const<T>::value>::type>::member;


    template<class CClass>
    template<const char *const name, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    !std::is_const<T>::value && !std::is_array<T>::value &&
                    Sizeof<T>::value == 0>::type>::array_size = 0;

    template<class CClass>
    template<const char *const name, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    !std::is_const<T>::value && !std::is_array<T>::value &&
                    Sizeof<T>::value != 0>::type>::array_size = 0;

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T[size], void>::array_size = 0;

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    size_t
            MemberContainer<CClass>::Container<name, const T[size], void>::array_size = 0;

    template<class CClass>
    template<const char *const name, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    std::is_const<T>::value && !std::is_array<T>::value &&
                    Sizeof<T>::value != 0>::type>::array_size = 0;

    template<class CClass>
    template<const char *const name, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type>::array_size = 0;

    template<class CClass>
    template<const char *const name, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T[], typename std::enable_if<std::is_const<T>::value>::type>::array_size = 0;


    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    PyObject *BitFieldContainer<CClass>::Container<name, T, bits>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;

        if (!std::is_reference<T>::value && (!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
            if (_this->template get_CObject<CClass>()) {
                return toPyObject<T>(_getter(*(_this->template get_CObject<CClass>())), false, 1);
            }
            PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
            return nullptr;
        } else if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "set_to")) {
            PyObject *pyVal = PyDict_GetItemString(kwds, "set_to");
            if (pyVal == Py_None) {
                PyErr_SetString(PyExc_SyntaxError, "Unexpected None value in member setter");
                return nullptr;
            }
            smart_ptr<T, false> value = toCObject<T, false, PythonClassWrapper<T> >(*pyVal);
            if (!BitFieldLimits<T, bits>::is_in_bounds(*value)) {
                PyErr_SetString(PyExc_ValueError, "Value out of bounds");
                return nullptr;
            }
            _setter(*(_this->template get_CObject<CClass_NoRef>()), *value);
        }  else if ((std::is_reference<T>::value && (!kwds || PyDict_Size(kwds)==0))||
                   (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "as_ref"))) {
            PyErr_SetString(PyExc_SyntaxError, "Cannot get reference to bit field by reference");
            return nullptr;
        } else {
            PyErr_SetString(PyExc_SyntaxError, "Invalid arguments to set class instance member variable in C");
            return nullptr;
        }
        return Py_None;
    }

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    void BitFieldContainer<CClass>::Container<name, T, bits>::
    setFromPyObject(CClass_NoRef *self, PyObject *pyobj) {
        smart_ptr<T, false> value = toCObject<T, false, PythonClassWrapper<T> >(*pyobj);
        if (!BitFieldLimits<T, bits>::is_in_bounds(*value)) {
            throw "Value out of bounds";
        }
        _setter(*self, *value);
    }

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    PyObject *BitFieldContainer<CClass>::ConstContainer<name, T, bits>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;

        if ((!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
            if (_this->template get_CObject<CClass>()) {
                return toPyObject<T>(_getter(*(_this->template get_CObject<CClass>())), false, 1);
            }
            PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
            return nullptr;
        } else if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "set_to")) {
            PyErr_SetString(PyExc_SyntaxError, "Cannot set const bit field");
            return nullptr;
        }
        return Py_None;
    }

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    void BitFieldContainer<CClass>::ConstContainer<name, T, bits>::
    setFromPyObject(CClass_NoRef *self, PyObject *pyobj) {
        (void) self;
        (void) pyobj;
        throw "Cannot set const bit field";
    }


    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    typename BitFieldContainer<CClass>::template Container<name, T, bits>::getter_t BitFieldContainer<CClass>::Container<name, T, bits>::_getter;

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    typename BitFieldContainer<CClass>::template Container<name, T, bits>::setter_t BitFieldContainer<CClass>::Container<name, T, bits>::_setter;

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    typename BitFieldContainer<CClass>::template ConstContainer<name, T, bits>::getter_t BitFieldContainer<CClass>::ConstContainer<name, T, bits>::_getter;

}

#endif
