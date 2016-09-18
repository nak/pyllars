//
// Created by jrusnak on 7/22/16.
//
#ifndef __PYLLARS__INTERNAL__CONVERSIONS_CPP__
#define __PYLLARS__INTERNAL__CONVERSIONS_CPP__

#include "pyllars_conversions.hpp"
#include "pyllars_callbacks.cpp"
#include "pyllars_pointer.cpp"

namespace __pyllars_internal {


    template<typename T, bool array_allocated, typename ClassWrapper>
    smart_ptr<typename std::remove_reference<T>::type, array_allocated> CObjectConversionHelper<T, array_allocated, ClassWrapper,
            typename std::enable_if<!std::is_array<T>::value &&
                                    !std::is_function<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_enum<typename std::remove_reference<T>::type>::value &&
                                    !std::is_integral<typename std::remove_reference<T>::type>::value &&
                                    !std::is_floating_point<typename std::remove_reference<T>::type>::value>::type>::
    toCObject(PyObject &pyobj) {
        //if (&pyobj == nullptr) {
        //    throw "Invalid argument for conversion";
        //}
        // Check different pointer type matches
        if (ClassWrapper::checkType(&pyobj)) {
            return smart_ptr<T_bare, array_allocated>((T_bare *)
                                                       reinterpret_cast<ClassWrapper *>
                                                       (&pyobj)->template get_CObject<T_bare>(), PTR_IS_NOT_ALLOCATED);
        } else if (NoRefClassWrapper::checkType(&pyobj)) {
            return smart_ptr<T_bare, array_allocated>(reinterpret_cast<NoRefClassWrapper *>(&pyobj)->
                    template get_CObject<typename std::remove_const<T_bare>::type>(), PTR_IS_NOT_ALLOCATED);
        } else if (ConstClassWrapper::checkType(&pyobj)) {
            //Must make copy of this item, since cast from const T to T requested
            //and cannot cast away const-ness
            ConstClassWrapper *self = reinterpret_cast<ConstClassWrapper *>(&pyobj);
            T_bare *copy = (T_bare *) ObjectLifecycleHelpers::Copy<const T>::new_copy(
                    *self->template get_CObject<const T>());
            return smart_ptr<T_bare, array_allocated>(copy, PTR_IS_ALLOCATED);
        } else if (NonConstClassWrapper::checkType(&pyobj)) {
            return smart_ptr<T_bare, array_allocated>(reinterpret_cast<NonConstClassWrapper *>(&pyobj)->
                    template get_CObject<typename std::remove_const<T_bare>::type>(), PTR_IS_NOT_ALLOCATED);
        } else {
            fprintf(stderr, "\n");
            PyObject_Print(&pyobj, stderr, 0);
            fprintf(stderr, "\n");
            PyObject_Print(PyObject_Type(&pyobj), stderr, 0);
            fprintf(stderr, "\n");
            throw "Invalid type conversion when converting to C object";
        }
    }


    template<typename T, bool array_allocated, typename ClassWrapper>
    smart_ptr<T, array_allocated> CObjectConversionHelper<T, array_allocated, ClassWrapper, typename std::enable_if<
            std::is_integral<typename std::remove_reference<T>::type>::value
            || std::is_enum<typename std::remove_reference<T>::type>::value>::type>::
    toCObject(PyObject &pyobj) {
        typedef typename std::remove_reference<typename std::remove_const<T>::type>::type T_bare;
        if (PyInt_Check(&pyobj)) {
            T_bare *value = new T_bare((T_bare) PyInt_AsLong(&pyobj));
            return smart_ptr<T, array_allocated>(value, PTR_IS_ALLOCATED);
        } else if (PyLong_Check(&pyobj)) {
            T_bare *value = new T_bare((T_bare) PyLong_AsLongLong(&pyobj));
            return smart_ptr<T, array_allocated>(value, PTR_IS_ALLOCATED);
        } else if (ClassWrapper::checkType(&pyobj)) {
            return smart_ptr<T, array_allocated>(reinterpret_cast<ClassWrapper * >(&pyobj)->template get_CObject<T_bare>(),
                                          PTR_IS_NOT_ALLOCATED);
        } else if (NoRefClassWrapper::checkType(&pyobj)) {
            return smart_ptr<T, array_allocated>(
                    reinterpret_cast<NoRefClassWrapper * >(&pyobj)->template get_CObject<T_bare>(),
                    PTR_IS_NOT_ALLOCATED);
        } else if (ConstClassWrapper::checkType(&pyobj)) {
            //Attempting to cast const T to T  which requires a copy
            ConstClassWrapper *self = reinterpret_cast<ConstClassWrapper *>(&pyobj);
            T_bare *copy = (T_bare *) ObjectLifecycleHelpers::Copy<const T>::new_copy(
                    *self->template get_CObject<const T>());
            return smart_ptr<T, array_allocated>(copy, PTR_IS_ALLOCATED);
        } else if (NonConstClassWrapper::checkType(&pyobj)) {
            //casting T to const T:
            return smart_ptr<T, array_allocated>(
                    reinterpret_cast<NonConstClassWrapper * >(&pyobj)->template get_CObject<std::remove_const<T_bare> >(),
                    PTR_IS_NOT_ALLOCATED);
        }
        throw "Invalid type converting to C object";
    }

    template<typename T, bool array_allocated, typename ClassWrapper>
    smart_ptr<T, array_allocated>  CObjectConversionHelper<T, array_allocated, ClassWrapper,
            typename std::enable_if<std::is_floating_point<typename std::remove_reference<T>::type>::value>::type>::
    toCObject(PyObject &pyobj) {
        typedef typename std::remove_reference<T>::type T_bare;

        if (PyFloat_Check(&pyobj)) {
            return smart_ptr<T, array_allocated>(new T_bare(PyFloat_AsDouble(&pyobj)), PTR_IS_ALLOCATED);
        } else if (ClassWrapper::checkType(&pyobj)) {
            T_bare *retval = (reinterpret_cast<ClassWrapper * >(&pyobj)->template get_CObject<T_bare>());
            return smart_ptr<T, array_allocated>(retval, PTR_IS_NOT_ALLOCATED);
        } else if (NoRefClassWrapper::checkType(&pyobj)) {
            T_bare *retval = (reinterpret_cast<NoRefClassWrapper * >(&pyobj)->template get_CObject<T_bare>());
            return smart_ptr<T, array_allocated>(retval, PTR_IS_NOT_ALLOCATED);
        } else if (NonConstClassWrapper::checkType(&pyobj)) {
            T_bare *retval = (reinterpret_cast<NonConstClassWrapper * >(&pyobj)->template get_CObject<T_bare>());
            return smart_ptr<T, array_allocated>(retval, PTR_IS_NOT_ALLOCATED);
        } else if (ConstClassWrapper::checkType(&pyobj)) {
            //Attempting to cast const T to T  which requires a copy
            ConstClassWrapper *self = reinterpret_cast<ConstClassWrapper *>(&pyobj);
            T_bare *copy = (T_bare *) ObjectLifecycleHelpers::Copy<const T>::new_copy(
                    *self->template get_CObject<const T>());
            return smart_ptr<T, array_allocated>(copy, PTR_IS_ALLOCATED);
        }
        PyObject_Print(&pyobj, stderr, 0);
        throw "Invalid type converting to C object";
    }

    template<typename ClassWrapper, typename ReturnType,
            typename ...Args>
    typename CObjectConversionHelper<ReturnType(*)(Args...), false, ClassWrapper, void>::ptr_t
    CObjectConversionHelper<ReturnType(*)(Args...), false, ClassWrapper, void>::
    toCObject(PyObject &pyobj) {
        if (!PyCallable_Check(&pyobj)) {
            throw "Python callback is not callable!";
        }
        callback_t *retval = new callback_t(PyCallbackWrapper<ReturnType, Args...>(&pyobj).get_C_callback());
        return smart_ptr<callback_t, false>(retval, PTR_IS_ALLOCATED);
    }

    template<typename ClassWrapper, typename ReturnType,
            typename ...Args>
    typename CObjectConversionHelper<ReturnType(*)(Args..., ...), false, ClassWrapper, void>::ptr_t
    CObjectConversionHelper<ReturnType(*)(Args..., ...), false, ClassWrapper, void>::
    toCObject(PyObject &pyobj) {
        if (!PyCallable_Check(&pyobj)) {
            throw "Python callback is not callable!";
        }
        callback_t *retval = new callback_t(PyCallbackWrapperVar<ReturnType, Args...>(&pyobj).get_C_callback());
        return smart_ptr<callback_t, false>(retval, PTR_IS_ALLOCATED);
    }

    template<typename ClassWrapper, bool array_allocated>
    smart_ptr<const char *, array_allocated>
    CObjectConversionHelper<const char *, array_allocated, ClassWrapper>::
    toCObject(PyObject &pyobj) {
        const char *name = nullptr;
        if (ConstClassWrapper::checkType(&pyobj)) {
            ConstClassWrapper *const self_ = (ConstClassWrapper *const) &pyobj;
            name = (const char *) self_->template get_CObject<const char *const>();
        } else if (ClassWrapper::checkType(&pyobj)) {
            ClassWrapper *const self_ = (ClassWrapper *const) &pyobj;
            name = (const char *) self_->template get_CObject<const char *>();
        } else if (NonConstClassWrapper::checkType(&pyobj)) {
            NonConstClassWrapper *const self_ = (NonConstClassWrapper *const) &pyobj;
            name = (const char *) self_->template get_CObject<const char *>();
        } else if (PyString_Check(&pyobj)) {
            name = (const char *) PyString_AS_STRING(&pyobj);
        } else {
            PyObject_Print(&pyobj, stderr, 0);
            throw "Conversion to C string from non-string or incompatible Python object!";
        }
        if (!name) { throw "Error converting string: null pointer encountered"; }

        return smart_ptr<const char *, false>(new (const char *)(name), PTR_IS_ALLOCATED);
    }

    template<typename ClassWrapper, bool array_allocated>
    typename CObjectConversionHelper<const char *const, array_allocated, ClassWrapper>::ptr_t
    CObjectConversionHelper<const char *const, array_allocated, ClassWrapper>::
    toCObject(PyObject &pyobj) {
        const char *name = nullptr;
        if (NonConstClassWrapper::checkType(&pyobj)) {
            NonConstClassWrapper *const self_ = ((NonConstClassWrapper *const) &pyobj);
            name = (const char *) self_->template get_CObject<const char *>();
        } else if (ClassWrapper::checkType(&pyobj)) {
            ClassWrapper *const self_ = ((ClassWrapper *const) &pyobj);
            name = (const char *) self_->template get_CObject<const char *const>();
        } else if (ConstClassWrapper::checkType(&pyobj)) {
            throw "Attempt to cast away const-ness (c-string)";
        } else if (PyString_Check(&pyobj)) {
            name = (const char *) PyString_AS_STRING(&pyobj);
        } else {
            PyObject_Print(&pyobj, stderr, 0);
            throw "Conversion to C-string from non-string Python object!";
        }
        if (!name) { throw "Error converting string: null pointer encountered"; }

        return smart_ptr<const char *const, false>(new (const char *)(name), PTR_IS_ALLOCATED);
    }

    template<typename ClassWrapper, bool array_allocated>
    typename CObjectConversionHelper<char *const, array_allocated, ClassWrapper>::ptr_t
    CObjectConversionHelper<char *const, array_allocated, ClassWrapper>::
    toCObject(PyObject &pyobj) {
        const char *name = nullptr;
        if (PyString_Check(&pyobj)) {
            name = (const char *) PyString_AS_STRING(&pyobj);
        } else {
            throw "Conversiont o C stgring from non-string Python object!";
        }
        if (!name) { throw "Error converting string: null pointer encountered"; }
        //since char * contents are mutable, must make copy
        //on return
        char *new_name = new char[strlen(name) + 1]{0};
        strcpy(new_name, name);
        return smart_ptr<char *const, false>(&new_name, array_allocated);//PTR_IS_ALLOCATED);
    }

    template< typename ClassWrapper, bool array_allocated>
    typename CObjectConversionHelper<char *, array_allocated, ClassWrapper>::ptr_t
    CObjectConversionHelper<char *, array_allocated, ClassWrapper>::
    toCObject(PyObject &pyobj) {
        const char *name = nullptr;
        if (PyString_Check(&pyobj)) {
            name = (const char *) PyString_AS_STRING(&pyobj);
        } else {
            throw "Conversion o C string from non-string Python object!";
        }
        if (!name) { throw "Error converting string: null pointer encountered"; }
        //since char * contents are mutable, must make copy
        //on return
        char *new_name = new char[strlen(name) + 1]{0};
        strcpy(new_name, name);
        return smart_ptr<char *, false>(&new_name, false);// PTR_IS_ALLOCATED);
    }

    template<typename T, const size_t size, const bool array_allocated, typename ClassWrapper>
    typename CObjectConversionHelper<T[size], array_allocated, ClassWrapper>::ptr_t
    CObjectConversionHelper<T[size], array_allocated, ClassWrapper>::
    toCObject(PyObject &pyobj) {

        if (PtrWrapper::checkType(&pyobj)) {
            PtrWrapper *self = reinterpret_cast<PtrWrapper * >(&pyobj);
            if (self->_arraySize != size) {
                throw "Attempt to convert between arrays of differing sizes";
            }
            T *val_ = *self->template get_CObject<T *>();
            T_array *val = (T_array *) &val_;
            return smart_ptr<T[size], array_allocated>(val, PTR_IS_ALLOCATED);
        } else if (ClassWrapper::checkType(&pyobj)) {
            return smart_ptr<T_array, array_allocated>(
                    reinterpret_cast<ClassWrapper * >(&pyobj)->template get_CObject<T_array>(),
                    PTR_IS_NOT_ALLOCATED);
        } else if (NonConstArrayWrapper::checkType(&pyobj)) {
            ConstArrayWrapper *self = reinterpret_cast<ConstArrayWrapper * >(&pyobj);
            T_array *val = (T_array *) self->template get_CObject<Const_T_array>();
            return smart_ptr<T_array, array_allocated>(val, PTR_IS_NOT_ALLOCATED);
        } else if (ConstArrayWrapper::checkType(&pyobj)) {
            //cannot cast away const-ness so make copy on return:
            ConstArrayWrapper *self = reinterpret_cast<ConstArrayWrapper * >(&pyobj);
            NonConst_T_array *val = new NonConst_T_array[1];
            T_array *self_value = (T_array *) self->template get_CObject<T_array>();
            for (size_t i = 0; i < size; ++i) {
                (*val)[i] = (*self_value)[i];
            }
            return smart_ptr<T_array, array_allocated>(val, PTR_IS_ALLOCATED);
        } else if (PyList_Check(&pyobj)) {
            if (PyList_Size(&pyobj) != size) {
                throw "Inconsistent sizes in array assignment";
            }
            NonConst_T_array *val = new NonConst_T_array[1];
            for (size_t i = 0; i < size; ++i) {
                PyObject *listitem = PyList_GetItem(&pyobj, i);
                if (ClassWrapper::checkTypeDereferenced(listitem)) {
                    (*val)[i] = *(reinterpret_cast< ClassWrapper * >(listitem)->template get_CObject<T>());
                } else {
                    delete[] val;
                    throw "Invalid type in array element assignment";
                }
            }

            return smart_ptr<T_array, array_allocated>(val, PTR_IS_ALLOCATED);
        }
        throw "Conversion to C-string from non-string Python object!";

    }

    template<typename T, typename ClassWrapper>
    PyObject *ConversionHelpers::PyObjectConversionHelper<T, ClassWrapper,
            typename std::enable_if<!std::is_integral<T>::value &&
                                    !std::is_enum<T>::value &&
                                    !std::is_floating_point<T>::value>::type>::
    toPyObject( typename std::remove_reference<T>::type &var, const bool asReference, const ssize_t array_size,
                const size_t depth ) {
        ObjContainer<T_NoRef> *const ref = (asReference ? new ObjContainerPtrProxy<T_NoRef, true>(&var, false, false)
                                                        : ObjectLifecycleHelpers::Copy<T>::new_copy(var));
        PyObject *pyobj = (PyObject *) ClassWrapper::createPy(array_size, ref, !asReference, false, nullptr, depth);
        if (!pyobj || !ClassWrapper::checkType(pyobj)) {
            PyErr_Format(PyExc_TypeError, "Unable to convert C type object to Python object %s: %s",
                         pyobj?pyobj->ob_type->tp_name:"NULL OBJ", ClassWrapper::get_name().c_str());
            pyobj = nullptr;
        }
        return pyobj;
    }

    template<typename T, typename ClassWrapper>
    PyObject *ConversionHelpers::PyObjectConversionHelper<T, ClassWrapper, typename std::enable_if<
            std::is_integral<T>::value || std::is_enum<T>::value>::type>::
    toPyObject(const T &var, const bool asReference, const ssize_t array_size, const size_t depth ) {
        (void) asReference;
        return PyInt_FromLong((long) var);
    }

    template<typename T, typename ClassWrapper>
    PyObject *
    ConversionHelpers::PyObjectConversionHelper<T, ClassWrapper, typename std::enable_if<std::is_floating_point<T>::value>::type>::
    toPyObject(const T &var, const bool asReference, const ssize_t array_size, const size_t depth) {
        (void) asReference;
        return PyFloat_FromDouble(var);
    }

    template<typename ClassWrapper>
    PyObject *ConversionHelpers:: PyObjectConversionHelper<const char *, ClassWrapper, void> ::
    toPyObject(const char *const &var, const bool asReference, const ssize_t array_size, const size_t depth) {
        (void) asReference;
        if (!var) {
            throw "NULL CHAR* encountered";
        }
        return PyString_FromString(var);

    }

    template<typename ClassWrapper>
    PyObject *ConversionHelpers:: PyObjectConversionHelper<char *, ClassWrapper, void>::
    toPyObject(char *const &var, const bool asReference, const ssize_t array_size, const size_t depth) {
        (void) asReference;
        if (!var) {
            throw "NULL CHAR* encountered";
        }
        return PyString_FromString(var);

    }

    template<typename ClassWrapper>
    PyObject* ConversionHelpers::PyObjectConversionHelper<const char *const, ClassWrapper, void>::
    toPyObject(const char *const &var, const bool asReference, const ssize_t array_size,
                                    const size_t depth){

        (void) asReference;
        if (!var) {
            throw "NULL CHAR* encountered";
        }
        return PyString_FromString(var);
    };


    template<typename T, typename E>
    PyObject *toPyObject(T &var, const bool asArgument, const ssize_t array_size, const size_t depth)  {
        return ConversionHelpers::PyObjectConversionHelper<T, PythonClassWrapper<T, E>>::
           toPyObject(var, asArgument, array_size, depth);
    }

    template<typename T, typename E>
    PyObject *toPyObject(const T &var, const bool asArgument, const ssize_t array_size, const size_t depth) {
        return ConversionHelpers::PyObjectConversionHelper<const T, PythonClassWrapper<const T, E>>::toPyObject(
                var, asArgument, array_size);
    }

}

#endif
