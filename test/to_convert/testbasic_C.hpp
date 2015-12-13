#include <stdio.h>
#include <string.h>

//test global variable
constexpr int data1 = 1;
const int data2 = 2;
int data3 = 3;

//basic struct declaration
struct TestStruct{

  static constexpr char* const msg = "Default constructed  message for TestCalss";
  TestStruct( ):str_member(msg){}
  TestStruct( const char* const  msg):str_member(msg){}

  const char* const method_string_return( double double_val, const int* int_ptr_val, const float& float_ref_val, 
					  const char* const c_string_val){
    static char str[1024];
    snprintf( str, 1024, "VALUES: %f %p:%d %f %s", double_val, int_ptr_val, int_ptr_val?*int_ptr_val:-1, float_ref_val, c_string_val);
    return str;
  }

  const char* const str_member;
  double double_member;

};

//function
int test_function( char* const * const out_msg, const size_t length){
  if( out_msg && length > strlen("test_function")){
    strcpy(*out_msg, "test_function");
  }
  return 42;
}

