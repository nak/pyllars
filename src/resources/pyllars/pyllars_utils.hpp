#ifndef __PYLLARS__INTERNAL__UTILS_H
#define __PYLLARS__INTERNAL__UTILS_H

#if PY_MAJOR_VERSION == 3
#define PyString_Check PyUnicode_Check
#define PyString_AsString(X) PyUnicode_AsUTF8((X))
#define PyInt_Check PyLong_Check
#define PyInt_FromLong PyLong_FromLong
#define PyInt_AsLong PyLong_AsLong
#define PyInt_Type PyLong_Type
#define PyString_FromString PyUnicode_FromString

#define Py_TPFLAGS_CHECKTYPES 0
#endif

#include <structmember.h>
#include <functional>
#include <memory>
#include <limits>
#include "pyllars_type_traits.hpp"

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

    template<typename T>
    struct ptr_depth {
        static constexpr size_t value = 0;
    };

    template<typename T>
    struct ptr_depth<T *> {
        static constexpr size_t value = ptr_depth<T>::value + 1;
    };

    template<typename T>
    struct ptr_depth<T *const> {
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

    template<>
    struct is_complete<void> {
        enum {
            value = 1
        };
    };

    template<typename T>
    struct ArraySize {
        static constexpr int size = 1;
    };

    template<typename T>
    struct ArraySize<T *> {
        static constexpr int size = -1;
    };

    template<typename T>
    struct ArraySize<T[]> {
        static constexpr int size = -1;
    };

    template<typename T_base, size_t arsize>
    struct ArraySize<T_base[arsize]> {
        static constexpr int size = arsize;
    };

    template<typename T, typename Z = void>
    struct Sizeof;

    template<typename T>
    struct Sizeof<T, typename std::enable_if<is_complete<T>::value && !std::is_void<T>::value>::type> {
        static constexpr size_t value = sizeof(T);
    };

    template<>
    struct Sizeof<void, void> {
        static constexpr size_t value = 0;
    };

    template<typename T>
    struct Sizeof<T, typename std::enable_if<!is_complete<T>::value>::type> {
        static constexpr size_t value = 0;
    };

    template<typename T, const size_t bits, typename E = void>
    struct BitFieldLimits;

    template<typename T, const size_t bits>
    struct BitFieldLimits<T, bits, typename std::enable_if<
            std::numeric_limits<T>::is_signed && std::is_integral<T>::value>::type> {
        static constexpr int64_t lower = -(1ull << bits);
        static constexpr int64_t upper = -lower + 1;

        static bool is_in_bounds(const T &value) { return (value >= lower) && (value <= upper); }
    };

    template<typename T, const size_t bits>
    struct BitFieldLimits<T, bits, typename std::enable_if<
            !std::numeric_limits<T>::is_signed && std::is_integral<T>::value>::type> {
        static constexpr uint64_t lower = 0;
        static constexpr uint64_t upper = (1ull << bits) - 1;

        static bool is_in_bounds(const T &value) { return (value >= lower) && (value <= upper); }
    };

    template<typename T, const size_t bits>
    struct BitFieldLimits<T, bits, typename std::enable_if<!std::is_integral<T>::value>::type> {
        static bool is_in_bounds(const T &value) { return true;/*no meaning here, so always return true*/}
    };
}


namespace __pyllars_internal {

    template <typename T1, typename T2>
    inline size_t offset_of(T1 T2::*member) {
        T2 object {};
        return size_t((long long)(&(object.*member)) - (long long)(&object));
    }

    template<typename T, typename Z=void>
    class Assign{
    public:

    };

    template<typename T>
    class Assign< T, typename std::enable_if< std::is_copy_assignable<T>::value >::type >{
    public:
        static T& assign(T& v1, const T&v2){
            return v1 = v2;
        }
    };

    template<typename T>
    class Assign< T, typename std::enable_if< !std::is_copy_assignable<T>::value >::type >{
    public:
        static T& assign(T& v1, const T&v2){
            throw "Unable to assign new value; type is not copy-assignable";
        }
    };

    template<typename T, typename Z=void>
    class AssignValue{
    public:

    };

    template<typename T>
    class AssignValue< T, typename std::enable_if< std::is_assignable<T, T>::value >::type >{
    public:
        static T assign(T &v1, const T& v2){
            v1 = v2;
            return v1;
        }
    };

    template<typename T>
    class AssignValue< T, typename std::enable_if< !std::is_assignable<T, T>::value >::type >{
    public:
        static T assign(T& v1, const T& v2){
            throw "Unable to assign new value; type is not copy-assignable";
        }
    };

}


#endif
