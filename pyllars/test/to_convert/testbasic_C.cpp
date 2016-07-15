
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
