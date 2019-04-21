//
// Created by jrusnak on 4/20/19.
//

#ifndef PYLLARS_FUNCTION_DEFNS_HPP
#define PYLLARS_FUNCTION_DEFNS_HPP

void void_return(double & value){
    value = 2.5 * value;
}

long long long_long_return(const int i, double * d){
    return 42;
}

#include <stdarg.h>

void void_return_varargs(double & value, ...){
    va_list argp;
    va_start(argp, value);
    double v = va_arg(argp, double);
    value = v;
    va_end(argp);
}

long long long_long_return_varargs(const int i, double * d, ...){
    va_list argp;
    va_start(argp, d);
    double v = va_arg(argp, double);
    *d = v;
    va_end(argp);
    return -i;
}

#endif //PYLLARS_FUNCTION_DEFNS_HPP
