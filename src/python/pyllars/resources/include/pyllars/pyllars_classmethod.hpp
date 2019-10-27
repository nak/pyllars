//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSMETHOD_HPP
#define PYLLARS_PYLLARS_CLASSMETHOD_HPP

namespace pyllars{

    template<const char *const name, const char* const kwlist[], typename Class, typename method_t, method_t method>
    class PyllarsClassMethod{
    private:
        class Initializer{
        public:
            Initializer(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template addMethod<name, kwlist, method_t, method>();
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename Class, typename method_t, method_t method>
    typename PyllarsClassMethod<name, kwlist, Class, method_t, method>::Initializer * const
        PyllarsClassMethod<name, kwlist, Class, method_t, method>::initializer = new
                    PyllarsClassMethod<name, kwlist, Class, method_t, method>::Initializer();
}

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
}

#endif
