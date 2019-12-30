//
// Created by jrusnak on 11/10/19.
//

#ifndef PYLLARS_PYLLARS_CLASSWRAPPER_STATICMEMBERS_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSWRAPPER_STATICMEMBERS_IMPL_HPP
#include "pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_classmembersemantics.impl.hpp"

namespace pyllars_internal {


    template<typename T, typename TrueType>
    template<const char *const name, typename FieldType>
    void
    PythonClassWrapper_Base<T, TrueType>::addStaticAttribute(FieldType *member) {

        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        ClassMember<name, T, FieldType>::member = member;
        PyMethodDef pyMeth = {name,
                              (PyCFunction) ClassMember<name, T, FieldType>::call,
                              METH_VARARGS | METH_KEYWORDS | METH_CLASS,
                              doc_string
        };
        Base::getTypeProxy().addPyMethod(pyMeth, false);
    }

}


#endif
