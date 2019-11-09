//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_membersemantics.impl.hpp"

#ifndef PYLLARS_PYLLARS_CLASSBITFIELD_HPP
#define PYLLARS_PYLLARS_CLASSBITFIELD_HPP

namespace pyllars{

    /**
     * template class to explicitly instantiate to add bitfield definition to a given class
     * @tparam name : the name of the bitfield attriute within Class
     * @tparam Class : the class to which the bitfield attribute belongs
     * @tparam AttrType : the underlying type given to the bitfield
     * @tparam bits : the size, in bits, of the bitfield
     * @tparam getter : a std::function pointer to get the value of the bitfield from a given instance of Class
     * @tparam setter : a std::function pointer to set the value of the bitfield of a given instance of Class, or nullptr/unspecified if Class is const type
     */
    template<const char *const name, typename Class, typename AttrType, size_t bits,
            std::function<AttrType(const Class &)> *getter,
            std::function<AttrType(Class &, const AttrType &)> *setter = nullptr>
    class PyllarsClassBitField{
    private:
        class Initializer{
        public:
            Initializer() {
                using namespace __pyllars_internal;
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                __pyllars_internal::PythonClassWrapper<Class>::template addBitField<name, AttrType, bits>(*getter, setter);
                return 0;
            }
        };

        static Initializer * const initializer;
    };

    template<const char *const name, typename Class, typename AttrType, size_t bits,
            std::function<AttrType(const Class &)> *getter,
            std::function<AttrType(Class &, const AttrType &)> *setter>
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
