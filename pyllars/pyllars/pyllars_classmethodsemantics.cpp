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
    template<bool with_ellipsis, typename CClass, typename T, typename ... Args>
    PyObject *ClassMethodCallSemantics<with_ellipsis, CClass, T,Args...>::
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
    template<bool with_ellipsis, typename CClass, typename T, typename ... Args>
    template<typename ...PyO>
    typename extent_as_pointer<T>::type
    ClassMethodCallSemantics<with_ellipsis, CClass, T,Args...>::
    call_methodC(typename FuncDef<with_ellipsis,0>::func_type method, PyObject *args, PyObject *kwds, PyO *...pyargs) {
        static char format[sizeof...(Args) + 1] = {0};
        memset(format, 'O', (size_t)
        sizeof...(Args));
        const size_t arg_count = kwds ? PyDict_Size(kwds) : 0 + args ? PyTuple_Size(args) : 0;
        const size_t kwd_count = kwds ? PyDict_Size(kwds) : 0;
        PyObject *extra_args = nullptr;
        PyObject *tuple = nullptr;

        if ((arg_count > sizeof...(Args)) && with_ellipsis) {
            tuple = PyTuple_GetSlice(args, 0, sizeof...(Args) - kwd_count);
            extra_args = PyTuple_GetSlice(args, sizeof...(Args) - kwd_count, arg_count);
        } else {
            tuple = args;
        }

        if (sizeof...(Args)> 0 && !PyArg_ParseTupleAndKeywords(tuple, kwds, format, (char **) kwlist, &pyargs...)) {
            throw "Invalid arguments to method call";
        }

        if( with_ellipsis){
            return FunctType::call(method, *toCObject<Args, false, PythonClassWrapper<Args> >(*pyargs)..., extra_args);
        }
        return method(*toCObject<Args, false, PythonClassWrapper<Args> >(*pyargs)...);
    }

    /**
     * call that converts python given arguments to make C call:
     **/
    template<bool with_ellipsis, typename CClass, typename T, typename ... Args>
    template<int ...S>
    typename extent_as_pointer<T>::type
    ClassMethodCallSemantics<with_ellipsis, CClass, T,Args...>::
    call_methodBase(typename FuncDef<with_ellipsis,0>::func_type method,
                    PyObject *args, PyObject *kwds, container<S...> s) {
        (void) s;
        PyObject pyobjs[sizeof...(Args) + 1];
        (void) pyobjs;
        return call_methodC(method, args, kwds, &pyobjs[S]...);
    }

    template<bool with_ellipsis, class CClass, typename ReturnType, typename ...Args>
    const char *const *ClassMethodCallSemantics<with_ellipsis, CClass, ReturnType, Args...>::kwlist;



    template<bool with_ellipsis, typename CClass, typename ...Args>
    PyObject *ClassMethodCallSemantics<with_ellipsis, CClass, void, Args...>::
    call(method_t method, PyObject *args, PyObject *kwds) {
        call_methodBase(method, args, kwds, typename argGenerator<sizeof...(Args)>::type());
        return Py_None;
    }


    template<bool with_ellipsis, typename CClass, typename ...Args>
    template<typename ...PyO>
    void ClassMethodCallSemantics<with_ellipsis, CClass, void, Args...>::
    call_methodC(typename FuncDef<with_ellipsis,0>::func_type method,
                 PyObject *args, PyObject *kwds,
                 PyO *...pyargs) {

        static char format[sizeof...(Args) + 1] = {0};
        memset(format, 'O', (size_t) sizeof...(Args));
        const size_t arg_count = kwds ? PyDict_Size(kwds) : 0 + args ? PyTuple_Size(args) : 0;
        const size_t kwd_count = kwds ? PyDict_Size(kwds) : 0;
        PyObject *extra_args = nullptr;
        PyObject *tuple = nullptr;

        if ((arg_count > sizeof...(Args)) && with_ellipsis) {
            tuple = PyTuple_GetSlice(args, 0, sizeof...(Args) - kwd_count);
            extra_args = PyTuple_GetSlice(args, sizeof...(Args) - kwd_count, arg_count);
        } else {
            tuple = args;
        }

        if (sizeof...(Args) && !PyArg_ParseTupleAndKeywords(tuple, kwds, format, (char **) kwlist, &pyargs...)) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to parse argument on method call");
            throw "Invalid arguments to method call";
        }
        if(with_ellipsis) {
            FunctType::call(method, *toCObject<Args, false, PythonClassWrapper<Args> >(*pyargs)..., extra_args);
        } else {
            method(*toCObject<Args, false, PythonClassWrapper<Args> >(*pyargs)...);
        }
    }

    template<bool with_ellipsis, typename CClass, typename ...Args>
    template<int ...S>
    void ClassMethodCallSemantics<with_ellipsis, CClass, void, Args...>::
    call_methodBase(typename FuncDef<with_ellipsis, 0>::func_type method,
                    PyObject *args, PyObject *kwds,
                    container<S...> unused) {
        (void) unused;
        PyObject pyobjs[sizeof...(Args) + 1];
        call_methodC(method, args, kwds, &pyobjs[S]...);
        (void) pyobjs;
    }

    template<bool with_ellipsis, class CClass, typename ...Args>
    const char *const *ClassMethodCallSemantics<with_ellipsis, CClass, void, Args...>::kwlist;

    template<bool with_ellipsis, typename CClass, typename T, typename ...Args>
    typename extent_as_pointer<T>::type ClassMethodCallSemantics<with_ellipsis, CClass, T, Args...>::FunctType::
    call(func_type func, Args... args, PyObject *extra_args) {
        typedef typename std::remove_reference<ReturnType>::type ReturnType_NoRef;
        if (!extra_args || PyTuple_Size(extra_args) == 0) {
            return func(args...);
        } else {
            const ssize_t extra_args_size = PyTuple_Size(extra_args);

            ffi_cif cif;
            ffi_type *arg_types[sizeof...(Args) + extra_args_size] = {FFIType<Args>::type()...};
            void *arg_values[sizeof...(Args) + extra_args_size] = {(void *) &args...};
            ffi_status status;

            // Because the return value from foo() is smaller than sizeof(long), it
            // must be passed as ffi_arg or ffi_sarg.
            ffi_arg result_small;
            unsigned char result_big[sizeof(ReturnType_NoRef)];
            void *result = (sizeof(ReturnType_NoRef) >= sizeof(ffi_arg)) ? (void *) &result_big[0]
                                                                         : (void *) &result_small;
	    
            // Specify the data type of each argument. Available types are defined
            // in <ffi/ffi.h>.
            union ArgType {
                int intvalue;
                long longvalue;
                double doublevalue;
                bool boolvalue;
                void *ptrvalue;
            };
            ArgType extra_arg_values[sizeof...(Args) + extra_args_size];
            for (size_t i = sizeof...(Args); i < sizeof...(Args) + extra_args_size; ++i) {
                PyObject *const nextArg = PyTuple_GetItem(extra_args, i - sizeof...(Args));
                const int subtype = getType(nextArg, arg_types[i]);
                switch (arg_types[i]->type) {
                    case FFI_TYPE_SINT32:
                        extra_arg_values[i].intvalue = PyInt_AsLong(nextArg);
                        arg_values[i] = &extra_arg_values[i].intvalue;
                        break;
                    case FFI_TYPE_SINT64:
                        extra_arg_values[i].longvalue = PyLong_AsLong(nextArg);
                        arg_values[i] = &extra_arg_values[i].longvalue;
                        break;
                    case FFI_TYPE_UINT8:
                        extra_arg_values[i].boolvalue = (nextArg == Py_True);
                        arg_values[i] = &extra_arg_values[i].boolvalue;
                        break;
                    case FFI_TYPE_DOUBLE:
                        extra_arg_values[i].doublevalue = PyFloat_AsDouble(nextArg);
                        arg_values[i] = &extra_arg_values[i].doublevalue;
                        break;
                    case FFI_TYPE_POINTER:
                        if (STRING_TYPE == subtype) {
                            extra_arg_values[i].ptrvalue = PyString_AsString(nextArg);
                            arg_values[i] = &extra_arg_values[i].ptrvalue;
                        } else if (COBJ_TYPE == subtype) {
                            static const size_t offset = offset_of<ObjContainer<Arbitrary>*, PythonClassWrapper<Arbitrary> >
                                    (&PythonClassWrapper<Arbitrary>::_CObject);
                            ObjContainer<void *> **ptrvalue =
                                    (ObjContainer<void *> **) (((char *) nextArg) + offset);
                            extra_arg_values[i].ptrvalue = ptrvalue ? (*ptrvalue)->ptr() : nullptr;
                        } else if (FUNC_TYPE == subtype) {
                            static const size_t offset =
                                    offset_of<typename PythonFunctionWrapper<true, with_ellipsis, int, int>::template FuncDef<with_ellipsis,0>::func_type , PythonFunctionWrapper<true, with_ellipsis, int, int> >(&PythonFunctionWrapper<true, with_ellipsis, int, int> ::_cfunc);
                            void **ptrvalue = (void **) (((char *) nextArg) + offset);
                            extra_arg_values[i].ptrvalue = *ptrvalue;
                        } else {
                            throw "Unable to convert Python object to C Object";
                        }
                        arg_values[i] = &extra_arg_values[i].ptrvalue;
                        break;
                    default:
                        throw "Python object cannot be converted to C object";
                        break;
                }
            }

            ffi_type *return_type = FFIType<ReturnType>::type();
            // Prepare the ffi_cif structure.
            if ((status = ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI,
                                           sizeof...(Args), sizeof...(Args) + extra_args_size,
                                           return_type, arg_types)) != FFI_OK) {
                throw "FFI error calling variadic function";
            }

            // Invoke the function.
            ffi_call(&cif, FFI_FN(func), result, arg_values);

            return *((ReturnType_NoRef *) result);

        }
    }


    template<bool with_ellipsis, typename CClass, typename ...Args>
    void ClassMethodCallSemantics<with_ellipsis, CClass, void, Args...>::FunctType::
    call(func_type func, Args... args, PyObject *extra_args) {
        if (!extra_args || PyTuple_Size(extra_args) == 0) {
            func(args...);
        } else {
            const ssize_t extra_args_size = PyTuple_Size(extra_args);

            ffi_cif cif;
            ffi_type *arg_types[sizeof...(Args) + extra_args_size] = {FFIType<Args>::type()...};
            void *arg_values[sizeof...(Args) + extra_args_size] = {(void *) &args...};
            ffi_status status;

            // Because the return value from foo() is smaller than sizeof(long), it
            // must be passed as ffi_arg or ffi_sarg.
            ffi_arg result_small;
            void *result = &result_small;

            // Specify the data type of each argument. Available types are defined
            // in <ffi/ffi.h>.
            union ArgType {
                int intvalue;
                long longvalue;
                double doublevalue;
                bool boolvalue;
                void *ptrvalue;
            };
            ArgType extra_arg_values[sizeof...(Args) + extra_args_size];
            for (size_t i = sizeof...(Args); i < sizeof...(Args) + extra_args_size; ++i) {
                PyObject *const nextArg = PyTuple_GetItem(extra_args, i - sizeof...(Args));
                const int subtype = getType(nextArg, arg_types[i]);
                switch (arg_types[i]->type) {
                    case FFI_TYPE_SINT32:
                        extra_arg_values[i].intvalue = PyInt_AsLong(nextArg);
                        arg_values[i] = &extra_arg_values[i].intvalue;
                        break;
                    case FFI_TYPE_SINT64:
                        extra_arg_values[i].longvalue = PyLong_AsLong(nextArg);
                        arg_values[i] = &extra_arg_values[i].longvalue;
                        break;
                    case FFI_TYPE_UINT8:
                        extra_arg_values[i].boolvalue = (nextArg == Py_True);
                        arg_values[i] = &extra_arg_values[i].boolvalue;
                        break;
                    case FFI_TYPE_DOUBLE:
                        extra_arg_values[i].doublevalue = PyFloat_AsDouble(nextArg);
                        arg_values[i] = &extra_arg_values[i].doublevalue;
                        break;
                    case FFI_TYPE_POINTER:
                        if (STRING_TYPE == subtype) {
                            extra_arg_values[i].ptrvalue = PyString_AsString(nextArg);
                            arg_values[i] = &extra_arg_values[i].ptrvalue;
                        } else if (COBJ_TYPE == subtype) {
                            static const size_t offset = offset_of<ObjContainer<Arbitrary>*, PythonClassWrapper<Arbitrary> >
                                    (&PythonClassWrapper<Arbitrary>::_CObject);
                            ObjContainer<void *> **ptrvalue =
                                    (ObjContainer<void *> **) (((char *) nextArg) + offset);
                            extra_arg_values[i].ptrvalue = ptrvalue ? (*ptrvalue)->ptr() : nullptr;
                        } else if (FUNC_TYPE == subtype) {
                            static const size_t offset =
                                    offset_of<typename PythonFunctionWrapper<true, with_ellipsis, int, int>::template FuncDef<with_ellipsis,0>::func_type , PythonFunctionWrapper<true, with_ellipsis, int, int> >(&PythonFunctionWrapper<true, with_ellipsis, int, int> ::_cfunc);
                            void **ptrvalue = (void **) (((char *) nextArg) + offset);
                            extra_arg_values[i].ptrvalue = *ptrvalue;
                        } else {
                            throw "Unable to convert Python object to C Object";
                        }
                        arg_values[i] = &extra_arg_values[i].ptrvalue;
                        break;
                    default:
                        throw "Python object cannot be converted to C object";
                        break;
                }
            }
            ffi_type *return_type = &ffi_type_sint;
            // Prepare the ffi_cif structure.
            if ((status = ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI,
                                           sizeof...(Args), sizeof...(Args) + extra_args_size,
                                           return_type, arg_types)) != FFI_OK) {
                throw "FFI error calling variadic function";
            }
            // Invoke the function.
            ffi_call(&cif, FFI_FN(func), result, arg_values);
        }
    }


    template<class CClass>
    template<bool with_ellipsis, const char *const name, typename ReturnType, typename ...Args>
    PyObject *ClassMethodContainer<CClass, typename std::enable_if<
            std::is_class<CClass>::value && !std::is_const<CClass>::value>::type>::Container<with_ellipsis, name, ReturnType, Args...>::
    call(PyObject *cls, PyObject *args, PyObject *kwds) {
        (void) cls;
        try {
            return ClassMethodCallSemantics<with_ellipsis, CClass, ReturnType, Args...>::call(method, args, kwds);
        } catch (const char* const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
        return nullptr;
    }


    template<class CClass>
    template<bool with_ellipsis, const char *const name, typename ReturnType, typename ...Args>
    PyObject *ClassMethodContainer<CClass, typename std::enable_if<
            std::is_class<CClass>::value && std::is_const<CClass>::value>::type>::Container<with_ellipsis, name, ReturnType, Args...>::
    call(PyObject *cls, PyObject *args, PyObject *kwds) {
        (void) cls;
        try {
            return ClassMethodCallSemantics<with_ellipsis, CClass, ReturnType, Args...>::call(method, args, kwds);
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Exception in class method");
            return nullptr;
        }
        return nullptr;
    }

    template<class CClass>
    template<bool with_ellipsis, const char *const name, typename ReturnType, typename ...Args>
    typename ClassMethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                                  !std::is_const<CClass>::value>::type>::template Container<with_ellipsis, name, ReturnType, Args...>::method_t
            ClassMethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                                 !std::is_const<CClass>::value>::type>::Container<with_ellipsis, name, ReturnType, Args...>::method;


    template<class CClass>
    template<bool with_ellipsis, const char *const name, typename ReturnType, typename ...Args>
    typename ClassMethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                                  std::is_const<CClass>::value>::type>::template Container<with_ellipsis, name, ReturnType, Args...>::method_t
            ClassMethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                                 std::is_const<CClass>::value>::type>::Container<with_ellipsis, name, ReturnType, Args...>::method;



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
            Assign<T>::assign(*member,*toCObject<T, std::is_array<T>::value, PythonClassWrapper<T> >(*pyarg));
            return Py_None;
        }
        return toPyObject<T>(*member, false, ArraySize<T>::size);
    }

    template<class CClass>
    template<const char *const name, typename T>
    void ClassMemberContainer<CClass>::Container<name, T>::
    setFromPyObject(PyObject *pyobj) {
        Assign<T>::assign(*member ,*toCObject<T, false, PythonClassWrapper<T>>(*pyobj));
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
