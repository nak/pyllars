//
// Created by jrusnak on 10/13/19.
//

#ifndef PYLLARS_PYLLARS_NAMESPACEWRAPPER_HPP
#define PYLLARS_PYLLARS_NAMESPACEWRAPPER_HPP

#include <sstream>
#include "pyllars.hpp"

namespace pyllars{

    struct NSInfoBase{};

    template <const char* const fully_qualified_name>
    struct NSInfo: public NSInfoBase{
        static const char * nsname(){
            static const char* name = nullptr;
            if (!name) {
                if constexpr (!fully_qualified_name) {
                    static const char *const pyllars_name = "";
                    name = pyllars_name;
                } else {
                    std::string qname = fully_qualified_name;
                    const size_t pos = qname.rfind("::");
                    static std::string sname = pos == std::string::npos ? qname : qname.substr(pos + 2);
                    name = sname.data();
                }
            }
            return name;
        }

        static PyObject* module(){
            if constexpr(fully_qualified_name == nullptr){
                static PyObject* module = PyImport_ImportModule("pyllars");
                return module;
            } else {
                static PyObject *mod = nullptr;
                if (!mod) {
                    auto docs = []() -> std::string {
                        static std::string text;
                        if (text.empty()) {
                            std::ostringstream strstream;
                            strstream << "Module corresponding to C++ namespace " << fully_qualified_name;
                            text = strstream.str();
                        }
                        return text.data();
                    };

#if PY_MAJOR_VERSION == 3
                    // Initialize Python3 module associated with this namespace
                    static PyModuleDef moddef = {
                            PyModuleDef_HEAD_INIT,
                            nsname(),
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

    };

    using GlobalNS = NSInfo<nullptr>;


    template<typename NSTag, typename Parent, typename Z = void>
    class PyllarsNamespace;

    template<typename NS, typename Parent>
    class PyllarsNamespace<NS, Parent, std::enable_if_t<std::is_base_of<NSInfoBase, NS>::value && (std::is_base_of<NSInfoBase, Parent>::value|| std::is_void<Parent>::value)> >{
    public:
        static PyObject* module() {
            return NS::module();
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
                    if (!NS::module() || !Parent::module()) {
                        status = __pyllars_internal::ERR_PYLLARS_ON_CREATE;
                    } else {
                        PyModule_AddObject(Parent::module(), NS::nsname(), NS::module());
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

    template<typename NS, typename Parent>
    typename PyllarsNamespace<NS, Parent, std::enable_if_t<std::is_base_of<NSInfoBase, NS>::value && (std::is_base_of<NSInfoBase, Parent>::value|| std::is_void<Parent>::value)> >::Initializer* const
            PyllarsNamespace<NS, Parent, std::enable_if_t<std::is_base_of<NSInfoBase, NS>::value && (std::is_base_of<NSInfoBase, Parent>::value|| std::is_void<Parent>::value)> >::initializer =
            new typename PyllarsNamespace<NS, Parent, std::enable_if_t<std::is_base_of<NSInfoBase, NS>::value && (std::is_base_of<NSInfoBase, Parent>::value|| std::is_void<Parent>::value)> >::Initializer();

}

#endif //PYLLARS_PYLLARS_NAMESPACEWRAPPER_HPP
