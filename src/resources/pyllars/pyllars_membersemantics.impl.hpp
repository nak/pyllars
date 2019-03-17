
#ifndef __PYLLARS_INTERNAL__MEMBERSEMANTICS_IMPL_
#define __PYLLARS_INTERNAL__MEMBERSEMANTICS_IMPL_

#include "pyllars_membersemantics.hpp"
#include "pyllars_classwrapper.impl.hpp"

namespace __pyllars_internal {

    template<class CClass>
    template<const char *const name, typename T>
    PyObject *MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::
    get(PyObject *self, void*) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        const ssize_t base_size = ArrayHelper<T>::base_sizeof();
        const ssize_t array_size = base_size > 0 ? sizeof(_this->get_CObject()->*member) / base_size
                                                 : UNKNOWN_SIZE;
        if (_this->get_CObject()) {
            return toPyObject<T>(_this->get_CObject()->*member, true, array_size);
        }
        PyErr_SetString(PyExc_RuntimeError, "Internal Pyllars Error: No C Object found to get member attribute value!");
        return nullptr;
    }


    template<class CClass>
    template<const char *const name, typename T>
    int MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::
    set(PyObject *self, PyObject* pyVal, void*) {
        if (!self) {
            PyErr_SetString(PyExc_RuntimeError, "Unexpceted nullptr value for self");
            return -1;
        }
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        if (pyVal == Py_None) {
            PyErr_SetString(PyExc_ValueError, "Unexpected None value in member setter");
            return -1;
        }
        try {
            Assign<T>::assign((_this->get_CObject()->*member),
                              *toCArgument<T, false, PythonClassWrapper<T> >(*pyVal));
        }catch(const char* const msg){
            PyErr_SetString(PyExc_RuntimeError, msg);
            return -1;
        }
        return 0;
    }

    template<class CClass>
    template<const char *const name, typename T>
    void MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::
    setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
        Assign<T>::assign(self->*member , *toCArgument<T, false, PythonClassWrapper<T> >(*pyobj));
    }

    template<class CClass>
    template<const char *const name, typename T>
    PyObject *MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0>::type>::
    get(PyObject *self, void*) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        const ssize_t base_size = ArrayHelper<T>::base_sizeof();
        const ssize_t array_size =
                base_size > 0 ? sizeof(_this->get_CObject()->*member) / base_size
                              : UNKNOWN_SIZE;
        if (_this->get_CObject()) {
            return toPyObject<T>(_this->get_CObject()->*member, false, array_size);
        }
        PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
        return nullptr;
    }

    template<class CClass>
    template<const char *const name, typename T>
    int MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0>::type>::
    set(PyObject *self, PyObject *pyVal, void*) {
       //nothing to do for zero size item
        return 0;

    }

    template<class CClass>
    template<const char *const name, typename T>
    void MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0>::type>::
    setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
        (void) self;
        (void) pyobj;
    }

    template<class CClass>
    template<const char *const name, typename T>
    PyObject *MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::
    get(PyObject *self, void* v) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        if (_this->get_CObject()) {
            const ssize_t base_size = ArrayHelper<T>::base_sizeof();
            const ssize_t array_size =
                    base_size > 0 ? sizeof(_this->get_CObject()->*member) / base_size
                                  : UNKNOWN_SIZE;
            return toPyObject<T>(_this->get_CObject()->*member, true, array_size);
        }
        PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
        return nullptr;
    }

    template<class CClass>
    template<const char *const name, typename T>
    int MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::
    set(PyObject *self, PyObject *pyVal, void*) {
        throw "Cannot set constant field";
    }

    template<class CClass>
    template<const char *const name, typename T>
    void MemberContainer<CClass>::Container<name, T, typename std::enable_if<
            std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::
    setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
        PyErr_SetString(PyExc_RuntimeError, "Attempt to set constant field");
        throw "Attempt to set constant field";
    }

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    PyObject *MemberContainer<CClass>::Container<name, T[size], void>::
    get(PyObject *self, void*) {
        try {
            if (!self) return nullptr;
            PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;

            if (array_size != size) {
                static char msg[250];
                snprintf(msg, 250, "Mismatched array sizes (static)%lld!=%lld", (long long) array_size,
                         (long long) size);
                PyErr_SetString(PyExc_TypeError, msg);
                return nullptr;
            }
            if (_this->get_CObject()) {
                const ssize_t base_size = ArrayHelper<T_array>::base_sizeof();
                const ssize_t array_size =
                        base_size > 0 ? sizeof(_this->get_CObject()->*member) / base_size
                                      : UNKNOWN_SIZE;

               PyObject* obj = toPyObject<T_array>(_this->get_CObject()->*member, AS_REFERNCE,
                                            array_size);

                ((PythonClassWrapper<T_array> *) obj)->make_reference(self);
                return obj;
            }
            PyErr_SetString(PyExc_RuntimeError, "Internal Error:No C Object found to get member attribute value!");
            return nullptr;
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    int MemberContainer<CClass>::Container<name, T[size], void>::
    set(PyObject *self, PyObject* pyVal, void*) {
        PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;
        try{
            if (pyVal == Py_None) {
                PyErr_SetString(PyExc_ValueError, "Unexpcted None value in member setter");
                return -1;
            }
            if (PyTuple_Check(pyVal)) {
                if (PyTuple_Size(pyVal) == size) {
                    for (size_t i = 0; i < size; ++i)
                        Assign<T>::assign((_this->get_CObject()->*
                         member)[i],  *toCArgument<T, true>(
                                *PyTuple_GetItem(pyVal, i)));
                } else {
                    static char msg[250];
                    snprintf(msg, 250, "Mismatched array sizes (tuple)%lld!=%lld",
                             (long long) PyTuple_Size(pyVal),
                             (long long) size);
                    PyErr_SetString(PyExc_IndexError, msg);
                    return -1;
                }
            } else if (PythonClassWrapper<T_array>::checkType(pyVal)) {
                T_array *val = ((PythonClassWrapper<T_array> *) pyVal)->get_CObject();
                for (size_t i = 0; i < size; ++i)
                    Assign<T>::assign((_this->get_CObject()->*member)[i] , (*val)[i]);

            }
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return -1;
        }
        return 0;
    }

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    void MemberContainer<CClass>::Container<name, T[size], void>::
    setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
        smart_ptr<T[size], false> val = toCArgument<T[size], false, PythonClassWrapper<T[size]> >(
                *pyobj);
        for (size_t i = 0; i < size; ++i) {
            (self->*member)[i] = (*val)[i];
        }
    }


    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    PyObject *MemberContainer<CClass>::Container<name, const T[size], void>::
    get(PyObject *self, void*) {
        if (!self) return nullptr;
        PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;

        const ssize_t base_size = ArrayHelper<T_array>::base_sizeof();
        const ssize_t array_size =
                base_size > 0 ? sizeof(_this->get_CObject()->*member) / base_size
                              : UNKNOWN_SIZE;
        if (array_size != size) {
            PyErr_SetString(PyExc_TypeError, "Mismatched array sizes");
            return nullptr;
        }
        if (_this->get_CObject()) {
            return toPyObject<T_array, size>(*(_this->get_CObject()->*member), true,
                                             array_size);
        }

        PyErr_SetString(PyExc_RuntimeError, "Cannot set object element!");
        return nullptr;

    }

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    void MemberContainer<CClass>::Container<name, const T[size], void>::
    setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
        smart_ptr<T[size], false> val = toCArgument<T[size], false, PythonClassWrapper<T[size]> >(
                *pyobj);
        for (size_t i = 0; i < size; ++i) {
            (self->*member)[i] = (*val)[i];
        }
    }


    template<class CClass>
    template<const char *const name, typename T>
    PyObject *MemberContainer<CClass>::Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type>::
    get(PyObject *self, void*) {
        if (!self) return nullptr;
        PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;

        if (_this->get_CObject()) {
            PyObject *obj = toPyObject<T[]>(*(_this->get_CObject()->*member), true, array_size);
            ((PythonClassWrapper<T> *) obj)->make_reference(self);
            return obj;
        }
        PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
        return nullptr;
    }


    template<class CClass>
    template<const char *const name, typename T>
    int MemberContainer<CClass>::Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type>::
    set(PyObject *self, PyObject* pyVal, void*) {
        if (!self) return -1;
        PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;
        try{
            if (pyVal == Py_None) {
                PyErr_SetString(PyExc_RuntimeError, "Unexpcted None value in member setter");
                return -1;
            }
            if (array_size == 0) {
                PyErr_SetString(PyExc_RuntimeError, "Attempt to set array elements on array of unknown size.");
                return -1;
            }
            if (PyTuple_Check(pyVal)) {
                if (PyTuple_Size(pyVal) == array_size) {
                    for (size_t i = 0; i < array_size; ++i)
                        Assign<T>::assign((_this->get_CObject()->*
                                           member)[i], *toCArgument<T, false, PythonClassWrapper<T> >(
                                *PyTuple_GetItem(pyVal, i)));
                } else {
                    static char msg[250];
                    snprintf(msg, 250, "Mismatched array sizes (tuple)%lld!=%lld",
                             (long long) PyTuple_Size(pyVal),
                             (long long) array_size);
                    PyErr_SetString(PyExc_IndexError, msg);
                    return -1;
                }
            } else if (PythonClassWrapper<T_array>::checkType(pyVal)) {
                T_array *val = ((PythonClassWrapper<T_array> *) pyVal)->get_CObject();
                //TODO: check size????
                for (size_t i = 0; i < array_size; ++i)
                    Assign<T>::assign((_this->get_CObject()->*member)[i], (*val)[i]);

            } else {
                PyErr_SetString(PyExc_ValueError, "Invalid argument type when setting attribute");
                return -1;
            }
        } catch( const char* const msg){
            PyErr_SetString(PyExc_RuntimeError, msg);
            return -1;
        }
        return 0;
    }

    template<class CClass>
    template<const char *const name, typename T>
    void MemberContainer<CClass>::Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type>::
    setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
        smart_ptr<T[], false> val = toCArgument<T[], false, PythonClassWrapper<T[]> >(*pyobj);
        for (size_t i = 0; i < array_size; ++i) {
            (self->*member)[i] = (*val)[i];
        }
    }

    template<class CClass>
    template<const char *const name, typename T>
    typename MemberContainer<CClass>::template Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0>::type>::member_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0>::type>::member;

    template<class CClass>
    template<const char *const name, typename T>
    typename MemberContainer<CClass>::template Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::member_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::member;

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    typename MemberContainer<CClass>::template Container<name, T[size], void>::member_t
            MemberContainer<CClass>::Container<name, T[size], void>::member;

    template<class CClass>
    template<const char *const name, typename T>
    typename MemberContainer<CClass>::template Container<name, T, typename std::enable_if<
            std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::member_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::member;

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    typename MemberContainer<CClass>::template Container<name, const T[size], void>::member_t
            MemberContainer<CClass>::Container<name, const T[size], void>::member;

    template<class CClass>
    template<const char *const name, typename T>
    typename MemberContainer<CClass>::template Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type>::member_t
            MemberContainer<CClass>::Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type>::member;

    template<class CClass>
    template<const char *const name, typename T>
    typename MemberContainer<CClass>::template Container<name, T[], typename std::enable_if<std::is_const<T>::value>::type>::member_t
            MemberContainer<CClass>::Container<name, T[], typename std::enable_if<std::is_const<T>::value>::type>::member;


    template<class CClass>
    template<const char *const name, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    !std::is_const<T>::value && !std::is_array<T>::value &&
                    Sizeof<T>::value == 0>::type>::array_size = 0;

    template<class CClass>
    template<const char *const name, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    !std::is_const<T>::value && !std::is_array<T>::value &&
                    Sizeof<T>::value != 0>::type>::array_size = 0;

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T[size], void>::array_size = 0;

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    size_t
            MemberContainer<CClass>::Container<name, const T[size], void>::array_size = 0;

    template<class CClass>
    template<const char *const name, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    std::is_const<T>::value && !std::is_array<T>::value &&
                    Sizeof<T>::value != 0>::type>::array_size = 0;

    template<class CClass>
    template<const char *const name, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type>::array_size = 0;

    template<class CClass>
    template<const char *const name, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T[], typename std::enable_if<std::is_const<T>::value>::type>::array_size = 0;


    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    PyObject *BitFieldContainer<CClass>::Container<name, T, bits>::
    get(PyObject *self, void*) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;

        if (_this->get_CObject()) {
            return toPyObject<T>(_getter(*(_this->get_CObject())), false, 1);
        }
        PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
        return nullptr;
    }

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    int BitFieldContainer<CClass>::Container<name, T, bits>::
    set(PyObject *self, PyObject* pyVal, void*) {
        if (!self) {
            PyErr_SetString(PyExc_RuntimeError, "Unexpected null value for self");
            return -1;
        }
        if(!pyVal){
            PyErr_SetString(PyExc_TypeError, "Null reference encountered on setting value");
            return -1;
        }
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        if (pyVal == Py_None) {
            PyErr_SetString(PyExc_ValueError, "Unexpected None value in member setter");
            return -1;
        }
        smart_ptr<T, false> value = toCArgument<T, false>(*pyVal);
        if (!BitFieldLimits<T, bits>::is_in_bounds(*value)) {
            PyErr_SetString(PyExc_ValueError, "Value out of bounds");
            return -1;
        }
        _setter(*(_this->get_CObject()), *value);
        return 0;
    }

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    void BitFieldContainer<CClass>::Container<name, T, bits>::
    setFromPyObject(CClass_NoRef *self, PyObject *pyobj) {
        smart_ptr<T, false> value = toCArgument<T, false, PythonClassWrapper<T> >(*pyobj);
        if (!BitFieldLimits<T, bits>::is_in_bounds(*value)) {
            throw "Value out of bounds";
        }
        _setter(*self, *value);
    }

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    PyObject *BitFieldContainer<CClass>::ConstContainer<name, T, bits>::
    get(PyObject *self, void*) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;

        if (_this->get_CObject()) {
            return toPyObject<T>(_getter(*(_this->get_CObject())), false, 1);
        }
        PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute value!");
        return nullptr;
    }

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    void BitFieldContainer<CClass>::ConstContainer<name, T, bits>::
    setFromPyObject(CClass_NoRef *self, PyObject *pyobj) {
        (void) self;
        (void) pyobj;
        throw "Cannot set const bit field";
    }


    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    typename BitFieldContainer<CClass>::template Container<name, T, bits>::getter_t BitFieldContainer<CClass>::Container<name, T, bits>::_getter;

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    typename BitFieldContainer<CClass>::template Container<name, T, bits>::setter_t BitFieldContainer<CClass>::Container<name, T, bits>::_setter;

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    typename BitFieldContainer<CClass>::template ConstContainer<name, T, bits>::getter_t BitFieldContainer<CClass>::ConstContainer<name, T, bits>::_getter;



}

#endif