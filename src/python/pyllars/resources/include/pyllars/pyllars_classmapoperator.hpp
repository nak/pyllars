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
    class PyllarsClassMapOperator<ValueType (Class::*)(KeyType), method>{
    private:
        class Initializer{
        public:
            Initializer(){
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace __pyllars_internal;
                typedef ValueType (Class::*method_t)(KeyType);
                PythonClassWrapper<Class>::template addMapOperator<KeyType, method_t, method>();
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
        class Initializer{
        public:
            Initializer(){
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace __pyllars_internal;
                typedef ValueType (Class::*method_t)(KeyType) const;
                PythonClassWrapper<Class>::template addMapOperator<KeyType, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;

    };


    template<typename Class, typename ValueType, typename KeyType, ValueType (Class::*method)(KeyType) const>
    typename PyllarsClassMapOperator<ValueType (Class::*)(KeyType) const, method>::Initializer * const
            PyllarsClassMapOperator<ValueType (Class::*)(KeyType) const, method>::initializer  = new
                    PyllarsClassMapOperator<ValueType (Class::*)(KeyType) const, method>::Initializer();
}


#endif
