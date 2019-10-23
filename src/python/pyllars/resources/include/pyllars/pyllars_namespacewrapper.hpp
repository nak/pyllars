//
// Created by jrusnak on 10/13/19.
//

#ifndef PYLLARS_PYLLARS_NAMESPACEWRAPPER_HPP
#define PYLLARS_PYLLARS_NAMESPACEWRAPPER_HPP

#include <sstream>
#include "pyllars.hpp"

namespace pyllars{

    class CommonNamespaceWrapper: public __pyllars_internal::CommonBaseWrapper{
    };

    template<const char* const fully_scoped_ns_name, typename Parent, typename Z = void>
    class PyllarsNamespaceWrapper;

    template<const char* const fully_scoped_ns_name, typename Parent>
    class PyllarsNamespaceWrapper<fully_scoped_ns_name, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value> >: public CommonNamespaceWrapper{
    public:

        static PyObject* module();

    private:

        class StaticInitializer{
        public:
            StaticInitializer();
        };
        static  StaticInitializer* const initializer;

    };

    template<const char *const fully_scoped_ns_name, typename Parent>
    typename PyllarsNamespaceWrapper<fully_scoped_ns_name, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value>  >::StaticInitializer* const
    PyllarsNamespaceWrapper<fully_scoped_ns_name, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value> >::initializer =
            new typename PyllarsNamespaceWrapper<fully_scoped_ns_name, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value> >::StaticInitializer();

    /**
     * specialization for global namespace
     */
    template<>
    class PyllarsNamespaceWrapper<nullptr, void, void>;


    using GlobalNamespaceWrapper = PyllarsNamespaceWrapper<nullptr, void, void>;
    
}

#endif //PYLLARS_PYLLARS_NAMESPACEWRAPPER_HPP
