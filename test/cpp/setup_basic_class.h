//
// Created by jrusnak on 3/24/19.
//

#ifndef PYLLARS_SETUP_BASIC_CLASS_H
#define PYLLARS_SETUP_BASIC_CLASS_H

#include "gtest/gtest.h"
#include "setup.h"
#include "class_test_defns.h"

static const char dbl_ptr_member_name[] = "double_ptr_member";
static const char int_array_member_name[] = "int_array";
static const char const_int_member_name[] = "const_int_value";
static const char method_name[] = "public_method";
static const char static_method_name[] = "static_public_method";
static const char create_bclass_method_name[] = "createBasicClass";
static const char create_bclass2_method_name[] = "createBasicClass2";
static const char class_const_member_name[] = "class_const_member";
static const char class_member_name[] = "class_member";
static const char create_method_name[] = "create_method";
static const char create_const_method_name[] = "create_const_method";
static const char enum_convert_name[] = "to_int";

class DLLEXPORT SetupBasicClass : public PythonBased {
public:
protected:
    static void SetUpTestSuite();

public:
    static void SetUpTestCase(){
        SetUpTestSuite();
    }
};

namespace pyllars_internal {
    template<>
    struct DLLEXPORT TypeInfo<BasicClass> {
        static constexpr const char *type_name = "BasicClass";
    };

    template<>
    struct DLLEXPORT TypeInfo<BasicClass2> {
        static constexpr const char *type_name = "BasicClass2";
    };
    template<>
    struct DLLEXPORT TypeInfo<NonDestructible> {
        static constexpr const char *type_name = "NonDestructible";
    };

    template<>
    struct DLLEXPORT TypeInfo<EnumClass > {
        static constexpr const char *type_name = "EnumClass";
    };
}
#endif //PYLLARS_SETUP_BASIC_CLASS_H
