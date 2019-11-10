//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"

#ifndef PYLLARS_PYLLARS_CLASSINSTANTITATION_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSINSTANTITATION_IMPL_HPP


namespace pyllars {


    using namespace __pyllars_internal;

    template<typename Class, typename Parent, typename ...BaseClass>
    class PyllarsClass{
    public:
    private:
        static_assert(!is_complete<Parent>::value
        || is_base_of<pyllars::NSInfoBase, Parent>::value
        || is_rich_class<Parent>::value);

        class Initializer {
        public:
            explicit Initializer() {
                using namespace __pyllars_internal;
                Init::registerInit(PythonClassWrapper<Class>::preinit);
                Init::registerReady(ready);
            }

            static status_t ready(){
                using namespace __pyllars_internal;
                if constexpr (sizeof...(BaseClass) > 0){
                    PythonClassWrapper<Class>::template addBaseClass<BaseClass...>();
                }
                return PythonClassWrapper<Class>::template ready<Parent>();
            }
        };

        static Initializer *const initializer;
    };

    template< typename Class, typename Parent, typename ...BaseClass>
    typename PyllarsClass<Class, Parent, BaseClass...>::Initializer *const
            PyllarsClass<Class, Parent, BaseClass...>::initializer = new PyllarsClass<Class, Parent, BaseClass...>::Initializer();


}

#endif
