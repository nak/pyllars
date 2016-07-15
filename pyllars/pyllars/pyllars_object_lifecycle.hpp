#ifndef PYLLARS_INTERNAL__OBJECTLIFECYCLEHELPERS_H
#define PYLLARS_INTERNAL__OBJECTLIFECYCLEHELPERS_H

#include <type_traits>
#include <vector>

#include <sys/types.h>
#include <Python.h>

#include "pyllars_defns.hpp"
//#include "pyllars_pointer.hpp"
#include "pyllars_classwrapper.hpp"

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
namespace __pyllars_internal {


    template<typename C, typename Z>
    struct PythonClassWrapper;

    template<typename T>
    struct ObjConatinerPtrProxy;

    class ObjectLifecycleHelpers {
    public:


        template<typename CClass, typename Z>
        friend
        struct PythonClassWrapper;

        template<typename T, bool is_array, typename ClassWrapper, typename E>
        friend class CObjectConversionHelper;

        template<typename T, typename PtrWrapper, typename E>
        friend class PyObjectConversionHelper;

        template<typename T>
        friend
        struct ObjConatinerPtrProxy;


    private:

        template<typename T, typename Z= void>
        struct Array;

        template<typename T>
        struct Array<T, typename std::enable_if<is_complete<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type> {
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

            static T_base &at(T array, size_t index) {
                return array[index];
            }
        };

        template<typename T>
        struct Array<T, typename std::enable_if<!is_complete<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type> {
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

            static T_base &at(T array, size_t index) {
                throw "Cannot dereference incomplete type";
            }
        };

    public:
        template<typename T, typename Z = void>
        struct Copy;

        template<typename T>
        struct Copy<T, typename std::enable_if<std::is_destructible<typename std::remove_reference<T>::type>::value &&
                                               std::is_assignable<typename std::remove_reference<T>::type, typename std::remove_reference<T>::type>::value &&
                                               std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> {
            typedef typename std::remove_volatile<typename std::remove_reference<T>::type>::type T_NoRef;

            static ObjContainer <T_NoRef> *new_copy(const T &value) {
                return new ObjContainerProxy<T_NoRef, const T &>(value);
            }

            static T_NoRef *new_copy(T_NoRef *const value) {
                return new T_NoRef(*value);
            }

            static void inplace_copy(T_NoRef *const to, const Py_ssize_t index, const T_NoRef *const from,
                                     const bool in_place) {
                if (in_place) {
                    to[index].~T();
                    new(&to[index]) T_NoRef(*from);
                } else {
                    to[index] = *from;
                }

            }
        };

        template<typename T>
        struct Copy<T, typename std::enable_if<
                (!std::is_assignable<typename std::remove_reference<T>::type, typename std::remove_reference<T>::type>::value ||
                 !std::is_destructible<typename std::remove_reference<T>::type>::value) &&
                std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> {
            typedef typename std::remove_reference<T>::type T_NoRef;

            static ObjContainer <T_NoRef> *new_copy(const T &value) {
                return new ObjContainerProxy<T_NoRef, const T &>(value);
            }

            static ObjContainer <T_NoRef> *new_copy(T_NoRef *const value) {
                return new ObjContainerProxy<T_NoRef, T_NoRef &>(*value);
            }

            static void inplace_copy(T_NoRef *const to, const Py_ssize_t index, const T_NoRef *const from,
                                     const bool in_place) {
                throw "Cannot copy over item of non-destructible type";
            }
        };

        template<typename T>
        struct Copy<T, typename std::enable_if<std::is_void<T>::value>::type> {
            static ObjContainer<void> *new_copy(T *const value) {
                throw "Attempt to copy void object";
            }

            static void inplace_copy(T *const to, const Py_ssize_t index, const T *const from, const bool in_place) {
                throw "Cannot copy void item";
            }
        };

        template<typename T, size_t size>
        struct Copy<T[size], typename std::enable_if<(size > 0)>::type> {
            typedef T T_array[size];

            static ObjContainer <T_array> *new_copy(const T_array &value) {
                T_array *new_value = new T_array[1];
                memcpy(*new_value, value, sizeof(T[size]));
                return new ObjContainerPtrProxy<T_array, true>(new_value, true);
            }

            static ObjContainer <T_array> *new_copy(T_array *const value) {
                return new ObjContainerPtrProxy<T_array, true>(value, false);
            }

            static void inplace_copy(T_array *const to, const Py_ssize_t index, const T_array *const from,
                                     const bool in_place) {
                for (ssize_t i = 0; i < size; ++i) {
                    (*to)[i] = (*from)[i];
                }
            }
        };

        template<typename T>
        struct Copy<T, typename std::enable_if<!std::is_void<T>::value &&
                                               (!std::is_array<T>::value || ArraySize<T>::size <= 0) &&
                                               !std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> {
            typedef typename std::remove_reference<T>::type T_NoRef;

            static ObjContainer <T_NoRef> *new_copy(const T_NoRef &value) {
                (void) value;
                throw "Attempt to copy non-copy-constructible object";
            }

            static ObjContainer <T_NoRef> *new_copy(T_NoRef *const value) {
                (void) value;
                throw "Attempt to copy non-copy-constructible object";
            }


            static void inplace_copy(T_NoRef *const to, const Py_ssize_t index, const T_NoRef *const from,
                                     const bool in_place) {
                throw "Attempt to copy non-copy-constructible object";
            }
        };

    private:
        template<typename To, typename From, typename Z = void>
        struct Assign;

        template<typename To, typename From>
        struct Assign<To, From, typename std::enable_if<
                !std::is_array<To>::value && std::is_assignable<typename std::remove_reference<To>::type,
                        typename std::remove_reference<From>::type>::value>::type> {

            static void assign(To &to, const Py_ssize_t index, const From &from, const size_t unused = 0) {
                (void) unused;
                to = from;
            }

        };

        template<typename To, typename From, size_t size>
        struct Assign<To[size], From[size], typename std::enable_if<std::is_assignable<typename std::remove_reference<To>::type,
                typename std::remove_reference<From>::type>::value>::type> {

            static void assign(To &to, const From &from, const size_t unused = 0) {
                (void) unused;
                for (size_t i = 0; i < size; ++i)
                    to[i] = from[i];
            }

        };

        template<typename To, typename From>
        struct Assign<To[], From[], typename std::enable_if<std::is_assignable<typename std::remove_reference<To>::type,
                typename std::remove_reference<From>::type>::value>::type> {

            static void assign(To &to, const From &from, const size_t size) {
                for (size_t i = 0; i < size; ++i)
                    to[i] = from[i];
            }

        };


        template<typename To, typename From>
        struct Assign<To, From, typename std::enable_if<!std::is_assignable<typename std::remove_reference<To>::type,
                typename std::remove_reference<From>::type>::value>::type> {

            static void assign(To &to, const From &from, const size_t unused = 0) {
                (void) unused;
                (void) to;
                (void) from;
                throw "Attempt to assign to unassignable object";
            }

        };



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
        template<typename T, typename Wrapper = PythonClassWrapper<T>, typename Z = void>
        class ObjectContent;

        /**
         * Specialization for non-pointer copiable and assignable class (instances)
         **/
        template<typename T, typename ClassWrapper>
        class ObjectContent<T, ClassWrapper,
                typename std::enable_if<
                        !std::is_void<T>::value && !std::is_pointer<T>::value && !std::is_array<T>::value>::type> {
        public:

            static PyObject *getObjectAt(T const from, const size_t index, const ssize_t elements_array_size,
                                         const size_t depth, const bool asArgument = true) {
	      PyErr_SetString(PyExc_TypeError,"Attempt to get element from non-array object");
	      return nullptr;
            }

            static void set(const size_t index, T *const to, const T *const from, const size_t depth) {
                if (depth > 1) {
                    Assign<T *, T *>::assign(((T **) to)[index], *((T **) from));
                } else {
                    Assign<T, T>::assign(to[index], *from);
                }
            }

            static T *getObjectPtr(ClassWrapper *const self) {
                return (T *) self->get_CObject();
            }

        };

        /**
         * Specialization for non-const copy-constructible non-ptr-to-void non-const pointer types
         **/
        template<typename T, typename PtrWrapper>
        class ObjectContent<T, PtrWrapper,
                typename std::enable_if<!std::is_function<typename std::remove_pointer<T>::type>::value &&
                                        !std::is_void<typename std::remove_pointer<T>::type>::value &&
                                        is_complete<typename std::remove_pointer<T>::type>::value &&
                                        (std::is_array<T>::value || std::is_pointer<T>::value)>::type> {
        public:
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

            static PyObject *getObjectAt(T from, const size_t index, const ssize_t elements_array_size,
                                         const size_t depth,
                                         const bool asArgument = true) {
                //TODO add reference to owning element to this object to not have go out of scope
                //until parent does!!!!
                if (depth == 1) {
                    return toPyObject<T_base>(Array<T>::at(from, index), true, elements_array_size);
                } else {
                    T *from_real = (T *) from;
                    return toPyObject<T>(from_real[index], asArgument, elements_array_size, depth - 1);
                }
            }

            static void set(const size_t index, T& to, T const from, const size_t depth) {
                if (depth == 1) {
                    Assign<T, T>::assign(Array<T>::at(to, index), *from);
                } else {
                    typedef typename std::remove_reference<T>::type T_NoRef;
                    Assign<T_NoRef, T_NoRef>::assign(Array<T_NoRef *>::at((T_NoRef *) to, index), *((T_NoRef *) from));
                }
            }

            static T *getObjectPtr(PtrWrapper *const self) {
                return (T *) &self->_CObject;
            }

        };

        /**
         * Specialization for non-const copy-constructible non-ptr-to-void non-const pointer types
         **/
        template<typename T, typename PtrWrapper>
        class ObjectContent<T, PtrWrapper,
                typename std::enable_if<std::is_function<typename std::remove_pointer<T>::type>::value>::type> {
        public:

            static PyObject *getObjectAt(T const from, const size_t index, const ssize_t elements_array_size,
                                         const size_t depth, const bool asArgument = true) {
	      PyErr_SetString(PyExc_TypeError,"Attempt to get element from non-array object");
	      return nullptr;
            }

            static void set(const size_t index, T const to, T const from, const size_t depth) {
                throw "Attempt to index function pointer";
            }

            static T *getObjectPtr(PtrWrapper *const self) {
                return (T *) &self->_CObject;
            }

        };

        /*
        * Specialization for non-const copy-constructible non-ptr-to-void non-const pointer types
        **/
        template<typename T, typename PtrWrapper>
        class ObjectContent<T, PtrWrapper,
                typename std::enable_if<!is_complete<typename std::remove_pointer<T>::type>::value &&
                                        !std::is_function<typename std::remove_pointer<T>::type>::value &&
                                        !std::is_void<typename std::remove_pointer<T>::type>::value>::type> {
        public:

            static PyObject *getObjectAt(T const from, const size_t index, const ssize_t elements_array_size,
                                         const size_t depth, const bool asArgument = true) {
	      PyErr_SetString(PyExc_TypeError,"Attempt to get element from non-array object");
	      return nullptr;
            }

            static void set(const size_t index, T const to, T const from, const size_t depth) {
                throw "Attempt to index function pointer";
            }

            static T *getObjectPtr(PtrWrapper *const self) {
                return (T *) &self->_CObject;
            }

        };

        ///////////////////////////////
        // DEALLOCATION LOGIC FOR VARIOUS TYPES
        ////////////////////////////////

        /**
         * Basic deallocation is to assume no special deconstruction is needed, just
         * basic cleanup
         **/
        template<typename PtrWrapper>
        struct BasicDeallocation {
            static void _free(PtrWrapper *self) {
                self->delete_raw_storage();
                if (self->_allocated) self->_CObject = nullptr;
                self->_allocated = false;
            }
        };

        template<typename Tptr, typename PtrWrapper, typename E = void>
        struct Deallocation;

        /**
         * If core type is not and array and is destructible, call in-place destructor
         * if needed in addition to basic clean-up
         **/
        template<typename T, typename PtrWrapper>
        struct Deallocation<T, PtrWrapper, typename std::enable_if<
                (std::is_pointer<T>::value || std::is_array<T>::value) &&
                std::is_class<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value &&
                std::is_destructible<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value
        >::type> {

            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

            static void _free(PtrWrapper *self) {
                if (self->_raw_storage) {
                    if (self->_depth == 1 && self->_max >= 0) {
                        for (Py_ssize_t j = 0; j <= self->_max; ++j) {
                            ((*self->_CObject)[j]).~T_base();
                        }
                    }
		    char * raw_storage = (char*) self->_raw_storage;
                    delete[] raw_storage;
                } else if (self->_allocated) {
                    delete self->_CObject;
                }
                self->_raw_storage = nullptr;
                if (self->_allocated) self->_CObject = nullptr;
                self->_allocated = false;
            }
        };

        /**
         * If core type is not and array and is destructible, call in-place destructor
         * if needed in addition to basic clean-up
         **/
        template<typename T, typename PtrWrapper>
        struct Deallocation<T, PtrWrapper, typename std::enable_if<
                (std::is_pointer<T>::value || std::is_array<T>::value) &&
                !std::is_class<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value &&
                std::is_destructible<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value
        >::type> {
            static void _free(PtrWrapper *self) {
                if (self->_raw_storage) {
                    // No class type here, so no need to call inline destructor
                    delete[] self->_raw_storage;
                } else if (self->_allocated) {
                    if (std::is_array<T>::value) {
                        delete[] self->_CObject;
                    } else {
                        delete self->_CObject;
                    }
                }
                self->_raw_storage = nullptr;
                if (self->_allocated) self->_CObject = nullptr;
                self->_allocated = false;
            }
        };

        /**
         * If core type is not and array and is destructible, call in-place destructor
         * if needed in addition to basic clean-up
         **/
        template<typename T, typename PtrWrapper>
        struct Deallocation<T, PtrWrapper, typename std::enable_if<
                !std::is_destructible<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type> {
            static void _free(PtrWrapper *self) {
                if (self->_raw_storage) {
                    // No class type here, so no need to call inline destructor
                    delete[] self->_raw_storage;
                } else if (self->_allocated) {
                    //Not destructible, so no delete call
                }
                self->_raw_storage = nullptr;
                if (self->_allocated) self->_CObject = nullptr;
                self->_allocated = false;
            }
        };


        ///////////////////////////////
        // DEALLOCATION LOGIC FOR VARIOUS TYPES
        ////////////////////////////////

        template<typename T>
        struct BasicAlloc {
            typedef typename std::remove_reference<T>::type T_NoRef;
  	    typedef PythonClassWrapper<T_NoRef*> PtrWrapper;
            class ConstructorContainer {
            public:
                typedef bool(*constructor)(const char *const kwlist[], PyObject *args, PyObject *kwds,
                                           ObjContainer <T_NoRef> *&cobj, const bool in_place);

                ConstructorContainer(const char *const kwlist[],
                                     constructor c) : _kwlist(kwlist),
                                                      _constructor(c) {
                }

	        bool operator()(PyObject *args, PyObject *kwds, ObjContainer <T_NoRef> *&cobj, const bool in_place) const {
                    return _constructor(_kwlist, args, kwds, cobj, in_place);
                }

            private:
                const char *const *const _kwlist;
                const constructor _constructor;
            };

            typedef std::vector<ConstructorContainer> constructor_list;

            static PyObject *allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                                       typename BasicAlloc<T>::constructor_list const &constructors) {
                (void) cls;
                //Check if argument is list of tuples, and if so construct
                //an array of objects to store "behind the pointer"
                Py_ssize_t size = 1;
                if ((!kwds || PyDict_Size(kwds) == 0) && args && PyTuple_Size(args) == 1 && PyList_Check(args)) {
                    PyObject *list = PyTuple_GetItem(args, 0);
                    size = PyList_Size(list);
                    char *raw_storage;
                    T_NoRef *values;
                    if (size > 1) {
                        raw_storage = (char *) operator new[](size * sizeof(T));
                        memset(raw_storage, 0, size * sizeof(T));
                        values = reinterpret_cast<T_NoRef *>(raw_storage);
                    } else {
                        raw_storage = nullptr;
                        values = nullptr;//constructor call will allocate and construct this if null
                    }
                    for (Py_ssize_t i = 0; i < PyTuple_Size(args); ++i) {
                        PyObject *constructor_pyargs = PyList_GetItem(list, i);
                        if (!PyTuple_Check(constructor_pyargs) && !PyDict_Check(constructor_pyargs)) {
                            PyErr_SetString(PyExc_TypeError, "Invalid element in list argument, expected tuple");
                            if (raw_storage) {
                                for (Py_ssize_t j = 0; j < i; ++j) {
                                    values[j].~T();
                                }
                            }
                            delete[] raw_storage;
                            PyErr_SetString(PyExc_RuntimeError,
                                            "NOTE: Freed memory, but no visible destructor available to call.");
                            return nullptr;
                        } else if (PyTuple_Check(constructor_pyargs)) {
                            for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                                try {
                                    static PyObject *emptylist = PyDict_New();
				    ObjContainer<T_NoRef>  *o = new ObjContainerPtrProxy<T_NoRef, false, true>(&values[i], size, true); 
                                    if ((*it)(args, emptylist, o, true)) { 
				      delete o;
				      break; 
				    }
				    delete o;
                                } catch (...) {
                                }
                                PyErr_Clear();
                            }
                        } else if (PyDict_Check(constructor_pyargs)) {

                            for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                                try {
                                    static PyObject *emptyargs = PyTuple_New(0);
				    ObjContainer<T_NoRef> * o = values? new ObjContainerPtrProxy<T_NoRef, false, true>(&values[i], 1, true): nullptr;
                                    if ((*it)(emptyargs, constructor_pyargs, o, false)) { 
				      delete o;
				      break; 
				    }
				    delete o;
                                } catch (...) {
                                }
                                PyErr_Clear();
                            }
                        }
                        if (!values ) {
                            PyErr_SetString(PyExc_RuntimeError,
                                            "Invalid constructor arguments on allocation.  Objects not destructible by design! ");
                            if (raw_storage) {
                                for (Py_ssize_t j = 0; j < i; ++j) {
                                    values[j].~T();
                                }
                                delete[] raw_storage;
                            }
                            return nullptr;
                        }
                    }
                    PtrWrapper *obj =
		      (PtrWrapper *) PtrWrapper::createPy2(size, &values, !raw_storage, false);
		    typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
                    obj->_raw_storage = (T_base*) raw_storage;
                    return (PyObject *) obj;
                }
                //otherwise, just have regular list of constructor arguments
                //for single object allocation
		char *raw = new char[sizeof(T_NoRef)];
		T_NoRef* ccobj = (T_NoRef*)raw;
                ObjContainerPtrProxy<T_NoRef, false, true> cobj = 
		  ObjContainerPtrProxy<T_NoRef, false, true>(ccobj, 1, true);
		ObjContainer<T_NoRef> *oo = & cobj;
		bool found = false;
                for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                    try {
		      if ((*it)(args, kwds, oo, true)) { found = true; break; }
                    } catch (...) {
                    }
                    PyErr_Clear();
                }
                if (!found) {
                    PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
		    delete[] raw;
                    return nullptr;
                }
                PtrWrapper *obj = PtrWrapper::createPy2(size, &ccobj, true, true);
                return (PyObject *) obj;
            }

        };


        template<typename T,
                typename PtrWrapper,
                typename ClassWrapper,
                typename Z = void>
        struct Alloc;

        template<typename T,
                typename PtrWrapper,
                typename ClassWrapper>
        struct Alloc<T, PtrWrapper, ClassWrapper,
                typename std::enable_if<!is_function_ptr<T>::value && !std::is_reference<T>::value &&
                                        std::is_constructible<T>::value &&
                                        std::is_destructible<T>::value>::type> :
                public BasicAlloc<T> {

            typedef typename std::remove_reference<T>::type T_NoRef;

            static void deallocREMOVE(ObjContainer <T_NoRef> *ptr, const bool inPlace, const size_t size) {
                if( inPlace && ptr){
                    for(size_t i =0; i < size; ++i){
                        ptr->ptr()[i].~T();
                    }
                } else {
                    delete ptr;
                }
            }


            static PyObject *allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                                       typename BasicAlloc<T>::constructor_list const &constructors) {
                (void) cls;
                //Check if argument is list of tuples, and if so construct
                //an array of objects to store "behind the pointer"

                Py_ssize_t size = 1;
                if ((!kwds || PyDict_Size(kwds) == 0) && args && (PyTuple_Size(args) == 1) &&
                    PyList_Check(PyTuple_GetItem(args, 0))) {
                    PyObject *list = PyTuple_GetItem(args, 0);
                    size = PyList_Size(list);
                    char *raw_storage;
                    T_NoRef *values;
                    PtrWrapper::initialize();


                    raw_storage = (char *) operator new[](size * sizeof(T));
                    memset(raw_storage, 0, size * sizeof(T));
                    values = (T_NoRef *) raw_storage;
                    for (Py_ssize_t i = 0; i < size; ++i) {
                        bool found = false;
                        PyObject *constructor_pyargs = PyList_GetItem(list, i);
                        if (!PyTuple_Check(constructor_pyargs) && !PyDict_Check(constructor_pyargs)) {
                            PyErr_SetString(PyExc_TypeError, "Invalid element in list argument, expected tuple");
			                //clean up before singalling exception
                            if (raw_storage) {
                                for (Py_ssize_t j = 0; j < i; ++j) {
                                    values[j].~T();
                                }
                                delete[] raw_storage;
                                raw_storage = nullptr;
                            }
			                //signal exception:
                            return nullptr;
                        } else if (PyTuple_Check(constructor_pyargs)) {
                            for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                                try {
                                    // declare t NOT be in-place as we will destroy the object created here
                                    ObjContainerPtrProxy<T_NoRef, false, false> cobj = ObjContainerPtrProxy<T_NoRef, false, false>(&values[i], size, false);
                                    ObjContainer<T_NoRef> *cobjptr = &cobj;
                                    if ((*it)(constructor_pyargs, nullptr, cobjptr, false)) {
                                        found = true;
                                        break;
                                    }
                                } catch (...) {
                                }
                                PyErr_Clear();
                            }
                        } else if (PyDict_Check(constructor_pyargs)) {
                            for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                                try {
                                    static PyObject *emptyargs = PyTuple_New(0);
                                    // declare t NOT be in-place as we will destroy the object created here
                                    ObjContainerPtrProxy<T_NoRef,false, false> cobj = ObjContainerPtrProxy<T_NoRef, false, false>(&values[i], size, false);
                                    ObjContainer<T_NoRef> *cobjptr = &cobj;
                                    if ((*it)(emptyargs, constructor_pyargs, cobjptr, false)) {
                                        found = true;
                                        break;
                                    }
                                } catch (...) {
                                }
                                PyErr_Clear();
                            }
                        }
                        if (!found || !values) {
                            PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
                            if (raw_storage) {
                                for (Py_ssize_t j = 0; j < i; ++j) {
                                    values[j].~T();
                                }
                                delete[] raw_storage;
                                raw_storage = nullptr;
                            }
                            return nullptr;
                        }
                    }
                    PtrWrapper *obj = (PtrWrapper *) PtrWrapper::createPy2(size, &values, false, true/*inplace*/);
                    if (raw_storage && obj) {
                        obj->set_raw_storage(values, size);
                    } else if (raw_storage){ // obj==nullptr
                        //clean up, since will be signalling exception
                        for (Py_ssize_t j = 0; j < size; ++j) {
                            values[j].~T();
                        }
                        delete[] raw_storage;
                        raw_storage = nullptr;
		            }
                    return (PyObject *) obj;
                }

                //otherwise, just have regular list of constructor arguments
                //for single object allocation
                PyObject *alloc_kwds = PyDict_New();
                PyDict_SetItemString(alloc_kwds, "__internal_allow_null", Py_True);
                ObjContainer<T_NoRef> *cobj = nullptr;
                for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                    try {
                        if ((*it)(args, kwds, cobj, false)) { break; }
                    } catch (...) {
                    }
                    PyErr_Clear();
                }
                if (!cobj) {
                    Py_DECREF(alloc_kwds);
                    PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
                    return nullptr;
                }
                return (PyObject *) PtrWrapper::createPy2(size, cobj ? cobj->ptrptr() : nullptr, true, false);
            }
        };

        template<typename T, typename PtrWrapper, typename ClassWrapper>
        struct Alloc<T, PtrWrapper,
                ClassWrapper,
                typename std::enable_if<!is_function_ptr<T>::value && !std::is_reference<T>::value &&
                                        std::is_constructible<T>::value &&
                                        !std::is_destructible<T>::value>::type> :
                public BasicAlloc<T> {

            typedef typename std::remove_reference<T>::type T_NoRef;


            static void deallocREMOVE(ObjContainer <T_NoRef> *ptr , const bool inPlace, const size_t size) {
                if( inPlace && ptr){
                    for(size_t i =0; i < size; ++i){
                        ptr->ptr()[i].~T();
                    }
                } else {
                    delete ptr;
                }
            }

            static PyObject *allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                                       typename BasicAlloc<T>::constructor_list const &constructors) {
                (void) cls;
                //Check if argument is list of tuples, and if so construct
                //an array of objects to store "behind the pointer"
                if ((!kwds || PyDict_Size(kwds) == 0) && args && PyTuple_Size(args) == 1 && PyList_Check(args)) {
                    PyObject *list = PyTuple_GetItem(args, 0);
                    const Py_ssize_t size = PyList_Size(list);
                    char *raw_storage;
                    T_NoRef *values;
                    //if (size > 1) {
                        raw_storage = (char *) operator new[](size * sizeof(ObjContainer<T_NoRef>));
                        memset(raw_storage, 0, size * sizeof(T));
                        values = reinterpret_cast<T_NoRef *>(raw_storage);
                    //} else {
                    //    raw_storage = nullptr;
                    //    values = nullptr;
                    //}
                    for (Py_ssize_t i = 0; i < PyTuple_Size(args); ++i) {
                        bool found = false;
                        PyObject *constructor_pyargs = PyList_GetItem(list, i);
                        if (!PyTuple_Check(constructor_pyargs) && !PyDict_Check(constructor_pyargs)) {
                            PyErr_SetString(PyExc_TypeError, "Invalid element in list argument, expected tuple");
                            if (raw_storage) {
                                if (raw_storage) {
                                    for (Py_ssize_t j = 0; j < i; ++j) {
                                        values[j].~T();
                                    }
                                }
                                delete[] raw_storage;
                            }
                            PyErr_SetString(PyExc_RuntimeError,
                                            "NOTE: Freed memory, but no visible destructor available to call.");
                            return nullptr;
                        } else if (PyTuple_Check(constructor_pyargs)) {
                            for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                                try {
                                    static PyObject *emptylist = PyDict_New();
                                    ObjContainer<T_NoRef > cobj = ObjContainerPtrProxy<T_NoRef, false, false>(&values[i]);
                                    if ((*it)(args, emptylist, cobj)) {
                                        found = true;
                                        break;
                                    }
                                } catch (...) {
                                }
                                PyErr_Clear();
                            }
                        } else if (PyDict_Check(constructor_pyargs)) {

                            for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                                try {
                                    static PyObject *emptyargs = PyTuple_New(0);
                                    ObjContainer<T_NoRef > cobj = ObjContainerPtrProxy<T_NoRef, false, false>( &values[i] );
                                    if ((*it)(emptyargs, constructor_pyargs, cobj)) {
                                        found = true;
                                        break;
                                    }
                                } catch (...) {
                                }
                                PyErr_Clear();
                            }
                        }
                        if (!found || !values) {
                            PyErr_SetString(PyExc_RuntimeError,
                                            "Invalid constructor arguments on allocation.  Objects not destructible by design! ");
                            if (raw_storage) {
                                for (Py_ssize_t j = 0; j < i; ++j) {
                                    values[j].~T();
                                }
                                delete[] raw_storage;
                            }
                            return nullptr;

                        }
                    }
                    PtrWrapper *obj = PtrWrapper::template createPy(size, values, !raw_storage);
                    obj->set_raw_storage(raw_storage);
                    return (PyObject *) obj;
                }
                //otherwise, just have regular list of constructor arguments
                //for single object allocation
                T *cobj = nullptr;
                for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                    try {
                        if ((*it)(args, kwds, cobj)) { break; }
                    } catch (...) {
                    }
                    PyErr_Clear();
                }
                if (!cobj) {
                    PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
                    return nullptr;
                }
                PythonClassWrapper<T_NoRef *, void> *obj =
                        PythonClassWrapper<T_NoRef *, void>::template createPy(1, cobj, true);

                return (PyObject *) obj;
            }

        };

        template<typename ReturnType, typename ...Args>
        struct Alloc<ReturnType(*)(Args...), PythonClassWrapper<ReturnType(**)(Args...)>,
                PythonClassWrapper<ReturnType(*)(Args...)>,
                void> :
                public BasicAlloc<ReturnType(*)(Args...) > {

            typedef PythonClassWrapper<ReturnType(**)(Args...), void> PtrWrapper;

            typedef ReturnType(*T)(Args...);

            typedef typename std::remove_reference<T>::type T_NoRef;

            static void deallocREMOVE(ObjContainer <T_NoRef> *ptr, const bool inPlace, const size_t size, const bool nonononon) {
                if( inPlace){
                    for(size_t i =0; i < size; ++i){
                        ptr->ptr()[i].~T();
                    }
                }
                delete ptr;
            }

            static PyObject *allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                                       typename BasicAlloc<T>::constructor_list const &constructors) {
                (void) cls;
                //Check if argument is list of tuples, and if so construct
                //an array of objects to store "behind the pointer"
                if ((!kwds || PyDict_Size(kwds) == 0) && args && PyTuple_Size(args) == 1 && PyList_Check(args)) {
                    PyObject *list = PyTuple_GetItem(args, 0);
                    const Py_ssize_t size = PyList_Size(list);
                    char *raw_storage;
                    T_NoRef *values;
                    if (size > 1) {
                        raw_storage = (char *) operator new[](size * sizeof(T));
                        memset(raw_storage, 0, size * sizeof(T));
                        values = reinterpret_cast<T_NoRef *>(raw_storage);
                    } else {
                        raw_storage = nullptr;
                        values = nullptr;
                    }
                    for (Py_ssize_t i = 0; i < PyTuple_Size(args); ++i) {
                        PyObject *constructor_pyargs = PyList_GetItem(list, i);
                        if (!PyTuple_Check(constructor_pyargs) && !PyDict_Check(constructor_pyargs)) {
                            PyErr_SetString(PyExc_TypeError, "Invalid element in list argument, expected tuple");
                            if (raw_storage) {
                                for (Py_ssize_t j = 0; j < i; ++j) {
                                    values[j].~T();
                                }
                                delete[] raw_storage;
                            }
                            return nullptr;
                        } else if (PyTuple_Check(constructor_pyargs)) {
                            for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                                try {
                                    T_NoRef *cobj = &(*values)[i];
                                    if ((*it)(constructor_pyargs, nullptr, cobj)) { break; }
                                } catch (...) {
                                }
                                PyErr_Clear();
                            }
                        } else if (PyDict_Check(constructor_pyargs)) {
                            for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                                try {
                                    static PyObject *emptyargs = PyTuple_New(0);
                                    T_NoRef *cobj = &(*values)[i];
                                    if ((*it)(emptyargs, constructor_pyargs, cobj, false)) { break; }
                                } catch (...) {
                                }
                                PyErr_Clear();
                            }

                            if (!values || !values[i]) {
                                PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
                                if (raw_storage) {
                                    for (Py_ssize_t j = 0; j < i; ++j) {
                                        values[j].~T();
                                    }
                                    delete[] raw_storage;
                                }
                                return nullptr;
                            }
                        }
                    }
                    if (!raw_storage) {
                        PyErr_SetString(PyExc_RuntimeError, "Invalid object creation");
                        return nullptr;
                    }
                    PtrWrapper *obj = PtrWrapper::createPy(size, values, !raw_storage);
                    obj->set_raw_storage(raw_storage);
                    return (PyObject *) obj;
                }

                //otherwise, just have regular list of constructor arguments
                //for single object allocation
                T *cobj = nullptr;
                for (auto it = constructors.begin(); it != constructors.end(); ++it) {
                    try {
                        if ((cobj = (*it)(args, kwds))) { break; }
                    } catch (...) {
                    }
                    PyErr_Clear();
                }
                if (!cobj) {
                    PyErr_SetString(PyExc_RuntimeError, "Invalid constructor arguments on allocation");
                    return nullptr;
                }
                PtrWrapper *obj = PythonClassWrapper<T_NoRef *, void>::template createPy<T_NoRef *>(1, cobj, true);
                obj->set_raw_storage(nullptr);
                return (PyObject *) obj;
            }
        };

        template<typename T>
        struct Alloc<T,
                PythonClassWrapper<T *, void>,
                PythonClassWrapper<T, void>,
                typename std::enable_if<std::is_void<typename std::remove_volatile<T>::type>::value>::type> :
                public BasicAlloc<T> {

            typedef PythonClassWrapper<void *, void> PtrWrapper;
            typedef typename std::remove_reference<T>::type C_NoRef;

            static void deallocREMOVE(ObjContainer <C_NoRef> *ptr) {
                (void) ptr;
            }

            static PyObject *allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                                       typename BasicAlloc<T>::constructor_list const &constructors) {
                (void) args;
                (void) kwds;
                (void) cls;
                PyErr_SetString(PyExc_RuntimeError, "Type is not directly constructible");
                return nullptr;
            }
        };


        template<typename T, typename PtrWrapper, typename ClassWrapper>
        struct Alloc<T, PtrWrapper,
                ClassWrapper,
                typename std::enable_if<!std::is_void<T>::value && !std::is_function<T>::value &&
                                        (std::is_reference<T>::value || !std::is_constructible<T>::value)>::type> :
                public BasicAlloc<T> {

            typedef typename std::remove_reference<T>::type C_NoRef;

            static void deallocREMOVE(ObjContainer <C_NoRef> *ptr) {
                (void) ptr;
            }

            static PyObject *allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                                       typename BasicAlloc<T>::constructor_list const &constructors) {
                (void) args;
                (void) kwds;
                (void) cls;
                (void) constructors;
                PyErr_SetString(PyExc_RuntimeError, "Type is not directly constructible");
                return nullptr;
            }
        };

        template<typename T, typename PtrWrapper, typename ClassWrapper>
        struct Alloc<T, PtrWrapper,
                ClassWrapper,
                typename std::enable_if<std::is_function<T>::value>::type> :
                public BasicAlloc<T> {

            typedef typename std::remove_reference<T>::type C_NoRef;

            static void deallocREMOVE(ObjContainer <C_NoRef> *ptr) {
                (void) ptr;
            }

            static PyObject *allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                                       typename BasicAlloc<T>::constructor_list const &constructors) {
                (void) args;
                (void) kwds;
                (void) cls;
                (void) constructors;
                PyErr_SetString(PyExc_RuntimeError, "Type is not directly constructible");
                return nullptr;
            }
        };

    };

    /**
     * Specialization for void-pointer type
     **/
    template<typename T, typename PtrWrapper>
    class ObjectLifecycleHelpers::ObjectContent<T *, PtrWrapper, typename std::enable_if<std::is_void<T>::value>::type> {
    public:

        static void set(const size_t index, T **const to, T **const from, const size_t depth) {
            to[index] = *from;
        }

        static PyObject *getObjectAt(T *const from, const size_t index, const ssize_t elements_array_size,
                                     const size_t depth, const bool asArgument = true) {
            (void) from;
            (void) index;
            (void) elements_array_size;
	    PyErr_SetString(PyExc_TypeError,"Attempt to get element from non-array object");
	    return nullptr;
        }

        static T **getObjectPtr(PtrWrapper *const self) {
            return (T **) &self->_CObject;
        }

    };


    template<typename T, typename PtrWrapper>
    class ObjectLifecycleHelpers::ObjectContent<T, PtrWrapper, typename std::enable_if<std::is_void<T>::value> > {
    public:

        static void set(const size_t index, void *const to, void *const from, const size_t depth) {
            throw "Attempt to access element from object  of type void";
        }

        static PyObject *getObjectAt(void *const from, const size_t index, const ssize_t elements_array_size,
                                     const size_t depth, const bool asArgument = true) {
            (void) from;
            (void) index;
            (void) elements_array_size;
	    PyErr_SetString(PyExc_TypeError,"Attempt to get element from non-array object");
	    return nullptr;
        }

        static void *getObjectPtr(PtrWrapper *const self) {
            return (void *) &self->_CObject;
        }

    };

    /**
     * Specialization for void-pointer type
     **/
    template<typename T, typename PtrWrapper>
    class ObjectLifecycleHelpers::ObjectContent<T *const, PtrWrapper, typename std::enable_if<std::is_void<T>::value>::type> {
    public:

        static void set(const size_t index, T *const *const to, T *const *const from, const size_t depth) {
            throw "Attempt to assign to const value";
        }

        static PyObject *getObjectAt(T *const from, const size_t index, const ssize_t elements_array_size,
                                     const size_t depth, const bool asArgument = true) {
            (void) from;
            (void) index;
            (void) elements_array_size;
	    PyErr_SetString(PyExc_TypeError,"Attempt to get element from non-array object");
	    return nullptr;
        }

        static T *const *getObjectPtr(PtrWrapper *const self) {
            return (T *const *) &self->_CObject;
        }

    };


}
#endif // PYLLARS_INTERNAL__OBJECTLIFECYCLEHELPERS_H
