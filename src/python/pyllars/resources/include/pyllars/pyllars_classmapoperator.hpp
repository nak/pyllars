//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_classwrapper-mapop.impl.hpp"

#ifndef PYLLARS_PYLLARS_CLASSMAPOPERATOR_HPP
#define PYLLARS_PYLLARS_CLASSMAPOPERATOR_HPP

namespace pyllars{
    /**
     * Explicitly instantiate to create a Python construct that wraps a C++ mapping operator (operator []) of a class
     *
     * @tparam method_t  the class-method type
     * @tparam method  pointer to the class method
     */
    template<typename method_t, method_t method>
    class PyllarsClassMapOperator;

    template<typename Class, typename ValueType, typename KeyType, ValueType (Class::*method)(KeyType)>
    class DLLEXPORT PyllarsClassMapOperator<ValueType (Class::*)(KeyType), method>{
    private:
        class DLLEXPORT Initializer{
        public:
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                typedef ValueType (Class::*method_t)(KeyType);
                typedef ValueType  (Class::*method_v_t)(KeyType) volatile;
                Classes<Class>::template addMapOperator<KeyType, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;

    };


    template<typename Class, typename ValueType, typename KeyType, ValueType (Class::*method)(KeyType)>
    typename PyllarsClassMapOperator<ValueType (Class::*)(KeyType), method>::Initializer * const
            PyllarsClassMapOperator<ValueType (Class::*)(KeyType), method>::initializer  = new
                    PyllarsClassMapOperator<ValueType (Class::*)(KeyType), method>::Initializer();

    template<typename Class, typename ValueType, typename KeyType, ValueType (Class::*method)(KeyType) const>
    class PyllarsClassMapOperator<ValueType (Class::*)(KeyType) const, method>{
    private:
        static_assert(!std::is_const<Class>::value && !std::is_volatile<Class>::value);
        class Initializer{
        public:
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                typedef ValueType (Class::*method_t)(KeyType) const;
                Classes<Class>::template addMapOperator<KeyType, method_t, method>();
                Classes<const Class>::template addMapOperator<KeyType, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;

    };


    template<typename Class, typename ValueType, typename KeyType, ValueType (Class::*method)(KeyType) const>
    typename PyllarsClassMapOperator<ValueType (Class::*)(KeyType) const, method>::Initializer * const
            PyllarsClassMapOperator<ValueType (Class::*)(KeyType) const, method>::initializer  = new
                    PyllarsClassMapOperator<ValueType (Class::*)(KeyType) const, method>::Initializer();

    template<typename Class, typename ValueType, typename KeyType, ValueType (Class::*method)(KeyType) volatile>
    class DLLEXPORT PyllarsClassMapOperator<ValueType (Class::*)(KeyType) volatile , method>{
    private:
        class DLLEXPORT Initializer{
        public:
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                typedef ValueType  (Class::*method_t)(KeyType) volatile;
                PythonClassWrapper<volatile Class>::template addMapOperator<KeyType, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;

    };


    template<typename Class, typename ValueType, typename KeyType, ValueType (Class::*method)(KeyType) volatile >
    typename PyllarsClassMapOperator<ValueType (Class::*)(KeyType) volatile , method>::Initializer * const
            PyllarsClassMapOperator<ValueType (Class::*)(KeyType) volatile , method>::initializer  = new
                    PyllarsClassMapOperator<ValueType (Class::*)(KeyType) volatile , method>::Initializer();

    template<typename Class, typename ValueType, typename KeyType, ValueType (Class::*method)(KeyType) const volatile>
    class DLLEXPORT PyllarsClassMapOperator<ValueType (Class::*)(KeyType) const volatile , method>{
    private:
        class DLLEXPORT Initializer{
        public:
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                typedef ValueType  (Class::*method_t)(KeyType) const volatile;
                PythonClassWrapper<volatile Class>::template addMapOperator<KeyType, method_t, method>();
                PythonClassWrapper<const volatile Class>::template addMapOperator<KeyType, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;

    };


    template<typename Class, typename ValueType, typename KeyType, ValueType (Class::*method)(KeyType) const volatile >
    typename PyllarsClassMapOperator<ValueType (Class::*)(KeyType) const volatile , method>::Initializer * const
            PyllarsClassMapOperator<ValueType (Class::*)(KeyType) const volatile , method>::initializer  = new
                    PyllarsClassMapOperator<ValueType (Class::*)(KeyType) const volatile , method>::Initializer();


}


#endif
