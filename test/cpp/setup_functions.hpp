//
// Created by jrusnak on 3/24/19.
//

#ifndef PYLLARS_SETUP_FUNCTIONS_HPP
#define PYLLARS_SETUP_FUNCTIONS_HPP
#include "setup.h"


class SetupFunctions: public PythonBased {
public:
    static void SetUpTestSuite();

public:
    static void SetUpTestCase(){
        SetUpTestSuite();
    }
};
#endif //PYLLARS_SETUP_STUFF_HPP
