//
// Created by jrusnak on 3/24/19.
//

#ifndef PYLLARS_SETUP_INHERITANCE_TEST_H
#define PYLLARS_SETUP_INHERITANCE_TEST_H
#include "setup.h"
#include "setup_basic_class.h"

class DLLEXPORT SetupInheritanceTest : public SetupBasicClass{
protected:
    static void SetUpTestSuite();

public:
    static void SetUpTestCase(){
        SetUpTestSuite();
    }
};


#endif //PYLLARS_SETUP_INHERITANCE_TEST_H
