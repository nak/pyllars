namespace __pyllars_internal{

    template< typename CClass, typename... Bases>
    struct PythonCPointerWrapper;

}

#ifndef __PYLLARS__INTERNAL__POINTER_H
#define __PYLLARS__INTERNAL__POINTER_H

#include <Python.h>
#include <structmember.h>
#include <limits.h>
#include <limits>
#include <vector>
#include <map>
#include <functional>
#include <typeinfo>

#include "pyllars/pyllars_classwrapper.h"

// TODO (jrusnak#1#): All adding of bases, but not through template parameter....
namespace __pyllars_internal {


////////////////////////////


template< typename CClass, typename... Bases>
struct PythonCPointerWrapper {

    PyObject_HEAD

    static int addType( const char* const name, PyTypeObject* const contained_type) {

        if (PyType_Ready(contained_type) < 0)
            return -1;

        Py_INCREF( contained_type );
        return PyObject_SetAttrString((PyObject*)&Type, name, (PyObject *)contained_type);

    }

    static void _dealloc( PythonCPointerWrapper* self) {
        if( self != nullptr ) {
            self->_content = nullptr;
            self->ob_type->tp_free((PyObject*)self);
        }
    }

    static PyObject *
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        PythonCPointerWrapper *self;

        self = (PythonCPointerWrapper*)type->tp_alloc(type, 0);
        if (self != nullptr) {
            self->_content = nullptr;
            self->_depth = 1;
        }
        return (PyObject *)self;

    }

    static int
    _init( PythonCPointerWrapper *self, PyObject *args, PyObject *kwds) {
        int status = -1;
        if( kwds && PyDict_Size(kwds)>0){
            PyErr_SetString(PyExc_TypeError, "Keyword argument(s) unexpected in Pointer cosntructor");
            status = -1;
            goto onerror;
        }
        if (args && PyTuple_Size(args)>1){
            PyErr_SetString(PyExc_TypeError, "Excpect only one object in Pointer constructor");
            status = -1;
        } else if (args && PyTuple_Size(args)==1){
            PyObject* pyobj = PyTuple_GetItem(args, 0);
            PyTypeObject * pytypeobj =  &PythonClassWrapper<CClass, Bases...>::Type;
            typedef typename std::remove_pointer< CClass>::type CClass_bare;
            PyTypeObject * pyptrtypeobj = &PythonCPointerWrapper<CClass_bare, Bases...>::Type;
            if (PyObject_TypeCheck(pyobj, pytypeobj)) {
                self->_content = reinterpret_cast< PythonClassWrapper<CClass, Bases...>*>(pyobj)->get_CObject();
                self->_depth = 1;
                status = 0;
            } else if (PyObject_TypeCheck(pyobj, pyptrtypeobj)){
                self->_content = reinterpret_cast< PythonCPointerWrapper<CClass_bare, Bases...>*>(pyobj)->ptr();
                self->_depth = 1;
                status = 0;
            } else {
                PyErr_SetString(PyExc_TypeError, "Invalid type for object when getting its pointer");
                status = -1;
            }
        } else {
            self->_content = nullptr;
            self->_depth = 0;
            status = 0;
        }
    onerror:
        return status;
    }


    static PyTypeObject Type;

    static const char* const name;

    void* ptr(){
        return _content;
    }
    void set_contents( typename std::remove_reference<CClass>::type * const contents){
        assert(!_content);
        _content = contents;
    }

    static const char* const get_name(){
        static std::string name_ = typeid(CClass).name() + std::string("_ptr");
        return name_.c_str();
    }

    static PyTypeObject* parent_class;

private:

    static PyObject* _at( PyObject * self, PyObject* args, PyObject* kwargs) {
        static const char* kwlist[] = {"index",nullptr};

        PyObject* result = Py_None;
        int index = -1;

        if( !PyArg_ParseTupleAndKeywords( args,kwargs, "i", (char**)kwlist, &index)) {
            return nullptr;
        }
        if (reinterpret_cast<PythonCPointerWrapper*>(self)->_depth == 1) {

            result =  PyObject_CallObject( (PyObject*)&PythonClassWrapper<CClass>::Type,
                                           Py_None);
            if(result)
                reinterpret_cast<PythonClassWrapper<CClass>* >(result)->_CObject = &((typename std::remove_reference<CClass>::type*)reinterpret_cast<PythonCPointerWrapper*>(self)->_content)[index];

        } else {

            result = PyObject_CallObject( (PyObject*)&PythonCPointerWrapper::Type,
                                          Py_None);
            if(result) {
                reinterpret_cast<PythonCPointerWrapper*>(result)->_content =
                    reinterpret_cast<void**>(reinterpret_cast<PythonCPointerWrapper*>(self)->_content)[index];
            }
        }

        return result;
    }

    static std::vector<PyMethodDef> s_methodCollection;
    static std::vector<PyMemberDef> s_memberCollection;
    static PyMethodDef s_methodList[];
    static PyMemberDef s_memberList[];

    //  !!!!!!CAUTION: If you change the layout you must consider changing
    //  !!!!!!The offset computation below
    void* _content;
    size_t _depth;
};

    template< typename CClass, typename ...Bases>
    PyTypeObject* PythonCPointerWrapper<CClass, Bases...>::parent_class = nullptr;

  /**https://www.google.com/search?client=ubuntu&channel=fs&q=python+api+get+module+of+Type&ie=utf-8&oe=utf-8
     * Specialized for pointers:
     **/
    template< typename T>
    class PyObjectConversionHelper<T,  typename  std::enable_if< std::is_pointer<T>::value >::type >{
    public:
        static PyObject* toPyObject( const T & var ){
            typedef typename std::remove_pointer<T>::type T_base;
            PyObject* pyobj =nullptr;
            if( std::is_pointer<T>::value ){
               if( !std::is_pointer<T>::value){
                    PythonClassWrapper<T>::addType("Pointer", &PythonCPointerWrapper<T>::Type);
               } else {
                    if( PyType_Ready(&PythonCPointerWrapper<T>::Type) < 0){
                        PyErr_SetString(PyExc_RuntimeError, "Error initializing pointer class type");
                        goto onerror;
                    }
                    PythonCPointerWrapper<T>::addType("Pointer", &PythonCPointerWrapper<T_base>::Type);
               }
            }

            if( PyType_Ready(&PythonCPointerWrapper<T>::Type) < 0){
                PyErr_SetString(PyExc_RuntimeError, "Error initializing pointer class type");
                goto onerror;
            }
            pyobj = PyObject_CallObject(  (PyObject*)&PythonCPointerWrapper<typename std::remove_pointer<T>::type>::Type, nullptr);
            if ( !pyobj || !PyObject_TypeCheck(pyobj, &PythonCPointerWrapper<typename std::remove_pointer<T>::type>::Type)){
                  PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                goto onerror;
            }
            reinterpret_cast<PythonCPointerWrapper<typename std::remove_pointer<T>::type>* >(pyobj)->set_contents( var );
            return pyobj;

            onerror:
                return Py_None;
        }
    };


template<typename CClass, typename ...Bases>
template< typename ...Args, const char * const ...names, typename ...PyO>
 std::remove_reference<CClass>* PythonClassWrapper<CClass, Bases...>::constructorC( PyObject *pytuple, PyObject *kwds, PyO*... pyargs){
    static char* kwlist[] = {names..., nullptr};
    char format[sizeof...(Args)+1] = {'O'};
    format[sizeof...(Args)] = 0;

    if(!PyArg_ParseTupleAndKeywords(pytuple, kwds, format, kwlist, pyargs...)){
      return nullptr;
    }
    return new CClass(toCObject<Args>(pyargs)...);
}


template< typename CClass,
          typename... Bases >
PyTypeObject PythonCPointerWrapper<CClass,  Bases...>::Type = {
    PyObject_HEAD_INIT(nullptr)
    0,                         /*ob_size*/
    PythonCPointerWrapper::name,             /*tp_name*/
    sizeof(PythonCPointerWrapper),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PythonCPointerWrapper<CClass,  Bases...>::_dealloc, /*tp_dealloc*/
    nullptr,                         /*tp_print*/
    nullptr,                         /*tp_getattr*/
    nullptr,                         /*tp_setattr*/
    nullptr,                         /*tp_compare*/
    nullptr,                         /*tp_repr*/
    nullptr,                         /*tp_as_number*/
    nullptr,                         /*tp_as_containeruence*/
    nullptr,                         /*tp_as_mapping*/
    nullptr,                         /*tp_hash */
    nullptr,                         /*tp_call*/
    nullptr,                         /*tp_str*/
    nullptr,                         /*tp_getattro*/
    nullptr,                         /*tp_setattro*/
    nullptr,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PythonCPointerWrapper object",           /* tp_doc */
    nullptr,		               /* tp_traverse */
    nullptr,		               /* tp_clear */
    nullptr,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    nullptr,		               /* tp_iter */
    nullptr,		               /* tp_iternext */
    PythonCPointerWrapper::s_methodCollection.data(),             /* tp_methods */
    PythonCPointerWrapper::s_memberCollection.data(),             /* tp_members */
    nullptr,                         /* tp_getset */
    nullptr,                         /* tp_base */
    nullptr,                         /* tp_dict */
    nullptr,                         /* tp_descr_get */
    nullptr,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PythonCPointerWrapper<CClass,  Bases...>::_init,      /* tp_init */
    nullptr,                         /* tp_alloc */
    PythonCPointerWrapper<CClass,  Bases...>::_new,                 /* tp_new */
};

template<typename CClass, typename ...Bases>
PyMethodDef PythonCPointerWrapper<CClass, Bases...>::s_methodList[] = {
    {
        "at", (PyCFunction)PythonCPointerWrapper::_at,
        METH_VARARGS,
        "Return the dereferenced item at provided index"
    },
    {nullptr}  /* Sentinel */
};
template<typename CClass, typename ...Bases>
std::vector<PyMethodDef> PythonCPointerWrapper<CClass, Bases...>::s_methodCollection =
   std::vector<PyMethodDef>(PythonCPointerWrapper::s_methodList, PythonCPointerWrapper::s_methodList+2);

struct UNUSED{
  PyObject_HEAD
};
template<typename CClass, typename ...Bases>
PyMemberDef PythonCPointerWrapper<CClass, Bases...>::s_memberList[] = { {
            //  !!!!!!!!!CAUTION : CANNOT USE OFFSETOF HERE, SO HAVE TO COMPUTE, BUT IS
            //  !!!!!!!!!DEPENDENT ON LAYOUT
              (char*)"depth", T_OBJECT_EX, sizeof(PythonCPointerWrapper) - sizeof(UNUSED) + sizeof(void*), 0,
              (char*)"depth of pointer"
            },
            {nullptr}/*Sentinel*/
};
template<typename CClass, typename ...Bases>
std::vector<PyMemberDef> PythonCPointerWrapper<CClass, Bases...>::s_memberCollection=
   std::vector<PyMemberDef>(PythonCPointerWrapper::s_memberList, PythonCPointerWrapper::s_memberList+2);;

#include <typeinfo>


template< typename CClass, typename ...Bases>
const char* const PythonCPointerWrapper<CClass, Bases...>::name = PythonCPointerWrapper<CClass, Bases...>::get_name();


template< typename CClass,
          typename... Bases >
void init_pyllars_pointer( const char* const name, PyObject* m ) {
    if (m == nullptr)
        return;
    if (PythonClassWrapper<CClass, Bases...>::addToModule( name, m ) < 0)
        return;
    PythonClassWrapper<CClass, Bases...>::parent_module = m;
    PythonCPointerWrapper<CClass, Bases...>::parent_class = &PythonClassWrapper<CClass, Bases...>::Type;
}

}

#endif
