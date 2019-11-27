
#include <cstdio>
#include <limits>
#include <cmath>
#include <map>

#include <pyllars/pyllars.hpp>
#include <pyllars/internal/pyllars_pointer.impl.hpp>
#include <pyllars/internal/pyllars_classwrapper.impl.hpp>
#include <pyllars/internal/pyllars_varargs.hpp>
#include <pyllars/internal/pyllars_function_wrapper.hpp>

static void _pyllars_import_to_top(PyObject* pyllars_mod, PyObject* module){
  if(!pyllars_mod || !module){
    return;
  }
  PyObject *dir = PyObject_Dir(module);
  if(!dir){
    return;
  }
  PyObject *iterator = PyObject_GetIter(dir);
  PyObject *name;
  while((name = PyIter_Next(iterator))){
    PyObject * obj = PyObject_GetAttr(module, name);
    if (obj){
      PyObject_SetAttr(pyllars_mod, name, obj);
    }
    Py_DECREF(name);
  }
  Py_XDECREF(dir);
}

#if PY_MAJOR_VERSION == 3

PyObject*
PyllarsInit(const char* const name){
  static PyObject *const pyllars_mod = PyImport_ImportModule("pyllars");
  const char* const doc = "Pyllars top-level module";
  PyModuleDef *moduleDef = new PyModuleDef{
    PyModuleDef_HEAD_INIT,
    name,
    doc,
    -1,
    nullptr
  };

  PyObject* mod = PyModule_Create(moduleDef);
  if(mod){
    Py_INCREF(mod);
    PyObject_SetAttrString(pyllars_mod, name, mod);
  }
  int status = 0;
  if ((status = __pyllars_internal::Init::init())!= 0){
        printf("Failed to init all types");
  } else {
    if ((status |= __pyllars_internal::Init::ready()) != 0){
      printf("Failed to ready all types");
    }
  }
  return (status == 0)?mod:nullptr;
}
#else
PyMODINIT_FUNC
int PyllarsInit(const char* const name){
  static PyObject *const pyllars_mod = PyImport_ImportModule("pyllars");
  PyObject *mod = Py_InitModule3(name, nullptr, "Pyllars top-level module");
  if(!mod) { return -1;}
  PyObject_SetAttr(pyllars_mod, name, mod);
  int rc = pyllars::Initializer::root?pyllars::Initializer::root->init(mod):0;
  if (0 == rc) {
    rc = pyllars::Initializer::root?pyllars::Initializer::root->init_last(mod):0;
  }
  if (rc != 0){
    printf("Failed to initializer some components of %s", name);
  }
  _pyllars_import_to_top(pyllar_mod, mod);
  return rc;
}
#endif


#include <pyllars/internal/pyllars_integer.hpp>
#include <pyllars/internal/pyllars_floating_point.hpp>
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"


namespace __pyllars_internal {
    std::vector<Init::ready_func_t> *Init::_readyFuncs = nullptr;
    std::vector<Init::ready_func_t> *Init::_initFuncs = nullptr;

    PyObject* NULL_ARGS(){
        static PyObject* args = PyTuple_Pack(1, PyFloat_FromDouble(0.129726362));  // bogus vale to make unique
        return args;
    }

    PyTypeObject CommonBaseWrapper::_BaseType = {
#if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(nullptr)
            0,                         /*ob_size*/
#endif
        "BasePythonFromCWrapper",             /*tp_name*/ /*filled on init*/
        sizeof(CommonBaseWrapper),             /*tp_basicsize*/
        0,                         /*tp_itemsize*/
        nullptr, //(destructor) CommonBaseWrapper::_dealloc, /*tp_dealloc*/
        nullptr,                         /*tp_print*/
        nullptr,                         /*tp_getattr*/
        nullptr,                         /*tp_setattr*/
        nullptr,                         /*tp_compare*/
        nullptr,                         /*tp_repr*/
        nullptr,          /*tp_as_number*/
        nullptr,                         /*tp_as_sequence*/
        nullptr,                         /*tp_as_mapping*/
        nullptr,                         /*tp_hash */
        nullptr,                         /*tp_call*/
        nullptr,                         /*tp_str*/
        nullptr,                         /*tp_getattro*/
        nullptr,                         /*tp_setattro*/
        nullptr,                         /*tp_as_buffer*/
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES, /*tp_flags*/
        "BasePythonFromCWrapper object",           /* tp_doc */
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
        CommonBaseWrapper::__init,  /* tp_init */
        nullptr,                         /* tp_alloc */
        CommonBaseWrapper::_new,             /* tp_new */
        nullptr,                         /*tp_free*/
        nullptr,                         /*tp_is_gc*/
#if PY_MAJOR_VERSION == 3
        nullptr,                         /*tp_base*/
#else
        &CommonBaseWrapper::Base::TypePtr,                         /*tp_bases*/
#endif
        nullptr,                         /*tp_mro*/
        nullptr,                         /*tp_cache*/
        nullptr,                         /*tp_subclasses*/
        nullptr,                          /*tp_weaklist*/
        nullptr,                          /*tp_del*/
        0,                          /*tp_version_tag*/
    };

    namespace {
        int _base_init(PyObject*self, PyObject *args, PyObject *kwds){
            return 0;
        }


    }

    PyTypeObject BasePtrType = {
#if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(nullptr)
            0,                         /*ob_size*/
#endif
            "C++ ptr type",             /*tp_name*/  //set on call to initialize
            sizeof(PythonClassWrapper<void*>) + 8,             /*tp_basicsize*/
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
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
            "base pointer wrapper object",           /* tp_doc */
            nullptr,                               /* tp_traverse */
            nullptr,                               /* tp_clear */
            nullptr,                               /* tp_richcompare */
            0,                             /* tp_weaklistoffset */
            nullptr,                               /* tp_iter */
            nullptr,                               /* tp_iternext */
            nullptr,             /* tp_methods */
            nullptr,             /* tp_members */
            nullptr,                         /* tp_getset */
            nullptr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            (initproc) _base_init,      /* tp_init */
            nullptr,                         /* tp_alloc */
            nullptr,                 /* tp_new */
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

#ifndef _MSVC_STL_VERSION

    int getType(PyObject *obj, ffi_type *&type) {
        int subtype = 0;
        if (!obj){
            throw "SystemError: null object encountered when getting ffi type";
        }
        if ( PythonClassWrapper<char>::checkType(obj)||
            PythonClassWrapper<char&>::checkType(obj)) {
            type = &ffi_type_sint8;
        } else if ( PythonClassWrapper<unsigned char>::checkType(obj)||
               PythonClassWrapper<unsigned char&>::checkType(obj)) {
            type = &ffi_type_uint8;
        } else if (PyInt_Check(obj) || PythonClassWrapper<int>::checkType(obj)||
                   PythonClassWrapper<int&>::checkType(obj)) {
            type = &ffi_type_sint32;
        } else if (PyLong_Check(obj)|| PythonClassWrapper<long long>::checkType(obj)||
                   PythonClassWrapper<long long&>::checkType(obj)|| PythonClassWrapper<long>::checkType(obj)||
                   PythonClassWrapper<long&>::checkType(obj)) {
            type = &ffi_type_sint64;
        } else if (PythonClassWrapper<float>::checkType(obj)||
                   PythonClassWrapper<float&>::checkType(obj)) {
            type = &ffi_type_float;
        }else if (PyFloat_Check(obj)|| PythonClassWrapper<double>::checkType(obj)||
                  PythonClassWrapper<double&>::checkType(obj)) {
            type = &ffi_type_double;
        } else if (PyBool_Check(obj)) {
            type = &ffi_type_uint8;
        } else if (PyString_Check(obj) || PythonClassWrapper<const char*>::checkType(obj)||
                    PythonClassWrapper<const char* const>::checkType(obj)) {
            type = &ffi_type_pointer;
            subtype = STRING_TYPE;
#if PY_MAJOR_VERSION == 3
        } else if (PyBytes_Check(obj)) {
            type = &ffi_type_pointer;
            subtype = COBJ_TYPE;
#endif
        } else if ( PyObject_TypeCheck(obj, &BasePtrType)) {
            type = &ffi_type_pointer;
            subtype = COBJ_TYPE;
        } else if (CommonBaseWrapper::IsClassType(obj)) {
            type = &ffi_type_pointer;
            subtype = COBJ_TYPE;
        } else if (CommonBaseWrapper::IsCFunctionType(obj)) {
            subtype = FUNC_TYPE;
            type = &ffi_type_pointer;
        } else {
            throw "Cannot conver Python object to C Object";
        }
        return subtype;
    }

    void * toFFI(PyObject* arg){

        // Specify the data type of each argument. Available types are defined
        // in <ffi/ffi.h>.
        union ArgType {
            int intvalue;
            long longvalue;
            double doublevalue;
            bool boolvalue;
            void *ptrvalue;
        };
        void* arg_value = nullptr;
        ffi_type *arg_type;
        ArgType generic_value;

        const int subtype = getType(arg, arg_type);
        switch (arg_type->type) {
            case FFI_TYPE_SINT32:
                generic_value.intvalue = PyInt_AsLong(arg);
                arg_value = &generic_value.intvalue;
                break;
            case FFI_TYPE_SINT64:
                generic_value.longvalue = PyLong_AsLong(arg);
                arg_value = &generic_value.longvalue;
                break;
            case FFI_TYPE_UINT8:
                generic_value.boolvalue = (arg == Py_True);
                arg_value = &generic_value.boolvalue;
                break;
            case FFI_TYPE_DOUBLE:
                generic_value.doublevalue = PyFloat_AsDouble(arg);
                arg_value = &generic_value.doublevalue;
                break;
            case FFI_TYPE_POINTER:
                if (STRING_TYPE == subtype) {
                    generic_value.ptrvalue = (void*) PyString_AsString(arg);
                    arg_value = &generic_value.ptrvalue;
                } else if (COBJ_TYPE == subtype) {
                    static const size_t offset = offset_of<ArgType*, PythonClassWrapper<ArgType> >
                            (&PythonClassWrapper<ArgType>::_CObject);
                    ArgType **ptrvalue = (ArgType **) (((char *) arg) + offset);
                    generic_value.ptrvalue = ptrvalue;
                } else if (FUNC_TYPE == subtype) {
                    typedef PythonFunctionWrapper<void(void)> wtype;
                    static const size_t offset = offset_of(&PythonFunctionWrapper<void(void)>::_function);//offsetof(wtype, _function);
                    void **ptrvalue = (void **) (((char *) arg) + offset);
                    generic_value.ptrvalue = *ptrvalue;
                } else {
                    throw "Unable to convert Python object to C Object";
                }
                arg_value = &generic_value.ptrvalue;
                break;
            default:
                throw "Python object cannot be converted to C object";
                break;
        }
        return arg_value;
    }
#endif

    template<>
    const char*
    fromPyStringLike<const char>(PyObject* obj){
        if (PyString_Check(obj)) {
            return PyString_AsString(obj);
        } else if (PyUnicode_Check(obj)){
            return PyUnicode_AsUTF8(obj);
        }
        return nullptr;
    }

    template<>
    char*
    fromPyStringLike<char>(PyObject* obj){
        if (PyBytes_Check(obj)) {
            return PyBytes_AsString(obj);
        }
        return nullptr;
    }


    template<>
    PyObject *set_array_values<const char **, false, -1, void>(const char **values, const ssize_t size, PyObject *from,
                                                               PyObject *referenced) {
        if (!PyList_Check(referenced) || PyList_Size(referenced) != size) {
            PyErr_SetString(PyExc_RuntimeError, "Internal error setting array elements");
        }
        if (PyList_Check(from)) {

            //have python list to set from
            if (PyList_Size(from) != size) {
                PyErr_SetString(PyExc_TypeError, "Invalid array size");
                return nullptr;
            }
            for (int i = 0; i < size; ++i) {
                PyObject *item = PyList_GetItem(from, i);
                if (!PyString_Check(item)) {
                    return nullptr;
                }
                const char *asstr = PyString_AsString(item);

                if (!asstr) {
                    PyErr_SetString(PyExc_TypeError, "Not a string on array elements assignment");
                    return nullptr;
                } else {
                    //make copy and keep reference
                    PyObject *pystr = PyString_FromString(asstr);
                    PyList_SetItem(referenced, i, pystr);
                    values[i] = (const char *const) PyString_AsString(pystr);
                }
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid argument type");
            return nullptr;
        }
        return Py_None;
    }

    bool CommonBaseWrapper::IsClassType(PyObject* obj){
        auto *pytype = (PyTypeObject *) PyObject_Type(obj);
        return strncmp(pytype->tp_name, tp_name_prefix, tp_name_prefix_len) == 0;
    }

    void CommonBaseWrapper::make_reference(PyObject* obj){
        if (_referenced) { Py_DECREF(_referenced); }
        if (obj) { Py_INCREF(obj); }
        _referenced = obj;
    }

    std::map< std::pair<PyTypeObject*, PyTypeObject*>, PyObject* (*)(PyObject*)> & CommonBaseWrapper::castMap() {
        static std::map< std::pair<PyTypeObject*, PyTypeObject*>, PyObject* (*)(PyObject*)> map;
        return map;
    }


    std::map<std::pair<PyTypeObject*, PyTypeObject*>, PyObject*(*)(PyObject*)>& CommonBaseWrapper::_castAsCArgument(){
        static std::map<std::pair<PyTypeObject*, PyTypeObject*>, PyObject*(*)(PyObject*)> container;
        return container;
    }

    PyObject *CommonBaseWrapper::_new(PyTypeObject *type, PyObject *args, PyObject *kwds){
        (void) args;
        (void) kwds;
        CommonBaseWrapper *self;
        self = (CommonBaseWrapper *) type->tp_alloc(type, 0);
        return reinterpret_cast<PyObject*>(self);
    }


    bool CommonBaseWrapper::IsCFunctionType(PyObject *obj) {
        auto *pytype = (PyTypeObject *) PyObject_Type(obj);
        return strncmp(pytype->tp_name, ptrtp_name_prefix, ptrtp_name_prefix_len) == 0;
    }

}

#include "pyllars_namespacewrapper.hpp"

namespace pyllars{
    std::map<std::string, PyObject*> NSInfoBase::module_list;
}