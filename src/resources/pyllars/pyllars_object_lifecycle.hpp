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

        template<typename T>
        struct Array {
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

            static T_base &at(T array, size_t index);
        };

        template<typename T>
        struct Copy {
            typedef typename std::remove_reference<T>::type T_NoRef;

            static ObjectContainer<T> *new_copy2(const T &value);

            static T_NoRef *new_copy(T_NoRef *value);

            static void inplace_copy(T_NoRef *to, Py_ssize_t index, const T_NoRef *from);
        };


    private:

        template<typename To, typename From>
        struct Assign {

            static void assign(To &to, const Py_ssize_t index, const From &from, const size_t arraySize = 0) {
                typedef typename std::remove_pointer<typename extent_as_pointer<To>::type>::type To_base;
                typedef typename std::remove_pointer<typename extent_as_pointer<From>::type>::type From_base;

                if constexpr ( !std::is_array<To>::value && std::is_assignable<typename std::remove_reference<To>::type,
                        typename std::remove_reference<From>::type>::value) {
                    to = from;
                } else if constexpr (std::is_array<To>::value && ArraySize<To>::size > 0 &&
                                     std::is_array<From>::value && ArraySize<From>::size == ArraySize<To>::size &&
                                     std::is_assignable<typename std::remove_reference<To_base>::type, typename std::remove_reference<From_base>::type>::value){
                    for (size_t i = 0; i < ArraySize<To>::size; ++i)
                        to[i] = from[i];
                }else if constexpr (std::is_array<To>::value && std::is_array<From>::value  &&
                                    std::is_assignable<typename std::remove_reference<To_base>::type, typename std::remove_reference<From_base>::type>::value){
                    for (size_t i = 0; i < arraySize; ++i)
                        to[i] = from[i];
                } else {
                    throw "Attempt to assign incompatible or unassinable type";
                }
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
        template<typename T>
        class ObjectContent {
        public:
            typedef PythonClassWrapper<T> ClassWrapper;
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

            static PyObject *getObjectAt(T from, size_t index, ssize_t elements_array_size,
                                         bool asArgument = true);

            static void set(size_t index, T *to, const T *from);

            static T *getObjectPtr(ClassWrapper *self);

        };

    };

}
#endif