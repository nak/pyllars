
#ifndef __PYLLARS_INTERNAL__MEMBERSEMANTICS_IMPL_
#define __PYLLARS_INTERNAL__MEMBERSEMANTICS_IMPL_

#include "pyllars_membersemantics.hpp"
#include "pyllars_classwrapper.impl.hpp"
#include "pyllars_conversions.impl.hpp"

namespace __pyllars_internal {

    template<const char *const name, typename CClass, typename T>
    PyObject *MemberContainer<name, CClass, T>::
    get(PyObject *self, void*) {
        if (!self) return nullptr;
        typedef typename std::remove_reference<T>::type T_NoRef;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        if constexpr ( !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0) {
            const ssize_t base_size = ArrayHelper<T>::base_sizeof();
            const ssize_t array_size = base_size > 0 ? sizeof(_this->get_CObject()->*member) / base_size
                                                     : UNKNOWN_SIZE;
            if (_this->get_CObject()) {
                return toPyObject<T_NoRef&>(_this->get_CObject()->*member, array_size);
            }
            PyErr_SetString(PyExc_RuntimeError,
                            "Internal Pyllars Error: No C Object found to get member attribute _CObject!");
            return nullptr;
        } else if constexpr( !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0){
            const ssize_t base_size = ArrayHelper<T>::base_sizeof();
            const ssize_t array_size =
                    base_size > 0 ? sizeof(_this->get_CObject()->*member) / base_size
                                  : UNKNOWN_SIZE;
            if (_this->get_CObject()) {
                return toPyObject<T_NoRef&>(_this->get_CObject()->*member, array_size);
            }
            PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute _CObject!");
            return nullptr;
        } else if constexpr ( std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0){
            if (_this->get_CObject()) {
                const ssize_t base_size = ArrayHelper<T>::base_sizeof();
                const ssize_t array_size =
                        base_size > 0 ? sizeof(_this->get_CObject()->*member) / base_size
                                      : UNKNOWN_SIZE;
                return toPyObject<T_NoRef&>(_this->get_CObject()->*member, array_size);
            }
            PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute _CObject!");
            return nullptr;
        } else if constexpr(std::is_array<T>::value && ArraySize<T>::size > 0){
            const ssize_t size = ArraySize<T>::size;
            try {
                if (array_size != size) {
                    static char msg[250];
                    snprintf(msg, 250, "Mismatched array sizes (static)%lld!=%lld", (long long) array_size,
                             (long long) size);
                    PyErr_SetString(PyExc_TypeError, msg);
                    return nullptr;
                }
                if (_this->get_CObject()) {
                    const ssize_t base_size = ArrayHelper<T>::base_sizeof();
                    const ssize_t array_size =
                            base_size > 0 ? sizeof(_this->get_CObject()->*member) / base_size
                                          : UNKNOWN_SIZE;

                    PyObject* obj = toPyObject<T_NoRef&>(_this->get_CObject()->*member, array_size);
                    if(!obj){
                        PyErr_SetString(PyExc_RuntimeError, "Failed to convert cobject to python");
                        return nullptr;
                    }
                    ((PythonClassWrapper<T> *) obj)->make_reference(self);
                    return obj;
                }
                PyErr_SetString(PyExc_RuntimeError, "Internal Error:No C Object found to get member attribute _CObject!");
                return nullptr;
            } catch (const char *const msg) {
                PyErr_SetString(PyExc_RuntimeError, msg);
                return nullptr;
            }
        } else if constexpr (std::is_array<T>::value){
            if (_this->get_CObject()) {
                PyObject *obj = toPyObject<T_NoRef&>(*(_this->get_CObject()->*member), array_size);
                if(!obj){
                    PyErr_SetString(PyExc_RuntimeError, "Failed to convert cobject to python");
                    return nullptr;
                }
                ((PythonClassWrapper<T> *) obj)->make_reference(self);
                return obj;
            }
            PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute _CObject!");
            return nullptr;
        }
    }


    template<const char *const name, typename CClass, typename T>
    int MemberContainer<name, CClass, T>::
    set(PyObject *self, PyObject* pyVal, void*) {
        if (!self) return -1;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        if constexpr ( !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0) {
            if (!self) {
                PyErr_SetString(PyExc_RuntimeError, "Unexpceted nullptr _CObject for self");
                return -1;
            }
            if (pyVal == Py_None) {
                PyErr_SetString(PyExc_ValueError, "Unexpected None _CObject in member setter");
                return -1;
            }
            try {
                Assignment<T>::assign((_this->get_CObject()->*member), toCArgument<T>(*pyVal).value());
            } catch (const char *const msg) {
                PyErr_SetString(PyExc_RuntimeError, msg);
                return -1;
            }
            return 0;
        } else if constexpr ( !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0){
            return 0;
        } else if constexpr ( std::is_const<T>::value ){
            throw "Cannot set const element";
        } else if constexpr (std::is_array<T>::value && ArraySize<T>::size > 0){
            static constexpr ssize_t size = ArraySize<T>::size;
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
            try{
                if (pyVal == Py_None) {
                    PyErr_SetString(PyExc_ValueError, "Unexpcted None _CObject in member setter");
                    return -1;
                }
                if (PyTuple_Check(pyVal)) {
                    if (PyTuple_Size(pyVal) == size) {
                        for (size_t i = 0; i < size; ++i) {
                            auto value =  toCArgument<T_base>(*PyTuple_GetItem(pyVal, i));
                            Assignment<T_base>::assign((_this->get_CObject()->*member)[i], value.value());
                        }
                        return 0;
                    } else {
                        static char msg[250];
                        snprintf(msg, 250, "Mismatched array sizes (tuple)%lld!=%lld",
                                 (long long) PyTuple_Size(pyVal),
                                 (long long) size);
                        PyErr_SetString(PyExc_IndexError, msg);
                        return -1;
                    }
                } else if (PythonClassWrapper<T>::checkType(pyVal)) {
                    T *val = reinterpret_cast<PythonClassWrapper<T> *>(pyVal)->get_CObject();
                    Assignment<T>::assign(_this->get_CObject()->*member, *val);
                    return 0;
                } else if (PythonClassWrapper<typename extent_as_pointer<T>::type>::checkType(pyVal)) {
                    T_base **val = reinterpret_cast<PythonClassWrapper<typename extent_as_pointer<T>::type> *>(pyVal)->get_CObject();
                    T_base * toVal = _this->get_CObject()->*member;
                    Assignment<T_base*>::assign(toVal, *val, ArraySize<T>::size );
                    return 0;
                }
            } catch (const char *const msg) {
                PyErr_SetString(PyExc_RuntimeError, msg);
                return -1;
            }
            PyErr_SetString(PyExc_TypeError, "Unknown or incompatible type in setting member of class");
            return -1;
        } else if constexpr (std::is_array<T>::value){
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
            try{
                if (pyVal == Py_None) {
                    PyErr_SetString(PyExc_RuntimeError, "Unexpcted None _CObject in member setter");
                    return -1;
                }
                if (array_size == 0) {
                    PyErr_SetString(PyExc_RuntimeError, "Attempt to set array elements on array of unknown size.");
                    return -1;
                }
                if (PyTuple_Check(pyVal)) {
                    if (PyTuple_Size(pyVal) == array_size) {
                        for (size_t i = 0; i < array_size; ++i)
                            Assignment<T>::assign((_this->get_CObject()->*
                                               member)[i], *toCArgument<T>(
                                    *PyTuple_GetItem(pyVal, i)));
                        return 0;
                    } else {
                        static char msg[250];
                        snprintf(msg, 250, "Mismatched array sizes (tuple)%lld!=%lld",
                                 (long long) PyTuple_Size(pyVal),
                                 (long long) array_size);
                        PyErr_SetString(PyExc_IndexError, msg);
                        return -1;
                    }
                } else if (PythonClassWrapper<T>::checkType(pyVal)) {
                    T *val = ((PythonClassWrapper<T> *) pyVal)->get_CObject();
                    //TODO: check size????
                    for (size_t i = 0; i < array_size; ++i)
                        Assignment<T>::assign((_this->get_CObject()->*member)[i], (*val)[i]);
                    return 0;
                }  else if (PythonClassWrapper<typename extent_as_pointer<T>::type>::checkType(pyVal)) {
                    T_base **val = reinterpret_cast<PythonClassWrapper<typename extent_as_pointer<T>::type> *>(pyVal)->get_CObject();
                    T_base * toVal = _this->get_CObject()->*member;
                    Assignment<T_base*>::assign(toVal, *val, ArraySize<T>::size );
                    return 0;
                } else {
                    PyErr_SetString(PyExc_ValueError, "Invalid argument type when setting attribute");
                    return -1;
                }
            } catch( const char* const msg){
                PyErr_SetString(PyExc_RuntimeError, msg);
                return -1;
            }
            PyErr_SetString(PyExc_TypeError, "Unknown or incompatible type in setting member of class");
            return -1;
        }
    }

    template<const char *const name, typename CClass, typename T>
    void MemberContainer<name, CClass, T>::
    setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
        if constexpr ( !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0) {
            Assignment<T>::assign(self->*member, *toCArgument<T>(*pyobj));
        } else if constexpr(  !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0){

        } else if constexpr( std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0){
            PyErr_SetString(PyExc_RuntimeError, "Attempt to set constant field");
            throw "Attempt to set constant field";
        } else if constexpr (std::is_array<T>::value && ArraySize<T>::size > 0){
            static constexpr ssize_t size = ArraySize<T>::size;
            argument_capture<T> val = toCArgument<T >(*pyobj);
            for (size_t i = 0; i < size; ++i) {
                (self->*member)[i] = (val.value())[i];
            }
        } else if constexpr (std::is_array<T>::value){
            throw "Cannot set _CObject of type of unknown array length";
        }
    }



    template<const char *const name, typename CClass, typename T>
    typename MemberContainer<name, CClass, T>::member_t MemberContainer<name, CClass, T>::member;


    template<const char *const name, typename CClass, typename T>
    size_t MemberContainer<name, CClass, T>::array_size = 0;


    ////////////////////////////


    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    PyObject *BitFieldContainer<CClass>::Container<name, T, bits>::
    get(PyObject *self, void*) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        typedef typename std::remove_reference<T>::type T_NoRef;
        Wrapper *_this = (Wrapper *) self;

        if (_this->get_CObject()) {
            return toPyObject<T_NoRef>(_getter(*(_this->get_CObject())), 1);
        }
        PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute _CObject!");
        return nullptr;
    }

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    int BitFieldContainer<CClass>::Container<name, T, bits>::
    set(PyObject *self, PyObject* pyVal, void*) {
        if (!self) {
            PyErr_SetString(PyExc_RuntimeError, "Unexpected null _CObject for self");
            return -1;
        }
        if(!pyVal){
            PyErr_SetString(PyExc_TypeError, "Null reference encountered on setting _CObject");
            return -1;
        }
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;
        if (pyVal == Py_None) {
            PyErr_SetString(PyExc_ValueError, "Unexpected None _CObject in member setter");
            return -1;
        }
        auto value = toCArgument<T>(*pyVal);
        if (!BitFieldLimits<T, bits>::is_in_bounds(value.value())) {
            PyErr_SetString(PyExc_ValueError, "Value out of bounds");
            return -1;
        }
        _setter(*(_this->get_CObject()), value.value());
        return 0;
    }

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    void BitFieldContainer<CClass>::Container<name, T, bits>::
    setFromPyObject(CClass_NoRef *self, PyObject *pyobj) {
        argument_capture<T> value = toCArgument<T>(*pyobj);
        if (!BitFieldLimits<T, bits>::is_in_bounds(value.value())) {
            throw "Value out of bounds";
        }
        _setter(*self, value.value());
    }

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    PyObject *BitFieldContainer<CClass>::ConstContainer<name, T, bits>::
    get(PyObject *self, void*) {
        if (!self) return nullptr;
        typedef PythonClassWrapper<CClass> Wrapper;
        Wrapper *_this = (Wrapper *) self;

        if (_this->get_CObject()) {
            return toPyObject<T>(_getter(*(_this->get_CObject())), 1);
        }
        PyErr_SetString(PyExc_RuntimeError, "Internal Error: No C Object found to get member attribute _CObject!");
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