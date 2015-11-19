#include <Python.h>
#include <structmember.h>
#include <limits.h>
#include <limits>

#include <functional>
#include "pyllars/pyllars_utils.h"

namespace __pyllars_internal {

/*********
 * Class to define Python wrapper to C class/type
 **/
 typedef const char* cname;

template< const char* const func_name, const char* const names[], typename ReturnType, typename ...Args>
struct PythonFunctionWrapper {
    PyObject_HEAD

    static PyTypeObject Type;

    static constexpr cname name=func_name;

    //template< typename Type>
    //static PyObject* pyObject(){ return nullptr;}

    static PyObject* _call(PyObject *callable_object, PyObject *args, PyObject *kw);

    static void _init();

 //   std::function<ReturnType(Args...)> _CObject;

    static PyObject *
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        PythonFunctionWrapper *self= (PythonFunctionWrapper*)type->tp_alloc(type, 0);

        return (PyObject *)self;

    }

    typedef ReturnType(*func_type)(Args...);

    static
    PythonFunctionWrapper* create(func_type func){
      static bool inited = false;
      if (!inited && (PyType_Ready(&Type) < 0) ){
          throw "Unable to initialize python object for c function wrapper";
      } else {
          inited = true;
          Py_INCREF(&Type);
          auto pyfuncobj = (PythonFunctionWrapper*)PyObject_CallObject((PyObject*)&Type, nullptr);
          pyfuncobj->_cfunc = func;
          return pyfuncobj;
      }
    }

    ReturnType call(Args... args){
      return _cfunc(args...);
    }

private:

    PythonFunctionWrapper():_cfunc(nullptr){}
    ~PythonFunctionWrapper(){}

    template<typename ...PyO>
    ReturnType callFuncBase( PyObject *pytuple, PyObject *kwds, PyO* ...pyargs){
        char format[sizeof...(Args)+1] = {0};
        memset(format, 'O', sizeof...(Args));
        if(!PyArg_ParseTupleAndKeywords(pytuple, kwds, format, (char**)names, pyargs...)){
          PyErr_Print();
          throw "Illegal arumgnet(s)";
        }
        return _cfunc(toCObject<Args>(*pyargs)...);
    }

    template<int ...S>
    ReturnType callFunc(PyObject* const tuple, PyObject* kw, container<S...> s) {
       return callFuncBase(tuple, kw, PyTuple_GetItem(s.pyobjs,S)...);
    }


    func_type _cfunc;
};


template< const char* const func_name, const char* const names[], typename ReturnType, typename... Args >
PyTypeObject PythonFunctionWrapper<func_name, names, ReturnType, Args...>::Type = {
    PyObject_HEAD_INIT(nullptr)
    0,                         /*ob_size*/
    func_name,             /*tp_name*/
    sizeof(PythonFunctionWrapper),             /*tp_basicsize*/
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
    _call,                         /*tp_call*/
    nullptr,                         /*tp_str*/
    nullptr,                         /*tp_getattro*/
    nullptr,                         /*tp_setattro*/
    nullptr,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT, /*tp_flags*/
    "PythonFunctionWrapper object",           /* tp_doc */
    nullptr,		               /* tp_traverse */
    nullptr,		               /* tp_clear */
    nullptr,		               /* tp_richcompare */
    0,		                       /* tp_weaklistoffset */
    nullptr,		               /* tp_iter */
    nullptr,		               /* tp_iternext */
    nullptr,             /* tp_methods */
    nullptr,             /* tp_members */
    nullptr,                         /* tp_getset */
    nullptr,                         /* tp_base */
    nullptr,                         /* tp_dict */
    nullptr,                         /* tp_descr_get */
    nullptr,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PythonFunctionWrapper::_init,  /* tp_init */
    nullptr,                         /* tp_alloc */
    PythonFunctionWrapper::_new,             /* tp_new */
};


template<const char* const func_name, const char* const names[], typename ReturnType, typename... Args>
void PythonFunctionWrapper<func_name, names, ReturnType, Args...>::_init(){
     if (PyType_Ready(&Type) < 0)
        return;

    Py_INCREF(&Type);
}

template<const char* const func_name, const char* const names[], typename ReturnType, typename... Args>
PyObject* PythonFunctionWrapper<func_name, names, ReturnType, Args...>::_call(PyObject *callable_object, PyObject *args, PyObject *kw){
  PythonFunctionWrapper& wrapper = *reinterpret_cast<PythonFunctionWrapper* const>(callable_object);
  return toPyObject<ReturnType>(wrapper.callFunc(args, kw, typename argGenerator<sizeof...(Args)>::type(args)), false);
}

}

