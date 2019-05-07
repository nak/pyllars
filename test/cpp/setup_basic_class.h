//
// Created by jrusnak on 3/24/19.
//

#ifndef PYLLARS_SETUP_BASIC_CLASS_H
#define PYLLARS_SETUP_BASIC_CLASS_H

#include "gtest/gtest.h"
#include "setup.h"


typedef const char c_string[];

constexpr c_string dbl_ptr_member_name = "double_ptr_member";
constexpr c_string int_array_member_name = "int_array";
constexpr c_string const_int_member_name = "const_int_value";
constexpr c_string method_name = "public_method";
constexpr c_string static_method_name = "static_public_method";
constexpr c_string create_bclass_method_name = "createBasicClass";
constexpr c_string create_bclass2_method_name = "createBasicClass2";
constexpr c_string class_const_member_name = "class_const_member";
constexpr c_string class_member_name = "class_member";
constexpr c_string create_method_name = "create_method";
constexpr c_string create_const_method_name = "create_const_method";
constexpr c_string enum_convert_name = "to_int";

class SetupBasicClass : public PythonBased {
public:
protected:
    static void SetUpTestSuite();
};

#endif //PYLLARS_SETUP_BASIC_CLASS_H
