#ifndef __PYLLARS__INTERNAL__UTILS_H
#define __PYLLARS__INTERNAL__UTILS_H

#include <structmember.h>
#include <memory>

#include "pyllars_pointer.hpp"

#undef NULL
#define NULL nullptr
/**
 * Some (perhaps odd) utility classes to aide in converting calls with python tuples
 * to full-blown argument lists used in C function calls
 **/
namespace __pyllars_internal {

    //type trait to determine if types is a function pointer
    template<typename T>
    struct is_function_ptr {
        static constexpr bool value = false;
    };


    template<typename T, typename...Args>
    struct is_function_ptr<T(*)(Args...)> {
        static constexpr bool value = true;
    };
    template<typename T, typename...Args>
    struct is_function_ptr<T(*const)(Args...)> {
        static constexpr bool value = true;
    };

    //type trait to determine depth of pointer
    // ptr_depth<int> = 0
    // ptr_depth<int*> = ptr_depth<int[]> = ptr_depth<int[size]> = 1
    // ptr_depth<int**> = 2
    // ...
    template<typename T>
    struct ptr_depth {
        static constexpr size_t value = 0;
    };

    template<typename T>
    struct ptr_depth<T *> {
        static constexpr size_t value = ptr_depth<T>::value + 1;
    };

    template<typename T, size_t size>
    struct ptr_depth<T[size]> {
        static constexpr size_t value = ptr_depth<T>::value + 1;
    };

    template<typename T, size_t depth>
    struct ptr_of_depth {
        typedef typename ptr_of_depth<T, depth - 1>::type *type;
    };

    template<typename T>
    struct ptr_of_depth<T, 0> {
        typedef T type;
    };

    template<typename T, size_t depth, size_t size>
    struct ptr_of_depth<T[size], depth> {
        typedef typename ptr_of_depth<T, depth - 1>::type type[size];
    };


    //convertion of type with extent to pointer
    // extent_as_pointer<T[]> = extent_as_pointer<T[size]> = T*
    template<typename T>
    struct extent_as_pointer {
        typedef T type;
    };

    template<typename T>
    struct extent_as_pointer<T *> {
        typedef typename extent_as_pointer<T>::type *type;
    };

    template<typename T>
    struct extent_as_pointer<T[]> {
        typedef typename extent_as_pointer<T>::type *type;
    };
    template<typename T>
    struct extent_as_pointer<const T[]> {
        typedef const typename extent_as_pointer<T>::type *type;
    };

    template<typename T, const size_t max>
    struct extent_as_pointer<T[max]> {
        typedef typename extent_as_pointer<T>::type *type;
    };

    template<typename T, const size_t max>
    struct extent_as_pointer<const T[max]> {
        typedef const typename extent_as_pointer<T>::type *type;
    };

    /**
     * container class for holding a tuple, to be expanded to an argument list
    **/
    template<int ...S>
    struct container {
    };

    /**
     * Counter class to expand arguments from a tuple object
     * to a set of arguments passed to a function
     **/
    template<int N, int ...S>
    struct argGenerator : argGenerator<N - 1, N - 1, S...> {

    };

    /**
     * 0th order specialization
    **/
    template<int ...S>
    struct argGenerator<0, S...> {
        typedef container<S...> type;

    };


    /**
     *  Types used to define a smart pointer that knows if contained
     *  element is dynamically allocated and should be deleted (or not)
     */
    template<typename T, bool is_array = false>
    struct smart_delete {

        smart_delete(const bool deleteable) : _deleteable(deleteable) {
        }

        void operator()(T *ptr) const {
            if (_deleteable) delete ptr;
        }

        const bool _deleteable;
    };

    template<typename T>
    struct smart_delete<T &, false> {

        smart_delete(const bool deleteable) : _deleteable(deleteable) {
        }

        void operator()(T *ptr) const {
            if (_deleteable) delete ptr;
        }

        const bool _deleteable;
    };

    template<typename T>
    struct smart_delete<T, true> {

        smart_delete(const bool deleteable) : _deleteable(deleteable) {
        }

        void operator()(T *ptr) const {
            if (_deleteable) delete[] ptr;
        }

        const bool _deleteable;
    };

    template<typename T, bool is_array = false>
    using smart_ptr = std::unique_ptr<typename std::remove_reference<T>::type, smart_delete<T, is_array> >;


    /**
     *  HACK(?) to determine if type is complete or not
     */
    struct char256 {
        char x[256];
    };


    template<typename T>
    char256 is_complete_helper(int(*)[sizeof(T)]);

    template<typename>
    char is_complete_helper(...);

    template<typename T>
    struct is_complete {
        enum {
            value = sizeof(is_complete_helper<T>(0)) != 1
        };
    };
}

#endif
