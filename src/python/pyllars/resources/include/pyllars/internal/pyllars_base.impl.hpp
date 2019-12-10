//
// Created by jrusnak on 12/10/19.
//
#include "pyllars_base.hpp"

#ifndef PYLLARS_PYTHON_BASE_IMPL_HPP
#define PYLLARS_PYTHON_BASE_IMPL_HPP
namespace pyllars_internal{

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
            nullptr ,                        /* tp_init */
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


}
#endif //PYLLARS_PYTHON_BASE_IMPL_HPP
