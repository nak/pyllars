#ifndef __GLOBALFUNCTIONS__
#define __GLOBALFUNCTIONS__
#include <time.h>

typedef char * str;
typedef const char* const const_str;

extern const char * const const_ptr_str ;
extern const char* const * const_ptr_ptr_str;

namespace outside{
    class ExternalDependency{
    public:
        ExternalDependency( ){}

    };
}

namespace trial{

  int some_global_function(const double & value, outside::ExternalDependency &d) throw(double );

    static constexpr double double_value = 2.33;
}

float global_number();

static constexpr long* null_long_ptr = (long*)0;

static long* null_long_ptr2 = (long*)0;

enum {FIRST=1, SECOND=2, THIRD=3} anonymous_enum_var;

enum  Enumeration {ZERO, ONE, TWO, THREE};
constexpr Enumeration enum_var = TWO;

enum class UnsizedClassEnum { A, B, C};

enum class SizedClassEnum: long { A, B, C};


#endif
