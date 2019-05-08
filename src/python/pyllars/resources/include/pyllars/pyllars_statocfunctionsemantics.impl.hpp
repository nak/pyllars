//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__CLASSMETHODSEMANTICS_CPP__
#define __PYLLARS_INTERNAL__CLASSMETHODSEMANTICS_CPP__

#include "pyllars_statocfunctionsemantics.impl.hpp"
#include "pyllars_function_wrapper.hpp"
#include "pyllars_utils.impl.hpp"
#include "pyllars_conversions.impl.hpp"
#include <cstddef>

namespace __pyllars_internal{

    namespace {
        struct Arbitrary{
            float fdata;
            int idata;
        };



        template<typename func_type, typename... PyO>
        typename func_traits<func_type>::ReturnType
        __call_static_func(func_type func, PyObject *extra_args, PyO *...pyobjs);

        template<typename ReturnType, typename ...Args, typename... PyO>
        ReturnType
        __call_static_func(ReturnType (*func)(Args..., ...), PyObject *extra_args, PyO *...pyargs) {
            typedef ReturnType (func_type)(Args..., ...);

            if (!extra_args || PyTuple_Size(extra_args) == 0) {
                if constexpr (std::is_void<ReturnType>::value) {
                    func(toCArgument<Args>(*pyargs).value()...);
                } else {
                    return func(toCArgument<Args>(*pyargs).value()...);
                }
            } else {
                const ssize_t extra_args_size = PyTuple_Size(extra_args);

                ffi_cif cif;
                ffi_type *arg_types[func_traits<func_type>::argsize + extra_args_size] = {FFIType<Args>::type()...};
                bool is_reference[] = {std::is_reference<Args>::value...};
                void *arg_values_bare[func_traits<func_type>::argsize + extra_args_size] = {(void*)toCArgument<Args>(*pyargs).ptr()...};
                void *arg_values[func_traits<func_type>::argsize + extra_args_size] = {(void*)toCArgument<Args>(*pyargs).ptr()...};
                for (int i = 0; i < sizeof...(Args); ++i){
                    if (is_reference[i]){
                        arg_values[i] = &arg_values_bare[i];
                        arg_types[i] = &ffi_type_pointer;
                    }
                }
                ffi_status status;

                // Because the return value from foo() is smaller than sizeof(long), it
                // must be passed as ffi_arg or ffi_sarg.
                ffi_arg result_small;
                void *result = &result_small;

                // Specify the data type of each argument. Available types are defined
                // in <ffi/ffi.h>.
                union ArgType {
                    char charvalue;
                    short shortvalue;
                    int intvalue;
                    long longvalue;
                    long long longlongvalue;
                    unsigned char ucharvalue;
                    unsigned short ushortvalue;
                    unsigned int uintvalue;
                    unsigned long ulongvalue;
                    unsigned long long ulonglongvalue;
                    float floatvalue;
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
                        case FFI_TYPE_SINT8:
                            extra_arg_values[i].charvalue = static_cast<char>(PyInt_AsLong(nextArg));
                            arg_values[i] = &extra_arg_values[i].charvalue;
                            break;
                        case FFI_TYPE_SINT16:
                            extra_arg_values[i].shortvalue = static_cast<short>(PyInt_AsLong(nextArg));
                            arg_values[i] = &extra_arg_values[i].shortvalue;
                            break;
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
                        case FFI_TYPE_UINT16:
                            extra_arg_values[i].ushortvalue = static_cast<unsigned short>(PyInt_AsLong(nextArg));
                            arg_values[i] = &extra_arg_values[i].ushortvalue;
                            break;
                        case FFI_TYPE_UINT32:
                            extra_arg_values[i].uintvalue = static_cast<unsigned int>(PyInt_AsLong(nextArg));
                            arg_values[i] = &extra_arg_values[i].uintvalue;
                            break;
                        case FFI_TYPE_UINT64:
                            extra_arg_values[i].ulongvalue = PyLong_AsUnsignedLongLong(nextArg);
                            arg_values[i] = &extra_arg_values[i].ulonglongvalue;
                            break;
                        case FFI_TYPE_FLOAT:
                            extra_arg_values[i].floatvalue = PyFloat_AsDouble(nextArg);
                            arg_values[i] = &extra_arg_values[i].floatvalue;
                            break;
                        case FFI_TYPE_DOUBLE:
                            extra_arg_values[i].doublevalue = PyFloat_AsDouble(nextArg);
                            arg_values[i] = &extra_arg_values[i].doublevalue;
                            break;
                        case FFI_TYPE_POINTER:
                            if (STRING_TYPE == subtype) {
                                extra_arg_values[i].ptrvalue = (void*) PyString_AsString(nextArg);
                                arg_values[i] = &extra_arg_values[i].ptrvalue;
                            } else if (COBJ_TYPE == subtype) {
                                static const size_t offset = offset_of<ObjectContainer<Arbitrary> *, PythonClassWrapper<Arbitrary> >
                                        (&PythonClassWrapper<Arbitrary>::_CObject);
                                auto **ptrvalue = (ObjectContainer<void *> **) (((char *) nextArg) + offset);
                                extra_arg_values[i].ptrvalue = ptrvalue ? (*ptrvalue)->ptr() : nullptr;
                            } else if (FUNC_TYPE == subtype) {
                                static constexpr bool with_ellipsis = true;


                                typedef PythonFunctionWrapper<func_type> wtype;
                                static const size_t offset = offsetof(wtype, _function);
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
                    return_type = &ffi_type_void;
                } else {
                    return_type = FFIType<ReturnType>::type();//&ffi_type_sint;
                }
                // Prepare the ffi_cif structure.
                if ((status = ffi_prep_cif_var(&cif,
                                               FFI_DEFAULT_ABI,
                                               func_traits<func_type>::argsize,
                                               func_traits<func_type>::argsize + extra_args_size,
                                               return_type, arg_types)) != FFI_OK) {
                    throw "FFI error calling variadic function";
                }
                // Invoke the function.
                ffi_call(&cif, FFI_FN(func), result, arg_values);
                if constexpr(!std::is_void<ReturnType>::value){
                    return *reinterpret_cast<ReturnType*>(result);
                }
            }
        }

        template<typename ReturnType, typename ...Args, typename... PyO>
        ReturnType
        __call_static_func(ReturnType (*func)(Args...), PyObject *extra_args, PyO *...pyargs) {
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
    template< typename func_type>
    PyObject *StaticCallSemantics<func_type>::
    call(func_type function, const char* const kwlist[], PyObject *args, PyObject *kwds) {
        typedef typename argGenerator<func_traits<func_type>::argsize>::type arg_generator_t;
        try {
            if constexpr (std::is_void<ReturnType>::value){
                invoke(function, kwlist, args, kwds, arg_generator_t());
                return Py_None;
            } else {
                ReturnType &&result = invoke(function, kwlist, args, kwds, arg_generator_t());
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
    template<typename func_type>
    template<int ...S>
    typename func_traits<func_type>::ReturnType
    StaticCallSemantics<func_type>::
    template invoke(func_type method, const char* const kwlist[], PyObject *args, PyObject *kwds, container<S...> s) {
        static char format[func_traits<func_type>::argsize + 1] = {0};
        if (func_traits<func_type>::argsize > 0)
            memset(format, 'O', (size_t) func_traits<func_type>::argsize);
        const ssize_t arg_count = kwds ? PyDict_Size(kwds) : 0 + args ? PyTuple_Size(args) : 0;
        const ssize_t kwd_count = kwds ? PyDict_Size(kwds) : 0;
        PyObject *extra_args = nullptr;
        PyObject *tuple = nullptr;

        if ((arg_count > func_traits<func_type>::argsize) && func_traits<func_type>::has_ellipsis) {
            tuple = PyTuple_GetSlice(args, 0, func_traits<func_type>::argsize - kwd_count);
            auto low = func_traits<func_type>::argsize - kwd_count;
            extra_args = PyTuple_GetSlice(args, low, arg_count);
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

    template<const char* const kwlist[], typename func_type, func_type *function>
    PyObject *
    StaticFunctionContainer<kwlist, func_type, function>::call(PyObject *cls, PyObject *args, PyObject *kwds) {
        (void) cls;
        if (!kwlist){
            PyErr_SetString(PyExc_SystemError, "Internal error -- unset (null) function invoked");
            return nullptr;
        }
        try {
            return StaticCallSemantics<func_type>::call(function, kwlist, args, kwds);
        } catch (const char* const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }


}

#endif
