//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_methodcallsemantics.impl.hpp"

#ifndef PYLLARS_PYLLARS_CLASSMETHOD_HPP
#define PYLLARS_PYLLARS_CLASSMETHOD_HPP

namespace pyllars{
    template<const char *const name, const char* const kwlist[], typename method_t, method_t method>
    class PyllarsClassMethod;

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...)>
    class PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...), method>{
    private:
        class Initializer{
        public:
            typedef ReturnType(Class::*method_t)(Args...);
            Initializer(){
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template addMethod<name, kwlist, method_t, method>();
                return  0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...)>
    typename PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...), method>::Initializer * const
        PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...), method>::initializer = new
                    PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...), method>::Initializer();



    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...) const>
    class PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const, method>{
    private:
        class Initializer{
        public:
            typedef ReturnType(Class::*method_t)(Args...) const;
            Initializer(){
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template addMethod<name, kwlist, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename Class, typename ReturnType, typename ...Args,  ReturnType(Class::*method)(Args...) const>
    typename PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const, method>::Initializer * const
            PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const, method>::initializer = new
                    PyllarsClassMethod<name, kwlist, ReturnType(Class::*)(Args...) const, method>::Initializer();
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
