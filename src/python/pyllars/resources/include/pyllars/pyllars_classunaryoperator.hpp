//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSUNARYOPERATOR_HPP
#define PYLLARS_PYLLARS_CLASSUNARYOPERATOR_HPP

namespace pyllars{

    template<const char *const name, typename Class, typename ReturnType,  ReturnType(Class::*method)()>
    class PyllarsClassUnaryOperator{
    private:
        class Initializer{
        public:
            Initializer(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template Op<name, ReturnType, method>::addUnaryOperator();
            }
        };

        static Initializer* const initializer;

    };

    template<const char *const name, typename Class, typename ReturnType,  ReturnType(Class::*method)()>
    typename PyllarsClassUnaryOperator<name, Class, ReturnType, method>::Initializer * const
            PyllarsClassUnaryOperator<name, Class, ReturnType, method>::initializer  = new
                    PyllarsClassUnaryOperator<name, Class, ReturnType, method>::Initializer();
}

namespace __pyllars_internal {

    template<typename T>
    template<const char* const name, typename ReturnType, ReturnType( core_type<T>::type::*method)()>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    template Op<name, ReturnType( core_type<T>::type::*)(), method>::addUnaryOperator() {
        static const char* const kwlist[1] = {nullptr};
        PythonClassWrapper<T>::_unaryOperators[name] = (unaryfunc) MethodContainer<kwlist, ReturnType(T::*)(), method>::callAsUnaryFunc;
    }


    template<typename T>
    template<const char* const name, typename ReturnType, ReturnType(core_type<T>::type::*method)() const>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    Op<name, ReturnType(core_type<T>::type::*)() const, method>::addUnaryOperator() {
        static const char* const kwlist[1] = {nullptr};
        PythonClassWrapper<T>::_unaryOperatorsConst()[name] = (unaryfunc) MethodContainer<kwlist, ReturnType(CClass::*)() const, method>::callAsUnaryFunc;
    }

}

#endif
