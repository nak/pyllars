#ifndef ___PYLLARS__FUNCTION_WRAPPER
#define ___PYLLARS__FUNCTION_WRAPPER

#include <limits>
#include <functional>
#include <Python.h>
#include <structmember.h>
#include <limits.h>
#include <ffi.h>

#include "pyllars/internal/pyllars_containment.hpp"
#include "pyllars_type_traits.hpp"
#include "pyllars_utils.hpp"
#include "pyllars_varargs.hpp"
#include "pyllars_staticfunctionsemantics.hpp"

namespace pyllars_internal {

    template<typename func_type>
    struct PythonFunctionWrapper : public CommonBaseWrapper {

        static PyTypeObject *getPyType() { return &_Type; }

        static int initialize(){
            if (PyType_Ready(&_Type) < 0) {
                PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
                PyErr_Print();
                return -1;
            }
            Py_INCREF(&_Type);
            return 0;
        }

        static bool checkType(PyObject *const obj) {
            return PyObject_TypeCheck(obj, &_Type);
        }

        template<const char *const kwlist[], func_type function>
        static
        PythonFunctionWrapper *createPy(const char *const name) {
            static bool inited = false;
            if (!inited && (PyType_Ready(&_Type) < 0)) {
                throw PyllarsException(PyExc_SystemError, "Unable to initialize python object for c function wrapper");
            } else {
                inited = true;
                auto pyfuncobj = reinterpret_cast<PythonFunctionWrapper *>(PyObject_CallObject((PyObject *)&_Type,
                                                                                               nullptr));
                if (!pyfuncobj) {
                    PyErr_Print();
                    throw PyllarsException(PyExc_SystemError, "Unable to create function callable");
                }
                pyfuncobj->_call_func = StaticFunctionContainer<kwlist, func_type*, function>::call;
                pyfuncobj->_function = function;
                if (!PyCallable_Check((PyObject *) pyfuncobj)) {
                    throw PyllarsException(PyExc_TypeError, "Python object is not callbable as expected");
                }
                return pyfuncobj;
            }
        }
    private:
        static PyTypeObject _Type;

        static PyObject *_call(PyObject *callable_object, PyObject *args, PyObject *kw);

        static int _init(PyObject *self, PyObject *args, PyObject *kwds);


        static PyObject *
        _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
            (void) args;
            (void) kwds;
            PythonFunctionWrapper *self = reinterpret_cast<PythonFunctionWrapper *>(type->tp_alloc(type, 0));
            return (PyObject *) self;
        }

        typedef PyObject *(*call_t)(PyObject *, PyObject *, PyObject *);

        call_t _call_func;
    public:
        func_type* _function;
    };

    //Python definition of Type for this function wrapper
    template<typename func_type>
    PyTypeObject PythonFunctionWrapper<func_type>::_Type = {
#if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(nullptr)
    0,                               /*ob_size*/
#endif
            pyllars_internal::type_name<func_type>(), /*tp_name*/
            sizeof(PythonFunctionWrapper),   /*tp_basicsize*/
            0,                               /*tp_itemsize*/
            nullptr,                         /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            nullptr,                         /*tp_getattr*/
            nullptr,                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            nullptr,                         /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            _call,                           /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT, /*tp_flags*/
            "PythonFunctionWrapper object",  /* tp_doc */
            nullptr,                         /* tp_traverse */
            nullptr,                         /* tp_clear */
            nullptr,                         /* tp_richcompare */
            0,                                 /* tp_weaklistoffset */
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
            _init,  /* tp_init */
            nullptr,                         /* tp_alloc */
            _new,     /* tp_new */
            nullptr,                         /*tp_free*/ //TODO: Implement a free??
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bass*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };

    template<typename func_type>
    int PythonFunctionWrapper<func_type>::_init(PyObject *self, PyObject *args, PyObject *kwds) {
        //avoid compiler warnings (including reinterpret cast to avoid type-punned warning)
        (void) self;
        (void) args;
        (void) kwds;
        if (PyType_Ready(&_Type) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to initialize Python type");
            return -1;
        }
        PyObject *const type = reinterpret_cast<PyObject *>(&_Type);
        Py_INCREF(type);
        return 0;
    }

    template<typename func_type>
    PyObject *
    PythonFunctionWrapper<func_type>::_call(PyObject *self, PyObject *args, PyObject *kw) {
        try {
            auto self_ = reinterpret_cast<PythonFunctionWrapper *>(self);
            return self_->_call_func(self, args, kw);
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
    }

    /*
    template<typename func_type>
    class PythonClassWrapper<func_type, typename std::enable_if<std::is_function<typename std::remove_pointer<func_type>::type>::value>::type>
            :
                    public PythonFunctionWrapper<func_type> {

    };
     */
}

#endif
