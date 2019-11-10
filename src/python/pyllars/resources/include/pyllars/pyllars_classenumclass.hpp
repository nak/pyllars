//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSENUMCLASS_HPP
#define PYLLARS_PYLLARS_CLASSENUMCLASS_HPP

namespace pyllars{

    template<const char *const name, typename EnumClass, typename Parent>
    class PyllarsClassEnumClassValue{
    private:
        template <EnumClass value>
        class Value {
            class Initializer {
            public:
                Initializer() {
                    __pyllars_internal::Init::registerInit(init);
                }

                static status_t init() {
                    using namespace __pyllars_internal;
                    PythonClassWrapper<EnumClass>::addEnumClassValue(name, value);
                    return 0;
                }
            };
            static Initializer* const initializer;

        };

        class Initializer{
        public:
            Initializer() {
                __pyllars_internal::Init::registerReady(ready);
            }


            static status_t ready(){
                using namespace __pyllars_internal;
                if constexpr(is_base_of<pyllars::NSInfoBase, Parent>::value) {
                    PyModule_AddObject(Parent::module(), name, (PyObject*) PythonClassWrapper<EnumClass>::getPyType());
                } else {
                    PyObject_SetAttrString((PyObject*) Parent::getPyType(), Types<EnumClass>::type_name(), (PyObject*) PythonClassWrapper<EnumClass>::getPyType());
                }
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, typename EnumClass, typename Parent>
    typename PyllarsClassEnumClassValue<name, EnumClass, Parent>::Initializer * const
            PyllarsClassEnumClassValue<name, EnumClass, Parent>::initializer = new
                    PyllarsClassEnumClassValue<name, EnumClass, Parent>::Initializer();

    template<const char *const name, typename EnumClass, typename Parent>
    template<EnumClass value>
    typename PyllarsClassEnumClassValue<name, EnumClass, Parent>::template Value<value>::Initializer * const
            PyllarsClassEnumClassValue<name, EnumClass, Parent>::Value<value>::initializer = new
                    PyllarsClassEnumClassValue<name, EnumClass, Parent>::Value<value>::Initializer();

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
