//
// Created by jrusnak on 10/13/19.
//
#include "pyllars_classwrapper.impl.hpp"

#ifndef PYLLARS_PYLLARS_CLASSINSTANTITATION_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSINSTANTITATION_IMPL_HPP


namespace pyllars {

    template<typename T, typename Parent, bool enabled = std::enable_if<std::is_base_of<__pyllars_internal::CommonBaseWrapper, Parent>::value>::value>
    class PyllarsClass {
    private:
        class Initializer {
            explicit Initializer() {
                using namespace __pyllars_internal;
                PythonClassWrapper<T>::readyImpl() = PythonClassWrapper<T>::template ready<Parent>();
            }
        };

        static Initializer *const initializer;
    };

    template<typename T, typename Parent, bool e>
    typename PyllarsClass<T, Parent, e>::Initializer *const
            PyllarsClass<T, Parent, e>::initializer = new PyllarsClass<T, Parent, e>::Initializer();

}
#endif
