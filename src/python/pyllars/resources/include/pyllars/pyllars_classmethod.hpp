//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_methodcallsemantics.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper-methods.impl.hpp"

#ifndef PYLLARS_PYLLARS_CLASSMETHOD_HPP
#define PYLLARS_PYLLARS_CLASSMETHOD_HPP

namespace pyllars{
    /**
     * Explicitly instatiate to map a C++ class method to a Python construct
     *
     * @tparam name  the name of the method
     * @tparam kwlist  the nullptr-terminate list of names for keywords (1-to-1 with arguments)
     * @tparam method_t  the class-signature of the method
     * @tparam method  pointer to the method
     */
    template<const char *const name, const char* const kwlist[], typename method_t, method_t method>
    class PyllarsClassMethod;

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...)>
    class PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...), method>{
    private:
        class Initializer{
        public:
            typedef ReturnType(Class::*method_t)(Args...);
            Initializer(){
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template addMethod<name, kwlist, method_t, method>();
                return  0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...)>
    typename PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...), method>::Initializer * const
        PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...), method>::initializer = new
                    PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...), method>::Initializer();



    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...) const>
    class PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const, method>{
    private:
        class Initializer{
        public:
            typedef ReturnType(Class::*method_t)(Args...) const;
            Initializer(){
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template addMethod<name, kwlist, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...) const>
    typename PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const, method>::Initializer * const
            PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const, method>::initializer = new
                    PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const, method>::Initializer();





    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args..., ...)>
    class PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...), method>{
    private:
        class Initializer{
        public:
            typedef ReturnType(Class::*method_t)(Args..., ...);
            Initializer(){
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template addMethod<name, kwlist, method_t, method>();
                return  0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args..., ...)>
    typename PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...), method>::Initializer * const
            PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...), method>::initializer = new
                    PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...), method>::Initializer();



    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args..., ...) const>
    class PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) const, method>{
    private:
        class Initializer{
        public:
            typedef ReturnType(Class::*method_t)(Args..., ...) const;
            Initializer(){
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template addMethod<name, kwlist, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args..., ...) const>
    typename PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) const, method>::Initializer * const
            PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) const, method>::initializer = new
                    PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) const, method>::Initializer();
}

#endif
