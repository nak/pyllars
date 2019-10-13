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
#include "pyllars_callbacks.hpp"
#include "pyllars_pointer.impl.hpp"
#include "pyllars_reference.impl.hpp"

namespace __pyllars_internal {

    namespace {

        template<typename T>
        struct Setter{
            static void setItem(PyObject* obj, const size_t index, T & val){
                if(!PyList_Check(obj)){
                    throw PyllarsException(PyExc_TypeError, "Invalid conversion in item of non-list object");
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
                    throw PyllarsException(PyExc_TypeError, "Invalid type conversion requested to non-integral type");
                } else if(PyFloat_Check(self)) {
                    if constexpr (std::is_floating_point<T>::value) {
                        PyList_SetItem(obj, index, PyFloat_FromDouble((double) val));
                        return;
                    }
                    throw PyllarsException(PyExc_TypeError, "Invalid type conversion requested to non-floating-point type");
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
                    throw PyllarsException(PyExc_TypeError, "Incompatible types in C conversion");
                }
                if (!self->get_CObject()) {
                    throw PyllarsException(PyExc_TypeError, "Cannot set null item");
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
        typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type T_bare;

        if constexpr(std::is_const<typename std::remove_reference<T>::type>::value || !std::is_reference<T>::value) {
            if (PythonClassWrapper<const T_bare>::checkType(&pyobj)) {
                return argument_capture<T>((T&)((const PythonClassWrapper<const T_bare> *) &pyobj)->toCArgument());
            } else if (PythonClassWrapper<const volatile T_bare>::checkType(&pyobj)) {
                if constexpr (std::is_volatile<T>::value) {
                    return (T&)((const PythonClassWrapper<const volatile T_bare> *) &pyobj)->toCArgument();
                }
            } else if (PythonClassWrapper<const T_bare &>::checkType(&pyobj)) {
                return (T&)((const PythonClassWrapper<const T_bare &> *) &pyobj)->toCArgument();
            } else if (PythonClassWrapper<const volatile T_bare &>::checkType(&pyobj)) {
                if constexpr (std::is_volatile<T>::value) {
                    return (T&)((const PythonClassWrapper<const volatile T_bare &> *) &pyobj)->toCArgument();
                }
            } else if (PythonClassWrapper<const volatile T_bare &&>::checkType(&pyobj)) {
                if constexpr (std::is_volatile<T>::value) {
                    return (T&)((const PythonClassWrapper<const volatile T_bare &&> *) &pyobj)->toCArgument();
                }
            } else if (PythonClassWrapper<const T_bare &&>::checkType(&pyobj)) {
                return (T&)((const PythonClassWrapper<const T_bare &&> *) &pyobj)->toCArgument();
            } else if (PythonClassWrapper<T_bare>::checkType(&pyobj)) {
                return ((PythonClassWrapper<T_bare> *) &pyobj)->toCArgument();
            } else if (PythonClassWrapper<T_bare&>::checkType(&pyobj)) {
                return ((PythonClassWrapper<T_bare&> *) &pyobj)->toCArgument();
            } else if (PythonClassWrapper<T_bare&&>::checkType(&pyobj)) {
                return ((PythonClassWrapper<T_bare&&> *) &pyobj)->toCArgument();
            } else if (PythonClassWrapper<volatile T_bare>::checkType(&pyobj)) {
                if constexpr (std::is_volatile<T>::value) {
                    return ((PythonClassWrapper<volatile T_bare> *) &pyobj)->toCArgument();
                }
            } else if (PythonClassWrapper<volatile T_bare &>::checkType(&pyobj)) {
                if constexpr (std::is_volatile<T>::value) {
                    return ((PythonClassWrapper<volatile T_bare &> *) &pyobj)->toCArgument();
                }
            } else if (PythonClassWrapper<volatile T_bare &&>::checkType(&pyobj)) {
                if constexpr (std::is_volatile<T>::value) {
                    return ((PythonClassWrapper<volatile T_bare &&> *) &pyobj)->toCArgument();
                }
            } else  if (&pyobj == Py_True || &pyobj == Py_False) {
                if constexpr (is_bool<T_bare>::value){
                    return argument_capture<T>(new bool(&pyobj == Py_True), false);
                }
            }
            typedef typename std::remove_reference<T_bare>::type T_NoRef;

            if constexpr(std::is_enum<T_bare>::value || std::is_integral<T_bare>::value){
                if (PyInt_Check(&pyobj)) {
                    if constexpr (std::is_signed<T_bare>::value) {
                        auto value = (T_bare) PyInt_AsLong(&pyobj);
                        return argument_capture<T>(new T_bare(value));
                    } else {
                        auto value = (T_bare) PyLong_AsUnsignedLongMask(&pyobj);
                        return argument_capture<T>(new T_bare(value));
                    }
                } else if (PyLong_Check(&pyobj)) {
                    // TODO: throughout code: be consistent on signed vs unsigned
                    // TODO: also add checks here and throughout on limits after conversion to C integral values
                    if (std::is_signed<T>::value) {
                        auto  value = (T_bare) PyLong_AsLongLong(&pyobj);
                        return argument_capture<T>(new T_bare(value));
                    } else {
                        T_bare value = (T_bare) PyLong_AsUnsignedLongLong(&pyobj);
                        return argument_capture<T>(new T_bare(value));
                    }
                }

            } else if constexpr (std::is_floating_point<T_NoRef >::value) {
                if (PyFloat_Check(&pyobj)) {
                    T_NoRef value = (T_NoRef) PyFloat_AsDouble(&pyobj);
                    return argument_capture<T>(new T_NoRef(value));
                }

            } else if constexpr(is_c_string_like<T_NoRef>::value){
                const char* text = nullptr;
                if (PyString_Check(&pyobj)) {
                    text= PyString_AsString(&pyobj);
                    if (!text) {
                        throw PyllarsException(PyExc_ValueError, "Error converting string: null pointer encountered");
                    }
                    return argument_capture<T>(new (const char*)(text), false);
                } else if (CommonBaseWrapper::template checkImplicitArgumentConversion<const char*>(&pyobj)) {
                    return argument_capture<T>(*((PythonClassWrapper<const char*> *)(&pyobj))->get_CObject());
                } else {
                    throw PyllarsException(PyExc_TypeError, "Invalid type or const conversion converting to C object");
                }

            } else if constexpr(is_bytes_like<T>::value) {
                char *bytes = nullptr;
                if (PyBytes_Check(&pyobj)) {
                    bytes = (char *) PyBytes_AsString(&pyobj);
                } else if (CommonBaseWrapper::template checkImplicitArgumentConversion<char *const>(&pyobj)) {
                    bytes = *reinterpret_cast<PythonClassWrapper<char *const> * >(&pyobj)->get_CObject();
                } else {
                    throw PyllarsException(PyExc_TypeError, "Invalid type or const conversion converting to C object");
                }
                if (!bytes) { throw PyllarsException(PyExc_ValueError, "Error converting string: null pointer encountered"); }
                return argument_capture<T>(new (char*)(bytes), false);
            } else if constexpr (std::is_array<T>::value && ArraySize<T>::size > 0) {
                constexpr auto size = ArraySize<T>::size;
                typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
                typedef typename std::remove_const<T_element>::type NonConst_T_array[size];
                if (PyList_Check(&pyobj)) {
                    if (PyList_Size(&pyobj) != size) {
                        throw PyllarsException(PyExc_TypeError, "Inconsistent sizes in array assignment");
                    }
                    NonConst_T_array *val = new NonConst_T_array[1];
                    for (size_t i = 0; i < size; ++i) {
                        PyObject *listitem = PyList_GetItem(&pyobj, i);
                        if (!listitem) {
                            throw PyllarsException(PyExc_ValueError, "Invalid null _CObject for list item in conversion to C array");
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
        } else  {
            //non-const reference
            if (PythonClassWrapper<const T_bare>::checkType(&pyobj) ||
                PythonClassWrapper<const volatile T_bare>::checkType(&pyobj) ||
                PythonClassWrapper<const T_bare &>::checkType(&pyobj) ||
                PythonClassWrapper<const volatile T_bare &>::checkType(&pyobj) ||
                PythonClassWrapper<const volatile T_bare &&>::checkType(&pyobj) ||
                PythonClassWrapper<const T_bare &&>::checkType(&pyobj)) {
                throw PyllarsException(PyExc_TypeError, "Cannot concert const type to a non-const reference");
            } else if (PythonClassWrapper<T_bare>::checkType(&pyobj)) {
                return ((PythonClassWrapper<T_bare> *) &pyobj)->toCArgument();
            } else if (PythonClassWrapper<T_bare&>::checkType(&pyobj)) {
                return ((PythonClassWrapper<T_bare&> *) &pyobj)->toCArgument();
            } else if (PythonClassWrapper<T_bare&&>::checkType(&pyobj)) {
                return ((PythonClassWrapper<T_bare&&> *) &pyobj)->toCArgument();
            } else if (PythonClassWrapper<volatile T_bare>::checkType(&pyobj)) {
                if constexpr (std::is_volatile<T>::value) {
                    return ((PythonClassWrapper<volatile T_bare> *) &pyobj)->toCArgument();
                }
            } else if (PythonClassWrapper<volatile T_bare &>::checkType(&pyobj)) {
                if constexpr (std::is_volatile<T>::value) {
                    return ((PythonClassWrapper<volatile T_bare &> *) &pyobj)->toCArgument();
                }
            } else if (PythonClassWrapper<volatile T_bare &&>::checkType(&pyobj)) {
                if constexpr (std::is_volatile<T>::value) {
                    return ((PythonClassWrapper<volatile T_bare &&> *) &pyobj)->toCArgument();
                }
            }
            throw PyllarsException(PyExc_TypeError, "Cannot convert immutable Python type to C reference type");
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
    struct PythonClassWrapper<T&, void>;

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
                typedef typename std::remove_pointer<typename extent_as_pointer<T_NoRef>::type>::type T_base;
                if constexpr (ArraySize<T_NoRef>::size > 0) {

                    //the rules of C++ and fixed arrays vs pointers and pass-by-reference are strange and mysterious...
                    T_base** v = new (T_base*)(&var[0]);
                    T* vv = (T*) *v;
                    pyobj = (PyObject *) ClassWrapper::fromCPointer((T&)*vv, array_size);
                } else {
                    typename std::remove_reference<T>::type *v = nullptr;
                    v = new T(var);
                    pyobj = (PyObject *) ClassWrapper::fromCPointer(*v, array_size);
                }
            } else {
                pyobj = (PyObject *) ClassWrapper::fromCObject(var);
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