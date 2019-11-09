//
// Created by jrusnak on 10/13/19.
//

#ifndef PYLLARS_PYLLARS_NAMESPACEWRAPPER_HPP
#define PYLLARS_PYLLARS_NAMESPACEWRAPPER_HPP

#include <sstream>
#include "pyllars.hpp"

namespace pyllars{

    //global ns tag
    struct Tag_GlobalNS{
        typedef const char* const cstring;
        static constexpr cstring name = nullptr;
    };

    class CommonNamespaceWrapper: public __pyllars_internal::CommonBaseWrapper{
    };

    template<typename NSTag, typename Parent, typename Z = void>
    class PyllarsNamespace;

    template<typename NSTag, typename Parent>
    class PyllarsNamespace<NSTag, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value || std::is_void<Parent>::value> >: public CommonNamespaceWrapper{
    public:
        static const char* const name;

        static PyObject* module(){
            if constexpr(std::is_same<Tag_GlobalNS, NSTag>::value){
                static PyObject* module = PyImport_ImportModule("pyllars");
                return module;
            } else {
                static PyObject *mod = nullptr;
                if (!mod) {
                    auto docs = []() -> std::string {
                        static std::string text;
                        if (text.empty()) {
                            std::ostringstream strstream;
                            strstream << "Module corresponding to C++ namespace " << name;
                            text = strstream.str();
                        }
                        return text.data();
                    };

#if PY_MAJOR_VERSION == 3
                    // Initialize Python3 module associated with this namespace
                    static PyModuleDef moddef = {
                            PyModuleDef_HEAD_INIT,
                            name,
                            docs().data(),
                            -1,
                            NULL, NULL, NULL, NULL, NULL
                    };
                    mod = PyModule_Create(&moddef);
#else
                    // Initialize Python2 module associated with this namespace
                mod = Py_InitModule3(name, nullptr, docs().data());
#endif
                }
                return mod;
            }
        }

    private:

        class Initializer{
        public:
            explicit Initializer() {
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                static int status = 0;
                if constexpr (!std::is_void<Parent>::value){
                    if (!PyllarsNamespace<NSTag, Parent>::module() || !Parent::module()) {
                        status = __pyllars_internal::ERR_PYLLARS_ON_CREATE;
                    } else {
                        PyModule_AddObject(Parent::module(), name,
                                           PyllarsNamespace<NSTag, Parent>::module());
                    }
                } else {
                    if (!module()){
                        status = -1;
                    }
                }
                if (status != 0){
                    PyErr_SetString(PyExc_SystemError, "Unable to add object to system");
                }
                return status;
            }
        };
        static  Initializer* const initializer;

    };

    template<typename NSTag, typename Parent>
    typename PyllarsNamespace<NSTag, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value || std::is_void<Parent>::value >  >::Initializer* const
            PyllarsNamespace<NSTag, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value|| std::is_void<Parent>::value > >::initializer =
            new typename PyllarsNamespace<NSTag, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value|| std::is_void<Parent>::value > >::Initializer();
    template<typename NSTag, typename Parent>
    const char* const
            PyllarsNamespace<NSTag, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value|| std::is_void<Parent>::value > >::name = NSTag::name;

    /**
     * specialization for global namespace
     */
    template
    class PyllarsNamespace<Tag_GlobalNS, void, void>;

    using GlobalNamespace = PyllarsNamespace<Tag_GlobalNS, void>;
    
}

#endif //PYLLARS_PYLLARS_NAMESPACEWRAPPER_HPP
