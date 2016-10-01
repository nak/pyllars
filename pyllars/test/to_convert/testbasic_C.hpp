#ifndef __TEST_BASICS__
#define __TEST_BASICS__

#include <stdio.h>
#include <string.h>
#include <string>
#include <map>
#include <stdio.h>
#include <stdarg.h>
//////////////
// TEST FOR VARIOUS C CONCEPTS
//////////////

/**
 * Global variables
 **/
//test global variable
constexpr int data1 = 1;
extern const int data2;
extern const char *const string_array[];

namespace test_pyllars {

    typedef const char *const cstringtype;

    /**
    * Function to make a copy of a C string
    **/
    const char *copy_string(const char *const s);

    /**
    * global enum definition
    **/
    typedef enum {
        FIRST = 1, SECOND = 2, FOURTH = 4
    } Enum_T;

    /**
     * Class enum
     */
    enum class ClassEnum{
        CE_FIRST=1, CE_SECOND=2, CE_THIRD=3
    };

    // type-defined struct
    typedef struct {
        double elements[122];
    } TypedefedStruct;

    //bitfields
    struct BitFields {
        BitFields() : const_bitfield2_signed_size3(-1) {
            this->bitfield1_unsigned_size1 = 0;
            this->bitfield3_anon_union_size4 = 2;
            this->bitfield_deeep_inner_anonymous = 0x7FFF;
            this->_subfields.bitfield5_named_field_size17 = 0xFF;
            this->entry._field = -22;
            this->entry._field2 = +22;
        }

        unsigned int bitfield1_unsigned_size1:1;
        const signed int const_bitfield2_signed_size3:3;
        //anonymous union
        union {
            unsigned char bitfield3_anon_union_size4:4;
            //nested unions
            union {
                unsigned char bitfield_inner_union_anonymous4:4;
                union {
                    unsigned long long bitfield_deeep_inner_anonymous:31;
                };
            };
        };
        //named union subfield
        union {
            unsigned short bitfield4_named_field_size7:7;
            unsigned long bitfield5_named_field_size17:17;
        } _subfields;

        //named struct subfield
        struct {
            signed _field;
            unsigned short _field2;
        } entry;
        int:32; //anonymous field (no way to manipulate directly, but test it doesn't upset the code derivation
    };

    // function with anonymous function param
    float function_anon_fcn_param( float(*operation)(float, float), const float value1, const float value2);

    //array-declared element
    typedef double double_array_unboudned[];


    /**
    * Basic struct declaration
    **/
    struct TestStruct {
        //nested struct
        struct {
            int d[2];
            struct {
                float f = 9.87654e+32;
            } inner2;
        } array_field[2];

        // Inner enum typedef
        typedef enum {
            INNER_THIRD = 3, INNER_FIFTH = 5, INNER_SIXTH = 10
        } InnerEnum_T;

        //inner class-level const C-style string
        static constexpr cstringtype msg = "Default constructed  TestStruct";

        /**
         * Default and non-default constructor
         **/
        TestStruct() : str_member(msg), double_member(1.23456789) {
            mapping["first"] = "Joe";
            mapping["second"] = "Jane";
        }

        TestStruct(const char *const msg2) :
                str_member(copy_string(msg2)), double_member(2.34567890) {
            mapping["first"] = "Joe";
            mapping["second"] = "Jane";
        }

        TestStruct(const TestStruct & t): str_member(copy_string(t.str_member)),
        double_member(t.double_member){

        }


        ~TestStruct() {
            fprintf(stderr, "\n\n============>Deleted %s\n\n", str_member);
        }

        int operator[](const int & index){
            return index*index;
        }

        //non-const mapping operator
        char *&operator[](const double &value) {
            static char *value_image = new char[1024];
            snprintf(value_image, 1024, "%f", value);
            return value_image;
        }

        double operator+(const double & value){
            return double_member + value;
        }

        double operator-(const double & value){
            return double_member - value;
        }

        double operator*(const double & value){
            return double_member*value;
        }

        double operator/(const double & value){
            return double_member/value;
        }

        int operator %(const long & value){
            return ((int) (double_member*10.0)) % value;
        }

        TestStruct& operator=(const double & value){
            double_member = value;
            return *this;
        }

        //const mapping operator
        const std::string& operator[](const char* const name) const{
            return mapping.at(std::string(name));
        }

        void alloc(int i) { }

        /**
         * Method that takes a number of parameters and prints them
         **/
        const char *const method_string_return(double double_val,
                                               const int *int_ptr_val,
                                               const float &float_ref_val,
                                               const char *const c_string_val,
                                               InnerEnum_T enum_val,
                                               Enum_T outer_enum_val,
                                               TestStruct &t) {
            static char str[1024];
            snprintf(str, 1024,
                     "VALUES:\n double value: %.3f\n *(%p):=%d\n flo"
                             ""
                             ""
                             "at reference: %.4f\n C-string-param: %s\n Inner Enum value: %d\n  Outer Enum Value: %d\n  Test Struct object: %s",
                     double_val, int_ptr_val, int_ptr_val ? *int_ptr_val : -1,
                     float_ref_val, c_string_val,
                     (int) enum_val,
                     (int) outer_enum_val,
                     t.str_member);
            return str;
        }


        static const double static_double() { return 1234596834.034; }


        //Const and non-const data members
        const char *const str_member;
        double double_member;

        //class static members
        static double static_double_member;
        static const double static_const_double_member;

    private:
        std::map<std::string, std::string> mapping;
    };

    struct InheritedStruct: public TestStruct{

        InheritedStruct():
                TestStruct(),
                inherited_value(214){
        }

        long get_inherited_value(){
            return inherited_value;
        }

        static int method_with_varargs(int first_arg, ...);
        static void  method_with_varargs_with_void_return(int first_arg, ...);

        long inherited_value;
    };

    //test of template instantiation
    template<typename type, type value>
    class TemplatedClass{
    public:
        static constexpr type templated_type_element = value;

    };

    template<typename type>
    class TemplatedClass2{
    public:
        TemplatedClass2():value("template2"){

        }
        type value;
    };

    template<typename type, type value> constexpr type
    TemplatedClass<type, value>::templated_type_element;

    typedef TemplatedClass<int, 641> ClassInstantationInt;

    //function that manipulates its first parameter
    int test_function(char *const *const out_msg, const size_t length);

    int var_arg_param_func(int val);

    //function with var args
    const char* function_var_args( float first_explicit, int second_explicit, ...);

    //function with var args and void return
    void function_var_args_void_return( float first_explicit, int second_explicit, ...);

}
template class test_pyllars::TemplatedClass<int, 641>;
template class test_pyllars::TemplatedClass2<std::string>;
template class test_pyllars::TemplatedClass2<test_pyllars::TestStruct>;
#endif
