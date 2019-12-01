//
// Created by jrusnak on 3/24/19.
//

#ifndef PYLLARS_SETUP_BITFIELDS_H
#define PYLLARS_SETUP_BITFIELDS_H

#include "setup.h"
#include "class_test_defns.h"

class DLLEXPORT SetupBitfields: public PythonBased {
protected:
    static void SetUpTestSuite();

public:
    static void SetUpTestCase(){
        SetUpTestSuite();
    }
};

namespace pyllars_internal{

    template<>
    struct TypeInfo<BitFieldContainerClass>{
        static constexpr const char* type_name = "BitFieldConstainer";
    };

}
#endif //PYLLARS_SETUP_BITFIELDS_H
