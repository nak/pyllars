//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_classwrapper-enums.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper-type.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper-staticmethods.impl.hpp"
#include "pyllars/internal/pyllars_conversions.impl.hpp"

#ifndef PYLLARS_PYLLARS_ENUM_HPP
#define PYLLARS_PYLLARS_ENUM_HPP

namespace pyllars{

    /**
     * Explicitly instantiate to map a C-style (non-class) enum to a Python construct
     *
     * @tparam name  name of the enum type; for anonymous enums, just use "<<anonymous>>"
     * @tparam EnumType  the type associated with the enum (can use decltype(enum-value) to get for anonymous type
     * @tparam Parent  either an instantiation of NSInfo<> if inside a namespace, pyllars::GlobalNS or
     *    the Class to which the enum belongs (if inside a class)
     */
    template<const char* name, typename EnumType, typename Parent, const char* const vnames[], EnumType ...values>
    class DLLEXPORT PyllarsEnum{
    public:
    private:

        class DLLEXPORT Initializer{
        public:
            Initializer() {
                pyllars_internal::Init::registerInit(init);
                pyllars_internal::Init::registerReady(ready);
            }
            static status_t init() {
                using namespace pyllars_internal;
                static std::vector<EnumType> evalues{values...};
                unsigned int counter = 0;
                for(auto &v: evalues){
                    PythonClassWrapper<EnumType>::addEnumValue(vnames[counter++], v);
                }

                if constexpr(name != nullptr) {
                    if constexpr (!is_base_of<pyllars::NSInfoBase, Parent>::value) {
                       PythonClassWrapper<Parent>::addStaticType(name, &PythonClassWrapper<EnumType>::getPyType);
                    }
                }

                if constexpr (!is_base_of<pyllars::NSInfoBase, Parent>::value && !is_scoped_enum<EnumType>::value) {
                    unsigned int counter = 0;
                    for (auto &v: evalues) {
                        //since all elements in place for EnumType definition, toPyObject can be called
                        // (and will init the pyhconclasswrapper for EnumType)
                        auto obj = PyObject_Call ((PyObject*)PythonClassWrapper<EnumType>::getPyType(), NULL_ARGS(), nullptr);
                        ((PythonClassWrapper<EnumType>*)obj)->set_CObject(&v);
                        PythonClassWrapper<Parent>::addClassObject(vnames[counter++], obj);
                    }
                }

                return 0;
            }

            static status_t ready(){
                using namespace pyllars_internal;
                if constexpr(name != nullptr) {
                    if constexpr (is_base_of<pyllars::NSInfoBase, Parent>::value) {
                        PyModule_AddObject(Parent::module(), name,
                                           (PyObject *) PythonClassWrapper<EnumType>::getPyType());
                        std::vector<EnumType> evalues{values...};
                        unsigned int counter = 0;
                        for (auto &v: evalues) {
                            PyModule_AddObject(Parent::module(), vnames[counter++], toPyObject(v, 1));
                        }
                    }
                }
                return 0;
            }
        };

        static Initializer * const initializer;
    };

    template<const char * name, typename EnumType, typename Parent, const char* const vnames[], EnumType ...values>
    typename PyllarsEnum<name, EnumType, Parent, vnames, values...>::Initializer * const
       PyllarsEnum<name, EnumType, Parent, vnames, values...>::initializer =
                new typename PyllarsEnum<name, EnumType, Parent, vnames, values...>::Initializer();

}

#endif
