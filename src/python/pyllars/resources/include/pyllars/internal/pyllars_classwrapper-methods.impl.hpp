//
// Created by jrusnak on 11/10/19.
//

#ifndef PYLLARS_PYLLARS_CLASSWRAPPER_METHODS_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSWRAPPER_METHODS_IMPL_HPP
#include "pyllars_classwrapper.hpp"
#include "pyllars_methodcallsemantics.impl.hpp"
#include "pyllars_funttraits.hpp"

namespace __pyllars_internal {

    template<typename Class>
    template<bool is_const>
    void PythonClassWrapper<Class,
            typename std::enable_if<is_rich_class<Class>::value>::type>::
    _addMethod(PyMethodDef method) {
        //insert at beginning to keep null sentinel at end of list:
        if constexpr(is_const) {
            _methodCollectionConst()[method.ml_name] = method;
        } else {
            _methodCollection()[method.ml_name] = method;
        }
    }


    template<typename Class>
    template<const char *const name, const char* const kwlist[], typename method_t, method_t method>
    void PythonClassWrapper<Class,
            typename std::enable_if<is_rich_class<Class>::value>::type>::
    addMethod() {
        static const char *const doc = "Call method ";
        char *doc_string = new char[func_traits<method_t>::type_name().size() + strlen(doc) + 1];
        snprintf(doc_string, func_traits<method_t>::type_name().size() + strlen(doc) + 1, "%s%s", doc, func_traits<method_t>::type_name().c_str());
        PyMethodDef pyMeth = {
                name,
                (PyCFunction) MethodContainer<kwlist, method_t, method>::call,
                METH_KEYWORDS | METH_VARARGS,
                doc_string
        };
        _addMethod<func_traits<method_t>::is_const_method>(pyMeth);
    }

    template<typename T>
    template<const char *const name, const char *const kwlist[], typename func_type, func_type method>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addStaticMethod() {
        static const char *const doc = "Call class method ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);

        PyMethodDef pyMeth = {
                name,
                (PyCFunction) StaticFunctionContainer<kwlist, func_type, method>::call,
                METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                doc_string
        };

        _addMethod<true>(pyMeth);
    }
}


#endif //PYLLARS_PYLLARS_CLASSWRAPPER_METHODS_IMPL_HPP
