#ifndef __PYLLARS__INTERNAL__POINTER_H
#define __PYLLARS__INTERNAL__POINTER_H
#include <type_traits>

#include <limits>
#include <vector>
#include <functional>

#include <Python.h>
#include <structmember.h>
#include <limits.h>
#include <sys/types.h>

#include "pyllars_defns.hpp"
#include "pyllars_classwrapper.hpp"
#include "pyllars_object_lifecycle.hpp"

// TODO (jrusnak#1#): All adding of bases, but not through template parameter....

namespace __pyllars_internal {

        template < const ssize_t max>
        struct RecursiveWrapper;

        struct PtrWrapperBase: public PtrWrapperBaseBase{

            template<typename T, typename Z = void >
            using ObjectContent = ObjectLifecycleHelpers::template ObjectContent < T,  Z >;

            template<typename Tptrtype>
            static PyObject* _at( PyObject* self_, PyObject* args, PyObject* kwargs) {
                if (!self_ || !((PtrWrapperBase*)self_)->_all_content._untyped_content){
                    PyErr_SetString(PyExc_RuntimeError, "Null pointer dereference");
                    return nullptr;
                }
                typedef typename extent_as_pointer<Tptrtype>::type Tptr;
                try{
                    PtrWrapperBase * const self = (PtrWrapperBase*) self_;
                    Py_ssize_t index = PtrWrapperBase::get_array_index(self, args, kwargs);
                    if( index <0 && self->_max >=0){
                        index = self->_max  - index - 1;
                    }
                    if( index <0 || (self->_max>= 0 &&index > self->_max)){
                        PyErr_SetString(PyExc_IndexError, "Index out of range");
                        return nullptr;
                    }
                    ssize_t array_size = UNKNOWN_SIZE;
                    if (self->_referenced_elements){
                        PtrWrapperBase* item = (PtrWrapperBase*) PyList_GetItem( self->_referenced_elements, index);
                            array_size = item?item->_arraySize:array_size;
                    }
                    CommonBaseWrapper* result = (CommonBaseWrapper*)
                       (PtrWrapperBase::ObjectContent<Tptr >::
                        getObjectAt( (Tptr*) &self->_all_content._untyped_content, index, array_size));
                    result->make_reference( (PyObject*)self );
                    return (PyObject*) result;
                } catch(const char* const msg){
                    PyErr_SetString( PyExc_RuntimeError, msg);
                    return nullptr;
                }
            }

            ssize_t getArraySize(){ return _arraySize;}
            void setArraySize( const ssize_t size){
                _arraySize = size;
                _max = size<0?-1:size-1;
            }

            template< typename T>
            static PtrWrapperBase* createPy(const ssize_t arraySize, T *cobj, const bool isAllocated,
                                            PyObject *referencing = nullptr);


        protected:

            PyObject* _referenced_elements;
            ssize_t _arraySize;
        private:
        };

        template < const ssize_t max>
        struct RecursiveWrapper: public PtrWrapperBase{


            template<typename T, typename Z = void >
            using ObjectContent = ObjectLifecycleHelpers::template ObjectContent < T,  Z >;

            static int initialize(const char* const name,
                      const char* const module_entry_name,
                      PyObject* module, const char* const fullname, PyTypeObject& Type,
                      PyCFunction addr, PyCFunctionWithKeywords element_at){
                int ret =  PtrWrapperBase::initialize(name, module_entry_name,
                                                      module, fullname,
                                                      addr,
                                                      element_at,
                                                      Type);
                return ret;
            }

            static int _init( RecursiveWrapper *self, PyObject *args, PyObject *kwds, PyTypeObject & Type) {
                self->_arraySize = -1;
                self->_referenced_elements = nullptr;
                self->_referenced = nullptr;
                self->_all_content._untyped_content= (void*)self->_all_content._untyped_content;
                return PtrWrapperBase::_init(self, args, kwds, &Type, max);
            }


            void set_contents_at(  const size_t index, void**contents ){
              assert(_all_content._untyped_content);
              ObjectContent<void*>::set(index, (void**)&_all_content._untyped_content, contents);
            }


            void set_contents( void** ptr, bool allocated){
                _allocated = allocated;
                _all_content._untyped_content = *ptr;
            }

            template<class T, const ssize_t max2, typename  Z>
            friend struct PythonCPointerWrapper;

        protected:

        private:

        };

        /**
         * specialize for regular pointer type
         **/
        template <typename T, const ssize_t last>
        struct PythonClassWrapper< T*, last, void > : public RecursiveWrapper< last>{

            typedef PythonClassWrapper<T> DereferencedWrapper;
            typedef PythonClassWrapper<T* const> ConstWrapper;
            typedef PythonClassWrapper<T*> NonConstWrapper;
            typedef PythonClassWrapper<T*> NoRefWrapper;
            typedef PythonClassWrapper<T*> AsPtrWrapper;

            static std::string get_name(){
                return PythonClassWrapper<T>::get_name() + '*';
            }

            static std::string get_module_entry_name(){
                return PythonClassWrapper<T>::get_module_entry_name() + '*';
            }

            static PyTypeObject Type;

            static int initialize(){
                int status  = DereferencedWrapper::initialize();

                return status!=0?status:initialize( (DereferencedWrapper::get_name() +"_array").c_str(),
                            (DereferencedWrapper::get_module_entry_name() +"[]").c_str(),
                            DereferencedWrapper::parent_module,
                            (std::string(DereferencedWrapper::Type.tp_name)+"[]").c_str()
                            );
            }

            static int initialize(const char* const name, const char* const module_entry_name,
                                  PyObject* module, const char* const fullname = nullptr){
              static bool initialized = false;
              if (initialized) return 0;
              Type.tp_init = (initproc)_init;
              int retval =  RecursiveWrapper< last >::initialize(name, module_entry_name,
									     module, fullname, Type,
									     PtrWrapperBase::addr<T*>,
									     PtrWrapperBase::_at<T*>);
              initialized = true;
              return retval;
            }

            static int _init( PythonClassWrapper *self, PyObject *args, PyObject *kwds);

            static PyMethodDef _methods[];
        };

        /**
         * Specialize for const-pointer types
         **/
        template <typename T, const ssize_t last>
        struct PythonClassWrapper< T* const, last, void > :
           public RecursiveWrapper< last >{

            typedef PythonClassWrapper<const T> DereferencedWrapper;
            typedef PythonClassWrapper<T* const> ConstWrapper;
            typedef PythonClassWrapper<T*> NonConstWrapper;
            typedef PythonClassWrapper<T* const> NoRefWrapper;
            typedef PythonClassWrapper<T* const> AsPtrWrapper;

            static std::string get_name(){
                return PythonClassWrapper<T>::get_name() + '*';
            }

           static std::string get_module_entry_name(){
                return PythonClassWrapper<T>::get_module_entry_name() + '*';
            }

            static PyTypeObject Type;

            static int initialize(){
                int status  = DereferencedWrapper::initialize();

                return status!=0?status:initialize( (DereferencedWrapper::get_name() +"_array").c_str(),
                            (DereferencedWrapper::get_module_entry_name() +"[]").c_str(),
                            DereferencedWrapper::parent_module,
                            (std::string(DereferencedWrapper::Type.tp_name)+"[]").c_str()
                            );
            }
            static int initialize(const char* const name, const char* const module_entry_name,
                                  PyObject* module, const char* const fullname = nullptr);

            static int _init( PythonClassWrapper *self, PyObject *args, PyObject *kwds);

            static PyMethodDef _methods[];

        };

        /**
         * specialize for fixed-size array types
         **/
        template <typename T, const size_t size, const ssize_t last>
        struct PythonClassWrapper< T[size], last, void > :
            public RecursiveWrapper< last >{

            typedef PythonClassWrapper<T> DereferencedWrapper;
            typedef PythonClassWrapper<T* const> ConstWrapper;
            typedef PythonClassWrapper<T*>  NonConstWrapper;
            typedef PythonClassWrapper<T[size]> NoRefWrapper;
            typedef PythonClassWrapper<T*> AsPtrWrapper;

            static PyTypeObject Type;

            static std::string get_name(){
              char int_image[100];
              snprintf( int_image, 100, "[%ld]", size);
              return PythonClassWrapper<T>::get_name() + int_image;
            }

           static std::string get_module_entry_name(){
                char int_image[100];
                snprintf( int_image, 100, "[%ld]", size);
                return PythonClassWrapper<T>::get_module_entry_name() + "_array_" + int_image;
            }

            static int initialize(){
                int status  = DereferencedWrapper::initialize();

                return status!=0?status:initialize( (DereferencedWrapper::get_name() +"_array").c_str(),
                            (DereferencedWrapper::get_module_entry_name() +"[]").c_str(),
                            DereferencedWrapper::parent_module,
                            (std::string(DereferencedWrapper::Type.tp_name)+"[]").c_str()
                            );
            }
           static int initialize(const char* const name, const char* const module_entry_name,
                                  PyObject* module, const char* const fullname = nullptr){
                static bool initialized = false;
                if (initialized) return 0;
                Type.tp_init = (initproc)_init;
                int retval =  RecursiveWrapper< last >::initialize(name, module_entry_name,
									       module, fullname, Type,
									       PtrWrapperBase::addr<T[size]>,
									       PtrWrapperBase::_at<T[size]>);
                initialized = true;
                return retval;
            }

            static int _init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
                self->_arraySize = UNKNOWN_SIZE;
                self->_referenced_elements = nullptr;
                self->_referenced = nullptr;
                int retval = RecursiveWrapper< last >::_init(self, args, kwds, Type);
                return retval;
            }

            static PyMethodDef _methods[];

        };

        /**
         * specialize for fixed-size array types of const element
         **/
        template <typename T, const size_t size, const ssize_t last>
        struct PythonClassWrapper< const T[size], last, void > :
            public RecursiveWrapper< last>{

            typedef PythonClassWrapper<const T> DereferencedWrapper;
            typedef PythonClassWrapper<const T* const> ConstWrapper;
            typedef PythonClassWrapper<const T*>  NonConstWrapper;
            typedef PythonClassWrapper<const T[size]> NoRefWrapper;
            typedef PythonClassWrapper<const T*> AsPtrWrapper;

            static PyTypeObject Type;

            static std::string get_name(){
              char int_image[100];
              snprintf( int_image, 100, "[%ld]", size);
              return PythonClassWrapper<T>::get_name() + int_image;
            }

           static std::string get_module_entry_name(){
                char int_image[100];
                snprintf( int_image, 100, "[%ld]", size);
                return PythonClassWrapper<T>::get_module_entry_name() + "_array_" + int_image;
            }

            static int initialize(){
                int status  = DereferencedWrapper::initialize();

                return status!=0?status:initialize( (DereferencedWrapper::get_name() +"_array").c_str(),
                            (DereferencedWrapper::get_module_entry_name() +"[]").c_str(),
                            DereferencedWrapper::parent_module,
                            (std::string(DereferencedWrapper::Type.tp_name)+"[]").c_str()
                            );
            }
           static int initialize(const char* const name, const char* const module_entry_name,
                                  PyObject* module, const char* const fullname = nullptr){
                static bool initialized = false;
                if (initialized) return 0;
                Type.tp_init = (initproc)_init;
                int retval =  RecursiveWrapper< last >::initialize(name, module_entry_name,
									       module, fullname, Type,
									       PtrWrapperBase::addr<const T[size]>,
									       PtrWrapperBase::_at<const T[size]>);
                initialized = true;
                return retval;
            }

            static int _init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
                //self->_all_content._untyped_content= (void**)self->_all_content._untyped_content;
                int retval = RecursiveWrapper<last >::_init(self, args, kwds, Type);
                return retval;
            }

            static PyMethodDef _methods[];

        };

        /**
         * Specialize for array types of unknown size
         **/
        template <typename T, const ssize_t last>
        struct PythonClassWrapper< T[], last, void > :
            public RecursiveWrapper< last >{

            typedef PythonClassWrapper<T> DereferencedWrapper;
            typedef PythonClassWrapper<T* const> ConstWrapper;
            typedef PythonClassWrapper<T*> NonConstWrapper;
            typedef PythonClassWrapper<T[]> NoRefWrapper;
            typedef PythonClassWrapper<T*> AsPtrWrapper;

            static std::string get_name(){
              return PythonClassWrapper<T>::get_name() + "[]";
            }

            static std::string get_module_entry_name(){
                return PythonClassWrapper<T>::get_module_entry_name() + "_array";
            }

            static PyTypeObject Type;

            static int initialize(){
                int status  = DereferencedWrapper::initialize();

                return status!=0?status:initialize( (DereferencedWrapper::get_name() +"_array").c_str(),
                            (DereferencedWrapper::get_module_entry_name() +"[]").c_str(),
                            DereferencedWrapper::parent_module,
                            (std::string(DereferencedWrapper::Type.tp_name)+"[]").c_str()
                            );
            }

           static int initialize(const char* const name, const char* const module_entry_name,
                                  PyObject* module, const char* const fullname = nullptr){
                static bool initialized = false;
                if (initialized) return 0;
                Type.tp_init = (initproc)_init;
                int retval =  RecursiveWrapper< last >::initialize(name, module_entry_name,
									       module, fullname, Type,
									       PtrWrapperBase::addr<T[]>,
									       PtrWrapperBase::_at<T[]>);
                initialized = true;
                return retval;
            }

            static int _init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
                //self->_all_content._untyped_content= (void**)self->_all_content._untyped_content;
                int retval = RecursiveWrapper< last >::_init(self, args, kwds, Type);
                return retval;
            }

            static PyMethodDef _methods[];

        };

        /**
         * Specialize for array types of const elements of unknown size
         **/
        template <typename T, const ssize_t last>
        struct PythonClassWrapper< T const [], last, void > :
            public RecursiveWrapper< last >{

            typedef PythonClassWrapper<T const> DereferencedWrapper;
            typedef PythonClassWrapper<T const * const> ConstWrapper;
            typedef PythonClassWrapper<T const *> NontConstWrapper;
            typedef PythonClassWrapper<T const []> NoRefWrapper;
            typedef PythonClassWrapper<T const *> AsPtrWrapper;

            static std::string get_name(){
                return PythonClassWrapper<T const>::get_name() + "[]";
            }

           static std::string get_module_entry_name(){
                return PythonClassWrapper<T const>::get_module_entry_name() + "_array";
            }

            static PyTypeObject Type;

            static int initialize(){
                int status  = DereferencedWrapper::initialize();

                return status!=0?status:initialize( (DereferencedWrapper::get_name() +"_array").c_str(),
                            (DereferencedWrapper::get_module_entry_name() +"[]").c_str(),
                            DereferencedWrapper::parent_module,
                            (std::string(DereferencedWrapper::Type.tp_name)+"[]").c_str()
                            );
            }

           static int initialize(const char* const name, const char* const module_entry_name,
                                  PyObject* module, const char* const fullname = nullptr){
                static bool initialized = false;
                if (initialized) return 0;
                Type.tp_init = (initproc)_init;
                int retval =  RecursiveWrapper<last >::initialize(name, module_entry_name,
									       module, fullname, Type,
									       PtrWrapperBase::addr<T const []>,
									       PtrWrapperBase::_at<T const[]>);
                initialized = true;
                return retval;
            }

            static int _init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
                //      self->_all_content._untyped_content= (void**)self->_all_content._untyped_content;
                int retval = RecursiveWrapper<last >::_init(self, args, kwds, Type);
                return retval;
            }

            static PyMethodDef _methods[];

        };


        ///////////////////////////////
        // Template definitions of static elements of template specializations
        /////////////////////////////////

        template <typename T, const ssize_t last>
        int PythonClassWrapper< T*, last, void >::_init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
            self->_arraySize = UNKNOWN_SIZE;
            self->_referenced_elements = nullptr;
            self->_referenced = nullptr;
            int retval = RecursiveWrapper<last >::_init(self, args, kwds, Type);
            if (retval == ERROR_TYPE_MISMATCH){
              try{
                PyObject* arg = PyTuple_GetItem( args, 0);
                if( PyObject_TypeCheck((PyObject*)self, &PythonClassWrapper<const char*>::Type) ||
                    PyObject_TypeCheck((PyObject*)self, &PythonClassWrapper<const char* const>::Type) ||
                    PyObject_TypeCheck((PyObject*)self, &PythonClassWrapper<char* const>::Type) ||
                    PyObject_TypeCheck((PyObject*)self, &PythonClassWrapper<char* >::Type)
                    ){
                  const char* const s = PyString_AsString( arg );
                  if(s){
                    char* new_s = new char[strlen(s)+1];
                    strcpy(new_s, s);
                    self->set_contents( (void**)&new_s, true);

                    PyErr_Clear();
                    retval = 0;
                  }
                }
              } catch(const char* const msg){
                PyErr_SetString(PyExc_RuntimeError, msg);
              }
            }
            return retval;
        }

        template <typename T, const ssize_t last>
        int PythonClassWrapper< T* const, last, void >::_init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
            //self->_all_content._untyped_content= (void**)self->_all_content._untyped_content;
            int retval = RecursiveWrapper< last >::_init(self, args, kwds, Type);
            if (retval == ERROR_TYPE_MISMATCH){
              try{
                PyObject* arg = PyTuple_GetItem( args, 0);
                if( PyObject_TypeCheck((PyObject*)self, &PythonClassWrapper<const char*>::Type) ||
                    PyObject_TypeCheck((PyObject*)self, &PythonClassWrapper<const char* const>::Type) ||
                    PyObject_TypeCheck((PyObject*)self, &PythonClassWrapper<char* const>::Type) ||
                    PyObject_TypeCheck((PyObject*)self, &PythonClassWrapper<char* >::Type)
                    ){
                  const char* const s = PyString_AsString( arg );
                  if(s){
                    char* new_s = new char[strlen(s)+1];
                    strcpy(new_s, s);
                    self->set_contents( (void**)&new_s, true);
                    PyErr_Clear();
                    retval = 0;
                  }
                }
              } catch(const char* const msg){
                PyErr_SetString(PyExc_RuntimeError, msg);
              }
            }
            return retval;
        }

        template <typename T, const ssize_t last>
        int PythonClassWrapper< T* const, last, void >::initialize(const char* const name, const char* const module_entry_name,
                                                                                          PyObject* module, const char* const fullname ){
            static bool initialized = false;
            if (initialized) return 0;
            Type.tp_init = (initproc)_init;
            int retval =  RecursiveWrapper<last >::initialize(name, module_entry_name,
									   module, fullname, Type,
									   PtrWrapperBase::addr<T* const>,
									   PtrWrapperBase::_at<T* const>);
            initialized = true;
            return retval;
        }

        template< typename T, const size_t size, const ssize_t last>
        PyMethodDef PythonClassWrapper< T[size], last >::_methods[] = {   {address_name, nullptr, METH_KEYWORDS, nullptr},
                                                                                                       {"at", nullptr, METH_VARARGS, nullptr},
                                                                                                       {nullptr, nullptr, 0, nullptr} /*sentinel*/
        };

        template< typename T, const size_t size, const ssize_t last>
        PyTypeObject PythonClassWrapper< T[size], last, void >::Type= {
            PyObject_HEAD_INIT(nullptr)
            0,                         /*ob_size*/
            nullptr,             /*tp_name*/  //set on call to initialize
            sizeof(PtrWrapperBase) + 8,             /*tp_basicsize*/
            0,                         /*tp_itemsize*/
            (destructor)PtrWrapperBase::_dealloc<T[size], ObjectLifecycleHelpers::BasicDeallocation >, /*tp_dealloc*/
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
            PtrWrapperBase::_new,                 /* tp_new */
            nullptr,                         /*tp_free*/ // no freeing of fixed-array
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bass*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
        };

        template< typename T, const size_t size, const ssize_t last>
        PyMethodDef PythonClassWrapper< const T[size], last, void >::_methods[] = {   {address_name, nullptr, METH_KEYWORDS, nullptr},
                                                                                                       {"at", nullptr, METH_VARARGS, nullptr},
                                                                                                       {nullptr, nullptr, 0, nullptr} /*sentinel*/
        };

        template< typename T, const size_t size, const ssize_t last>
        PyTypeObject PythonClassWrapper< const T[size], last, void >::Type= {
            PyObject_HEAD_INIT(nullptr)
            0,                         /*ob_size*/
            nullptr,             /*tp_name*/  //set on call to initialize
            sizeof(PtrWrapperBase) + 8,             /*tp_basicsize*/
            0,                         /*tp_itemsize*/
            (destructor)PtrWrapperBase::_dealloc<const T[size], ObjectLifecycleHelpers::BasicDeallocation >, /*tp_dealloc*/
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
            PtrWrapperBase::_new,                 /* tp_new */
            nullptr,                         /*tp_free*/ // no freeing of fixed-array
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bass*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
        };

        template <typename T, const ssize_t last>
        PyMethodDef PythonClassWrapper< T* const, last, void >::_methods[] = {   {address_name, nullptr, METH_KEYWORDS, nullptr},
                                                                                                        {"at", nullptr, METH_VARARGS, nullptr},
                                                                                                        {nullptr, nullptr, 0, nullptr} /*sentinel*/
        };

        template <typename T, const ssize_t last>
        PyTypeObject PythonClassWrapper< T* const, last, void >::Type= {
            PyObject_HEAD_INIT(nullptr)
            0,                         /*ob_size*/
            nullptr,             /*tp_name*/  //set on call to initialize
            sizeof(PtrWrapperBase) + 8,             /*tp_basicsize*/
            0,                         /*tp_itemsize*/
            (destructor)PtrWrapperBase::_dealloc<T* const, ObjectLifecycleHelpers::Deallocation<T*> >, /*tp_dealloc*/
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
            0,                               /* tp_dictoffset */
            nullptr,                         /* tp_init */
            nullptr,                         /* tp_alloc */
            PtrWrapperBase::_new,               /* tp_new */
            (freefunc)ObjectLifecycleHelpers::Deallocation<T* const>::_free,          /*tp_free*/
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bass*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
        };

        template <typename T, const ssize_t last>
        PyMethodDef PythonClassWrapper< T*, last, void >::_methods[] = {   {address_name, nullptr, METH_KEYWORDS, nullptr},
                                                                                                  {"at", nullptr, METH_VARARGS, nullptr},
                                                                                                  {nullptr, nullptr, 0, nullptr} /*sentinel*/
        };

        template <typename T, const ssize_t last>
        PyTypeObject PythonClassWrapper< T*, last, void >::Type= {
            PyObject_HEAD_INIT(nullptr)
            0,                         /*ob_size*/
            nullptr,             /*tp_name*/  //set on call to initialize
            sizeof(PtrWrapperBase) + 8,             /*tp_basicsize*/
            0,                         /*tp_itemsize*/
            (destructor)PtrWrapperBaseBase::_dealloc<T*, ObjectLifecycleHelpers::Deallocation<T*> >, /*tp_dealloc*/
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
            PtrWrapperBase::_new,                 /* tp_new */
            (freefunc)ObjectLifecycleHelpers::Deallocation<T*>::_free,                         /*tp_free*/ //TODO: Implement a free??
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bass*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
        };

        template <typename T, const ssize_t last>
        PyTypeObject PythonClassWrapper<T[], last, void>::Type = {

            PyObject_HEAD_INIT(nullptr)
            0,                         /*ob_size*/
            nullptr,             /*tp_name*/ /*filled on init*/
            sizeof(PythonClassWrapper),             /*tp_basicsize*/
            0,                         /*tp_itemsize*/
            (destructor)PtrWrapperBaseBase::_dealloc<T*, ObjectLifecycleHelpers::ArrayDeallocation<T*> >, /*tp_dealloc*/
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
            "PythonClassWrapper object",           /* tp_doc */
            nullptr,		               /* tp_traverse */
            nullptr,		               /* tp_clear */
            nullptr,		               /* tp_richcompare */
            0,		                       /* tp_weaklistoffset */
            nullptr,		               /* tp_iter */
            nullptr,		               /* tp_iternext */
            _methods,             /* tp_methods */
            nullptr,             /* tp_members */
            nullptr,                         /* tp_getset */
            CommonBaseWrapper::Base::TypePtr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            (initproc)PythonClassWrapper::_init,  /* tp_init */
            nullptr,                         /* tp_alloc */
            PythonClassWrapper::_new,             /* tp_new */
            (freefunc)ObjectLifecycleHelpers::ArrayDeallocation<T*>::_free,                         /*tp_free*/
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bases*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
        };

        template< typename T, const ssize_t last>
        PyMethodDef PythonClassWrapper< T[], last, void >::_methods[] = {   {address_name, nullptr, METH_KEYWORDS, nullptr},
                                                                                                       {"at", nullptr, METH_VARARGS, nullptr},
                                                                                                       {nullptr, nullptr, 0, nullptr} /*sentinel*/
        };

        template <typename T, const ssize_t last>
        PyTypeObject PythonClassWrapper<T const[], last, void>::Type = {

            PyObject_HEAD_INIT(nullptr)
            0,                         /*ob_size*/
            nullptr,             /*tp_name*/ /*filled on init*/
            sizeof(PythonClassWrapper),             /*tp_basicsize*/
            0,                         /*tp_itemsize*/
            (destructor)PtrWrapperBase::_dealloc<const T *, ObjectLifecycleHelpers::ArrayDeallocation< const T  *>  >, /*tp_dealloc*/
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
            "PythonClassWrapper object",           /* tp_doc */
            nullptr,		               /* tp_traverse */
            nullptr,		               /* tp_clear */
            nullptr,		               /* tp_richcompare */
            0,		                       /* tp_weaklistoffset */
            nullptr,		               /* tp_iter */
            nullptr,		               /* tp_iternext */
            _methods,             /* tp_methods */
            nullptr,             /* tp_members */
            nullptr,                         /* tp_getset */
            CommonBaseWrapper::Base::TypePtr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            (initproc)PythonClassWrapper::_init,  /* tp_init */
            nullptr,                         /* tp_alloc */
            PythonClassWrapper::_new,        /* tp_new */
            (freefunc)ObjectLifecycleHelpers::ArrayDeallocation<T const *>::_free,              /*tp_free*/ //TODO: Implement a free??
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bases*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
        };

        template< typename T, const ssize_t last>
        PyMethodDef PythonClassWrapper< T const[], last, void >::_methods[] = {   {address_name, nullptr, METH_KEYWORDS, nullptr},
                                                                                                       {"at", nullptr, METH_VARARGS, nullptr},
                                                                                                       {nullptr, nullptr, 0, nullptr} /*sentinel*/
        };

	  template< typename T>
	   PtrWrapperBase* PtrWrapperBase::createPy(const ssize_t arraySize, T *const cobj, const bool isAllocated,
                                                PyObject *referencing){
	    assert( std::is_pointer<T>::value || std::is_array<T>::value );
        static PyObject* kwds = PyDict_New();
	    static PyObject* emptyargs = PyTuple_New(0);
	    PyDict_SetItemString( kwds, "__internal__null_allowed", Py_True);
	    typedef PythonClassWrapper< T*> PyPtrWrapper;
	    PyPtrWrapper* pyobj = (PyPtrWrapper*)PyObject_Call((PyObject*)&PyPtrWrapper::Type, emptyargs, kwds);
	    pyobj->set_contents( (void**)cobj, isAllocated);
	    pyobj->setArraySize( arraySize);
	    if(referencing) pyobj->make_reference( referencing );
	    return (PtrWrapperBase*) pyobj;
	  }

}
#endif
