//
// Created by jrusnak on 7/23/16.
//

#ifndef __PYLLARS_INTERNAL__OBJECT_LIFECYCLE_CPP__
#define __PYLLARS_INTERNAL__OBJECT_LIFECYCLE_CPP__

#include "pyllars_containment.hpp"
#include "pyllars_object_lifecycle.hpp"

namespace __pyllars_internal {

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
    Copy<T, typename std::enable_if<std::is_destructible<typename std::remove_reference<T>::type>::value &&
                                    std::is_assignable<typename std::remove_reference<T>::type &, typename std::remove_reference<T>::type>::value &&
                                    std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy2(const T &value) {
        return ObjectContainerAllocated<T>::new_container(value);
    }

    template<typename T>
    typename std::remove_reference<T>::type *ObjectLifecycleHelpers::
    Copy<T, typename std::enable_if<std::is_destructible<typename std::remove_reference<T>::type>::value &&
                                    std::is_assignable<typename std::remove_reference<T>::type &, typename std::remove_reference<T>::type>::value &&
                                    std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy(T_NoRef *const value) {
        return new T_NoRef(*value);
    }

    template<typename T>
    void ObjectLifecycleHelpers::
    Copy<T, typename std::enable_if<std::is_destructible<typename std::remove_reference<T>::type>::value &&
                                    std::is_assignable<typename std::remove_reference<T>::type &, typename std::remove_reference<T>::type>::value &&
                                    std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    inplace_copy(T_NoRef *const to, const Py_ssize_t index, const T_NoRef *const from) {
        to[index] = *from;
    }


    template<typename T>
    ObjectContainer<T> *
    ObjectLifecycleHelpers::
    Copy<T, typename std::enable_if<
            (!std::is_assignable<typename std::remove_reference<T>::type &, typename std::remove_reference<T>::type>::value ||
             !std::is_destructible<typename std::remove_reference<T>::type>::value) &&
            std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy2(const T_NoRef &value) {
        return ObjectContainerAllocated<T>::new_container(value);
    }

    template<typename T>
    typename std::remove_reference<T>::type *ObjectLifecycleHelpers::
    Copy<T, typename std::enable_if<
            (!std::is_assignable<typename std::remove_reference<T>::type &, typename std::remove_reference<T>::type>::value ||
             !std::is_destructible<typename std::remove_reference<T>::type>::value) &&
            std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy(T_NoRef *const value) {
        return new T_NoRef(*value);
    }

    template<typename T>
    void ObjectLifecycleHelpers::
    Copy<T, typename std::enable_if<
            (!std::is_assignable<typename std::remove_reference<T>::type &, typename std::remove_reference<T>::type>::value ||
             !std::is_destructible<typename std::remove_reference<T>::type>::value) &&
            std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    inplace_copy(T_NoRef *const to, const Py_ssize_t index, const T_NoRef *const from) {
        throw "Cannot copy over item of non-destructible type";
    }


    template<typename T, size_t size>
    ObjectContainer<T[size]> *
    ObjectLifecycleHelpers::Copy<T[size], typename std::enable_if<(size > 0) &&
        !std::is_const<T>::value &&
        std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy2(T_array &value) {
        return ObjectContainerAllocated<T_array>::new_container(value);
    }

    namespace{

            template<typename T, size_t size>
            struct FixedArrayHelper_{
                T value[size];
            };
        }

    template<typename T, size_t size>
    typename ObjectLifecycleHelpers::Copy<T[size], typename std::enable_if<(size > 0) &&
                                                                           !std::is_const<T>::value &&
                                                                           std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::T_array *
    ObjectLifecycleHelpers::Copy<T[size], typename std::enable_if<(size > 0) &&
                                                                  !std::is_const<T>::value &&
                                                                  std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy(T_array *const value) {
        typedef typename std::remove_const<T>::type T_base;
        FixedArrayHelper<T, size>* new_array = new FixedArrayHelper<T, size>();
        for (size_t i = 0; i < size; ++i) new_array->value[i] = (*value)[i];
        return &new_array->value;
    }

    template<typename T, size_t size>
    void ObjectLifecycleHelpers::Copy<T[size], typename std::enable_if<(size > 0) &&
                                                                       !std::is_const<T>::value &&
                                                                       std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    inplace_copy(T_array *const to, const Py_ssize_t index, const T_array *const from) {
        for (size_t i = 0; i < size; ++i) {
            (*to)[i] = (*from)[i];
        }
    }

    template<typename T, size_t size>
    ObjectContainer<T[size]> *
    ObjectLifecycleHelpers::Copy<T[size], typename std::enable_if<(size > 0) &&
        std::is_const<T>::value &&
        std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy2(const T_array &value) {
        throw "Attempt to copy const object";
    }

    template<typename T, size_t size>
    typename ObjectLifecycleHelpers::Copy<T[size], typename std::enable_if<(size > 0) &&
                                                                           std::is_const<T>::value &&
                                                                           std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::T_array
    *ObjectLifecycleHelpers::Copy<T[size], typename std::enable_if<(size > 0) &&
                                                                   std::is_const<T>::value &&
                                                                   std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy(T_array *const value) {
        throw "Attempt to copy const object";
    }

    template<typename T, size_t size>
    void ObjectLifecycleHelpers::Copy<T[size], typename std::enable_if<(size > 0) &&
                                                                       std::is_const<T>::value &&
                                                                       std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    inplace_copy(T_array *const to, const Py_ssize_t index, const T_array *const from) {
        throw "Attempt to copy const object";
    }


    template<typename T>
    ObjectContainer<T> *
    ObjectLifecycleHelpers::
    Copy<T, typename std::enable_if<!std::is_void<T>::value &&
                                    (!std::is_array<T>::value || ArraySize<T>::size <= 0) &&
                                    !std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy2(const T_NoRef &value) {
        (void) value;
        throw "Attempt to copy non-copy-constructible object";
    }

    template<typename T>
    typename std::remove_reference<T>::type *
    ObjectLifecycleHelpers::
    Copy<T, typename std::enable_if<!std::is_void<T>::value &&
        (!std::is_array<T>::value || ArraySize<T>::size <= 0) &&
        !std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy(T_NoRef *const value) {
        (void) value;
        throw "Attempt to copy non-copy-constructible object";
    }


    template<typename T>
    void
    ObjectLifecycleHelpers::
    Copy<T, typename std::enable_if<!std::is_void<T>::value &&
                                    (!std::is_array<T>::value || ArraySize<T>::size <= 0) &&
                                    !std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    inplace_copy(T_NoRef *const to, const Py_ssize_t index, const T_NoRef *const from) {
        throw "Attempt to copy non-copy-constructible object";
    }


    template<typename T>
    PyObject *
    ObjectLifecycleHelpers::
    ObjectContent<T,
            typename std::enable_if<
                    !std::is_void<T>::value && !std::is_pointer<T>::value && !std::is_array<T>::value>::type>::
    getObjectAt(T const from, const size_t index, const ssize_t elements_array_size,
                const bool asArgument) {
        PyErr_SetString(PyExc_TypeError, "Attempt to get element from non-array object");
        return nullptr;
    }

    template<typename T>
    void ObjectLifecycleHelpers::
    ObjectContent<T,
            typename std::enable_if<
                    !std::is_void<T>::value && !std::is_pointer<T>::value && !std::is_array<T>::value>::type>::
    set(const size_t index, T *const to, const T *const from) {
        if (ptr_depth<T>::value > 1) {
            Assign<T *, T *>::assign(((T **) to)[index], *((T **) from));
        } else {
            Assign<T, T>::assign(to[index], *from);
        }
    }

    template<typename T>
    T *ObjectLifecycleHelpers::
    ObjectContent<T,
            typename std::enable_if<
                    !std::is_void<T>::value && !std::is_pointer<T>::value && !std::is_array<T>::value>::type>::
    getObjectPtr(ClassWrapper *const self) {
        return (T *) self->get_CObject();
    }


    template<typename T>
    PyObject *ObjectLifecycleHelpers::
    ObjectContent<T,
            typename std::enable_if<!std::is_function<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_void<typename std::remove_pointer<T>::type>::value &&
                                    is_complete<typename std::remove_pointer<T>::type>::value &&
                                    (std::is_array<T>::value || std::is_pointer<T>::value)>::type>::
    getObjectAt(T from, const size_t index, const ssize_t elements_array_size,
                const bool asArgument) {
        //TODO add reference to owning element to this object to not have go out of scope
        //until parent does!!!!
        return toPyObject<T_base>(Array<T>::at(from, index), true, elements_array_size);
    }

    template<typename T>
    void ObjectLifecycleHelpers::
    ObjectContent<T,
            typename std::enable_if<!std::is_function<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_void<typename std::remove_pointer<T>::type>::value &&
                                    is_complete<typename std::remove_pointer<T>::type>::value &&
                                    (std::is_array<T>::value || std::is_pointer<T>::value)>::type>::
    set(const size_t index, T &to, T const from) {
        if (ptr_depth<T>::value == 1) {
            Assign<T, T>::assign(Array<T>::at(to, index), *from);
        } else {
            typedef typename std::remove_reference<T>::type T_NoRef;
            Assign<T_NoRef, T_NoRef>::assign(Array<T_NoRef *>::at((T_NoRef *) to, index), *((T_NoRef *) from));
        }
    }

    template<typename T>
    T *ObjectLifecycleHelpers::
    ObjectContent<T,
            typename std::enable_if<!std::is_function<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_void<typename std::remove_pointer<T>::type>::value &&
                                    is_complete<typename std::remove_pointer<T>::type>::value &&
                                    (std::is_array<T>::value || std::is_pointer<T>::value)>::type>::
    getObjectPtr(PtrWrapper *const self) {
        return (T *) &self->_CObject;
    }


    template<typename T>
    PyObject *ObjectLifecycleHelpers::
    ObjectContent<T,
            typename std::enable_if<std::is_function<typename std::remove_pointer<T>::type>::value>::type>::
    getObjectAt(T const from, const size_t index, const ssize_t elements_array_size,
                const bool asArgument) {
        PyErr_SetString(PyExc_TypeError, "Attempt to get element from non-array object");
        return nullptr;
    }

    template<typename T>
    void ObjectLifecycleHelpers::
    ObjectContent<T,
            typename std::enable_if<std::is_function<typename std::remove_pointer<T>::type>::value>::type>::
    set(const size_t index, T const to, T const from) {
        throw "Attempt to index function pointer";
    }

    template<typename T>
    T *ObjectLifecycleHelpers::
    ObjectContent<T,
            typename std::enable_if<std::is_function<typename std::remove_pointer<T>::type>::value>::type>::
    getObjectPtr(PtrWrapper *const self) {
        return (T *) &self->_CObject;
    }


    template<typename T>
    PyObject *ObjectLifecycleHelpers::
    ObjectContent<T,
            typename std::enable_if<!is_complete<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_function<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_void<typename std::remove_pointer<T>::type>::value>::type>::
    getObjectAt(T const from, const size_t index, const ssize_t elements_array_size,
                const bool asArgument) {
        //TODO add reference to owning element to this object to not have go out of scope
        //until parent does!!!!
        return toPyObject<T_base>(Array<T>::at(from, index), asArgument, elements_array_size);
    }

    template<typename T>
    void ObjectLifecycleHelpers::
    ObjectContent<T,
            typename std::enable_if<!is_complete<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_function<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_void<typename std::remove_pointer<T>::type>::value>::type>::
    set(const size_t index, T const to, T const from) {
        throw "Attempt to index function pointer";
    }

    template<typename T>
    T *ObjectLifecycleHelpers::
    ObjectContent<T,
            typename std::enable_if<!is_complete<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_function<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_void<typename std::remove_pointer<T>::type>::value>::type>::
    getObjectPtr(PtrWrapper *const self) {
        return (T *) &self->_CObject;
    }

    template<typename T>
    void ObjectLifecycleHelpers::ObjectContent<T *, typename std::enable_if<std::is_void<T>::value>::type>::
    set(const size_t index, T **const to, T **const from) {
        to[index] = *from;
    }

    template<typename T>
    PyObject *
    ObjectLifecycleHelpers::ObjectContent<T *, typename std::enable_if<std::is_void<T>::value>::type>::
    getObjectAt(T *const from, const size_t index, const ssize_t elements_array_size,
                const bool asArgument) {
        (void) from;
        (void) index;
        (void) elements_array_size;
        PyErr_SetString(PyExc_TypeError, "Attempt to get element from non-array object");
        return nullptr;
    }

    template<typename T>
    T **ObjectLifecycleHelpers::ObjectContent<T *, typename std::enable_if<std::is_void<T>::value>::type>::
    getObjectPtr(PtrWrapper *const self) {
        return (T **) &self->_CObject;
    }


    template<typename T>
    void ObjectLifecycleHelpers::ObjectContent<T, typename std::enable_if<std::is_void<T>::value> >::
    set(const size_t index, void *const to, void *const from) {
        throw "Attempt to access element from object  of type void";
    }

    template<typename T>
    PyObject *ObjectLifecycleHelpers::ObjectContent<T, typename std::enable_if<std::is_void<T>::value> >::
    getObjectAt(void *const from, const size_t index, const ssize_t elements_array_size,
                const bool asArgument) {
        (void) from;
        (void) index;
        (void) elements_array_size;
        PyErr_SetString(PyExc_TypeError, "Attempt to get element from non-array object");
        return nullptr;
    }

    template<typename T>
    void *ObjectLifecycleHelpers::ObjectContent<T, typename std::enable_if<std::is_void<T>::value> >::
    getObjectPtr(PtrWrapper *const self) {
        return (void *) &self->_CObject;
    }


    template<typename T>
    void
    ObjectLifecycleHelpers::ObjectContent<T *const, typename std::enable_if<std::is_void<T>::value>::type>::
    set(const size_t index, T *const *const to, T *const *const from) {
        throw "Attempt to assign to const value";
    }

    template<typename T>
    PyObject *
    ObjectLifecycleHelpers::ObjectContent<T *const, typename std::enable_if<std::is_void<T>::value>::type>::
    getObjectAt(T *const from, const size_t index, const ssize_t elements_array_size,
                const bool asArgument) {
        (void) from;
        (void) index;
        (void) elements_array_size;
        PyErr_SetString(PyExc_TypeError, "Attempt to get element from non-array object");
        return nullptr;
    }

    template<typename T>
    T *const *
    ObjectLifecycleHelpers::ObjectContent<T *const, typename std::enable_if<std::is_void<T>::value>::type>::
    getObjectPtr(PtrWrapper *const self) {
        return (T *const *) &self->_CObject;
    }
}


#endif
