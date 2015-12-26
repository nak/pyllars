#ifndef PYLLARS_INTERNAL__OBJECTLIFECYCLEHELPERS_H
#define PYLLARS_INTERNAL__OBJECTLIFECYCLEHELPERS_H

#include <type_traits>
#include <vector>

#include <sys/types.h>
#include <Python.h>

#include "pyllars_defns.hpp"

/***********************************
* One's head hurts thinking of all the different types of types in C++:
* pointers to a type, const types, const pointers to a type, const pointers to a const type,
* fixed array types, arrays of unknown length, function pointers, .....
*
* To minimize the combinatorics in having gobs of wrapper classes with duplication of code,
* the strategy here is to limit the specializations to specific needs through a container
* of helpers for object lifeccyle management:  from allocation, deallocatin and object
* mainpulation and conversion
************************************/
namespace __pyllars_internal{

    class PtrWrapperBaseBase;

    template <bool is_complete, const ssize_t max>
    struct RecursiveWrapper;

    template<typename CT, bool is_complete, const ssize_t last, typename Base, typename Z>
    struct PythonTWrapper;

    //template <bool is_complete, const ssize_t max,  const size_t depth>
    //class RecusriveWrapper;

    class ObjectLifecycleHelpers
    {
    public:

        friend class PtrWrapperBaseBase;

        friend class PtrWrapperBase;

        template <bool is_complete, const ssize_t max>
        friend struct RecursiveWrapper;

        template<typename CClass, bool is_complete, const ssize_t last , typename Base, typename Z >
        friend struct PythonClassWrapper;

    private:

        ////////////////////////////////
        // POINTER DERFERENCING AND ARRAY INDEXING SET/GET HELPERS
        //
        // NOTE: SOME CLASSES/TYPES ARE NON-COPIABLE, INCOMPLETE, ETC.
        // IN THESE CASES, IT IS POSSIBLE TO SPECIALIZE TO NOT EVEN
        // PROVIDE RESTRICTED METHODS IN PYTHON THAT WOULD NORMALLY LEADE
        // TO AN ILLEGAL OPERATION IN C++.  HOWEVER, THIS WOULD MOST LIKELY
        // CONFUSE THE PYTHON DEVELOPER EXPECTING TO SEE AN INTERFACE THAT
        // ISN'T THERE.  INSTEAD, THE OPTIONS WILL THROUGH AN EXCEPTION THAT
        // WILL TRANSLATE INTO A PYTHON EXCEPTION TO THE USER AT RUN-TIME
        // TO BETTER CLUE IN THE DEVELOPER.
        ////////////////////////////////

        /**
         * Helper for Setting and Getting T values in dereferencing
         * pointer-to-class objects
         **/
        template <typename T, const bool is_complete, typename Z = void >
        class ObjectContent;

        /**
         * Specialization for non-pointer copiable and assignable class (instances)
         **/
        template< typename T, const bool is_complete>
        class ObjectContent<T, is_complete,
                    typename std::enable_if< !std::is_pointer<T>::value &&
                                             std::is_copy_constructible<T>::value &&
                                             std::is_assignable<T, T>::value>::type >{
        public:

            static void set( const size_t index, T* const to, const T* const from){
                to[index] = *from;
            }

            static T* getObjectPtr(PythonTWrapper<T, is_complete, -1, PythonBase, void> * const self){
                return (T*)self->_CObject;
            }

        };

        /**
         * Specialization for non-pointer non-array copiable but not assignable
         * class (instances)
         **/
        template< typename T, const bool is_complete>
        class ObjectContent<T, is_complete,
                     typename std::enable_if< !std::is_pointer<T>::value &&
                                        !std::is_array<T>::value &&
                                        std::is_copy_constructible<T>::value &&
                                        !std::is_assignable<T, T>::value >::type >{
        public:

            static void set( const size_t index, T* const to, const T* const from){
                throw "Object is not assignable";
            }

            static T* getObjectPtr(PythonTWrapper<T, is_complete, -1, PythonBase, void> * const self){
                return (T*)&self->_CObject;
            }

        };

        /**
         * Specialization for non-pointer non-array non-vois non-copiable
         * class (instances)
         **/
        template< typename T, const bool is_complete>
        class ObjectContent<T, is_complete,
                            typename std::enable_if< !std::is_pointer<T>::value &&
                                                !std::is_array<T>::value &&
                                                !std::is_void<T>::value &&
                                                !std::is_copy_constructible<T>::value>::type >{
        public:

            static void set( const size_t index, T* const to, const T* const from){
                throw "setting void type";
            }

            static T* getObjectPtr(PythonTWrapper<T, is_complete, -1, PythonBase, void> * const self){
                return (T*)&self->_CObject;
            }
        };

        /**
         * Specialization for non-array non-void non-copieable
         * class (instances)
         **/
        template< typename T, const bool is_complete>
        class ObjectContent<T, is_complete,
                           typename std::enable_if< !std::is_array<T>::value &&
                                                !std::is_void<T>::value &&
                                                !std::is_copy_constructible<T>::value>::type >{
        public:

            static PyObject* getObjectAt( T** const from, const size_t index){
                throw "Getting element from incomplete type";
            }

            static void set( const size_t index, T* const to, const T* const from){
                throw "setting void type";
            }

            static T* getObjectPtr(PythonTWrapper<T, is_complete, -1, PythonBase, void> * const self){
                return (T*)&self->_CObject;
            }

        };

        /**
         * Specialization for void type
         **/
        template< typename T, bool is_complete>
        class ObjectContent<T, is_complete, typename std::enable_if< std::is_void<T>::value >::type >{
        public:

            static PyObject* getObjectAt( T** const from, const size_t index){
                throw "Getting element from incomplete type";
            }

            static void set( const size_t index, T* const to, const T* const from){
                throw "setting void type";
            }

            static T* getObjectPtr(PythonTWrapper<T, is_complete, -1, PythonBase, void> * const self){
                return (T*)&self->_CObject;
            }

        };

        /**
         * Specialization for non-const copy-constructible non-ptr-to-void non-const pointer types
         **/
        template< typename T, const bool is_complete>
        class ObjectContent<T*, is_complete,
                           typename std::enable_if< !std::is_const<T>::value &&
                                                std::is_copy_constructible<T>::value &&
                                                !std::is_void< T >::value>::type >{
        public:

            static PyObject* getObjectAt( T** const from, const size_t index, const ssize_t elements_array_size){
                //TODO add reference to owning element to this object to not have go out of scope
                //until parent does!!!!
                return toPyObject<T, is_complete>((*from)[index], true, elements_array_size);
            }

            static void set( const size_t index, T** const to, T** const from){
                to[index] = *from;
            }

            static T** getObjectPtr(PtrWrapperBaseBase* const self){
                return (T**)&self->_all_content._untyped_content;
            }

        };

        /**
         * Specialization for non-const copy-constrible non-ptr-to-voie const pointer types
         **/
        template< typename T, const bool is_complete>
        class ObjectContent<T* const, is_complete,
                           typename std::enable_if< !std::is_const<T>::value &&
                                                std::is_copy_constructible<T>::value &&
                                                !std::is_void< T >::value>::type >{
        public:

            static PyObject* getObjectAt( T* const * const from, const size_t index, const ssize_t elements_array_size){
                //TODO add reference to owning element to this object to not have go out of scope
                //until parent does!!!!
                return toPyObject<T, is_complete>((*from)[index], false, elements_array_size);
            }

            static void set( const size_t index, T* const * const to, T* const * const from){
                to[index] = *from;
            }

             static T** getObjectPtr(PtrWrapperBaseBase* const self){
                return (T**)&self->_all_content._untyped_content;
            }

        };

        /**
         * Speicalization for non-const-pointer-to-const type that is copyh cosntructibe and non-void
         **/
        template< typename T, const bool is_complete>
        class ObjectContent<const T*, is_complete,
                           typename std::enable_if< std::is_copy_constructible<T>::value &&
                                                    !std::is_void< T >::value>::type >{
        public:

            static PyObject* getObjectAt( const T** const from, const size_t index, const ssize_t elements_array_size){
                //TODO add reference to owning element to this object to not have go out of scope
                //until parent does!!!!
                return toPyObject<T, is_complete>((*from)[index], false, elements_array_size);
            }

            static void set( const size_t index, const T** const to, const T** const from){
                throw "Attempt to set const element in array";
            }

            static const T** getObjectPtr(PtrWrapperBaseBase* const self){
                return (const T**)&self->_all_content._untyped_content;
            }

        };

        /**
         * Speicalization for const-pointer-to-const type that is copyh cosntructibe and non-void
         **/
        template< typename T, const bool is_complete>
        class ObjectContent<const T* const, is_complete,
                           typename std::enable_if< std::is_copy_constructible<T>::value &&
                                                    !std::is_void< T >::value>::type >{
        public:

            static PyObject* getObjectAt( const T* const* const from, const size_t index, const ssize_t elements_array_size){
                //TODO add reference to owning element to this object to not have go out of scope
                //until parent does!!!!
                return toPyObject<T, is_complete>((*from)[index], false, elements_array_size);
            }

            static void set( const size_t index, const T* const* const to,  const T* const* const from){
                throw "Attempt to set const element in array";
            }

            static const T** getObjectPtr(PtrWrapperBaseBase* const self){
                return (const T**)&self->_all_content._untyped_content;
            }

        };

        /**
         * Specialization for non-const-pointer to types that are not constructible
         **/
        template< typename T, const bool is_complete>
        class ObjectContent<T*, is_complete, typename std::enable_if< !std::is_copy_constructible<T>::value >::type >{
        public:

            static PyObject* getObjectAt( T** const from, const size_t index, const ssize_t elements_array_size){
                (void)from; (void)index; (void) elements_array_size;
                throw "Getting element of incomplete or non-copiable element type";
            }

            static void set( const size_t index, T** const to, const T** const from){
                to[index] = (T*)*from;
            }

            static T** getObjectPtr(PtrWrapperBaseBase* const self){
                return (T**)&self->_all_content._untyped_content;
            }

        };

        /**
         * Specialization for fixed-size arrays of copy-construtible types
         **/
        template< typename T, const size_t size, const bool is_complete>
        class ObjectContent<T[size], is_complete, typename std::enable_if< std::is_copy_constructible<T>::value >::type >{
        public:
            typedef T T_array[size];
            static PyObject* getObjectAt( T_array* const from, const size_t index, const ssize_t elements_array_size){
                //TODO add reference to owning element to this object to not have go out of scope
                //until parent does!!!!
                return toPyObject<T, is_complete>((*from)[index], true, elements_array_size);
            }

            static void set( const size_t index, T_array* const to, const T_array* const from){
                for (size_t i = 0; i < size; ++i)
                    to[index][i] = (*from)[i];
            }

           static T** getObjectPtr(PtrWrapperBaseBase* const self){
                return (T**)&self->_all_content._untyped_content;
            }

        };

        /**
         * Specialization for fixed-size arrays of NON-copy-construtible types
         **/
        template< typename T, const size_t size, const bool is_complete>
        class ObjectContent<T[size], is_complete, typename std::enable_if< !std::is_copy_constructible<T>::value >::type >{
        public:
            typedef T T_array[size];
            static PyObject* getObjectAt( T_array* const from, const size_t index, const ssize_t elements_array_size){
                (void)from;(void)index;(void)elements_array_size;
                throw "Getting element of incomplete or non-copiable element type";
            }

            static void set( const size_t index, T_array* const to, const T_array* const from){
                for (size_t i = 0; i < size; ++i)
                    to[index][i] = (*from)[i];
            }

            static T** getObjectPtr(PtrWrapperBaseBase* const self){
                return (T**)&self->_all_content._untyped_content;
            }

        };


        /**
         * Specialization for variable-sized array-of-const  of NON-copy-construtible types
         **/
        template< typename T, const bool is_complete>
        class ObjectContent<T const[], is_complete, typename std::enable_if< !std::is_copy_constructible<T>::value >::type >{
        public:
            typedef const T T_array[];
            static PyObject* getObjectAt( T const ** const from, const size_t index, const ssize_t elements_array_size){
                //TODO add reference to owning element to this object to not have go out of scope
                //until parent does!!!!
                return toPyObject<T, is_complete>((*from)[index], true, elements_array_size);
            }

            static void set( const size_t index, T const ** const to, const T* const * const from){
                throw "Attemp to set value of array of unknown extent";
            }

           static const T** getObjectPtr(PtrWrapperBaseBase* const self){
                return (const T**)&self->_all_content._untyped_content;
            }

        };

        /**
         * Specialization for variable-sized array-of-const  of copy-construtible types
         **/
        template< typename T, const bool is_complete>
        class ObjectContent<T const[], is_complete, typename std::enable_if< std::is_copy_constructible<T>::value >::type >{
        public:
            typedef const T T_array[];
            static PyObject* getObjectAt( T const ** from, const size_t index, const ssize_t elements_array_size){
                (void)from; (void)index; (void) elements_array_size;
                throw "Getting element of incomplete or non-copiable element type";
            }

            static void set( const size_t index, T const ** const to, const T** const from){
                (void) index; (void)to; (void)from;
                throw "Attemp to set value of array of unknown extent";
            }

            static const T** getObjectPtr(PtrWrapperBaseBase* const self){
                return (const T**)&self->_all_content._untyped_content;
            }


        };

        /**
         * Specialization for void-pointer type
         **/
        template<  const bool is_complete>
        class ObjectContent<void*, is_complete, void >{
        public:

            static void set( const size_t index, void** const to,  void** const from){
                to[index] = *from;
            }

            static PyObject* getObjectAt( void** const from, const size_t index, const ssize_t elements_array_size){
                (void)from;(void)index;(void)elements_array_size;
                throw "Attempt to get void value";
            }

            static void** getObjectPtr(PtrWrapperBaseBase* const self){
                return (void**)&self->_all_content._untyped_content;
            }

        };


        ///////////////////////////////
        // DEALLOCATION LOGIC FOR VARIOUS TYPES
        ////////////////////////////////

        /**
         * Basic deallocation is to assume no special deconstruction is needed, just
         * basic cleanup
         **/
        struct BasicDeallocation{
            static void _free( PtrWrapperBaseBase* self){
                if( self->_raw_storage){
                    delete[] self->_raw_storage;
                }
                self ->_raw_storage = nullptr;
                self->_all_content._untyped_content = nullptr;
                self->_all_content._func_content = nullptr;
                self->_allocated = false;
            }
        };

        template <typename Tptr,  typename E = void>
        struct Deallocation;

        /**
         * If core type is not and array and is destructible, call in-place destructor
         * if needed in addition to basic clean-up
         **/
        template< typename T>
        struct Deallocation< T*, typename std::enable_if< !std::is_array<T>::value && std::is_destructible<T>::value >::type >{
            static void _free( PtrWrapperBaseBase* self){
              typedef T* Tptr;
                if( self->_raw_storage){
		  Tptr rawobj = (Tptr)self->_raw_storage;
                  if( self->_max >=0 ){
                    for( Py_ssize_t j = 0; j <= (self->_max); ++j){
                        rawobj[j].~T();
                    }
                  }
                  delete[] self->_raw_storage;
                } else if(self->_allocated){
		    Tptr obj = (Tptr)self->_all_content._untyped_content;
                    delete obj;
                }
                self ->_raw_storage = nullptr;
                self->_all_content._untyped_content = nullptr;
                self->_all_content._func_content = nullptr;
                self->_allocated = false;
            }
        };

        /**
         * If an array, cannot destruct an array, so should not have raw storage in this cas.
         * cover that base just in case (as best as possible), but do delete the "real" object
         * pointer
         **/
        template< typename T>
        struct Deallocation< T*, typename std::enable_if< std::is_array<T>::value && std::is_destructible<T>::value >::type >{
            static void _free( PtrWrapperBaseBase* self){
              typedef T* Tptr;
                Tptr obj = (Tptr)self->_all_content._untyped_content;
                if( self->_raw_storage){

                  delete[] self->_raw_storage;
                } else if(self->_allocated){
                    delete obj;
                }
                self ->_raw_storage = nullptr;
                self->_all_content._untyped_content = nullptr;
                self->_all_content._func_content = nullptr;
                self->_allocated = false;
            }
        };

        /**
         * cons-pointer version of destructible non-array type
         **/
        template< typename T>
        struct Deallocation< T* const, typename std::enable_if< !std::is_array<T>::value && std::is_destructible<T>::value >::type >{
            static void _free( PtrWrapperBaseBase* self){
              typedef T* Tptr;
                Tptr obj = (Tptr)self->_all_content._untyped_content;
                if( self->_raw_storage){
                  if( self->_max >=0 ){
                    for( Py_ssize_t j = 0; j <= (self->_max); ++j){
                      obj[j].~T();
                    }
                  }
                  delete[] self->_raw_storage;
                } else if(self->_allocated){
                    delete obj;
                }
                self ->_raw_storage = nullptr;
                self->_all_content._untyped_content = nullptr;
                self->_all_content._func_content = nullptr;
                self->_allocated = false;
            }
        };

        /**
         * const-pointer version of array-type destructible
         **/
        template< typename T>
        struct Deallocation< T* const, typename std::enable_if< std::is_array<T>::value && std::is_destructible<T>::value >::type >{
            static void _free( PtrWrapperBaseBase* self){
              typedef T* Tptr;
                Tptr obj = (Tptr)self->_all_content._untyped_content;
                if( self->_raw_storage){
                  delete[] self->_raw_storage;
                } else if(self->_allocated){
                    delete obj;
                }
                self ->_raw_storage = nullptr;
                self->_all_content._untyped_content = nullptr;
                self->_all_content._func_content = nullptr;
                self->_allocated = false;
            }
        };

        /**
         * for types, not destructible, just do basic deallocation
         **/
       template< typename Tptr>
       struct Deallocation< Tptr, typename std::enable_if< !std::is_destructible<typename std::remove_pointer<Tptr>::type>::value>::type  >{
          static void _free( PtrWrapperBaseBase* self){
               BasicDeallocation::_free(self);
            }
        };

        /**
         * for allocations/deallocation using new[]
         **/
        template <typename Tptr,  typename E = void>
        struct ArrayDeallocation;

        /**
         * specialize when core type is NOT destructible
         **/
        template <typename Tptr>
        struct ArrayDeallocation< Tptr, typename std::enable_if< !std::is_destructible<typename std::remove_pointer<Tptr>::type>::value >::type>{
            static void _free( PtrWrapperBaseBase* self){
                BasicDeallocation::_free(self);
            }
        };

        /**
         * specializatino when core type is not destructible
         **/
        template <typename Tptr>
        struct ArrayDeallocation< Tptr, typename std::enable_if< std::is_destructible<typename std::remove_pointer<Tptr>::type>::value >::type>{
            static void _free( PtrWrapperBaseBase* self){
            typedef typename std::remove_pointer<Tptr>::type T;
            Tptr obj = (Tptr)self->_all_content._untyped_content;
            if( self->_raw_storage){
                if( self->_max >=0 ){
                    for( Py_ssize_t j = 0; j<=self->_max; ++j){
                        obj[j].~T();
                    }
                }
                delete[] self->_raw_storage;
            } else if(self->_allocated){
                delete[] obj;
            }

            self ->_raw_storage = nullptr;
            self->_all_content._untyped_content = nullptr;
            self->_all_content._func_content = nullptr;
            self->_allocated = false;
            }
        };

        ///////////////////////////////
        // DEALLOCATION LOGIC FOR VARIOUS TYPES
        ////////////////////////////////

        template< typename T, typename PtrWrapper>
        struct BasicAlloc{
            typedef typename std::remove_reference<T>::type T_NoRef;

            class ConstructorContainer{
            public:
                typedef bool(*constructor)( const char* const kwlist[], PyObject *args, PyObject *kwds, T_NoRef* & cobj);
                ConstructorContainer( const char* const kwlist[],
                                      constructor c):_kwlist(kwlist),
                    _constructor(c){
                }

                bool operator()(PyObject* args, PyObject*kwds, T_NoRef* & cobj) const{
                    return _constructor(_kwlist, args, kwds, cobj);
                }

            private:
                const char* const * const _kwlist;
                const constructor _constructor;
            };

            typedef std::vector<ConstructorContainer> constructor_list;

            static PyObject* allocbase( PyObject* cls, PyObject *args, PyObject* kwds, typename BasicAlloc<T, PtrWrapper>::constructor_list const & constructors){
                (void)cls;
                PyObject* alloc_kwds  = PyDict_New();
                PyDict_SetItemString(alloc_kwds, "__internal_allow_null", Py_True);
                //Check if argument is list of tuples, and if so construct
                //an array of objects to store "behind the pointer"
                if( (!kwds || PyDict_Size(kwds) == 0) && args && PyTuple_Size(args) == 1 && PyList_Check(args)){
                    PyObject* list = PyTuple_GetItem(args, 0);
                    const Py_ssize_t size = PyList_Size(list);
                    char * raw_storage;
                    T_NoRef* values;
                    if(size > 1){
                        raw_storage = (char*)operator new[](size*sizeof(T));
                        memset(raw_storage, 0, size*sizeof(T));
                        values = reinterpret_cast<T_NoRef*>(raw_storage);
                    } else {
                        raw_storage = nullptr;
                        values = nullptr;
                    }
                    PyDict_SetItemString(alloc_kwds, "size", PyLong_FromSsize_t(size));
                    PtrWrapper* obj=
                        (PtrWrapper*)PyObject_Call((PyObject*)&PtrWrapper::Type, args, alloc_kwds);
                    Py_DECREF(alloc_kwds);
                    for(Py_ssize_t i = 0; i < PyTuple_Size(args); ++i){
                        PyObject* constructor_pyargs = PyList_GetItem(list, i);
                        if (!PyTuple_Check(constructor_pyargs) && !PyDict_Check(constructor_pyargs)){
                            PyErr_SetString(PyExc_TypeError, "Invalid element in list argument, expected tuple");
                            Py_DECREF(obj);
                            if(raw_storage){
                                for (Py_ssize_t j = 0; j < i; ++j){
                                    values[j].~T();
                                }
                            }
                            delete[] raw_storage;
                            PyErr_SetString(PyExc_RuntimeError, "NOTE: Freed memory, but no visible destructor available to call.");
                            return nullptr;
                        } else if (PyTuple_Check(constructor_pyargs)){
                            for (auto it = constructors.begin(); it != constructors.end(); ++it){
                            try{
                                static PyObject* emptylist = PyDict_New();
                                if ( (*it)(args, emptylist, values[i])){ break;}
                            } catch(...){
                            }
                            PyErr_Clear();
                        }
                    } else if (PyDict_Check(constructor_pyargs)){

                        for (auto it = constructors.begin(); it != constructors.end(); ++it){
                            try{
                                static PyObject* emptyargs = PyTuple_New(0);
                                if ((*it)(emptyargs, constructor_pyargs, values?values[i]:values)){ break;}
                            } catch(...){
                            }
                            PyErr_Clear();
                        }
                    }
                    if (!values || !values[i]){
                        PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation.  Objects not destructible by design! ");
                        Py_DECREF(obj);
                        if(raw_storage){
                            for (Py_ssize_t j = 0; j < i; ++j){
                                values[j].~T();
                            }
                            delete[] raw_storage;
                        }
                        return nullptr;
                    }
                }
                obj->set_contents(values, true);
                obj->_raw_storage = raw_storage;
                return (PyObject*)obj;
            }
            //otherwise, just have regular list of constructor arguments
            //for single object allocation
            T *cobj = nullptr;
            for (auto it = constructors.begin(); it != constructors.end(); ++it){
              try{
                if ((*it)(args, kwds, cobj)){ break;}
              } catch(...){
              }
              PyErr_Clear();
            }
            if (!cobj){
              Py_DECREF(alloc_kwds);
              PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
              return nullptr;
            }
            PtrWrapper* obj=
              (PtrWrapper*)PyObject_Call((PyObject*)&PtrWrapper::Type, args, alloc_kwds);
            Py_DECREF(alloc_kwds);
            obj->template set_contents<T_NoRef>(cobj);
            return (PyObject*)obj;
          }

        };


        template<typename T, bool complete,
                  typename PtrWrapper,
                  typename ClassWrapper,
                  typename Z = void>
        struct  Alloc;

        template< typename T ,
                  typename PtrWrapper,
                  typename ClassWrapper>
        struct  Alloc<T, true, PtrWrapper, ClassWrapper,
                  typename std::enable_if< !is_function_ptr<T>::value && !std::is_reference<T>::value &&
                                                std::is_constructible<T>::value &&
                                                std::is_destructible<T>::value>::type>:
            public BasicAlloc<T, PtrWrapper>{

            typedef typename std::remove_reference<T>::type T_NoRef;

            static void dealloc( T_NoRef* ptr){
                delete ptr;
            }


            static PyObject* allocbase( PyObject* cls, PyObject *args, PyObject* kwds, typename BasicAlloc<T, PtrWrapper>::constructor_list const & constructors){
                (void)cls;
                static PyObject* emptylist = PyDict_New();
                //Check if argument is list of tuples, and if so construct
                //an array of objects to store "behind the pointer"


                if( (!kwds || PyDict_Size(kwds) == 0) && args &&  (PyTuple_Size(args) == 1) &&
                    PyList_Check(PyTuple_GetItem(args, 0)) ){
                  PyObject* list = PyTuple_GetItem(args, 0);
                  const Py_ssize_t size = PyList_Size(list);
                  char * raw_storage;
                  T_NoRef** values;
                  PyObject* alloc_kwds  = PyDict_New();
                  PyDict_SetItemString(alloc_kwds, "size", PyLong_FromSsize_t(size));
                  PtrWrapper::initialize( (ClassWrapper::get_name() + "_array").c_str(),
                               (ClassWrapper::get_module_entry_name() + "_array").c_str(),
                               ClassWrapper::parent_module,
                               (std::string(ClassWrapper::Type.tp_name) +"[]").c_str());

                  PtrWrapperBaseBase* obj=
                    (PtrWrapperBaseBase*)PyObject_Call((PyObject*)&PtrWrapper::Type, emptylist, alloc_kwds);
                  if( !obj ){
                    PyErr_SetString( PyExc_RuntimeError, "Unable to create object");
                    return nullptr;
                  }
                  raw_storage = (char*)operator new[](size*sizeof(T));
                  memset(raw_storage, 0, size*sizeof(T));
                  values = (T_NoRef**)&raw_storage;
                  Py_DECREF(alloc_kwds);
                  bool found = false;
                  for(Py_ssize_t i = 0; i < size; ++i){
                    PyObject* constructor_pyargs = PyList_GetItem(list, i);
                    if (!PyTuple_Check(constructor_pyargs) && !PyDict_Check(constructor_pyargs)){
                      PyErr_SetString(PyExc_TypeError, "Invalid element in list argument, expected tuple");
                      Py_DECREF(obj);
                      if(raw_storage){
                        for (Py_ssize_t j = 0; j < i; ++j){
                          (*values)[j].~T();
                        }
                        delete[] raw_storage;
                      }
                      return nullptr;
                    } else if (PyTuple_Check(constructor_pyargs)){
                      for (auto it = constructors.begin(); it != constructors.end(); ++it){
                        try{
                          T_NoRef* cobj = &(*values)[i];
                          if ( (*it)(constructor_pyargs, nullptr, cobj)){ found = true; break;}
                        } catch(...){
                        }
                        PyErr_Clear();
                      }
                    } else if (PyDict_Check(constructor_pyargs)){
                      for (auto it = constructors.begin(); it != constructors.end(); ++it){
                        try{
                          static PyObject* emptyargs = PyTuple_New(0);
                          T_NoRef* cobj = &(*values)[i];
                          if ((*it)(emptyargs, constructor_pyargs, cobj)){ found = true; break;}
                        } catch(...){
                        }
                        PyErr_Clear();
                      }
                    }
                    if (!found || !values){
                      PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
                      Py_DECREF(obj);
                      if(raw_storage){
                        for (Py_ssize_t j = 0; j < i; ++j){
                          (*values)[j].~T();
                        }
                        delete[] raw_storage;
                      }
                      return nullptr;
                    }
                  }
                  if(raw_storage){
                    obj->_raw_storage = raw_storage;
                    obj->_all_content._untyped_content =  (void**)values;
                    obj->_allocated = false;
                  } else {
                    obj->_allocated = true;
                  }
                  return (PyObject*)obj;
                }

                //otherwise, just have regular list of constructor arguments
                //for single object allocation
                PyObject* alloc_kwds  = PyDict_New();
                PyDict_SetItemString(alloc_kwds, "__internal_allow_null", Py_True);
                T_NoRef *cobj = nullptr;
                for (auto it = constructors.begin(); it != constructors.end(); ++it){
                    try{
                        if ( (*it)(args, kwds, cobj)){ break;}
                    } catch(...){
                    }
                    PyErr_Clear();
                }
                if (!cobj){
                    Py_DECREF(alloc_kwds);
                    PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
                    return nullptr;
                }
                PtrWrapperBaseBase* obj=
                  (PtrWrapperBaseBase*)PyObject_Call((PyObject*)&PtrWrapper::Type,args, alloc_kwds);
                Py_DECREF(alloc_kwds);
                obj->_all_content._untyped_content = (void**) cobj;
                obj->_allocated = true;
                return (PyObject*)obj;
              }
        };

        template<typename T, typename PtrWrapper, typename ClassWrapper>
        struct  Alloc<T, true, PtrWrapper,
                   ClassWrapper,
                   typename std::enable_if< !is_function_ptr<T>::value && !std::is_reference<T>::value &&
                                     std::is_constructible<T>::value &&
                                    !std::is_destructible<T>::value>::type>:
            public BasicAlloc<T, PtrWrapper>{

            typedef typename std::remove_reference<T>::type T_NoRef;


            static void dealloc( T_NoRef* ptr){
                delete ptr;
            }

          static PyObject* allocbase( PyObject* cls, PyObject *args, PyObject* kwds, typename BasicAlloc<T, PtrWrapper>::constructor_list const & constructors){
            (void)cls;
            PyObject* alloc_kwds  = PyDict_New();
            PyDict_SetItemString(alloc_kwds, "__internal_allow_null", Py_True);
            //Check if argument is list of tuples, and if so construct
            //an array of objects to store "behind the pointer"
            if( (!kwds || PyDict_Size(kwds) == 0) && args && PyTuple_Size(args) == 1 && PyList_Check(args)){
              PyObject* list = PyTuple_GetItem(args, 0);
              const Py_ssize_t size = PyList_Size(list);
              char * raw_storage;
              T_NoRef* values;
              if(size > 1){
                raw_storage = (char*)operator new[](size*sizeof(T));
                memset(raw_storage, 0, size*sizeof(T));
                values = reinterpret_cast<T_NoRef*>(raw_storage);
              } else {
                raw_storage = nullptr;
                values = nullptr;
              }
              PyDict_SetItemString(alloc_kwds, "size", PyLong_FromSsize_t(size));
              PtrWrapper* obj=
                (PtrWrapper*)PyObject_Call((PyObject*)&PtrWrapper::Type, args, alloc_kwds);
              Py_DECREF(alloc_kwds);
              for(Py_ssize_t i = 0; i < PyTuple_Size(args); ++i){
                PyObject* constructor_pyargs = PyList_GetItem(list, i);
                if (!PyTuple_Check(constructor_pyargs) && !PyDict_Check(constructor_pyargs)){
                  PyErr_SetString(PyExc_TypeError, "Invalid element in list argument, expected tuple");
                  Py_DECREF(obj);
                  if(raw_storage){
                    if(raw_storage){
                      for (Py_ssize_t j = 0; j < i; ++j){
                        values[j].~T();
                      }
                    }
                    delete[] raw_storage;
                  }
                  PyErr_SetString(PyExc_RuntimeError, "NOTE: Freed memory, but no visible destructor available to call.");
                  return nullptr;
                } else if (PyTuple_Check(constructor_pyargs)){
                  for (auto it = constructors.begin(); it != constructors.end(); ++it){
                    try{
                      static PyObject* emptylist = PyDict_New();
                      if ( (*it)(args, emptylist, values[i])){ break;}
                    } catch(...){
                    }
                    PyErr_Clear();
                  }
                } else if (PyDict_Check(constructor_pyargs)){

                  for (auto it = constructors.begin(); it != constructors.end(); ++it){
                    try{
                      static PyObject* emptyargs = PyTuple_New(0);
                      if ((*it)(emptyargs, constructor_pyargs, values?values[i]:values)){ break;}
                    } catch(...){
                    }
                    PyErr_Clear();
                  }
                }
                if (!values || !values[i]){
                  PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation.  Objects not destructible by design! ");
                  Py_DECREF(obj);
                  if(raw_storage){
                    for (Py_ssize_t j = 0; j < i; ++j){
                      values[j].~T();
                    }

                    delete[] raw_storage;
                  }

                  return nullptr;

                }
              }
              obj->set_contents(values, true);
              obj->_raw_storage = raw_storage;
              return (PyObject*)obj;
            }
            //otherwise, just have regular list of constructor arguments
            //for single object allocation
            T *cobj = nullptr;
            for (auto it = constructors.begin(); it != constructors.end(); ++it){
              try{
                if ((*it)(args, kwds, cobj)){ break;}
              } catch(...){
              }
              PyErr_Clear();
            }
            if (!cobj){
              Py_DECREF(alloc_kwds);
              PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
              return nullptr;
            }
            PythonTWrapper<T_NoRef*, true, -1, PythonBase, void>* obj=
              (PythonTWrapper<T_NoRef*, true, -1, PythonBase, void>*)PyObject_Call((PyObject*)&PythonTWrapper<T_NoRef*, true, -1, PythonBase, void>::Type,
                                                                         args, alloc_kwds);
            Py_DECREF(alloc_kwds);
            obj->template set_contents<T>(cobj);
            return (PyObject*)obj;
          }

        };

        template<typename ReturnType, typename ...Args>
        struct  Alloc<ReturnType(*)(Args...), true, PythonClassWrapper<ReturnType(**)(Args...), true, -1, PythonBase, void>,
                  PythonClassWrapper<ReturnType(*)(Args...), true, -1, PythonBase, void>,
                   void >:
            public BasicAlloc<ReturnType(*)(Args...),  PythonClassWrapper< ReturnType(**)(Args...), true, -1, PythonBase, void> >{

            typedef PythonClassWrapper< ReturnType(**)(Args...), true, -1, PythonBase, void> PtrWrapper;
            typedef ReturnType(*T)(Args...);
            typedef typename std::remove_reference<T>::type T_NoRef;

            static void dealloc( T_NoRef* ptr){
              delete ptr;
            }

            static PyObject* allocbase( PyObject* cls, PyObject *args, PyObject* kwds, typename BasicAlloc<T, PtrWrapper>::constructor_list const & constructors){
                (void)cls;
                PyObject* alloc_kwds  = PyDict_New();
                PyDict_SetItemString(alloc_kwds, "__internal_allow_null", Py_True);
                //Check if argument is list of tuples, and if so construct
                //an array of objects to store "behind the pointer"
                if( (!kwds || PyDict_Size(kwds) == 0) && args && PyTuple_Size(args) == 1 && PyList_Check(args)){
                    PyObject* list = PyTuple_GetItem(args, 0);
                    const Py_ssize_t size = PyList_Size(list);
                    char * raw_storage;
                    T_NoRef* values;
                    if (size > 1){
                        raw_storage = (char*)operator new[](size*sizeof(T));
                        memset(raw_storage, 0, size*sizeof(T));
                        values = reinterpret_cast<T_NoRef*>(raw_storage);
                    } else {
                        raw_storage = nullptr;
                        values = nullptr;
                    }
                    PyDict_SetItemString(alloc_kwds, "size", PyLong_FromSsize_t(size));
                    PtrWrapper* obj=
                      (PtrWrapper*)PyObject_Call((PyObject*)&PtrWrapper::Type, args, alloc_kwds);
                    Py_DECREF(alloc_kwds);
                    for(Py_ssize_t i = 0; i < PyTuple_Size(args); ++i){
                      PyObject* constructor_pyargs = PyList_GetItem(list, i);
                      if (!PyTuple_Check(constructor_pyargs) && !PyDict_Check(constructor_pyargs)){
                        PyErr_SetString(PyExc_TypeError, "Invalid element in list argument, expected tuple");
                        Py_DECREF(obj);
                        if (raw_storage){
                          for (Py_ssize_t j = 0; j < i; ++j){
                            values[j].~T();
                          }
                          delete[] raw_storage;
                        }
                        return nullptr;
                      } else if (PyTuple_Check(constructor_pyargs)){
                        for (auto it = constructors.begin(); it != constructors.end(); ++it){
                          try{
                            T_NoRef* cobj = &(*values)[i];
                            if ((*it)(constructor_pyargs, nullptr,cobj)){ break;}
                          } catch(...){
                          }
                          PyErr_Clear();
                        }
                      } else if (PyDict_Check(constructor_pyargs)){
                        for (auto it = constructors.begin(); it != constructors.end(); ++it){
                          try{
                            static PyObject* emptyargs = PyTuple_New(0);
                            T_NoRef* cobj = &(*values)[i];
                            if ((*it)(emptyargs, constructor_pyargs, cobj )){ break;}
                          } catch(...){
                          }
                          PyErr_Clear();
                        }

                        if (!values || !values[i]){
                          PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
                          Py_DECREF(obj);
                          if(raw_storage){
                            for (Py_ssize_t j = 0; j < i; ++j){
                              values[j].~T();
                            }
                            delete[] raw_storage;
                          }
                          return nullptr;
                        }
                    }
                }
                if(!raw_storage){
                  PyErr_SetString( PyExc_RuntimeError, "Invalid object creation");
                  return nullptr;
                }
                obj->_raw_storage = raw_storage;
                obj->set_contents((void**)&obj->_raw_storage, true);
                obj->_raw_storage = raw_storage;
                return (PyObject*)obj;
              }

              //otherwise, just have regular list of constructor arguments
              //for single object allocation
              T *cobj = nullptr;
              for (auto it = constructors.begin(); it != constructors.end(); ++it){
                try{
                  if ((cobj = (*it)(args, kwds ))){ break;}
                } catch(...){
                }
                PyErr_Clear();
              }
              if (!cobj){
                Py_DECREF(alloc_kwds);
                PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
                return nullptr;
              }
              PythonTWrapper<T_NoRef*, true, -1, PythonBase, void>* obj=
                (PythonTWrapper<T_NoRef*, true, -1, PythonBase, void>*)PyObject_Call((PyObject*)&PythonTWrapper<T_NoRef*, true, -1, PythonBase, void>::Type,
                                                                        args, alloc_kwds);
              Py_DECREF(alloc_kwds);
              obj->set_contents((void**)cobj);
              return (PyObject*)obj;
            }
        };

        template<typename T, bool is_complete>
        struct  Alloc<T, is_complete,
                       PythonClassWrapper< void*, is_complete, -1, PythonBase, void>,
                       PythonClassWrapper< void, is_complete, -1, PythonBase, void>,
                       typename std::enable_if< std::is_void<T>::value>::type>:
            public BasicAlloc<T,  PythonClassWrapper< void*, false, -1, PythonBase, void> >{

            typedef  PythonClassWrapper< void*, is_complete, -1, PythonBase, void> PtrWrapper;
            typedef typename std::remove_reference<T>::type C_NoRef;

            static void dealloc( C_NoRef* ptr){
              (void) ptr;
            }

            static PyObject* allocbase( PyObject* cls, PyObject *args, PyObject* kwds, typename BasicAlloc<T, PtrWrapper>::constructor_list const & constructors){
              (void) args; (void) kwds; (void)cls;
              PyErr_SetString(PyExc_RuntimeError, "Type is not directly constructible");
              return nullptr;
            }
        };

       template<typename T, bool is_complete>
        struct  Alloc<const T, is_complete,
                       PythonClassWrapper< const void*, is_complete, -1, PythonBase, void>,
                       PythonClassWrapper< const void, is_complete, -1, PythonBase, void>,
                       typename std::enable_if< std::is_void<T>::value>::type>:
            public BasicAlloc<T,  PythonClassWrapper< void*, false, -1, PythonBase, void> >{

            typedef typename std::remove_reference<const T>::type C_NoRef;

            static void dealloc( C_NoRef* ptr){
              (void) ptr;
            }

            static PyObject* allocbase( PyObject* cls, PyObject *args, PyObject* kwds){
              (void) args; (void) kwds; (void)cls;
              PyErr_SetString(PyExc_RuntimeError, "Type is not directly constructible");
              return nullptr;
            }
        };

        template<typename T, bool is_complete, typename PtrWrapper, typename ClassWrapper>
        struct  Alloc<T, is_complete, PtrWrapper,
                   ClassWrapper,
                   typename std::enable_if< !std::is_void<T>::value && !std::is_function<T>::value &&
                         (std::is_reference<T>::value || !std::is_constructible<T>::value)>::type>:
            public BasicAlloc<T, PtrWrapper>{

            typedef typename std::remove_reference<T>::type C_NoRef;

            static void dealloc( C_NoRef* ptr){
              (void)ptr;
            }

            static PyObject* allocbase( PyObject* cls, PyObject *args, PyObject* kwds, typename BasicAlloc<T, PtrWrapper>::constructor_list const & constructors){
              (void) args; (void) kwds; (void)cls; (void)constructors;
              PyErr_SetString(PyExc_RuntimeError, "Type is not directly constructible");
              return nullptr;
            }
        };

        template<typename T, bool is_complete, typename PtrWrapper, typename ClassWrapper>
        struct  Alloc<T, is_complete,  PtrWrapper,
                  ClassWrapper,
                  typename std::enable_if<std::is_function<T>::value>::type>:
            public BasicAlloc<T, PtrWrapper>{

            typedef typename std::remove_reference<T>::type C_NoRef;

            static void dealloc( C_NoRef* ptr){
                (void)ptr;
            }

            static PyObject* allocbase( PyObject* cls, PyObject *args, PyObject* kwds, typename BasicAlloc<T, PtrWrapper>::constructor_list const & constructors){
              (void) args; (void) kwds; (void)cls; (void)constructors;
              PyErr_SetString(PyExc_RuntimeError, "Type is not directly constructible");
              return nullptr;
            }
        };

    };

}
#endif // PYLLARS_INTERNAL__OBJECTLIFECYCLEHELPERS_H
