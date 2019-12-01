#ifndef __PYLLARS__TYPE_TRAITS__
#define __PYLLARS__TYPE_TRAITS__

#include <type_traits>
#include <functional>
#include <stddef.h>

namespace pyllars_internal {
    template<typename T>
    struct is_bool{
        static constexpr bool value = false;
    };

    template<>
    struct is_bool<bool>{
        static constexpr bool value = true;
    };


    template<typename E>
    using is_scoped_enum = std::integral_constant<
            bool,
            std::is_enum<E>::value && !std::is_convertible<E, int>::value>;

    template<typename T>
    struct core_type{
        typedef typename std::remove_const_t<std::remove_cv_t<T> > type;
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


    template <typename Base, typename Class>
    struct ApplyCv{
        typedef Class type;
    };

    template <typename Base, typename Class>
    struct ApplyCv<const Base, Class>{
        typedef const Class type;
    };

    template <typename Base, typename Class>
    struct ApplyCv<volatile Base, Class>{
        typedef volatile Class type;
    };

    template <typename Base, typename Class>
    struct ApplyCv<const volatile Base, Class>{
        typedef const volatile Class type;
    };

    template<typename T>
    struct is_rich_class {
        typedef typename std::remove_reference<T>::type T_NoRef;
        static constexpr bool value =
                !std::is_arithmetic<T_NoRef>::value &&
                !std::is_floating_point<T_NoRef>::value &&
                !std::is_array<T_NoRef>::value &&
                !std::is_pointer<T_NoRef>::value &&
                !std::is_reference<T>::value &&
                !std::is_function<T_NoRef>::value;
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
        typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type T_base;
        static constexpr bool value = !std::is_reference<T>::value &&
                !std::is_function<typename std::remove_pointer<T>::type>::value &&
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
    struct extent_as_pointer<T * const volatile> {
        //typedef typename extent_as_pointer<T>::type *type;
        typedef T * const volatile type;
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

    template<typename T>
    struct extent_as_pointer<const volatile T[]> {
        // typedef const typename extent_as_pointer<T>::type *type;
        typedef const volatile T *type;
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

    template<typename T, const size_t max>
    struct extent_as_pointer<const volatile T[max]> {
        //  typedef const typename extent_as_pointer<T>::type *type;
        typedef const volatile T *type;
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

    template <typename T>
    struct to_const{
        typedef const T type;
    };

    template <typename T>
    struct to_const<T&>{
        typedef const T& type;
    };
    namespace check {
        struct No {
        };

        template<typename X, typename Y>
        No operator==(const X &, const Y &);

        template<typename X, typename Y=X>
        struct ComparisonExists {
            typedef std::remove_volatile_t <X> X_bare;
            typedef std::remove_volatile_t <Y> Y_bare;
            typedef decltype(std::declval<X_bare>() == std::declval<Y_bare>()) Candidate;
            static constexpr bool value =
            std::is_constructible<std::remove_reference_t <X> >::value &&
            std::is_constructible<std::remove_reference_t <Y> >::value &&
            !std::is_same< Candidate, No>::value ;
        };
    }
    template<typename X, typename Y> using has_operator_compare = typename check::ComparisonExists<X, Y>;

}

#endif
