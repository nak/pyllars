#ifndef __PYLLARS__CONTAINMENT__H
#define __PYLLARS__CONTAINMENT__H

#include <functional>
#include <Python.h>

#include "pyllars_type_traits.hpp"
#include "pyllars_utils.hpp"

namespace pyllars_internal{

    //////////////////////////////////////
    // Containers for Cobjects in varoious forms.  Containers allow for a wrapper that can
    // convert what would normally be a compil-time error in C to a more Pythonic run-time error
    /////////////////////////////////////



    // CONSTRUCTOR Logic
    // converts what would be a compile-time error for non-constructible occurrences into run-time erorrs
    // for compatibility in Python

    template<typename T>
    struct DLLEXPORT Constructor{
        template<typename ...Args>
        static T* inplace_allocate(T& obj, Args ...args){
            if constexpr (std::is_constructible<T>::value){
                if(std::is_array<T>::value){
                    if (ArraySize<T>::size > 0){
                        for (size_t i = 0; i < ArraySize<T>::size; ++i){
                            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
                            auto objs = (T_element*)obj;
                            Constructor<T_element>::inplace_allocate(objs[i], args...);
                        }
                        return &obj;
                    }
                    throw PyllarsException(PyExc_TypeError, "Request to instantiate non-constructible object");
                } else {
                    return new ((void*)&obj)T(std::forward<typename extent_as_pointer<Args>::type>>(args)...);
                }
            }
            throw PyllarsException(PyExc_TypeError, "Request to instantiate non-constructible object");
        }
/*
        template<typename ...Args>
        static T* allocate(Args ...args){
            if constexpr (sizeof...(args) == 0 && !std::is_default_constructible<T>::value){
                throw PyllarsException(PyExc_TypeError, "Request to default-construct non-cdefault-constructiblt object");
            }
            if constexpr (std::is_constructible<T>::value){
                if(std::is_array<T>::value){
                    if (ArraySize<T>::size > 0){
                        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
                        return new T_element[ArraySize<T>::size]{T(std::forward<typename extent_as_pointer<Args>::type>>(args)...)};
                    }
                    throw PyllarsException(PyExc_TypeError, "Request to instantiate object of unknown size");
                } else {
                    return new T(std::forward<typename extent_as_pointer<Args>::type>>(args)...);
                }
            }
            throw PyllarsException(PyExc_TypeError, "Request to instantiate non-constructible object");
        }
*/

        template<typename ...Args>
        static T* allocate_array(const size_t size, Args ...args){
            if constexpr (std::is_constructible<T>::value){
                if constexpr (std::is_array<T>::value){
                    if constexpr (ArraySize<T>::size > 0){
                        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
                        return (T*)new T_element[size][ArraySize<T>::size]{{T(std::forward<typename extent_as_pointer<Args>::type>>(args)...)}};
                    }
                    throw PyllarsException(PyExc_TypeError, "Request to instantiate object of unknown size");
                } else {
                    return (T*)new T[size]{T(std::forward<typename extent_as_pointer<Args>::type>(args)...)};
                }
            }
            throw PyllarsException(PyExc_TypeError, "Request to instantiate non-constructible object");
        }
    };

    ///////////////////////////////////


}
#endif