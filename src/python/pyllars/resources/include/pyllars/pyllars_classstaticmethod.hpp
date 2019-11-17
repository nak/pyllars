//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"

#ifndef PYLLARS_PYLLARS_CLASSSTATICMETHOD_HPP
#define PYLLARS_PYLLARS_CLASSSTATICMETHOD_HPP

namespace pyllars{

    /**
     * Explicitly instantiate to map a C++ static class method to a Python construct
     *
     * @tparam name  the name of the static method
     * @tparam kwlist  nullptr-terminated list of keywords in 1-to-1 association with arguments
     * @tparam Class  the class to which the static method belongs
     * @tparam function_t  the signature of the static method
     * @tparam function   pointer to the static method
     */
    template<const char *const name, const char* const kwlist[], typename Class, typename function_t, function_t function>
    class PyllarsClassStaticMethod{
    private:
        class Initializer{
        public:
            Initializer(){
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template addStaticMethod<name, kwlist, function_t, function>();
                return 0;
            }
        };

        static Initializer* const initializer;
    };
    template<const char *const name, const char* const kwlist[], typename Class, typename function_t, function_t function>
    typename PyllarsClassStaticMethod<name, kwlist, Class, function_t, function>::Initializer * const
    PyllarsClassStaticMethod<name, kwlist, Class, function_t, function>::initializer = new
            PyllarsClassStaticMethod<name, kwlist, Class, function_t, function>::Initializer();
}


#endif
