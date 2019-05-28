#ifndef __PYLLARS__CONTAINMENT__H
#define __PYLLARS__CONTAINMENT__H

#include <functional>
#include "pyllars_type_traits.hpp"
#include "pyllars_utils.hpp"

namespace __pyllars_internal{

    //////////////////////////////////////
    // Containers for Cobjects in varoious forms.  Containers allow for a wrapper that can
    // convert what would normally be a compil-time error in C to a more Pythonic run-time error
    /////////////////////////////////////

    namespace {

        /**
         * Helper for dealing with schizoprhenic fixed arrays in c (is it a pointer or an object with contigous memory?)
         * @tparam T: type of element in array
         * @tparam size : size of array
         */
        template<typename T>
        struct FixedArrayHelper;

        template<typename T, size_t size>
        struct FixedArrayHelper<T[size]>{
            typedef T T_array[size];

            void * operator new(const std::size_t count,  T_array& from){
                auto bytes = ::operator new(count);
                T* values = reinterpret_cast<T*>(bytes);
                for(int i = 0; i < size; ++i){
                    new (values+i) T(from[i]);
                }
                return bytes;
            }
            void * operator new(const std::size_t count,  const T* const from){
                auto bytes = ::operator new(count);
                T* values = reinterpret_cast<T*>(bytes);
                for(int i = 0; i < size; ++i){
                    new (values+i) T(from[i]);
                }
                return bytes;
            }

            T_array &values(){
                return *reinterpret_cast<T_array*>(&_data);
            }

            T_array *ptr(){
                return reinterpret_cast<T_array*>(&_data);
            }

            ~FixedArrayHelper(){
                if constexpr (std::is_destructible<T>::value) {
                    T *values = reinterpret_cast<T *>(_data);
                    for (int i = 0; i < size; ++i) {
                        values[i].~T();
                    }
                }
            }

            FixedArrayHelper():_values(reinterpret_cast<T*const>(_data)){
            }

        private:
            unsigned char _data[size*sizeof(T)];
            T *const _values;
        };

    }

    template<typename T, size_t size>
    struct FixedArrayHelper<const T[size]>{
        typedef const T T_array[size];
        typedef T T_nonconst_array[size];

        void * operator new(const std::size_t count,  T_array from){
            auto bytes = ::operator new(count);
            T* values = reinterpret_cast<T*>(bytes);
            for(int i = 0; i < size; ++i){
                new (values+i) T(from[i]);
            }
            return bytes;
        }


        void * operator new(const std::size_t count,  T_nonconst_array from){
            auto bytes = ::operator new(count);
            T* values = reinterpret_cast<T*>(bytes);
            for(int i = 0; i < size; ++i){
                new (values+i) T(from[i]);
            }
            return bytes;
        }

        T_array &values(){
            return *reinterpret_cast<T_array*>(&_data);
        }

        ~FixedArrayHelper(){
            if constexpr (std::is_destructible<T>::value) {
                T *values = reinterpret_cast<T *>(_data);
                for (int i = 0; i < size; ++i) {
                    values[i].~T();
                }
            }
        }

    private:
        unsigned char _data[size*sizeof(T)];
    };


    /////////////////////////////////////////////////////


    // CONSTRUCTOR Logic
    // converts what would be a compile-time error for non-constructible occurrences into run-time erorrs
    // for compatibility in Python

    template<typename T>
    struct Constructor{
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
                    throw "Request to instantiate non-constructible object";
                } else {
                    return new ((void*)&obj)T(std::forward<typename extent_as_pointer<Args>::type>>(args)...);
                }
            }
            throw "Request to instantiate non-constructible object";
        }

        template<typename ...Args>
        static T* allocate(Args ...args){
            if constexpr (sizeof...(args) == 0 && !std::is_default_constructible<T>::value){
                throw "Request to default-construct non-cdefault-constructiblt object";
            }
            if constexpr (std::is_constructible<T>::value){
                if(std::is_array<T>::value){
                    if (ArraySize<T>::size > 0){
                        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
                        return new T_element[ArraySize<T>::size]{T(std::forward<typename extent_as_pointer<Args>::type>>(args)...)};
                    }
                    throw "Request to instantiate object of unknown size";
                } else {
                    return new T(std::forward<typename extent_as_pointer<Args>::type>>(args)...);
                }
            }
            throw "Request to instantiate non-constructible object";
        }


        template<typename ...Args>
        static T* allocate_array(const size_t size, Args ...args){
            if constexpr (std::is_constructible<T>::value){
                if constexpr (std::is_array<T>::value){
                    if constexpr (ArraySize<T>::size > 0){
                        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
                        return new T_element[size][ArraySize<T>::size]{{T(std::forward<typename extent_as_pointer<Args>::type>>(args)...)}};
                    }
                    throw "Request to instantiate object of unknown size";
                } else {
                    return new T[size]{T(std::forward<typename extent_as_pointer<Args>::type>(args)...)};
                }
            }
            throw "Request to instantiate non-constructible object";
        }
    };

    ///////////////////////////////////


}
#endif