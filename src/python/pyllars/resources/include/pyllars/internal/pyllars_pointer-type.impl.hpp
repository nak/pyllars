//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__POINTER_TYPE_CPP_
#define __PYLLARS_INTERNAL__POINTER_TYPE_CPP_

#include "pyllars/pyllars.hpp"
#include "pyllars_pointer.hpp"
//#include "pyllars/internal/pyllars_floating_point.hpp"
//#include "pyllars/internal/pyllars_integer.hpp"
#include "pyllars_classwrapper.hpp"
#include "pyllars_type_traits.hpp"
#include "pyllars_reference.hpp"


#if PY_MAJOR_VERSION >= 3
#  define Py_TPFLAGS_HAVE_ITER 0
#endif

namespace pyllars_internal {

    template<typename T>
    int
    PythonPointerWrapperBase<T>::_initialize(PyTypeObject &Type) {
        static bool initialized = false;
        if (initialized) return 0;
        initialized = true;
        assert(Type.tp_basicsize > 0);
        Type.tp_dealloc =  (destructor) &_dealloc;
        Type.tp_as_sequence = &_seqmethods;
        Type.tp_flags =  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE|  Py_TPFLAGS_HAVE_ITER;
        Type.tp_iter = Iter::iter;
        Type.tp_iternext = Iter::iter;
        assert(Type.tp_methods);
        Type.tp_new = _new;

        if (PyType_Ready(CommonBaseWrapper::getRawType()) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
            return -1;
        }
        if (PyType_Ready(&Type) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
            return -1;
        }
        _seqmethods.sq_length = _size;
        _seqmethods.sq_concat = _concat;
        _seqmethods.sq_repeat = _repeat;
        _seqmethods.sq_item = _get_item;
        _seqmethods.sq_ass_item = _set_item;
        _seqmethods.sq_contains = _contains;
        _seqmethods.sq_inplace_concat = _inplace_concat;
        _seqmethods.sq_inplace_repeat = _inplace_repeat;

        Type.tp_as_sequence = &_seqmethods;

        Py_INCREF(&Type);
        return 0;
    }


    template<typename T>
    int
    PythonPointerWrapperBase<T>::
    _initbase(PythonPointerWrapperBase *self, PyObject *args, PyObject *kwds, PyTypeObject *pytypeobj) {
        if (!self) return -1;
        self->_referenced = nullptr;
        self->_max = last;
        self->_directlyAllocated = false;
        if (kwds && PyDict_Size(kwds) > 1) {
            PyErr_SetString(PyExc_TypeError, "Unexpected keyword argument(s) in Pointer cosntructor");
            return -1;
        } else if (kwds && PyDict_Size(kwds) == 1) {
            PyObject *sizeItem;
            if ((sizeItem = PyDict_GetItemString(kwds, "size"))) {
                if (last > 0) {
                    PyErr_SetString(PyExc_RuntimeError, "Attempt to dynamically set size on fixed-size array");
                    return -1;
                }
                if (PyLong_Check(sizeItem)) {
                    //if size arg is truly an integer and is positive or zero, set the property here (and if not a fixed-size array)
                    self->_max = (ssize_t)(PyLong_AsLongLong(sizeItem) - 1);
                    if (self->_max < 0) {
                        PyErr_SetString(PyExc_TypeError, "Invalid negative size _CObject in Pointer constructor");
                        return -1;
                    }
                } else {
                    PyErr_SetString(PyExc_TypeError,
                                    "Invalid type for size keyword argument in Pointer constructor");
                    return -1;
                }
            } else {
                PyErr_SetString(PyExc_TypeError, "Unexpected keyword argument in Pointer constructor");
                return -1;
            }
        }

        const bool have_args = args != NULL_ARGS();

        //if have an argument, set pointer _CObject, otherwise set to nullptr
        if (have_args && PyTuple_Size(args) > 1) {
            PyErr_SetString(PyExc_TypeError, "Excpect only one object in Pointer constructor");
            return-1;
        } else if (have_args && PyTuple_Size(args) == 1) {
            // we are asked to make a new pointer from an existing object:
            PyObject *pyobj = PyTuple_GetItem(args, 0);
            if (!pyobj) {
                self->set_CObject(nullptr);
                return 0;
            }
            if (PyObject_TypeCheck(pyobj, pytypeobj)) {
                // if this is an instance of a basic class:
                self->set_CObject(reinterpret_cast<PythonPointerWrapperBase *>(pyobj)->get_CObject());
                self->make_reference(pyobj);
            } else if (PyUnicode_Check(pyobj) && (is_bytes_like<T>::value || is_c_string_like<T>::value)) {
                if constexpr (std::is_same<T, const char*>::value || std::is_same<T, const char* const>::value) {
                    self->make_reference(pyobj);
                    self->set_CObject(new T(PyUnicode_AsUTF8(pyobj)));
                } else {
                    PyErr_SetString(PyExc_TypeError, "Mismatched types when assigning pointer");
                    return ERROR_TYPE_MISMATCH;
                }
            } else {
                PyErr_SetString(PyExc_TypeError, "Mismatched types when assigning pointer");
                return ERROR_TYPE_MISMATCH;
            }
            return self->get_CObject() ? 0 : -1;
        } else {
            self->set_CObject(nullptr);
            return 0;
        }
    }

    template<typename T>
    PySequenceMethods PythonPointerWrapperBase<T>::_seqmethods;


    ///////////////////////////


    template<typename T>
    int
    PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value > 1)>::type>::
    _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
        if (!self) { return -1; }

        self->_referenced = nullptr;
        PyTypeObject *const coreTypePtr = PythonClassWrapper<typename core_type<T>::type>::getPyType();
        self->template populate_type_info<T>(&Base::checkType, coreTypePtr);
        int result = Base::_initbase(self, args, kwds, Base::getRawType());

        if (result == ERROR_TYPE_MISMATCH && (ptr_depth<T>::value == 1) &&
            (PythonClassWrapper<const char *>::checkType((PyObject *) self) ||
             PythonClassWrapper<const char *const>::checkType((PyObject *) self)
            )) {
            if constexpr (std::is_same<T, const char*>::value || std::is_same<T, const char* const>::value) {
                PyObject *arg = PyTuple_GetItem(args, 0);
                const char *s = fromPyStringLike<const char>(arg);
                if (!s) {
                    PyErr_SetString(PyExc_TypeError, "Cannot convert argument to const char* type");
                    return -1;
                }
                self->make_reference(arg);
                self->set_CObject(s);
            }
            PyErr_Clear();
            return 0;
        }
        return result;
    }

    template<typename T>
    PyMethodDef
    PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value > 1) >::type>::
    _methods[] ={{address_name, (PyCFunction) PythonClassWrapper::_addr, METH_KEYWORDS | METH_VARARGS, nullptr},
             {"at",         (PyCFunction) Base::_at,                 METH_KEYWORDS | METH_VARARGS, nullptr},
             {nullptr,      nullptr, 0, nullptr} /*sentinel*/
    };


    template<typename T>
    PyMethodDef
    PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value == 1)>::type>::
    _methods[] = {{address_name, (PyCFunction) PythonClassWrapper::_addr, METH_KEYWORDS | METH_VARARGS, nullptr},
             {"at",         (PyCFunction) Base::_at,                 METH_KEYWORDS | METH_VARARGS, nullptr},
             {nullptr,      nullptr, 0, nullptr} /*sentinel*/
            };


////////////////////////////////

    template<typename T>
    int
    PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value == 1)>::type>::
    _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds) {


        self->compare = [](CommonBaseWrapper* self_, CommonBaseWrapper* other)->bool{
            return PyObject_TypeCheck(other, Base::getPyType()) &&
                   (*reinterpret_cast<PythonClassWrapper *>(self_)->get_CObject() ==
                    *reinterpret_cast<PythonClassWrapper *>(other)->get_CObject());
        };

        self->hash = [](CommonBaseWrapper* self)->size_t{
            typedef typename extent_as_pointer<typename std::remove_cv<T>::type>::type T_bare;
            static std::hash<T_bare> hasher;

            return hasher((T_bare) *((PythonClassWrapper*)self)->get_CObject());
        };

        if (!self) { return -1; }
        self->_referenced = nullptr;
        PyTypeObject *const coreTypePtr = PythonClassWrapper<typename core_type<T>::type>::getPyType();
        self->template populate_type_info<T>(&Base::checkType, coreTypePtr);

        int result = Base::_initbase(self, args, kwds, Base::getPyType());

        if (result == ERROR_TYPE_MISMATCH && (ptr_depth<T>::value == 1) &&
            (PythonClassWrapper<const char *>::checkType((PyObject *) self) ||
             PythonClassWrapper<const char *const>::checkType((PyObject *) self))) {
            if constexpr (std::is_same<T, const char*>::value || std::is_same<T, const char* const>::value) {
                PyObject *arg = PyTuple_GetItem(args, 0);
                const char *s = fromPyStringLike<const char>(arg);
                if (!s) {
                    PyErr_SetString(PyExc_TypeError, "Cannot convert argument to const char* type");
                    return -1;
                }
                self->make_reference(arg);
                self->set_CObject(new T(s));
            }
            PyErr_Clear();
            return 0;

        }
        return result;
    }


    template<typename T>
    const std::string
    PythonPointerWrapperBase<T>::Iter::name = std::string(type_name<T>()) + std::string(" iterator");

    template<typename T>
    PyTypeObject
    PythonPointerWrapperBase<T>::Iter::_Type = {
    #if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(nullptr, 0)
    #else
    PyObject_HEAD_INIT(nullptr)
        0,                         /*ob_size*/
    #endif
            name.c_str(),             /*tp_name*/  //set on call to initialize
            sizeof(Iter) + 8,             /*tp_basicsize*/
            0,                         /*tp_itemsize*/
            nullptr, /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            nullptr,                         /*tp_getattr*/
            nullptr,                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            nullptr,                         /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            nullptr,                         /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_ITER, /*tp_flags*/
            "PythonCPointerWrapper iterator object",           /* tp_doc */
            nullptr,                               /* tp_traverse */
            nullptr,                               /* tp_clear */
            nullptr,                               /* tp_richcompare */
            0,                             /* tp_weaklistoffset */
            iter,                               /* tp_iter */
            iternext,                               /* tp_iternext */
            nullptr,             /* tp_methods */
            nullptr,             /* tp_members */
            nullptr,                         /* tp_getset */
            nullptr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            nullptr,      /* tp_init */
            nullptr,                         /* tp_alloc */
            PyType_GenericNew,                 /* tp_new */
            nullptr,                         /*tp_free*/ // no freeing of fixed-array
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_base*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };

    namespace {

        template<typename T, typename ...Other>
        void for_each_init_ptr() {
            int unused[] = {(CommonBaseWrapper::addCast<T, Other>(
                    PythonClassWrapper<T>::getRawType(),
                    PythonClassWrapper<Other>::getRawType(),
                    &CommonBaseWrapper::template interpret_cast<T, Other>), 0)...};
            (void)unused;
        }
    }

    template <typename  T>
    void PythonPointerWrapperBase<T>::_initAddCArgCasts(){
        static_assert(is_pointer_like<T>::value);
        typedef std::remove_cv_t <T> T_bare;

        if constexpr (!std::is_const<T>::value && !std::is_volatile<T>::value) {
            for_each_init_ptr<T, T_bare &, const T_bare &, const T_bare>();
            for_each_init_ptr<T&, T_bare &, const T_bare &, T_bare &&, const T_bare &&, const T_bare>();
            for_each_init_ptr<T&&, T_bare &, const T_bare &, T_bare &&, const T_bare &&, const T_bare>();
        } else if (std::is_const<T>::value && !std::is_volatile<T>::value){
            for_each_init_ptr<T, const T_bare &, const T_bare &&, T_bare , const T_bare>();
            for_each_init_ptr<T&, const T_bare &, const T_bare &&, T_bare , const T_bare>();
            for_each_init_ptr<T&&, const T_bare &, const T_bare &&, T_bare , const T_bare>();
        } else if (!std::is_const<T>::value && std::is_volatile<T>::value){
            for_each_init_ptr<T, volatile T_bare &, const volatile T_bare &, volatile T_bare &&, const volatile T_bare &&, const volatile T_bare>();
            for_each_init_ptr<T&, volatile T_bare &, const volatile T_bare &,volatile T_bare &&, const volatile T_bare &&, const volatile T_bare>();
            for_each_init_ptr<T&&, volatile T_bare &, const volatile T_bare &,volatile T_bare &&, const volatile T_bare &&, const volatile T_bare>();
        } else {
            for_each_init_ptr<T, const volatile T_bare &,const volatile T_bare &&, volatile T_bare , const volatile T_bare>();
            for_each_init_ptr<T&, const volatile T_bare &,  const volatile T_bare &&, volatile T_bare , const volatile T_bare>();
            for_each_init_ptr<T&&, const volatile T_bare &,  const volatile T_bare &&, volatile T_bare , const volatile T_bare>();
        }

    }


}

#endif
