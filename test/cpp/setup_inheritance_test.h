//
// Created by jrusnak on 3/24/19.
//

#ifndef PYLLARS_SETUP_INHERITANCE_TEST_H
#define PYLLARS_SETUP_INHERITANCE_TEST_H

#include "setup_basic_class.h"

class SetupInheritanceTest : public SetupBasicClass{
protected:
    static void SetUpTestSuite();
};


#endif //PYLLARS_SETUP_INHERITANCE_TEST_H
