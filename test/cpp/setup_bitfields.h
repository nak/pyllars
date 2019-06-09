//
// Created by jrusnak on 3/24/19.
//

#ifndef PYLLARS_SETUP_BITFIELDS_H
#define PYLLARS_SETUP_BITFIELDS_H

#include "setup.h"

class SetupBitfields: public PythonBased {
protected:
    static void SetUpTestSuite();

public:
    static void SetUpTestCase(){
        SetUpTestSuite();
    }
};


#endif //PYLLARS_SETUP_BITFIELDS_H
