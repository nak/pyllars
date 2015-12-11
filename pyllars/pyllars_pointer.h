#ifndef __PYLLARS__INTERNAL__POINTER_H
#define __PYLLARS__INTERNAL__POINTER_H
#include <sys/types.h>
#include <type_traits>


#include <Python.h>
#include <structmember.h>
#include <limits.h>
#include <limits>
#include <vector>
#include <functional>
#include "pyllars_defns.h"
#include "pyllars_classwrapper.h"
// TODO (jrusnak#1#): All adding of bases, but not through template parameter....

namespace __pyllars_internal {


    namespace{
        constexpr ssize_t MAX_PTR_DEPTH = 20;
    }


    template<typename T, bool is_complete, const ssize_t max>
    class PythonCPointerWrapper< T, is_complete, max, ptr_depth<ZERO_DEPTH> >: public PythonClassWrapper<T, is_complete>{
    public:
        PythonCPointerWrapper(): PythonClassWrapper<T, is_complete>(),
           _content(*PythonClassWrapper<T, is_complete>::_CObject){
        }
        T& _content;
    };

    template< bool is_complete, const ssize_t max>
    class PythonCPointerWrapper< void, is_complete, max, ptr_depth<ZERO_DEPTH> >: public PythonClassWrapper<void, is_complete>{
    public:
        void* _content;
    };


    template< typename CClass,  bool is_complete, const ssize_t max>
    struct PythonCPointerWrapper<CClass, is_complete, max, ptr_depth<CClass> > {

        PyObject_HEAD

        static constexpr size_t depth = ptr_depth<CClass>::value+1;

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
            Type.tp_methods = s_methodCollection.data();

            char* new_name = new char[ptr_name.size() + 1];
            strcpy(new_name, ptr_name.c_str());
            Type.tp_name = new_name;
            ptr_name += '*';
            if (PyType_Ready(&Type) < 0)
                return -1;
            //reinterpret to avoid compiler warning:
            PyObject* const type = reinterpret_cast<PyObject*>(&Type);
            Py_INCREF(type);

            initialized = true;
            if (module == nullptr)
                return 0;
            PyModule_AddObject(module, name, (PyObject*)&Type);

            parent_module = module;

            return 0;
        }

        static PyObject* addr(PyObject* self, PyObject *args){
            if( (args&&PyTuple_Size(args)>0)){
                PyErr_BadArgument();
                return nullptr;
            }
            PyObject* obj = nullptr;
            if(depth < MAX_PTR_DEPTH){
                obj = toPyObject< typename PtrToPtrTo<CClass>::type, is_complete>( &reinterpret_cast<PythonCPointerWrapper*>(self)->_content, false);
            } else {
                PyErr_SetString(PyExc_TypeError, "Max pointer depth reached");
            }
            PyErr_Clear();
            return obj;
        }


      /*
        static int addType( const char* const name, PyTypeObject* const contained_type) {

            if (PyType_Ready(contained_type) < 0)
                return -1;

            Py_INCREF( contained_type );
            return PyObject_SetAttrString((PyObject*)&Type, name, (PyObject *)contained_type);

        }*/

        static void _dealloc( PythonCPointerWrapper* self) {
            if( self != nullptr ) {
                if (self->_referenced)
                    Py_DECREF(self->_referenced);
                self->_referenced = nullptr;
                self->_content = nullptr;
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
                if( max > 0){
                    PyErr_SetString(PyExc_RuntimeError, "Attempt to dynamically set size on fixed-size array");
                    status = -1;
                    goto onerror;
                }
                if (PyLong_Check( sizeItem)){
                    //if size arg is truly an integer and is positive or zero, set the property here (and if not a fixed-size array)
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
            //if have an argument, set pointer value, otherwise set to nullptr
            if (args && PyTuple_Size(args)>1){
                PyErr_SetString(PyExc_TypeError, "Excpect only one object in Pointer constructor");
                status = -1;
            } else if (args && PyTuple_Size(args)==1){
                // we are asked to make a new pointer from an existing object:
                PyObject* pyobj = PyTuple_GetItem(args, 0);
                typedef PythonCPointerWrapper<typename std::remove_pointer<CClass>::type, is_complete, -1, ptr_depth<typename rm_ptr<CClass>::type > > PtrWrapper;
                PyTypeObject * pytypeobj =  &PtrWrapper::Type;

                if (PyObject_TypeCheck(pyobj, pytypeobj)) {
                    // if this is an instance of a basic class:
                    self->make_reference(pyobj);
                    self->_content = &reinterpret_cast<PtrWrapper*>(pyobj)->_content;
                    status = 0;
                } else {
                    PyErr_SetString(PyExc_TypeError, "Mismatched types when assigning pointer");
                    status = -1;
                    goto onerror;
                }
             } else {
                self->_content = nullptr;
                status = 0;
            }
        onerror:
            return status;
        }

        CClass* get_CObject(){
            return _content;
        }

        static PyObject* _at( PythonCPointerWrapper * self, PyObject* args, PyObject* kwargs) {
            static const char* kwlist[] = {"index",nullptr};

            Py_ssize_t index = -1;

            //TODO Change "i" to be long long
            if( !PyArg_ParseTupleAndKeywords( args,kwargs, "i", (char**)kwlist, &index)) {
                 PyErr_SetString( PyExc_TypeError, "Unable to parse index");
                return nullptr;
            }

            index = (index < 0 && self->_max >=0)?self->_max - index +1:index;
            if ( self->_max >=0 && index > self->_max){
              PyErr_SetString( PyExc_IndexError, "Index out of bounds");
              return nullptr;
            }
            return Setter<CClass>::getObjectAt( self->_content, index);

        }

        static PyTypeObject Type;

       void set_contents( CClass* ptr){
            _content = ptr;
        }


        void make_reference( PyObject *obj){
            if(_referenced){
                throw "Making reference to object while refeence already exists";
            }
            Py_INCREF(obj);
            _referenced = obj;

        }

//       static PyTypeObject* const parent_class;
        template<class T, bool is_complete2, const ssize_t max2, typename  Z>
        friend struct PythonCPointerWrapper;


       void set_contents_at(  const size_t index, CClass const *contents ){
            assert(!_content);
            Setter<CClass>::set(index, _content, contents);
        }

    private:

        template <typename T, typename Z = void >
        class Setter;

        template< typename T>
        class Setter<T, typename std::enable_if< std::is_copy_constructible<T>::value>::type >{
        public:
            static void set( T* const to, T* from, const size_t index){
                *to->_content = from[index];
            }

            static PyObject* getObjectAt( T* const from, const size_t index){
                //TODO add reference to owning element to this object to not have go out of scope
                //until parent does!!!!
                return toPyObject<T, is_complete>(from[index], true);
            }

            static void set( const size_t index, T* const to, const T* const from){
                to[index] = *from;
            }
        };

       template< typename T>
        class Setter<T, typename std::enable_if<!std::is_void<T>::value && !std::is_copy_constructible<T>::value>::type >{
        public:
            static void set( T* const to, T* from, const size_t index){
                throw "Setting void type";
            }
           static PyObject* getObjectAt( T* const from, const size_t index){
                throw "getting array lement of incomplete type!";
            }

            static void set( const size_t index, T* const to, const T* const from){
                throw "setting void type";
            }
        };

        template< typename T>
        class Setter<T, typename std::enable_if< std::is_void<T>::value >::type >{
        public:
            static void set( T* const to, T* from, const size_t index){
                throw "Setting void type";
            }
           static PyObject* getObjectAt( T* const from, const size_t index){
                throw "Getting a void type makes no sense!";
            }

            static void set( const size_t index, T* const to, const T* const from){
                throw "setting void type";
            }
        };

        template < typename T, typename E = void>
        class PtrToPtrTo;

        //limit pointer depth:
        template <typename T>
        class PtrToPtrTo< T, typename std::enable_if< (ptr_depth<T>::value < MAX_PTR_DEPTH) >::type >{
        public:
            typedef T** type;
        };

        template <typename T>
        class PtrToPtrTo< T, typename std::enable_if< (ptr_depth<T>::value >= MAX_PTR_DEPTH) >::type >{
        public:
            typedef void* type;
        };


        static std::vector<PyMethodDef> s_methodCollection;
        static std::vector<PyMemberDef> s_memberCollection;
        static PyMethodDef s_methodList[];

        //  !!!!!!CAUTION: If you change the layout you must consider changing
        //  !!!!!!The offset computation below
        CClass* _content;
        ssize_t _max;
        PyObject* _referenced;
    };


    template<typename T, bool is_complete, const ssize_t max>
    class PythonCPointerWrapper< T*, is_complete, max, typename std::enable_if< !(ptr_depth<T>::value <MAX_PTR_DEPTH), ptr_depth<T> >::type >{
    public:
       //Don't allow infinite recursive pointer defn.
    };


    template< typename CClass, bool is_complete,const ssize_t max>
    PyObject* PythonCPointerWrapper<CClass, is_complete, max,  ptr_depth<CClass> >::parent_module = nullptr;

    /**
     * Specialized for pointers:
     **/
    template< typename T,  bool is_complete, const ssize_t max >
    class PyObjectConversionHelper<T, is_complete, max, typename  std::enable_if< !is_function_ptr<typename std::remove_pointer<T>::type >::value && std::is_pointer<T>::value >::type>{
    public:
        static PyObject* toPyObject( const T & var, const bool asArgument ){
            (void)asArgument;
            PyObject* pyobj =nullptr;
            typedef typename std::remove_pointer<T>::type T_base;
            typedef PythonCPointerWrapper<T_base, is_complete, max> PtrWrapper;
           if (!PtrWrapper::Type.tp_name){
                PtrWrapper::initialize((PythonClassWrapper<T_base, is_complete>::get_name()).c_str(),
                                                                             PythonClassWrapper<T_base, is_complete>::parent_module,
                                                                             PythonClassWrapper<T, is_complete>::Type.tp_name);
            }
            if( PyType_Ready(&PtrWrapper::Type) < 0){
                PyErr_SetString(PyExc_RuntimeError, "Error initializing pointer class type");
                goto onerror;
            }
            if (PtrWrapper::parent_module){
              PyModule_AddObject( PtrWrapper::parent_module,
                          PtrWrapper::Type.tp_name,
                          (PyObject*) &PtrWrapper::Type );
            }

            {
                PyObject* emptyTuple = PyTuple_New(0);
                pyobj = PyObject_CallObject( (PyObject*)&PtrWrapper::Type, emptyTuple);
                Py_DECREF(emptyTuple);
            }
            if ( !pyobj || !PyObject_TypeCheck(pyobj, &PtrWrapper::Type)){
                PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                goto onerror;
            }
            reinterpret_cast<PtrWrapper*>(pyobj)->set_contents (var);
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
            //typedef typename std::remove_pointer<T>::type T_base;
            PyObject* pyobj =nullptr;
            if( std::is_pointer<T>::value ){
                typedef typename std::remove_pointer<T>::type T_base;
                if (!PythonCPointerWrapper<T_base, is_complete, max>::Type.tp_name){
                    PythonCPointerWrapper<T_base, is_complete, max>::initialize((PythonClassWrapper<T_base, is_complete>::get_name()).c_str(), PythonClassWrapper<T_base, is_complete>::parent_module,
                       PythonClassWrapper<T_base, is_complete>::Type.tp_name);
                }
                if( PyType_Ready(&PythonCPointerWrapper<T_base, is_complete, max>::Type) < 0){
                    PyErr_SetString(PyExc_RuntimeError, "Error initializing pointer class type");
                    goto onerror;
                }

            }


            typedef PythonCPointerWrapper<typename std::remove_pointer<T>::type, is_complete, max> PtrWrapper;
            {
                PyObject* emptyTuple = PyTuple_New(0);
                pyobj = PyObject_CallObject( (PyObject*)&PtrWrapper::Type, emptyTuple);
                Py_DECREF(emptyTuple);
            }
            if ( !pyobj || !PyObject_TypeCheck(pyobj, &PtrWrapper::Type)){
                PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                goto onerror;
            }
            reinterpret_cast<PtrWrapper*>(pyobj)-> set_contents( var);
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


    template< typename CClass, bool is_complete, const ssize_t max>
    PyTypeObject PythonCPointerWrapper<CClass, is_complete, max, ptr_depth<CClass> >::Type= {
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
        nullptr,             /* tp_methods */
        nullptr,             /* tp_members */
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
    };

    template<typename CClass, bool is_complete, const ssize_t max>
    PyMethodDef PythonCPointerWrapper<CClass, is_complete, max, ptr_depth<CClass> >::s_methodList[] = {
        {
            "at", (PyCFunction)PythonCPointerWrapper<CClass, is_complete, max, ptr_depth<CClass> >::_at,
            METH_VARARGS,
            "Return the dereferenced item at provided index"
        },
        {nullptr, nullptr, 0, nullptr}  /* Sentinel */
    };

    template<typename CClass, bool is_complete, const ssize_t max>
    std::vector<PyMethodDef> PythonCPointerWrapper<CClass, is_complete, max>::s_methodCollection =
       std::vector<PyMethodDef>(PythonCPointerWrapper::s_methodList, PythonCPointerWrapper::s_methodList+2);

    struct UNUSED{
      PyObject_HEAD
    };

    template<typename CClass, bool is_complete, const ssize_t max>
    std::vector<PyMemberDef> PythonCPointerWrapper<CClass, is_complete,  max>::s_memberCollection;



}

#endif
