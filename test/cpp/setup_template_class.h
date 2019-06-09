//
// Created by jrusnak on 3/24/19.
//

#ifndef PYLLARS_SETUP_TEMPLATE_CLASS_H
#define PYLLARS_SETUP_TEMPLATE_CLASS_H

#include "gtest/gtest.h"
#include "setup.h"
#include "template_class_defns.h"

typedef const char c_string[];

extern c_string  single_type_template_name = "SingleTypeClassTemplate";

class SetupTemplateClass : public PythonBased {
public:
protected:
    static void SetUpTestSuite();

public:
    static void SetUpTestCase(){
        SetUpTestSuite();
    }
};


using SingleTypeTemplateClassInt = SingleTypeParamTemplateClass<int>;
using SingleTypeTemplateClassCharPtr = SingleTypeParamTemplateClass<const char*>;

#endif
