#ifndef __GLOBALFUNCTIONS__
#define __GLOBALFUNCTIONS__

typedef char * str;
typedef const char* const const_str;

extern const char * const const_ptr_str ;
extern const char* const * const_ptr_ptr_str;


namespace outside{
    class ExternalDependency{
    };
}

namespace trial{

  int some_global_function(const double & value, outside::ExternalDependency &d) throw(double );

}

float global_number();

static constexpr long* null_long_ptr = (long*)0;

static long* null_long_ptr2 = (long*)0;

#endif
