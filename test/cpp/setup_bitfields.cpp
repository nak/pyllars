//
// Created by jrusnak on 3/24/19.
//

#include "setup_bitfields.h"

#include "pyllars/pyllars.hpp"
#include "pyllars/pyllars_classwrapper.impl.hpp"
#include "pyllars/pyllars_pointer.impl.hpp"
#include "pyllars/pyllars_membersemantics.impl.hpp"
#include "pyllars/pyllars_classmembersemantics.impl.hpp"
#include "pyllars/pyllars_classmethodsemantics.impl.hpp"
#include "pyllars/pyllars_conversions.impl.hpp"

#include "class_test_defns.h"

typedef const char c_string[];

constexpr c_string bit_name = "bit";

template<>
const char* const __pyllars_internal::_Types<BitFieldContainerClass>::type_name = "BitFieldContainer";

void
SetupBitfields::SetUpTestSuite() {
    static bool inited = false;
    if (inited) return;
    inited = true;
    PythonBased::SetUpTestSuite();
    using namespace __pyllars_internal;
    static const char *const empty_list[] = {nullptr};
    {
        typedef PythonClassWrapper<BitFieldContainerClass> Class;
        Class::addConstructor<>(empty_list);
        __pyllars_internal::BitFieldContainer<BitFieldContainerClass>::Container<bit_name, unsigned char, 1>::getter_t getter =
                [](const BitFieldContainerClass &c) -> unsigned char { return c.bit; };
        __pyllars_internal::BitFieldContainer<BitFieldContainerClass>::Container<bit_name, unsigned char, 1>::setter_t setter =
                [](BitFieldContainerClass &c, const unsigned char &value) -> unsigned char {
                    c.bit = value;
                    return value;
                };
        Class::addBitField<bit_name, unsigned char, 1>(getter, setter);
        ASSERT_EQ(Class::initialize(), 0);
    }
}