//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"

#ifndef PYLLARS_PYLLARS_CLASSINSTANTITATION_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSINSTANTITATION_IMPL_HPP


namespace pyllars {

    template <typename Class>
    class BasePyllarsClass{
        static const char* name;
    };

    template<const char* const _name, typename Class, typename Parent, typename Z = typename std::enable_if<std::is_base_of<__pyllars_internal::CommonBaseWrapper, Parent>::value>::type>
    class PyllarsClass : public BasePyllarsClass<Class>{
    public:
    private:
        class Initializer {
        public:
            explicit Initializer() {
                __pyllars_internal::_Types<Class>::type_name = pyllars::BasePyllarsClass<Class>::name = _name;
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::preinit();
                __pyllars_internal::Init::registerReady( PythonClassWrapper<Class>::template ready<Parent>);
            }
        };

        static Initializer *const initializer;
    };

    template<const char* const name, typename Class, typename Parent, typename e>
    typename PyllarsClass<name, Class, Parent, e>::Initializer *const
            PyllarsClass<name, Class, Parent, e>::initializer = new PyllarsClass<name, Class, Parent, e>::Initializer();


}

template<typename Class>
const char* __pyllars_internal::_Types<Class>::type_name = nullptr;

#endif
