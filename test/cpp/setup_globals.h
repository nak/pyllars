//
// Created by jrusnak on 3/24/19.
//

#ifndef PYLLARS_SETUP_GLOBALS_H
#define PYLLARS_SETUP_GLOBALS_H

#include "setup.h"

class DLLEXPORT SetupGlobals: public PythonBased {

protected:
    static void SetUpTestSuite();

    static PyObject* pymod;

};


#endif //PYLLARS_SETUP_GLOBALS_H
