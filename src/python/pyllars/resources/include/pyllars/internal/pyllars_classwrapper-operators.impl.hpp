//
// Created by jrusnak on 11/10/19.
//

#ifndef PYLLARS_PYLLARS_CLASSWRAPPER_OPERATORS_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSWRAPPER_OPERATORS_IMPL_HPP
#include "pyllars_classwrapper.hpp"
#include "pyllars_methodcallsemantics.impl.hpp"
#include "pyllars/pyllars.hpp"

namespace __pyllars_internal {
    template<typename T>
    template<OpUnaryEnum kind, typename ReturnType, ReturnType( core_type<T>::type::*method)()>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    template Op<kind, ReturnType( core_type<T>::type::*)(), method>::addUnaryOperator() {
        PythonClassWrapper<T>::_unaryOperators[kind] = (unaryfunc) MethodContainer<pyllars_empty_kwlist, ReturnType(T::*)(), method>::callAsUnaryFunc;
    }


    template<typename T>
    template<OpUnaryEnum kind, typename ReturnType, ReturnType(core_type<T>::type::*method)() const>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    Op<kind, ReturnType(core_type<T>::type::*)() const, method>::addUnaryOperator() {
        PythonClassWrapper<T>::_unaryOperatorsConst()[kind] = (unaryfunc) MethodContainer<pyllars_empty_kwlist, ReturnType(T::*)() const, method>::callAsUnaryFunc;
    }

    template<typename T>
    template<OpBinaryEnum kind, typename ReturnType, typename ArgType,
            ReturnType(core_type<T>::type::*method)(ArgType)>
    void PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    BinaryOp<kind, ReturnType(core_type<T>::type::*)(ArgType), method>::addBinaryOperator() {
        PythonClassWrapper<T>::_binaryOperators()[kind] = (binaryfunc) MethodContainer<pyllars_empty_kwlist,  ReturnType(core_type<T>::type::*)(ArgType), method>::callAsBinaryFunc;
    }

    template<typename T>
    template<OpBinaryEnum kind, typename ReturnType, typename ArgType,
            ReturnType(core_type<T>::type::*method)(ArgType) const>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    BinaryOp<kind, ReturnType(core_type<T>::type::*)(ArgType) const, method>::addBinaryOperator() {
        static constexpr cstring kwlist[] = {"operand", nullptr};
        PythonClassWrapper<T>::_binaryOperatorsConst()[kind] = (binaryfunc) MethodContainer< pyllars_empty_kwlist,  ReturnType(core_type<T>::type::*)(ArgType) const,
                method>::callAsBinaryFunc;
    }
}


#endif
