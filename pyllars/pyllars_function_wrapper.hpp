
#include <limits>
#include <functional>
#include <Python.h>
#include <structmember.h>
#include <limits.h>

#include "pyllars_utils.hpp"

namespace __pyllars_internal {

    /*********
     * Class to define Python wrapper to C class/type
     **/

    template< bool is_base_return_complete,  typename ReturnType, typename ...Args>
    struct PythonFunctionWrapper {
        PyObject_HEAD

        typedef ReturnType(*func_type)(Args...);

        static PyTypeObject Type;

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
        PythonFunctionWrapper* create(const char* const func_name, func_type func, const char* const names[]){
            static bool inited = false;
            Py_ssize_t index = 0;
            if (!inited && (PyType_Ready(&Type) < 0) ){
                throw "Unable to initialize python object for c function wrapper";
            } else {
                inited = true;
                PyTypeObject*  type = new PyTypeObject(Type);
                Py_INCREF(type);
                char* name = new char[strlen(func_name)+1];
                strcpy(name, func_name);
                type->tp_name = name;
                auto pyfuncobj = (PythonFunctionWrapper*)PyObject_CallObject((PyObject*)type, nullptr);
                pyfuncobj->_cfunc = func;
                while( names[index] ){
                    pyfuncobj->_kwlist.push_back( names[index++] );
                }
                pyfuncobj->_kwlist.push_back(nullptr);
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
            if(!PyArg_ParseTupleAndKeywords(pytuple, kwds, format, (char**)_kwlist.data(), &pyargs...)){
              PyErr_Print();
              throw "Illegal arumgnet(s)";
            }
            return _cfunc(*toCObject<Args, false, PythonClassWrapper<Args> >(*pyargs)...);
        }

        template<int ...S>
        ReturnType callFunc(PyObject* const tuple, PyObject* kw, container<S...> s) {
           (void)s;//only used for unpacking arguments into a list and determine the int... args to this template
           PyObject pyobjs[sizeof...(S)];
	   (void)pyobjs;
           return callFuncBase(tuple, kw, &pyobjs[S]...);
        }


        func_type _cfunc;
        std::vector<const char*> _kwlist;
    };

    //Python definition of Type for this function wrapper
    template< bool  is_base_return_complete, typename ReturnType, typename... Args >
    PyTypeObject PythonFunctionWrapper< is_base_return_complete, ReturnType, Args...>::Type = {
        PyObject_HEAD_INIT(nullptr)
        0,                               /*ob_size*/
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

    template<bool is_base_return_complete, typename ReturnType, typename... Args>
      int PythonFunctionWrapper< is_base_return_complete, ReturnType, Args...>::_init(PyObject* self, PyObject* args, PyObject*kwds){
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

    template<bool is_base_return_complete, typename ReturnType, typename... Args>
    PyObject* PythonFunctionWrapper<is_base_return_complete, ReturnType, Args...>::_call(PyObject *callable_object, PyObject *args, PyObject *kw){
      try{
        PythonFunctionWrapper& wrapper = *reinterpret_cast<PythonFunctionWrapper* const>(callable_object);
        typedef typename std::remove_pointer< typename extent_as_pointer< ReturnType>::type>::type BaseType;
        ReturnType result = wrapper.callFunc(args, kw, typename argGenerator<sizeof...(Args)>::type());
        const ssize_t array_size = sizeof(result)/sizeof(BaseType);
        return toPyObject<ReturnType, is_base_return_complete>( result, false, array_size );
      } catch( const char* const msg){
        PyErr_SetString(PyExc_RuntimeError, msg);

        return nullptr;
      }
    }

    /** specialize for void returns **/
    template< typename ...Args>
    struct PythonFunctionWrapper<true, void, Args...> {
        PyObject_HEAD

        typedef void(*func_type)(Args...);

        static PyTypeObject Type;

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
        PythonFunctionWrapper* create(const char* const func_name, func_type func, const char* const names[]){
            static bool inited = false;
            Py_ssize_t index = 0;
            PyTypeObject*  type = new PyTypeObject(Type);
            Py_INCREF(type);
            char* name = new char[strlen(func_name)+1];
            strcpy(name, func_name);
            type->tp_name = name;
           if (!inited && (PyType_Ready(type) < 0) ){
                throw "Unable to initialize python object for c function wrapper";
            } else {
                inited = true;
                 auto pyfuncobj = (PythonFunctionWrapper*)PyObject_CallObject((PyObject*)type, nullptr);
                pyfuncobj->_cfunc = func;
                while( names[index] ){
                    pyfuncobj->_kwlist.push_back( names[index++] );
                }
                pyfuncobj->_kwlist.push_back(nullptr);
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
        void callFuncBase( PyObject *pytuple, PyObject *kwds, PyO* ...pyargs){
            if(!_cfunc){
                PyErr_SetString(PyExc_RuntimeError, "Uninitialized C callable!");
                PyErr_Print();
                throw "Uninitialized C callable";
            }
            char format[sizeof...(Args)+1] = {0};
            memset(format, 'O', sizeof...(Args));
            if(!PyArg_ParseTupleAndKeywords(pytuple, kwds, format, (char**)_kwlist.data(), &pyargs...)){
              PyErr_Print();
              throw "Illegal arumgnet(s)";
            }
            _cfunc(*toCObject<Args, false, PythonClassWrapper<Args> >(*pyargs)...);
        }

        template<int ...S>
        void callFunc(PyObject* const tuple, PyObject* kw, container<S...> s) {
           (void)s;//only used for unpacking arguments into a list and determine the int... args to this template
           PyObject pyobjs[sizeof...(S)];
	   (void)pyobjs;
           callFuncBase(tuple, kw, &pyobjs[S]...);
        }


        func_type _cfunc;
        std::vector<const char*> _kwlist;
    };

    //Python definition of Type for this function wrapper
    template<  typename... Args >
    PyTypeObject PythonFunctionWrapper<true, void, Args...>::Type = {
        PyObject_HEAD_INIT(nullptr)
        0,                               /*ob_size*/
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

    template<typename... Args>
    int PythonFunctionWrapper< true, void, Args...>::_init(PyObject* self, PyObject* args, PyObject*kwds){
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

    template<typename... Args>
    PyObject* PythonFunctionWrapper<true, void, Args...>::_call(PyObject *callable_object, PyObject *args, PyObject *kw){
      try{
        PythonFunctionWrapper& wrapper = *reinterpret_cast<PythonFunctionWrapper* const>(callable_object);
        wrapper.callFunc(args, kw, typename argGenerator<sizeof...(Args)>::type());
        return Py_None;
      } catch( const char* const msg){
        PyErr_SetString(PyExc_RuntimeError, msg);
        PyErr_Print();
        return nullptr;
      }
    }

}

