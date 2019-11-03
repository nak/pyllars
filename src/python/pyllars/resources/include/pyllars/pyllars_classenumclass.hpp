//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSENUMCLASS_HPP
#define PYLLARS_PYLLARS_CLASSENUMCLASS_HPP

namespace pyllars{

    template<const char *const name, typename EnumClass, EnumClass value>
    class PyllarsClassEnumClassValue{
    private:
        class Initializer{
        public:
            Initializer(){
                using namespace __pyllars_internal;
                PythonClassWrapper<EnumClass>::addEnumClassValue(name, value);
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, typename EnumClass, EnumClass value>
    typename PyllarsClassEnumClassValue<name, EnumClass, value>::Initializer * const
            PyllarsClassEnumClassValue<name, EnumClass, value>::initializer = new
                    PyllarsClassEnumClassValue<name, EnumClass, value>::Initializer();

}

namespace __pyllars_internal {
    template<typename T>
    int PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    addEnumClassValue( const char* const name, const T value){
        if constexpr (std::is_constructible<T>::value) {
            _classEnumValues()[name] = new T(value);
        }
        return 0;
    }
}

#endif
