//
// Created by jrusnak on 11/10/19.
//

#ifndef PYLLARS_PYLLARS_CLASSWRAPPER_OPERATORS_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSWRAPPER_OPERATORS_IMPL_HPP
#include "pyllars/pyllars.hpp"
#include "pyllars_classwrapper.hpp"
#include "pyllars_methodcallsemantics.impl.hpp"

namespace pyllars_internal {
    static const char * const operand_kwlist[] = {"operand", nullptr};

    template<typename T, typename TrueType>
    template<OpBinaryEnum kind, typename ReturnType, typename ArgType, ReturnType(core_type<T>::type::*method)(ArgType)>
    void PythonClassWrapper_Base<T, TrueType>::
    BinaryOp<kind, ReturnType(core_type<T>::type::*)(ArgType), method>::addBinaryOperator() {
         Base::getTypeProxy()._binaryOperators[kind] = (binaryfunc) MethodContainer<operand_kwlist,  ReturnType(core_type<T>::type::*)(ArgType), method>::callAsBinaryFunc;
    }

    template<typename T, typename TrueType>
    template<OpBinaryEnum kind, typename ReturnType, typename ArgType, ReturnType(core_type<T>::type::*method)(ArgType) const>
    void PythonClassWrapper_Base<T, TrueType>::
    BinaryOp<kind, ReturnType(core_type<T>::type::*)(ArgType) const, method>::addBinaryOperator() {
        Base::getTypeProxy()._binaryOperatorsConst[kind] =
                (binaryfunc) MethodContainer< operand_kwlist,  ReturnType(core_type<T>::type::*)(ArgType) const,method>::
                        callAsBinaryFunc;
    }
}


#endif
