//
// Created by jrusnak on 1/3/20.
//

#ifndef PYLLARS_PYLLARS_BASE_CONVERSIONS_IMPL_HPP
#define PYLLARS_PYLLARS_BASE_CONVERSIONS_IMPL_HPP
#include "pyllars_base.hpp"
#include "pyllars_classwrapper.hpp"

namespace pyllars_internal{

    template <typename T>
    template <typename T_element>
    void PythonBaseWrapper<T>::Initializer::addPointers() {
        typedef std::remove_cv_t <T> T_bare;
        if constexpr(std::is_volatile<T>::value &&std::is_const<T>::value) {
            // T_element *  const volatile
            addConversion<T_element * >();
            addConversion<T_element * const >();
            addConversion<T_element * volatile >();
            addConversion<T_element * const volatile >();
            addConversion<T_element * const volatile &>();
            addConversion<T_element * const volatile &&>();
        } else if constexpr(std::is_const<T>::value) {
            // T_element * const
            addConversion<T_element * >();
            addConversion<T_element * volatile >();
            addConversion<T_element * const volatile>();
            addConversion<T_element * const &>();
            addConversion<T_element * const volatile &>();
            addConversion<T_element * const &&>();
            addConversion<T_element * const volatile &&>();
        } else if constexpr(std::is_volatile<T>::value) {
            // T_element * volatile
            addConversion<T_bare>();
            addConversion<T_element * const>();
            addConversion<T_element * const volatile>();
            addConversion<T_element * volatile &>();
            addConversion<T_element * const volatile &>();
            addConversion<T_element * volatile &&>();
            addConversion<T_element * const volatile &&>();
        } else {
            addConversion<T_element *>();
            addConversion<T_element * const >();
            addConversion<T_element * volatile *>();
            addConversion<T_element * const volatile>();
            addConversion<T_element * &>();
            addConversion<T_element * const &>();
            addConversion<T_element * volatile &>();
            addConversion<T_element * const volatile &>();
            addConversion<T_element * &&>();
            addConversion<T_element * const &&>();
            addConversion<T_element * volatile &&>();
            addConversion<T_element * const volatile &&>();
        }
    }

    template <typename T>
    PythonBaseWrapper<T>::Initializer::Initializer() {
        if constexpr (std::is_rvalue_reference<T>::value) {
            typedef std::remove_reference_t<T> T_NoRef;
            typedef std::remove_cv_t <T_NoRef > T_bare;

            addConversion<T_NoRef>();
            if constexpr(std::is_volatile<T>::value && std::is_const<T>::value){
                addConversion<const volatile T_bare &>();
                addConversion<T_bare >();
                addConversion<const T_bare >();
                addConversion<volatile T_bare >();
                addConversion<const volatile T_bare >();
            } else if constexpr(std::is_const<T>::value) {
                addConversion<const T_bare &>();
                addConversion<const volatile T_bare &&>();
                addConversion<const volatile T_bare &>();
                addConversion<T_bare>();
                addConversion<const T_bare>();
                addConversion<const T_bare&>();
            } else if constexpr(std::is_volatile<T>::value){
                addConversion<volatile T_bare &>();
                addConversion<const volatile T_bare &>();
                addConversion<const volatile T_bare &&>();
                addConversion<T_bare>();
                addConversion<const T_bare>();
                addConversion<const volatile T_bare>();
            } else {
                addConversion<T_bare&>();
                addConversion<const T_bare>();
                addConversion<const T_bare&>();
                addConversion<const T_bare&&>();
                addConversion<const volatile T_bare>();
                addConversion<const volatile T_bare&>();
                addConversion<const volatile T_bare&&>();
            }
        } else if (std::is_reference<T>::value) {
            typedef std::remove_reference_t<T> T_NoRef;
            typedef std::remove_cv_t<T_NoRef> T_bare;

            addConversion<T_NoRef>();
            if constexpr(std::is_volatile<T>::value &&std::is_const<T>::value) {
                //const volatile T_bare&
                addConversion<T_bare>();
                addConversion<const T_bare>();
                addConversion<volatile T_bare>();
                addConversion<const volatile T_bare>();
            } else if constexpr(std::is_const<T>::value) {
                //const  & T_bare
                addConversion<T_bare>();
                addConversion<const T_bare>();
                addConversion<volatile T_bare>();
                addConversion<const volatile T_bare>();
                addConversion<const volatile T_bare &>();
            } else if constexpr(std::is_volatile<T>::value) {
                //volatile T_bare&
                addConversion<T_bare>();
                addConversion<const T_bare>();
                addConversion<const volatile T_bare>();
                addConversion<const volatile T_bare&>();
            } else {
                //T_bare &
                addConversion<T_bare>();
                addConversion<const T_bare>();
                addConversion<const volatile T_bare>();
                addConversion<T_bare &&>();
                addConversion<const T_bare &>();
                addConversion<volatile T_bare &>();
                addConversion<const volatile T_bare &>();
            }
        } else if (std::is_pointer<T>::value){
            typedef std::remove_pointer_t<T> T_element;
            typedef std::remove_cv_t<T_element> T_bare_element;
            typedef T_bare_element * T_ptr_bare;

            addPointers<T_element>();
            if constexpr(std::is_volatile<T>::value &&std::is_const<T>::value) {
                // const volatile T_element *
                addPointers<const volatile T_element>();
            } else if constexpr(std::is_const<T>::value) {
                addPointers<const T_element>();
                addPointers<const volatile T_element>();
            } else if constexpr(std::is_volatile<T>::value) {
                addPointers<volatile T_element>();
                addPointers<const volatile T_element>();
            } else {
                addPointers<const volatile T_element>();
            }
        } else if (is_pointer_like<T>::value){ // fixed array types

        } else {
            typedef std::remove_cv_t<T> T_bare;

            if constexpr(std::is_volatile<T>::value &&std::is_const<T>::value) {
                //const volatile T_bare
                addConversion<T_bare>();
                addConversion<const T_bare>();
                addConversion<volatile T_bare>();
                addConversion<const volatile T_bare &>();
                addConversion<const volatile T_bare &&>();
            } else if constexpr(std::is_const<T>::value) {
                //const  T_bare
                addConversion<T_bare>();
                addConversion<volatile T_bare>();
                addConversion<const volatile T_bare>();
                addConversion<const T_bare &>();
                addConversion<const volatile T_bare &>();
                addConversion<const T_bare &&>();
                addConversion<const volatile T_bare &&>();
            } else if constexpr(std::is_volatile<T>::value) {
                //volatile T_bare
                addConversion<T_bare>();
                addConversion<const T_bare>();
                addConversion<const volatile T_bare>();
                addConversion<volatile T_bare&>();
                addConversion<const volatile T_bare&>();
                addConversion<volatile T_bare &&>();
                addConversion<const volatile T_bare &&>();
            } else {
                //T_bare
                addConversion<T_bare>();
                addConversion<const T_bare>();
                addConversion<volatile T_bare>();
                addConversion<const volatile T_bare>();
                addConversion<T_bare &>();
                addConversion<const T_bare &>();
                addConversion<volatile T_bare &>();
                addConversion<const volatile T_bare &>();
                addConversion<T_bare &&>();
                addConversion<volatile T_bare &&>();
                addConversion<const T_bare &&>();
                addConversion<const volatile T_bare &&>();
            }
        }
    }

    template <typename T>
    template <typename To>
    void PythonBaseWrapper<T>::addConversion() {
        static_assert(std::is_convertible<T, To>::value);
        if constexpr (!std::is_same<T, To>::value){
            PyTypeObject* selfType = PythonClassWrapper<T>::getRawType();
            PyTypeObject* toType = PythonClassWrapper<T>::getRawType();
            CommonBaseWrapper::addConversion(selfType, toType,  &interpret_cast<T, To>);
        }
    }

}
#endif //PYLLARS_PYLLARS_BASE_CONVERSIONS_IMPL_HPP
