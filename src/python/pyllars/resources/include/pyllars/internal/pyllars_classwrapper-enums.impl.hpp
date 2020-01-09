//
// Created by jrusnak on 11/10/19.
//

#ifndef PYLLARS_PYLLARS_CLASSWRAPPER_ENUMCLASS_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSWRAPPER_ENUMCLASS_IMPL_HPP
#include "pyllars_classwrapper.hpp"


namespace pyllars_internal {

    template<typename T>
    template<typename EnumT>
    int Classes<T>::
    addEnumValue( const char* const name, const EnumT &value){
        Base::_classEnumValues()[name] = &value;
        return 0;
    }


}


#endif
