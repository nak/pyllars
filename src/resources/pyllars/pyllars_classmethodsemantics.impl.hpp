//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__CLASSMETHODSEMANTICS_CPP__
#define __PYLLARS_INTERNAL__CLASSMETHODSEMANTICS_CPP__

#include "pyllars_classmethodsemantics.hpp"
#include "pyllars_function_wrapper.hpp"

namespace __pyllars_internal{

    namespace {

        template<typename func_type, typename... PyO>
        typename func_traits<func_type>::ReturnType
        __call_static_func(func_type func, PyObject *extra_args, PyO *...pyobjs);

        template<typename ReturnType, typename ...Args, typename... PyO>
        ReturnType
        __call_static_func(ReturnType (*func)(Args..., ...), PyObject *extra_args, PyO *...pyargs) {
            typedef ReturnType (*func_type)(Args..., ...);

            if (!extra_args || PyTuple_Size(extra_args) == 0) {
                if constexpr (std::is_void<ReturnType>::value) {
                    func(toCArgument(*pyargs)...);
                } else {
                    return func(toCArgument(*pyargs)...);
                }
            } else {
                const ssize_t extra_args_size = PyTuple_Size(extra_args);

                ffi_cif cif;
                ffi_type *arg_types[func_traits<func_type>::argsize + extra_args_size] = {FFIType<Args>::type()...};
                void *arg_values[
                        func_traits<func_type>::argsize + extra_args_size] = {(void *) &toCArgument(pyargs).value()...};
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
                ArgType extra_arg_values[func_traits<func_type>::argsize + extra_args_size];
                for (size_t i = func_traits<func_type>::argsize;
                     i < func_traits<func_type>::argsize + extra_args_size; ++i) {
                    PyObject *const nextArg = PyTuple_GetItem(extra_args, i - func_traits<func_type>::argsize);
                    const int subtype = getType(nextArg, arg_types[i]);
                    switch (arg_types[i]->type) {
                        case FFI_TYPE_SINT32:
                            extra_arg_values[i].intvalue = static_cast<int>(PyInt_AsLong(nextArg));
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
                                static const size_t offset = offset_of<ObjectContainer<Arbitrary> *, PythonClassWrapper<Arbitrary> >
                                        (&PythonClassWrapper<Arbitrary>::_CObject);
                                auto **ptrvalue = (ObjectContainer<void *> **) (((char *) nextArg) + offset);
                                extra_arg_values[i].ptrvalue = ptrvalue ? (*ptrvalue)->ptr() : nullptr;
                            } else if (FUNC_TYPE == subtype) {
                                static constexpr bool with_ellipsis = true;
                                typedef typename PythonFunctionWrapper<true, with_ellipsis, int, int>::template Wrapper<> wtype;
                                typedef typename PythonFunctionWrapper<true, with_ellipsis, int, int>::template Wrapper<>::FuncContainer ftype;
                                static const size_t offset = offset_of<ftype, wtype>(&wtype::_cfunc);
                                void **ptrvalue = (void **) (((char *) nextArg) + offset);
                                extra_arg_values[i].ptrvalue = *ptrvalue;
                            } else {
                                throw "Unable to convert Python object to C Object";
                            }
                            arg_values[i] = &extra_arg_values[i].ptrvalue;
                            break;
                        default:
                            throw "Python object cannot be converted to C object";
                    }
                }
                ffi_type *return_type = nullptr;
                if constexpr(std::is_void<ReturnType>::value) {
                    return_type = FFI_TYPE_VOID;
                } else {
                    return_type = FFIType<ReturnType>::type();//&ffi_type_sint;
                }
                // Prepare the ffi_cif structure.
                if ((status = ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI,
                                               func_traits<func_type>::argsize,
                                               func_traits<func_type>::argsize + extra_args_size,
                                               return_type, arg_types)) != FFI_OK) {
                    throw "FFI error calling variadic function";
                }
                // Invoke the function.
                ffi_call(&cif, FFI_FN(func), result, arg_values);
            }
        }

        template<typename ReturnType, typename ...Args, typename... PyO>
        ReturnType
        __call_static_func(ReturnType (*func)(Args...), PyObject *extra_args, PyO *...pyargs) {
            typedef ReturnType (*func_type)(Args...);

            if (!extra_args || PyTuple_Size(extra_args) == 0) {
                if constexpr (std::is_void<ReturnType>::value) {
                    func(toCArgument<Args>(*pyargs).value()...);
                } else {
                    return func(toCArgument<Args>(*pyargs).value()...);
                }
            } else {
                throw "Too many arguments provided to function call";
            }
        }
    }

    /**
    * Used for regular methods:
    */
    template<typename CClass, const char* const kwlist[], typename func_type>
    PyObject *StaticCallSemantics<CClass, kwlist, func_type>::
    call(func_type method, PyObject *args, PyObject *kwds) {
        typedef typename argGenerator<func_traits<func_type>::argsize>::type arg_generator_t;
        try {
            if constexpr (std::is_void<ReturnType>::value){
                inoke(method, args, kwds, arg_generator_t());
                return Py_None;
            } else {
                ReturnType &&result = inoke(method, args, kwds, arg_generator_t());
                const ssize_t array_size = ArraySize<ReturnType>::size;//sizeof(result) / sizeof(T_base);
                return toPyObject<ReturnType>(result, array_size);
            }
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }

    /**
     * call that converts python given arguments to make C call:
     **/
    template<typename CClass, const char* const kwlist[], typename func_type>
    template<int ...S>
    typename func_traits<func_type>::ReturnType
    StaticCallSemantics<CClass, kwlist, func_type>::
    inoke(func_type method, PyObject *args, PyObject *kwds, container<S...> s) {
        static char format[func_traits<func_type>::argsize + 1] = {0};
        if (func_traits<func_type>::argsize > 0)
            memset(format, 'O', (size_t) func_traits<func_type>::argsize);
        const ssize_t arg_count = kwds ? PyDict_Size(kwds) : 0 + args ? PyTuple_Size(args) : 0;
        const ssize_t kwd_count = kwds ? PyDict_Size(kwds) : 0;
        PyObject *extra_args = nullptr;
        PyObject *tuple = nullptr;

        if ((arg_count > func_traits<func_type>::argsize) && func_traits<func_type>::has_ellipsis) {
            tuple = PyTuple_GetSlice(args, 0, func_traits<func_type>::argsize - kwd_count);
            extra_args = PyTuple_GetSlice(args, func_traits<func_type>::argsize - kwd_count, arg_count);
        } else {
            tuple = args;
        }
        (void) s;
        PyObject *pyobjs[func_traits<func_type>::argsize];
        memset(pyobjs, 0, func_traits<func_type>::argsize*sizeof(PyObject*));
        (void) pyobjs;
        if (func_traits<func_type>::argsize > 0 && !PyArg_ParseTupleAndKeywords(tuple, kwds, format, (char **) kwlist, &pyobjs[S]...)) {
            throw "Invalid arguments to method call";
        }

        if constexpr (std::is_void<ReturnType>::value) {
            __call_static_func(method, extra_args, pyobjs[S]...);
        } else {
            return __call_static_func(method, extra_args, pyobjs[S]...);

        }
    }

    template<class CClass, const char *const name, const char* const kwlist[],  typename func_type>
    PyObject *ClassMethodContainer<CClass, name, kwlist, func_type>::
    call(PyObject *cls, PyObject *args, PyObject *kwds) {
        (void) cls;
        if (!function){
            PyErr_SetString(PyExc_SystemError, "Internal error -- unset (null) function invoked");
            return nullptr;
        }
        try {
            return StaticCallSemantics<CClass, kwlist, func_type>::call(function, args, kwds);
        } catch (const char* const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }


    template<class CClass, const char *const name, const char* const kwlist[], typename func_type>
    func_type*
    ClassMethodContainer<CClass, name, kwlist,func_type>::function;



}

#endif
