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
        typedef typename std::remove_reference<T>::type T_NoRef;
        static constexpr bool value =
                !std::is_arithmetic<T_NoRef>::value &&
                !std::is_floating_point<T_NoRef>::value &&
                !std::is_array<T_NoRef>::value &&
                !std::is_pointer<T_NoRef>::value;
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

    template<typename T>
    struct is_pointer_like{
        typedef typename std::remove_const<typename std::remove_reference<T>::type>::type T_base;
        static constexpr bool value =  !std::is_function<typename std::remove_pointer<T>::type>::value &&
                (std::is_pointer<T_base>::value || std::is_array<T_base>::value) ;
    };


    //convertion of type with extent to pointer
    // extent_as_pointer<T[]> = extent_as_pointer<T[size]> = T*
    template<typename T>
    struct extent_as_pointer {
        typedef T type;
    };

    template<typename T>
    struct extent_as_pointer<T *> {
        //typedef typename extent_as_pointer<T>::type *type;
        typedef T *type;
    };

    template<typename T>
    struct extent_as_pointer<T * const> {
        //typedef typename extent_as_pointer<T>::type *type;
        typedef T * const type;
    };

    template<typename T>
    struct extent_as_pointer<T[]> {
        // typedef typename extent_as_pointer<T>::type *type;
        typedef T *type;
    };

    template<typename T>
    struct extent_as_pointer<const T[]> {
        // typedef const typename extent_as_pointer<T>::type *type;
        typedef const T *type;
    };

    template<typename T, const size_t max>
    struct extent_as_pointer<T[max]> {
        // typedef typename extent_as_pointer<T>::type *type;
        typedef T *type;
    };

    template<typename T, const size_t max>
    struct extent_as_pointer<const T[max]> {
        //  typedef const typename extent_as_pointer<T>::type *type;
        typedef const T *type;
    };
    template <bool asReference, typename T>
    struct apply_const;

    template <typename T>
    struct apply_const<false, T>{
        typedef T type;
    };

    template <typename T>
    struct apply_const<true, T>{
        typedef const T type;
    };

    template <typename T>
    struct as_argument{
        static constexpr bool asArgument = std::is_reference<T>::value|| std::is_array<typename std::remove_reference<T>::type>::value;
        typedef typename apply_const<!asArgument, T>::type type;
    };


    /**
     * Sometimes we need to store a value that is a reference (e.g. the value of PythonClassWrapper<int&>,
     * which we represent internally as int* (an equivalent representation for us, and avoids explicit init needs)
     * This class helps make the representation somewhat transparent, allowing common base code to exist with minor
     * differences handled through if constexpr, etc.
     * Yes, this is quirky
     * @tparam T
     */
    template <typename T>
    struct representation{
        typedef T type;

        static inline T& value(T& obj){
            return obj;
        }

        static inline T* addr(T& obj){
            return &obj;
        }
    };

    template <typename T>
    struct representation<T&>{
        typedef T *type;

        static inline T* value(T& obj){
            return &obj;
        }

        static inline T& value(T* obj){
            return *obj;
        }

        static inline T* addr(T* obj){
            return obj;
        }
    };


}

#endif
