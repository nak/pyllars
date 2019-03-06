#ifndef PYLLARS_INTERNAL__OBJECTLIFECYCLEHELPERS_H
#define PYLLARS_INTERNAL__OBJECTLIFECYCLEHELPERS_H

#include <type_traits>
#include <vector>

#include <sys/types.h>
#include <Python.h>

#include "pyllars_defns.hpp"
//#include "pyllars_pointer.hpp"
//#include "pyllars_classwrapper.hpp"

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

        template<typename T, bool is_array, typename E>
        friend
        class CObjectConversionHelper;

        template<typename T, typename PtrWrapper, typename E>
        friend
        class PyObjectConversionHelper;

        template<typename T>
        friend
        struct ObjConatinerPtrProxy;


    private:

        template<typename T, typename Z= void>
        struct Array;

        template<typename T>
        struct Array<T, typename std::enable_if<is_complete<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value&&
                                                !std::is_void <typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type> {
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

            static T_base &at(T array, size_t index);
        };

        template<typename T>
        struct Array<T, typename std::enable_if<!std::is_void<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value &&
                                                !is_complete<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type> {
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

            static T_base &at(T array, size_t index);
        };


        template<typename T>
        struct Array<T, typename std::enable_if<std::is_const<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value &&
                                                std::is_void <typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type> {
            typedef const unsigned char T_base;

            static T_base &at(const void* array, size_t index){
                return ((const unsigned char*)array)[index];
            }
        };

        template<typename T>
        struct Array<T, typename std::enable_if<!std::is_const<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value &&
                                                std::is_void <typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value >::type> {
            typedef unsigned char T_base;

            static T_base &at(void* array, size_t index){
                return ((unsigned char*)array)[index];
            }
        };


    public:
        template<typename T, typename Z = void>
        struct Copy;

        template<typename T>
        struct Copy<T, typename std::enable_if<std::is_destructible<typename std::remove_reference<T>::type>::value &&
                                               std::is_assignable<typename std::remove_reference<T>::type&, typename std::remove_reference<T>::type>::value &&
                                               std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> {
            typedef typename std::remove_reference<T>::type T_NoRef;
            static ObjContainer <T_NoRef> *new_copy2(const T &value) ;

            static T_NoRef *new_copy(T_NoRef *const value);

            static void inplace_copy(T_NoRef *const to, const Py_ssize_t index, const T_NoRef *const from);
        };

        template<typename T>
        struct Copy<T, typename std::enable_if<
                (!std::is_assignable<typename std::remove_reference<T>::type&, typename std::remove_reference<T>::type>::value ||
                 !std::is_destructible<typename std::remove_reference<T>::type>::value) &&
                std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> {
            typedef typename std::remove_reference<T>::type T_NoRef;

            static ObjContainer <T_NoRef> *new_copy2(const T_NoRef &value) ;

            static T_NoRef *new_copy(T_NoRef *const value) ;

            static void inplace_copy(T_NoRef *const to, const Py_ssize_t index, const T_NoRef *const from);
        };

        template<typename T>
        struct Copy<T, typename std::enable_if<std::is_void<T>::value >::type> {
             static ObjContainer<T> *new_copy2(T *const value) {
                throw "Attempt to copy void object";
            }

            static ObjContainer<T> *new_copy2(const unsigned char& value) {
                throw "Attempt to copy void object";
            }

            static T*new_copy(T * const value) {
                throw "Attempt to copy void object";
            }

            static void inplace_copy(T *const to, const Py_ssize_t index, const T *const from, const bool in_place) {
                throw "Cannot copy void item";
            }
        };

        template<typename T, size_t size>
        struct Copy<T[size], typename std::enable_if<(size > 0) &&
                                                      !std::is_const<T>::value &&
                                                      std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type > {
            typedef T T_array[size];
            typedef typename std::remove_reference<T>::type T_NoRef;

            static ObjContainer <T_array> *new_copy2(const T_array &value) ;

            static T_array *new_copy(T_array *const value);

            static void inplace_copy(T_array *const to, const Py_ssize_t index, const T_array *const from);
        };

        template<typename T, size_t size>
        struct Copy<T[size], typename std::enable_if<(size > 0) &&
                                                      std::is_const<T>::value &&
                                                      std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type > {
            typedef T T_array[size];
            typedef typename std::remove_reference<T>::type T_NoRef;

            static ObjContainer <T_array> *new_copy2(const T_array &value) ;

            static T_array *new_copy(T_array *const value);

            static void inplace_copy(T_array *const to, const Py_ssize_t index, const T_array *const from);
        };

        template<typename T>
        struct Copy<T, typename std::enable_if<!std::is_void<T>::value &&
                                               (!std::is_array<T>::value || ArraySize<T>::size <= 0) &&
                                               !std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> {
            typedef typename std::remove_reference<T>::type T_NoRef;
            typedef typename std::remove_pointer<T_NoRef>::type& T_baseref;

            static ObjContainer <T_NoRef> *new_copy2(const T_NoRef &value);

            static T_NoRef *new_copy(T_NoRef *const value);

            static void inplace_copy(T_NoRef *const to, const Py_ssize_t index, const T_NoRef *const from);
        };


        template<typename T, size_t size>
        struct Copy<T[size], typename std::enable_if<(size > 0) &&
                                                     !std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type > {
            typedef T T_array[size];
            typedef typename std::remove_reference<T>::type T_NoRef;
            typedef typename std::remove_pointer<T_NoRef>::type& T_baseref;

            static ObjContainer <T_array> *new_copy2(const T_array &value) { throw "cannot copy oobject : is not copy construcftible";}

            static T_array* new_copy(T_array *const value){ throw "cannot copy oobject : is not copy construcftible";}

            static void inplace_copy(T_array *const to, const Py_ssize_t index, const T_array *const from){
                throw "cannot copy oobject : is not copy construcftible";
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
                for (size_t i = 0; i < size; ++i){to[i] = from[i];}
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
        // POINTER DEREFERENCING AND ARRAY INDEXING SET/GET HELPERS
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
        template<typename T, typename Z = void>
        class ObjectContent;

        /**
         * Specialization for non-pointer copiable and assignable class (instances)
         **/
        template<typename T>
        class ObjectContent<T,
                typename std::enable_if<
                        !std::is_void<T>::value && !std::is_pointer<T>::value && !std::is_array<T>::value>::type> {
        public:
            typedef PythonClassWrapper<T> ClassWrapper;
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

            static PyObject *getObjectAt(T const from, const size_t index, const ssize_t elements_array_size,
                                         const bool asArgument = true);

            static void set(const size_t index, T *const to, const T *const from);

            static T *getObjectPtr(ClassWrapper *const self);

        };

        /**
         * Specialization for non-const copy-constructible non-ptr-to-void non-const pointer types
         **/
        template<typename T>
        class ObjectContent<T,
                typename std::enable_if<!std::is_function<typename std::remove_pointer<T>::type>::value &&
                                        !std::is_void<typename std::remove_pointer<T>::type>::value &&
                                        is_complete<typename std::remove_pointer<T>::type>::value &&
                                        (std::is_array<T>::value || std::is_pointer<T>::value)>::type> {
        public:
            typedef PythonClassWrapper<T> PtrWrapper;
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

            static PyObject *getObjectAt(T from, const size_t index, const ssize_t elements_array_size,
                                         const bool asArgument = true);

            static void set(const size_t index, T &to, T const from);

            static T *getObjectPtr(PtrWrapper *const self);

        };

        /**
         * Specialization for function types
         **/
        template<typename T>
        class ObjectContent<T,
                typename std::enable_if<std::is_function<typename std::remove_pointer<T>::type>::value>::type> {
        public:
            typedef PythonClassWrapper<T> PtrWrapper;
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

            static PyObject *getObjectAt(T const from, const size_t index, const ssize_t elements_array_size,
                                        const bool asArgument = true);

            static void set(const size_t index, T const to, T const from);

            static T *getObjectPtr(PtrWrapper *const self);

        };

        /*
        * Specialization for non-const copy-constructible non-ptr-to-void non-const pointer types
        **/
        template<typename T>
        class ObjectContent<T,
                typename std::enable_if<!is_complete<typename std::remove_pointer<T>::type>::value &&
                                        !std::is_function<typename std::remove_pointer<T>::type>::value &&
                                        !std::is_void<typename std::remove_pointer<T>::type>::value>::type> {
        public:
            typedef PythonClassWrapper<T> PtrWrapper;
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

            static PyObject *getObjectAt(T const from, const size_t index, const ssize_t elements_array_size,
                                         const bool asArgument = true);

            static void set(const size_t index, T const to, T const from);

            static T *getObjectPtr(PtrWrapper *const self);

        };

        ///////////////////////////////
        // DEALLOCATION LOGIC FOR VARIOUS TYPES
        ////////////////////////////////

        /**
         * Basic deallocation is to assume no special deconstruction is needed, just
         * basic cleanup
         **/
        template<typename T>
        struct BasicDeallocation {
            typedef PythonClassWrapper<T> PtrWrapper;
            static void _free(T obj, const bool as_array);
        };

        template<typename Tptr, typename E = void>
        struct Deallocation;

        /**
         * If core type is not and array and is destructible, call in-place destructor
         * if needed in addition to basic clean-up
         **/
        template<typename T>
        struct Deallocation<T, typename std::enable_if<
                (std::is_pointer<T>::value || std::is_array<T>::value) &&
                std::is_destructible<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value
        >::type> {
            typedef PythonClassWrapper<T> PtrWrapper;
            static void _free(T obj, const bool as_array);

        };


        /**
         * If core type is not and array and is destructible, call in-place destructor
         * if needed in addition to basic clean-up
         **/
        template<typename T>
        struct Deallocation<T, typename std::enable_if<
                !std::is_destructible<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type> {
            typedef PythonClassWrapper<T> PtrWrapper;
            static void _free(T obj, const bool as_array);
        };


        ///////////////////////////////
        // ALLOCATION LOGIC FOR VARIOUS TYPES
        ////////////////////////////////

        template<typename T>
        struct BasicAlloc {
            typedef typename std::remove_reference<T>::type T_NoRef;
            typedef PythonClassWrapper<T_NoRef *> PtrWrapper;

            class ConstructorContainer {
            public:
                typedef T_NoRef* (*constructor)(const char *const kwlist[], PyObject *args, PyObject *kwds, const bool in_place);

                ConstructorContainer(const char *const kwlist[],  constructor c);

                T_NoRef* operator()(PyObject *args, PyObject *kwds,  const bool in_place) const ;

            private:
                const char *const *const _kwlist;
                const constructor _constructor;
            };

            typedef std::vector<ConstructorContainer> constructor_list;

            static PyObject *allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                                       typename BasicAlloc<T>::constructor_list const &constructors);
        };


        template<typename T, typename Z = void>
        struct Alloc;

        template<typename T>
        struct Alloc<T,
                     typename std::enable_if<!is_function_ptr<T>::value && !std::is_reference<T>::value &&
                             std::is_constructible<T>::value &&
                             !std::is_pointer<T>::value>::type> : public BasicAlloc<T> {
            typedef PythonClassWrapper<T*> PtrWrapper;
            typedef typename std::remove_reference<T>::type T_NoRef;

            static PyObject *allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                                       typename BasicAlloc<T>::constructor_list const &constructors);
        };

        template<typename T>
        struct Alloc<T,
                typename std::enable_if<!is_function_ptr<T>::value && !std::is_reference<T>::value &&
                                        std::is_constructible<T>::value &&
                                        std::is_pointer<T>::value>::type > : public BasicAlloc<T> {
            typedef PythonClassWrapper<T*> PtrWrapper;
            typedef typename std::remove_reference<T>::type T_NoRef;

            static PyObject *allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                                       typename BasicAlloc<T>::constructor_list const &constructors);
        };

        template<typename T>
        struct Alloc<T,
                     typename std::enable_if<!is_function_ptr<T>::value && !std::is_reference<T>::value &&
                                             !std::is_constructible<T>::value >::type> : public BasicAlloc<T> {
            typedef PythonClassWrapper<T*> PtrWrapper;
            typedef typename std::remove_reference<T>::type T_NoRef;

            static PyObject *allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                                       typename BasicAlloc<T>::constructor_list const &constructors);
        };

        template<typename ReturnType, typename ...Args>
        struct Alloc<ReturnType(*)(Args...), void> :
                public BasicAlloc<ReturnType(*)(Args...)> {

            typedef PythonClassWrapper<ReturnType(**)(Args...), void> PtrWrapper;

            typedef ReturnType(*T)(Args...);

            typedef typename std::remove_reference<T>::type T_NoRef;

            static PyObject *allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                                       typename BasicAlloc<T>::constructor_list const &constructors);

        };

        template<typename T>
        struct Alloc<T,
                typename std::enable_if<std::is_void<typename std::remove_volatile<T>::type>::value>::type> :
                public BasicAlloc<T> {

            typedef PythonClassWrapper<void *, void> PtrWrapper;
            typedef typename std::remove_reference<T>::type C_NoRef;

            static PyObject *allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                                       typename BasicAlloc<T>::constructor_list const &constructors);

        };


        template<typename T>
        struct Alloc<T,
                typename std::enable_if<!std::is_void<T>::value && !std::is_function<T>::value &&
                                        (std::is_reference<T>::value || !std::is_constructible<T>::value)>::type> :
                public BasicAlloc<T> {
            typedef PythonClassWrapper<T*> PtrWrapper;
            typedef typename std::remove_reference<T>::type C_NoRef;

            static PyObject *allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                                       typename BasicAlloc<T>::constructor_list const &constructors);

        };

        template<typename T>
        struct Alloc<T,
                typename std::enable_if<std::is_function<T>::value>::type> :
                public BasicAlloc<T> {
            typedef PythonClassWrapper<T*> PtrWrapper;
            typedef typename std::remove_reference<T>::type C_NoRef;

            static PyObject *allocbase(PyObject *cls, PyObject *args, PyObject *kwds,
                                       typename BasicAlloc<T>::constructor_list const &constructors);

        };

    };

    /**
     * Specialization for void-pointer type
     **/
    template<typename T>
    class ObjectLifecycleHelpers::ObjectContent<T *, typename std::enable_if<std::is_void<T>::value>::type> {
    public:
        typedef PythonClassWrapper<T> PtrWrapper;

        static void set(const size_t index, T **const to, T **const from);

        static PyObject *getObjectAt(T *const from, const size_t index, const ssize_t elements_array_size,
                                     const bool asArgument = true) ;

        static T **getObjectPtr(PtrWrapper *const self);

    };


    template<typename T>
    class ObjectLifecycleHelpers::ObjectContent<T, typename std::enable_if<std::is_void<T>::value> > {
    public:
        typedef PythonClassWrapper<T> PtrWrapper;

        static void set(const size_t index, void *const to, void *const from) ;

        static PyObject *getObjectAt(void *const from, const size_t index, const ssize_t elements_array_size,
                                     const bool asArgument = true);

        static void *getObjectPtr(PtrWrapper *const self);

    };

    /**
     * Specialization for void-pointer type
     **/
    template<typename T>
    class ObjectLifecycleHelpers::ObjectContent<T *const, typename std::enable_if<std::is_void<T>::value>::type> {
    public:
        typedef PythonClassWrapper<T> PtrWrapper;

        static void set(const size_t index, T *const *const to, T *const *const from);

        static PyObject *getObjectAt(T *const from, const size_t index, const ssize_t elements_array_size,
                                     const bool asArgument = true);

        static T *const *getObjectPtr(PtrWrapper *const self);

    };


}
#endif // PYLLARS_INTERNAL__OBJECTLIFECYCLEHELPERS_H
