//
// Created by jrusnak on 10/13/19.
//
#include "pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSENUM_HPP
#define PYLLARS_PYLLARS_CLASSENUM_HPP

namespace pyllars{

    template<const char *const name, typename Class, typename EnumType, EnumType value>
    class PyllarsClassEnum{
    private:
        class Initializer{
        public:
            Initializer(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template addEnumValue<EnumType>(name, value);
            }
        };

        static Initializer * const initializer;
    };

    template<const char *const name, typename Class, typename EnumType, EnumType value>
    typename PyllarsClassEnum<name, Class, EnumType, value>::Initializer * const
            PyllarsClassEnum<name, Class, EnumType, value>::initializer= new
        PyllarsClassEnum<name, Class, EnumType, value>::Initializer();

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
