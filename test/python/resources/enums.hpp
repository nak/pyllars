enum GlobalEnum {ONE, TWO, THREE};

enum class GlobalEnumClass:int {CL_ONE, CL_TWO, CL_THREE};

struct Struct{
    enum {ONE = 1, TWO = 12, THREE = 23};

    enum class ClassEnum{ONE = 1, TWO, THREE};

};

namespace scoped{

    enum {ONE, TWO, THREE};

    enum class ScopedClassEnum{ONE, TWO, THREE};
}