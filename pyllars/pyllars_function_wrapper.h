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

    template< const char* const func_name, const char* const names[], typename ReturnType, typename ...Args>
    struct PythonFunctionWrapper {
        PyObject_HEAD

        typedef const char* cname;
        typedef ReturnType(*func_type)(Args...);

        static PyTypeObject Type;

        static constexpr cname name = func_name;

        static PyObject* _call(PyObject *callable_object, PyObject *args, PyObject *kw);

        static int _init(PyObject* self, PyObject* args, PyObject*kwds);

        static PyObject *
        _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
            (void)args;
            (void)kwds;
            PythonFunctionWrapper *self= (PythonFunctionWrapper*)type->tp_alloc(type, 0);
            return (PyObject *)self;
        }


        static
        PythonFunctionWrapper* create(func_type func){
          static bool inited = false;
          if (!inited && (PyType_Ready(&Type) < 0) ){
              throw "Unable to initialize python object for c function wrapper";
          } else {
              inited = true;
              PyObject* const type = reinterpret_cast<PyObject*>(&Type);
              Py_INCREF(type);
              auto pyfuncobj = (PythonFunctionWrapper*)PyObject_CallObject(type, nullptr);
              pyfuncobj->_cfunc = func;
              if(!PyCallable_Check((PyObject*)pyfuncobj)){
                throw "Python object is not callbable as expected!";
              }
              return pyfuncobj;
          }
        }

    private:

        PythonFunctionWrapper():_cfunc(nullptr){}
        ~PythonFunctionWrapper(){}

        template<typename ...PyO>
        ReturnType callFuncBase( PyObject *pytuple, PyObject *kwds, PyO* ...pyargs){
            if(!_cfunc){
                PyErr_SetString(PyExc_RuntimeError, "Uninitialized C callable!");
                PyErr_Print();
                throw "Uninitialized C callable";
            }
            char format[sizeof...(Args)+1] = {0};
            memset(format, 'O', sizeof...(Args));
            if(!PyArg_ParseTupleAndKeywords(pytuple, kwds, format, (char**)names, &pyargs...)){
              PyErr_Print();
              throw "Illegal arumgnet(s)";
            }
            return _cfunc(*toCObject<Args>(*pyargs)...);
        }

        template<int ...S>
        ReturnType callFunc(PyObject* const tuple, PyObject* kw, container<S...> s) {
           (void)s;//only used for unpacking arguments into a list and determine the int... args to this template
           PyObject pyobjs[sizeof...(S)];
           return callFuncBase(tuple, kw, &pyobjs[S]...);
        }


        func_type _cfunc;
    };

    //Python definition of Type for this function wrapper
    template< const char* const func_name, const char* const names[], typename ReturnType, typename... Args >
    PyTypeObject PythonFunctionWrapper<func_name, names, ReturnType, Args...>::Type = {
        PyObject_HEAD_INIT(nullptr)
        0,                               /*ob_size*/
        func_name,                       /*tp_name*/
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
        nullptr,		                 /* tp_traverse */
        nullptr,		                 /* tp_clear */
        nullptr,		                 /* tp_richcompare */
        0,		                         /* tp_weaklistoffset */
        nullptr,		                 /* tp_iter */
        nullptr,		                 /* tp_iternext */
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
        PythonFunctionWrapper::_new,     /* tp_new */
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

    template<const char* const func_name, const char* const names[], typename ReturnType, typename... Args>
    int PythonFunctionWrapper<func_name, names, ReturnType, Args...>::_init(PyObject* self, PyObject* args, PyObject*kwds){
         //avoid compiler warnings (including reinterpret cast to avoid type-punned warning)
         (void) self;
         (void) args;
         (void) kwds;
         if (PyType_Ready(&Type) < 0){
            PyErr_SetString(PyExc_RuntimeError,"Unable to initialize Python type");
            return -1;
        }
        PyObject* const type = reinterpret_cast<PyObject*>(&Type);
        Py_INCREF(type);
        return 0;
    }

    template<const char* const func_name, const char* const names[], typename ReturnType, typename... Args>
    PyObject* PythonFunctionWrapper<func_name, names, ReturnType, Args...>::_call(PyObject *callable_object, PyObject *args, PyObject *kw){
      try{
        PythonFunctionWrapper& wrapper = *reinterpret_cast<PythonFunctionWrapper* const>(callable_object);
        return toPyObject<ReturnType>(wrapper.callFunc(args, kw, typename argGenerator<sizeof...(Args)>::type()), false);
      } catch( const char* const msg){
        PyErr_SetString(PyExc_RuntimeError, msg);
        PyErr_Print();
        return Py_None;
      }
    }


}

