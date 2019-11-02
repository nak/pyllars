//
// Created by jrusnak on 10/13/19.
//
#include "pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSBINARYOPERATOR_HPP
#define PYLLARS_PYLLARS_CLASSBINARYOPERATOR_HPP

namespace pyllars{

    template<const char *const name,  const char* const kwlist[2], typename Class, typename ReturnType, typename ArgType, ReturnType(Class::*method)(ArgType)>
    class PyllarsClassBinaryOperator{
    private:
        class Initializer{
        public:
            Initializer(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template BinaryOp<name, kwlist, ReturnType, ArgType, method>::addBinaryOperator();
            }
        };

        static Initializer * const initializer;
    };

    template<const char *const name,  const char* const kwlist[2], typename Class, typename ReturnType, typename ArgType, ReturnType(Class::*method)(ArgType)>
    typename PyllarsClassBinaryOperator<name, kwlist, Class, ReturnType, ArgType, method>::Initializer * const
            PyllarsClassBinaryOperator<name, kwlist, Class, ReturnType, ArgType, method>::initializer = new
                    PyllarsClassBinaryOperator<name, kwlist, Class, ReturnType, ArgType, method>::Initializer();

}

namespace __pyllars_internal {

    template<typename T>
    template<OpBinaryEnum kind, const char* const kwlist[2], typename ReturnType, typename ArgType,
            ReturnType(core_type<T>::type::*method)(ArgType)>
    void PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    BinaryOp<kind, kwlist, ReturnType(core_type<T>::type::*)(ArgType), method>::addBinaryOperator() {
        PythonClassWrapper<T>::_binaryOperators()[kind] = (binaryfunc) MethodContainer<kwlist,  ReturnType(core_type<T>::type::*)(ArgType), method>::callAsBinaryFunc;
    }

    template<typename T>
    template<OpBinaryEnum kind, const char* const kwlist[2], typename ReturnType, typename ArgType,
            ReturnType(core_type<T>::type::*method)(ArgType) const>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    BinaryOp<kind, kwlist, ReturnType(core_type<T>::type::*)(ArgType) const, method>::addBinaryOperator() {
        PythonClassWrapper<T>::_binaryOperatorsConst()[kind] = (binaryfunc) MethodContainer< kwlist,  ReturnType(core_type<T>::type::*)(ArgType) const,
                method>::callAsBinaryFunc;
    }

}

#endif
