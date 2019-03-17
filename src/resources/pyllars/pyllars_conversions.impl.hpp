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


    template<typename T>
    argument_capture<T>
    CObjectConversionHelper<T>::
    toCArgument(PyObject &pyobj) {
        typedef typename std::remove_reference<T>::type T_NoRef;
        auto self = (CommonBaseWrapper*) &pyobj;
        if constexpr(is_bool<T>::value) {
            return &pyobj == Py_True?true:false;
        } else if constexpr(std::is_enum<T_NoRef>::value || std::is_integral<T_NoRef>::value){
            typedef typename std::remove_reference<typename std::remove_const<T>::type>::type T_bare;
            if (PyInt_Check(&pyobj)) {
                T_bare *value = new T_bare((T_bare) PyInt_AsLong(&pyobj));
                return argument_capture<T>(value);
            } else if (PyLong_Check(&pyobj)) {
                T_bare *value = new T_bare((T_bare) PyLong_AsLongLong(&pyobj));
                return argument_capture<T>(value);
            }
            if (CommonBaseWrapper::template checkImplicitArgumentConversion<T>(&pyobj)) {
                return argument_capture<T>(
                        *reinterpret_cast<PythonClassWrapper<T> * >(&pyobj)->get_CObject());
            }
        } else if constexpr (std::is_floating_point<T_NoRef >::value) {
            if (PyFloat_Check(&pyobj)) {
                if (!std::is_const<T>::value && std::is_reference<T>::value) {
                    throw "Invalid const-ness in conversion of argument";
                }
                T *value = new T(PyFloat_AsDouble(&pyobj));
                return argument_capture<T>(value);
            }
            if (CommonBaseWrapper::template checkImplicitArgumentConversion<T>(&pyobj)) {
                return argument_capture<T>(
                        *reinterpret_cast<PythonClassWrapper<T> * >(&pyobj)->get_CObject());
            }
        } else if constexpr(is_c_string_like<T>::value){
            auto self = (CommonBaseWrapper*) &pyobj;
            const char* text = nullptr;
            if (PyString_Check(&pyobj)) {
                text= (const char *) PyString_AsString(&pyobj);
            } else if (CommonBaseWrapper::template checkImplicitArgumentConversion<const char*>(&pyobj)) {
                text = *((PythonClassWrapper<const char*> *)(&pyobj))->get_CObject();
            } else {
                throw "Invalid type or const conversion converting to C object";
            }
            if (!text) {
                throw "Error converting string: null pointer encountered";
            }
            return argument_capture<T>(new (T)(text), false);// &pyobj);
        } else if constexpr(is_bytes_like<T>::value) {
            auto self = (CommonBaseWrapper *) &pyobj;
            char *bytes = nullptr;
            if (PyBytes_Check(&pyobj)) {
                bytes = (char *) PyBytes_AsString(&pyobj);
            } else if (CommonBaseWrapper::template checkImplicitArgumentConversion<char *const>(&pyobj)) {
                bytes = *reinterpret_cast<PythonClassWrapper<char *const> * >(&pyobj)->get_CObject();
            } else {
                throw "Invalid type or const conversion converting to C object";
            }
            if (!bytes) { throw "Error converting string: null pointer encountered"; }
            return smart_py_reference<T>(new (T)(bytes), &pyobj);
        } else if constexpr (std::is_array<T>::value && ArraySize<T>::size > 0){
            auto self = (CommonBaseWrapper*) &pyobj;
            constexpr auto size = ArraySize<T>::size;
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
            typedef typename std::remove_const<T_element>::type NonConst_T_array[size];
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
                    (*val)[i] = CObjectConversionHelper<T_element>::toCArgument(*listitem).value();
                }
                auto reverse_capture = [&pyobj, val, is_bytes, is_str, is_int, is_float]() {
                    if constexpr(!std::is_const<T>::value) {
                        for (size_t i = 0; i < size; ++i) {
                            Setter<T_element>::setItem(&pyobj, i, (*val)[i], is_bytes[i], is_str[i], is_int[i], is_float[i]);
                        }
                    }
                };
                return argument_capture<T>(val, true, reverse_capture);
            }
        }
        if (CommonBaseWrapper::template checkImplicitArgumentConversion<T>(&pyobj)) {
            return argument_capture<T>(
                   *reinterpret_cast<PythonClassWrapper<T> * >(&pyobj)->get_CObject());
        }
        throw "Invalid type or const conversion converting to C object";
    }

    template<typename ReturnType, typename ...Args>
    argument_capture<ReturnType(*)(Args...)>
    CObjectConversionHelper<ReturnType(*)(Args...)>::
    toCArgument(PyObject &pyobj) {
        if (!PyCallable_Check(&pyobj)) {
            throw "Python callback is not callable!";
        }
        callback_t *retval = new callback_t(PyCallbackWrapper<ReturnType, Args...>(&pyobj).get_C_callback());
        return smart_ptr<callback_t, false>(retval, PTR_IS_ALLOCATED);
    }

    template<typename ReturnType, typename ...Args>
    argument_capture<ReturnType(*)(Args..., ...)>
    CObjectConversionHelper<ReturnType(*)(Args..., ...)>::
    toCArgument(PyObject &pyobj) {
        if (!PyCallable_Check(&pyobj)) {
            throw "Python callback is not callable!";
        }
        callback_t *retval = new callback_t(PyCallbackWrapperVar<ReturnType, Args...>(&pyobj).get_C_callback());
        return argument_capture<callback_t >(retval);
    }

    ///////////////////////////


    template<typename T>
    PyObject *
    ConversionHelpers::PyObjectConversionHelper<T>::
    toPyObject(typename std::remove_reference<T>::type &var, const bool asReference, const ssize_t array_size) {
        if constexpr(is_bool<T>::value) {
            return var?Py_True:Py_False;
        } else if constexpr (std::is_integral<T>::value || std::is_enum<T>::value){
                (void) asReference;
            PyTypeObject *type = PythonClassWrapper<T>::getPyType();
            PyObject *args = PyTuple_New(1);
            auto pyInt = PyLong_FromLong((long int) var);
            PyTuple_SetItem(args, 0, pyInt);
            auto ret = PyObject_Call((PyObject *) type, args, nullptr);
            Py_DECREF(args);
            return ret;
        } else if constexpr(std::is_floating_point<T>::value) {
            (void) asReference;
            PyTypeObject *type = PythonClassWrapper<T>::getPyType();
            PyObject *pyFloat = PyFloat_FromDouble((double) var);
            PyObject *args = PyTuple_New(1);
            PyTuple_SetItem(args, 0, pyFloat);
            auto ret = PyObject_Call((PyObject *) type, args, nullptr);
            Py_DECREF(args);
            return ret;
        } else if constexpr (is_c_string_like<T>::value){
            (void) asReference;
            if (!var) {
                throw "NULL CHAR* encountered";
            }
            PyTypeObject *type = PythonClassWrapper<T>::getPyType();
            auto obj =  PyUnicode_FromString((const char*)var);
            if(!obj){
                PyErr_SetString(PyExc_ValueError, "Unable to extact unicode from given c-string");
                return nullptr;
            }
            PyObject *args = PyTuple_New(1);
            PyTuple_SetItem(args, 0, obj);
            auto ret = PyObject_Call((PyObject*) type, args, nullptr);
            Py_DECREF(args);
            if(!ret){
                PyErr_SetString(PyExc_TypeError, "Invalid conversion from C object to Python");
            }
            return ret;
        } else if constexpr (is_bytes_like<T>::value){
            (void) asReference;
            if (!var) {
                throw "NULL CHAR* encountered";
            }
            PyTypeObject *type = PythonClassWrapper<T>::getPyType();
            auto obj =  PyBytes_FromString((char*)var);
            if(!obj){
                PyErr_SetString(PyExc_ValueError, "Unable to extact unicode from given c-string");
                return nullptr;
            }
            PyObject *args = PyTuple_New(1);
            PyTuple_SetItem(args, 0, obj);
            auto ret = PyObject_Call((PyObject*) type, args, nullptr);
            Py_DECREF(args);
            if(!ret){
                PyErr_SetString(PyExc_TypeError, "Invalid conversion from C object to Python");
            }
            return ret;
        } else {
            PyObject *pyobj = asReference ?
                              (PyObject *) ClassWrapper::createPy(array_size, var, ContainmentKind::BY_REFERENCE) :
                              (PyObject *) ClassWrapper::createPy(array_size,
                                                                  *ObjectLifecycleHelpers::Copy<T>::new_copy(&var),
                                                                  ContainmentKind::ALLOCATED);
            if (!pyobj || !ClassWrapper::checkType(pyobj)) {
                PyErr_Format(PyExc_TypeError, "Unable to convert C type object to Python object %s: %s",
                             pyobj ? pyobj->ob_type->tp_name : "NULL OBJ", Types<T>::type_name());
                pyobj = nullptr;
            }
            return pyobj;
        }
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
