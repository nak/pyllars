//
// Created by jrusnak on 7/22/16.
//
#ifndef __PYLLARS__INTERNAL__CONVERSIONS_CPP__
#define __PYLLARS__INTERNAL__CONVERSIONS_CPP__

#include <Python.h>
#include "pyllars_utils.hpp"
#include "pyllars_conversions.hpp"
#include "pyllars_object_lifecycle.impl.hpp"
#include "pyllars_callbacks.hpp"
#include "pyllars_pointer.hpp"

namespace __pyllars_internal {

    namespace {

        template<typename T>
        struct Setter{
            static void setItem(PyObject* obj, const size_t index, T & val){
                if(!PyList_Check(obj)){
                    throw "Invalid conversion in item of non-list object";
                }
                auto self = (PythonClassWrapper<T> *) PyList_GetItem(obj, index);
                if (PyLong_Check(self)) {
                    if constexpr (std::is_integral<T>::value) {
                        if constexpr (std::is_signed<T>::value) {
                            PyList_SetItem(obj, index, PyLong_FromLongLong((long long) val));
                        } else {
                            PyList_SetItem(obj, index, PyLong_FromUnsignedLongLong((unsigned long long) val));
                        }
                        return;
                    }
                    throw "Invalid type conversion requested to non-integral type";
                } else if(PyFloat_Check(self)) {
                    if constexpr (std::is_floating_point<T>::value) {
                        PyList_SetItem(obj, index, PyFloat_FromDouble((double) val));
                        return;
                    }
                    throw "Invalid type conversion requested to non-floating-point type";
                } else if constexpr (is_c_string_like<T>::value) {
                    if (PyString_Check(self)) {
                        PyList_SetItem(obj, index, PyString_FromString((const char *) val));
                        return;
                    }
                } else if constexpr (is_bytes_like<T>::value) {
                    if (PyBytes_Check(self)){
                        PyList_SetItem(obj, index, PyBytes_FromString(val));
                        return;
                    }
                }
                if (!PyObject_TypeCheck((PyObject *) self, PythonClassWrapper<T>::getPyType())) {
                    throw "Incompatible types in C conversion";
                }
                if (!self->get_CObject()) {
                    throw "Cannot set null item";
                }
                *self->get_CObject() = val;
            }
        };

    }


    /**
     * template function to convert python to C object (for purpose of passing
     * as paramter to a C function)
     **/
    template<typename T >
    class CObjectConversionHelper {
    public:
        typedef typename std::remove_reference<T>::type T_bare;

        static argument_capture<T> toCArgument(PyObject &pyobj) ;
    };

    /**
     * Specialization for callbacks (functions)
     **/
    template<typename ReturnType, typename ...Args>
    class CObjectConversionHelper<ReturnType(*)(Args...)> {
    public:
        typedef ReturnType(*callback_t)(Args...);

        static argument_capture<callback_t > toCArgument(PyObject &pyobj) ;
    };

    /**
     * Specialization for callbacks (functions) with ellipsis argument
     **/
    template<typename ReturnType, typename ...Args>
    class CObjectConversionHelper<ReturnType(*)(Args..., ...)> {
    public:
        typedef ReturnType(*callback_t)(Args..., ...);

        static argument_capture<callback_t> toCArgument(PyObject &pyobj);
    };

    template<typename T >
    argument_capture<T>
    toCArgument(PyObject &pyobj) {
        return CObjectConversionHelper<T>::toCArgument(pyobj);
    }

    template<typename T, const size_t size >
    argument_capture<T[size]>
    toCArgument(PyObject &pyobj) {
        return CObjectConversionHelper<T[size] >::toCArgument(pyobj);
    }

    template<typename T>
    argument_capture<T>
    CObjectConversionHelper<T>::
    toCArgument(PyObject &pyobj) {
        constexpr bool is_const_ref = std::is_reference<T>::value && std::is_const<T>::value;
        typedef typename std::remove_reference<T>::type T_NoRef;
        if constexpr(is_bool<T>::value) {
            return &pyobj == Py_True?true:false;
        } else if constexpr(is_const_ref || (std::is_enum<T_NoRef>::value || std::is_integral<T_NoRef>::value)){
            typedef typename std::remove_reference<typename std::remove_const<T>::type>::type T_bare;
            if (PyInt_Check(&pyobj)) {
                if constexpr (std::is_signed<T>::value) {
                    T_bare value = (T_bare) PyInt_AsLong(&pyobj);
                    return argument_capture<T>(new T_bare(value));
                } else {
                    T_bare value = (T_bare) PyLong_AsUnsignedLongMask(&pyobj);
                    return argument_capture<T>(new T_bare(value));
                }
            } else if (PyLong_Check(&pyobj)) {
                // TODO: throughout code: be consistent on signed vs unsigned
                // TODO: also add checks here and throughout on limits after conversion to C integral values
                if (std::is_signed<T>::value) {
                    T_bare value = (T_bare) PyLong_AsLongLong(&pyobj);
                    return argument_capture<T>(new T_bare(value));
                } else {
                    T_bare value = (T_bare) PyLong_AsUnsignedLongLong(&pyobj);
                    return argument_capture<T>(new T_bare(value));
                }
            }
            if (CommonBaseWrapper::template checkImplicitArgumentConversion<T>(&pyobj)) {
                return argument_capture<T>(
                        *reinterpret_cast<PythonClassWrapper<T> * >(&pyobj)->get_CObject());
            }
        } else if constexpr (is_const_ref || std::is_floating_point<T_NoRef >::value) {
            if (PyFloat_Check(&pyobj)) {
                T_NoRef value = (T_NoRef) PyFloat_AsDouble(&pyobj);
                return argument_capture<T>(new T_NoRef(value));
            }

        } else if constexpr(is_c_string_like<T>::value){
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
            char *bytes = nullptr;
            if (PyBytes_Check(&pyobj)) {
                bytes = (char *) PyBytes_AsString(&pyobj);
            } else if (CommonBaseWrapper::template checkImplicitArgumentConversion<char *const>(&pyobj)) {
                bytes = *reinterpret_cast<PythonClassWrapper<char *const> * >(&pyobj)->get_CObject();
            } else {
                throw "Invalid type or const conversion converting to C object";
            }
            if (!bytes) { throw "Error converting string: null pointer encountered"; }
            return argument_capture<T>(new (T)(bytes), false);// &pyobj);
        } else if constexpr (std::is_array<T>::value && ArraySize<T>::size > 0){
            constexpr auto size = ArraySize<T>::size;
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
            typedef typename std::remove_const<T_element>::type NonConst_T_array[size];
            if (PyList_Check(&pyobj)) {
                if (PyList_Size(&pyobj) != size) {
                    throw "Inconsistent sizes in array assignment";
                }
                NonConst_T_array *val = new NonConst_T_array[1];
                for (size_t i = 0; i < size; ++i) {
                    PyObject *listitem = PyList_GetItem(&pyobj, i);
                    if(!listitem){
                        throw "Invalid null value for list item in conversion to C array";
                    }
                    (*val)[i] = CObjectConversionHelper<T_element>::toCArgument(*listitem).value();
                }
                auto reverse_capture = [&pyobj, val]() {
                    if constexpr(!std::is_const<T>::value) {
                        for (size_t i = 0; i < size; ++i) {
                            Setter<T_element>::setItem(&pyobj, i, (*val)[i]);
                        }
                    }
                };
                return argument_capture<T>(val, true, reverse_capture);
            }
        }
        if (CommonBaseWrapper::template checkImplicitArgumentConversion<T>(&pyobj)) {
            T & val =  *reinterpret_cast<PythonClassWrapper<T> * >(&pyobj)->get_CObject();
            return argument_capture<T>(val);
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
        return argument_capture<callback_t>(retval);
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


    /////////////////////////
    // C-to-Python conversions
    //////////////////////////


    class ConversionHelpers {
    public:

        ///////////
        // Helper conversion functions
        //////////

        /**
         * Define conversion helper class, which allows easier mechanism
         * for necessary specializations
         **/
        template<typename T>
        class PyObjectConversionHelper{
        public:
            typedef PythonClassWrapper<T> ClassWrapper;
            typedef typename std::remove_reference<T>::type T_NoRef;

            static PyObject *toPyObject(T var, const ssize_t array_size = -1);
        };


    };


    template<typename T>
    PyObject *
    ConversionHelpers::PyObjectConversionHelper<T>::
    toPyObject(T var, const ssize_t array_size) {
        if constexpr(is_bool<T>::value) {
            return var?Py_True:Py_False;
        } else {
            PyObject *pyobj = nullptr;
            if constexpr (std::is_array<typename std::remove_reference<T>::type>::value ||
                          std::is_pointer<typename std::remove_reference<T>::type>::value) {
                if constexpr (std::is_reference<T>::value){
                    pyobj = (PyObject *) ClassWrapper::createPyReference(var, array_size);
                } else {
                    pyobj = (PyObject *) ClassWrapper::createPyFromAllocatedInstance(
                            *ObjectLifecycleHelpers::Copy<T>::new_copy(&var),
                            array_size);
                }
            } else {
                if constexpr (std::is_reference<T>::value) {
                    pyobj = (PyObject *) ClassWrapper::createPyReference(var);
                } else {
                    pyobj = (PyObject *) ClassWrapper::createPyFromAllocated( ObjectLifecycleHelpers::Copy<T>::new_copy(&var));
                }
            }
            if (!pyobj || !ClassWrapper::checkType(pyobj)) {
                PyErr_Format(PyExc_TypeError, "Unable to convert C type object to Python object %s: %s",
                             pyobj ? pyobj->ob_type->tp_name : "NULL OBJ", Types<T>::type_name());
                pyobj = nullptr;
            }
            return pyobj;
        }
    }

    template<typename T>
    PyObject *toPyObject(T var,  const ssize_t array_size) {
        return ConversionHelpers::PyObjectConversionHelper<T>::toPyObject(var, array_size);
    }

    template<typename T, ssize_t size>
    PyObject *toPyObject(T var[size],  const ssize_t array_size) {
        return ConversionHelpers::PyObjectConversionHelper<T[size]>::toPyObject(var, size);
    }

    ////////////////////


    template<typename T>
    PyObject *toPyArgument(T &var, const ssize_t array_size){
        return ConversionHelpers::PyObjectConversionHelper<T&>::toPyObject(var, array_size);
    }

    template<typename T, size_t size>
    PyObject *toPyArgument(T var[size], const ssize_t){
        return ConversionHelpers::PyObjectConversionHelper<T[size]>::toPyObject(var, size);
    }
}

#endif