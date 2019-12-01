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

    template<typename T>
    template<OpBinaryEnum kind, typename ReturnType, typename ArgType,
            ReturnType(core_type<T>::type::*method)(ArgType)>
    void PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    BinaryOp<kind, ReturnType(core_type<T>::type::*)(ArgType), method>::addBinaryOperator() {
        PythonClassWrapper<T>::_binaryOperators()[kind] = (binaryfunc) MethodContainer<operand_kwlist,  ReturnType(core_type<T>::type::*)(ArgType), method>::callAsBinaryFunc;
    }

    template<typename T>
    template<OpBinaryEnum kind, typename ReturnType, typename ArgType,
            ReturnType(core_type<T>::type::*method)(ArgType) const>
    void PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    BinaryOp<kind, ReturnType(core_type<T>::type::*)(ArgType) const, method>::addBinaryOperator() {
        PythonClassWrapper<T>::_binaryOperatorsConst()[kind] =
                (binaryfunc) MethodContainer< operand_kwlist,  ReturnType(core_type<T>::type::*)(ArgType) const,method>::
                        callAsBinaryFunc;
    }
}


#endif
