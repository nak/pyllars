#ifndef __PYLLARS__TYPE_TRAITS__
#define __PYLLARS__TYPE_TRAITS__

#include <type_traits>

namespace __pyllars_internal {
    template<typename T>
    struct core_type{
        typedef typename std::remove_const<typename std::remove_volatile
                <typename std::remove_reference<T>::type>::type>::type type;
    };

    template<typename T>
    struct is_rich_class {
        static constexpr bool value =
                !std::is_arithmetic<T>::value &&
                !std::is_floating_point<T>::value &&
                !std::is_array<T>::value &&
                !std::is_pointer<T>::value;
    };
}

#endif
