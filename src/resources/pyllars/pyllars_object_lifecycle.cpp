//
// Created by jrusnak on 7/23/16.
//

#ifndef __PYLLARS_INTERNAL__OBJECT_LIFECYCLE_CPP__
#define __PYLLARS_INTERNAL__OBJECT_LIFECYCLE_CPP__

#include "pyllars_object_lifecycle.hpp"

namespace __pyllars_internal{

    template<typename T>
    typename std::remove_pointer<typename extent_as_pointer<T>::type>::type  &
    ObjectLifecycleHelpers::Array<T, typename std::enable_if<is_complete<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value&&
                                                             !std::is_void <typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type>::
    at(T array, size_t index) {
            return array[index];
    }


    template<typename T>
    typename std::remove_pointer<typename extent_as_pointer<T>::type>::type  &
    ObjectLifecycleHelpers::Array<T, typename std::enable_if<!std::is_void<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value &&
                                                             !is_complete<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type> ::
    at(T array, size_t index) {
            throw "Cannot dereference incomplete type";
    }


    template<typename T>
    ObjContainer <typename std::remove_reference<T>::type> *ObjectLifecycleHelpers::
      Copy<T, typename std::enable_if<std::is_destructible<typename std::remove_reference<T>::type>::value &&
                                      std::is_assignable<typename std::remove_reference<T>::type, typename std::remove_reference<T>::type>::value &&
                                      std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> ::
    new_copy(const T &value) {
        return new ObjContainerProxy<T_NoRef, const T &>(value);
    }

    template<typename T>
    typename std::remove_reference<T>::type *ObjectLifecycleHelpers::
    Copy<T, typename std::enable_if<std::is_destructible<typename std::remove_reference<T>::type>::value &&
                                    std::is_assignable<typename std::remove_reference<T>::type, typename std::remove_reference<T>::type>::value &&
                                    std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> ::
    new_copy(T_NoRef *const value) {
        return new T_NoRef(*value);
    }

    template<typename T>
    void ObjectLifecycleHelpers::
    Copy<T, typename std::enable_if<std::is_destructible<typename std::remove_reference<T>::type>::value &&
                                    std::is_assignable<typename std::remove_reference<T>::type, typename std::remove_reference<T>::type>::value &&
                                    std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> ::
    inplace_copy(T_NoRef *const to, const Py_ssize_t index, const T_NoRef *const from,
                             const bool in_place) {
        if (in_place) {
            to[index].~T();
            new(&to[index]) T_NoRef(*from);
        } else {
            to[index] = *from;
        }

    }


    template<typename T>
    ObjContainer <typename std::remove_reference<T>::type> *ObjectLifecycleHelpers::
        Copy<T, typename std::enable_if<
            (!std::is_assignable<typename std::remove_reference<T>::type, typename std::remove_reference<T>::type>::value ||
             !std::is_destructible<typename std::remove_reference<T>::type>::value) &&
             std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy(const T_NoRef &value) {
        return new ObjContainerProxy<T_NoRef, const T_NoRef &>(value);
    }

    template<typename T>
    ObjContainer <typename std::remove_reference<T>::type> *ObjectLifecycleHelpers::
      Copy<T, typename std::enable_if<
            (!std::is_assignable<typename std::remove_reference<T>::type, typename std::remove_reference<T>::type>::value ||
             !std::is_destructible<typename std::remove_reference<T>::type>::value) &&
              std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy(T_NoRef *const value) {
        return new ObjContainerProxy<T_NoRef, T_NoRef &>(*value);
    }

    template<typename T>
    void ObjectLifecycleHelpers::
      Copy<T, typename std::enable_if<
            (!std::is_assignable<typename std::remove_reference<T>::type, typename std::remove_reference<T>::type>::value ||
                    !std::is_destructible<typename std::remove_reference<T>::type>::value) &&
                    std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> ::
    inplace_copy(T_NoRef *const to, const Py_ssize_t index, const T_NoRef *const from,
                             const bool in_place) {
        throw "Cannot copy over item of non-destructible type";
    }


    template<typename T, size_t size>
    ObjContainer <T[size]> *ObjectLifecycleHelpers::Copy<T[size], typename std::enable_if<(size > 0) &&
                                                                                          !std::is_const<T>::value &&
                                                                                          std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy(const T_array &value) {
      typedef typename std::remove_const<T>::type T_nonconst_array[size];
      T_nonconst_array *new_value = new T_nonconst_array[1];
      for(size_t i = 0; i < size; ++i) new_value[0][i] = value[i];
      return new ObjContainerPtrProxy<T_array, true>((T_array*)(new_value), true);
    }

    template<typename T, size_t size>
    ObjContainer <T[size]> *ObjectLifecycleHelpers::Copy<T[size], typename std::enable_if<(size > 0) &&
                                                                                          !std::is_const<T>::value &&
                                                                                          std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy(T_array *const value) {
      typedef typename std::remove_const<T>::type T_nonconst_array[size];
      T_nonconst_array *new_value = new T_nonconst_array[1];
      for(size_t i = 0; i < size; ++i) new_value[0][i] = (*value)[i];
      return new ObjContainerPtrProxy<T_array, true>((T_array*)(new_value), true);
    }

    template<typename T, size_t size>
    void ObjectLifecycleHelpers::Copy<T[size], typename std::enable_if<(size > 0) &&
                                                                       !std::is_const<T>::value &&
                                                                       std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    inplace_copy(T_array *const to, const Py_ssize_t index, const T_array *const from,
                             const bool in_place) {
        for (size_t i = 0; i < size; ++i) {
            (*to)[i] = (*from)[i];
        }
    }

    template<typename T, size_t size>
    ObjContainer <T[size]> *ObjectLifecycleHelpers::Copy<T[size], typename std::enable_if<(size > 0) &&
                                                                                          std::is_const<T>::value &&
                                                                                          std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
     new_copy(const T_array &value) {
               throw "Attempt to copy const object";
    }

    template<typename T, size_t size>
    ObjContainer <T[size]> *ObjectLifecycleHelpers::Copy<T[size], typename std::enable_if<(size > 0) &&
                                                                                          std::is_const<T>::value &&
                                                                                          std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy(T_array *const value) {
               throw "Attempt to copy const object";
    }

    template<typename T, size_t size>
    void ObjectLifecycleHelpers::Copy<T[size], typename std::enable_if<(size > 0) &&
                                                                       std::is_const<T>::value &&
                                                                       std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    inplace_copy(T_array *const to, const Py_ssize_t index, const T_array *const from,
                             const bool in_place) {
         throw "Attempt to copy const object";
    }


    template<typename T>
    ObjContainer <typename std::remove_reference<T>::type> *ObjectLifecycleHelpers::
            Copy<T, typename std::enable_if<!std::is_void<T>::value &&
                                            (!std::is_array<T>::value || ArraySize<T>::size <= 0) &&
                                            !std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy(const T_NoRef &value) {
        (void) value;
        throw "Attempt to copy non-copy-constructible object";
    }

    template<typename T>
    ObjContainer <typename std::remove_reference<T>::type> *ObjectLifecycleHelpers::
            Copy<T, typename std::enable_if<!std::is_void<T>::value &&
                                            (!std::is_array<T>::value || ArraySize<T>::size <= 0) &&
                                            !std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    new_copy(T_NoRef *const value) {
        (void) value;
        throw "Attempt to copy non-copy-constructible object";
    }


    template<typename T>
    void ObjectLifecycleHelpers::
            Copy<T, typename std::enable_if<!std::is_void<T>::value &&
                                            (!std::is_array<T>::value || ArraySize<T>::size <= 0) &&
                                            !std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type>::
    inplace_copy(T_NoRef *const to, const Py_ssize_t index, const T_NoRef *const from,
                             const bool in_place) {
        throw "Attempt to copy non-copy-constructible object";
    }



    template<typename T, typename ClassWrapper>
    PyObject *ObjectLifecycleHelpers::
            ObjectContent<T, ClassWrapper,
            typename std::enable_if<
                    !std::is_void<T>::value && !std::is_pointer<T>::value && !std::is_array<T>::value>::type>::
    getObjectAt(T const from, const size_t index, const ssize_t elements_array_size,
                                 const size_t depth, const bool asArgument) {
        PyErr_SetString(PyExc_TypeError, "Attempt to get element from non-array object");
        return nullptr;
    }

    template<typename T, typename ClassWrapper>
    void ObjectLifecycleHelpers::
            ObjectContent<T, ClassWrapper,
            typename std::enable_if<
                    !std::is_void<T>::value && !std::is_pointer<T>::value && !std::is_array<T>::value>::type>::
    set(const size_t index, T *const to, const T *const from, const size_t depth) {
        if (depth > 1) {
            Assign<T *, T *>::assign(((T **) to)[index], *((T **) from));
        } else {
            Assign<T, T>::assign(to[index], *from);
        }
    }

    template<typename T, typename ClassWrapper>
    T *ObjectLifecycleHelpers::
            ObjectContent<T, ClassWrapper,
            typename std::enable_if<
                    !std::is_void<T>::value && !std::is_pointer<T>::value && !std::is_array<T>::value>::type>::
    getObjectPtr(ClassWrapper *const self) {
        return (T *) self->get_CObject();
    }


    template<typename T, typename PtrWrapper>
    PyObject *ObjectLifecycleHelpers::
            ObjectContent<T, PtrWrapper,
            typename std::enable_if<!std::is_function<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_void<typename std::remove_pointer<T>::type>::value &&
                                    is_complete<typename std::remove_pointer<T>::type>::value &&
                                    (std::is_array<T>::value || std::is_pointer<T>::value)>::type>::
    getObjectAt(T from, const size_t index, const ssize_t elements_array_size,
                          const size_t depth,
                          const bool asArgument) {
        //TODO add reference to owning element to this object to not have go out of scope
        //until parent does!!!!
        if (depth == 1) {
            return toPyObject<T_base>(Array<T>::at(from, index), true, elements_array_size);
        } else {
            T *from_real = (T *) from;
            return toPyObject<T>(from_real[index], asArgument, elements_array_size, depth - 1);
        }
    }

    template<typename T, typename PtrWrapper>
    void ObjectLifecycleHelpers::
            ObjectContent<T, PtrWrapper,
            typename std::enable_if<!std::is_function<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_void<typename std::remove_pointer<T>::type>::value &&
                                    is_complete<typename std::remove_pointer<T>::type>::value &&
                                    (std::is_array<T>::value || std::is_pointer<T>::value)>::type>::
    set(const size_t index, T &to, T const from, const size_t depth) {
        if (depth == 1) {
            Assign<T, T>::assign(Array<T>::at(to, index), *from);
        } else {
            typedef typename std::remove_reference<T>::type T_NoRef;
            Assign<T_NoRef, T_NoRef>::assign(Array<T_NoRef *>::at((T_NoRef *) to, index), *((T_NoRef *) from));
        }
    }

    template<typename T, typename PtrWrapper>
    T *ObjectLifecycleHelpers::
            ObjectContent<T, PtrWrapper,
            typename std::enable_if<!std::is_function<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_void<typename std::remove_pointer<T>::type>::value &&
                                    is_complete<typename std::remove_pointer<T>::type>::value &&
                                    (std::is_array<T>::value || std::is_pointer<T>::value)>::type>::
    getObjectPtr(PtrWrapper *const self) {
        return (T *) &self->_CObject;
    }


    template<typename T, typename PtrWrapper>
    PyObject *ObjectLifecycleHelpers::
      ObjectContent<T, PtrWrapper,
            typename std::enable_if<std::is_function<typename std::remove_pointer<T>::type>::value>::type> ::
    getObjectAt(T const from, const size_t index, const ssize_t elements_array_size,
                                 const size_t depth, const bool asArgument) {
        PyErr_SetString(PyExc_TypeError, "Attempt to get element from non-array object");
        return nullptr;
    }

    template<typename T, typename PtrWrapper>
    void ObjectLifecycleHelpers::
      ObjectContent<T, PtrWrapper,
            typename std::enable_if<std::is_function<typename std::remove_pointer<T>::type>::value>::type> ::
    set(const size_t index, T const to, T const from, const size_t depth) {
        throw "Attempt to index function pointer";
    }

    template<typename T, typename PtrWrapper>
    T *ObjectLifecycleHelpers::
      ObjectContent<T, PtrWrapper,
            typename std::enable_if<std::is_function<typename std::remove_pointer<T>::type>::value>::type> ::
    getObjectPtr(PtrWrapper *const self) {
        return (T *) &self->_CObject;
    }


    template<typename T, typename PtrWrapper>
    PyObject *ObjectLifecycleHelpers::
      ObjectContent<T, PtrWrapper,
            typename std::enable_if<!is_complete<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_function<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_void<typename std::remove_pointer<T>::type>::value>::type>::
    getObjectAt(T const from, const size_t index, const ssize_t elements_array_size,
                                 const size_t depth, const bool asArgument ) {
        PyErr_SetString(PyExc_TypeError, "Attempt to get element from non-array object");
        return nullptr;
    }

    template<typename T, typename PtrWrapper>
    void ObjectLifecycleHelpers::
      ObjectContent<T, PtrWrapper,
            typename std::enable_if<!is_complete<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_function<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_void<typename std::remove_pointer<T>::type>::value>::type>::
    set(const size_t index, T const to, T const from, const size_t depth) {
        throw "Attempt to index function pointer";
    }

    template<typename T, typename PtrWrapper>
    T *ObjectLifecycleHelpers::
      ObjectContent<T, PtrWrapper,
            typename std::enable_if<!is_complete<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_function<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_void<typename std::remove_pointer<T>::type>::value>::type>::
    getObjectPtr(PtrWrapper *const self) {
        return (T *) &self->_CObject;
    }



    template<typename PtrWrapper>
    void ObjectLifecycleHelpers::BasicDeallocation<PtrWrapper>::_free(PtrWrapper *self) {
            self->delete_raw_storage();
            if (self->_allocated) self->_CObject = nullptr;
            self->_allocated = false;
    }


    /**
     * If core type is not and array and is destructible, call in-place destructor
     * if needed in addition to basic clean-up
     **/
    template<typename T, typename PtrWrapper>
    void ObjectLifecycleHelpers::Deallocation<T, PtrWrapper, typename std::enable_if<
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            std::is_class<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value &&
            std::is_destructible<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value
           >::type>::
    _free(PtrWrapper *self) {
            if (self->_raw_storage) {
                if (self->_depth == 1 && self->_max >= 0) {
                    for (Py_ssize_t j = 0; j <= self->_max; ++j) {
                        ((*self->_CObject)[j]).~T_base();
                    }
                }
                char *raw_storage = (char *) self->_raw_storage;
                delete[] raw_storage;
            } else if (self->_allocated) {
                delete self->_CObject;
            }
            self->_raw_storage = nullptr;
            if (self->_allocated) self->_CObject = nullptr;
            self->_allocated = false;
    }

    /**
     * If core type is not and array and is destructible, call in-place destructor
     * if needed in addition to basic clean-up
     **/
    template<typename T, typename PtrWrapper>
    void ObjectLifecycleHelpers::Deallocation<T, PtrWrapper, typename std::enable_if<
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            !std::is_class<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value &&
            std::is_destructible<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value
          >::type>::
    _free(PtrWrapper *self) {
            if (self->_raw_storage) {
                // No class type here, so no need to call inline destructor
                delete[] self->_raw_storage;
            } else if (self->_allocated) {
                if (std::is_array<T>::value) {
                    delete[] self->_CObject;
                } else {
                    delete self->_CObject;
                }
            }
            self->_raw_storage = nullptr;
            if (self->_allocated) self->_CObject = nullptr;
            self->_allocated = false;
    }


    /**
     * If core type is not and array and is destructible, call in-place destructor
     * if needed in addition to basic clean-up
     **/
    template<typename T, typename PtrWrapper>
    void ObjectLifecycleHelpers::Deallocation<T, PtrWrapper, typename std::enable_if<
            !std::is_destructible<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type>::
    _free(PtrWrapper *self) {
            if (self->_raw_storage) {
                // //Not destructible, so no delete call
            } else if (self->_allocated) {
                //Not destructible, so no delete call
            }
            self->_raw_storage = nullptr;
            if (self->_allocated) self->_CObject = nullptr;
            self->_allocated = false;
    }



    template<typename T>
    ObjectLifecycleHelpers::BasicAlloc<T>::ConstructorContainer::
    ConstructorContainer(const char *const kwlist[],
                         constructor c) : _kwlist(kwlist),
                                          _constructor(c) {
    }

    template<typename T>
    bool ObjectLifecycleHelpers::BasicAlloc<T>::ConstructorContainer::
    operator()(PyObject *args, PyObject *kwds, ObjContainer <T_NoRef> *&cobj,
                    const bool in_place) const {
        return _constructor(_kwlist, args, kwds, cobj, in_place);
    }


    template<typename T>
    PyObject *ObjectLifecycleHelpers::BasicAlloc<T>::
    allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                               typename BasicAlloc<T>::constructor_list const &constructors) {
        (void) cls;
        //Check if argument is list of tuples, and if so construct
        //an array of objects to store "behind the pointer"
        Py_ssize_t size = 1;
        if ((!kwds || PyDict_Size(kwds) == 0) && args && PyTuple_Size(args) == 1 && PyList_Check(args)) {
            PyObject *list = PyTuple_GetItem(args, 0);
            size = PyList_Size(list);
            char *raw_storage;
            T_NoRef *values;
            if (size > 1) {
                raw_storage = (char *) operator new[](size * sizeof(T));
                memset(raw_storage, 0, size * sizeof(T));
                values = reinterpret_cast<T_NoRef *>(raw_storage);
            } else {
                raw_storage = nullptr;
                values = nullptr;//constructor call will allocate and construct this if null
            }
            for (Py_ssize_t i = 0; i < PyTuple_Size(args); ++i) {
                PyObject *constructor_pyargs = PyList_GetItem(list, i);
                if (!PyTuple_Check(constructor_pyargs) && !PyDict_Check(constructor_pyargs)) {
                    PyErr_SetString(PyExc_TypeError, "Invalid element in list argument, expected tuple");
                    if (raw_storage) {
                        for (Py_ssize_t j = 0; j < i; ++j) {
                            values[j].~T();
                        }
                    }
                    delete[] raw_storage;
                    PyErr_SetString(PyExc_RuntimeError,
                                    "NOTE: Freed memory, but no visible destructor available to call.");
                    return nullptr;
                } else if (PyTuple_Check(constructor_pyargs)) {
                    for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                        try {
                            static PyObject *emptylist = PyDict_New();
                            ObjContainer<T_NoRef> *o = new ObjContainerPtrProxy<T_NoRef, false, true>(
                                    &values[i], size, true);
                            if ((*it)(args, emptylist, o, true)) {
                                delete o;
                                break;
                            }
                            delete o;
                        } catch (...) {
                        }
                        PyErr_Clear();
                    }
                } else if (PyDict_Check(constructor_pyargs)) {

                    for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                        try {
                            static PyObject *emptyargs = PyTuple_New(0);
                            ObjContainer<T_NoRef> *o = values ? new ObjContainerPtrProxy<T_NoRef, false, true>(
                                    &values[i], 1, true) : nullptr;
                            if ((*it)(emptyargs, constructor_pyargs, o, false)) {
                                delete o;
                                break;
                            }
                            delete o;
                        } catch (...) {
                        }
                        PyErr_Clear();
                    }
                }
                if (!values) {
                    PyErr_SetString(PyExc_RuntimeError,
                                    "Invalid constructor arguments on allocation.  Objects not destructible by design! ");
                    if (raw_storage) {
                        for (Py_ssize_t j = 0; j < i; ++j) {
                            values[j].~T();
                        }
                        delete[] raw_storage;
                    }
                    return nullptr;
                }
            }
            PtrWrapper *obj =
                    (PtrWrapper *) PtrWrapper::createPy2(size, &values, !raw_storage, false);
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
            obj->_raw_storage = (T_base *) raw_storage;
            return (PyObject *) obj;
        }
        //otherwise, just have regular list of constructor arguments
        //for single object allocation
        char *raw = new char[sizeof(T_NoRef)];
        T_NoRef *ccobj = (T_NoRef *) raw;
        ObjContainerPtrProxy<T_NoRef, false, true> cobj =
                ObjContainerPtrProxy<T_NoRef, false, true>(ccobj, 1, true);
        ObjContainer<T_NoRef> *oo = &cobj;
        bool found = false;
        for (auto it = constructors.begin(); it != constructors.end(); ++it) {
            try {
                if ((*it)(args, kwds, oo, true)) {
                    found = true;
                    break;
                }
            } catch (...) {
            }
            PyErr_Clear();
        }
        if (!found) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
            delete[] raw;
            return nullptr;
        }
        PtrWrapper *obj = PtrWrapper::createPy2(size, &ccobj, true, true);
        return (PyObject *) obj;
    }



    template<typename T,
            typename PtrWrapper,
            typename ClassWrapper>
    void ObjectLifecycleHelpers::Alloc<T, PtrWrapper, ClassWrapper,
            typename std::enable_if<!is_function_ptr<T>::value && !std::is_reference<T>::value &&
                                    std::is_constructible<T>::value &&
                                    std::is_destructible<T>::value>::type>::
    deallocREMOVE(ObjContainer <T_NoRef> *ptr, const bool inPlace, const size_t size) {
        if (inPlace && ptr) {
            for (size_t i = 0; i < size; ++i) {
                ptr->ptr()[i].~T();
            }
        } else {
            delete ptr;
        }
    }


    template<typename T,
            typename PtrWrapper,
            typename ClassWrapper>
    PyObject *ObjectLifecycleHelpers::Alloc<T, PtrWrapper, ClassWrapper,
            typename std::enable_if<!is_function_ptr<T>::value && !std::is_reference<T>::value &&
                                    std::is_constructible<T>::value &&
                                    std::is_destructible<T>::value>::type>::
    allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                               typename BasicAlloc<T>::constructor_list const &constructors) {
        (void) cls;
        //Check if argument is list of tuples, and if so construct
        //an array of objects to store "behind the pointer"

        Py_ssize_t size = 1;
        if ((!kwds || PyDict_Size(kwds) == 0) && args && (PyTuple_Size(args) == 1) &&
            PyList_Check(PyTuple_GetItem(args, 0))) {
            PyObject *list = PyTuple_GetItem(args, 0);
            size = PyList_Size(list);
            char *raw_storage;
            T_NoRef *values;
            PtrWrapper::initialize();


            raw_storage = (char *) operator new[](size * sizeof(T));
            memset(raw_storage, 0, size * sizeof(T));
            values = (T_NoRef *) raw_storage;
            for (Py_ssize_t i = 0; i < size; ++i) {
                bool found = false;
                PyObject *constructor_pyargs = PyList_GetItem(list, i);
                if (!PyTuple_Check(constructor_pyargs) && !PyDict_Check(constructor_pyargs)) {
                    PyErr_SetString(PyExc_TypeError, "Invalid element in list argument, expected tuple");
                    //clean up before singalling exception
                    if (raw_storage) {
                        for (Py_ssize_t j = 0; j < i; ++j) {
                            values[j].~T();
                        }
                        delete[] raw_storage;
                        raw_storage = nullptr;
                    }
                    //signal exception:
                    return nullptr;
                } else if (PyTuple_Check(constructor_pyargs)) {
                    for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                        try {
                            // declare t NOT be in-place as we will destroy the object created here
                            ObjContainerPtrProxy<T_NoRef, false, false> cobj = ObjContainerPtrProxy<T_NoRef, false, false>(
                                    &values[i], size, false);
                            ObjContainer<T_NoRef> *cobjptr = &cobj;
                            if ((*it)(constructor_pyargs, nullptr, cobjptr, false)) {
                                found = true;
                                break;
                            }
                        } catch (...) {
                        }
                        PyErr_Clear();
                    }
                } else if (PyDict_Check(constructor_pyargs)) {
                    for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                        try {
                            static PyObject *emptyargs = PyTuple_New(0);
                            // declare t NOT be in-place as we will destroy the object created here
                            ObjContainerPtrProxy<T_NoRef, false, false> cobj = ObjContainerPtrProxy<T_NoRef, false, false>(
                                    &values[i], size, false);
                            ObjContainer<T_NoRef> *cobjptr = &cobj;
                            if ((*it)(emptyargs, constructor_pyargs, cobjptr, false)) {
                                found = true;
                                break;
                            }
                        } catch (...) {
                        }
                        PyErr_Clear();
                    }
                }
                if (!found || !values) {
                    PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
                    if (raw_storage) {
                        for (Py_ssize_t j = 0; j < i; ++j) {
                            values[j].~T();
                        }
                        delete[] raw_storage;
                        raw_storage = nullptr;
                    }
                    return nullptr;
                }
            }
            PtrWrapper *obj = (PtrWrapper *) PtrWrapper::createPy2(size, &values, false, true/*inplace*/);
            if (raw_storage && obj) {
                obj->set_raw_storage(values, size);
            } else if (raw_storage) { // obj==nullptr
                //clean up, since will be signalling exception
                for (Py_ssize_t j = 0; j < size; ++j) {
                    values[j].~T();
                }
                delete[] raw_storage;
                raw_storage = nullptr;
            }
            return (PyObject *) obj;
        }

        //otherwise, just have regular list of constructor arguments
        //for single object allocation
        PyObject *alloc_kwds = PyDict_New();
        PyDict_SetItemString(alloc_kwds, "__internal_allow_null", Py_True);
        ObjContainer<T_NoRef> *cobj = nullptr;
        for (auto it = constructors.begin(); it != constructors.end(); ++it) {
            try {
                if ((*it)(args, kwds, cobj, false)) { break; }
            } catch (...) {
            }
            PyErr_Clear();
        }
        if (!cobj) {
            Py_DECREF(alloc_kwds);
            PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation or no public constructor for class");
            return nullptr;
        }
        return (PyObject *) PtrWrapper::createPy2(size, cobj ? cobj->ptrptr() : nullptr, true, false);
    }



    template<typename T, typename PtrWrapper, typename ClassWrapper>
    void ObjectLifecycleHelpers::Alloc<T, PtrWrapper,
            ClassWrapper,
            typename std::enable_if<!is_function_ptr<T>::value && !std::is_reference<T>::value &&
                                    std::is_constructible<T>::value &&
                                    !std::is_destructible<T>::value>::type> ::
    deallocREMOVE(ObjContainer <T_NoRef> *ptr, const bool inPlace, const size_t size) {
        if (inPlace && ptr) {
            for (size_t i = 0; i < size; ++i) {
                ptr->ptr()[i].~T();
            }
        } else {
            delete ptr;
        }
    }

    template<typename T, typename PtrWrapper, typename ClassWrapper>
    PyObject *ObjectLifecycleHelpers::Alloc<T, PtrWrapper,
            ClassWrapper,
            typename std::enable_if<!is_function_ptr<T>::value && !std::is_reference<T>::value &&
                                    std::is_constructible<T>::value &&
                                    !std::is_destructible<T>::value>::type> ::
    allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                               typename BasicAlloc<T>::constructor_list const &constructors) {
        (void) cls;
        //Check if argument is list of tuples, and if so construct
        //an array of objects to store "behind the pointer"
        if ((!kwds || PyDict_Size(kwds) == 0) && args && PyTuple_Size(args) == 1 && PyList_Check(args)) {
            PyObject *list = PyTuple_GetItem(args, 0);
            const Py_ssize_t size = PyList_Size(list);
            char *raw_storage;
            T_NoRef *values;
            //if (size > 1) {
            raw_storage = (char *) operator new[](size * sizeof(ObjContainer<T_NoRef>));
            memset(raw_storage, 0, size * sizeof(T));
            values = reinterpret_cast<T_NoRef *>(raw_storage);
            //} else {
            //    raw_storage = nullptr;
            //    values = nullptr;
            //}
            for (Py_ssize_t i = 0; i < PyTuple_Size(args); ++i) {
                bool found = false;
                PyObject *constructor_pyargs = PyList_GetItem(list, i);
                if (!PyTuple_Check(constructor_pyargs) && !PyDict_Check(constructor_pyargs)) {
                    PyErr_SetString(PyExc_TypeError, "Invalid element in list argument, expected tuple");
                    if (raw_storage) {
                        if (raw_storage) {
                            for (Py_ssize_t j = 0; j < i; ++j) {
                                values[j].~T();
                            }
                        }
                        delete[] raw_storage;
                    }
                    PyErr_SetString(PyExc_RuntimeError,
                                    "NOTE: Freed memory, but no visible destructor available to call.");
                    return nullptr;
                } else if (PyTuple_Check(constructor_pyargs)) {
                    for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                        try {
                            static PyObject *emptylist = PyDict_New();
                            ObjContainer<T_NoRef> cobj = ObjContainerPtrProxy<T_NoRef, false, false>(
                                    &values[i]);
                            if ((*it)(args, emptylist, cobj)) {
                                found = true;
                                break;
                            }
                        } catch (...) {
                        }
                        PyErr_Clear();
                    }
                } else if (PyDict_Check(constructor_pyargs)) {

                    for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                        try {
                            static PyObject *emptyargs = PyTuple_New(0);
                            ObjContainer<T_NoRef> cobj = ObjContainerPtrProxy<T_NoRef, false, false>(
                                    &values[i]);
                            if ((*it)(emptyargs, constructor_pyargs, cobj)) {
                                found = true;
                                break;
                            }
                        } catch (...) {
                        }
                        PyErr_Clear();
                    }
                }
                if (!found || !values) {
                    PyErr_SetString(PyExc_RuntimeError,
                                    "Invalid constructor arguments on allocation.  Objects not destructible by design! ");
                    if (raw_storage) {
                        for (Py_ssize_t j = 0; j < i; ++j) {
                            values[j].~T();
                        }
                        delete[] raw_storage;
                    }
                    return nullptr;

                }
            }
            PtrWrapper *obj = PtrWrapper::template createPy(size, values, !raw_storage);
            obj->set_raw_storage(raw_storage);
            return (PyObject *) obj;
        }
        //otherwise, just have regular list of constructor arguments
        //for single object allocation
        T *cobj = nullptr;
        for (auto it = constructors.begin(); it != constructors.end(); ++it) {
            try {
                if ((*it)(args, kwds, cobj)) { break; }
            } catch (...) {
            }
            PyErr_Clear();
        }
        if (!cobj) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
            return nullptr;
        }
        PythonClassWrapper<T_NoRef *, void> *obj =
                PythonClassWrapper<T_NoRef *, void>::template createPy(1, cobj, true);

        return (PyObject *) obj;
    }






    template<typename ReturnType, typename ...Args>
    void ObjectLifecycleHelpers::Alloc<ReturnType(*)(Args...), PythonClassWrapper<ReturnType(**)(Args...)>,
            PythonClassWrapper<ReturnType(*)(Args...)>,
            void>::
    deallocREMOVE(ObjContainer <T_NoRef> *ptr, const bool inPlace, const size_t size,
                              const bool nonononon) {
        if (inPlace) {
            for (size_t i = 0; i < size; ++i) {
                ptr->ptr()[i].~T();
            }
        }
        delete ptr;
    }

    template<typename ReturnType, typename ...Args>
    PyObject *ObjectLifecycleHelpers::Alloc<ReturnType(*)(Args...), PythonClassWrapper<ReturnType(**)(Args...)>,
            PythonClassWrapper<ReturnType(*)(Args...)>,
            void>::
    allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                               typename BasicAlloc<T>::constructor_list const &constructors) {
        (void) cls;
        //Check if argument is list of tuples, and if so construct
        //an array of objects to store "behind the pointer"
        if ((!kwds || PyDict_Size(kwds) == 0) && args && PyTuple_Size(args) == 1 && PyList_Check(args)) {
            PyObject *list = PyTuple_GetItem(args, 0);
            const Py_ssize_t size = PyList_Size(list);
            char *raw_storage;
            T_NoRef *values;
            if (size > 1) {
                raw_storage = (char *) operator new[](size * sizeof(T));
                memset(raw_storage, 0, size * sizeof(T));
                values = reinterpret_cast<T_NoRef *>(raw_storage);
            } else {
                raw_storage = nullptr;
                values = nullptr;
            }
            for (Py_ssize_t i = 0; i < PyTuple_Size(args); ++i) {
                PyObject *constructor_pyargs = PyList_GetItem(list, i);
                if (!PyTuple_Check(constructor_pyargs) && !PyDict_Check(constructor_pyargs)) {
                    PyErr_SetString(PyExc_TypeError, "Invalid element in list argument, expected tuple");
                    if (raw_storage) {
                        for (Py_ssize_t j = 0; j < i; ++j) {
                            values[j].~T();
                        }
                        delete[] raw_storage;
                    }
                    return nullptr;
                } else if (PyTuple_Check(constructor_pyargs)) {
                    for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                        try {
                            T_NoRef *cobj = &(*values)[i];
                            if ((*it)(constructor_pyargs, nullptr, cobj)) { break; }
                        } catch (...) {
                        }
                        PyErr_Clear();
                    }
                } else if (PyDict_Check(constructor_pyargs)) {
                    for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                        try {
                            static PyObject *emptyargs = PyTuple_New(0);
                            T_NoRef *cobj = &(*values)[i];
                            if ((*it)(emptyargs, constructor_pyargs, cobj, false)) { break; }
                        } catch (...) {
                        }
                        PyErr_Clear();
                    }

                    if (!values || !values[i]) {
                        PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
                        if (raw_storage) {
                            for (Py_ssize_t j = 0; j < i; ++j) {
                                values[j].~T();
                            }
                            delete[] raw_storage;
                        }
                        return nullptr;
                    }
                }
            }
            if (!raw_storage) {
                PyErr_SetString(PyExc_RuntimeError, "Invalid object creation");
                return nullptr;
            }
            PtrWrapper *obj = PtrWrapper::createPy(size, values, !raw_storage);
            obj->set_raw_storage(raw_storage);
            return (PyObject *) obj;
        }

        //otherwise, just have regular list of constructor arguments
        //for single object allocation
        T *cobj = nullptr;
        for (auto it = constructors.begin(); it != constructors.end(); ++it) {
            try {
                if ((cobj = (*it)(args, kwds))) { break; }
            } catch (...) {
            }
            PyErr_Clear();
        }
        if (!cobj) {
            PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
            return nullptr;
        }
        PtrWrapper *obj = PythonClassWrapper<T_NoRef *, void>::template createPy<T_NoRef *>(1, cobj, true);
        obj->set_raw_storage(nullptr);
        return (PyObject *) obj;
    }



    template<typename T>
    void ObjectLifecycleHelpers::Alloc<T,
            PythonClassWrapper<T *, void>,
            PythonClassWrapper<T, void>,
            typename std::enable_if<std::is_void<typename std::remove_volatile<T>::type>::value>::type>::
    deallocREMOVE(ObjContainer <C_NoRef> *ptr) {
        (void) ptr;
    }

    template<typename T>
    PyObject *ObjectLifecycleHelpers::Alloc<T,
            PythonClassWrapper<T *, void>,
            PythonClassWrapper<T, void>,
            typename std::enable_if<std::is_void<typename std::remove_volatile<T>::type>::value>::type>::
    allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                               typename BasicAlloc<T>::constructor_list const &constructors) {
        (void) args;
        (void) kwds;
        (void) cls;
        PyErr_SetString(PyExc_RuntimeError, "Type is not directly constructible");
        return nullptr;
    }



    template<typename T, typename PtrWrapper, typename ClassWrapper>
    void ObjectLifecycleHelpers::Alloc<T, PtrWrapper,
            ClassWrapper,
            typename std::enable_if<!std::is_void<T>::value && !std::is_function<T>::value &&
                                    (std::is_reference<T>::value || !std::is_constructible<T>::value)>::type>::
    deallocREMOVE(ObjContainer <C_NoRef> *ptr) {
        (void) ptr;
    }

    template<typename T, typename PtrWrapper, typename ClassWrapper>
    PyObject *ObjectLifecycleHelpers::Alloc<T, PtrWrapper,
            ClassWrapper,
            typename std::enable_if<!std::is_void<T>::value && !std::is_function<T>::value &&
                                    (std::is_reference<T>::value || !std::is_constructible<T>::value)>::type>::
    allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                               typename BasicAlloc<T>::constructor_list const &constructors) {
        (void) args;
        (void) kwds;
        (void) cls;
        (void) constructors;
        PyErr_SetString(PyExc_RuntimeError, "Type is not directly constructible");
        return nullptr;
    }



    template<typename T, typename PtrWrapper, typename ClassWrapper>
    void ObjectLifecycleHelpers::Alloc<T, PtrWrapper,
            ClassWrapper,
            typename std::enable_if<std::is_function<T>::value>::type>::
    deallocREMOVE(ObjContainer <C_NoRef> *ptr) {
        (void) ptr;
    }

    template<typename T, typename PtrWrapper, typename ClassWrapper>
    PyObject *ObjectLifecycleHelpers::Alloc<T, PtrWrapper,
            ClassWrapper,
            typename std::enable_if<std::is_function<T>::value>::type>::
    allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                               typename BasicAlloc<T>::constructor_list const &constructors) {
        (void) args;
        (void) kwds;
        (void) cls;
        (void) constructors;
        PyErr_SetString(PyExc_RuntimeError, "Type is not directly constructible");
        return nullptr;
    }


    template<typename T, typename PtrWrapper>
    void ObjectLifecycleHelpers::ObjectContent<T *, PtrWrapper, typename std::enable_if<std::is_void<T>::value>::type> ::
    set(const size_t index, T **const to, T **const from, const size_t depth){
        to[index] = *from;
    }

    template<typename T, typename PtrWrapper>
    PyObject *ObjectLifecycleHelpers::ObjectContent<T *, PtrWrapper, typename std::enable_if<std::is_void<T>::value>::type> ::
    getObjectAt(T *const from, const size_t index, const ssize_t elements_array_size,
                                 const size_t depth, const bool asArgument) {
        (void) from;
        (void) index;
        (void) elements_array_size;
        PyErr_SetString(PyExc_TypeError, "Attempt to get element from non-array object");
        return nullptr;
    }

    template<typename T, typename PtrWrapper>
    T **ObjectLifecycleHelpers::ObjectContent<T *, PtrWrapper, typename std::enable_if<std::is_void<T>::value>::type> ::
    getObjectPtr(PtrWrapper *const self) {
        return (T **) &self->_CObject;
    }



    template<typename T, typename PtrWrapper>
    void ObjectLifecycleHelpers::ObjectContent<T, PtrWrapper, typename std::enable_if<std::is_void<T>::value> > ::
    set(const size_t index, void *const to, void *const from, const size_t depth) {
        throw "Attempt to access element from object  of type void";
    }

    template<typename T, typename PtrWrapper>
    PyObject *ObjectLifecycleHelpers::ObjectContent<T, PtrWrapper, typename std::enable_if<std::is_void<T>::value> > ::
    getObjectAt(void *const from, const size_t index, const ssize_t elements_array_size,
                                 const size_t depth, const bool asArgument ) {
        (void) from;
        (void) index;
        (void) elements_array_size;
        PyErr_SetString(PyExc_TypeError, "Attempt to get element from non-array object");
        return nullptr;
    }

    template<typename T, typename PtrWrapper>
    void *ObjectLifecycleHelpers::ObjectContent<T, PtrWrapper, typename std::enable_if<std::is_void<T>::value> > ::
    getObjectPtr(PtrWrapper *const self) {
        return (void *) &self->_CObject;
    }




    template<typename T, typename PtrWrapper>
    void ObjectLifecycleHelpers::ObjectContent<T *const, PtrWrapper, typename std::enable_if<std::is_void<T>::value>::type>::
    set(const size_t index, T *const *const to, T *const *const from, const size_t depth) {
        throw "Attempt to assign to const value";
    }

    template<typename T, typename PtrWrapper>
    PyObject *ObjectLifecycleHelpers::ObjectContent<T *const, PtrWrapper, typename std::enable_if<std::is_void<T>::value>::type>::
    getObjectAt(T *const from, const size_t index, const ssize_t elements_array_size,
                                 const size_t depth, const bool asArgument ) {
        (void) from;
        (void) index;
        (void) elements_array_size;
        PyErr_SetString(PyExc_TypeError, "Attempt to get element from non-array object");
        return nullptr;
    }

    template<typename T, typename PtrWrapper>
    T *const *ObjectLifecycleHelpers::ObjectContent<T *const, PtrWrapper, typename std::enable_if<std::is_void<T>::value>::type>::
    getObjectPtr(PtrWrapper *const self) {
        return (T *const *) &self->_CObject;
    }
}


#endif
