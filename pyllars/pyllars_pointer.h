
#ifndef __PYLLARS__INTERNAL__POINTER_H
#define __PYLLARS__INTERNAL__POINTER_H
namespace __pyllars_internal{

    template< typename CClass, typename content_type=const void*>
    struct PythonCPointerWrapper;

}

#include <Python.h>
#include <structmember.h>
#include <limits.h>
#include <limits>
#include <vector>
#include <functional>
#include "pyllars/pyllars_classwrapper.h"

// TODO (jrusnak#1#): All adding of bases, but not through template parameter....
namespace __pyllars_internal {
    template<typename T>
    class Depth{
    public:
      static size_t __get_depth(){
	return 0;
      }
    };

   template<typename T>
   class Depth<T*>{
   public:
     static size_t __get_depth(){
        return Depth<T>::__get_depth()+1;
     }
   };


    template< typename CClass, typename content_type>
    struct PythonCPointerWrapper {

        PyObject_HEAD

        typedef typename std::remove_reference<CClass>::type CClass_NoRef;

        static PyObject* parent_module;

        static void initialize(const char* const name, PyObject* module){
            if (module == nullptr)
                return;
            if (addToModule( name, module ) < 0)
                return;
            parent_module = module;
        }

        static PyObject* addr(PyObject* self, PyObject *args){
            if( (args&&PyTuple_Size(args)>0)){
                PyErr_BadArgument();
                return nullptr;
            }
            PyObject* obj= toPyObject( (CClass_NoRef*)&reinterpret_cast<PythonCPointerWrapper*>(self)->_content, false);
            reinterpret_cast<PythonCPointerWrapper<CClass>*>(obj)->_depth = reinterpret_cast<PythonCPointerWrapper*>(self)->_depth+1;
            PyErr_Clear();
            return obj;
        }


        /**
         * Add a type definition to the given module
         * @return: 0 on success, negative otherwise
         **/
        static int addToModule( const char* const name, PyObject* const to_module) {
            Type.tp_name = name;

            PyMethodDef pyMeth = {
              address_name,
              addr,
              METH_KEYWORDS,
              nullptr
            };
            s_methodCollection.insert(s_methodCollection.begin(), pyMeth);
            Type.tp_methods = s_methodCollection.data();
            if (PyType_Ready(&Type) < 0)
                return -1;
            PyObject* const type = reinterpret_cast<PyObject*>(&Type);
            Py_INCREF(type);
            PyModule_AddObject(to_module, name, type);


            return 0;
        }

        static int addType( const char* const name, PyTypeObject* const contained_type) {

            if (PyType_Ready(contained_type) < 0)
                return -1;

            Py_INCREF( contained_type );
            return PyObject_SetAttrString((PyObject*)&Type, name, (PyObject *)contained_type);

        }

        static void _dealloc( PythonCPointerWrapper* self) {
            if( self != nullptr ) {
                self->_content = nullptr;
		self->_depth = 0;
                self->ob_type->tp_free((PyObject*)self);
            }
        }

        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
            (void)args;
            (void)kwds;
            PythonCPointerWrapper *self;

            self = (PythonCPointerWrapper*)type->tp_alloc(type, 0);
            if (self != nullptr) {
                self->_content = nullptr;
                self->_depth = 0;
            }
            return (PyObject *)self;

        }

        static int _init( PythonCPointerWrapper *self, PyObject *args, PyObject *kwds) {
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
                PyTypeObject * pytypeobj =  &PythonClassWrapper<CClass>::Type;
                typedef typename std::remove_pointer< CClass>::type CClass_bare;
                PyTypeObject * pyptrtypeobj = &PythonCPointerWrapper<CClass_bare>::Type;
                if (PyObject_TypeCheck(pyobj, pytypeobj)) {
                    self->_content = reinterpret_cast<PythonClassWrapper<CClass>*>(pyobj)->get_CObject();
                    self->_depth = Depth<CClass>::__get_depth()+1;
                    status = 0;
                } else if (PyObject_TypeCheck(pyobj, pyptrtypeobj)){
                    self->_content = reinterpret_cast< PythonCPointerWrapper<CClass_bare>*>(pyobj)->ptr();
                    self->_depth = Depth<CClass>::__get_depth()+1;
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

        content_type ptr(){
            return _content;
        }


        void set_contents( typename std::remove_reference<CClass>::type * const contents){
            assert(!_content);
            _content = contents;
	    if(_content){ _depth = Depth<CClass>::__get_depth()+1;}
        }

        static const char* get_name(){
            static std::string name_ = typeid(CClass).name() + std::string("_ptr");
            return name_.c_str();
        }

        static PyTypeObject* const parent_class;

    private:

        static PyObject* _at( PyObject * self, PyObject* args, PyObject* kwargs) {
            static const char* kwlist[] = {"index",nullptr};

            PyObject* result = Py_None;
            int index = -1;

            if( !PyArg_ParseTupleAndKeywords( args,kwargs, "i", (char**)kwlist, &index)) {
                return nullptr;
            }
            PythonCPointerWrapper* self_ = reinterpret_cast<PythonCPointerWrapper*>(self);
                if (self_->_depth == 0) {
                PyErr_SetString( PyExc_RuntimeError, "Attempt to dereference null C object");
            } else if (self_->_depth == 1) {
                PyObject* kw = PyDict_New();
                PyDict_SetItemString(kw, "__internal_allow_null", PyBool_FromLong(true));

                result =  PyObject_Call( (PyObject*)&PythonClassWrapper<CClass>::Type, nullptr, kw);
                Py_DECREF(kw);
                if(result)
                    reinterpret_cast<PythonClassWrapper<CClass>* >(result)->_CObject = &((typename std::remove_reference<CClass>::type*)(self_->_content))[index];
            } else {
                result = PyObject_CallObject( (PyObject*)&PythonCPointerWrapper::Type,
                                              nullptr);
                if(result) {
                    PythonCPointerWrapper* result_ = reinterpret_cast<PythonCPointerWrapper*>(result);
                    PythonCPointerWrapper* self_ = reinterpret_cast<PythonCPointerWrapper*>(self);
                    result_->_content = reinterpret_cast<const void* const*>(self_->_content)[index];
                    result_->_depth = self_->_depth-1;
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
        content_type _content;
        size_t _depth;
    };



    template< typename CClass, typename content_type>
    PyTypeObject* const PythonCPointerWrapper<CClass, content_type>::parent_class = &PythonClassWrapper<CClass>::Type;

    template< typename CClass, typename content_type>
    PyObject* PythonCPointerWrapper<CClass, content_type>::parent_module = nullptr;

    /**
     * https://www.google.com/search?client=ubuntu&channel=fs&q=python+api+get+module+of+Type&ie=utf-8&oe=utf-8
     * Specialized for pointers:
     **/
    template< typename T>
    class PyObjectConversionHelper<T,  typename  std::enable_if< std::is_pointer<T>::value >::type >{
    public:
        static PyObject* toPyObject( const T & var, const bool asArgument ){
            (void)asArgument;
            typedef typename std::remove_pointer<T>::type T_base;
            PyObject* pyobj =nullptr;
            if( std::is_pointer<T>::value ){

                if( PyType_Ready(&PythonCPointerWrapper<T_base>::Type) < 0){
                    PyErr_SetString(PyExc_RuntimeError, "Error initializing pointer class type");
                    goto onerror;
                }
                PythonClassWrapper<T_base>::addType("Pointer", &PythonCPointerWrapper<T_base>::Type);

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

  /**
     * https://www.google.com/search?client=ubuntu&channel=fs&q=python+api+get+module+of+Type&ie=utf-8&oe=utf-8
     * Specialized for pointers:
     **/
   template<>
    class PyObjectConversionHelper<const char*,  typename  std::enable_if< std::is_pointer<char*>::value >::type >{
    public:
        static PyObject* toPyObject( const char* const & var, const bool asArgument ){
            (void)asArgument;
            if (!var){
                throw "NULL CHAR* encountered";
            }
            return PyString_FromString(var);

        }
    };

   template<>
    class PyObjectConversionHelper< char*,  typename  std::enable_if< std::is_pointer<char*>::value >::type >{
    public:
        static PyObject* toPyObject( char* const & var, const bool asArgument ){
            (void)asArgument;
            if (!var){
                throw "NULL CHAR* encountered";
            }
            return PyString_FromString(var);

        }
    };


    template< typename CClass, typename content_type>
    PyTypeObject PythonCPointerWrapper<CClass, content_type>::Type = {
        PyObject_HEAD_INIT(nullptr)
        0,                         /*ob_size*/
        PythonCPointerWrapper::name,             /*tp_name*/
        sizeof(PythonCPointerWrapper),             /*tp_basicsize*/
        0,                         /*tp_itemsize*/
        (destructor)PythonCPointerWrapper<CClass>::_dealloc, /*tp_dealloc*/
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
        (initproc)PythonCPointerWrapper<CClass>::_init,      /* tp_init */
        nullptr,                         /* tp_alloc */
        PythonCPointerWrapper<CClass>::_new,                 /* tp_new */
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

    template<typename CClass, typename content_type>
    PyMethodDef PythonCPointerWrapper<CClass, content_type>::s_methodList[] = {
        {
            "at", (PyCFunction)PythonCPointerWrapper::_at,
            METH_VARARGS,
            "Return the dereferenced item at provided index"
        },
        {nullptr, nullptr, 0, nullptr}  /* Sentinel */
    };

    template<typename CClass, typename content_type>
    std::vector<PyMethodDef> PythonCPointerWrapper<CClass, content_type>::s_methodCollection =
       std::vector<PyMethodDef>(PythonCPointerWrapper::s_methodList, PythonCPointerWrapper::s_methodList+2);

    struct UNUSED{
      PyObject_HEAD
    };
    template<typename CClass, typename content_type>
    PyMemberDef PythonCPointerWrapper<CClass, content_type>::s_memberList[] = {
               {
                //  !!!!!!!!!CAUTION : CANNOT USE OFFSETOF HERE, SO HAVE TO COMPUTE, BUT IS
                //  !!!!!!!!!DEPENDENT ON LAYOUT
                  (char*)"depth", T_OBJECT_EX, sizeof(PythonCPointerWrapper) - sizeof(UNUSED) + sizeof(void*), 0,
                  (char*)"depth of pointer"
                },
                {nullptr, 0, 0, 0, nullptr}/*Sentinel*/
    };

    template<typename CClass, typename content_type>
    std::vector<PyMemberDef> PythonCPointerWrapper<CClass, content_type>::s_memberCollection=
       std::vector<PyMemberDef>(PythonCPointerWrapper::s_memberList, PythonCPointerWrapper::s_memberList+2);

    template< typename CClass, typename content_type>
    const char* const PythonCPointerWrapper<CClass, content_type>::name = PythonCPointerWrapper<CClass, content_type>::get_name();



}

#endif
