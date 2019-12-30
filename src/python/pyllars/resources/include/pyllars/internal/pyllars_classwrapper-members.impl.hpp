//
// Created by jrusnak on 11/10/19.
//

#ifndef PYLLARS_PYLLARS_CLASSWRAPPER_MEMBERS_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSWRAPPER_MEMBERS_IMPL_HPP
#include "pyllars_membersemantics.impl.hpp"
#include "pyllars_classwrapper.hpp"

namespace pyllars_internal {
    template<typename T, typename TrueType>
    template<const char *const name, typename FieldType>
    void
    PythonClassWrapper_Base<T, TrueType>::addAttribute(typename MemberPtr<FieldType>::member_t member) {
        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        const ssize_t array_size = ArraySize<FieldType>::size;
        MemberContainer<name, T, FieldType>::member = member;
        MemberContainer<name, T, FieldType>::array_size = array_size;
        Base::getTypeProxy()._member_getters[name] = MemberContainer<name, T, FieldType>::get;
        if constexpr (!std::is_const<FieldType>::value) {
            Base::getTypeProxy()._member_setters[name] = MemberContainer<name, T, FieldType>::set;
        }
    }

}


#endif //PYLLARS_PYLLARS_CLASSWRAPPER_METHODS_IMPL_HPP
