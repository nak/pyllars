//
// Created by jrusnak on 11/10/19.
//

#ifndef PYLLARS_PYLLARS_CLASSWRAPPER_ENUMCLASS_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSWRAPPER_ENUMCLASS_IMPL_HPP
#include "pyllars_classwrapper.hpp"


namespace pyllars_internal {

    template<typename T>
    template<typename EnumT>
    int PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addEnumValue( const char* const name, const EnumT &value){
        _classEnumValues()[name] = &value;
        return 0;
    }

    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addClassObject( const char* const name, PyObject* const obj){
        _classObjects()[name] = obj;
    }

    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addStaticType(const char *const name, PyTypeObject *(*pyobj)()) {
        _classTypes()[std::string(name)] = pyobj;
    }
}


#endif
