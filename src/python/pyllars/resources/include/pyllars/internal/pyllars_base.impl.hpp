//
// Created by jrusnak on 12/10/19.
//
#include "pyllars_base.hpp"

#ifndef PYLLARS_PYTHON_BASE_IMPL_HPP
#define PYLLARS_PYTHON_BASE_IMPL_HPP
namespace pyllars_internal {

    template<typename T>
    DLLEXPORT CommonBaseWrapper::TypedProxy
            PythonBaseWrapper<T>::_Type = CommonBaseWrapper::TypedProxy(new PyTypeObject{
#if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(nullptr)
    0,                         /*ob_size*/
#endif
            pyllars_internal::type_name<T>(), /*tp_name*/
            -1,                              /*tp_basicsize -- filled in by inheriting intialize call*/
            0,                               /*tp_itemsize*/
            nullptr,                         /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            nullptr,                         /*tp_getattr*/
            nullptr,                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            new PyNumberMethods{0},          /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            nullptr,                         /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES, /*tp_flags*/
            "PythonClassWrapper object",     /* tp_doc */
            nullptr,                         /* tp_traverse */
            nullptr,                         /* tp_clear */
            nullptr,                         /* tp_richcompare */
            0,                               /* tp_weaklistoffset */
            nullptr,                         /* tp_iter */
            nullptr,                         /* tp_iternext */
            nullptr,                         /* tp_methods */
            nullptr,                         /* tp_members */
            nullptr,                         /* tp_getset */
            nullptr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                               /* tp_dictoffset */
            nullptr,                        /* tp_init */
            nullptr,                         /* tp_alloc */
            nullptr,                         /* tp_new */
            nullptr,                         /*tp_free*/
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bases*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                         /*tp_weaklist*/
            nullptr,                         /*tp_del*/
            0,                               /*tp_version_tag*/
    });


    template<typename T>
    template<typename T_Noref>
    typename PythonBaseWrapper<T>::storage_type
    PythonBaseWrapper<T>::allocate(PyObject *args, PyObject *kwds, unsigned char *location) {
        for (auto const &[kwlist_, constructor] : _constructors()) {
            (void) kwlist_;
            try {
                auto obj = constructor(kwlist_, args, kwds, location);
                if (obj) {
                    return obj;
                }
            } catch (PyllarsException &) {
                //try next one
            } catch (std::exception const &e) {
                PyllarsException::raise_internal_cpp(e.what());
                return nullptr;
            } catch (...) {
                PyllarsException::raise_internal_cpp();
                return nullptr;
            }
            PyErr_Clear();
        }
        if ((PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 0) {
            PyErr_SetString(PyExc_ValueError, "Creation of null C object not allowed");
            return nullptr;
        } else {
            if (!PyErr_Occurred()) {
                PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
            }
            return nullptr;
        }

        PyErr_SetString(PyExc_TypeError, "Unknown type for init");
        return nullptr;
    }


    template<typename Class, typename Other>
    PyObject *
    CommonBaseWrapper::interpret_cast(PyObject *self) {
        auto self_ = (PythonBaseWrapper<Class> *) self;
        auto castWrapper = reinterpret_cast<PythonBaseWrapper<Other> *>(
                PyObject_Call((PyObject *) PythonBaseWrapper<Other>::getRawType(),
                              NULL_ARGS(), nullptr));
        typedef typename std::remove_reference_t<Other> Other_NoRef;
        if (castWrapper) {
            castWrapper->set_CObject(const_cast<Other_NoRef *>(self_->get_CObject()));
        }
        return (PyObject *) castWrapper;
    }

    template<typename T>
    bool
    CommonBaseWrapper::checkImplicitArgumentConversion(PyObject *obj) {
        static constexpr bool to_is_const = std::is_const<T>::value;
        static constexpr bool to_is_reference = std::is_reference<T>::value;
        typedef typename core_type<T>::type core_t;
        auto const self = reinterpret_cast<CommonBaseWrapper *>(obj);
        return (bool) PyObject_TypeCheck(obj, CommonBaseWrapper::getBaseType()) && // is truly wrapping a C object
               (self->_coreTypePtr == PythonBaseWrapper<core_t>::getRawType()) && //core type match
               (to_is_const || !to_is_reference || !self->_is_const); // logic for conversion-is-allowed
    }

    namespace {

        template<typename T, typename ...Other>
        void for_each_init_ptrs() {

            int unused[] = {(CommonBaseWrapper::addCast(
                    PythonBaseWrapper<T>::getRawType(),
                    PythonBaseWrapper<Other>::getRawType(),
                    &CommonBaseWrapper::template interpret_cast<T, Other>), 0)...};
            (void) unused;

        }
    }

    template<typename T>
    void PythonBaseWrapper<T>::_addReinterpretations() {
        if constexpr(!std::is_reference<T>::value) {
            typedef std::remove_cv_t<T> T_bare;
            if constexpr (!std::is_const<T>::value && !std::is_volatile<T>::value) {
                for_each_init_ptrs<T, T_bare &, const T_bare &, const T_bare>();
                for_each_init_ptrs<T &, T_bare &, const T_bare &, T_bare &&, const T_bare &&, const T_bare>();
                for_each_init_ptrs<T &&, T_bare &, const T_bare &, T_bare &&, const T_bare &&, const T_bare>();
            } else if (std::is_const<T>::value && !std::is_volatile<T>::value) {
                for_each_init_ptrs<T, const T_bare &, const T_bare &&, T_bare, const T_bare>();
                for_each_init_ptrs<T &, const T_bare &, const T_bare &&, T_bare, const T_bare>();
                for_each_init_ptrs<T &&, const T_bare &, const T_bare &&, T_bare, const T_bare>();
            } else if (!std::is_const<T>::value && std::is_volatile<T>::value) {
                for_each_init_ptrs<T, volatile T_bare &, const volatile T_bare &, volatile T_bare &&, const volatile T_bare &&, const volatile T_bare>();
                for_each_init_ptrs<T &, volatile T_bare &, const volatile T_bare &, volatile T_bare &&, const volatile T_bare &&, const volatile T_bare>();
                for_each_init_ptrs<T &&, volatile T_bare &, const volatile T_bare &, volatile T_bare &&, const volatile T_bare &&, const volatile T_bare>();
            } else {
                for_each_init_ptrs<T, const volatile T_bare &, const volatile T_bare &&, volatile T_bare, const volatile T_bare>();
                for_each_init_ptrs<T &, const volatile T_bare &, const volatile T_bare &&, volatile T_bare, const volatile T_bare>();
                for_each_init_ptrs<T &&, const volatile T_bare &, const volatile T_bare &&, volatile T_bare, const volatile T_bare>();
            }
        }
    }



    template<typename T>
    template<const char *const kwlist[], typename ...Args>
    void PythonBaseWrapper<T>::
    addConstructor() {
        _constructors().push_back(ConstructorContainer(kwlist,  &create < Args... >));
    }

    template<typename T>
    template<typename ...PyO>
    bool PythonBaseWrapper<T>::
    _parsePyArgs(const char *const kwlist[], PyObject *args, PyObject *kwds, PyO *&...pyargs) {
        char format[sizeof...(PyO) + 1] = {0};
        if (sizeof...(PyO) > 0)
            memset(format, 'O', sizeof...(PyO));
        return (args && !kwds && sizeof...(PyO) == 0) ||
               PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...);
    }

    template<typename T>
    template<typename ...Args>
    std::remove_reference_t<T> *
    PythonBaseWrapper<T>::
    _createBaseBase(unsigned char *location, argument_capture <Args> ... args) {
        if (location)
            return new(location) T(std::forward<typename extent_as_pointer<Args>::type>(args.value())...);
        return new T(std::forward<typename extent_as_pointer<Args>::type>(args.value())...);
    }


    template<typename T>
    template<typename ...Args, int ...S>
    std::remove_reference_t<T> *
    PythonBaseWrapper<T>::
    _createBase(unsigned char *location, PyObject *args, PyObject *kwds,
                const char *const kwlist[], container<S...>, _____fake <Args> *...) {
       if (args && PyTuple_Size(args) != sizeof...(Args)) {
            return nullptr;
        }
        PyObject *pyobjs[sizeof...(Args) + 1];
        (void) pyobjs;
        if (!_parsePyArgs(kwlist, args, kwds, pyobjs[S]...)) {
            PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
            return nullptr;
        }

        return _createBaseBase<Args...>(location, pyllars_internal::toCArgument<Args>(*pyobjs[S])...);
    }

    template<typename T>
    template<typename ...Args>
    std::remove_reference_t<T> *
    PythonBaseWrapper<T>::
    create(const char *const kwlist[], PyObject *args, PyObject *kwds, unsigned char *location) {
        return _createBase<Args...>(location, args, kwds, kwlist, typename argGenerator<sizeof...(Args)>::type(),
                                    (_____fake < Args > *)
                                    nullptr...);
    }
}

#endif //PYLLARS_PYTHON_BASE_IMPL_HPP
