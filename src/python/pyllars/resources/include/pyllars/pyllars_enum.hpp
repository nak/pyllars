//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_ENUM_HPP
#define PYLLARS_PYLLARS_ENUM_HPP

namespace pyllars{

    template<const char* const name, typename EnumType, typename Parent>
    class PyllarsEnum{
    public:
        template<EnumType value>
        class Value{
        private:
            class Initializer{
            public:
                Initializer() {
                    __pyllars_internal::Init::registerInit(init);
                }

                static status_t init(){
                    using namespace __pyllars_internal;
                    if constexpr (std::is_base_of<pyllars::NSInfoBase, Parent>::value){
                        PyModule_AddObject(Parent::module(), name, toPyObject(value));
                    } else {
                        PythonClassWrapper<Parent>::template addEnumValue<EnumType>(name, value);
                    }
                    return 0;
                }

            };

            static Initializer * const initializer;

        };
    private:

        class Initializer{
        public:
            Initializer() {
                __pyllars_internal::Init::registerReady(ready);
            }

            static status_t ready(){
                using namespace __pyllars_internal;
                if constexpr(name != nullptr) {
                    if constexpr (is_base_of<pyllars::NSInfoBase, Parent>::value) {
                        PyModule_AddObject(Parent::module(), name, (PyObject*) PythonClassWrapper<EnumType>::getPyType());
                    } else {
                        PyObject_SetAttrString((PyObject*) PythonClassWrapper<Parent>::getPyType(), name, (PyObject*) PythonClassWrapper<EnumType>::getPyType());
                    }
                }
                return 0;
            }
        };

        static Initializer * const initializer;
    };

    template<const char *const name, typename EnumType, typename Parent>
    typename PyllarsEnum<name, EnumType, Parent>::Initializer * const
            PyllarsEnum<name, EnumType, Parent>::initializer= new
        PyllarsEnum<name, EnumType, Parent>::Initializer();

    template<const char *const name, typename EnumType, typename Parent>
    template<EnumType value>
    typename PyllarsEnum<name, EnumType, Parent>::template Value<value>::Initializer * const
            PyllarsEnum<name, EnumType, Parent>::Value<value>::initializer= new
                    PyllarsEnum<name, EnumType, Parent>::Value<value>::Initializer();

}

namespace __pyllars_internal {
    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addClassMember(const char *const name, PyObject *pyobj) {
        if (!_Type.tp_dict) {
            _Type.tp_dict = PyDict_New();
        }
        PyDict_SetItemString(_Type.tp_dict, name, pyobj);
    }

    template<typename T>
    template<typename EnumT>
    int PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addEnumValue( const char* const name, EnumT value){
        addClassMember(name, PyInt_FromLong((long int)value));
        return 0;
    }
}

#endif
