//
// Created by jrusnak on 12/3/19.
//


#include "pyllars/internal/pyllars_integer.hpp"
#include "pyllars/pyllars_class.hpp"
#include "pyllars/pyllars_namespacewrapper.hpp"

namespace pyllars_internal {


#ifdef _MSC_VER
    __declspec(noinline)
#endif
    PyTypeObject *PyNumberCustomBase::getRawType() {
        return &_Type;
    }

    DLLEXPORT PyTypeObject PyNumberCustomBase::_Type = {
#if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(nullptr)
    0,                         /*ob_size*/
#endif
            "PyllarsNumberBase", /*tp_name*/
            sizeof(PyNumberCustomBase), /*tp_basicsize*/
            0, /*tp_itemsize*/
            nullptr, /*tp_dealloc*/
            nullptr, /*tp_print*/
            nullptr, /*tp_getattr*/
            nullptr, /*tp_setattr*/
            nullptr, /*tp_as_sync*/
            nullptr, /*tp_repr*/

            nullptr, /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            nullptr,                         /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES, /*tp_flags*/
            "Base umber type in pyllars",           /* tp_doc */
            nullptr,                       /* tp_traverse */
            nullptr,                       /* tp_clear */
            nullptr,                       /* tp_richcompare */
            0,                               /* tp_weaklistoffset */
            nullptr,                       /* tp_iter */
            nullptr,                       /* tp_iternext */
            nullptr,             /* tp_methods */
            nullptr,             /* tp_members */
            nullptr,                         /* tp_getset */
            nullptr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            nullptr,  /* tp_init */
            nullptr,                         /* tp_alloc */
            PyType_GenericNew,             /* tp_new */
            nullptr,                         /*tp_free*/
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bases*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };


}


namespace pyllars{

    template
    class DLLEXPORT PyllarsClass<bool, GlobalNS>;

    template
    class DLLEXPORT PyllarsClass<const bool, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<char, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<unsigned char, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<signed char, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<short, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<unsigned short, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<int, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<unsigned int, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<long, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<unsigned long, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<long long, GlobalNS>;


    template
    class DLLEXPORT PyllarsClass<unsigned long long, GlobalNS>;

    ///////////////////


}