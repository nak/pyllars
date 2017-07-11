
#include "classes.hpp"

namespace trial{

    Main::Main(){
    }

    Main::Main(const Main &){
    }

    Main::~Main(){
    }

    Main &Main::operator=(const Main & ){
        return *this;
    }

    Main::Inner::Inner(){}


	float Main::Inner::Tertiary::timestamp(){
    	return 4.2;
	}


    template <typename T, T d>
    void TemplateClass<T, d>::method1(const float& value){
    }

    template <typename T, T d>
    double TemplateClass<T, d>::method2(T& value){
        return 4.2;
    }

    template<> class TemplateClass<int>;


   int some_global_function(const double & value, outside::Dep& d) throw(double ){
    return 42;
   }

}


 const char * const const_ptr_str = 0;
 const char* const * const_ptr_ptr_str = 0;