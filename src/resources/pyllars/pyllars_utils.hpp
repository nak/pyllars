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
#include "pyllars_defns.hpp"

#undef NULL
#define NULL nullptr
/**
 * Some (perhaps odd) utility classes to aide in converting calls with python tuples
 * to full-blown argument lists used in C function calls
 **/
namespace __pyllars_internal {

    template<typename T>
    struct ptr_depth {
        static constexpr size_t value = 0;
    };
    template<typename T>
    struct ptr_depth<T&> {
        static constexpr size_t value = ptr_depth<T>::value;
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
        static constexpr ssize_t size = -1;
    };

    template<typename T>
    struct ArraySize<T *> {
        static constexpr ssize_t size = -1;
    };

    template<typename T>
    struct ArraySize<T[]> {
        static constexpr ssize_t size = -1;
    };

    template<typename T_base, size_t arsize>
    struct ArraySize<T_base[arsize]> {
        static constexpr size_t size = arsize;
    };


    template<typename T>
    struct ArraySize<T *&> {
        static constexpr ssize_t size = -1;
    };

    template<typename T>
    struct ArraySize<T(&)[]> {
        static constexpr ssize_t size = -1;
    };

    template<typename T_base, size_t arsize>
    struct ArraySize<T_base(&)[arsize]> {
        static constexpr size_t size = arsize;
    };

    template<typename T>
    struct Sizeof{
        static constexpr size_t value = Sizeof::typeSize();

    private:
        static constexpr size_t typeSize(){
            if constexpr (is_complete<T>::value && !std::is_void<T>::value){
                return sizeof(T);
            } else {
                return 0;
            }
        }
    };

    template<typename T, const size_t bits>
    struct BitFieldLimits {
        static bool is_in_bounds(const T &value) {
            if constexpr (std::is_integral<T>::value && std::is_signed<T>::value){
                static constexpr int64_t lower = -(1ull << bits);
                static constexpr int64_t upper = -lower + 1;
                return (value >= lower) && (value <= upper);
            } else if constexpr (std::is_integral<T>::value && std::is_unsigned<T>::value){
                static constexpr uint64_t lower = 0;
                static constexpr uint64_t upper = (1ull << bits) - 1;
                return (value >= lower) && (value <= upper);
            } else {
                return true;/*no meaning here, so always return true*/
            }
        }
    };

    template <typename T1, typename T2>
    inline size_t offset_of(T1 T2::*member) {
        T2 object {};
        return size_t((long long)(&(object.*member)) - (long long)(&object));
    }


    template<typename To, typename From=To>
    struct Assignment {

        static void assign(To &to, const From &from, const size_t arraySize = 0) {
            typedef typename std::remove_pointer<typename extent_as_pointer<To>::type>::type To_base;
            typedef typename std::remove_pointer<typename extent_as_pointer<From>::type>::type From_base;

            if constexpr ( !std::is_array<To>::value && std::is_assignable<To&, typename std::remove_reference<From>::type>::value) {
                to = from;
            } else if constexpr (std::is_array<To>::value && ArraySize<To>::size > 0 &&
                                 std::is_array<From>::value && ArraySize<From>::size == ArraySize<To>::size &&
                                 std::is_assignable<typename std::remove_reference<To_base>::type&, typename std::remove_reference<From_base>::type>::value){
                for (size_t i = 0; i < ArraySize<To>::size; ++i)
                    to[i] = from[i];
            }else if constexpr (std::is_array<To>::value && std::is_array<From>::value  &&
                                std::is_assignable<typename std::remove_reference<To_base>::type&, typename std::remove_reference<From_base>::type>::value){
                for (size_t i = 0; i < arraySize; ++i)
                    to[i] = from[i];
            } else {
                throw "Attempt to assign incompatible or unassignable type";
            }
        }

    };

    template<typename T>
    struct func_traits;

    template<typename RType, typename ...Args>
    struct func_traits<RType(Args...)>{
        constexpr static bool has_ellipsis = false;
        constexpr static size_t argsize = sizeof...(Args);


        typedef RType(*type)(Args...);
        typedef RType ReturnType;

        const static std::string type_name(){
            std::string n =  _Types<RType>::type_name() + std::string("(*)(");
            std::string arg_names[] = {_Types<Args>::type_name()...};
            for (int i = 0; i < sizeof...(Args); ++i){
                n += arg_names + ",";
            }
            n += std::string(")");
        }
    };

    template<typename RType, typename ...Args>
    struct func_traits<RType(Args..., ...)>{
        constexpr static bool has_ellipsis = false;
        constexpr static size_t argsize = sizeof...(Args);

        typedef RType(*type)(Args..., ...);
        typedef RType ReturnType;
        const static std::string type_name(){
            std::string n =  _Types<RType>::type_name() + std::string("(*)(");
            std::string arg_names[] = {_Types<Args>::type_name()...};
            for (int i = 0; i < sizeof...(Args); ++i){
                n += arg_names + ",";
            }
            n += std::string(" ...)");
        }

    };
}


#endif
