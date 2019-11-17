//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper-bitfields.impl.hpp"
#include "pyllars/internal/pyllars_membersemantics.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSBITFIELD_HPP
#define PYLLARS_PYLLARS_CLASSBITFIELD_HPP

namespace pyllars{

    /**
     * Explicitly instantiate thjis class to add bitfield definition to a given class at compile-time
     *
     * @tparam name  the name of the bitfield attriute within Class
     * @tparam Class  the class to which the bitfield attribute belongs
     * @tparam AttrType  the underlying type given to the bitfield
     * @tparam bits  the size, in bits, of the bitfield
     * @tparam getter  a std::function pointer to get the value of the bitfield from a given instance of Class
     * @tparam setter  a std::function pointer to set the value of the bitfield of a given instance of Class, or nullptr/unspecified if Class is const type
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
#endif
