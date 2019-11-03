//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSUNARYOPERATOR_HPP
#define PYLLARS_PYLLARS_CLASSUNARYOPERATOR_HPP

namespace pyllars{

    using OpUnaryEnum = __pyllars_internal::OpUnaryEnum;
    using OpBinaryEnum = __pyllars_internal::OpBinaryEnum;


    template<typename method_t>
    struct OpKind;

    template< typename Class, typename ReturnType>
    struct OpKind<ReturnType(Class::*)()>{
        typedef OpUnaryEnum OpEnum;
    };


    template< typename Class, typename ReturnType>
    struct OpKind<ReturnType(Class::*)() const>{
        typedef OpUnaryEnum OpEnum;
    };

    template< typename Class, typename ReturnType, typename ArgType>
    struct OpKind<ReturnType(Class::*)(ArgType)>{
        typedef OpBinaryEnum OpEnum;
    };


    template< typename Class, typename ReturnType, typename ArgType>
    struct OpKind<ReturnType(Class::*)(ArgType) const>{
        typedef OpBinaryEnum OpEnum;
    };


    template <typename method_t, method_t method, typename OpKind<method_t>::OpEnum kind>
    class PyllarsClassOperator;

    template<OpUnaryEnum kind, typename Class, typename ReturnType,  ReturnType(Class::*method)()>
    class PyllarsClassOperator<ReturnType(Class::*)(), method, kind>{
    private:
        class Initializer{
        public:
            Initializer(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template Op<kind,  ReturnType(Class::*)(), method>::addUnaryOperator();
            }
        };

        static Initializer* const initializer;

    };

    template<OpUnaryEnum kind, typename Class, typename ReturnType,  ReturnType(Class::*method)()>
    typename PyllarsClassOperator<ReturnType(Class::*)(), method, kind>::Initializer * const
            PyllarsClassOperator<ReturnType(Class::*)(), method, kind>::initializer  = new
                    PyllarsClassOperator<ReturnType(Class::*)(), method, kind>::Initializer();



    template<OpUnaryEnum kind, typename Class, typename ReturnType,  ReturnType(Class::*method)() const>
    class PyllarsClassOperator<ReturnType(Class::*)() const, method, kind>{
    private:
        class Initializer{
        public:
            Initializer(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template Op<kind,  ReturnType(Class::*)() const, method>::addUnaryOperator();
            }
        };

        static Initializer* const initializer;

    };

    template<OpUnaryEnum kind, typename Class, typename ReturnType,  ReturnType(Class::*method)() const>
    typename PyllarsClassOperator<ReturnType(Class::*)() const, method, kind>::Initializer * const
            PyllarsClassOperator<ReturnType(Class::*)() const, method, kind>::initializer  = new
                    PyllarsClassOperator<ReturnType(Class::*)() const, method, kind>::Initializer();



    template<OpBinaryEnum kind, typename Class, typename ReturnType,  typename ArgType, ReturnType(Class::*method)(ArgType)>
    class PyllarsClassOperator<ReturnType(Class::*)(ArgType), method, kind>{
    private:
        class Initializer{
        public:
            Initializer(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template BinaryOp<kind,  ReturnType(Class::*)(ArgType), method>::addBinaryOperator();
            }
        };

        static Initializer* const initializer;

    };

    template<OpBinaryEnum kind, typename Class, typename ReturnType,  typename ArgType, ReturnType(Class::*method)(ArgType)>
    typename PyllarsClassOperator<ReturnType(Class::*)(ArgType), method, kind>::Initializer * const
            PyllarsClassOperator<ReturnType(Class::*)(ArgType), method, kind>::initializer  = new
                    PyllarsClassOperator<ReturnType(Class::*)(ArgType), method, kind>::Initializer();



    template<OpBinaryEnum kind, typename Class, typename ReturnType,  typename ArgType, ReturnType(Class::*method)(ArgType) const>
    class PyllarsClassOperator<ReturnType(Class::*)(ArgType) const, method, kind>{
    private:
        class Initializer{
        public:
            Initializer(){
                using namespace __pyllars_internal;
                PythonClassWrapper<Class>::template BinaryOp<kind,  ReturnType(Class::*)(ArgType) const, method>::addBinaryOperator();
            }
        };

        static Initializer* const initializer;

    };

    template<OpBinaryEnum kind, typename Class, typename ReturnType, typename ArgType, ReturnType(Class::*method)(ArgType) const>
    typename PyllarsClassOperator<ReturnType(Class::*)(ArgType) const, method, kind>::Initializer * const
            PyllarsClassOperator<ReturnType(Class::*)(ArgType) const, method, kind>::initializer  = new
                    PyllarsClassOperator<ReturnType(Class::*)(ArgType) const, method, kind>::Initializer();
}

namespace __pyllars_internal {

    template<typename T>
    template<OpUnaryEnum kind, typename ReturnType, ReturnType( core_type<T>::type::*method)()>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    template Op<kind, ReturnType( core_type<T>::type::*)(), method>::addUnaryOperator() {
        static const char* const kwlist[1] = {nullptr};
        PythonClassWrapper<T>::_unaryOperators[kind] = (unaryfunc) MethodContainer<kwlist, ReturnType(T::*)(), method>::callAsUnaryFunc;
    }


    template<typename T>
    template<OpUnaryEnum kind, typename ReturnType, ReturnType(core_type<T>::type::*method)() const>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    Op<kind, ReturnType(core_type<T>::type::*)() const, method>::addUnaryOperator() {
        static const char* const kwlist[1] = {nullptr};
        PythonClassWrapper<T>::_unaryOperatorsConst()[kind] = (unaryfunc) MethodContainer<kwlist, ReturnType(CClass::*)() const, method>::callAsUnaryFunc;
    }

    template<typename T>
    template<OpBinaryEnum kind, typename ReturnType, typename ArgType,
            ReturnType(core_type<T>::type::*method)(ArgType)>
    void PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    BinaryOp<kind, ReturnType(core_type<T>::type::*)(ArgType), method>::addBinaryOperator() {
        static constexpr cstring kwlist[] = {"operand", nullptr};
        PythonClassWrapper<T>::_binaryOperators()[kind] = (binaryfunc) MethodContainer<kwlist,  ReturnType(core_type<T>::type::*)(ArgType), method>::callAsBinaryFunc;
    }

    template<typename T>
    template<OpBinaryEnum kind, typename ReturnType, typename ArgType,
            ReturnType(core_type<T>::type::*method)(ArgType) const>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    BinaryOp<kind, ReturnType(core_type<T>::type::*)(ArgType) const, method>::addBinaryOperator() {
        static constexpr cstring kwlist[] = {"operand", nullptr};
        PythonClassWrapper<T>::_binaryOperatorsConst()[kind] = (binaryfunc) MethodContainer< kwlist,  ReturnType(core_type<T>::type::*)(ArgType) const,
                method>::callAsBinaryFunc;
    }
}

#endif
