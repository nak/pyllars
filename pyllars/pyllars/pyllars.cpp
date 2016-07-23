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
    for (auto it = _initializers->begin(); it != _initializers->end(); ++ it){
        status |= (*it)();
    }
    return status;
}