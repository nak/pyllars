//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"

#ifndef PYLLARS_PYLLARS_CLASSINSTANTITATION_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSINSTANTITATION_IMPL_HPP


namespace pyllars {



    template<typename Class, typename Parent, typename Z = typename std::enable_if<std::is_base_of<__pyllars_internal::CommonBaseWrapper, Parent>::value>::type>
    class PyllarsClass{
    public:
    private:
        class Initializer {
        public:
            explicit Initializer() {
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::preinit();
                __pyllars_internal::Init::registerReady( PythonClassWrapper<Class>::template ready<Parent>);
            }
        };

        static Initializer *const initializer;
    };

    template< typename Class, typename Parent, typename e>
    typename PyllarsClass<Class, Parent, e>::Initializer *const
            PyllarsClass<Class, Parent, e>::initializer = new PyllarsClass<Class, Parent, e>::Initializer();


}

#endif
