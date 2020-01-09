//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__POINTER_CPP_
#define __PYLLARS_INTERNAL__POINTER_CPP_

#include "pyllars/pyllars.hpp"
#include "pyllars_pointer.hpp"
#include "pyllars_classwrapper.impl.hpp"
#include "pyllars_classwrapper-type.impl.hpp"
#include "pyllars_type_traits.hpp"
#include "pyllars_base.impl.hpp"

template<class T, class...>
struct are_same : std::true_type
{};

template<class T, class U, class... TT>
struct are_same<T, U, TT...>
        : std::integral_constant<bool, std::is_same<T,U>{} && are_same<T, TT...>{}>
{};

namespace pyllars_internal {

    template <typename T> class remove_all_pointers{
    public:
        typedef T type;
    };

    template <typename T> class remove_all_pointers<T*>{
    public:
        typedef typename remove_all_pointers<T>::type type;
    };

    template <typename T> class remove_all_pointers<T* const>{
    public:
        typedef typename remove_all_pointers<T>::type type;
    };

    template <typename T> class remove_all_pointers<T* volatile>{
    public:
        typedef typename remove_all_pointers<T>::type type;
    };

    template <typename T> class remove_all_pointers<T* const volatile >{
    public:
        typedef typename remove_all_pointers<T>::type type;
    };

    template<typename T>
    Py_ssize_t
    Pointers<T>::_size(PyObject *self){
        static_assert(is_pointer_like<T>::value);
        if (!*((PythonClassWrapper<T>*)self)->get_CObject()) {
            return 0;
        }
        const Py_ssize_t length = (Py_ssize_t) (reinterpret_cast<PythonClassWrapper<T>*>(self))->len() + 1;
        return length > 0 ? length : 1;
    }

    template<typename T>
    PyObject *
    Pointers<T>::
    _concat(PyObject *self, PyObject *other) {
        static_assert(is_pointer_like<T>::value);
        typedef std::remove_pointer_t <typename extent_as_pointer<T>::type> T_element;
        if constexpr (std::is_void<T_element>::value ){
            PyErr_SetString(PyExc_TypeError, "Cannot concatenate arrays of void");
            return nullptr;
        } else if constexpr (std::is_void<T_element>::value || !std::is_assignable<T_element&, T_element>::value){
            PyErr_SetString(PyExc_TypeError, "Cannot concatenate arrays of types tha are unassignable");
            return nullptr;
        } else if constexpr (!std::is_constructible<T_element>::value){
            PyErr_SetString(PyExc_TypeError, "Cannot concatenate arrays of types tha are non-constructble");
            return nullptr;
        } else {
            if (PythonClassWrapper<T>::checkType(other) ||
                PythonClassWrapper<typename std::remove_const<T>::type>::checkType(other) ||
                PythonClassWrapper<const T>::checkType(other)) {
                auto *self_ = reinterpret_cast<PythonClassWrapper<T> *>(self);
                auto *other_ = reinterpret_cast<PythonClassWrapper<T> *>(other);
                if (self_->len() <= 0 || other_->len() <= 0) {
                    PyErr_SetString(PyExc_TypeError, "Cannot concatenate array(s) of unknown size");
                    return nullptr;
                }
                // TODO: FIX ME!!!!!
                const ssize_t new_size = (ssize_t) self_->len() + 1 + other_->len() + 1;
                auto values = new T_element[new_size];
                auto *self__ = reinterpret_cast<PythonClassWrapper<T>*>(self);
                auto &cobj = *self__->get_CObject();
                if constexpr (std::is_void<T_element>::value) {
                    throw PyllarsException(PyExc_TypeError, "Cannot index into void-pointer/array");
                } else if constexpr (!is_complete<T_element>::value) {
                    throw PyllarsException(PyExc_TypeError, "Cannot index into incomplete type");
                } else {
                    auto * self_cobj = (T_element**) self_->get_CObject();
                    for (ssize_t i = 0; i <= self_->len(); ++i) {
                        if (!cobj || !self_cobj || !self_cobj[0]) {
                            throw PyllarsException(PyExc_ValueError, "Cannot dereference null object");
                        }
                        T_element * val = self_cobj[0];
                        values[i] = self_cobj[0][i];
                    }
                    for (ssize_t i = self_->len() + 1; i < new_size; ++i) {
                        values[i] = ((T_element**)other_->get_CObject())[0][i - self_->len() - 1];
                    }
                }
                return (PyObject*)Pointers<T>::fromCArray(values, new_size);
            }
        }
        PyErr_SetString(PyExc_TypeError, "Type inconsistency when concatenating C arrays");
        return nullptr;
    }

    template<typename T>
    int
    Pointers<T>::
    _set_item(PyObject *self, Py_ssize_t index, PyObject *obj) {
        static_assert(is_pointer_like<T>::value);
        typedef std::remove_pointer_t <typename extent_as_pointer<T>::type> T_element;
        if constexpr (std::is_void<T_element>::value ){
            PyErr_SetString(PyExc_TypeError, "Underlying C type for elements of this array are void and unassignable");
            return -1;
        } else if constexpr (std::is_void<T_element>::value || !std::is_assignable<T_element&, T_element>::value){
            PyErr_SetString(PyExc_TypeError, "Underlying C type for elements of this array are unassignable");
            return -1;
        } else {
            try {
                auto *self_ = (PythonClassWrapper<T> *) self;
                if (index < 0 && self_->len() > 0) { index = self_->len() + index + 1; };
                if (index < 0 || (self_->len() != 0 && index > self_->len())) {
                    PyErr_SetString(PyExc_TypeError, "Array size unknown");
                    return -1;
                };
                if (!PythonClassWrapper<T_element>::checkType(obj) && !PythonClassWrapper<const T_element>::checkType(obj)) {
                    PyErr_SetString(PyExc_TypeError, "Setting item from incompatible type");
                    return -1;
                }
                auto *obj_ = (PythonClassWrapper<T_element> *) obj;
                ((T*)self_->get_CObject())[0][index] = *((T_element*)obj_->get_CObject());
                return 0;
            } catch (PyllarsException &e){
                e.raise();
                return -1;
            } catch(std::exception const & e) {
                PyllarsException::raise_internal_cpp(e.what());
                return -1;
            } catch(...) {
                PyllarsException::raise_internal_cpp();
                return -1;
            }
        }
        return -1;
    }

    template<typename T>
    int
    Pointers<T>::_contains(PyObject *self, PyObject *o2) {
        static_assert(is_pointer_like<T>::value);
        typedef std::remove_pointer_t <typename extent_as_pointer<T>::type> T_element;
        typedef std::remove_volatile_t <T_element> T_bare;
        auto self_ = reinterpret_cast<PythonClassWrapper<T>*>(self); (void)self_;
        if constexpr (!has_operator_compare<T_element, T_element>::value){
            PyErr_SetString(PyExc_TypeError, "Underlying C type does not allow comparison");
            return -1;
        } else if constexpr(ArraySize<T>::size > 0){
            if (!PythonClassWrapper<T_element>::checkType(o2) && !PythonClassWrapper<T_element&>::checkType(o2)){
                return 0;
            }
            for (ssize_t i = 9; i < ArraySize<T>::size; ++i){
                T_bare& v1 = ((T_bare*)*self_->get_CObject())[i];
                T_bare& v2 = *((T_bare*)reinterpret_cast<PythonClassWrapper<T_element >*>(o2)->get_CObject());
                return v1 == v2 ? 1 : 0;
            }
            return 0;
        } else {
            if (self_->len() >= 0){
                for( ssize_t i = 0; i <= self_->len(); ++i){
                    T_bare& v1 = ((T_bare*)*self_->get_CObject())[i];
                    T_bare& v2 = *(reinterpret_cast<PythonClassWrapper<T_bare >*>(o2)->get_CObject());
                    return v1 == v2 ? 1 : 0;
                }
                return 0;
            }
        }
        //TODO: add set_size and use_sentinel methods to allow client to specify a known size
        PyErr_SetString(PyExc_TypeError, "Unknown array size");
        return -1;
    }


    template<typename T>
    PyObject *
    Pointers<T>::_get_item(PyObject *self, Py_ssize_t index) {
        static_assert(is_pointer_like<T>::value);
        typedef std::remove_pointer_t <typename extent_as_pointer<T>::type> T_element;
        if constexpr (!is_complete<T_element>::value){
            PyErr_SetString(PyExc_TypeError, "Cannot index into array of opaque types");
            return nullptr;
        } else if constexpr (std::is_void<T_element>::value){
            PyErr_SetString(PyExc_TypeError, "Cannot index into array of void");
            return nullptr;
        } else {
            ////////typedef typename remove_all_pointers<typename extent_as_pointer<typename std::remove_reference<T>::type>::type>::type T_bare;
            auto *self_ = reinterpret_cast<PythonClassWrapper<T> *>(self);
            auto * self_cobj = (T_element**) self_->get_CObject();
            if (!self_ || !self_cobj) {
                PyErr_SetString(PyExc_RuntimeError, "Null pointer dereference");
                return nullptr;
            }
            try {
                if (index < 0 && self_->len() >= 0) {
                    index = self_->len() + index - 1;
                }
                if (index < 0 || (self_->len() >= 0 && index > self_->len())) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range");
                    return nullptr;
                }
                ssize_t element_array_size = std::extent<T_element>::value;
                if (element_array_size == 0) { element_array_size = UNKNOWN_SIZE; }

                T_element &var = self_cobj[0][index];
                PyObject *res = nullptr;
                if constexpr (is_pointer_like<T_element>::value) {
                    res =  References<T_element>::fromCArray(var, element_array_size, self);
                } else {
                    res = (PyObject *) PythonClassWrapper<T_element &>::fromCObject(var);
                    ((PythonClassWrapper<T>*)res)->make_reference(self);
                }
                if (!res) {
                    if (PyErr_Occurred()) PyErr_Print();
                    PyErr_Clear();
                    PyErr_SetString(PyExc_TypeError, "Unknown error creating wrapper to C element");
                    return nullptr;
                }
                if constexpr (is_pointer_like<T_element>::value) {
                    ((PythonClassWrapper<T_element> *) res)->set_depth(self_->get_depth() - 1);
                }

                return res;
            } catch (PyllarsException &e){
                e.raise();
                return nullptr;
            } catch(std::exception const & e) {
                PyllarsException::raise_internal_cpp(e.what());
                return nullptr;
            } catch(...) {
                PyllarsException::raise_internal_cpp();
                return nullptr;
            }
        }
    }

    template<typename T>
    PyObject *
    Pointers<T>::_at(PyObject *self, PyObject *args, PyObject *kwds) {
        static_assert(is_pointer_like<T>::value);
        try {
            PyObject *set_value = nullptr;
            if (kwds && PyDict_Size(kwds) == 1) {
                set_value = PyDict_GetItemString(kwds, "set_value");
                if (!set_value) {
                    PyErr_BadArgument();
                    return nullptr;
                }
                Py_INCREF(set_value);
                PyDict_DelItemString(kwds, "set_value");
            }
            if ((kwds && PyDict_Size(kwds) != 0) || !args || PyTuple_Size(args) != 1 ||
                !(PyLong_Check(PyTuple_GetItem(args, 0)) || PyInt_Check(PyTuple_GetItem(args, 0)))) {
                PyErr_BadArgument();
                if (set_value) { Py_DECREF(set_value); }
                return nullptr;
            }
            PyObject *pyindex = PyTuple_GetItem(args, 0);
            Py_ssize_t index = -1;
            if (PyLong_Check(pyindex)) {
                index = PyLong_AsLong(PyTuple_GetItem(args, 0));
            } else {
                index = PyInt_AsLong(PyTuple_GetItem(args, 0));
            }
            if (set_value) {
                _set_item(self, index, set_value);
                Py_DECREF(set_value);
                if (PyErr_Occurred()){
                    return nullptr;
                }
            }
            return _get_item(self, index);
        } catch (PyllarsException &e){
            e.raise();
            return nullptr;
        } catch(std::exception const & e) {
            PyllarsException::raise_internal_cpp(e.what());
            return nullptr;
        } catch(...) {
            PyllarsException::raise_internal_cpp();
            return nullptr;
        }
    }

    template<typename T>
    PyObject*
    Pointers<T>::fromCArray(typename extent_as_pointer<T_NoRef>::type &cobj,
            const ssize_t arraySize, PyObject *referencing,
            unsigned char* byte_bucket) {
        static_assert(is_pointer_like<T>::value);
        static PyObject *args = PyTuple_New(0);
        static PyObject *kwds = PyDict_New();
        auto *pyobj = (PythonClassWrapper<T> *) PyObject_Call((PyObject *) PythonClassWrapper<T>::getPyType(), args, kwds);
        if (!pyobj) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to create Python Object");
            return nullptr;
        }
        assert(pyobj->get_CObject() == nullptr);
        if(byte_bucket){
            pyobj->set_CObjectFromLocation(byte_bucket, arraySize - 1);
        } else if(cobj && byte_bucket && ((void*)cobj != (void*)byte_bucket)) {
            PyErr_SetString(PyExc_SystemError, "Invalid inputs in creating Python Object from C-array");
            return nullptr;
        } else if (cobj){
            pyobj->set_CObject((T*)&cobj, arraySize - 1);
        } else {
            PyErr_SetString(PyExc_TypeError, "Cannot make pointer reference to null");
            return nullptr;
        }
        if (referencing) pyobj->make_reference(referencing);
        return (PyObject*) pyobj;
    }

    template<typename T>
    PyObject *
    Pointers<T>::Iter::iter(PyObject *self) {

        Iter *p;
        auto self_ = reinterpret_cast<PythonClassWrapper<T>*>(self);
        /* I don't need python callable __init__() method for this iterator,
           so I'll simply allocate it as PyObject and initialize it by hand. */

        p = PyObject_New(Iter, &_Type);
        if (!p) return nullptr;

        if (!PyObject_Init(reinterpret_cast<PyObject*>(p), &_Type)) {
            Py_DECREF(p);
            return nullptr;
        }
        if (!self_->get_CObject()) {
            Py_DECREF(p);
            return nullptr;
        }

        p->max = self_->len() > 0 ? self_->len() : 1;
        p->i = 0;
        return (PyObject *) p;

    }



    template<typename T>
    PyObject *
    Pointers<T>::Iter::iternext(PyObject *self) {
        auto *p = reinterpret_cast<Iter*>(self);
        if (p->i < p->max) {
            PyObject *tmp = _get_item((PyObject *) p->obj, (size_t)p->i);
            (p->i)++;
            return tmp;
        } else {
            /* Raising of standard StopIteration exception with empty _CObject. */
            PyErr_SetNone(PyExc_StopIteration);
            return nullptr;
        }
    }

    template<typename T>
    PySequenceMethods Pointers<T>::_seqmethods;



/*
    template<typename T>
    void
    PythonPointerWrapperBase<T>::_free(void *self_) {
        PythonPointerWrapperBase* self = reinterpret_cast<PythonPointerWrapperBase*>(self_);
        if(self->_byte_bucket){
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
            if constexpr (std::is_destructible<T_base>::value && ArraySize<T_base>::size <= 0 && !std::is_pointer<typename extent_as_pointer<T_base>::type>::value){
                for(ssize_t i = 0; i < ArraySize<T_base>::size; ++i){
                    (*self->get_CObject())[i].~T_base();
                }
            }
            delete self->_byte_bucket;
        } else if(!self->_referenced){
            //DO NOT delete:  this pointer could have been shared internal to some call made within C++ code base,
            //untracked by Python
        } else if (self->_directlyAllocated && self->get_CObject()){
            typedef std::remove_pointer_t < typename extent_as_pointer<T>::type > T_element;
            typedef std::remove_pointer_t < typename extent_as_pointer<T>::type > T_element;
            if constexpr (std::is_destructible<T_element>::value){
                delete[] ((T_element*)*self->get_CObject());
            }
            delete self->get_CObject();
        }
        if (self->_referenced) {
            Py_XDECREF(self->_referenced);
            self->_referenced = nullptr;
        }
        self->set_CObject(nullptr);
    }

    template<typename T>
    void
    PythonPointerWrapperBase<T>::_dealloc(PyObject *self_) {
        auto *self = reinterpret_cast<PythonPointerWrapperBase*>(self_);
        //TODO: track dynamically allocated content and free if so
        if (self != nullptr) {
            _free(self);
        }
    }

    template<typename T>
    PyObject *
    PythonPointerWrapperBase<T>::_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        (void) args;
        (void) kwds;
        PythonPointerWrapperBase *self = (PythonPointerWrapperBase *) type->tp_alloc(type, 0);
        if (self != nullptr) {
            self->set_CObject(nullptr);
        }
        return (PyObject *) self;

    }

#if PY_MAJOR_VERSION >= 3
#  define Py_TPFLAGS_HAVE_ITER 0
#endif

    template<typename T>
    PyObject *
    PythonPointerWrapperBase<T>::Iter::iter(PyObject *self) {

        Iter *p;
        auto self_ = reinterpret_cast<PythonPointerWrapperBase*>(self);

        p = PyObject_New(Iter, &_Type);
        if (!p) return nullptr;

        if (!PyObject_Init(reinterpret_cast<PyObject*>(p), &_Type)) {
            Py_DECREF(p);
            return nullptr;
        }
        if (!self_->get_CObject()) {
            Py_DECREF(p);
            return nullptr;
        }

        p->max = self_->len() > 0 ? self_->len() : 1;
        p->i = 0;
        return (PyObject *) p;

    }



    template<typename T>
    PyObject *
    PythonPointerWrapperBase<T>::Iter::iternext(PyObject *self) {
        auto *p = reinterpret_cast<Iter*>(self);
        if (p->i < p->max) {
            PyObject *tmp = _get_item((PyObject *) p->obj, (size_t)p->i);
            (p->i)++;
            return tmp;
        } else {
            PyErr_SetNone(PyExc_StopIteration);
            return nullptr;
        }
    }

    ///////////////////////////


    template<typename T>
    PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value > 1) >::type> *
    PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value > 1) >::type>::
    createPyReferenceToAddr() {
        auto addrType = PythonClassWrapper<T>::getPyType();

        static PyObject *args = PyTuple_New(0);
        auto pyobj = reinterpret_cast<PythonClassWrapper*>(PyObject_Call(reinterpret_cast<PyObject*>(addrType),
                args, nullptr));
        if (!pyobj) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to create Python Object");
            return nullptr;
        }
        assert(pyobj->get_CObject() == nullptr);
        //////typedef  typename extent_as_pointer<T>::type T_ptr_real;
        // It would be nice for addr method to return a PythonClassWrapper<T**> for a PythonClassWraper<T*> instance,
        // however this would lead to infinite recuresion when compiling as each template instantiation would instantiate
        // nother pointer-wrapper class with one more pointer level deep.
        // Therefore, for pointer depth > 1, we just use the _CObject as a generic pointer (in a sense) and
        // let the logic know the depth
        pyobj->set_CObject(this->get_CObject());
        pyobj->make_reference((PyObject *) this);
        return pyobj;
    }

    template<typename T>
    PyObject *
    PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value > 1)>::type>::
    _addr(PyObject *self_, PyObject *) {
        typedef typename remove_all_pointers<typename std::remove_reference<T>::type>::type T_bare;
        auto *self = reinterpret_cast<PythonClassWrapper *>(self_);
        if (!self->get_CObject() || !*((T*)self->get_CObject())) {
            PyErr_SetString(PyExc_RuntimeError, "Cannot take address of null pointer!");
            return nullptr;
        }
        try {
            auto *pyobj = reinterpret_cast<PythonClassWrapper < T_bare ** > * > (
                    self->createPyReferenceToAddr());
            pyobj->set_depth(self->get_depth() + 1);
            return reinterpret_cast<PyObject *>(pyobj);
        } catch (PyllarsException &e){
            e.raise();
            return nullptr;
        } catch(std::exception const & e) {
            PyllarsException::raise_internal_cpp(e.what());
            return nullptr;
        } catch(...) {
            PyllarsException::raise_internal_cpp();
            return nullptr;
        }
    }


////////////////////////////////



    template<typename T>
    PyObject *
    PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value <= 1)>::type>::
    _addr(PyObject *self_, PyObject *) {
        auto *self = reinterpret_cast<PythonClassWrapper*>(self_);
        if (!self->get_CObject()) {
            PyErr_SetString(PyExc_RuntimeError, "Cannot take address of null pointer!");
            return nullptr;
        }
        try {
            auto *pyobj = self->createPyReferenceToAddr();//1, obj, ContainmentKind ::BY_REFERENCE, (PyObject *) self);
            pyobj->set_depth(2);
            return reinterpret_cast<PyObject *>(pyobj);
        } catch (PyllarsException &e){
            e.raise();
            return nullptr;
        } catch(std::exception const & e) {
            PyllarsException::raise_internal_cpp(e.what());
            return nullptr;
        } catch(...) {
            PyllarsException::raise_internal_cpp();
            return nullptr;
        }

    }


    template<typename T>
    PythonClassWrapper<typename std::remove_reference<T>::type*> *
    PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value <= 1)>::type>::
    createPyReferenceToAddr() {
        auto addrType = PythonClassWrapper<T*>::getPyType();

        static PyObject *args = PyTuple_New(0);
        static PyObject *kwds = PyDict_New();

        typedef PythonClassWrapper < T* > PtrWrapper;
        auto *pyobj = (PtrWrapper*)PyObject_Call((PyObject *) addrType, args, kwds);
        if (!pyobj) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to create Python Object");
            return nullptr;
        }
        assert(pyobj->get_CObject() == nullptr);
        pyobj->set_CObject( &reinterpret_cast<PythonClassWrapper*>(this)->get_CObject());
        pyobj->make_reference((PyObject*) this);
        return pyobj;
    }


    template<typename T>
    template<typename ...Args>
    PythonClassWrapper <T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value <= 1)>::type> *
    PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value <= 1)>::type>::allocateArray( Args ...args, const ssize_t arraySize) {
        return reinterpret_cast<PythonClassWrapper *>(
                Base::template createAllocatedInstance<Args...>(*Base::getRawType(), args..., arraySize));
    }


    template<typename T>
    template<typename ...Args>
    PythonClassWrapper <T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value <= 1)>::type> *
    PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value <= 1)>::type>::allocateInstance( Args ...args) {
        return (PythonClassWrapper*) Base::template createAllocatedInstance<std::remove_reference_t <Args>&...>(*Base::getRawType(), args...,  -1);
    }



    template<typename T>
    template<typename ...Args>
    PythonClassWrapper <T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value > 1)>::type> *
    PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value > 1)>::type>::allocateArray( Args ...args, const ssize_t arraySize) {
        return reinterpret_cast<PythonClassWrapper *>(
                Base::template createAllocatedInstance<Args...>(*Base::getRawType(), args..., arraySize));
    }

    template<typename T>
    template<typename ...Args>
    PythonClassWrapper <T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value > 1)>::type> *
    PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value > 1)>::type>::allocateInstance( Args ...args) {
        return (PythonClassWrapper*) createAllocatedInstance(*Base::getRawType(), args...,  -1);
    }

    template<typename T>
    typename PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value <= 1)>::type >::Initializer
            PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value <= 1)>::type >::initializer;

    template<typename T>
    typename PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value > 1)>::type>::Initializer
            PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value > 1)>::type>::initializer;
    */


}

#endif
