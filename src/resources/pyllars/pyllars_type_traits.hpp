#ifndef __PYLLARS__TYPE_TRAITS__
#define __PYLLARS__TYPE_TRAITS__

#include <type_traits>

namespace __pyllars_internal {
    template<typename T>
    struct is_bool{
        static constexpr bool value = false;
    };

    template<>
    struct is_bool<bool>{
        static constexpr bool value = true;
    };


    template<typename T>
    struct core_type{
        typedef typename std::remove_const<typename std::remove_volatile
                <typename std::remove_reference<T>::type>::type>::type type;
    };

    template<typename T>
    struct non_const_pointer{
        typedef T type;
    };

    template<typename T>
    struct non_const_pointer<const T*>{
        typedef T* type;
    };


    template<typename T>
    struct is_unbounded_array{
        static constexpr bool value = false;
    };

    template<typename T, size_t size>
    struct is_unbounded_array<T[size]>{
        static constexpr bool value = false;
    };

    template<typename T>
    struct is_unbounded_array<T*>{
        static constexpr bool value = true;
    };

    template<typename T>
    struct is_unbounded_array<T* const>{
        static constexpr bool value = true;
    };


    template<typename T>
    struct is_rich_class {
        static constexpr bool value =
                !std::is_arithmetic<T>::value &&
                !std::is_floating_point<T>::value &&
                !std::is_array<T>::value &&
                !std::is_pointer<T>::value;
    };

    template<typename T>
    struct is_c_string_like{
        static constexpr bool value = false;
    };

    template<>
    struct is_c_string_like<const char*>{
        static constexpr bool value = true;
    };

    template<>
    struct is_c_string_like<const char* const>{
        static constexpr bool value = true;
    };

    template<typename T>
    struct is_bytes_like{
        static constexpr bool value = false;
    };

    template<>
    struct is_bytes_like<char*>{
        static constexpr bool value = true;
    };

    template<>
    struct is_bytes_like<char* const>{
        static constexpr bool value = true;
    };
}

#endif
