#include "functions.hpp"

int global_function(const double & const_double_ref_param, short short_param){
    return 42;
}
namespace trial{

    int scoped_function(double * const const_flost_ptr_param){
        return 43;
    }

    struct A& scoped_function_incomplete_return(struct T const & struct_param){
        static A* aptr = (A*)0;
        return *aptr;
    }

}

