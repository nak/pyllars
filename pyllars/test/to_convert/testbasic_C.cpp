
#include "testbasic_C.hpp"

/**
 * Global variables
 **/
//test global variable
const int data2 = 2;
int data3 = 3;
const char* const string_array[] = {"one", "two", "three"};


double test_pyllars::TestStruct::static_double_member = -9.87654321;
const double test_pyllars::TestStruct::static_const_double_member = -9.090909;

constexpr test_pyllars::cstringtype test_pyllars::TestStruct::msg;

//function that manipulates its first parameter
int test_pyllars::test_function( char* const * const out_msg, const size_t length){
  if( out_msg && length > strlen("test_function")){
    strcpy(*out_msg, "test_function");
  }
  return out_msg?strlen(*out_msg):0;
}


/**
 * Function to make a copy of a C string
 **/
const char* test_pyllars::copy_string( const char* const s){
  if (!s) return nullptr;
  char* retval = new char[strlen(s)+1];
  strcpy(retval, s);
  return (const char*) retval;
}

//function with var args
int test_pyllars::function_var_args( float first_explicit, int second_explicit, ...){
  va_list argp;
  va_start(argp, second_explicit);
  int ival = va_arg(argp, int);
  long lval = va_arg(argp, long);
  double dval = va_arg(argp, double);
  const char*  stringval = va_arg(argp, const char*);
  const TestStruct* tsval = va_arg(argp, TestStruct*);
  va_end(argp);
  char return_string[120] = {0};
  snprintf(return_string, 120, "%d %ld %f %s %f", ival, lval, dval, stringval, tsval->double_member );
  return 9;
}


float test_pyllars::function_anon_fcn_param( float(*operation)(float, float), const float value1, const float value2){
  return operation(value1, value2);
}