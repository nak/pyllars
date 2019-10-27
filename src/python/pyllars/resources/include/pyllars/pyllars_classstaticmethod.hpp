//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSSTATICMETHOD_HPP
#define PYLLARS_PYLLARS_CLASSSTATICMETHOD_HPP

namespace pyllars{

    template<const char *const name, const char* const kwlist[], typename T, typename function_t, function_t *function>
    class PyllarsClassStaticMethod{
    private:
        class Initializer{
        public:
            Initializer(){
                using namespace __pyllars_internal;
                PythonClassWrapper<T>::template addClassMethod<name, kwlist, function_t, function>();
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, const char* const kwlist[], typename T, typename function_t, function_t *function>
    typename PyllarsClassStaticMethod<name, kwlist, T, function_t, function>::Initializer * const
    PyllarsClassStaticMethod<name, kwlist, T, function_t, function>::initializer = new
            PyllarsClassStaticMethod<name, kwlist, T, function_t, function>::Initializer();
}

namespace __pyllars_internal {



}

#endif
