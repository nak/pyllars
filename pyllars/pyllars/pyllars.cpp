#include "pyllars.hpp"

std::vector< pyllars::Initializer::initializer_t> *pyllars::Initializer::_initializers = nullptr;

pyllars::Initializer::Initializer(const initializer_t initializer) {
    if(!_initializers){
        _initializers = new std::vector<initializer_t>();
    }
    _initializers->push_back(initializer);
}


status_t pyllars::Initializer::init(){
    int status = 0;
    if(!_initializers) return 0;
    pyllars_mod = Py_InitModule3("pyllars", nullptr, "Pyllars top-level module");
    if (!pyllars_mod) return 1;
    for (auto it = _initializers->begin(); it != _initializers->end(); ++ it){
        status |= (*it)();
    }
    return status;
}


#include <map>
#include "pyllars_utils.hpp"
#include "pyllars_defns.hpp"

std::map<std::string, size_t> *__pyllars_internal::CommonBaseWrapper::classes;
std::map<std::string, size_t> *__pyllars_internal::CommonBaseWrapper::functions;
