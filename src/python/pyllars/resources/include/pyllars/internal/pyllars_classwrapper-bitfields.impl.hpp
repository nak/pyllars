//
// Created by jrusnak on 11/10/19.
//

#ifndef PYLLARS_PYLLARS_CLASSWRAPPER_BITFIELDS_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSWRAPPER_BITFIELDS_IMPL_HPP
#include "pyllars_classwrapper.hpp"
#include "pyllars_membersemantics.impl.hpp"

namespace pyllars_internal {

    template<typename T>
    template<const char *const name, typename FieldType, const size_t bits>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addBitField(
            typename std::function< FieldType(const T_NoRef&)> &getter,
            typename  std::function< FieldType(T_NoRef&, const FieldType&)>  *setter) {
        static const char *const doc = "Get bit-field attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::_getter = getter;
        _member_getters()[name] = BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::get;
        if (setter) {
            BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::_setter = *setter;
            _member_setters()[name] = BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::set;
        }
    }

}


#endif
