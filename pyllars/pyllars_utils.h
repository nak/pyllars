#include "pyllars/pyllars_pointer.h"

#ifndef __PYLLARS__INTERNAL__UTILS_H
#define __PYLLARS__INTERNAL__UTILS_H

#include <structmember.h>
#include <memory>

/**
 * Some (perhaps odd) utility classes to aide in converting calls with python tuples
 * to full-blown argument lists used in C function calls
 **/
namespace __pyllars_internal{

    template<typename T>
    struct extent_as_pointer{
      typedef T type;
    };

    template<typename T>
    struct extent_as_pointer<T[]>{
      typedef T* type;
    };

    template<typename T, const size_t max>
    struct extent_as_pointer<T[max]>{
      typedef T* type;
    };

    /**
     * container class for holding a tuple, to be expanded to an argument list
    **/
    template<int ...S>
    struct container {
    };

    /**
     * Counter class to expand arguments from a tuple object
     * to a set of arguments passed to a function
     **/
    template<int N, int ...S>
    struct argGenerator : argGenerator<N-1, N-1, S...> {

    };

    /**
     * 0th order specialization
    **/
    template<int ...S>
    struct argGenerator<0, S...> {
      typedef container<S...> type;

    };

    template <typename T>
    struct smart_delete{

        smart_delete(const bool deleteable):_deleteable(deleteable){
        }

        void operator()(T* ptr) const{
            if(_deleteable) delete ptr;
        }

        const bool _deleteable;
    };

    template <typename T>
    struct smart_delete<T&>{

        smart_delete(const bool deleteable):_deleteable(deleteable){
        }

        void operator()(T* ptr) const{
            if(_deleteable) delete ptr;
        }

        const bool _deleteable;
    };

    template <typename T>
    struct smart_delete<T[]>{

        smart_delete(const bool deleteable):_deleteable(deleteable){
        }

        void operator()(T* ptr) const{
            if(_deleteable) delete [] ptr;
        }

        const bool _deleteable;
    };

    template <typename T>
    using smart_ptr = std::unique_ptr< typename std::remove_reference<T>::type, smart_delete<T>>;

}

#define CLASS_NAME(T) #T

#endif
