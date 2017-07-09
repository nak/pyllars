#ifndef __FUNCTIONS__
#define __FUNCTIONS__
int global_function(const double & const_double_ref_param, short short_param);

struct A;

namespace trial{
    struct T;

    int scoped_function(double * const const_flost_ptr_param);

    struct A& scoped_function_incomplete_return(struct T const & struct_param);

}

#endif