//
// Created by jrusnak on 11/17/19.
//

#ifndef PYLLARS_PYLLARS_FUNTTRAITS_HPP
#define PYLLARS_PYLLARS_FUNTTRAITS_HPP
namespace __pyllars_internal{

    template<typename T>
    struct func_traits;

    template<typename RType, typename ...Args>
    struct func_traits<RType(*)(Args...)>{
        constexpr static bool has_ellipsis = false;
        constexpr static ssize_t argsize = sizeof...(Args);


        typedef RType(*type)(Args...);
        typedef RType ReturnType;

        const static std::string type_name(){
            return __pyllars_internal::type_name<RType(Args...)>();
        }

        template<type func>
        static RType call(Args ...args){
            if constexpr (std::is_void<RType>::value){
                func(args...);
            }else {
                return func(args...);
            }
        }

    };
    template<typename RType, typename ...Args>
    struct func_traits<RType(Args...)>{
        constexpr static bool has_ellipsis = false;
        constexpr static ssize_t argsize = sizeof...(Args);


        typedef RType(*type)(Args...);
        typedef RType ReturnType;

        const static std::string type_name(){
            return __pyllars_internal::type_name<RType(Args...)>();
        }

    };

    template<typename RType, typename ...Args>
    struct func_traits<RType(Args...) noexcept>{
        constexpr static bool has_ellipsis = false;
        constexpr static ssize_t argsize = sizeof...(Args);


        typedef RType(*type)(Args...) noexcept;
        typedef RType ReturnType;

        const static std::string type_name(){
            return __pyllars_internal::type_name<RType(Args...)>();
        }

    };

    template<typename RType, typename ...Args>
    struct func_traits<RType(*)(Args...) noexcept>{
        constexpr static bool has_ellipsis = false;
        constexpr static ssize_t argsize = sizeof...(Args);


        typedef RType(*type)(Args...) noexcept;
        typedef RType ReturnType;

        const static std::string type_name(){
            return __pyllars_internal::type_name<RType(Args...)>();
        }

        template<type func>
        static RType call(Args ...args) noexcept{
            if constexpr (std::is_void<RType>::value){
                func(args...);
            }else {
                return func(args...);
            }
        }
    };

    template<typename RType, typename ...Args>
    struct func_traits<RType(Args..., ...)>{
        constexpr static bool has_ellipsis = true;
        constexpr static ssize_t argsize = sizeof...(Args);

        typedef RType(*type)(Args..., ...);
        typedef RType ReturnType;
        const static std::string type_name(){
            return __pyllars_internal::type_name<RType(Args..., ...)>();
        }

    };

    template<typename RType, typename ...Args>
    struct func_traits<RType(*)(Args..., ...)>{
        constexpr static bool has_ellipsis = true;
        constexpr static ssize_t argsize = sizeof...(Args);

        typedef RType(*type)(Args..., ...);
        typedef RType ReturnType;
        const static std::string type_name(){
            return __pyllars_internal::type_name<RType(Args..., ...)>();
        }

        template<type func, typename ...VarArgs>
        static RType call(Args ...args, VarArgs ...varargs){
            if constexpr (std::is_void<RType>::value){
                func(args..., varargs...);
            }else {
                return func(args..., varargs...);
            }
        }
    };

    template<typename RType, typename ...Args>
    struct func_traits<RType(Args..., ...) noexcept>{
        constexpr static bool has_ellipsis = true;
        constexpr static ssize_t argsize = sizeof...(Args);

        typedef RType(*type)(Args..., ...) noexcept;
        typedef RType ReturnType;
        const static std::string type_name(){
            return __pyllars_internal::type_name<RType(Args..., ...)>();
        }

    };

    template<typename RType, typename ...Args>
    struct func_traits<RType(*)(Args..., ...) noexcept>{
        constexpr static bool has_ellipsis = true;
        constexpr static ssize_t argsize = sizeof...(Args);

        typedef RType(*type)(Args..., ...) noexcept;
        typedef RType ReturnType;
        const static std::string type_name(){
            return __pyllars_internal::type_name<RType(Args..., ...)>();
        }

        template<type func, typename ...VarArgs>
        static RType call(Args ...args, VarArgs ...varargs) noexcept{
            if constexpr (std::is_void<RType>::value){
                func(args..., varargs...);
            }else {
                return func(args..., varargs...);
            }
        }

    };
}
#endif //PYLLARS_PYLLARS_FUNTTRAITS_HPP
