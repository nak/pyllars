//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSSTATICMETHOD_HPP
#define PYLLARS_PYLLARS_CLASSSTATICMETHOD_HPP

namespace pyllars{

    template<const char *const name, const char* const kwlist[], typename Class, typename function_t, function_t *function>
    class PyllarsClassStaticMethod{
    private:
        class Initializer{
        public:
            Initializer(){
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template addClassMethod<name, kwlist, function_t, function>();
                return 0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename Class, typename function_t, function_t *function>
    typename PyllarsClassStaticMethod<name, kwlist, Class, function_t, function>::Initializer * const
    PyllarsClassStaticMethod<name, kwlist, Class, function_t, function>::initializer = new
            PyllarsClassStaticMethod<name, kwlist, Class, function_t, function>::Initializer();
}


#endif
