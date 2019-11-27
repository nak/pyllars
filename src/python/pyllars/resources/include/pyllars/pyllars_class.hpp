//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_classwrapper-enums.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"

#ifndef PYLLARS_PYLLARS_CLASSINSTANTITATION_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSINSTANTITATION_IMPL_HPP


namespace pyllars {

    using namespace __pyllars_internal;

    /**
     * Instantiate a given Class, inheriting from all its BaseClass's
     * This class is fully private and is only used to statically instantiate instances that map C++ class construct
     * into the Python ecosystem.  It is, from a client's perspective, a compile-time construct only.
     *
     * @tparam Class  class to instantiate
     * @tparam Parent  Parent (either NSInfo<> instance for a namespace or another Class) that contains the given Class
     * @tparam BaseClass  List of base classes this class inherits from, if any
     */
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
                Init::registerInit(init);
                Init::registerReady(ready);
            }

            static status_t init() {
                if constexpr(is_complete<Parent>::value) {
                    //for incomplete types the following will be evaled and cause compile errors if included in if constexpr above
                    if constexpr(!std::is_base_of<pyllars::NSInfoBase, Parent>::value) {
                        PythonClassWrapper<Parent>::addClassObject(Types<Class>::type_name(),
                                                                   (PyObject*) PythonClassWrapper<Class>::getRawType());
                    }
                }
                return 0;
            }

            template <typename ...CvBaseClass>
            struct ForEach{
            public:
                ForEach() {
		            static std::vector<int> unused {(PythonClassWrapper<typename ApplyCv<CvBaseClass, Class>::type>::template addBaseClass<CvBaseClass>(), 0)...};
		            (void)unused;
                }
            };

            static status_t ready(){
                using namespace __pyllars_internal;
                //add each base class in parameter pack...
                static std::vector<int> unused{(ForEach<BaseClass, const BaseClass, volatile BaseClass, const volatile BaseClass>(), 0)...};
                (void)unused;

                //we are now ready to ready the Python type associated with this class:
                int status = PythonClassWrapper<Class>::template ready<Parent>();
                if constexpr(is_complete<Parent>::value) {
                    //for incomplete types the following will be evaled and cause compile errors if included in if constexpr above
                    if constexpr(std::is_base_of<pyllars::NSInfoBase, Parent>::value) {
                        status |= PyModule_AddObject(Parent::module(), Types<Class>::type_name(),
                                      (PyObject *) PythonClassWrapper<Class>::getPyType());
                    }
                }
                return status;
            }
        };

        static Initializer *const initializer;
    };

    template< typename Class, typename Parent, typename ...BaseClass>
    typename PyllarsClass<Class, Parent, BaseClass...>::Initializer *const
            PyllarsClass<Class, Parent, BaseClass...>::initializer = new typename PyllarsClass<Class, Parent, BaseClass...>::Initializer();


}


#endif
