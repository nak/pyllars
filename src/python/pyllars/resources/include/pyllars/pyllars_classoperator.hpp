//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_classwrapper-operators.impl.hpp"
#include "pyllars/internal/pyllars_methodcallsemantics.impl.hpp"

#ifndef PYLLARS_PYLLARS_CLASSUNARYOPERATOR_HPP
#define PYLLARS_PYLLARS_CLASSUNARYOPERATOR_HPP

namespace pyllars{

    using OpUnaryEnum = pyllars_internal::OpUnaryEnum;
    using OpBinaryEnum = pyllars_internal::OpBinaryEnum;


    template<typename method_t>
    struct OpKind;

    template< typename Class, typename ReturnType>
    struct DLLEXPORT OpKind<ReturnType(Class::*)()>{
        typedef OpUnaryEnum OpEnum;
    };


    template< typename Class, typename ReturnType>
    struct DLLEXPORT OpKind<ReturnType(Class::*)() const>{
        typedef OpUnaryEnum OpEnum;
    };

    template< typename Class, typename ReturnType, typename ArgType>
    struct DLLEXPORT OpKind<ReturnType(Class::*)(ArgType)>{
        typedef OpBinaryEnum OpEnum;
    };


    template< typename Class, typename ReturnType, typename ArgType>
    struct DLLEXPORT OpKind<ReturnType(Class::*)(ArgType) const>{
        typedef OpBinaryEnum OpEnum;
    };

    /**
     * Explicitly instantiate to map a C++ unary or binary operator to a Python Construct
     *
     * @tparam method_t  the class signature of the operator
     * @tparam method  pointer to the operator
     * @tparam kind  One of the pyllars::OpUnaryEnum or pyllars::OpBinaryEnum values, which should be consitent with
     *    the type of operator method being provided
     */
    template <typename method_t, method_t method, typename OpKind<method_t>::OpEnum kind>
    class PyllarsClassOperator;

    template<OpUnaryEnum kind, typename Class, typename ReturnType,  ReturnType(Class::*method)()>
    class DLLEXPORT PyllarsClassOperator<ReturnType(Class::*)(), method, kind>{
    private:
        class Initializer{
        public:
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                PythonClassWrapper<Class>::template Op<kind,  ReturnType(Class::*)(), method>::addUnaryOperator();
                return 0;
            }
        };

        static Initializer* const initializer;

    };

    template<OpUnaryEnum kind, typename Class, typename ReturnType,  ReturnType(Class::*method)()>
    typename PyllarsClassOperator<ReturnType(Class::*)(), method, kind>::Initializer * const
            PyllarsClassOperator<ReturnType(Class::*)(), method, kind>::initializer  = new
                    PyllarsClassOperator<ReturnType(Class::*)(), method, kind>::Initializer();



    template<OpUnaryEnum kind, typename Class, typename ReturnType,  ReturnType(Class::*method)() const>
    class DLLEXPORT PyllarsClassOperator<ReturnType(Class::*)() const, method, kind>{
    private:
        class Initializer{
        public:
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                PythonClassWrapper<Class>::template Op<kind,  ReturnType(Class::*)() const, method>::addUnaryOperator();
                return 0;
            }
        };

        static Initializer* const initializer;

    };

    template<OpUnaryEnum kind, typename Class, typename ReturnType,  ReturnType(Class::*method)() const>
    typename PyllarsClassOperator<ReturnType(Class::*)() const, method, kind>::Initializer * const
            PyllarsClassOperator<ReturnType(Class::*)() const, method, kind>::initializer  = new
                    PyllarsClassOperator<ReturnType(Class::*)() const, method, kind>::Initializer();



    template<OpBinaryEnum kind, typename Class, typename ReturnType,  typename ArgType, ReturnType(Class::*method)(ArgType)>
    class DLLEXPORT PyllarsClassOperator<ReturnType(Class::*)(ArgType), method, kind>{
    private:
        class Initializer{
        public:
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                PythonClassWrapper<Class>::template BinaryOp<kind,  ReturnType(Class::*)(ArgType), method>::addBinaryOperator();
                return 0;
            }
        };

        static Initializer* const initializer;

    };

    template<OpBinaryEnum kind, typename Class, typename ReturnType,  typename ArgType, ReturnType(Class::*method)(ArgType)>
    typename PyllarsClassOperator<ReturnType(Class::*)(ArgType), method, kind>::Initializer * const
            PyllarsClassOperator<ReturnType(Class::*)(ArgType), method, kind>::initializer  = new
                    PyllarsClassOperator<ReturnType(Class::*)(ArgType), method, kind>::Initializer();



    template<OpBinaryEnum kind, typename Class, typename ReturnType,  typename ArgType, ReturnType(Class::*method)(ArgType) const>
    class DLLEXPORT PyllarsClassOperator<ReturnType(Class::*)(ArgType) const, method, kind>{
    private:
        class DLLEXPORT Initializer{
        public:
            Initializer(){
                pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace pyllars_internal;
                PythonClassWrapper<Class>::template BinaryOp<kind,  ReturnType(Class::*)(ArgType) const, method>::addBinaryOperator();
                return 0;
            }
        };

        static Initializer* const initializer;

    };

    template<OpBinaryEnum kind, typename Class, typename ReturnType, typename ArgType, ReturnType(Class::*method)(ArgType) const>
    typename PyllarsClassOperator<ReturnType(Class::*)(ArgType) const, method, kind>::Initializer * const
            PyllarsClassOperator<ReturnType(Class::*)(ArgType) const, method, kind>::initializer  = new
                    PyllarsClassOperator<ReturnType(Class::*)(ArgType) const, method, kind>::Initializer();
}

#endif
