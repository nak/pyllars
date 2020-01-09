//
// Created by jrusnak on 7/22/16.
//
#ifndef __PYLLARS__INTERNAL__CONVERSIONS_CPP__
#define __PYLLARS__INTERNAL__CONVERSIONS_CPP__

#include <Python.h>
#include "pyllars_integer.hpp"
#include "pyllars_floating_point.hpp"
#include "pyllars_utils.hpp"
#include "pyllars_conversions.hpp"
#include "pyllars/internal/pyllars_callbacks.hpp"

#include "pyllars_pointer.impl.hpp"
#include "pyllars_reference.impl.hpp"

namespace pyllars_internal {

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

    template<typename T>
    argument_capture<T>
    CObjectConversionHelper<T>::
    toCArgument(PyObject &pyobj) {
        typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type T_bare;

        if (PyObject_TypeCheck(&pyobj, PythonClassWrapper<T>::getPyType())) {
            return argument_capture<T>(*((PythonClassWrapper<T> *) &pyobj)->get_CObject());
        }

        auto *casted = (PythonClassWrapper<T> *) CommonBaseWrapper::reinterpret<T>(&pyobj);
        if (casted) {
            if (!casted->get_CObject()){
                throw PyllarsException(PyExc_RuntimeError, "Error casting through known conversion");
            }
            return argument_capture<T>(*casted->get_CObject());
        }
        if constexpr ((!std::is_reference<T>::value  || std::is_const<std::remove_reference_t <T>>::value) &&
                      std::is_fundamental<std::remove_reference_t <T> >::value) {
            if (PyObject_TypeCheck(&pyobj, PyNumberCustomBase::getPyType())) {
                auto obj = (PyNumberCustomBase *) &pyobj;
                auto *new_val = new std::remove_reference_t<T>(obj->template reinterpret<std::remove_reference_t <T> >());
                return argument_capture<T>(*new_val, [new_val]() { delete new_val; });
            } else if (PyObject_TypeCheck(&pyobj, PyFloatingPtCustomBase::getRawType())) {
                auto obj = (PyFloatingPtCustomBase *) &pyobj;
                auto *new_val = new std::remove_reference_t<T>(obj->template reinterpret<std::remove_reference_t <T> >());
                return argument_capture<T>(*new_val, [new_val]() { delete new_val; });
            } else if (PyLong_Check(&pyobj)){
                auto *new_val = new std::remove_reference_t<T>(std::is_signed<T>::value?PyLong_AsLongLong(&pyobj):
                                   PyLong_AsUnsignedLongLong(&pyobj));
                if (PyErr_Occurred()){
                    PyErr_Clear();
                    throw PyllarsException(PyExc_TypeError, "Attempt to cast PyLong to incompatible type");
                }
                return argument_capture<T>(*new_val, [new_val](){delete new_val;});
            } else if (PyInt_Check(&pyobj)){
                auto *new_val = new std::remove_reference_t<T>(PyInt_AsLong(&pyobj));
                return argument_capture<T>(*new_val, [new_val](){delete new_val;});
                if (PyErr_Occurred()){
                    PyErr_Clear();
                    throw PyllarsException(PyExc_TypeError, "Attempt to cast PyInt to incompatible type");
                }
            } else if (PyFloat_Check(&pyobj)){
                auto *new_val = new std::remove_reference_t<T>(PyFloat_AsDouble(&pyobj));
                if (PyErr_Occurred()){
                    PyErr_Clear();
                    throw PyllarsException(PyExc_TypeError, "Attempt to cast PyFloat to incompatible type");
                }
                return argument_capture<T>(*new_val, [new_val]() { delete new_val; });
            }
        }
        typedef std::remove_pointer_t< typename extent_as_pointer<std::remove_reference_t <T> >::type > T_element;
        if constexpr (is_pointer_like<std::remove_reference_t <T> >::value &&
                      std::is_const<T_element>::value &&
                      std::is_copy_assignable<std::remove_cv_t<T_element> >::value &&
                      std::is_constructible<std::remove_cv_t<T_element> , std::remove_cv_t<T_element> >::value) {
            if (PyList_Check(&pyobj)) {
                typedef std::remove_cv_t<T_element> T_bare;

                const size_t size = PyList_Size(&pyobj);
                if (size != 0) {
                    auto *element = PyList_GetItem(&pyobj, 0);
                    if constexpr (std::is_pointer<T>::value) {
                        T_element a_value = CObjectConversionHelper<T_element>::toCArgument(*element).value();
                        T values = new T_element[size]{a_value};
                        auto *values_ptr = new std::remove_reference_t<T>(values);
                        for (size_t i = 0; i < size; ++i) {
                            element = PyList_GetItem(&pyobj, i);
                            const_cast<std::remove_const_t<T_element> *>(values)[i] = CObjectConversionHelper<T_element>::toCArgument(
                                    *element).value();
                        }
                        return argument_capture<T>(*values_ptr, [values_ptr] ()->void{
                            delete values_ptr;
                        });
                    } else { //array of extent
                        if (size != ArraySize<T>::size) {
                            throw PyllarsException(PyExc_TypeError,
                                                   "Conversion to C array of extent of different size requested");
                        }
                        auto arg = CObjectConversionHelper<T_element>::toCArgument(*element);
                        //if (!arg){
                        //    throw PyllarsException(PyExc_RuntimeError, "Unable to convert to C argument");
                       // }
                        T_element a_value = arg.value();
                        auto *values_ptr = (std::remove_reference_t<T>*) new std::remove_reference_t<T>{{a_value}};
                        if (PyErr_Occurred()){
                            throw PyllarsException(PyExc_RuntimeError, "Unable to convert to C argument");
                        }
                        for (size_t i = 1; i < size; ++i) {
                            element = PyList_GetItem(&pyobj, i);
                            (const_cast<std::remove_const_t<T_element> *>(*values_ptr))[i] = CObjectConversionHelper<T_element>::toCArgument(
                                    *element).value();
                        }
                        return argument_capture<T>(*values_ptr,  [values_ptr] ()->void{
                            delete [] values_ptr;
                        });
                    }
                }
            }
        }

        if constexpr(is_c_string_like<std::remove_reference_t <T> >::value){
            if (PyUnicode_Check(&pyobj)){
                auto *text =  new std::remove_reference_t <T> (PyUnicode_AsUTF8(&pyobj));
                return argument_capture<T>(*text, [text]()->void{ delete text; });
            }
        } else if constexpr(is_bytes_like<std::remove_reference_t <T> >::value){
            if (PyBytes_Check(&pyobj)) {
                auto * bytes = new std::remove_reference_t<T>(PyBytes_AsString(&pyobj));
                return argument_capture<T>(*bytes, [bytes]()->void{ delete bytes;});
            }
        }

        typedef typename std::remove_reference<T_bare>::type T_NoRef;
        if constexpr (is_bool<T_bare>::value && std::is_const<std::remove_reference_t<T> >::value) {
            static bool TRUE = true;
            static bool FALSE = false;
            TRUE = true; //ensure these always remain unchanged/proper
            FALSE = false;
            if (&pyobj == Py_True) {
                return argument_capture<T>(TRUE);
            } else if (&pyobj == Py_False) {
                return argument_capture<T>(FALSE);
            }
        }
        throw PyllarsException(PyExc_TypeError, "Invalid type or const conversion converting to C object");
    }

    template<typename ReturnType, typename ...Args>
    argument_capture<ReturnType(*)(Args...)>
    CObjectConversionHelper<ReturnType(*)(Args...)>::
    toCArgument(PyObject &pyobj) {
        if (!PyCallable_Check(&pyobj)) {
            throw PyllarsException(PyExc_TypeError, "Python callback is not callable");
        }
        callback_t *retval = new callback_t(PyCallbackWrapper<ReturnType, Args...>(&pyobj).get_C_callback());
        return argument_capture<callback_t>(retval);
    }

    template<typename ReturnType, typename ...Args>
    argument_capture<ReturnType(*)(Args..., ...)>
    CObjectConversionHelper<ReturnType(*)(Args..., ...)>::
    toCArgument(PyObject &pyobj) {
        if (!PyCallable_Check(&pyobj)) {
            throw PyllarsException(PyExc_TypeError, "Python callback is not callable");
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
            if constexpr ( (std::is_array<T>::value ) || std::is_pointer<T>::value) {
                //if constexpr (!std::is_array<T>::_CObject){
                typedef typename std::remove_pointer_t<typename extent_as_pointer<T_NoRef>::type> T_element;
                typedef typename extent_as_pointer<T>::type T_bare;
                if constexpr (ArraySize<T_NoRef>::size > 0) {
                    typedef  std::remove_pointer_t <typename ClassWrapper::storage_type> storage_type;
                    T_bare vv = var;
                    auto * v = (storage_type) vv;
                    pyobj = (PyObject *) Pointers<T>::fromCArray(v, array_size);
                } else {
                    typename std::remove_reference<T>::type *v = nullptr;
                    v = new T(var);
                    pyobj = (PyObject *) Pointers<T>::fromCArray(*v, array_size);
                }
            } else {
                pyobj = (PyObject *) ClassWrapper::fromCObject(var);
            }
            if (!pyobj || !ClassWrapper::checkType(pyobj)) {
                if (PyErr_Occurred()) PyErr_Print();
                PyErr_Clear();
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