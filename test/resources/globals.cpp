
#include "globals.hpp"

namespace trial{

   int some_global_function(const double & value, outside::ExternalDependency& d) throw(double ){
      return 42;
   }

}

float global_numer(){
    return 4.2f;
}

const char * const const_ptr_str = 0;
const char* const * const_ptr_ptr_str = 0;