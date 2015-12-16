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
    constexpr size_t MAX_PTR_DEPTH=20;
  }

  struct WrapperBase: public CommonBaseWrapper{
        static PyObject* parent_module;

        static int initialize( const char* const name, const char* const module_entry_name,

                             PyObject* module, const char* const fullname,
                             PyCFunction addr_method, PyCFunctionWithKeywords element_method, PyTypeObject& Type){
            if(!name)
                return -1;


            std::string ptr_name = std::string(fullname?fullname:name);
            Type.tp_methods[0].ml_meth = addr_method;
            Type.tp_methods[1].ml_meth = (PyCFunction)element_method;

            char* new_name = new char[ptr_name.size() + 1];
            strcpy(new_name, ptr_name.c_str());
            Type.tp_name = new_name;

            if (PyType_Ready(&Type) < 0){
                PyErr_Print();
                return -1;
            }
            //reinterpret to avoid compiler warning:
            PyObject* const type = reinterpret_cast<PyObject*>(&Type);
            Py_INCREF(type);

            if (module == nullptr)
                return 0;
            PyModule_AddObject(module, module_entry_name, (PyObject*)&Type);

            parent_module = module;

            return 0;
        }

        template <typename T, const bool is_complete>
        static PyObject* _addr(PyObject* self, PyObject *args, T* cobj, const size_t depth){
          try{
            if( (args&&PyTuple_Size(args)>0)){
                PyErr_BadArgument();
                return nullptr;
            }
            if ( ptr_depth<T*>::value >= MAX_PTR_DEPTH)
              throw "Maximum pointer depth on dynamic pointer-type creation reached";
            PyObject* pyobj = nullptr;
            pyobj = toPyObject< typename PtrTo<T>::type, is_complete>( (typename PtrTo<T>::type) cobj, false);

            PyErr_Clear();
            return pyobj;
          } catch(const char* const msg){
            PyErr_SetString( PyExc_RuntimeError, msg);
            return nullptr;
          }
        }

        template<typename T, const bool is_complete>
        inline static PyObject* addr(PyObject* self, PyObject *args){
          try{
            return WrapperBase::_addr<T, is_complete>
               (self, args, (T*) reinterpret_cast<WrapperBase*>(self)->_all_content._untyped_content, ptr_depth<T>::value+1);
          } catch(const char* const msg){
            PyErr_SetString( PyExc_RuntimeError, msg);
            return nullptr;
          }
        }

        template<typename T, const bool is_complete>
        static PyObject* _at( PyObject* self_, PyObject* args, PyObject* kwargs) {
          try{
            WrapperBase * const self = (WrapperBase*) self_;
            const Py_ssize_t index = WrapperBase::get_array_index(self, args, kwargs);
            CommonBaseWrapper* result = (CommonBaseWrapper*)(index<0?nullptr:WrapperBase::Setter<T, is_complete>::getObjectAt( (T*) &self->_all_content._untyped_content, index));
            result->make_reference( (PyObject*)self );
            return (PyObject*) result;
          } catch(const char* const msg){
            PyErr_SetString( PyExc_RuntimeError, msg);
            return nullptr;
          }
        }

        static void _dealloc( WrapperBase* self) {
            //TODO: track dynamically allocated content and free if so
            if( self != nullptr ) {
                if (self->_referenced)
                    Py_DECREF(self->_referenced);
                self->_referenced = nullptr;
                self->_all_content._untyped_content = nullptr;
                self->_all_content._func_content = nullptr;
                self->baseClass.ob_type->tp_free((PyObject*)self);
                if (self->_referenced){
                  Py_XDECREF( self->_referenced );
                  self->_referenced = nullptr;
                }            
            }
        }

        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
            (void)args;
            (void)kwds;
            WrapperBase *self;

            self = (WrapperBase*)type->tp_alloc(type, 0);
            if (self != nullptr) {
                self->_all_content._untyped_content = nullptr;
                self->_all_content._func_content = nullptr;
                /*if( ((PyObject*)self)->ob_type->tp_base && PythonBase::TypePtr->tp_new){
                  PyObject* empty = PyTuple_New(0);
                  PythonBase::TypePtr->tp_new( ((PyObject*)self)->ob_type->tp_base, empty, nullptr);
                  Py_DECREF( empty );
                  }*/
            }
            return (PyObject *)self;

        }

        static int _init( WrapperBase *self, PyObject *args, PyObject *kwds, PyTypeObject* pytypeobj, const size_t max) {
            int status = -1;
            if(!self) return status;
            self->_referenced = nullptr;
            self->_max = max;
            if( ((PyObject*)self)->ob_type->tp_base && PythonBase::TypePtr->tp_init){
              PyObject* empty = PyTuple_New(0);
              PythonBase::TypePtr->tp_init( (PyObject*)&self->baseClass, empty, nullptr);
              Py_DECREF( empty );
            }
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

                if (PyObject_TypeCheck(pyobj, pytypeobj)) {
                    // if this is an instance of a basic class:
                    self->make_reference(pyobj);
                    self->_all_content = reinterpret_cast<WrapperBase*>(pyobj)->_all_content;
                    status = 0;
                } else {
                    PyErr_SetString(PyExc_TypeError, "Mismatched types when assigning pointer");
                    status = -1;
                    goto onerror;
                }
             } else {
                self->_all_content._untyped_content = nullptr;
                self->_all_content._func_content = nullptr;
                status = 0;
            }
        onerror:
            return status;
        }


        static Py_ssize_t get_array_index( WrapperBase * self, PyObject* args, PyObject* kwargs) {
            static const char* kwlist[] = {"index",nullptr};
            static constexpr Py_ssize_t INVALID_INDEX = -1;
            long index = -1;

            if( !PyArg_ParseTupleAndKeywords( args,kwargs, "l", (char**)kwlist, &index)) {
                 PyErr_SetString( PyExc_TypeError, "Unable to parse index");
                return INVALID_INDEX;
            }

            index = (index < 0 && self->_max >=0)?self->_max - index +1:index;
            if (( self->_max >=0 && index > self->_max) || index < 0){
              PyErr_SetString( PyExc_IndexError, "Index out of bounds");
              return INVALID_INDEX;
            }
            return index;
        }


    protected:

        template <typename T, const bool is_complete, typename Z = void >
        class Setter;

        template< typename T, const bool is_complete>
        class Setter<T, is_complete, typename std::enable_if< !std::is_pointer<T>::value &&
                                                                std::is_copy_constructible<T>::value &&
                                                                std::is_assignable<T, T>::value>::type >{
        public:


            static void set( const size_t index, T* const to, const T* const from){
                to[index] = *from;
            }
        };

        template< typename T, const bool is_complete>
        class Setter<T, is_complete, typename std::enable_if< !std::is_pointer<T>::value &&
                                                              !std::is_array<T>::value &&
                                                              std::is_copy_constructible<T>::value &&
                                                              !std::is_assignable<T, T>::value >::type >{
        public:


            static void set( const size_t index, T* const to, const T* const from){
              throw "Object is not assignable";
            }
        };

        template< typename T, const bool is_complete>
        class Setter<T, is_complete, typename std::enable_if< !std::is_pointer<T>::value &&
                                                              !std::is_array<T>::value &&
                                                              !std::is_void<T>::value &&
                                                              !std::is_copy_constructible<T>::value>::type >{
        public:


            static void set( const size_t index, T* const to, const T* const from){
                throw "setting void type";
            }
        };

        template< typename T, const bool is_complete>
        class Setter<T, is_complete, typename std::enable_if< !std::is_array<T>::value &&
                                                              !std::is_void<T>::value &&
                                                              !std::is_copy_constructible<T>::value>::type >{
        public:

            static PyObject* getObjectAt( T** const from, const size_t index){
                throw "Getting element from incomplete type";
            }

            static void set( const size_t index, T* const to, const T* const from){
                throw "setting void type";
            }
        };


        template< typename T, bool is_complete>
        class Setter<T, is_complete, typename std::enable_if< std::is_void<T>::value >::type >{
        public:

            static void set( const size_t index, T* const to, const T* const from){
                throw "setting void type";
            }
        };

        template< typename T, const bool is_complete>
        class Setter<T*, is_complete, typename std::enable_if< !std::is_const<T>::value &&
                                                                std::is_copy_constructible<T>::value &&
                                                               !std::is_void< T >::value>::type >{
        public:

            static PyObject* getObjectAt( T** const from, const size_t index){
              //TODO add reference to owning element to this object to not have go out of scope
              //until parent does!!!!
              return toPyObject<T, is_complete>((*from)[index], false);
            }

            static void set( const size_t index, T** const to, T** const from){
                to[index] = *from;
            }
        };

        template< typename T, const bool is_complete>
        class Setter<const T*, is_complete, typename std::enable_if< std::is_copy_constructible<T>::value &&
                                                                     !std::is_void< T >::value>::type >{
        public:

            static PyObject* getObjectAt( const T** const from, const size_t index){
              //TODO add reference to owning element to this object to not have go out of scope
              //until parent does!!!!
              return toPyObject<T, is_complete>((*from)[index], false);
            }

            static void set( const size_t index, const T** const to, const T** const from){
                throw "Attempt to set const element in array";
            }
        };
       template< typename T, const bool is_complete>
        class Setter<const T* const, is_complete, typename std::enable_if< std::is_copy_constructible<T>::value &&
                                                                     !std::is_void< T >::value>::type >{
        public:

            static PyObject* getObjectAt( const T* const* const from, const size_t index){
              //TODO add reference to owning element to this object to not have go out of scope
              //until parent does!!!!
              return toPyObject<T, is_complete>((*from)[index], false);
            }

            static void set( const size_t index, const T* const* const to,  const T* const* const from){
                throw "Attempt to set const element in array";
            }
        };

        template< typename T, const bool is_complete>
        class Setter<T*, is_complete, typename std::enable_if< !std::is_copy_constructible<T>::value >::type >{
        public:

            static PyObject* getObjectAt( T** const from, const size_t index){
              throw "Getting element of incomplete or non-copiable element type";
            }

            static void set( const size_t index, T** const to, const T** const from){
                to[index] = *from;
            }
        };

        template< typename T, const size_t size, const bool is_complete>
        class Setter<T[size], is_complete, typename std::enable_if< std::is_copy_constructible<T>::value >::type >{
        public:
            typedef T T_array[size];
            static PyObject* getObjectAt( T_array* const from, const size_t index){
              //TODO add reference to owning element to this object to not have go out of scope
              //until parent does!!!!
              return toPyObject<T, is_complete>((*from)[index], true);
            }

            static void set( const size_t index, T_array* const to, const T_array* const from){
                for (size_t i = 0; i < size; ++i)
                    to[index][i] = (*from)[i];
            }
        };

        template< typename T, const size_t size, const bool is_complete>
        class Setter<T[size], is_complete, typename std::enable_if< !std::is_copy_constructible<T>::value >::type >{
        public:
            typedef T T_array[size];
            static PyObject* getObjectAt( T_array* const from, const size_t index){
              throw "Getting element of incomplete or non-copiable element type";
            }

            static void set( const size_t index, T_array* const to, const T_array* const from){
                for (size_t i = 0; i < size; ++i)
                    to[index][i] = (*from)[i];
            }
        };

        template<  const bool is_complete>
        class Setter<void*, is_complete, void >{
        public:

            static void set( const size_t index, void** const to,  void** const from){
                to[index] = *from;
            }

           static PyObject* getObjectAt( void** const from, const size_t index){
              throw "Attempt to get void value";
            }
        };


        template < typename T, typename E = void>
        class PtrTo;

        //limit pointer depth:
        template <typename T>
        class PtrTo< T, typename std::enable_if< (!std::is_function<T>::value) && (ptr_depth<T>::value < MAX_PTR_DEPTH) >::type >{
        public:
            typedef T* type;

            static T*
            cast( T const * t){ return const_cast<T*>(t);}
        };

        template <typename T>
        class PtrTo< T, typename std::enable_if< (ptr_depth<T>::value >= MAX_PTR_DEPTH) >::type >{
        public:
            typedef void* type;

            static T*
            cast( T const * t){ return nullptr;}
       };



        template <typename ReturnType, typename ...Args >
        class PtrTo< ReturnType(Args...), void >{
        public:
            typedef ReturnType(*functype)(Args...);
            typedef functype* type;
            static functype
            cast( functype   t){ return t;}
        };


        //  !!!!!!CAUTION: If you change the layout you must consider changing
        //  !!!!!!The offset computation below
        typedef void(*generic_func_ptr)();
        union{
            void* _untyped_content;
            generic_func_ptr _func_content;
        } _all_content;
        ssize_t _max;
    };

    template <bool is_complete, const ssize_t max,  const size_t depth>
    struct RecursiveWrapper: public WrapperBase{



      static int initialize(const char* const name,
                            const char* const module_entry_name,
                            PyObject* module, const char* const fullname, PyTypeObject& Type,
                            PyCFunction addr, PyCFunctionWithKeywords element_at){
        int ret =  WrapperBase::initialize(name, module_entry_name,
                                           module, fullname,
                                           addr,
                                           element_at,
                                           Type);
            return ret;
        }

         static int _init( RecursiveWrapper *self, PyObject *args, PyObject *kwds, PyTypeObject & Type) {
            self->_all_content._untyped_content= (void**)self->_all_content._untyped_content;
            return WrapperBase::_init(self, args, kwds, &Type, max);
        }


        void set_contents_at(  const size_t index, void**contents ){
            assert(!_all_content._untyped_content);
            Setter<void*, is_complete>::set(index, (void**)_all_content._untyped_content, contents);
        }

        template< typename T>
        T* get_CObject(){
            return (T*)_all_content._untyped_content;
        }

        template <size_t size>
        void set_contents( void* ptr[size]){
            _all_content._untyped_content = (void**)ptr;
        }

        void set_contents( void** ptr){
            _all_content._untyped_content = (void**)ptr;
        }

        template<class T, bool is_complete2, const ssize_t max2, typename  Z>
        friend struct PythonCPointerWrapper;

    };

    template <typename T, bool is_complete, const ssize_t last>
    struct PythonClassWrapper< T*, is_complete, last, PythonBase, void > :
       public RecursiveWrapper<is_complete, last, ptr_depth<T*>::value >{

       static std::string get_name(){
            return PythonClassWrapper<T>::get_name() + '*';
       }

        static PyTypeObject Type;

      static int initialize(const char* const name, const char* const module_entry_name,
                            PyObject* module, const char* const fullname = nullptr){
            static bool initialized = false;
            if (initialized) return 0;
            Type.tp_init = (initproc)_init;
            int retval =  RecursiveWrapper<is_complete, last, ptr_depth<T*>::value >::initialize(name, module_entry_name,
                                                                                                 module, fullname, Type,
                                                                                                 WrapperBase::addr<T*, is_complete>,
                                                                                                 WrapperBase::_at<T*, is_complete>);
            initialized = true;
            return retval;
        }

        static int _init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
            self->_all_content._untyped_content= (void**)self->_all_content._untyped_content;
            return RecursiveWrapper<is_complete, last, ptr_depth<T*>::value >::_init(self, args, kwds, Type);
        }

        static PyMethodDef _methods[];
    };

    template <typename T, bool is_complete, const ssize_t last>
    struct PythonClassWrapper< T* const, is_complete, last, PythonBase, void > :
       public RecursiveWrapper<is_complete, last, ptr_depth<T*>::value >{

        static std::string get_name(){
            return PythonClassWrapper<T>::get_name() + '*';
        }

        static PyTypeObject Type;

      static int initialize(const char* const name, const char* const module_entry_name,
                            PyObject* module, const char* const fullname = nullptr){
            static bool initialized = false;
            if (initialized) return 0;
            Type.tp_init = (initproc)_init;
            int retval =  RecursiveWrapper<is_complete, last, ptr_depth<T*>::value >::initialize(name, module_entry_name,
                                                                                                 module, fullname, Type,
                                                                                                 WrapperBase::addr<T* const,is_complete>,
                                                                                                 WrapperBase::_at<T* const,is_complete>);
            initialized = true;
            return retval;
        }

        static int _init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
            self->_all_content._untyped_content= (void**)self->_all_content._untyped_content;
            return RecursiveWrapper<is_complete, last, ptr_depth<T*>::value >::_init(self, args, kwds, Type);
        }
        static PyMethodDef _methods[];

   };

    template <typename T, bool is_complete, const size_t size, const ssize_t last>
    struct PythonClassWrapper< T[size], is_complete, last, PythonBase, void > :
       public RecursiveWrapper<is_complete, last, ptr_depth<T>::value+1 >{
       static std::string get_name(){
            char int_image[100];
            snprintf( int_image, 100, "[%ld]", size);
            return PythonClassWrapper<T>::get_name() + int_image;
       }

        static PyTypeObject Type;

      static int initialize(const char* const name, const char* const module_entry_name,
                            PyObject* module, const char* const fullname = nullptr){
            static bool initialized = false;
            if (initialized) return 0;
            Type.tp_init = (initproc)_init;
            int retval =  RecursiveWrapper<is_complete, last, ptr_depth<T*>::value >::initialize(name, module_entry_name,
                                                                                                 module, fullname, Type,
                                    WrapperBase::addr<T[size], is_complete>,
                                    WrapperBase::_at<T[size], is_complete>);
            initialized = true;
            return retval;
        }

        static int _init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
            self->_all_content._untyped_content= (void**)self->_all_content._untyped_content;
            return RecursiveWrapper<is_complete, last, ptr_depth<T*>::value >::_init(self, args, kwds, Type);
        }
      static PyMethodDef _methods[];

   };


    PyObject* WrapperBase::parent_module = nullptr;

    template< typename T, bool is_complete, const size_t size, const ssize_t last>
    PyMethodDef PythonClassWrapper< T[size], is_complete, last, PythonBase, void >::_methods[] = {   {address_name, nullptr, METH_KEYWORDS, nullptr},
            {"at", nullptr, METH_VARARGS, nullptr},
            {nullptr, nullptr, 0, nullptr} /*sentinel*/
    };

    template< typename T, bool is_complete, const size_t size, const ssize_t last>
    PyTypeObject PythonClassWrapper< T[size], is_complete, last, PythonBase, void >::Type= {
        PyObject_HEAD_INIT(nullptr)
        0,                         /*ob_size*/
        nullptr,             /*tp_name*/  //set on call to initialize
        sizeof(WrapperBase) + 8,             /*tp_basicsize*/
        0,                         /*tp_itemsize*/
        (destructor)WrapperBase::_dealloc, /*tp_dealloc*/
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
        nullptr,                               /* tp_traverse */
        nullptr,                               /* tp_clear */
        nullptr,                               /* tp_richcompare */
        0,                             /* tp_weaklistoffset */
        nullptr,                               /* tp_iter */
        nullptr,                               /* tp_iternext */
        _methods,             /* tp_methods */
        nullptr,             /* tp_members */
        nullptr,                         /* tp_getset */
        nullptr,                         /* tp_base */
        nullptr,                         /* tp_dict */
        nullptr,                         /* tp_descr_get */
        nullptr,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        nullptr,      /* tp_init */
        nullptr,                         /* tp_alloc */
        WrapperBase::_new,                 /* tp_new */
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

    template <typename T, bool is_complete, const ssize_t last>
    PyMethodDef PythonClassWrapper< T* const, is_complete, last, PythonBase, void >::_methods[] = {   {address_name, nullptr, METH_KEYWORDS, nullptr},
            {"at", nullptr, METH_VARARGS, nullptr},
            {nullptr, nullptr, 0, nullptr} /*sentinel*/
    };

    template <typename T, bool is_complete, const ssize_t last>
    PyTypeObject PythonClassWrapper< T* const, is_complete, last, PythonBase, void >::Type= {
        PyObject_HEAD_INIT(nullptr)
        0,                         /*ob_size*/
        nullptr,             /*tp_name*/  //set on call to initialize
        sizeof(WrapperBase) + 8,             /*tp_basicsize*/
        0,                         /*tp_itemsize*/
        (destructor)WrapperBase::_dealloc, /*tp_dealloc*/
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
        nullptr,                               /* tp_traverse */
        nullptr,                               /* tp_clear */
        nullptr,                               /* tp_richcompare */
        0,                             /* tp_weaklistoffset */
        nullptr,                               /* tp_iter */
        nullptr,                               /* tp_iternext */
        _methods,             /* tp_methods */
        nullptr,             /* tp_members */
        nullptr,                         /* tp_getset */
        nullptr,                         /* tp_base */
        nullptr,                         /* tp_dict */
        nullptr,                         /* tp_descr_get */
        nullptr,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        nullptr,      /* tp_init */
        nullptr,                         /* tp_alloc */
        WrapperBase::_new,                 /* tp_new */
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

    template <typename T, bool is_complete, const ssize_t last>
    PyMethodDef PythonClassWrapper< T*, is_complete, last, PythonBase, void >::_methods[] = {   {address_name, nullptr, METH_KEYWORDS, nullptr},
            {"at", nullptr, METH_VARARGS, nullptr},
            {nullptr, nullptr, 0, nullptr} /*sentinel*/
    };
    template <typename T, bool is_complete, const ssize_t last>
    PyTypeObject PythonClassWrapper< T*, is_complete, last, PythonBase, void >::Type= {
        PyObject_HEAD_INIT(nullptr)
        0,                         /*ob_size*/
        nullptr,             /*tp_name*/  //set on call to initialize
        sizeof(WrapperBase) + 8,             /*tp_basicsize*/
        0,                         /*tp_itemsize*/
        (destructor)WrapperBase::_dealloc, /*tp_dealloc*/
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
        nullptr,                               /* tp_traverse */
        nullptr,                               /* tp_clear */
        nullptr,                               /* tp_richcompare */
        0,                             /* tp_weaklistoffset */
        nullptr,                               /* tp_iter */
        nullptr,                               /* tp_iternext */
        _methods,             /* tp_methods */
        nullptr,             /* tp_members */
        nullptr,                         /* tp_getset */
        nullptr,                         /* tp_base */
        nullptr,                         /* tp_dict */
        nullptr,                         /* tp_descr_get */
        nullptr,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        nullptr,      /* tp_init */
        nullptr,                         /* tp_alloc */
        WrapperBase::_new,                 /* tp_new */
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

}

#endif
