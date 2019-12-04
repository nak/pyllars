//
// Created by jrusnak on 11/10/19.
//

#ifndef PYLLARS_PYLLARS_CLASSWRAPPER_METHODS_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSWRAPPER_METHODS_IMPL_HPP
#include "pyllars_classwrapper.hpp"
#include "pyllars_methodcallsemantics.impl.hpp"
#include "pyllars_funttraits.hpp"
#include "pyllars_staticfunctionsemantics.impl.hpp"
#include "pyllars_classwrapper-staticmethods.impl.hpp"

namespace pyllars_internal {

    template<typename Class>
    void PythonClassWrapper<Class,
            typename std::enable_if<is_rich_class<Class>::value>::type>::
    _addMethodNonConst(PyMethodDef method) {
        //insert at beginning to keep null sentinel at end of list:
        _methodCollection()[method.ml_name] = method;
    }

    template<typename Class>
    void PythonClassWrapper<Class,
            typename std::enable_if<is_rich_class<Class>::value>::type>::
    _addMethodConst(PyMethodDef method) {
        //insert at beginning to keep null sentinel at end of list:
        _methodCollectionConst()[method.ml_name] = method;
    }


    template<typename Class>
    template<const char *const name, const char* const kwlist[], typename method_t, method_t method>
    void PythonClassWrapper<Class,
            typename std::enable_if<is_rich_class<Class>::value>::type>::
    addMethod() {
        static std::string doc = std::string("Call method ") +  func_traits<method_t>::type_name();
        PyMethodDef pyMeth = {
                name,
                (PyCFunction) MethodContainer<kwlist, method_t, method>::call,
                METH_KEYWORDS | METH_VARARGS,
                doc.c_str()
        };
        if constexpr(func_traits<method_t>::is_const_method) {
            _addMethodConst(pyMeth);
            if constexpr (!std::is_const<Class>::value) {
                PythonClassWrapper<const Class>::template addMethod<name, kwlist, method_t, method>();
            }
        } else {
            _addMethodNonConst(pyMeth);
        }
    }

}


#endif //PYLLARS_PYLLARS_CLASSWRAPPER_METHODS_IMPL_HPP
