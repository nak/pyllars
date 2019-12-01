#ifndef PYLLARS_TEMPLATES_H
#define PYLLARS_TEMPLATES_H

#include <Python.h>
#include <string.h>
#include "pyllars/internal/pyllars_templates.hpp"
#include "pyllars/internal/pyllars_defns.hpp"

namespace pyllars_internal {



        PyTypeObject TemplateDict::_Type = {
#if PY_MAJOR_VERSION == 3
                PyVarObject_HEAD_INIT(nullptr, 0)
#else
        PyObject_HEAD_INIT(nullptr)
                0,                               /*ob_size*/
#endif
                "TemplateDictType",                         /*tp_name*/
                sizeof(TemplateDict),   /*tp_basicsize*/
                0,                               /*tp_itemsize*/
                nullptr,                         /*tp_dealloc*/
                nullptr,                         /*tp_print*/
                nullptr,                         /*tp_getattr*/
                nullptr,                         /*tp_setattr*/
                nullptr,                         /*tp_compare*/
                nullptr,                         /*tp_repr*/
                nullptr,                         /*tp_as_number*/
                nullptr,                         /*tp_as_sequence*/
                new PyMappingMethods{
                        (lenfunc)TemplateDict::_mp_lengh,
                        (binaryfunc)TemplateDict::_mp_subscript,
                        (objobjargproc) TemplateDict::_mp_ass_subscript
                    },          /*tp_as_mapping*/
                nullptr,                         /*tp_hash */
                nullptr,                         /*tp_call*/
                nullptr,                         /*tp_str*/
                nullptr,                         /*tp_getattro*/
                nullptr,                         /*tp_setattro*/
                nullptr,                         /*tp_as_buffer*/
                Py_TPFLAGS_DEFAULT,              /*tp_flags*/
                "Dict type for holding template defns",  /* tp_doc */
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
                (initproc) TemplateDict::_init,  /* tp_init */
                nullptr,                         /* tp_alloc */
                (newfunc)TemplateDict::_new,     /* tp_new */
                nullptr,                         /*tp_free*/ //TODO: Implement a free??
                nullptr,                         /*tp_is_gc*/
                nullptr,                         /*tp_bass*/
                nullptr,                         /*tp_mro*/
                nullptr,                         /*tp_cache*/
                nullptr,                         /*tp_subclasses*/
                nullptr,                          /*tp_weaklist*/
                nullptr,                          /*tp_del*/
                0,                          /*tp_version_tag*/
                nullptr,                          /*tp_finalize*/

        };
}


#endif