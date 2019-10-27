//
// Created by jrusnak on 10/13/19.
//
#include "pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSBITFIELD_HPP
#define PYLLARS_PYLLARS_CLASSBITFIELD_HPP

namespace pyllars{

    template<const char *const name, typename Class, typename AttrType, size_t bits,
            typename __pyllars_internal::BitFieldContainer<typename std::remove_reference<Class>::type>::template Container<name, AttrType, bits>::getter_t *getter,
            typename __pyllars_internal::BitFieldContainer<typename std::remove_reference<Class>::type>::template Container<name, AttrType, bits>::setter_t *setter >
    class PyllarsClassBitField{
    private:
        class Initializer{
        public:
            Initializer(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template addBitField<name, AttrType, bits>(getter, setter);
            }
        };

        static Initializer * const initializer;
    };

    template<const char *const name, typename Class, typename AttrType, size_t bits,
            typename __pyllars_internal::BitFieldContainer<typename std::remove_reference<Class>::type>::template Container<name, AttrType, bits>::getter_t *getter,
            typename __pyllars_internal::BitFieldContainer<typename std::remove_reference<Class>::type>::template Container<name, AttrType, bits>::setter_t *setter >
    typename PyllarsClassBitField<name, Class, AttrType, bits, getter, setter>::Initializer * const
            PyllarsClassBitField<name, Class, AttrType, bits, getter, setter>::initializer = new
                    PyllarsClassBitField<name, Class, AttrType, bits, getter, setter>::Initializer();

    }

namespace __pyllars_internal {

    template<typename T>
    template<const char *const name, typename FieldType, const size_t bits>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addBitField(
            typename BitFieldContainer<typename std::remove_reference<T>::type>::template Container<name, FieldType, bits>::getter_t &getter,
            typename BitFieldContainer<typename std::remove_reference<T>::type>::template Container<name, FieldType, bits>::setter_t *setter) {
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
