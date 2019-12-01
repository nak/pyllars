//
// Created by jrusnak on 11/10/19.
//

#ifndef PYLLARS_PYLLARS_CLASSWRAPPER_STATICMEMBERS_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSWRAPPER_STATICMEMBERS_IMPL_HPP
#include "pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_classmembersemantics.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper-methods.impl.hpp"

namespace pyllars_internal {


    template<typename Class>
    template<const char *const name, typename FieldType>
    void PythonClassWrapper<Class,
            typename std::enable_if<is_rich_class<Class>::value>::type>::
    addStaticAttribute(FieldType *member) {

        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        ClassMember<name, T_NoRef, FieldType>::member = member;
        PyMethodDef pyMeth = {name,
                              (PyCFunction) ClassMember<name, T_NoRef, FieldType>::call,
                              METH_VARARGS | METH_KEYWORDS | METH_CLASS,
                              doc_string
        };
        _addMethodConst(pyMeth);
    }

}


#endif
