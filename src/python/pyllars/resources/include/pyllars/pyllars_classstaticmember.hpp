//
// Created by jrusnak on 10/13/19.
//
#include "pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSSTATICMEMBER_HPP
#define PYLLARS_PYLLARS_CLASSSTATICMEMBER_HPP

namespace pyllars{

    template<const char *const name, typename T, typename Attr, Attr* attr>
    class PyllarsClassStaticMember{
    private:
        class Initializer{
        public:
            Initializer(){
                using namespace __pyllars_internal;
                PythonClassWrapper<T>::template addClassAttribute<name, Attr>(name, attr);
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, typename T, typename Attr, Attr* attr>
    typename PyllarsClassStaticMember<name, T, Attr, attr>::Initializer * const
            PyllarsClassStaticMember<name, T, Attr, attr>::initializer = new
                    PyllarsClassStaticMember<name, T, Attr, attr>::Initializer();
}

namespace __pyllars_internal {




    template<typename T>
    template<const char *const name, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addClassAttribute(FieldType *member) {

        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        ClassMember<name, T_NoRef, FieldType>::member = member;
        PyMethodDef pyMeth = {name,
                              (PyCFunction) ClassMember<name, T_NoRef, FieldType>::call,
                              METH_VARARGS | METH_KEYWORDS | METH_CLASS,
                              doc_string
        };
        _addMethod<true>(pyMeth);
    }
}

#endif
