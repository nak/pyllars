//
// Created by jrusnak on 3/24/19.
//

#include "setup_bitfields.h"

#include "pyllars/pyllars.hpp"
#include "pyllars/internal/pyllars_classwrapper.impl.hpp"
#include "pyllars/internal/pyllars_classwrapper-type.impl.hpp"
#include "pyllars/internal/pyllars_pointer.impl.hpp"
#include "pyllars/internal/pyllars_membersemantics.impl.hpp"
#include "pyllars/internal/pyllars_classmembersemantics.impl.hpp"
#include "pyllars/internal/pyllars_staticfunctionsemantics.impl.hpp"
#include "pyllars/internal/pyllars_conversions.impl.hpp"


#include "pyllars/pyllars_classmethod.hpp"
#include "pyllars/pyllars_classstaticmethod.hpp"
#include "pyllars/pyllars_classconstructor.hpp"
#include "pyllars/pyllars_enum.hpp"
#include "pyllars/pyllars_classoperator.hpp"
#include "pyllars/pyllars_classstaticmember.hpp"
#include "pyllars/pyllars_classmember.hpp"
#include "pyllars/pyllars_classbitfield.hpp"
#include "pyllars/pyllars_class.hpp"

namespace {
    const char bit_name[] = "bit";
    const char *const empty_list[] = {nullptr};
}


void
SetupBitfields::SetUpTestSuite() {
    PythonBased::SetUpTestSuite();
    using namespace pyllars_internal;
    {
        typedef PythonClassWrapper<BitFieldContainerClass> Class;
        Class::addConstructor<empty_list>();
        pyllars_internal::BitFieldContainer<BitFieldContainerClass>::Container<bit_name, unsigned char, 1>::getter_t getter =
                [](const BitFieldContainerClass &c) -> unsigned char { return c.bit; };
        pyllars_internal::BitFieldContainer<BitFieldContainerClass>::Container<bit_name, unsigned char, 1>::setter_t setter =
                [](BitFieldContainerClass &c, const unsigned char &value) -> unsigned char {
                    c.bit = value;
                    return value;
                };
        Class::addBitField<bit_name, unsigned char, 1>(getter, &setter);
        ASSERT_EQ(Class::initialize(), 0);
    }
}