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
    class PyllarsNamespace;

    template<const char* const fully_scoped_ns_name, typename Parent>
    class PyllarsNamespace<fully_scoped_ns_name, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value || std::is_void<Parent>::value> >: public CommonNamespaceWrapper{
    public:

        static PyObject* module(){
            if constexpr(fully_scoped_ns_name == nullptr){
                return PyImport_ImportModule("pyllars");
            } else {
                static PyObject *mod = nullptr;
                static std::string full_name = std::string(fully_scoped_ns_name ? fully_scoped_ns_name : "");
                static size_t rfind = full_name.rfind("::");
                static std::string ns_name = rfind == std::string::npos ? full_name : full_name.substr(+2);
                if (!mod) {
                    auto docs = []() -> std::string {
                        static std::string text;
                        if (text.empty()) {
                            std::ostringstream strstream;
                            strstream << "Module corresponding to C++ namespeace" << fully_scoped_ns_name;
                            text = strstream.str();
                        }
                        return text.data();
                    };

#if PY_MAJOR_VERSION == 3
                    // Initialize Python3 module associated with this namespace
                    static PyModuleDef moddef = {
                            PyModuleDef_HEAD_INIT,
                            ns_name.data(),
                            docs(),
                            -1,
                            NULL, NULL, NULL, NULL, NULL
                    };
                    mod = PyModule_Create(&moddef);
#else
                    // Initialize Python2 module associated with this namespace
                mod = Py_InitModule3(ns_name.data(), nullptr, docs());
#endif
                }
                return mod;
            }
        }

    private:

        class Initializer{
        public:
            explicit Initializer(){
                static int status = 0;
                Py_Initialize();
                if constexpr (!std::is_void<Parent>::value){
                    static std::string full_name = std::string(fully_scoped_ns_name?fully_scoped_ns_name:"");
                    static size_t pos = full_name.rfind("::");
                    static std::string ns_name = (pos == std::string::npos)?full_name:full_name.substr(pos+ 2);
                    if (!PyllarsNamespace<fully_scoped_ns_name, Parent>::module() || !Parent::module()) {
                        status = __pyllars_internal::ERR_PYLLARS_ON_CREATE;
                    } else {
                        PyModule_AddObject(Parent::module(), ns_name,
                                           PyllarsNamespace<fully_scoped_ns_name, Parent>::module());
                    }
                } else {
                    if (!module()){
                        status = -1;
                    }
                }
                if (status != 0){
                    PyErr_SetString(PyExc_SystemError, "Unable to add object to system");
                }
            }
        };
        static  Initializer* const initializer;

    };

    template<const char *const fully_scoped_ns_name, typename Parent>
    typename PyllarsNamespace<fully_scoped_ns_name, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value || std::is_void<Parent>::value >  >::Initializer* const
    PyllarsNamespace<fully_scoped_ns_name, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value|| std::is_void<Parent>::value > >::initializer =
            new typename PyllarsNamespace<fully_scoped_ns_name, Parent, std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value|| std::is_void<Parent>::value > >::Initializer();

    /**
     * specialization for global namespace
     */
    template
    class PyllarsNamespace<nullptr, void, void>;


    using GlobalNamespace = PyllarsNamespace<nullptr, void>;
    
}

#endif //PYLLARS_PYLLARS_NAMESPACEWRAPPER_HPP
