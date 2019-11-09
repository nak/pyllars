//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_classmembersemantics.impl.hpp"

#ifndef PYLLARS_PYLLARS_CLASSSTATICMEMBER_HPP
#define PYLLARS_PYLLARS_CLASSSTATICMEMBER_HPP

namespace pyllars{

    template<const char *const name, typename Class, typename Attr, Attr* attr>
    class PyllarsClassStaticMember{
    private:
        class Initializer{
        public:
            Initializer(){
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template addClassAttribute<name, Attr>(attr);
                return 0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, typename Class, typename Attr, Attr* attr>
    typename PyllarsClassStaticMember<name, Class, Attr, attr>::Initializer * const
            PyllarsClassStaticMember<name, Class, Attr, attr>::initializer = new
                    PyllarsClassStaticMember<name, Class, Attr, attr>::Initializer();
}

namespace __pyllars_internal {




    template<typename Class>
    template<const char *const name, typename FieldType>
    void PythonClassWrapper<Class,
            typename std::enable_if<is_rich_class<Class>::value>::type>::
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
