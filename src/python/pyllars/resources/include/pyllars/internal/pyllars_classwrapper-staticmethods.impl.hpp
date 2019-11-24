//
// Created by jrusnak on 11/10/19.
//

#ifndef PYLLARS_PYLLARS_CLASSWRAPPER_STATICMETHODS_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSWRAPPER_STATICMETHODS_IMPL_HPP
#include "pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_classmembersemantics.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper-methods.impl.hpp"

namespace __pyllars_internal {

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


#endif
