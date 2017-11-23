
#include "classes.hpp"

namespace trial{

    Main::Main(){
    }

    Main::Main(const Main &){
    }

    Main::~Main(){
    }

    int  Main::varargs_method(double d, ...){
        return (int)d;
    }

    int  Main::const_varargs_method(double d, ...) const{
        return (int)d;
    }

    Main &Main::operator=(const Main & ){
        return *this;
    }

    Main::Inner::Inner(){}


}
