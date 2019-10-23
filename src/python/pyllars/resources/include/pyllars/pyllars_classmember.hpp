//
// Created by jrusnak on 10/13/19.
//
#include "pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSMEMBER_HPP
#define PYLLARS_PYLLARS_CLASSMEMBER_HPP

namespace pyllars{

    template<const char *const name, typename Class, typename AttrType, typename __pyllars_internal::MemberContainer<name, Class, AttrType>::member_t attr>
    class PyllarsClassMember{
    private:
        class Initializer{
        public:
            Initializer(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template addAttribute<name, AttrType>(name, attr);
            }
        };

        static Initializer * const initializer;
    };

    template<const char *const name, typename Class, typename AttrType, typename __pyllars_internal::MemberContainer<name, Class, AttrType>::member_t attr>
    typename PyllarsClassMember<name, Class, AttrType, attr>::Initializer * const
            PyllarsClassMember<name, Class, AttrType, attr>::initializer = new
                    PyllarsClassMember<name, Class, AttrType, attr>::Initializer();
}

namespace __pyllars_internal {

    template<typename T>
    template<const char *const name, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addAttribute(typename MemberContainer<name, T_NoRef, FieldType>::member_t member) {

        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        const ssize_t array_size = ArraySize<FieldType>::size;
        MemberContainer<name, T_NoRef, FieldType>::member = member;
        MemberContainer<name, T_NoRef, FieldType>::array_size = array_size;
        _member_getters()[name] = MemberContainer<name, T_NoRef, FieldType>::get;
        if constexpr (!std::is_const<FieldType>::value) {
            _member_setters()[name] = MemberContainer<name, T_NoRef, FieldType>::set;
        }
    }

}

#endif
