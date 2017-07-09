#include "pyllars_templates.hpp"
#include <string.h>

namespace __pyllars_internal {

    int
    PythonClassTemplate::addParameterSet(PyObject * set[]) {
        if(!_parameterSets){
            _parameterSets = new std::vector<PyObject**>();
        }
        _parameterSets->push_back(set);
        return 0;
    }

    bool
    PythonClassTemplate::_compare(PyObject **set, PyObject *tuple) {
        PyObject **argPtr = set;
        Py_ssize_t index = 0;
        while (*(++argPtr)) {//first is Type to be returned and not to be parsed here
            PyObject *item = PyTuple_GetItem(tuple, index++);
            if (PyInt_Check(*argPtr) && PyInt_Check(item)) {
                if (PyInt_AsLong(*argPtr) != PyInt_AsLong(item)) {
                    return false;
                }
            } else if (PyLong_Check(*argPtr) && PyLong_Check(item)) {
                if (PyLong_AsLong(*argPtr) != PyLong_AsLong(item)) {
                    return false;
                }
            } else if (PyString_Check(*argPtr) && PyString_Check(item)) {

                if (strcmp(PyString_AsString(*argPtr), PyString_AsString(item)) != 0)
                {
                    return false;
                }
                #if PY_MAJOR_VERSION == 3
            } else if (PyBytes_Check(*argPtr) && PyBytes_Check(item)) {
                if (strcmp(PyBytes_AsString(*argPtr), PyBytes_AsString(item)) != 0)
                {
                    return false;
                }
                #endif
            } else if (PyFloat_Check(*argPtr) && PyFloat_Check(item)) {
                if (PyFloat_AsDouble(*argPtr) != PyFloat_AsDouble(item)){
                    return false;  //I don't think this case really can happen in C++ anyway
                }
            } else if(PyBool_Check(item) && PyBool_Check(*argPtr)){
                return item == *argPtr;
            } else if (PyTuple_Check(*argPtr) && PyTuple_Check(item)){
                if (PyTuple_Size(tuple) < 2){
                    return false;
                }
                PyObject* item1 = PyTuple_GetItem(*argPtr, 0);
                PyObject* item2 = PyTuple_GetItem(*argPtr, 1);
                PyObject* type = PyTuple_GetItem(item, 0);
                PyObject* str = PyTuple_GetItem(item, 1);
                if(item1 == (PyObject*) &PyLong_Type){
		  if(type!= (PyObject*)&PyInt_Type && 
		     type != (PyObject*)&PyLong_Type){
                        return false;
                    }
                    if(!PyString_Check(str)){ return false;}

                    if(!strcmp(PyString_AsString(item2), PyString_AsString(str))==0){
                        return false;
                    }
                } else if(item1 == (PyObject*)&PyFloat_Type){
                    if(!PyFloat_Check(type)){
                        return false;
                    }
                    if(!PyString_Check(str)){ return false;}
                    if(!strcmp(PyString_AsString(item2), PyString_AsString(str))==0){
                        return false;
                    }
                } else {
                    return false;
                }
            } else if (PyType_Check(*argPtr) && PyType_Check(item)) {
                if (*argPtr != item) {
                    return false;
                }
            } else {
                return false;
            }
        }
        return true;
    }

    PyObject *
    PythonClassTemplate::_call(PyObject *callable_object,
                                         PyObject *args,
                                         PyObject *kw) {
        PythonClassTemplate* self = (PythonClassTemplate*) callable_object;
        if (!kw && PyDict_Size(kw) > 0) {
            PyErr_BadArgument();
            return nullptr;
        }
        if(self->_parameterSets){
          for (PyObject **set : *self->_parameterSets) {
                if (_compare(set, args)) {
                    return set[0];
                }
          }
        }

        PyErr_SetString(PyExc_ValueError, "No template instantiation exists for given parameters");
        return nullptr;
    }

    PyObject *
    PythonClassTemplate::_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        (void) args;
        (void) kwds;
        return type->tp_alloc(type, 0);
    }

    int
    PythonClassTemplate::_init(PyObject *self, PyObject *args, PyObject *kwds) {
        (void) self;
        (void) args;
        (void) kwds;
	if(!((PythonClassTemplate *) self)->_parameterSets )
	  ((PythonClassTemplate *) self)->_parameterSets = new std::vector<PyObject **>();
        return 0;
    }

    void
    PythonClassTemplate::create(const char *const name, const char *const short_name,
                                             PyObject *const module,
                                             std::map<std::string, PythonClassTemplate*> & map) {
        PyTypeObject* Type = new PyTypeObject{
        #if PY_MAJOR_VERSION == 3
                PyVarObject_HEAD_INIT(nullptr, 0)
        #else
                PyObject_HEAD_INIT(nullptr)
                0,                               /*ob_size*/
        #endif
                name,                         /*tp_name*/
                sizeof(PythonClassTemplate),   /*tp_basicsize*/
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
                PythonClassTemplate::_call,   /*tp_call*/
                nullptr,                         /*tp_str*/
                nullptr,                         /*tp_getattro*/
                nullptr,                         /*tp_setattro*/
                nullptr,                         /*tp_as_buffer*/
                Py_TPFLAGS_DEFAULT, /*tp_flags*/
                "PythonClassTemplate function",  /* tp_doc */
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
                PythonClassTemplate::_init,  /* tp_init */
                nullptr,                         /* tp_alloc */
                PythonClassTemplate::_new,     /* tp_new */
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
        if (PyType_Ready(Type) < 0) {
            throw("Unable to initialize Pyllars python-type");
        }
        PyObject *const type = reinterpret_cast<PyObject *>(Type);
        Py_INCREF(type);
        PythonClassTemplate* obj = (PythonClassTemplate*)PyObject_Call(type, PyTuple_New(0), nullptr);
        PyModule_AddObject(module, short_name, (PyObject*)obj);
        map[name] = obj;
    }

}
