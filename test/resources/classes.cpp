
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

    Main &Main::operator=(const Main & ){
        return *this;
    }

    Main::Inner::Inner(){}

    Main Main2::static_vararg_method(const char* const pattern, ...){
        return Main();
    }

	float Main::Inner::Tertiary::timestamp(){
    	return 4.2;
	}

    void Main2::static_method(const Main2 i){
    }

    double Main2::dbl_static_member = 4.2;

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