enum GlobalEnum {ONE, TWO, THREE};

enum class GlobalEnumClass:int {CL_ONE, CL_TWO, CL_THREE};

struct Struct{
    enum {ONE, TWO, THREE};

    enum class ClassEnum{ONE, TWO, THREE};

};

namespace scoped{

    enum {ONE, TWO, THREE};

    enum class ScopedClassEnum{ONE, TWO, THREE};
}