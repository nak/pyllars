//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_membersemantics.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper-members.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSMEMBER_HPP
#define PYLLARS_PYLLARS_CLASSMEMBER_HPP

namespace pyllars{

    /**
     * Explicitly instatniate to add an attribute to a given Class
     *
     * @tparam name  The name of the attribute
     * @tparam Class  The Class to which the attribute belongs
     * @tparam AttrType  The type of the attribute
     * @tparam attr  Pointer to the class attribute
     */
    template<const char *const name, typename Class, typename AttrType,typename pyllars_internal::MemberContainer<name, Class, AttrType>::member_t attr>
    class DLLEXPORT PyllarsClassMember{
    private:
        class DLLEXPORT Initializer{
        public:
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                PythonClassWrapper<Class>::template addAttribute<name, AttrType>(attr);
                return 0;
            }
        };

        static Initializer * const initializer;
    };

    template<const char *const name, typename Class, typename AttrType, typename pyllars_internal::MemberContainer<name, Class, AttrType>::member_t attr>
    typename PyllarsClassMember<name, Class, AttrType, attr>::Initializer * const
            PyllarsClassMember<name, Class, AttrType, attr>::initializer = new
                    PyllarsClassMember<name, Class, AttrType, attr>::Initializer();
}


#endif
