
#ifndef __PYLLARS__INTERNAL__POINTER_H
#define __PYLLARS__INTERNAL__POINTER_H
#include <sys/types.h>
#include <type_traits>
namespace __pyllars_internal{

  template< typename CClass, bool is_complete = true, const ssize_t max=-1, typename content_type = typename std::remove_reference<CClass>::type* >
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


    namespace{
        constexpr ssize_t MAX_PTR_DEPTH = 100;
    }

    template< typename CClass,  bool is_complete, const ssize_t max, typename content_type>
    struct PythonCPointerWrapper {

        PyObject_HEAD

        typedef typename std::remove_reference<CClass>::type CClass_NoRef;

        static PyObject* parent_module;

        static int initialize(const char* const name, PyObject* module, const char* const fullname = nullptr){
           if(!name)
                return -1;
            static bool initialized = false;
            if (initialized) return 0;

            PyMethodDef pyMeth = {
                address_name,
                addr,
                METH_KEYWORDS,
                nullptr
            };
            std::string ptr_name = std::string(fullname?fullname:name) + '*';
            s_methodCollection.insert(s_methodCollection.begin(), pyMeth);
            Type[0].tp_methods = s_methodCollection.data();
            for(int depth = 0; depth < 100; ++depth){
                if (depth > 0) Type[depth] = Type[depth-1];

                char* new_name = new char[ptr_name.size() + 1];
                strcpy(new_name, ptr_name.c_str());
                Type[depth].tp_name = new_name;
                ptr_name += '*';
                if (PyType_Ready(&Type[depth]) < 0)
                    return -1;
                //reinterpret to avoid compiler warning:
                PyObject* const type = reinterpret_cast<PyObject*>(&Type[depth]);
                Py_INCREF(type);
            }
            initialized = true;
            if (module == nullptr)
                return 0;
            if (addToModule( name, module ) < 0)
                return -1;
            parent_module = module;

            return 0;
        }

        static PyObject* addr(PyObject* self, PyObject *args){
            if( (args&&PyTuple_Size(args)>0)){
                PyErr_BadArgument();
                return nullptr;
            }
            PyObject* obj= toPyObject<CClass_NoRef*, is_complete>( (CClass_NoRef*)&reinterpret_cast<PythonCPointerWrapper*>(self)->_content, false);
            reinterpret_cast<PythonCPointerWrapper*>(obj)->_depth =
                reinterpret_cast<PythonCPointerWrapper*>(self)->_depth+1;
            PyErr_Clear();
            return obj;
        }


        /**
         * Add a type definition to the given module
         * @return: 0 on success, negative otherwise
         **/
        static int addToModule( const char* const name, PyObject* const to_module) {
            PyModule_AddObject(to_module, name, (PyObject*)&Type);
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
                if (self->_referenced)
                    Py_DECREF(self->_referenced);
                self->_referenced = nullptr;
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
                self->_depth = ptr_depth<CClass>::value;
            }
            return (PyObject *)self;

        }

        static int _init( PythonCPointerWrapper *self, PyObject *args, PyObject *kwds) {
            int status = -1;
            if(!self) return status;
            self->_referenced = nullptr;
            self->_max = max;
            if( kwds && PyDict_Size(kwds)>1){
                PyErr_SetString(PyExc_TypeError, "Unexpected keyword argument(s) in Pointer cosntructor");
                status = -1;
                goto onerror;
            } else if (kwds && PyDict_Size(kwds)==1){
            PyObject* sizeItem;
            if ( (sizeItem = PyDict_GetItemString(kwds, "size"))){
                if (PyLong_Check( sizeItem)){
                    self->_max = PyLong_AsLongLong( sizeItem )-1;
                    if( self->_max < 0){
                        PyErr_SetString(PyExc_TypeError, "Invalid negative size value in Pointer constructor");
                            status = -1;
                            goto onerror;
                    }
                    } else {
                      PyErr_SetString(PyExc_TypeError, "Invalid type for size keyword argument in Pointer constructor");
                      status = -1;
                      goto onerror;
                    }
                } else {
                    PyErr_SetString(PyExc_TypeError, "Unexpected keword argument in Pointer constructor");
                    status = -1;
                    goto onerror;
                }
            }
            if (args && PyTuple_Size(args)>1){
                PyErr_SetString(PyExc_TypeError, "Excpect only one object in Pointer constructor");
                status = -1;
            } else if (args && PyTuple_Size(args)==1){
	        // we are asked to make a new pointer from an existing object:
                PyObject* pyobj = PyTuple_GetItem(args, 0);
                PyTypeObject * pytypeobj =  &PythonClassWrapper<CClass, is_complete>::Type;
                typedef typename std::remove_pointer< CClass>::type CClass_bare;
                if (PyObject_TypeCheck(pyobj, pytypeobj)) {
		    // if this is an instance of a basic class:
		    self->make_reference(pyobj);
                    self->_content = (content_type)reinterpret_cast<PythonClassWrapper<CClass, is_complete>*>(pyobj)->get_CObject();
                    self->_depth = ptr_depth<CClass>::value+1;
                    status = 0;
                } else {
		  bool found = false;
		  for (int i = 0 ; i < MAX_PTR_DEPTH; ++i){
		    PyTypeObject * pyptrtypeobj = &PythonCPointerWrapper<CClass_bare, is_complete, max, content_type>::Type[i];
		    if (PyObject_TypeCheck(pyobj, pyptrtypeobj)){
		      // take the pointer of a pointer:
		      self->_content = reinterpret_cast< PythonCPointerWrapper<CClass_bare, is_complete, max, content_type>*>(pyobj)->ptr();
		      self->_depth = i+1;
		      status = 0;
		      found = true;
		      break;
		    }
		  }
		  if( !found ) {
                    PyErr_SetString(PyExc_TypeError, "Invalid type for object when getting its pointer");
                    status = -1;
		  }
		}
            } else {
                self->_content = nullptr;
                self->_depth = 0;
                status = 0;
            }
        onerror:
            return status;
        }

        static PyTypeObject Type[MAX_PTR_DEPTH];

        content_type ptr(){
            return _content;
        }

        void make_reference( PyObject *obj){
            if(_referenced){
                throw "Making reference to object while refeence already exists";
            }
            Py_INCREF(obj);
            _referenced = obj;

        }


        void set_contents( CClass_NoRef * const contents, const ssize_t depth ){
            assert(depth > 0);//should never get to 0 value
            assert(!_content);

            _content = (content_type)contents;
            _depth = _content?depth+ptr_depth<CClass>::value:0;
        }

        static PyTypeObject* const parent_class;

    private:

        template< typename Class, bool>
        struct Dereferencer;

        template< typename Class>
        struct Dereferencer<Class, true>{
            static typename std::remove_reference<Class>::type * at( const Py_ssize_t index, typename std::remove_reference<Class>::type * const array){
                return &array[index];
            }
        };

       template< typename Class>
        struct Dereferencer<Class, false>{
            static Class * at( const Py_ssize_t index, Class* const array){
              throw "Cannot dereference pointer to incomplete type";
            }
        };


        static PyObject* _at( PythonCPointerWrapper * self, PyObject* args, PyObject* kwargs) {
            static const char* kwlist[] = {"index",nullptr};

            PyObject* result = Py_None;
            Py_ssize_t index = -1;

            //TODO Change "i" to be long long
            if( !PyArg_ParseTupleAndKeywords( args,kwargs, "i", (char**)kwlist, &index)) {
                // PyErr_SetString( PyExc_IndexError, "Unable to parse index");
                return nullptr;
            }

            index = (index < 0 && self->_max >=0)?self->_max - index +1:index;
            if ( self->_max >=0 && index > self->_max){
              PyErr_SetString( PyExc_IndexError, "Index out of bounds");
              return nullptr;
            }

            PythonCPointerWrapper* self_ = reinterpret_cast<PythonCPointerWrapper*>(self);
            if (self_->_depth == 0) {
                PyErr_SetString( PyExc_RuntimeError, "Attempt to dereference null C object");
                return nullptr;
            } else if (self_->_depth == 1) {
                PyObject* kw = PyDict_New();
                    PyDict_SetItemString(kw, "__internal_allow_null", PyBool_FromLong(true));
                    PyObject* emptyTuple = PyTuple_New(0);
                    result =  PyObject_Call( (PyObject*)&PythonClassWrapper<CClass, is_complete>::Type, emptyTuple, kw);
                    Py_DECREF(kw);
                    Py_DECREF(emptyTuple);
                    if(result)
                        reinterpret_cast<PythonClassWrapper<CClass, is_complete>* >(result)->_CObject =
                         Dereferencer<CClass_NoRef, is_complete>::at(index, ((typename std::remove_reference<CClass>::type*)(self_->_content)));
            } else {
                PyObject* emptyTuple = PyTuple_New(0);
                result = PyObject_CallObject( (PyObject*)&PythonCPointerWrapper::Type,
                                              emptyTuple);
                Py_DECREF(emptyTuple);
                if(result) {
                    PythonCPointerWrapper* result_ = reinterpret_cast<PythonCPointerWrapper*>(result);
                    PythonCPointerWrapper* self_ = reinterpret_cast<PythonCPointerWrapper*>(self);
                    result_->_content = (content_type) Dereferencer<content_type const , true>::at(index, reinterpret_cast<content_type const *  >(self_->_content));
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
        ssize_t _max;
        PyObject* _referenced;
    };

    template<typename T, bool is_complete, const ssize_t max, typename content_type>
    class PythonCPointerWrapper< T*, is_complete, max, content_type>: public PythonCPointerWrapper<T, is_complete, max, content_type>{
    public:
        // The primary definition of this class is enough to keep track of pointer depth, albeit in a perhaps fake way
        // But this allows declarations of based on a "truer" type that inherits to map down to the one class for definition
    };

    template< typename CClass, bool is_complete, const ssize_t max, typename content_type>
    PyTypeObject* const PythonCPointerWrapper<CClass, is_complete, max, content_type>::parent_class = &PythonClassWrapper<CClass, is_complete>::Type;

    template< typename CClass, bool is_complete,  const ssize_t max, typename content_type>
    PyObject* PythonCPointerWrapper<CClass, is_complete, max, content_type>::parent_module = nullptr;

    /**
     * https://www.google.com/search?client=ubuntu&channel=fs&q=python+api+get+module+of+Type&ie=utf-8&oe=utf-8
     * Specialized for pointers:
     **/
    template< typename T,  bool is_complete, const ssize_t max >
    class PyObjectConversionHelper<T, is_complete, max, typename  std::enable_if< !is_function_ptr<typename std::remove_pointer<T>::type >::value && std::is_pointer<T>::value >::type>{
    public:
        static PyObject* toPyObject( const T & var, const bool asArgument ){
            (void)asArgument;
            static constexpr ssize_t depth = ptr_depth<T>::value;
            //typedef typename std::remove_pointer<T>::type T_base;
            PyObject* pyobj =nullptr;
            if( std::is_pointer<T>::value ){
                typedef typename std::remove_pointer<T>::type T_base;
                if (!PythonCPointerWrapper<T_base, is_complete, max>::Type[depth].tp_name){
                    PythonCPointerWrapper<T_base, is_complete, max>::initialize((PythonClassWrapper<T_base, is_complete>::get_name()).c_str(),
                                                                    PythonClassWrapper<T_base, is_complete>::parent_module,
                       PythonClassWrapper<T_base, is_complete>::Type.tp_name);
                }
                if( PyType_Ready(&PythonCPointerWrapper<T_base, is_complete, max>::Type[depth]) < 0){
                    PyErr_SetString(PyExc_RuntimeError, "Error initializing pointer class type");
                    goto onerror;
                }

            }


            typedef PythonCPointerWrapper<typename std::remove_pointer<T>::type, is_complete, max> PtrWrapper;
            {
                PyObject* emptyTuple = PyTuple_New(0);
                pyobj = PyObject_CallObject( (PyObject*)&PtrWrapper::Type[ptr_depth<T>::value], emptyTuple);
                Py_DECREF(emptyTuple);
            }
            if ( !pyobj || !PyObject_TypeCheck(pyobj, &PtrWrapper::Type[depth])){
                PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                goto onerror;
            }
            reinterpret_cast<PtrWrapper*>(pyobj)-> set_contents( var , ptr_depth<T>::value +1);
            return pyobj;

        onerror:
            return Py_None;
        }
    };

   /**
     *  Specialized for pointer-to-pointer-to-function:
     **/
    template< typename T, bool is_complete, const ssize_t max >
    class PyObjectConversionHelper<T,  is_complete, max, typename  std::enable_if< is_function_ptr< typename std::remove_pointer<T>::type >::value >::type>{
    public:
        static PyObject* toPyObject( const T & var, const bool asArgument ){
            (void)asArgument;
            static constexpr ssize_t depth = ptr_depth<T>::value;
            //typedef typename std::remove_pointer<T>::type T_base;
            PyObject* pyobj =nullptr;
            if( std::is_pointer<T>::value ){
                typedef typename std::remove_pointer<T>::type T_base;
                if (!PythonCPointerWrapper<T_base, is_complete, max>::Type[depth].tp_name){
                    PythonCPointerWrapper<T_base, is_complete, max>::initialize((PythonClassWrapper<T_base, is_complete>::get_name()).c_str(), PythonClassWrapper<T_base, true>::parent_module,
                       PythonClassWrapper<T_base, is_complete>::Type.tp_name);
                }
                if( PyType_Ready(&PythonCPointerWrapper<T_base, is_complete, max>::Type[depth]) < 0){
                    PyErr_SetString(PyExc_RuntimeError, "Error initializing pointer class type");
                    goto onerror;
                }

            }


            typedef PythonCPointerWrapper<typename std::remove_pointer<T>::type, is_complete, max> PtrWrapper;
            {
                PyObject* emptyTuple = PyTuple_New(0);
                pyobj = PyObject_CallObject( (PyObject*)&PtrWrapper::Type[ptr_depth<T>::value], emptyTuple);
                Py_DECREF(emptyTuple);
            }
            if ( !pyobj || !PyObject_TypeCheck(pyobj, &PtrWrapper::Type[depth])){
                PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                goto onerror;
            }
            reinterpret_cast<PtrWrapper*>(pyobj)-> set_contents( *var , ptr_depth<T>::value +1);
            return pyobj;

        onerror:
            return Py_None;
        }
    };

    /**
     * Specialized for pointers:
     **/
    template< const ssize_t max>
    class PyObjectConversionHelper<const char*, true, max, typename  std::enable_if< std::is_pointer<char*>::value >::type >{
    public:
        static PyObject* toPyObject( const char* const & var, const bool asArgument ){
            (void)asArgument;
            if (!var){
                throw "NULL CHAR* encountered";
            }
            return PyString_FromString(var);

        }
    };

    template<const ssize_t max>
    class PyObjectConversionHelper< char*,  true, max, typename  std::enable_if< std::is_pointer<char*>::value >::type >{
    public:
        static PyObject* toPyObject( char* const & var, const bool asArgument ){
            (void)asArgument;
            if (!var){
                throw "NULL CHAR* encountered";
            }
            return PyString_FromString(var);

        }



    };


   template< typename CClass, bool is_complete,  const ssize_t max, typename content_type>
        PyTypeObject PythonCPointerWrapper<CClass, is_complete, max, content_type>::Type[MAX_PTR_DEPTH] = {{
        PyObject_HEAD_INIT(nullptr)
        0,                         /*ob_size*/
        nullptr,             /*tp_name*/  //set on call to initialize
        sizeof(PythonCPointerWrapper),             /*tp_basicsize*/
        0,                         /*tp_itemsize*/
        (destructor)PythonCPointerWrapper::_dealloc, /*tp_dealloc*/
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
        (initproc)PythonCPointerWrapper::_init,      /* tp_init */
        nullptr,                         /* tp_alloc */
        PythonCPointerWrapper::_new,                 /* tp_new */
        nullptr,                         /*tp_free*/ //TODO: Implement a free??
        nullptr,                         /*tp_is_gc*/
        nullptr,                         /*tp_bass*/
        nullptr,                         /*tp_mro*/
        nullptr,                         /*tp_cache*/
        nullptr,                         /*tp_subclasses*/
        nullptr,                          /*tp_weaklist*/
        nullptr,                          /*tp_del*/
        0,                          /*tp_version_tag*/
    }};

   template<typename CClass, bool is_complete,  const ssize_t max, typename content_type>
    PyMethodDef PythonCPointerWrapper<CClass, is_complete, max, content_type>::s_methodList[] = {
        {
            "at", (PyCFunction)PythonCPointerWrapper::_at,
            METH_VARARGS,
            "Return the dereferenced item at provided index"
        },
        {nullptr, nullptr, 0, nullptr}  /* Sentinel */
    };

    template<typename CClass, bool is_complete,  const ssize_t max, typename content_type>
    std::vector<PyMethodDef> PythonCPointerWrapper<CClass, is_complete, max, content_type>::s_methodCollection =
       std::vector<PyMethodDef>(PythonCPointerWrapper::s_methodList, PythonCPointerWrapper::s_methodList+2);

    struct UNUSED{
      PyObject_HEAD
    };
    template<typename CClass, bool is_complete,  const ssize_t max, typename content_type>
      PyMemberDef PythonCPointerWrapper<CClass, is_complete, max, content_type>::s_memberList[] = {
               {
                //  !!!!!!!!!CAUTION : CANNOT USE OFFSETOF HERE, SO HAVE TO COMPUTE, BUT IS
                //  !!!!!!!!!DEPENDENT ON LAYOUT
                  (char*)"depth", T_OBJECT_EX, sizeof(PythonCPointerWrapper) - sizeof(UNUSED) + sizeof(content_type), 0,
                  (char*)"depth of pointer"
                },
                {nullptr, 0, 0, 0, nullptr}/*Sentinel*/
    };

    template<typename CClass, bool is_complete, const ssize_t max, typename content_type>
    std::vector<PyMemberDef> PythonCPointerWrapper<CClass, is_complete, max, content_type>::s_memberCollection=
       std::vector<PyMemberDef>(PythonCPointerWrapper::s_memberList, PythonCPointerWrapper::s_memberList+2);



}

#endif
