//
// Created by jrusnak on 11/10/19.
//

#ifndef PYLLARS_PYLLARS_CLASSWRAPPER_ENUMCLASS_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSWRAPPER_ENUMCLASS_IMPL_HPP
#include "pyllars_classwrapper.hpp"


namespace pyllars_internal {

    template<typename T, typename TrueType>
    template<typename EnumT>
    int PythonClassWrapper_Base<T, TrueType>::
    addEnumValue( const char* const name, const EnumT &value){
        Base::_classEnumValues()[name] = &value;
        return 0;
    }


}


#endif
