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
    template<OpBinaryEnum kind, typename method_t, method_t method>
    void Classes<T>::BinaryOp<kind, method_t, method>::addBinaryOperator() {
        if constexpr(is_const_method<method_t>::value){
            Classes<T>::Base::getTypeProxy()._binaryOperators[kind] = (binaryfunc) MethodContainer<operand_kwlist, method_t, method>::callAsBinaryFunc;
        } else {
            Classes<T>::Base::getTypeProxy()._binaryOperatorsConst[kind] =
                    (binaryfunc) MethodContainer<operand_kwlist, method_t, method>::callAsBinaryFunc;
        }
    }

}


#endif
