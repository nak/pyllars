//
// Created by jrusnak on 10/13/19.
//
#include "pyllars_namespacewrapper.hpp"

namespace pyllars {

    template<const char *const fully_scoped_ns_name,  typename Parent>
    PyObject *
    PyllarsNamespaceWrapper<fully_scoped_ns_name, Parent,  std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value> >::module() {
        static PyObject *mod = nullptr;
        static std::string full_name = std::string(fully_scoped_ns_name?fully_scoped_ns_name:"");
        static size_t rfind = full_name.rfind("::");
        static std::string ns_name = rfind == std::string::npos?full_name:full_name.substr(+ 2);
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


    template<const char *const fully_scoped_ns_name, typename Parent>
    PyllarsNamespaceWrapper<fully_scoped_ns_name, Parent,  std::enable_if_t<std::is_base_of<CommonNamespaceWrapper, Parent>::value> >::StaticInitializer::StaticInitializer() {
        static int status = 0;
        static std::string full_name = std::string(fully_scoped_ns_name?fully_scoped_ns_name:"");
        static size_t pos = full_name.rfind("::");
        static std::string ns_name = (pos == std::string::npos)?full_name:full_name.substr(pos+ 2);

        if(!PyllarsNamespaceWrapper<fully_scoped_ns_name, Parent>::module() || !Parent::module()){
            status = __pyllars_internal::ERR_PYLLARS_ON_CREATE;
        } else {
            PyModule_AddObject(Parent::module(), ns_name, PyllarsNamespaceWrapper<fully_scoped_ns_name, Parent>::module());
        }

        if (status != 0){
            PyErr_SetString(PyExc_SystemError, "Unable to add object to system");
        }
    }


    template<>
    class PyllarsNamespaceWrapper<nullptr, void, void> {
    public:

        static PyObject* module() {
            return PyImport_ImportModule("pyllars");
        }

    };

}