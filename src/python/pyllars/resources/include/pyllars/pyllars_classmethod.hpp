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
     * Explicitly instantiate to map a C++ class method to a Python construct
     *
     * @tparam name  the name of the method
     * @tparam kwlist  the nullptr-terminate list of names for keywords (1-to-1 with arguments)
     * @tparam method_t  the class-signature of the method
     * @tparam method  pointer to the method
     */
    template<const char *const name, const char* const kwlist[], typename method_t, method_t method>
    class PyllarsClassMethod;

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...)>
    class DLLEXPORT PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...), method>{
    private:
        class Initializer{
        public:
            typedef ReturnType(Class::*method_t)(Args...);
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                PythonClassWrapper<Class>::template addMethod<name, kwlist, method_t, method>();
                return  0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...)>
    typename PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...), method>::Initializer * const
        PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...), method>::initializer = new
                    typename PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...), method>::Initializer();


    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...) const>
    class DLLEXPORT PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const, method>{
    private:
        class Initializer{
        public:
            typedef ReturnType(Class::*method_t)(Args...) const;
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                PythonClassWrapper<Class>::template addMethod<name, kwlist, method_t, method>();
                PythonClassWrapper<const Class>::template addMethod<name, kwlist, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...) const>
    typename PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const, method>::Initializer * const
            PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const, method>::initializer = new
                  typename  PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const, method>::Initializer();


    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...) volatile >
    class DLLEXPORT PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) volatile, method>{
    private:
        class Initializer{
        public:
            typedef ReturnType(Class::*method_t)(Args...) volatile;
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                PythonClassWrapper<volatile Class>::template addMethod<name, kwlist, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...) volatile >
    typename PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) volatile , method>::Initializer * const
            PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) volatile , method>::initializer = new
                    typename  PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) volatile , method>::Initializer();


    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...) const volatile >
    class DLLEXPORT PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const volatile , method>{
    private:
        class Initializer{
        public:
            typedef ReturnType(Class::*method_t)(Args...) const volatile ;
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                PythonClassWrapper<volatile Class>::template addMethod<name, kwlist, method_t, method>();
                PythonClassWrapper<const volatile Class>::template addMethod<name, kwlist, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...) const volatile >
    typename PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const volatile, method>::Initializer * const
            PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const volatile , method>::initializer = new
                    typename  PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const volatile , method>::Initializer();




    ////////VARARGS//////////////


    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args..., ...)>
    class DLLEXPORT PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...), method>{
    private:
        class Initializer{
        public:
            typedef ReturnType(Class::*method_t)(Args..., ...);
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
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
    class DLLEXPORT PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) const, method>{
    private:
        class DLLEXPORT Initializer{
        public:
            typedef ReturnType(Class::*method_t)(Args..., ...) const;
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                PythonClassWrapper<Class>::template addMethod<name, kwlist, method_t, method>();
                PythonClassWrapper<const Class>::template addMethod<name, kwlist, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args..., ...) const>
    typename PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) const, method>::Initializer * const
            PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) const, method>::initializer = new
                    PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) const, method>::Initializer();



    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args..., ...) volatile >
    class DLLEXPORT PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) volatile , method>{
    private:
        class DLLEXPORT Initializer{
        public:
            typedef ReturnType(Class::*method_t)(Args..., ...) volatile ;
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                PythonClassWrapper<volatile Class>::template addMethod<name, kwlist, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args..., ...) volatile >
    typename PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) volatile , method>::Initializer * const
            PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) volatile , method>::initializer = new
                    PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) volatile , method>::Initializer();



    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args..., ...) const volatile >
    class DLLEXPORT PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) const volatile , method>{
    private:
        class DLLEXPORT Initializer{
        public:
            typedef ReturnType(Class::*method_t)(Args..., ...) const volatile ;
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                PythonClassWrapper<volatile Class>::template addMethod<name, kwlist, method_t, method>();
                PythonClassWrapper<const volatile Class>::template addMethod<name, kwlist, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args..., ...) const volatile >
    typename PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) const volatile, method>::Initializer * const
            PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) const volatile , method>::initializer = new
                    PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args..., ...) const volatile , method>::Initializer();
}

#endif
