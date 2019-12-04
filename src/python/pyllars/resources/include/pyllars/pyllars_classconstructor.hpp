//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_classwrapper-xtors.impl.hpp"

#ifndef PYLLARS_PYLLARS_CLASSCONSTRUCTOR_HPP
#define PYLLARS_PYLLARS_CLASSCONSTRUCTOR_HPP

namespace pyllars{

    /**
     * Explicitly instantiate this class to add a constructor to the Python definition of a Class, at compile-time
     *
     * @tparam kwlist  nullptr-terminated list of keywoard names for the args;  length should match # of Args
     * @tparam Class  the Class to add the constructor to
     * @tparam Args  list of argment types definitiong the signature of the constructor
     */
    template< const char* const kwlist[], typename Class, typename ...Args>
    class DLLEXPORT PyllarsClassConstructor{
    private:
        class DLLEXPORT Initializer{
        public:
            Initializer() {
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                PythonClassWrapper<Class>::template addConstructor<kwlist, Args...>();
                PythonClassWrapper<const Class>::template addConstructor<kwlist, Args...>();
                PythonClassWrapper<volatile Class>::template addConstructor<kwlist, Args...>();
                PythonClassWrapper<const volatile Class>::template addConstructor<kwlist, Args...>();
                return 0;
            }

        };

        static Initializer * const initializer;
    };

    template< const char* const kwlist[], typename Class, typename ...Args>
    typename PyllarsClassConstructor<kwlist, Class, Args...>::Initializer* const
            PyllarsClassConstructor<kwlist, Class, Args...>::initializer = new
                    typename PyllarsClassConstructor<kwlist, Class, Args...>::Initializer();


}


#endif
