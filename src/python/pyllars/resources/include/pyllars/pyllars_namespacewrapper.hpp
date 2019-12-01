//
// Created by jrusnak on 10/13/19.
//

#ifndef PYLLARS_PYLLARS_NAMESPACEWRAPPER_HPP
#define PYLLARS_PYLLARS_NAMESPACEWRAPPER_HPP

#include <sstream>
#include "pyllars.hpp"

namespace pyllars{

    struct DLLEXPORT NSInfoBase{

        static bool has_entry(const char* const name ){
#ifdef _MSC_VER
            return module_list().count(std::string(name)) != 0;
#else
            return module_list.count(std::string(name)) != 0;
#endif
        }

        static PyObject* module(const char* const fully_qualified_name, std::string name){
#ifdef _MSC_VER
#else
#endif
            if (!has_entry(fully_qualified_name)){
                std::ostringstream strstream;
                strstream << "Module corresponding to C++ namespace " << fully_qualified_name;
                auto *docs = new std::string(strstream.str());

#if PY_MAJOR_VERSION == 3
                // Initialize Python3 module associated with this namespace
                auto *moddef = new PyModuleDef{
                        PyModuleDef_HEAD_INIT,
                        (new std::string(name))->c_str(),
                        docs->c_str(),
                        -1,
                        nullptr, nullptr, nullptr, nullptr, nullptr
                };
                auto mod = PyModule_Create(moddef);
                if (!mod){
                    PyErr_Print();
                    fprintf(stderr, "Failed to create module '%s':  '%s'\n\n  %s\n\n", fully_qualified_name, name.c_str(),
                            docs->c_str());
                }
#ifdef _MSC_VER
                module_list()[std::string(fully_qualified_name)] = mod;
#else
                module_list[std::string(fully_qualified_name)] = mod;
#endif

#else
                // Initialize Python2 module associated with this namespace
                    mod = Py_InitModule3(name, nullptr, docs().c_str());
#endif
            }
#ifdef _MSC_VER
            return module_list()[std::string(fully_qualified_name)];
#else
            return module_list[std::string(fully_qualified_name)];
#endif
        }
    private:
#ifdef _MSC_VER
        static std::map<std::string, PyObject*> module_list(){
            static std::map<std::string, PyObject*> map;
            return map;
        }
#else
        static std::map<std::string, PyObject*> module_list;
#endif
    };

    template <const char* const fully_qualified_name>
    struct DLLEXPORT NSInfo: public NSInfoBase{
        static constexpr const char* qualified_name = fully_qualified_name;

        static std::string nsname(){
            std::string name;
            if (name.empty()) {
                if constexpr (!fully_qualified_name) {
                    name = "";
                } else {
                    std::string qname = fully_qualified_name;
                    const size_t pos = qname.rfind("::");
                    name = pos == std::string::npos ? qname : qname.substr(pos + 2);
                }
            }
            return name;
        }

        static PyObject* module(){
            if (fully_qualified_name == nullptr || fully_qualified_name[0] == '\0'){
                return PyImport_ImportModule("pyllars");
            }
            return NSInfoBase::module(fully_qualified_name, nsname().c_str());
        }

    };

    using GlobalNS = NSInfo<nullptr>;


    template<typename NSTag, typename Parent, typename Z = void>
    class PyllarsNamespace;

    template<typename NS, typename Parent>
    class DLLEXPORT PyllarsNamespace<NS, Parent, std::enable_if_t<std::is_base_of<NSInfoBase, NS>::value && (std::is_base_of<NSInfoBase, Parent>::value|| std::is_void<Parent>::value)> >{
    public:
        static PyObject* module() {
            return NS::module();
        }

    private:

        class DLLEXPORT Initializer{
        public:
            explicit Initializer() {
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                int status = 0;
                if (NSInfoBase::has_entry(NS::qualified_name)){
                    return 0;
                }
                if constexpr (!std::is_void<Parent>::value){
                    if (!NS::module() || !Parent::module()) {
                        status = pyllars_internal::ERR_PYLLARS_ON_CREATE;
  		                fprintf(stderr, "Unable to create module '%s' with non-void parent: '%s'\n", NS::nsname().c_str(), Parent::nsname().c_str());
        		        PyErr_SetString(PyExc_SystemError, "Unable to create module");
                    } else {
                        printf("Adding %s to %s\n", NS::nsname().c_str(), Parent::nsname().c_str());
                        status = PyModule_AddObject(Parent::module(), NS::nsname().c_str(), NS::module());
			            if (status != 0){
			                fprintf(stderr, "Unable to add module '%s' to '%s':  %p %p\n", NS::nsname().c_str(), Parent::nsname().c_str(), NS::module(), Parent::module());
			            }
                    }
                } else {
                    if (!module()){
                        fprintf(stderr, "Unable to create module '%s'\n", NS::nsname().c_str());
                        PyErr_SetString(PyExc_SystemError, "Unable to create module ");
                        status = -1;
                    }
                }
                if (status != 0){
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
