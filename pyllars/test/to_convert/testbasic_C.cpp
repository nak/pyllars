
#include "testbasic_C.hpp"

/**
 * Global variables
 **/
//test global variable
const int data2 = 2;
int data3 = 3;
const char* const string_array[] = {"one", "two", "three"};



//function that manipulates its first parameter
int test::test_function( char* const * const out_msg, const size_t length){
  if( out_msg && length > strlen("test_function")){
    strcpy(*out_msg, "test_function");
  }
  return out_msg?strlen(*out_msg):0;
}


/**
 * Function to make a copy of a C string
 **/
const char* test::copy_string( const char* const s){
  if (!s) return nullptr;
  char* retval = new char[strlen(s)+1];
  strcpy(retval, s);
  return (const char*) retval;
}
