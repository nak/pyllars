//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_function_wrapper.hpp"
#include "pyllars/internal/pyllars_staticfunctionsemantics.impl.hpp"

#ifndef PYLLARS_PYLLARS_FUNCTION_HPP
#define PYLLARS_PYLLARS_FUNCTION_HPP

namespace pyllars{

    template<const char *const name, const char* kwlist[], typename function_t, function_t f, typename Parent, typename Z = void>
    class PyllarsFunction;

    template<const char *const name, const char* kwlist[], typename function_t, function_t f, typename Parent>
    class DLLEXPORT PyllarsFunction<name, kwlist, function_t, f, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value> >{
    private:
        class DLLEXPORT Initializer{
        public:
            Initializer(){
                pyllars_internal::Init::registerReady(&ready);
            }

            static status_t ready(){
                using namespace pyllars;
                auto module = Parent::module();
                if(pyllars_internal::PythonFunctionWrapper<function_t>::initialize() != 0){
                    return -1;
                }
                if (module){
                    PyObject * created = pyllars_internal::PythonFunctionWrapper<function_t>::template createPy<kwlist, f>(name);
                    if (!created){
                        return -1;
                    }
                    PyModule_AddObject(Parent::module(), name, created);
                } else {
                  return -1;
                }
                return 0;
            }
        };

        static Initializer * const initializer;
    };

    template<const char *const name, const char* kwlist[], typename function_t, function_t f, typename Parent>
    typename PyllarsFunction<name, kwlist, function_t, f, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value> >::Initializer * const
            PyllarsFunction<name, kwlist, function_t, f, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value>  >::initializer= new
        PyllarsFunction<name, kwlist, function_t, f, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value> >::Initializer();

}


#endif
