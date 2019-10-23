//
// Created by jrusnak on 10/13/19.
//
#include "pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSCONSTRUCTOR_HPP
#define PYLLARS_PYLLARS_CLASSCONSTRUCTOR_HPP

namespace pyllars{

    template< const char* const kwlist[], typename Class, typename ...Args>
    class PyllarsClassConstructor{
    private:
        class Initializer{
        public:
            Initializer(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template addConstructor<Args...>(kwlist);
            }
        };

        static Initializer * const initializer;
    };

    template< const char* const kwlist[], typename Class, typename ...Args>
    typename PyllarsClassConstructor<kwlist, Class, Args...>::Initializer* const
            PyllarsClassConstructor<kwlist, Class, Args...>::initializer = new
                    PyllarsClassConstructor<kwlist, Class, Args...>::Initializer();


}

namespace __pyllars_internal {

    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addConstructorBase(const char *const kwlist[], constructor_t c) {
        _constructors().push_back(ConstructorContainer(kwlist, c));
    }

    template<typename T>
    template< typename ...Args>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addConstructor(const char *const kwlist[]){
        addConstructorBase(kwlist, &create<Args...>);
    }

}

#endif
