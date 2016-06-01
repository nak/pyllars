#ifndef test_basics
#define test_basics

#include <stdio.h>
#include <string.h>

//////////////
// TEST FOR VARIOUS C CONCEPTS
//////////////

/**
 * Global variables
 **/
//test global variable
extern constexpr int data1 = 1;
extern const int data2 = 2;
extern int data3 = 3;
extern const char* const string_array[] = {"one", "two", "three"};

typedef const char* const cstringtype;

/**
 * Function to make a copy of a C string
 **/
const char* copy_string( const char* const s){
  if (!s) return nullptr;
  char* retval = new char[strlen(s)+1];
  strcpy(retval, s);
  return (const char*) retval;
}

/**
 * global enum definition
 **/
typedef enum { FIRST=1, SECOND=2, FOURTH=4} Enum_T;

typedef struct{
  double elements[122];
} TypedefedStruct;

struct BitFields{
    unsigned int bitfield1_unsigned_size1:1;
    signed int bitfield2_signed_size3:3;
    union{
        unsigned char bitfield3_anon_union_size4:4;
        union{
            unsigned char bitfield_inner_union_anonymous4:4;
	  union{
	    unsigned long long bitfield_deeep_innter_anonymouts:31;
	  };
        };
    };
    union{
        unsigned short bitfield4_named_field_size7:7;
        unsigned long bitfield5_named_field_size17:17;
    } _subfields;

  struct {
    unsigned _field;
    unsigned short _field2;
  } entry;
};

typedef double double_array_unboudned [];

/**
 * Basic struct declaration
 **/
struct TestStruct{

  struct{ 
    int d[2];
  } array_field[2];

  // Inner enum typedef
  typedef enum { INNER_THIRD=3, INNER_FIFTH=5, INNER_SIXTH=10} InnerEnum_T;

  //class-level const C-style string
  static constexpr cstringtype msg = "Default constructed  TestStruct";

  /**
   * Default and non-default constructor
   **/
  TestStruct( ):str_member(msg), double_member(1.23456789){}
  TestStruct( const char* const  msg2):
    str_member(copy_string(msg2)),double_member(2.34567890){}

  ~TestStruct(){ fprintf(stderr, "\n\n============>Deleted %s\n\n", str_member);}
  /**
   * Method that takes a number of parmaeters and prints them
   **/
  const char* const method_string_return( double double_val,
                                          const int* int_ptr_val,
                                          const float& float_ref_val,
					  const char* const c_string_val,
                                          InnerEnum_T enum_val,
                                          Enum_T outer_enum_val,
                                          TestStruct & t){
    static char str[1024];
    snprintf( str, 1024, "VALUES:\n double value: %.3f\n *(%p):=%d\n float reference: %.4f\n C-string-param: %s\n Inner Enum value: %d\n  Outer Enum Value: %d\n  Test Struct object: %s",
              double_val, int_ptr_val, int_ptr_val?*int_ptr_val:-1,
              float_ref_val, c_string_val,
              (int)enum_val,
              (int)outer_enum_val,
              t.str_member );
    return str;
  }
    static const  double static_double(){ return 1234596834.034;}


  //Const and non-const data members
  const char* const str_member;
  double double_member;

};

//function that manipulates its first parameter
int test_function( char* const * const out_msg, const size_t length){
  if( out_msg && length > strlen("test_function")){
    strcpy(*out_msg, "test_function");
  }
  return out_msg?strlen(*out_msg):0;
}
#endif
