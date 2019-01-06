
#include "globals.hpp"

namespace trial{

   int some_global_function(const double & value, outside::ExternalDependency& d) throw(double ){
      return 42;
   }

}

float global_number(){
    return 4.2f;
}

long long scoped::scoped_function(const scoped::TestClass::InnerTestClass &inner){
    return (long long)inner.value;
}

const char * const const_ptr_str = 0;
const char* const * const_ptr_ptr_str = 0;

scoped::TestClass::InnerTestClass  scoped::TestClass::inner_instance;