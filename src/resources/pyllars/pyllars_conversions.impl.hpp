//
// Created by jrusnak on 7/22/16.
//
#ifndef __PYLLARS__INTERNAL__CONVERSIONS_CPP__
#define __PYLLARS__INTERNAL__CONVERSIONS_CPP__

#include <Python.h>
#include "pyllars_utils.hpp"
#include "pyllars_conversions.hpp"

#include "pyllars_callbacks.hpp"
#include "pyllars_pointer.hpp"

namespace __pyllars_internal {


    template<typename T, bool array_allocated>
    smart_ptr<typename std::remove_reference<T>::type, array_allocated>
    CObjectConversionHelper<T, array_allocated,
            typename std::enable_if<!std::is_array<T>::value &&
                                    !std::is_function<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_enum<typename std::remove_reference<T>::type>::value &&
                                    !std::is_integral<typename std::remove_reference<T>::type>::value &&
                                    !std::is_floating_point<typename std::remove_reference<T>::type>::value>::type>::
    toCArgument(PyObject &pyobj) {
        if (CommonBaseWrapper::template checkImplicitArgumentConversion<T>(&pyobj)) {
            return smart_ptr<T, array_allocated>(
                    reinterpret_cast<PythonClassWrapper<T> * >(&pyobj)->get_CObject(),
                    PTR_IS_NOT_ALLOCATED);
        } else {
            throw "Invalid type or const conversion converting to C object";
        }
    }


    template<typename T, bool array_allocated>
    smart_ptr<T, array_allocated> CObjectConversionHelper<T, array_allocated, typename std::enable_if<
            std::is_integral<typename std::remove_reference<T>::type>::value
            || std::is_enum<typename std::remove_reference<T>::type>::value>::type>::
    toCArgument(PyObject &pyobj) {
        auto self = (CommonBaseWrapper*) &pyobj;

        typedef typename std::remove_reference<typename std::remove_const<T>::type>::type T_bare;
        if (PyInt_Check(&pyobj)) {
            T_bare *value = new T_bare((T_bare) PyInt_AsLong(&pyobj));
            return smart_ptr<T, array_allocated>(value, PTR_IS_ALLOCATED);
        } else if (PyLong_Check(&pyobj)) {
            T_bare *value = new T_bare((T_bare) PyLong_AsLongLong(&pyobj));
            return smart_ptr<T, array_allocated>(value, PTR_IS_ALLOCATED);
        }
        if (CommonBaseWrapper::template checkImplicitArgumentConversion<T>(&pyobj)) {
            return smart_ptr<T, array_allocated>(
                    reinterpret_cast<PythonClassWrapper<T> * >(&pyobj)->get_CObject(),
                    PTR_IS_NOT_ALLOCATED);
        } else {
            throw "Invalid type or const conversion converting to C object";
        }
    }

    template<typename T, bool array_allocated>
    smart_ptr<T, array_allocated> CObjectConversionHelper<T, array_allocated,
            typename std::enable_if<std::is_floating_point<typename std::remove_reference<T>::type>::value>::type>::
    toCArgument(PyObject &pyobj) {
        if (PyFloat_Check(&pyobj)){
            if (!std::is_const<T>::value && std::is_reference<T>::value){
                throw "Invalid const-ness in conversion of argument";
            }
            T *value = new T( PyFloat_AsDouble(&pyobj));
            return smart_ptr<T, array_allocated>(value, PTR_IS_ALLOCATED);
        }
        if (CommonBaseWrapper::template checkImplicitArgumentConversion<T>(&pyobj)) {
            return smart_ptr<T, array_allocated>(
                    reinterpret_cast<PythonClassWrapper<T> * >(&pyobj)->get_CObject(),
                    PTR_IS_NOT_ALLOCATED);
        } else {
            throw "Invalid type or const conversion converting to C object";
        }
    }           

    template<typename ReturnType, typename ...Args>
    typename CObjectConversionHelper<ReturnType(*)(Args...), false, void>::ptr_t
    CObjectConversionHelper<ReturnType(*)(Args...), false, void>::
    toCArgument(PyObject &pyobj) {
        if (!PyCallable_Check(&pyobj)) {
            throw "Python callback is not callable!";
        }
        callback_t *retval = new callback_t(PyCallbackWrapper<ReturnType, Args...>(&pyobj).get_C_callback());
        return smart_ptr<callback_t, false>(retval, PTR_IS_ALLOCATED);
    }

    template<typename ReturnType, typename ...Args>
    typename CObjectConversionHelper<ReturnType(*)(Args..., ...), false, void>::ptr_t
    CObjectConversionHelper<ReturnType(*)(Args..., ...), false, void>::
    toCArgument(PyObject &pyobj) {
        if (!PyCallable_Check(&pyobj)) {
            throw "Python callback is not callable!";
        }
        callback_t *retval = new callback_t(PyCallbackWrapperVar<ReturnType, Args...>(&pyobj).get_C_callback());
        return smart_ptr<callback_t, false>(retval, PTR_IS_ALLOCATED);
    }

    template<bool array_allocated>
    smart_ptr<const char *, array_allocated>
    CObjectConversionHelper<const char *, array_allocated>::
    toCArgument(PyObject &pyobj) {
        auto self = (CommonBaseWrapper*) &pyobj;

        if (PyString_Check(&pyobj)) {
            auto name = (const char *) PyString_AsString(&pyobj);
            if (!name) { throw "Error converting string: null pointer encountered"; }
            return smart_ptr<const char *, false>(new (const char*)(name), PTR_IS_ALLOCATED);
        } else if (PyBytes_Check(&pyobj)){
            auto name = (char *) PyBytes_AsString(&pyobj);
            if (!name) { throw "Error converting string: null pointer encountered"; }
            return smart_ptr<const char *, false>(new (const char*)(name), PTR_IS_ALLOCATED);
        }
        if (CommonBaseWrapper::template checkImplicitArgumentConversion<const char*>(&pyobj)) {
            return smart_ptr<const char*, array_allocated>(
                    reinterpret_cast<PythonClassWrapper<const char*> * >(&pyobj)->get_CObject(),
                    PTR_IS_NOT_ALLOCATED);
        } else {
            throw "Invalid type or const conversion converting to C object";
        }
    }

    template<bool array_allocated>
    typename CObjectConversionHelper<const char *const, array_allocated>::ptr_t
    CObjectConversionHelper<const char *const, array_allocated>::
    toCArgument(PyObject &pyobj) {
        auto self = (CommonBaseWrapper*) &pyobj;

        if (PyString_Check(&pyobj)) {
            auto name = (const char *) PyString_AsString(&pyobj);
            if (!name) { throw "Error converting string: null pointer encountered"; }
            return smart_ptr<const char * const, false>(new (const char* const)(name), PTR_IS_ALLOCATED);
        }
        if (CommonBaseWrapper::template checkImplicitArgumentConversion<const char* const>(&pyobj)) {
            return smart_ptr<const char* const, array_allocated>(
                    reinterpret_cast<PythonClassWrapper<const char* const> * >(&pyobj)->get_CObject(),
                    PTR_IS_NOT_ALLOCATED);
        } else {
            throw "Invalid type or const conversion converting to C object";
        }
    }

    template<bool array_allocated>
    typename CObjectConversionHelper<char *const, array_allocated >::ptr_t
    CObjectConversionHelper<char *const, array_allocated >::
    toCArgument(PyObject &pyobj) {
        auto self = (CommonBaseWrapper*) &pyobj;
        if (PyString_Check(&pyobj)) {
            auto name = (char * const ) PyString_AsString(&pyobj);
            if (!name) { throw "Error converting string: null pointer encountered"; }
            return smart_ptr<char * const, false>(new (char* const)(name), PTR_IS_ALLOCATED);
        }
        if (CommonBaseWrapper::template checkImplicitArgumentConversion< char* const>(&pyobj)) {
            return smart_ptr< char* const, array_allocated>(
                    reinterpret_cast<PythonClassWrapper<char* const> * >(&pyobj)->get_CObject(),
                    PTR_IS_NOT_ALLOCATED);
        } else {
            throw "Invalid type or const conversion converting to C object";
        }
    }

    template<bool array_allocated>
    typename CObjectConversionHelper<char *, array_allocated>::ptr_t
    CObjectConversionHelper<char *, array_allocated>::
    toCArgument(PyObject &pyobj) {
        if (PyString_Check(&pyobj)) {
            char * name = (char *) PyString_AsString(&pyobj);
            if (!name) { throw "Error converting string: null pointer encountered"; }
            return smart_ptr<char *, false>(new (char*)(name), PTR_IS_ALLOCATED);
#if PY_MAJOR_VERSION >= 3
        } else if (PyBytes_Check(&pyobj)) {
            auto bytes = (char* ) PyBytes_AsString(&pyobj);
            if(!bytes){
                throw "Error converting bytes: null pointer encountered";
            }
            return smart_ptr<char*, false>(&bytes, PTR_IS_NOT_ALLOCATED);
        }
#endif
        if (CommonBaseWrapper::template checkImplicitArgumentConversion<const char* const>(&pyobj)) {
            return smart_ptr<const char* const, array_allocated>(
                    reinterpret_cast<PythonClassWrapper<char*> * >(&pyobj)->get_CObject(),
                    PTR_IS_NOT_ALLOCATED);
        } else {
            throw "Invalid type or const conversion converting to C object";
        }
    }

    namespace {
        template<typename T, typename E=void>
        struct limits{
            static constexpr bool is_in_bounds(long v){
                return false;
            }
            static constexpr bool is_in_bounds(unsigned  long v){
                return false;
            }
        };

        template<typename T>
        struct limits<T,  typename std::enable_if<std::is_integral<T>::value ||
                                                  std::is_floating_point<T>::value>::type>{
            static constexpr bool is_in_bounds(long v){
                return v <= std::numeric_limits<T>::max() || v >= std::numeric_limits<T>::min();
            }
            static constexpr bool is_in_bounds(unsigned  long v){
                return v <= std::numeric_limits<T>::max() || v >= std::numeric_limits<T>::min();
            }
        };

        template<typename T, typename E=void>
        struct Setter{
            static PyObject* setItem(PyObject* obj, const size_t index, T & val, const bool is_bytes, const bool is_str, const bool is_int, const bool is_float){
                if(!PyList_Check(obj)){
                    throw "Invalid conversion in item of non-list object";
                }
                auto self =(PythonClassWrapper<T>*)  PyList_GetItem(obj, index);
                if(!PyObject_TypeCheck((PyObject*)self, PythonClassWrapper<T>::getPyType())){
                    throw "Incompatible types in C conversion";
                }
                if (!self->get_CObject()){
                    throw "Cannot set null item";
                }
                *self->get_CObject() = val;
                return (PyObject*)self;
            }
        };

        template<typename T>
        struct Setter<T, typename std::enable_if<std::is_integral<T>::value>::type >{
            static void setItem(PyObject* obj, const size_t index, T & val, const bool is_bytes, const bool is_str, const bool is_int, const bool is_float){
                if (is_int) {
                    PyList_SetItem(obj, index, PyLong_FromLong(val));
                } else {
                    if(!PyList_Check(obj)){
                        throw "Attempt to set item in non-list object";
                    }
                    auto self =(PythonClassWrapper<T>*)  PyList_GetItem(obj, index);

                    if(!PyObject_TypeCheck((PyObject*)self, PythonClassWrapper<T>::getPyType())){
                        throw "Invalid C type conversion";
                    }
                    if(!self->get_CObject()){
                        throw "Cannot set null item";
                    }
                    *self->get_CObject() = val;
                }
            }
        };

        template<typename T>
        struct Setter<T, typename std::enable_if<std::is_floating_point<T>::value>::type >{
            static void setItem(PyObject* obj, const size_t index,  T & val, const bool is_bytes, const bool is_str, const bool is_int, const bool is_float){
                if (is_float) {
                    PyList_SetItem(obj, index, PyFloat_FromDouble(val));
                } else {
                    if(!PyList_Check(obj)){
                        throw "Attempt to set item in non-list object";
                    }
                    auto self =(PythonClassWrapper<T>*)  PyList_GetItem(obj, index);

                    if(!PyObject_TypeCheck((PyObject*)self, PythonClassWrapper<T>::getPyType())){
                        throw "Invalid C type conversion";
                    }
                    if (!self->get_CObject()){
                        throw "Cannot set null item";
                    }
                    *self->get_CObject() = val;
                }
            }
        };

        template<>
        struct Setter<char*, void>{
            static void setItem(PyObject* obj, const size_t index, char* & val, const bool is_bytes, const bool is_str, const bool is_int, const bool is_float){
                if (is_bytes) {
                    PyList_SetItem(obj, index, PyBytes_FromString((const char *) val));
                } else if(is_str){
                    PyList_SetItem(obj, index, PyString_FromString((const char *) val));
                } else {
                    if(!PyObject_TypeCheck(obj, PythonClassWrapper<char*>::getPyType())){
                        throw "Invalid type for conversion";
                    }
                    auto self = (PythonClassWrapper<char*>*) PyList_GetItem(obj, index);
                    if (!self->get_CObject()){
                        throw "Cannot set null item";
                    }
                    *self->get_CObject() = val;
                }
            }
        };

        template<>
        struct Setter<const char*, void>{
            static void setItem(PyObject* obj, const size_t index, const char* & val, const bool is_bytes, const bool is_str, const bool is_int, const bool is_float){
                if(!PyList_Check(obj)){
                    throw "Attempt to set list item on non-list object";
                }
                if (is_bytes) {
                    PyList_SetItem(obj, index, PyBytes_FromString((const char *) val));
                } else if(is_str){
                    PyList_SetItem(obj, index, PyString_FromString((const char *) val));
                } else {
                    auto self = (PythonClassWrapper<const char*>*) PyList_GetItem(obj, index);
                    if(!PyObject_TypeCheck((PyObject*)self, PythonClassWrapper<const char*>::getPyType())){
                        throw "Invalid type for conversion";
                    }
                    if (!self->get_CObject()){
                        throw "Cannot set null item";
                    }
                    *self->get_CObject() = val;
                }
            }
        };

    }

    template<typename T, const size_t size, const bool array_allocated>
    typename CObjectConversionHelper<T[size], array_allocated >::ptr_t
    CObjectConversionHelper<T[size], array_allocated>::
    toCArgument(PyObject &pyobj) {
        auto self = (CommonBaseWrapper*) &pyobj;
        if (PyList_Check(&pyobj)) {
            bool is_bytes[size] = {false};
            bool is_str[size] = {false};
            bool is_int[size] = {false};
            bool is_float[size] = {false};
            if (PyList_Size(&pyobj) != size) {
                throw "Inconsistent sizes in array assignment";
            }
            NonConst_T_array *val = new NonConst_T_array[1];
            for (size_t i = 0; i < size; ++i) {
                PyObject *listitem = PyList_GetItem(&pyobj, i);
                if(!listitem){
                    throw "Invalid null value for list item in conversion to C array";
                }
                is_bytes[i] = PyBytes_Check(listitem);
                is_str[i] = PyString_Check(listitem);
                is_int[i] = PyLong_Check(listitem);
                is_float[i] = PyFloat_Check(listitem);
                (*val)[i] = *CObjectConversionHelper<T, array_allocated>::toCArgument(*listitem);
            }
            auto reverse_capture = [&pyobj, val, is_bytes, is_str, is_int, is_float]() {
                for (size_t i = 0; i < size; ++i) {
                    Setter<T>::setItem(&pyobj, i, (*val)[i], is_bytes[i], is_str[i], is_int[i], is_float[i]);
                }
            };
            return smart_ptr_with_reverse_capture<T[size], array_allocated>(val, reverse_capture, PTR_IS_ALLOCATED);
        }
        if (CommonBaseWrapper::template checkImplicitArgumentConversion<T[size]>(&pyobj)) {
            return smart_ptr_with_reverse_capture<T[size], array_allocated>(
                    reinterpret_cast<PythonClassWrapper<T[size]> * >(&pyobj)->get_CObject(),
                    nullptr,
                    PTR_IS_NOT_ALLOCATED);
        }
        throw "Conversion from incompatible type or const-ness";
    }

    template<typename T>
    PyObject *
    ConversionHelpers::PyObjectConversionHelper<T,
            typename std::enable_if<!std::is_integral<T>::value &&
                                    !std::is_enum<T>::value &&
                                    !std::is_floating_point<T>::value&&
                                    !is_c_string_like<T>::value &&
                                    !is_bytes_like<T>::value>::type>::
    toPyObject(typename std::remove_reference<T>::type &var, const bool asReference, const ssize_t array_size) {
        PyObject *pyobj =asReference?
                         (PyObject *) ClassWrapper::createPy(array_size, var, ContainmentKind::BY_REFERENCE):
                         (PyObject *)  ClassWrapper::createPy(array_size, *ObjectLifecycleHelpers::Copy<T>::new_copy(&var), ContainmentKind::ALLOCATED);
        if (!pyobj || !ClassWrapper::checkType(pyobj)) {
            PyErr_Format(PyExc_TypeError, "Unable to convert C type object to Python object %s: %s",
                         pyobj ? pyobj->ob_type->tp_name : "NULL OBJ", Types<T>::type_name());
            pyobj = nullptr;
        }
        return pyobj;
    }

    template<typename T>
    PyObject *ConversionHelpers::PyObjectConversionHelper<T, typename std::enable_if<
            std::is_integral<T>::value || std::is_enum<T>::value>::type>::
    toPyObject(const T &var, const bool asReference, const ssize_t array_size) {
        static PyObject *args = PyTuple_New(1);
        (void) asReference;
        PyTypeObject *type = PythonClassWrapper<T>::getPyType();
        PyTuple_SetItem(args, 0, PyLong_FromLong((long int) var));
        return PyObject_Call((PyObject *) type, args, nullptr);
    }


    template<typename T>
    PyObject *
    ConversionHelpers::PyObjectConversionHelper<T, typename std::enable_if<std::is_floating_point<T>::value>::type>::
    toPyObject(const T &var, const bool asReference, const ssize_t array_size) {
        static PyObject *args = PyTuple_New(1);
        (void) asReference;
        PyTypeObject *type = PythonClassWrapper<T>::getPyType();
        PyTuple_SetItem(args, 0, PyFloat_FromDouble((double) var));
        return PyObject_Call((PyObject*) type, args, nullptr);
    }

    template<typename T>
    PyObject *ConversionHelpers::PyObjectConversionHelper<T, typename std::enable_if<is_c_string_like<T>::value ||
        is_bytes_like<T>::value>::type>::
    toPyObject(T &var, const bool asReference, const ssize_t array_size) {
        static PyObject *args = PyTuple_New(1);
        (void) asReference;
        if (!var) {
            throw "NULL CHAR* encountered";
        }
        PyTypeObject *type = PythonClassWrapper<T>::getPyType();
        PyTuple_SetItem(args, 0, PyBytes_FromString((const char*)var));
        return PyObject_Call((PyObject*) type, args, nullptr);
    }

    template<typename T>
    PyObject *toPyObject(T &var, const bool asArgument, const ssize_t array_size) {
        return ConversionHelpers::PyObjectConversionHelper<T>::
        toPyObject(var, asArgument, array_size);
    }

    template<typename T>
    PyObject *toPyObject(const T &var, const bool asArgument, const ssize_t array_size) {
        return ConversionHelpers::PyObjectConversionHelper<const T>::toPyObject(
                var, asArgument, array_size);
    }

}

#endif
