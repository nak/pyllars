//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_classwrapper-staticmembers.impl.hpp"

#ifndef PYLLARS_PYLLARS_CLASSSTATICMEMBER_HPP
#define PYLLARS_PYLLARS_CLASSSTATICMEMBER_HPP

namespace pyllars{

    /**
     * Explicitly instantiate to add a static attribute to a given Class
     *
     * @tparam name  the name of the static attribute
     * @tparam Class   the class to which the attribute blongs
     * @tparam Attr   the type of the static attribute
     * @tparam attr  pointer to the static attribute
     */
    template<const char *const name, typename Class, typename Attr, Attr* attr>
    class PyllarsClassStaticMember{
    private:
        class Initializer{
        public:
            Initializer(){
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template addStaticAttribute<name, Attr>(attr);
                return 0;
            }
        };

        static Initializer* const initializer;
    };

    template<const char *const name, typename Class, typename Attr, Attr* attr>
    typename PyllarsClassStaticMember<name, Class, Attr, attr>::Initializer * const
            PyllarsClassStaticMember<name, Class, Attr, attr>::initializer = new
                    PyllarsClassStaticMember<name, Class, Attr, attr>::Initializer();
}


#endif
