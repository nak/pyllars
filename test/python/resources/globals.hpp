#ifndef __GLOBALFUNCTIONS__
#define __GLOBALFUNCTIONS__
#include <time.h>


typedef char * str;
typedef const char* const const_str;


extern const char * const const_ptr_str ;
extern const char* const * const_ptr_ptr_str;
static constexpr long* null_long_ptr = (long*)0;
static long* null_long_ptr2 = (long*)0;

namespace scoped{
    class TestClass{
    public:
        TestClass( ):long_long_member(51), const_int_member(123){
        }

        float method(const int param_one){
            return 4.2;
        }

        class InnerTestClass{
        public:
            float value;
        };

        long long long_long_member;
        static constexpr float static_const_float_member = 42.1f;
        const int const_int_member;
        static InnerTestClass inner_instance;

        enum {INNER_ONE=1, INNER_TWO, INNER_THREE};
    };

    long long scoped_function(const TestClass::InnerTestClass &inner);

}


namespace outside{
    class ExternalDependency;
}

namespace trial{

    int some_global_function(const double & value, outside::ExternalDependency &d) ;//throw(double );

    static constexpr double double_value = 2.33;
}

float global_number();


enum {FIRST=1, SECOND=2, THIRD=3} anonymous_enum_var;
enum  Enumeration {ZERO, ONE, TWO, THREE};
constexpr Enumeration enum_var = TWO;

#endif
