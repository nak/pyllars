#ifndef PYLLARS_INTERNAL__OBJECTLIFECYCLEHELPERS_H
#define PYLLARS_INTERNAL__OBJECTLIFECYCLEHELPERS_H

#include <type_traits>
#include <vector>

#include <sys/types.h>
#include <Python.h>

#include "pyllars_defns.hpp"
#include "pyllars_containment.hpp"

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
    struct ObjectContainer;

    class ObjectLifecycleHelpers {
    public:


        template<typename CClass, typename Z>
        friend
        struct PythonClassWrapper;

        template<typename T, typename Z= void>
        struct Array;

        template<typename T>
        struct Array<T, typename std::enable_if<
                is_complete<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value &&
                !std::is_void<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type> {
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

            static T_base &at(T array, size_t index);
        };

        template<typename T>
        struct Array<T, typename std::enable_if<
                !std::is_void<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value &&
                !is_complete<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type> {
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

            static T_base &at(T array, size_t index);
        };


        template<typename T>
        struct Array<T, typename std::enable_if<
                std::is_const<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value &&
                std::is_void<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type> {
            typedef const unsigned char T_base;

            static T_base &at(const void *array, size_t index) {
                return ((const unsigned char *) array)[index];
            }
        };

        template<typename T>
        struct Array<T, typename std::enable_if<
                !std::is_const<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value &&
                std::is_void<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type> {
            typedef unsigned char T_base;

            static T_base &at(void *array, size_t index) {
                return ((unsigned char *) array)[index];
            }
        };


    public:
        template<typename T, typename Z = void>
        struct Copy;

        template<typename T>
        struct Copy<T, typename std::enable_if<std::is_destructible<typename std::remove_reference<T>::type>::value &&
                                               std::is_assignable<typename std::remove_reference<T>::type &, typename std::remove_reference<T>::type>::value &&
                                               std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> {
            typedef typename std::remove_reference<T>::type T_NoRef;

            static ObjectContainer<T> *new_copy2(const T &value);

            static T_NoRef *new_copy(T_NoRef *value);

            static void inplace_copy(T_NoRef *to, Py_ssize_t index, const T_NoRef *from);
        };

        template<typename T>
        struct Copy<T, typename std::enable_if<
                (!std::is_assignable<typename std::remove_reference<T>::type &, typename std::remove_reference<T>::type>::value ||
                 !std::is_destructible<typename std::remove_reference<T>::type>::value) &&
                std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> {
            typedef typename std::remove_reference<T>::type T_NoRef;

            static ObjectContainer<T> *new_copy2(const T_NoRef &value);

            static T_NoRef *new_copy(T_NoRef *value);

            static void inplace_copy(T_NoRef *to, Py_ssize_t index, const T_NoRef *from);
        };

        template<typename T>
        struct Copy<T, typename std::enable_if<std::is_void<T>::value>::type> {
            static ObjectContainer<T> *new_copy2(T *const value) {
                throw "Attempt to copy void object";
            }

            static ObjectContainer<T> *new_copy2(const unsigned char &value) {
                throw "Attempt to copy void object";
            }

            static T *new_copy(T *const value) {
                throw "Attempt to copy void object";
            }

            static void inplace_copy(T *const to, const Py_ssize_t index, const T *const from, const bool in_place) {
                throw "Cannot copy void item";
            }
        };

        template<typename T, size_t size>
        struct Copy<T[size], typename std::enable_if<(size > 0) &&
                                                     !std::is_const<T>::value &&
                                                     std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> {
            typedef T T_array[size];
            typedef typename std::remove_reference<T>::type T_NoRef;

            static ObjectContainer<T_array> *new_copy2( T_array &value);

            static T_array *new_copy(T_array *value);

            static void inplace_copy(T_array *to, Py_ssize_t index, const T_array *from);
        };

        template<typename T, size_t size>
        struct Copy<T[size], typename std::enable_if<(size > 0) &&
                                                     std::is_const<T>::value &&
                                                     std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> {
            typedef T T_array[size];
            typedef typename std::remove_reference<T>::type T_NoRef;

            static ObjectContainer<T_array> *new_copy2(const T_array &value);

            static T_array *new_copy(T_array *value);

            static void inplace_copy(T_array *to, Py_ssize_t index, const T_array *from);
        };

        template<typename T>
        struct Copy<T, typename std::enable_if<!std::is_void<T>::value &&
                                               (!std::is_array<T>::value || ArraySize<T>::size <= 0) &&
                                               !std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> {
            typedef typename std::remove_reference<T>::type T_NoRef;
            typedef typename std::remove_pointer<T_NoRef>::type &T_baseref;

            static ObjectContainer<T> *new_copy2(const T_NoRef &value);

            static T_NoRef *new_copy(T_NoRef *value);

            static void inplace_copy(T_NoRef *to, Py_ssize_t index, const T_NoRef *from);
        };


        template<typename T, size_t size>
        struct Copy<T[size], typename std::enable_if<(size > 0) &&
                                                     !std::is_copy_constructible<typename std::remove_reference<T>::type>::value>::type> {
            typedef T T_array[size];
            typedef typename std::remove_reference<T>::type T_NoRef;
            typedef typename std::remove_pointer<T_NoRef>::type &T_baseref;

            static ObjectContainer<T_array> *
            new_copy2(const T_array &value) { throw "cannot copy oobject : is not copy construcftible"; }

            static T_array *new_copy(T_array *const value) { throw "cannot copy oobject : is not copy construcftible"; }

            static void inplace_copy(T_array *const to, const Py_ssize_t index, const T_array *const from) {
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
                for (size_t i = 0; i < size; ++i) { to[i] = from[i]; }
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

            static PyObject *getObjectAt(T from, size_t index, ssize_t elements_array_size,
                                         bool asArgument = true);

            static void set(size_t index, T *to, const T *from);

            static T *getObjectPtr(ClassWrapper *self);

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

            static PyObject *getObjectAt(T from, size_t index, ssize_t elements_array_size,
                                         bool asArgument = true);

            static void set(size_t index, T &to, T from);

            static T *getObjectPtr(PtrWrapper *self);

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

            static PyObject *getObjectAt(T from, size_t index, ssize_t elements_array_size,
                                         bool asArgument = true);

            static void set(size_t index, T to, T from);

            static T *getObjectPtr(PtrWrapper *self);

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

            static PyObject *getObjectAt(T from, size_t index, ssize_t elements_array_size,
                                         bool asArgument = true);

            static void set(size_t index, T to, T from);

            static T *getObjectPtr(PtrWrapper *self);

        };

    };


    /**
     * Specialization for void-pointer type
     **/
    template<typename T>
    class ObjectLifecycleHelpers::ObjectContent<T *, typename std::enable_if<std::is_void<T>::value>::type> {
    public:
        typedef PythonClassWrapper<T> PtrWrapper;

        static void set(size_t index, T **to, T **from);

        static PyObject *getObjectAt(T *from, size_t index, ssize_t elements_array_size,
                                     bool asArgument = true) ;

        static T **getObjectPtr(PtrWrapper *self);

    };


    template<typename T>
    class ObjectLifecycleHelpers::ObjectContent<T, typename std::enable_if<std::is_void<T>::value> > {
    public:
        typedef PythonClassWrapper<T> PtrWrapper;

        static void set(size_t index, void *to, void *from) ;

        static PyObject *getObjectAt(void *from, size_t index, ssize_t elements_array_size,
                                     bool asArgument = true);

        static void *getObjectPtr(PtrWrapper *self);

    };

    /**
     * Specialization for void-pointer type
     **/
    template<typename T>
    class ObjectLifecycleHelpers::ObjectContent<T *const, typename std::enable_if<std::is_void<T>::value>::type> {
    public:
        typedef PythonClassWrapper<T> PtrWrapper;

        static void set(size_t index, T *const *to, T *const *from);

        static PyObject *getObjectAt(T *from, size_t index, ssize_t elements_array_size,
                                     bool asArgument = true);

        static T *const *getObjectPtr(PtrWrapper *self);

    };


}
#endif // PYLLARS_INTERNAL__OBJECTLIFECYCLEHELPERS_H
