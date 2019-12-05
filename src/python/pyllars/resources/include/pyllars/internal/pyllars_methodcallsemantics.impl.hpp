//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__METHODCALLSEMANTICS_CPP_
#define __PYLLARS_INTERNAL__METHODCALLSEMANTICS_CPP_
#include "pyllars_varargs.hpp"
#include "pyllars_methodcallsemantics.hpp"
#include "pyllars/internal/pyllars_conversions.impl.hpp"


namespace pyllars_internal {

    template<typename CClass, typename ReturnType, typename... Args>
    ReturnType
    func_traits<ReturnType (CClass::*)(Args..., ...)>::invoke(type method, CClass &self,
                                                              PyObject *extra_args,
                                                              typename PyObjectPack<Args>::type... pyargs) {
        if (!extra_args){
            if constexpr (std::is_void<ReturnType>::value) {
                return (self.*method)(toCArgument<Args>(*pyargs).value()...);
            } else {
                (self.*method)(toCArgument<Args>(*pyargs).value()...);
            }
        }
        typedef typename std::remove_reference<ReturnType>::type ReturnType_NoRef;

        const ssize_t extra_args_size = PyTuple_Size(extra_args);

        ffi_cif cif;
        ffi_type *arg_types[sizeof...(Args) + extra_args_size] = {FFIType<Args>::type()...};
        void *arg_values[sizeof...(Args) + extra_args_size + 1] = {(void *) &self, (void *) toCArgument<Args>(*pyargs).ptr()...};
        ffi_status status;

        // Because the return _CObject from foo() is smaller than sizeof(long), it
        // must be passed as ffi_arg or ffi_sarg.
        ffi_arg result_small;
        unsigned char result_big[sizeof(ReturnType_NoRef)];
        void *result = (sizeof(ReturnType_NoRef) >= sizeof(ffi_arg)) ? (void *) &result_big[0]
                                                                     : (void *) &result_small;

        for (size_t i = sizeof...(Args) + 1; i < sizeof...(Args) + extra_args_size + 1; ++i) {
            PyObject *const nextArg = PyTuple_GetItem(extra_args, i - sizeof...(Args));
            arg_values[i] = toFFI(nextArg);
            const int subtype = getType(nextArg, arg_types[i]);
        }

        ffi_type *return_type = FFIType<ReturnType>::type();
        // Prepare the ffi_cif structure.
        if ((status = ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI,
                                       sizeof...(Args), sizeof...(Args) + extra_args_size,
                                       return_type, arg_types)) != FFI_OK) {
            throw PyllarsException(PyExc_TypeError, "FFI error calling variadic function");
        }

        // Invoke the function.
        ffi_call(&cif, FFI_FN(method), result, arg_values);

        return *((ReturnType_NoRef *) result);

    }


    template<typename CClass, typename ReturnType, typename... Args>
    ReturnType
    func_traits<ReturnType (CClass::*)(Args..., ...) const>::invoke(type method, const CClass &self,
                                                                    PyObject *extra_args,
                                                                    typename PyObjectPack<Args>::type... pyargs) {
        if (!extra_args){
            if constexpr (std::is_void<ReturnType>::value) {
                return (self.*method)(toCArgument<Args>(*pyargs).value()...);
            } else {
                (self.*method)(toCArgument<Args>(*pyargs).value()...);
            }
        }

        typedef typename std::remove_reference<ReturnType>::type ReturnType_NoRef;

        const ssize_t extra_args_size = PyTuple_Size(extra_args);

        ffi_cif cif;
        ffi_type *arg_types[sizeof...(Args) + extra_args_size] = {FFIType<Args>::type()...};
        void *arg_values[sizeof...(Args) + extra_args_size + 1] = {(void *) &self, (void *) toCArgument<Args>(*pyargs).ptr()...};
        ffi_status status;

        // Because the return _CObject from foo() is smaller than sizeof(long), it
        // must be passed as ffi_arg or ffi_sarg.
        ffi_arg result_small;
        unsigned char result_big[sizeof(ReturnType_NoRef)];
        void *result = (sizeof(ReturnType_NoRef) >= sizeof(ffi_arg)) ? (void *) &result_big[0]
                                                                     : (void *) &result_small;

        for (size_t i = sizeof...(Args) + 1; i < sizeof...(Args) + extra_args_size + 1; ++i) {
            PyObject *const nextArg = PyTuple_GetItem(extra_args, i - sizeof...(Args));
            //const int subtype = getType(nextArg, arg_types[i]);
            arg_values[i] = toFFI(nextArg);
        }

        ffi_type *return_type = FFIType<ReturnType>::type();
        // Prepare the ffi_cif structure.
        if ((status = ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI,
                                       sizeof...(Args), sizeof...(Args) + extra_args_size,
                                       return_type, arg_types)) != FFI_OK) {
            throw PyllarsException(PyExc_TypeError, "FFI error calling variadic function");
        }

        // Invoke the function.
        ffi_call(&cif, FFI_FN(method), result, arg_values);

        return *((ReturnType_NoRef *) result);

    }







    template<typename CClass, typename ReturnType, typename... Args>
    ReturnType
    func_traits<ReturnType (CClass::*)(Args..., ...) volatile>::invoke(type method, volatile CClass &self,
                                                                       PyObject *extra_args,
                                                                       typename PyObjectPack<Args>::type... pyargs) {
        if (!extra_args){
            if constexpr (std::is_void<ReturnType>::value) {
                return (self.*method)(toCArgument<Args>(*pyargs).value()...);
            } else {
                (self.*method)(toCArgument<Args>(*pyargs).value()...);
            }
        }
        typedef typename std::remove_reference<ReturnType>::type ReturnType_NoRef;

        const ssize_t extra_args_size = PyTuple_Size(extra_args);

        ffi_cif cif;
        ffi_type *arg_types[sizeof...(Args) + extra_args_size] = {FFIType<Args>::type()...};
        void *arg_values[sizeof...(Args) + extra_args_size + 1] = {(void *) &self, (void *) toCArgument<Args>(*pyargs).ptr()...};
        ffi_status status;

        // Because the return _CObject from foo() is smaller than sizeof(long), it
        // must be passed as ffi_arg or ffi_sarg.
        ffi_arg result_small;
        unsigned char result_big[sizeof(ReturnType_NoRef)];
        void *result = (sizeof(ReturnType_NoRef) >= sizeof(ffi_arg)) ? (void *) &result_big[0]
                                                                     : (void *) &result_small;

        for (size_t i = sizeof...(Args) + 1; i < sizeof...(Args) + extra_args_size + 1; ++i) {
            PyObject *const nextArg = PyTuple_GetItem(extra_args, i - sizeof...(Args));
            arg_values[i] = toFFI(nextArg);
            const int subtype = getType(nextArg, arg_types[i]);
        }

        ffi_type *return_type = FFIType<ReturnType>::type();
        // Prepare the ffi_cif structure.
        if ((status = ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI,
                                       sizeof...(Args), sizeof...(Args) + extra_args_size,
                                       return_type, arg_types)) != FFI_OK) {
            throw PyllarsException(PyExc_TypeError, "FFI error calling variadic function");
        }

        // Invoke the function.
        ffi_call(&cif, FFI_FN(method), result, arg_values);

        return *((ReturnType_NoRef *) result);

    }


    template<typename CClass, typename ReturnType, typename... Args>
    ReturnType
    func_traits<ReturnType (CClass::*)(Args..., ...) const volatile>::invoke(type method, const volatile CClass &self,
                                                                    PyObject *extra_args,
                                                                    typename PyObjectPack<Args>::type... pyargs) {
        if (!extra_args){
            if constexpr (std::is_void<ReturnType>::value) {
                return (self.*method)(toCArgument<Args>(*pyargs).value()...);
            } else {
                (self.*method)(toCArgument<Args>(*pyargs).value()...);
            }
        }

        typedef typename std::remove_reference<ReturnType>::type ReturnType_NoRef;

        const ssize_t extra_args_size = PyTuple_Size(extra_args);

        ffi_cif cif;
        ffi_type *arg_types[sizeof...(Args) + extra_args_size] = {FFIType<Args>::type()...};
        void *arg_values[sizeof...(Args) + extra_args_size + 1] = {(void *) &self, (void *) toCArgument<Args>(*pyargs).ptr()...};
        ffi_status status;

        // Because the return _CObject from foo() is smaller than sizeof(long), it
        // must be passed as ffi_arg or ffi_sarg.
        ffi_arg result_small;
        unsigned char result_big[sizeof(ReturnType_NoRef)];
        void *result = (sizeof(ReturnType_NoRef) >= sizeof(ffi_arg)) ? (void *) &result_big[0]
                                                                     : (void *) &result_small;

        for (size_t i = sizeof...(Args) + 1; i < sizeof...(Args) + extra_args_size + 1; ++i) {
            PyObject *const nextArg = PyTuple_GetItem(extra_args, i - sizeof...(Args));
            //const int subtype = getType(nextArg, arg_types[i]);
            arg_values[i] = toFFI(nextArg);
        }

        ffi_type *return_type = FFIType<ReturnType>::type();
        // Prepare the ffi_cif structure.
        if ((status = ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI,
                                       sizeof...(Args), sizeof...(Args) + extra_args_size,
                                       return_type, arg_types)) != FFI_OK) {
            throw PyllarsException(PyExc_TypeError, "FFI error calling variadic function");
        }

        // Invoke the function.
        ffi_call(&cif, FFI_FN(method), result, arg_values);

        return *((ReturnType_NoRef *) result);

    }









    template<const char *const kwlist[], typename method_t, method_t method>
    PyObject* MethodContainer<kwlist, method_t, method>::
    call(PyObject* self,  PyObject *args, PyObject *kwds) {
        try {
            PyTypeObject *baseTyp = PythonClassWrapper<CClass>::getPyType();
            PyTypeObject *derivedTyp = self->ob_type;
            auto key = std::pair{baseTyp, derivedTyp};
            if (CommonBaseWrapper::castMap().count(key) == 0) {
                return call_base(self, args, kwds);
            } else {
                return call(CommonBaseWrapper::castMap()[key](self), args, kwds);
            }
        } catch (PyllarsException & e){
            PyErr_SetString(e.type(), e.msg());
            return nullptr;
        } catch (...){
            PyErr_SetString(PyExc_SystemError, "Unknown exception in Pyllars");
            return nullptr;
        }
    }

    template<const char *const kwlist[], typename method_t, method_t method>
    PyObject* MethodContainer<kwlist, method_t, method>::
    call_base(PyObject* self,  PyObject *args, PyObject *kwds) {
        constexpr ssize_t argsize = func_traits<method_t>::argsize;
        const ssize_t pyargsize = PyTuple_Size(args) + (kwds ? PyDict_Size(kwds) : 0);
        if constexpr(func_traits<method_t>::has_ellipsis) {
            if (pyargsize < argsize) {
                throw PyllarsException(PyExc_TypeError, "Invalid arguments to method call");
            }
        } else {
            if (pyargsize != argsize) {
                throw PyllarsException(PyExc_TypeError, "Invalid arguments to method call");
            }
        }
        typedef typename argGenerator<argsize>::type arg_generator_t;
        auto self_ = reinterpret_cast<PythonClassWrapper<CClass>*>(self);
        CClass &this_ = *self_->get_CObject();
        if constexpr (std::is_void<ReturnType>::value) {
            call_methodBase(this_, method, args, kwds, arg_generator_t());
            return Py_None;
        } else {
            //TODO: volatile and const volatile in is_same comparison?  Just create a strip_const type trait?
            if constexpr((std::is_reference<ReturnType>::value &&  (std::is_same<CClass&, ReturnType>::value || std::is_same<const CClass&, ReturnType>::value))){
                auto &result = call_methodBase(this_, args, kwds, arg_generator_t());
                if (&result == &this_){
                    //return a pointer/reference to "this" and so just return same Pyhton self object
                    //(otherwise reference semantics might get hosed and worse)
                    Py_INCREF(self);
                    return self;
                }
                return toPyObject(result, 1);
            } else if constexpr((std::is_pointer<ReturnType>::value && (std::is_same<CClass*, ReturnType>::value || std::is_same<const CClass*, ReturnType>::value))){
                auto *result = call_methodBase(this_, args, kwds, arg_generator_t());
                if (result == this_){
                    //return a pointer/reference to "this" and so just return same Pyhton self object
                    //(otherwise reference semantics might get hosed and worse)
                    Py_INCREF(self);
                    return self;
                }
                return toPyObject(result, 1);
            }
            return toPyObject<ReturnType>(call_methodBase(this_, args, kwds, arg_generator_t()), 1);
        }
    }

    template<const char *const kwlist[], typename method_t, method_t method>
    PyObject *MethodContainer<kwlist, method_t, method>::callAsUnaryFunc(PyObject *self){
        static auto emptyargs = PyTuple_New(0);
        return call(self, emptyargs, nullptr);
    }

    template<const char *const kwlist[], typename method_t, method_t method>
    PyObject *MethodContainer<kwlist, method_t, method>::callAsBinaryFunc(PyObject *self, PyObject *arg) {
        auto args = PyTuple_New(1);
        PyTuple_SetItem(args, 0, arg);
        auto retval = call(self, /*Py_BuildValue("(O)",*/ args, nullptr);
        Py_INCREF(arg); // because tuple setitem steals one
        Py_DECREF(args);
        return retval;
    }

    template<const char *const kwlist[], typename method_t, method_t method>
    template<typename ...PyO>
    typename func_traits<method_t>::ReturnType
    MethodContainer<kwlist, method_t, method>::
    call_methodC(
            CClass & self,
            PyObject *args, PyObject *kwds, PyO *...pyargs) {
        static char format[func_traits<method_t>::argsize + 1] = {0};
        const ssize_t arg_count = kwds ? PyDict_Size(kwds) : 0 + args ? PyTuple_Size(args) : 0;
        const ssize_t kwd_count = kwds ? PyDict_Size(kwds) : 0;
        PyObject *extra_args = nullptr;
        PyObject *tuple = nullptr;

        if (PyTuple_Size(args) > func_traits<method_t>::argsize ){
            tuple = PyTuple_GetSlice(args, 0, func_traits<method_t>::argsize - kwd_count);
            extra_args = PyTuple_GetSlice(args,func_traits<method_t>::argsize - kwd_count, arg_count);
        } else {
            tuple = args;
        }
        if (func_traits<method_t>::argsize > 0)
            memset(format, 'O', func_traits<method_t>::argsize);
        if (kwds && !PyArg_ParseTupleAndKeywords(tuple, kwds, format, (char **) kwlist, &pyargs...)) {
            throw PyllarsException(PyExc_TypeError, "Invalid arguments to method call");
        } else if (!kwds && !PyArg_ParseTuple(tuple, format, &pyargs...)) {
            throw PyllarsException(PyExc_TypeError, "Invalid arguments to method call");
        }
        return func_traits<method_t>::invoke(method, self, extra_args, pyargs...);

    }


    /**
     * call that converts python given arguments to make C call:
     **/
    template<const char *const kwlist[], typename method_t, method_t method>
    template<int ...S>
    typename func_traits<method_t>::ReturnType
    MethodContainer<kwlist, method_t, method>::
    call_methodBase(
            CClass & self,
            PyObject *args, PyObject *kwds, container<S...> s) {
        (void) s;
        PyObject pyobjs[func_traits<method_t>::argsize + 1];
        (void) pyobjs;
        if (!method) {
            throw PyllarsException(PyExc_TypeError, "Null method pointer encountered");
        }
        return call_methodC(self, args, kwds, &pyobjs[S]...);

    }
}

#endif
