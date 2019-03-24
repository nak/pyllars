//
// Created by jrusnak on 7/23/16.
//

#ifndef __PYLLARS_INTERNAL__OBJECT_LIFECYCLE_CPP__
#define __PYLLARS_INTERNAL__OBJECT_LIFECYCLE_CPP__

#include "pyllars_containment.hpp"
#include "pyllars_object_lifecycle.hpp"

namespace __pyllars_internal {


    namespace{

        template<typename T, size_t size>
        struct FixedArrayHelper_{
            T value[size];
        };
    }

    template<typename T>
    typename std::remove_pointer<typename extent_as_pointer<T>::type>::type &
    ObjectLifecycleHelpers::Array<T>::
    at(T array, size_t index) {
        if constexpr((std::is_array<T>::value || std::is_pointer<T>::value)) {
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
            if constexpr (std::is_void<T_base>::value ){
                throw "Cannot index into void-pointer/array";
            }
            if (!array) {
                throw "Cannot dereference null object!";
            }
            return array[index];
        }
        throw "Cannot get element of non-array or array-of/pointer-to  incomplete or void type";
    }


    template<typename T>
    ObjectContainer<T> *ObjectLifecycleHelpers::
    Copy<T>::
    new_copy2(const T &value) {
        typedef typename std::remove_reference<T>::type T_NoRef;
        if constexpr ((std::is_destructible<T_NoRef>::value && std::is_assignable<T_NoRef&, T_NoRef >::value && std::is_copy_constructible<T_NoRef>::value) ||
                ((!std::is_assignable<T_NoRef  &, T_NoRef >::value || !std::is_destructible<T_NoRef >::value) && std::is_copy_constructible<T_NoRef >::value) ||
                (std::is_array<T>::value && (ArraySize<T>::size > 0) && !std::is_const<T>::value && std::is_copy_constructible<T_NoRef >::value)){
            return new ObjectContainerConstructed<T, const T&>(value);
        } else {
            throw "Attempt to copy non-copy-constructible object";
        }
    }

    template<typename T>
    typename std::remove_reference<T>::type *ObjectLifecycleHelpers::
    Copy<T>::
    new_copy(T_NoRef *const value) {
        typedef typename std::remove_reference<T>::type T_NoRef;
        if constexpr ((std::is_destructible<T_NoRef>::value && std::is_assignable<T_NoRef &, T_NoRef>::value && std::is_copy_constructible<T_NoRef>::value) ||
                ( (!std::is_assignable<T_NoRef &, T_NoRef>::value || !std::is_destructible<T_NoRef>::value) && std::is_copy_constructible<T_NoRef>::value)){
            return new T_NoRef(*value);
        } else if constexpr (!std::is_const<T>::value && std::is_array<T>::value && ArraySize<T>::size > 0){
            auto new_array = new (*value) FixedArrayHelper<T>;
            return reinterpret_cast<T_NoRef*>(new_array);
        } else {
            throw "Attempt to copy non-copy-constructible object";
        }
    }




    template<typename T>
    void ObjectLifecycleHelpers::
    Copy<T>::
    inplace_copy(T_NoRef *const to, const Py_ssize_t index, const T_NoRef *const from) {
        typedef typename std::remove_reference<T>::type T_NoRef;
        static constexpr ssize_t size = ArraySize<T>::size;
        if constexpr((std::is_destructible<T_NoRef>::value &&
                      std::is_assignable<T_NoRef &, T_NoRef>::value &&
                      std::is_copy_constructible<T_NoRef>::value)) {
            to[index] = *from;
        } else if constexpr ((size > 0) &&
                             !std::is_const<T>::value &&
                             std::is_copy_constructible<T_NoRef>::value){
            for (size_t i = 0; i < size; ++i) {
                (*to)[i] = (*from)[i];
            }
        } /*else if constexpr(!std::is_void<T>::value &&
                            (!std::is_array<T>::value || ArraySize<T>::size <= 0) &&
                            !std::is_copy_constructible<typename std::remove_reference<T>::type>::value){

        }*/ else {
            throw "Attempt to copy non-copy-constructible object";
        }
    }

    ///////////////////////////////

    template<typename T>
    PyObject *
    ObjectLifecycleHelpers::
    ObjectContent<T>::
    getObjectAt(T const from, const size_t index, const ssize_t elements_array_size,
                const bool asArgument) {
        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
        if constexpr ( !std::is_void<T>::value && !std::is_pointer<T>::value && !std::is_array<T>::value) {
            PyErr_SetString(PyExc_TypeError, "Attempt to get element from non-array object");
            return nullptr;
        } else if constexpr (!std::is_function<T_base>::value &&!std::is_void<T_base>::value && is_complete<T_base>::value &&
                             (std::is_array<T>::value || std::is_pointer<T>::value)){
            //TODO add reference to owning element to this object to not have go out of scope
            //until parent does!!!!
            return toPyObject<T_base>(Array<T>::at(from, index), true, elements_array_size);
        }  else if constexpr(!is_complete<T_base>::value && !std::is_function<T_base>::value && !std::is_void<T_base>::value){
            //TODO add reference to owning element to this object to not have go out of scope
            //until parent does!!!!
            return toPyObject<T_base>(Array<T>::at(from, index), asArgument, elements_array_size);
        } else if constexpr(!std::is_array<T>::value ){
            PyErr_SetString(PyExc_TypeError, "Attempt to get element from non-array object");
            return nullptr;
        }
    }



    template<typename T>
    void ObjectLifecycleHelpers::
    ObjectContent<T>::
    set(const size_t index, T *const to, const T *const from) {
        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
        if constexpr ( !std::is_void<T>::value && !std::is_pointer<T>::value && !std::is_array<T>::value) {
            if (ptr_depth<T>::value > 1) {
                Assignment<T *, T *>::assign(((T **) to)[index], *((T **) from));
            } else {
                Assignment<T, T>::assign(to[index], *from);
            }
        } else if constexpr (!std::is_function<typename std::remove_pointer<T>::type>::value &&
                             !std::is_void<typename std::remove_pointer<T>::type>::value &&
                             is_complete<typename std::remove_pointer<T>::type>::value &&
                             (std::is_array<T>::value || std::is_pointer<T>::value)){
            if (ptr_depth<T>::value == 1) {
                Assignment<T, T>::assign(Array<T>::at(to, index), *from);
            } else {
                typedef typename std::remove_reference<T>::type T_NoRef;
                Assignment<T_NoRef, T_NoRef>::assign(Array<T_NoRef *>::at((T_NoRef *) to, index), *((T_NoRef *) from));
            }
        } else if constexpr((std::is_array<T>::value || std::is_pointer<T>::value) && std::is_void<T_base>::value){
            to[index] = *from;
        } else {
            throw "Attempt to index non-indexable C type";
        }

    }


    template<typename T>
    T *ObjectLifecycleHelpers::
    ObjectContent<T>::
    getObjectPtr(PythonClassWrapper<T> *const self) {
        return (T *) &self->_CObject;
    }


}


#endif
