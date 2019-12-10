//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__POINTER_CPP_
#define __PYLLARS_INTERNAL__POINTER_CPP_

#include "pyllars/pyllars.hpp"
#include "pyllars_pointer.hpp"
#include "pyllars/internal/pyllars_floating_point.hpp"
#include "pyllars/internal/pyllars_integer.hpp"
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"
#include "pyllars/internal/pyllars_reference.impl.hpp"
#include "pyllars/internal/pyllars_type_traits.hpp"
#include "pyllars_reference.impl.hpp"

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
    PythonPointerWrapperBase<T>::_size(PyObject *self){
        if (!*((PythonPointerWrapperBase*)self)->get_CObject()) {
            return 0;
        }
        const Py_ssize_t length = (Py_ssize_t) (reinterpret_cast<PythonPointerWrapperBase*>(self))->_max + 1;
        return length > 0 ? length : 1;
    }

    template<typename T>
    PyObject *
    PythonPointerWrapperBase<T>::
    _concat(PyObject *self, PyObject *other) {
        if constexpr (std::is_void<T_base>::value ){
            PyErr_SetString(PyExc_TypeError, "Cannot concatenate arrays of void");
            return nullptr;
        } else if constexpr (std::is_void<T_base>::value || !std::is_assignable<T_base&, T_base>::value){
            PyErr_SetString(PyExc_TypeError, "Cannot concatenate arrays of types tha are unassignable");
            return nullptr;
        } else if constexpr (!std::is_constructible<T_base>::value){
            PyErr_SetString(PyExc_TypeError, "Cannot concatenate arrays of types tha are non-constructble");
            return nullptr;
        } else {
            if (PythonClassWrapper<T>::checkType(other) ||
                PythonClassWrapper<typename std::remove_const<T>::type>::checkType(other) ||
                PythonClassWrapper<const T>::checkType(other)) {
                auto *self_ = reinterpret_cast<PythonPointerWrapperBase *>(self);
                auto *other_ = reinterpret_cast<PythonPointerWrapperBase *>(other);
                if (self_->_max <= 0 || other_->_max <= 0) {
                    PyErr_SetString(PyExc_TypeError, "Cannot concatenate array(s) of unknown size");
                    return nullptr;
                }
                // TODO: FIX ME!!!!!
                const ssize_t new_size = (ssize_t) self_->_max + 1 + other_->_max + 1;
                auto values = new T_base[new_size];
                auto *self__ = reinterpret_cast<PythonClassWrapper<T>*>(self);
                T &cobj = *self__->get_CObject();
                if constexpr (std::is_void<T_base>::value) {
                    throw PyllarsException(PyExc_TypeError, "Cannot index into void-pointer/array");
                } else if constexpr (!is_complete<T_base>::value) {
                    throw PyllarsException(PyExc_TypeError, "Cannot index into incomplete type");
                } else {
                    for (ssize_t i = 0; i <= self_->_max; ++i) {
                        if (!cobj || !self_->get_CObject()[0]) {
                            throw PyllarsException(PyExc_ValueError, "Cannot dereference null object");
                        }
                        values[i] = self_->get_CObject()[0][i];
                    }
                    for (ssize_t i = self_->_max + 1; i < new_size; ++i) {
                        values[i] = other_->get_CObject()[0][i - self_->_max - 1];
                    }
                }
                return (PyObject*)PythonClassWrapper<T_base*>::fromCPointer(values, new_size);
            }
        }
        PyErr_SetString(PyExc_TypeError, "Type inconsistency when concatenating C arrays");
        return nullptr;
    }


    template<typename T>
    int
    PythonPointerWrapperBase<T>::
    _set_item(PyObject *self, Py_ssize_t index, PyObject *obj) {
        if constexpr (std::is_void<T_base>::value ){
            PyErr_SetString(PyExc_TypeError, "Underlying C type for elements of this array are void and unassignable");
            return -1;
        } else if constexpr (std::is_void<T_base>::value || !std::is_assignable<T_base&, T_base>::value){
            PyErr_SetString(PyExc_TypeError, "Underlying C type for elements of this array are unassignable");
            return -1;
        } else {
            try {
                auto *self_ = (PythonPointerWrapperBase *) self;
                auto *self__ = (PythonClassWrapper<T> *) self;
                if (index < 0 && self_->_max > 0) { index = self_->_max + index + 1; };
                if (index < 0 || (self_->_max != 0 && index > self_->_max)) {
                    PyErr_SetString(PyExc_TypeError, "Array size unknown");
                    return -1;
                };
                if (!PythonClassWrapper<T_base>::checkType(obj) && !PythonClassWrapper<const T_base>::checkType(obj)) {
                    PyErr_SetString(PyExc_TypeError, "Setting item from incompatible type");
                    return -1;
                }
                auto *obj_ = (PythonClassWrapper<T_base> *) obj;
                self__->get_CObject()[0][index] = *obj_->get_CObject();
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
    PythonPointerWrapperBase<T>::
    _contains(PyObject *self, PyObject *o2) {
        typedef std::remove_volatile_t <T_base> T_bare;
        auto self_ = reinterpret_cast<PythonPointerWrapperBase*>(self); (void)self_;
        if constexpr (!has_operator_compare<T_base, T_base>::value){
            PyErr_SetString(PyExc_TypeError, "Underlying C type does not allow comparison");
            return -1;
        } else if constexpr(ArraySize<T>::size > 0){
            if (!PythonClassWrapper<T_base>::checkType(o2) && !PythonClassWrapper<T_base&>::checkType(o2)){
                return 0;
            }
            for (ssize_t i = 9; i < ArraySize<T>::size; ++i){
                T_bare& v1 = ((T_bare*)*self_->get_CObject())[i];
                T_bare& v2 = *((T_bare*)reinterpret_cast<PythonClassWrapper<T_base >*>(o2)->get_CObject());
                return v1 == v2 ? 1 : 0;
            }
            return 0;
        } else {
            if (self_->_max >= 0){
                for( ssize_t i = 0; i <= self_->_max; ++i){
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
    PythonPointerWrapperBase<T>::
    _get_item(PyObject *self, Py_ssize_t index) {
        if constexpr (!is_complete<T_base>::value){
            PyErr_SetString(PyExc_TypeError, "Cannot index into array of opaque types");
            return nullptr;
        } else if constexpr (std::is_void<T_base>::value){
            PyErr_SetString(PyExc_TypeError, "Cannot index into array of void");
            return nullptr;
        } else {
            ////////typedef typename remove_all_pointers<typename extent_as_pointer<typename std::remove_reference<T>::type>::type>::type T_bare;
            auto *self_ = reinterpret_cast<PythonPointerWrapperBase *>(self);
            if (!self_ || !self_->get_CObject()) {
                PyErr_SetString(PyExc_RuntimeError, "Null pointer dereference");
                return nullptr;
            }
            try {
                if (index < 0 && self_->_max >= 0) {
                    index = self_->_max + index - 1;
                }
                if (index < 0 || (self_->_max >= 0 && index > self_->_max)) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range");
                    return nullptr;
                }
                ssize_t element_array_size = std::extent<T_base>::value;
                if (element_array_size == 0) { element_array_size = UNKNOWN_SIZE; }

                T_base &var = self_->get_CObject()[0][index];
                PyObject *res = nullptr;
                if constexpr (is_pointer_like<T_base>::value) {
                    if (self_->_depth == 2) {
                        res = (PyObject *) PythonClassWrapper<T_base &>::fromCPointer(var, element_array_size,
                                                                                      self);
                    } else {
                        res = (PyObject *) PythonClassWrapper<T &>::fromCPointer((T &) var, element_array_size,
                                                                                 self);
                    }
                } else {
                    res = (PyObject *) PythonClassWrapper<T_base &>::fromCObject(var, self);
                }
                if (!res) {
                    if (PyErr_Occurred()) PyErr_Print();
                    PyErr_Clear();
                    PyErr_SetString(PyExc_TypeError, "Unknown error creating wrapper to C element");
                    return nullptr;
                }
                if constexpr (is_pointer_like<T_base>::value) {
                    ((PythonPointerWrapperBase<T_base> *) res)->_depth = self_->_depth - 1;
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
    PythonPointerWrapperBase<T>::
    _at(PyObject *self, PyObject *args, PyObject *kwds) {
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
    PythonPointerWrapperBase <T> *
    PythonPointerWrapperBase<T>::
    fromCPointer(PyTypeObject &Type,  T *cobj, const ssize_t arraySize, PyObject *referencing, unsigned char* byte_bucket) {
        if (_initialize(Type) != 0) {
            PyErr_SetString(PyExc_SystemError, "System error: failed to initialize type");
            return nullptr;
        }
        static PyObject *args = PyTuple_New(0);
        static PyObject *kwds = PyDict_New();
        auto *pyobj = (PythonPointerWrapperBase *) PyObject_Call((PyObject *) &Type, args, kwds);
        if (!pyobj) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to create Python Object");
            return nullptr;
        }
        assert(pyobj->get_CObject() == nullptr);
        //T * v=  ObjectLifecycleHelpers::Copy<T >::new_copy((typename extent_as_pointer<T>::type)cobj);
        pyobj->_byte_bucket = byte_bucket;
        if(cobj) {
            pyobj->set_CObject(cobj);
        } else if(byte_bucket){
          pyobj->set_CObject((T*)&byte_bucket);
        } else {
            PyErr_SetString(PyExc_TypeError, "Cannot make pointer reference to null");
            return nullptr;
        }
        if (arraySize > 0) { pyobj->_max = arraySize - 1; }
        if (referencing) pyobj->make_reference(referencing);
        return pyobj;
    }


    template<typename T>
    template<typename ...Args>
    PythonPointerWrapperBase <T> *
    PythonPointerWrapperBase<T>::
    createAllocatedInstance(PyTypeObject &Type,Args... args,  ssize_t arraySize) {
        //constexpr bool is_same_type[] = {std::is_same<typename std::remove_reference<Args>::type, T>::value...};
        if constexpr (!std::is_constructible<T_base, Args...>::value){
            PyErr_SetString(PyExc_TypeError, "Type is not constructible base on provided arguments");
            return nullptr;
        } else {
            if (ArraySize<T>::size > 0 && arraySize != -1) {
                PyErr_SetString(PyExc_SystemError, "Attempt to allocate array of fixed-sized-array types");
                return nullptr;
            } else if (ArraySize<T>::size > 0) {
                arraySize = ArraySize<T>::size;
            }

            if (_initialize(Type) != 0) {
                PyErr_SetString(PyExc_SystemError, "System error: failed to initialize type");
                return nullptr;
            }
            static PyObject *pyargs = PyTuple_New(0);
            static PyObject *kwds = PyDict_New();
            auto *pyobj = (PythonPointerWrapperBase *) PyObject_Call((PyObject *) &Type, pyargs, kwds);
            if (!pyobj) {
                PyErr_SetString(PyExc_RuntimeError, "Unable to create Python Object");
                return nullptr;
            }
            assert(pyobj->get_CObject() == nullptr);
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

            if (arraySize >= 0) {
                try {
                    typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
                    if constexpr (std::is_volatile<T>::value || std::is_volatile<T_base>::value){
                        PyErr_SetString(PyExc_TypeError, "Unable to allocate array of volatile instances");
                        return nullptr;
                    } else {
                        pyobj->set_CObject (new T());
                        auto result = Constructor<T_base>::template allocate_array<Args...>((size_t) arraySize,
                                                                                            args...);
                        *pyobj->get_CObject() = static_cast<T>(result);
                    }
                } catch (PyllarsException &e) {
                    e.raise();
                    return nullptr;
                } catch(std::exception const & e) {
                    PyllarsException::raise_internal_cpp(e.what());
                    return nullptr;
                } catch(...) {
                    PyllarsException::raise_internal_cpp();
                    return nullptr;
                }
            } else {
                pyobj->set_CObject(new typename extent_as_pointer<T>::type(nullptr));
                T_base *new_value = new T_base(args...);
                *pyobj->get_CObject() =  new_value;
            }
            if (arraySize > 0) { pyobj->_max = arraySize - 1; }
            return pyobj;
        }
    }


    template<typename T>
    void
    PythonPointerWrapperBase<T>::_free(void *self_) {
        PythonPointerWrapperBase* self = reinterpret_cast<PythonPointerWrapperBase*>(self_);
        if(self->_byte_bucket){
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
            if constexpr (std::is_destructible<T_base>::value && ArraySize<T_base>::size <= 0){
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
        PythonPointerWrapperBase *self;
        self = (PythonPointerWrapperBase *) type->tp_alloc(type, 0);
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

        p->max = self_->_max > 0 ? self_->_max : 1;
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
            /* Raising of standard StopIteration exception with empty _CObject. */
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
        pyobj->set_CObject = ((T*) &(this->get_CObject()));
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
            pyobj->_depth = self->_depth + 1;
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
    PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value == 1)>::type>::
    _addr(PyObject *self_, PyObject *) {
        auto *self = reinterpret_cast<PythonClassWrapper*>(self_);
        if (!self->get_CObject()) {
            PyErr_SetString(PyExc_RuntimeError, "Cannot take address of null pointer!");
            return nullptr;
        }
        try {
            auto *pyobj = self->createPyReferenceToAddr();//1, obj, ContainmentKind ::BY_REFERENCE, (PyObject *) self);
            pyobj->_depth = 2;
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
    PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value == 1)>::type>::
    createPyReferenceToAddr() {
        auto addrType = PythonClassWrapper<T*>::getPyType();

        static PyObject *args = PyTuple_New(0);
        static PyObject *kwds = PyDict_New();
        auto *pyobj = (PythonClassWrapper < T* > *)PyObject_Call((PyObject *) addrType, args, kwds);
        if (!pyobj) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to create Python Object");
            return nullptr;
        }
        assert(pyobj->get_CObject() == nullptr);
        pyobj->set_CObject(&reinterpret_cast<PythonClassWrapper*>(this)->get_CObject());
        pyobj->make_reference((PyObject*) this);
        return pyobj;
    }



}

#endif
