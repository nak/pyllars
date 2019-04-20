#ifndef ___PYLLARS__FUNCTION_WRAPPER
#define ___PYLLARS__FUNCTION_WRAPPER

#include <limits>
#include <functional>
#include <Python.h>
#include <structmember.h>
#include <limits.h>
#include <ffi.h>

#include "pyllars_containment.hpp"
#include "pyllars_utils.hpp"
#include "pyllars_varargs.hpp"

namespace __pyllars_internal {

    template<typename func_type>
    struct PythonFunctionWrapper : public CommonBaseWrapper {

        static PyTypeObject * getPyType(){ return &Type;}

    private:
        static PyTypeObject Type;

        static PyObject *_call(PyObject *callable_object, PyObject *args, PyObject *kw);

        static int _init(PyObject *self, PyObject *args, PyObject *kwds);

        static PyObject *
        _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
            (void) args;
            (void) kwds;
            PythonFunctionWrapper *self = reinterpret_cast<PythonFunctionWrapper*>(type->tp_alloc(type, 0));
            return (PyObject *) self;
        }

        static bool checkType(PyObject *const obj) {
            return PyObject_TypeCheck(obj, &Type);
        }

        static
        PythonFunctionWrapper *createPy(func_type &func, const char* const name, const char* const kwlist[]) {
            static bool inited = false;
            Py_ssize_t index = 0;
            static const std::string tname = func_traits<func_type>::type_name();

            Type.tp_name = tname.c_str();
            if (!inited && (PyType_Ready(&Type) < 0)) {
                throw "Unable to initialize python object for c function wrapper";
            } else if(!inited){
                inited = true;
                auto pyfuncobj = reinterpret_cast<PythonFunctionWrapper*>(PyObject_CallObject((PyObject *) Type, nullptr));
                if (!pyfuncobj){
                  PyErr_Print();
                  throw "Unable to create function callable";
                }
                pyfuncobj->_func_container = GlobalFunctionContainer<func_type>(func, kwlist);
                if (!PyCallable_Check((PyObject *) pyfuncobj)) {
                    throw "Python object is not callbable as expected!";
                }
                return pyfuncobj;
            }
        }

        GlobalFunctionContainer<func_type> *_func_container;
    };

    //Python definition of Type for this function wrapper
    template<typename func_type>
    PyTypeObject PythonFunctionWrapper<func_type>::Type = {
    #if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
    #else
            PyObject_HEAD_INIT(nullptr)
            0,                               /*ob_size*/
    #endif
            nullptr,                         /*tp_name*/
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
        if (PyType_Ready(&Type) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to initialize Python type");
            return -1;
        }
        PyObject *const type = reinterpret_cast<PyObject *>(&Type);
        Py_INCREF(type);
        return 0;
    }

    template<typename func_type>
    PyObject *
    PythonFunctionWrapper<func_type>::_call(PyObject *self, PyObject *args, PyObject *kw) {
        try {
            auto self_ = reinterpret_cast<PythonFunctionWrapper*>(self);
            return self_->_func_container->call(self, args, kw);
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
    }

    template<typename func_type>
    class PythonClassWrapper<func_type, typename std::enable_if<std::is_function<typename std::remove_pointer<func_type>::type>::value>::type>:
            public PythonFunctionWrapper<func_type>{
            };
}

#endif
