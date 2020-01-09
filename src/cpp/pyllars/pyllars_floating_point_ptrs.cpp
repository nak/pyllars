
//
// Created by johnj on 11/29/2019.
//
#include <pyllars/internal/pyllars_pointer-type.impl.hpp>
#include "pyllars/internal/pyllars_classwrapper-type.impl.hpp"
#include "pyllars/internal/pyllars_pointer.impl.hpp"
#include <pyllars/internal/pyllars_pointer-createAllocatedInstance.impl.hpp>
#include "pyllars/internal/pyllars_reference.impl.hpp"
#include "pyllars/internal/pyllars_integer.hpp"
#include "pyllars/pyllars_class.hpp"

namespace pyllars_internal{

    template
    struct DLLEXPORT PythonClassWrapper<float&&> ;

    template
    struct DLLEXPORT PythonClassWrapper<double&&>;

    template
    struct DLLEXPORT PythonClassWrapper<const float&&>;

    template
    struct DLLEXPORT PythonClassWrapper<const double&&>;


    template
    struct DLLEXPORT PythonClassWrapper<volatile float&&>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile double&&>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile float&&>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile double&&>;


    template
    struct DLLEXPORT PythonClassWrapper<float&> ;

    template
    struct DLLEXPORT PythonClassWrapper<double&>;

    template
    struct DLLEXPORT PythonClassWrapper<const float&>;

    template
    struct DLLEXPORT PythonClassWrapper<const double&>;


    template
    struct DLLEXPORT PythonClassWrapper<volatile float&>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile double&>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile float&>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile double&>;




    template
    struct DLLEXPORT PythonClassWrapper<float*> ;

    template
    struct DLLEXPORT PythonClassWrapper<double*>;

    template
    struct DLLEXPORT PythonClassWrapper<const float*>;

    template
    struct DLLEXPORT PythonClassWrapper<const double*>;


    template
    struct DLLEXPORT PythonClassWrapper<volatile float*>;

    template
    struct DLLEXPORT PythonClassWrapper<volatile double*>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile float*>;

    template
    struct DLLEXPORT PythonClassWrapper<const volatile double*>;

}

namespace pyllars{

    template
    class PyllarsClass<float, GlobalNS >;

    template
    class PyllarsClass<double, GlobalNS >;

    template
    class PyllarsClass<const float, GlobalNS >;

    template
    class PyllarsClass<const double, GlobalNS >;

    template
    class PyllarsClass<volatile float, GlobalNS >;

    template
    class PyllarsClass<volatile double, GlobalNS >;

    template
    class PyllarsClass<const volatile float, GlobalNS >;

    template
    class PyllarsClass<const volatile double, GlobalNS >;

    template
    class PyllarsClass<float*, GlobalNS >;

    template
    class PyllarsClass<double*, GlobalNS >;

    template
    class PyllarsClass<const float*, GlobalNS >;

    template
    class PyllarsClass<const double*, GlobalNS >;

    template
    class PyllarsClass<volatile float*, GlobalNS >;

    template
    class PyllarsClass<volatile double*, GlobalNS >;

    template
    class PyllarsClass<const volatile float*, GlobalNS >;

    template
    class PyllarsClass<const volatile double*, GlobalNS >;


    template
    class PyllarsClass<float&, GlobalNS >;

    template
    class PyllarsClass<double&, GlobalNS >;

    template
    class PyllarsClass<const float&, GlobalNS >;

    template
    class PyllarsClass<const double&, GlobalNS >;

    template
    class PyllarsClass<volatile float&, GlobalNS >;

    template
    class PyllarsClass<volatile double&, GlobalNS >;

    template
    class PyllarsClass<const volatile float&, GlobalNS >;

    template
    class PyllarsClass<const volatile double&, GlobalNS >;


    template
    class PyllarsClass<float&&, GlobalNS >;

    template
    class PyllarsClass<double&&, GlobalNS >;

    template
    class PyllarsClass<const float&&, GlobalNS >;

    template
    class PyllarsClass<const double&&, GlobalNS >;

    template
    class PyllarsClass<volatile float&&, GlobalNS >;

    template
    class PyllarsClass<volatile double&&, GlobalNS >;

    template
    class PyllarsClass<const volatile float&&, GlobalNS >;

    template
    class PyllarsClass<const volatile double&&, GlobalNS >;


}