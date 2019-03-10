//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__METHODCALLSEMANTICS_CPP_
#define __PYLLARS_INTERNAL__METHODCALLSEMANTICS_CPP_

#include "pyllars_methodcallsemantics.hpp"

#include "pyllars_classwrapper.impl.hpp"

namespace __pyllars_internal {

    template< bool is_const, const char* const kwlist[], typename CClass, typename T, typename ...Args>
    typename MethodCallSemantics<is_const, true, kwlist, CClass, T, Args...>::ReturnType
    MethodCallSemantics<is_const, true, kwlist, CClass, T, Args...>::
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
                            static const size_t offset = offset_of<ObjectContainer<Arbitrary>*, PythonClassWrapper<Arbitrary> >
                                    (&PythonClassWrapper<Arbitrary>::_CObject);
                            ObjectContainer<void *> **ptrvalue =
                                    (ObjectContainer<void *> **) (((char *) nextArg) + offset);
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

    template<bool is_const, const char* const kwlist[], typename CClass, typename ...Args>
    void MethodCallSemantics<is_const, true, kwlist, CClass, void, Args...>::
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
                            static const size_t offset = offset_of<ObjectContainer<Arbitrary>*, PythonClassWrapper<Arbitrary> >
                                    (&PythonClassWrapper<Arbitrary>::_CObject);
                            ObjectContainer<void *> **ptrvalue =
                                    (ObjectContainer<void *> **) (((char *) nextArg) + offset);
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

    template<bool is_const, const char* const kwlist[], typename CClass, typename T, typename ... Args>
    PyObject *MethodCallSemantics<is_const, false, kwlist, CClass, T, Args...>::
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

    template<bool is_const, const char* const kwlist[], typename CClass, typename T, typename ... Args>
    PyObject *MethodCallSemantics<is_const, true, kwlist, CClass, T, Args...>::
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

    template<bool is_const,const char* const kwlist[], typename CClass,  typename ... Args>
    PyObject *MethodCallSemantics<is_const, false, kwlist, CClass, void, Args...>::
    call(method_t method, typename std::remove_reference<CClass>::type &self, PyObject *args, PyObject *kwds) {
        try {
            call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type());

        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
        return Py_None;
    }

    template<bool is_const, const char* const kwlist[], typename CClass, typename T, typename ... Args>
    template<typename ...PyO>
    typename extent_as_pointer<T>::type
    MethodCallSemantics<is_const, false, kwlist, CClass, T, Args...>::
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

        T retval = make_call( self, method, *toCArgument<Args, false>(*pyargs)...);
        return retval;
    }



    template<bool is_const, const char* const kwlist[], typename CClass, typename T, typename ... Args>
    template<typename ...PyO>
    typename extent_as_pointer<T>::type
    MethodCallSemantics<is_const, true, kwlist, CClass, T, Args...>::
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
        T retval = call( method, self,  *toCArgument<Args, false >(*pyargs)..., extra_args);
        return retval;

    }


    /**
     * call that converts python given arguments to make C call:
     **/
    template<bool is_const, const char* const kwlist[], typename CClass, typename T, typename ... Args>
    template<int ...S>
    typename extent_as_pointer<T>::type
    MethodCallSemantics<is_const, false, kwlist, CClass, T, Args...>::
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


    template<bool is_const, const char* const kwlist[], typename CClass, typename ...Args>
    PyObject *MethodCallSemantics<is_const, false, kwlist, CClass, void, Args...>::
    toPyObj(CClass &self) {
        (void) self;
        return Py_None;
    }

     /**
     * call that converts python given arguments to make C call:
     **/
    template<bool is_const, const char* const kwlist[], typename CClass, typename T, typename ... Args>
    template<int ...S>
    typename extent_as_pointer<T>::type
    MethodCallSemantics<is_const, true, kwlist, CClass, T, Args...>::
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

    template<bool is_const, const char* const kwlist[], typename CClass, typename ...Args>
    PyObject *MethodCallSemantics<is_const, true, kwlist, CClass, void, Args...>::
    toPyObj(CClass &self) {
        (void) self;
        return Py_None;
    }

    template<bool is_const, const char* const kwlist[], typename CClass, typename ...Args>
    template<typename ...PyO>
    void MethodCallSemantics<is_const, false, kwlist, CClass, void, Args...>::
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
            (self.*method)(*toCArgument<Args, false>(*pyargs)...);
        }
    }

    template<bool is_const, const char* const kwlist[], typename CClass, typename ...Args>
    template<int ...S>
    void MethodCallSemantics<is_const, false, kwlist, CClass, void, Args...>::
    call_methodBase(method_t method,
                    typename std::remove_reference<CClass>::type &self,
                    PyObject *args, PyObject *kwds,
                    container<S...> unused) {
        (void) unused;
        PyObject pyobjs[sizeof...(Args) + 1];
        call_methodC(method, self, args, kwds, &pyobjs[S]...);
        (void) pyobjs;
    }


   template<bool is_const, const char* const kwlist[], typename CClass, typename ...Args>
    template<typename ...PyO>
    void MethodCallSemantics<is_const, true, kwlist, CClass, void, Args...>::
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
            (self.*method)(*toCArgument<Args, false>(*pyargs)...);
        }
    }

    template<bool is_const, const char* const kwlist[], typename CClass, typename ...Args>
    template<int ...S>
    void MethodCallSemantics<is_const, true, kwlist, CClass, void, Args...>::
    call_methodBase(method_t method,
                    typename std::remove_reference<CClass>::type &self,
                    PyObject *args, PyObject *kwds,
                    container<S...> unused) {
        (void) unused;
        PyObject pyobjs[sizeof...(Args) + 1];
        call_methodC(method, self, args, kwds, &pyobjs[S]...);
        (void) pyobjs;
    }


    template<class CClass, const char* const name>
    PyObject *MethodContainer<CClass, name, typename std::enable_if<
            !std::is_const<CClass>::value>::type>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        if (_this->get_CObject()) {
            for (const auto&[method, kwlist]: _methods){
                try {
                    return method->_call(self, args, kwds);
                } catch (...) {
                    PyErr_Clear();
                }
                PyErr_SetString(PyExc_TypeError, "No matching method signature for provided argu,emts");
            }

        }
        return nullptr;
    }

    template<typename CClass, const char* const name>
    std::vector< std::pair<
        typename MethodContainer<CClass, name, typename std::enable_if<!std::is_const<CClass>::value>::type>::BaseContainer*,
                                                                               const char* const*> >
            MethodContainer<CClass, name, typename std::enable_if<!std::is_const<CClass>::value>::type>::_methods;


    template<class CClass, const char* const name>
    PyObject *MethodContainer<CClass, name, typename std::enable_if<
           std::is_const<CClass>::value>::type>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;
        if (_this->get_CObject()) {
            for (const auto &[method, kwlist]: _methods) {
                try {
                    return method->_call(self, args, kwds);
                } catch (...) {
                    PyErr_Clear();
                }
            }
            PyErr_SetString(PyExc_TypeError, "No matching method signature for provided argu,emts");
        }
        return nullptr;
    }

    template<class CClass, const char* const name>
    typename MethodContainer<CClass, name, typename std::enable_if<std::is_const<CClass>::value>::type>::method_collection_t
            MethodContainer<CClass, name, typename std::enable_if<std::is_const<CClass>::value>::type>::_methods;



    template<class CClass, const char* const name>
    template<bool is_const, const char* const kwlist[], typename ReturnType, typename ...Args>
    PyObject *MethodContainer<CClass, name, typename std::enable_if<
            !std::is_const<CClass>::value>::type>::Container<is_const, kwlist, ReturnType, Args...>::
    _call(PyObject *self, PyObject *args, PyObject *kwds) {
        typedef PythonClassWrapper<CClass> Wrapper;
        if (!self || !PyObject_TypeCheck(self, Wrapper::getPyType())) return nullptr;
        Wrapper *_this = (Wrapper *) self;
        if (_this->get_CObject()) {
            try {
                return MethodCallSemantics<is_const, false, kwlist, CClass, ReturnType, Args...>::
                        call(_method, *_this->get_CObject(),  args, kwds);
            } catch (...) {
                return nullptr;
            }
        }
        return nullptr;
    }

    template<class CClass, const char* const name>
    template<bool is_const, const char* const kwlist[], typename ReturnType, typename ...Args>
    typename MethodContainer<CClass, name, typename std::enable_if<
            !std::is_const<CClass>::value>::type>::template Container<is_const, kwlist, ReturnType, Args...>::method_t
    MethodContainer<CClass, name, typename std::enable_if<
            !std::is_const<CClass>::value>::type>::Container<is_const, kwlist, ReturnType, Args...>::_method;

    template<class CClass, const char* const name>
    template<bool is_const, const char* const kwlist[], typename ReturnType, typename ...Args>
    PyObject *MethodContainer<CClass, name, typename std::enable_if<
            std::is_const<CClass>::value>::type>::Container<is_const, kwlist, ReturnType, Args...>::
    _call(PyObject *self, PyObject *args, PyObject *kwds) {
        typedef PythonClassWrapper<CClass> Wrapper;
        if (!self || !PyObject_TypeCheck(self, Wrapper::getPyType())) return nullptr;
        Wrapper *_this = (Wrapper *) self;
        if (_this->get_CObject()) {
            try {
                return MethodCallSemantics<true, false, kwlist, CClass, ReturnType, Args...>::
                call(_method, *_this->get_CObject(),  args, kwds);
            } catch (...) {
                return nullptr;
            }
        }
        return nullptr;
    }

    template<class CClass, const char* const name>
    template<bool is_const, const char* const kwlist[], typename ReturnType, typename ...Args>
    typename MethodContainer<CClass, name, typename std::enable_if<
            std::is_const<CClass>::value>::type>::template Container<is_const, kwlist, ReturnType, Args...>::method_t
    MethodContainer<CClass, name, typename std::enable_if<
            std::is_const<CClass>::value>::type>::Container<is_const, kwlist, ReturnType, Args...>::_method;

//////////////////////////////////////////////


    template<class CClass, const char* const name>
    PyObject *MethodContainerVarargs<CClass, name, typename std::enable_if<!std::is_const<CClass>::value>::type>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        if (_this->get_CObject()) {
            for (const auto &[method, kwlist]: _methods) {
                try {
                    return method->_call(self, args, kwds);
                } catch (...) {
                    PyErr_Clear();
                }
            }
            PyErr_SetString(PyExc_TypeError, "No matching method signature for provided argu,emts");
        }
        return nullptr;
    }

    template<typename CClass, const char* const name>
    typename MethodContainerVarargs<CClass, name, typename std::enable_if<!std::is_const<CClass>::value>::type>::method_collection_t
            MethodContainerVarargs<CClass, name, typename std::enable_if<!std::is_const<CClass>::value>::type>::_methods;


    template<class CClass, const char* const name>
    PyObject *MethodContainerVarargs<CClass, name, typename std::enable_if<std::is_const<CClass>::value>::type>::
    call(PyObject *self, PyObject *args, PyObject *kwds) {
        if (!self) return nullptr;
        PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;
        if (_this->get_CObject()) {
            for (const auto &[method, kwlist]: _methods) {
                try {
                    return method->_call(self, args, kwds);
                } catch (...) {
                    PyErr_Clear();
                }
                PyErr_SetString(PyExc_TypeError, "No matching method signature for provided argu,emts");
            }
        }
        return nullptr;
    }

    template<class CClass, const char* const name>
    std::vector<std::pair<
        typename MethodContainerVarargs<CClass, name, typename std::enable_if<std::is_const<CClass>::value>::type>::BaseContainer*,
                                                                     const char* const*> >
            MethodContainerVarargs<CClass, name, typename std::enable_if<std::is_const<CClass>::value>::type>::_methods;


    template<class CClass, const char* const name>
    template<bool is_const, const char* const kwlist[], typename ReturnType, typename ...Args>
    PyObject *MethodContainerVarargs<CClass, name, typename std::enable_if<!std::is_const<CClass>::value>::type>::
        Container<is_const, kwlist, ReturnType, Args...>::
    _call(PyObject *self, PyObject *args, PyObject *kwds) {
        typedef PythonClassWrapper<CClass> Wrapper;
        if (!self || !PyObject_TypeCheck(self, Wrapper::getPyType())) return nullptr;
        Wrapper *_this = (Wrapper *) self;
        if (_this->get_CObject()) {
            try {
                return MethodCallSemantics<is_const, true, kwlist, CClass, ReturnType, Args...>::
                call(_method, *_this->get_CObject(),  args, kwds);
            } catch (...) {
                return nullptr;
            }
        }
        return nullptr;
    }

    template<class CClass, const char* const name>
    template<bool is_const, const char* const kwlist[], typename ReturnType, typename ...Args>
    typename MethodContainerVarargs<CClass, name, typename std::enable_if<!std::is_const<CClass>::value>::type>::
            template Container<is_const, kwlist, ReturnType, Args...>::method_t
                    MethodContainerVarargs<CClass, name, typename std::enable_if<!std::is_const<CClass>::value>::type>::
                    Container<is_const, kwlist, ReturnType, Args...>::_method;

    template<class CClass, const char* const name>
    template<bool is_const, const char* const kwlist[], typename ReturnType, typename ...Args>
    PyObject *MethodContainerVarargs<CClass, name, typename std::enable_if<
            std::is_const<CClass>::value>::type>::Container<is_const, kwlist, ReturnType, Args...>::
    _call(PyObject *self, PyObject *args, PyObject *kwds) {
        typedef PythonClassWrapper<CClass> Wrapper;
        if (!self || !PyObject_TypeCheck(self, Wrapper::getPyType())) return nullptr;
        Wrapper *_this = (Wrapper *) self;
        if (_this->get_CObject()) {
            try {
                return MethodCallSemantics<true, true, kwlist, CClass, ReturnType, Args...>::
                call(_method, *_this->get_CObject(),  args, kwds);
            } catch (...) {
                return nullptr;
            }
        }
        return nullptr;
    }

    template<class CClass, const char* const name>
    template<bool is_const, const char* const kwlist[], typename ReturnType, typename ...Args>
    typename MethodContainerVarargs<CClass, name, typename std::enable_if<
            std::is_const<CClass>::value>::type>::template Container<is_const, kwlist, ReturnType, Args...>::method_t
                    MethodContainerVarargs<CClass, name, typename std::enable_if<
                    std::is_const<CClass>::value>::type>::Container<is_const, kwlist, ReturnType, Args...>::_method;
}

#endif