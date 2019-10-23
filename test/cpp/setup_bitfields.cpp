//
// Created by jrusnak on 3/24/19.
//

#include "setup_bitfields.h"

#include "pyllars/pyllars.hpp"
#include "pyllars/pyllars_classwrapper.impl.hpp"
#include "pyllars/pyllars_pointer.impl.hpp"
#include "pyllars/pyllars_membersemantics.impl.hpp"
#include "pyllars/pyllars_classmembersemantics.impl.hpp"
#include "pyllars/pyllars_staticfunctionsemantics.impl.hpp"
#include "pyllars/pyllars_conversions.impl.hpp"

#include "class_test_defns.h"

#include "pyllars/pyllars_classmethod.hpp"
#include "pyllars/pyllars_classstaticmethod.hpp"
#include "pyllars/pyllars_classconstructor.hpp"
#include "pyllars/pyllars_classenum.hpp"
#include "pyllars/pyllars_classenumclass.hpp"
#include "pyllars/pyllars_classbinaryoperator.hpp"
#include "pyllars/pyllars_classunaryoperator.hpp"
#include "pyllars/pyllars_classstaticmember.hpp"
#include "pyllars/pyllars_classmember.hpp"
#include "pyllars/pyllars_classbitfield.hpp"
#include "pyllars/pyllars_class.hpp"


typedef const char c_string[];

constexpr c_string bit_name = "bit";

template<>
const char* const __pyllars_internal::_Types<BitFieldContainerClass>::type_name = "BitFieldContainer";

void
SetupBitfields::SetUpTestSuite() {
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
        Class::addBitField<bit_name, unsigned char, 1>(getter, &setter);
        ASSERT_EQ(Class::initialize(), 0);
    }
}