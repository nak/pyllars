//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__METHODCALLSEMANTICS_CPP_
#define __PYLLARS_INTERNAL__METHODCALLSEMANTICS_CPP_

#include "pyllars_methodcallsemantics.hpp"
#include "pyllars_classwrapper.cpp"

namespace __pyllars_internal {

    template< bool is_const, typename CClass, typename T, typename ...Args>
    typename MethodCallSemantics<is_const, true, CClass, T, Args...>::ReturnType MethodCallSemantics<is_const, true, CClass, T, Args...>::
    call( method_t method, typename std::remove_reference<CClass>::type &self, Args... args, PyObject *extra_args) {
        typedef typename std::remove_reference<ReturnType>::type ReturnType_NoRef;
        /*if (!extra_args || PyTuple_Size(extra_args) == 0) {
            return self.*method(args...);
        } else*/  {  // bug in g++ forces call this way:
            const ssize_t extra_args_size = PyTuple_Size(extra_args);

            ffi_cif cif;
            ffi_type *arg_types[sizeof...(Args) + extra_args_size] = {FFIType<Args>::type()...};
            void *arg_values[sizeof...(Args) + extra_args_size + 1] = {(void*)&self, (void *) &args...};
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
            for (size_t i = sizeof...(Args) + 1; i < sizeof...(Args) + extra_args_size + 1; ++i) {
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
                            typedef typename PythonFunctionWrapper<true, true, int, int>::template Wrapper<> wtype;
                            typedef typename PythonFunctionWrapper<true, true, int, int>::template Wrapper<>::FuncContainer ftype;
                            static const size_t offset = offset_of<ftype , wtype >(&wtype::_cfunc);
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
            ffi_call(&cif, FFI_FN(method), result, arg_values);

            return *((ReturnType_NoRef *) result);

        }
    }

    template<bool is_const, typename CClass, typename ...Args>
    void MethodCallSemantics<is_const, true, CClass, void, Args...>::
    call(method_t method, typename std::remove_reference<CClass>::type &self, Args... args, PyObject *extra_args) {
        if (!extra_args || PyTuple_Size(extra_args) == 0) {
            self.*method(args...);
        } else {
            const ssize_t extra_args_size = PyTuple_Size(extra_args);

            ffi_cif cif;
            ffi_type *arg_types[sizeof...(Args) + extra_args_size] = {FFIType<Args>::type()...};
            void *arg_values[sizeof...(Args) + extra_args_size + 1] = {(void*)&self, (void *) &args...};
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
            for (size_t i = sizeof...(Args) + 1; i < sizeof...(Args) + extra_args_size + 1; ++i) {
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
                            typedef typename PythonFunctionWrapper<true, true, int, int>::template Wrapper<> wtype;
                            typedef typename PythonFunctionWrapper<true, true, int, int>::template Wrapper<>::FuncContainer ftype;
                            static const size_t offset = offset_of<ftype , wtype >(&wtype::_cfunc);
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
            ffi_call(&cif, FFI_FN(method), result, arg_values);

        }
    }


    template<bool is_const, typename CClass, typename T, typename ... Args>
    PyObject *MethodCallSemantics<is_const, false, CClass, T, Args...>::
    call(method_t method, typename std::remove_reference<CClass>::type &self, PyObject *args, PyObject *kwds) {
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

    template<bool is_const,typename CClass, typename T, typename ... Args>
    PyObject *MethodCallSemantics<is_const, true, CClass, T, Args...>::
    call(method_t method, typename std::remove_reference<CClass>::type &self, PyObject *args, PyObject *kwds) {
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

    template<bool is_const,typename CClass,  typename ... Args>
    PyObject *MethodCallSemantics<is_const, false, CClass, void, Args...>::
    call(method_t method, typename std::remove_reference<CClass>::type &self, PyObject *args, PyObject *kwds) {
        try {
            call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type());

        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
        return Py_None;
    }



    template<bool is_const, typename CClass, typename T, typename ... Args>
    template<typename ...PyO>
    typename extent_as_pointer<T>::type
    MethodCallSemantics<is_const, false, CClass, T, Args...>::
    call_methodC(
            method_t method,
            typename std::remove_reference<CClass>::type &self,
            PyObject *args, PyObject *kwds, PyO *...pyargs) {
        static char format[sizeof...(Args) + 1] = {0};



        if (sizeof...(Args) > 0)
            memset(format, 'O', sizeof...(Args));
        if (kwds && !PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...)) {
            PyErr_Print();
            throw "Invalid arguments to method call";
        } else if (!kwds && !PyArg_ParseTuple(args, format, &pyargs...) ){
	        PyErr_Print();
	        throw "Invalid arguments to method call";
	    }

        T retval = make_call( self, method, *toCObject<Args, false, PythonClassWrapper<Args> >(*pyargs)...);
        return retval;
    }



    template<bool is_const, typename CClass, typename T, typename ... Args>
    template<typename ...PyO>
    typename extent_as_pointer<T>::type
    MethodCallSemantics<is_const, true, CClass, T, Args...>::
    call_methodC(
            method_t method,
            typename std::remove_reference<CClass>::type &self,
            PyObject *args, PyObject *kwds, PyO *...pyargs) {
        static char format[sizeof...(Args) + 1] = {0};
        const size_t arg_count = kwds ? PyDict_Size(kwds) : 0 + args ? PyTuple_Size(args) : 0;
        const size_t kwd_count = kwds ? PyDict_Size(kwds) : 0;
        PyObject *extra_args = nullptr;
        PyObject *tuple = nullptr;

        tuple = PyTuple_GetSlice(args, 0, sizeof...(Args) - kwd_count);
        extra_args = PyTuple_GetSlice(args, sizeof...(Args) - kwd_count, arg_count);

        if (sizeof...(Args) > 0)
            memset(format, 'O', sizeof...(Args));
        if (kwds && !PyArg_ParseTupleAndKeywords(tuple, kwds, format, (char **) kwlist, &pyargs...)) {
            PyErr_Print();
            throw "Invalid arguments to method call";
        } else if (!kwds && !PyArg_ParseTuple(tuple, format, &pyargs...) ){
	        PyErr_Print();
	        throw "Invalid arguments to method call";
	    }
        T retval = call( method, self,  *toCObject<Args, false, PythonClassWrapper<Args> >(*pyargs)..., extra_args);
        return retval;

    }


    /**
     * call that converts python given arguments to make C call:
     **/
    template<bool is_const,typename CClass, typename T, typename ... Args>
    template<int ...S>
    typename extent_as_pointer<T>::type
    MethodCallSemantics<is_const, false, CClass, T, Args...>::
    call_methodBase(
            method_t method,
            typename std::remove_reference<CClass>::type &self,
            PyObject *args, PyObject *kwds, container<S...> s) {
        (void) s;
        PyObject pyobjs[sizeof...(Args) + 1];
        (void) pyobjs;
	if(!method){
	  throw "Null method pointer encountered";
	}
        return call_methodC(method, self, args, kwds, &pyobjs[S]...);

    }

    template<bool is_const, class CClass, typename ReturnType, typename ...Args>
    const char *const *
            MethodCallSemantics<is_const, false, CClass, ReturnType, Args...>::kwlist;


    template<bool is_const, typename CClass, typename ...Args>
    PyObject *MethodCallSemantics<is_const, false, CClass, void, Args...>::
    toPyObj(CClass &self) {
        (void) self;
        return Py_None;
    }

     /**
     * call that converts python given arguments to make C call:
     **/
    template<bool is_const, typename CClass, typename T, typename ... Args>
    template<int ...S>
    typename extent_as_pointer<T>::type
    MethodCallSemantics<is_const, true, CClass, T, Args...>::
    call_methodBase(
            method_t method,
            typename std::remove_reference<CClass>::type &self,
            PyObject *args, PyObject *kwds, container<S...> s) {
        (void) s;
        PyObject pyobjs[sizeof...(Args) + 1];
        (void) pyobjs;
	if(!method){
	  throw "Null method pointer encountered";
	}
        return call_methodC(method, self, args, kwds, &pyobjs[S]...);

    }

    template<bool is_const, class CClass, typename ReturnType, typename ...Args>
    const char *const *
            MethodCallSemantics<is_const, true, CClass, ReturnType, Args...>::kwlist;


    template<bool is_const, typename CClass, typename ...Args>
    PyObject *MethodCallSemantics<is_const, true, CClass, void, Args...>::
    toPyObj(CClass &self) {
        (void) self;
        return Py_None;
    }

    template<bool is_const, typename CClass, typename ...Args>
    template<typename ...PyO>
    void MethodCallSemantics<is_const, false, CClass, void, Args...>::
    call_methodC(method_t method,
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

    template<bool is_const, typename CClass, typename ...Args>
    template<int ...S>
    void MethodCallSemantics<is_const, false, CClass, void, Args...>::
    call_methodBase(method_t method,
                    typename std::remove_reference<CClass>::type &self,
                    PyObject *args, PyObject *kwds,
                    container<S...> unused) {
        (void) unused;
        PyObject pyobjs[sizeof...(Args) + 1];
        call_methodC(method, self, args, kwds, &pyobjs[S]...);
        (void) pyobjs;
    }


   template<bool is_const, typename CClass, typename ...Args>
    template<typename ...PyO>
    void MethodCallSemantics<is_const, true, CClass, void, Args...>::
    call_methodC(method_t method,
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

    template<bool is_const, typename CClass, typename ...Args>
    template<int ...S>
    void MethodCallSemantics<is_const, true, CClass, void, Args...>::
    call_methodBase(method_t method,
                    typename std::remove_reference<CClass>::type &self,
                    PyObject *args, PyObject *kwds,
                    container<S...> unused) {
        (void) unused;
        PyObject pyobjs[sizeof...(Args) + 1];
        call_methodC(method, self, args, kwds, &pyobjs[S]...);
        (void) pyobjs;
    }


    template<bool is_const, class CClass, typename ...Args>
    const char *const *MethodCallSemantics<is_const, false, CClass, void, Args...>::kwlist;

   template<bool is_const, class CClass, typename ...Args>
    const char *const *MethodCallSemantics<is_const, true, CClass, void, Args...>::kwlist;


    template<class CClass, bool is_const>
    template<const char *const name, typename ReturnType, typename ...Args>
    PyObject *MethodContainer<CClass, is_const, typename std::enable_if<
            std::is_class<CClass>::value &&
            !std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        if (_this->template get_CObject<CClass>()) {
            try {
                return MethodCallSemantics<is_const, false, CClass, ReturnType, Args...>::call(method,
                                                                              *_this->template get_CObject<CClass>(),
                                                                              args, kwds);
            } catch (...) {
                return nullptr;
            }
        }
        return nullptr;
    }

    template<typename CClass, bool is_const>
    template<const char *const name, typename ReturnType, typename ...Args>
    typename MethodContainer<CClass, is_const, typename std::enable_if<std::is_class<CClass>::value &&
                                                             !std::is_const<CClass>::value>::type>::template Container<name, ReturnType, Args...>::method_t
            MethodContainer<CClass, is_const, typename std::enable_if<std::is_class<CClass>::value &&
                                                            !std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::method;


    template<class CClass, bool is_const>
    template<const char *const name, typename ReturnType, typename ...Args>
    PyObject *MethodContainer<CClass, is_const, typename std::enable_if<
            std::is_class<CClass>::value && std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;
        if (_this->template get_CObject<CClass>()) {
            try {
                return MethodCallSemantics<is_const, false, CClass, ReturnType, Args...>::call(method,
                                                                              *_this->template get_CObject<CClass>(),
                                                                              args, kwds);
            } catch (...) {
                return nullptr;
            }
        }
        return nullptr;
    }

    template<class CClass, bool is_const>
    template<const char *const name, typename ReturnType, typename ...Args>
    typename MethodContainer<CClass, is_const, typename std::enable_if<std::is_class<CClass>::value &&
                                                             std::is_const<CClass>::value>::type>::template Container<name, ReturnType, Args...>::method_t
            MethodContainer<CClass, is_const, typename std::enable_if<std::is_class<CClass>::value &&
                                                            std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::method;


//////////////////////////////////////////////


    template<class CClass, bool is_const>
    template<const char *const name, typename ReturnType, typename ...Args>
    PyObject *MethodContainerVarargs<CClass, is_const, typename std::enable_if<
            std::is_class<CClass>::value &&
            !std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        if (_this->template get_CObject<CClass>()) {
            try {
                return MethodCallSemantics<is_const, true, CClass, ReturnType, Args...>::call(method,
                                                                              *_this->template get_CObject<CClass>(),
                                                                              args, kwds);
            } catch (...) {
                return nullptr;
            }
        }
        return nullptr;
    }

    template<typename CClass, bool is_const>
    template<const char *const name, typename ReturnType, typename ...Args>
    typename MethodContainerVarargs<CClass, is_const, typename std::enable_if<std::is_class<CClass>::value &&
                                                             !std::is_const<CClass>::value>::type>::template Container<name, ReturnType, Args...>::method_t
            MethodContainerVarargs<CClass, is_const, typename std::enable_if<std::is_class<CClass>::value &&
                                                            !std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::method;


    template<class CClass, bool is_const>
    template<const char *const name, typename ReturnType, typename ...Args>
    PyObject *MethodContainerVarargs<CClass, is_const, typename std::enable_if<
            std::is_class<CClass>::value && std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;
        if (_this->template get_CObject<CClass>()) {
            try {
                return MethodCallSemantics<is_const, true, CClass, ReturnType, Args...>::call(method,
                                                                              *_this->template get_CObject<CClass>(),
                                                                              args, kwds);
            } catch (...) {
                return nullptr;
            }
        }
        return nullptr;
    }

    template<class CClass, bool is_const>
    template<const char *const name, typename ReturnType, typename ...Args>
    typename MethodContainerVarargs<CClass, is_const, typename std::enable_if<std::is_class<CClass>::value &&
                                                             std::is_const<CClass>::value>::type>::template Container<name, ReturnType, Args...>::method_t
            MethodContainerVarargs<CClass, is_const, typename std::enable_if<std::is_class<CClass>::value &&
                                                            std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::method;



/////////////////////////////////////////////


    template<class CClass>
    template<const char *const name, typename T>
    PyObject *MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::
    get(PyObject *self) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        const ssize_t base_size = ArrayHelper<T>::base_sizeof();
        const ssize_t array_size = base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                                                 : UNKNOWN_SIZE;
        if (_this->template get_CObject<CClass>()) {
            return toPyObject<T>(_this->template get_CObject<CClass>()->*member, true, array_size);
        }
        PyErr_SetString(PyExc_RuntimeError, "Internal Pyllars Error: No C Object found to get member attribute value!");
        return nullptr;
    }


    template<class CClass>
    template<const char *const name, typename T>
    int MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::
    set(PyObject *self, PyObject* pyVal) {
        if (!self) {
            PyErr_SetString(PyExc_RuntimeError, "Unexpceted nullptr value for self");
            return -1;
        }
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        if (pyVal == Py_None) {
            PyErr_SetString(PyExc_ValueError, "Unexpected None value in member setter");
            return -1;
        }
        try {
            Assign<T>::assign((_this->template get_CObject<CClass_NoRef>()->*member),
                              *toCObject<T, false, PythonClassWrapper<T> >(*pyVal));
        }catch(const char* const msg){
            PyErr_SetString(PyExc_RuntimeError, msg);
            return -1;
        }
        return 0;
    }

    template<class CClass>
    template<const char *const name, typename T>
    void MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::
    setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
        Assign<T>::assign(self->*member , *toCObject<T, false, PythonClassWrapper<T> >(*pyobj));
    }

    template<class CClass>
    template<const char *const name, typename T>
    PyObject *MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0>::type>::
    get(PyObject *self) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        const ssize_t base_size = ArrayHelper<T>::base_sizeof();
        const ssize_t array_size =
                base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                              : UNKNOWN_SIZE;
        if (_this->template get_CObject<CClass>()) {
            return toPyObject<T>(_this->template get_CObject<CClass>()->*member, false, array_size);
        }
        PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
        return nullptr;
    }

    template<class CClass>
    template<const char *const name, typename T>
    int MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0>::type>::
    set(PyObject *self, PyObject *pyVal) {
       //nothing to do for zero size item
        return 0;

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
    get(PyObject *self) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        if (_this->template get_CObject<CClass_NoRef>()) {
            const ssize_t base_size = ArrayHelper<T>::base_sizeof();
            const ssize_t array_size =
                    base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                                  : UNKNOWN_SIZE;
            return toPyObject<T>(_this->template get_CObject<CClass_NoRef>()->*member, true, array_size);
        }
        PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
        return nullptr;
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
    get(PyObject *self) {
        try {
            if (!self) return nullptr;
            PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;

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

               PyObject* obj = toPyObject<T_array>(_this->template get_CObject<CClass>()->*member, false,
                                            array_size);
                ((PythonClassWrapper<T> *) obj)->make_reference(self);
                return obj;
            }
            PyErr_SetString(PyExc_RuntimeError, "Internal Error:No C Object found to get member attribute value!");
            return nullptr;
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    int MemberContainer<CClass>::Container<name, T[size], void>::
    set(PyObject *self, PyObject* pyVal) {
        PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;
        try{
            if (pyVal == Py_None) {
                PyErr_SetString(PyExc_ValueError, "Unexpcted None value in member setter");
                return -1;
            }
            if (PyTuple_Check(pyVal)) {
                if (PyTuple_Size(pyVal) == size) {
                    for (size_t i = 0; i < size; ++i)
                        Assign<T>::assign((_this->template get_CObject<CClass_NoRef>()->*
                         member)[i],  *toCObject<T, true, PythonClassWrapper<T> >(
                                *PyTuple_GetItem(pyVal, i)));
                } else {
                    static char msg[250];
                    snprintf(msg, 250, "Mismatched array sizes (tuple)%lld!=%lld",
                             (long long) PyTuple_Size(pyVal),
                             (long long) size);
                    PyErr_SetString(PyExc_IndexError, msg);
                    return -1;
                }
            } else if (PythonClassWrapper<T_array>::checkType(pyVal)) {
                T_array *val = ((PythonClassWrapper<T_array> *) pyVal)->template get_CObject<T_array>();
                for (size_t i = 0; i < size; ++i)
                    Assign<T>::assign((_this->template get_CObject<T_array>()->*member)[i] , (*val)[i]);

            }
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return -1;
        }
        return 0;
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
    get(PyObject *self) {
        if (!self) return nullptr;
        PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;

        const ssize_t base_size = ArrayHelper<T_array>::base_sizeof();
        const ssize_t array_size =
                base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                              : UNKNOWN_SIZE;
        if (array_size != size) {
            PyErr_SetString(PyExc_TypeError, "Mismatched array sizes");
            return nullptr;
        }
        if (_this->template get_CObject<CClass>()) {
            return toPyObject<T_array, size>(*(_this->template get_CObject<CClass>()->*member), true,
                                             array_size);
        }

        PyErr_SetString(PyExc_RuntimeError, "Cannot set object element!");
        return nullptr;

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
    get(PyObject *self) {
        if (!self) return nullptr;
        PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;

        if (_this->template get_CObject<CClass>()) {
            PyObject *obj = toPyObject<T[]>(*(_this->template get_CObject<CClass>()->*member), true, array_size);
            ((PythonClassWrapper<T> *) obj)->make_reference(self);
            return obj;
        }
        PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
        return nullptr;
    }


    template<class CClass>
    template<const char *const name, typename T>
    int MemberContainer<CClass>::Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type>::
    set(PyObject *self, PyObject* pyVal) {
        if (!self) return nullptr;
        PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;
        try{
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
                        Assign<T>::assign((_this->template get_CObject<CClass_NoRef>()->*
                                           member)[i], *toCObject<T, false, PythonClassWrapper<T> >(
                                *PyTuple_GetItem(pyVal, i)));
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
                    Assign<T>::assign((_this->template get_CObject<T_array>()->*member)[i], (*val)[i]);

            } else {
                PyErr_SetString(PyExc_ValueError, "Invalid argument type when setting attribute");
                return -1;
            }
        } catch( const char* const msg){
            PyErr_SetString(PyExc_RuntimeError, msg);
            return -1;
        }
        return 0;
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
    get(PyObject *self) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;

        if (_this->template get_CObject<CClass>()) {
            return toPyObject<T>(_getter(*(_this->template get_CObject<CClass>())), false, 1);
        }
        PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
        return nullptr;
    }

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    int BitFieldContainer<CClass>::Container<name, T, bits>::
    set(PyObject *self, PyObject* pyVal) {
        if (!self) {
            PyErr_SetString(PyExc_RuntimeError, "Unexpected null value for self");
            return -1;
        }
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        if (pyVal == Py_None) {
            PyErr_SetString(PyExc_ValueError, "Unexpected None value in member setter");
            return -1;
        }
        smart_ptr<T, false> value = toCObject<T, false, PythonClassWrapper<T> >(*pyVal);
        if (!BitFieldLimits<T, bits>::is_in_bounds(*value)) {
            PyErr_SetString(PyExc_ValueError, "Value out of bounds");
            return -1;
        }
        _setter(*(_this->template get_CObject<CClass_NoRef>()), *value);
        return 0;
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
    get(PyObject *self) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;

        if (_this->template get_CObject<CClass>()) {
            return toPyObject<T>(_getter(*(_this->template get_CObject<CClass>())), false, 1);
        }
        PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
        return nullptr;
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
