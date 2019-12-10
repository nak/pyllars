#ifndef __PYLLARS__CONVERSIONS_H_
#define __PYLLARS__CONVERSIONS_H_

#include <memory>
#include <Python.h>

#include "pyllars_utils.hpp"
#include <utility>
/**
 * This header file defines conversions from Python to C objects
 **/
namespace pyllars_internal {


    /////////////////////////
    // Python-to-C conversions
    //////////////////////////

    /**
     * Class, ONLY TO BE USED IN A LOCAL CONTEXT for capturing a
     * reference to an instance of a type (or in some cases a newly
     * allocated copy to be controlled by this instance)
     * TODO: Investigate using ObjectContainer instead(?)
     * @tparam T : Type of instance reference to capture
     */
    template<typename T>
    struct DLLEXPORT argument_capture{
        typedef typename std::remove_reference<T>::type T_bare;
        static constexpr auto empty_func = [](){};


        /**
         *
         * @param value : reference-to-instance to be captured
         * @param reverse_capture : optional function to be executed once this
         *    instance goes out of scope.  This is most likley to do a reverse
         *    capture of data from a call to a c function back into a Python (list) object,
         *    as C allows modification of its non-const-by-reference inputs to functions
         */
        argument_capture(T& value,  std::function<void()> reverse_capture=empty_func):
                _reverse_capture(reverse_capture), _valueP(nullptr), _value(value){}

        /**
         *
         * @param value : a newly allocated (pointer to) instance to be captured
         * @param array_allocated : whether array-allocation used or not to allocate instance
         * @param revers_capture : as above
         */

        ~argument_capture(){
            _reverse_capture();
            _valueP = nullptr;
        }

        inline std::remove_reference_t <T>& value(){
            return _value;
        }

        inline const std::remove_reference_t <T>& value() const{
            return _value;
        }

        inline T_bare * ptr(){
            return &_value;
        }

    private:
        std::function<void()> _reverse_capture;
        std::remove_reference_t <T>* _valueP;
        std::remove_reference_t <T>& _value;
    };

    /**
     * function to convert python object to underlying C type using a class helper
     **/
    template<typename T >
    argument_capture<T>
    toCArgument(PyObject &pyobj);

    /**
     * function to convert python object to underlying C type using a class helper
     **/
    template<typename T, const size_t size >
    argument_capture<T[size]>
    toCArgument(PyObject &pyobj);

    /**
     * convert C Object to python object
     * @param var: value to convert
     * @param asArgument: whether to be used as argument or not (can determine if copy is made or reference semantics used)
     **/
    template<typename T>
    PyObject *toPyObject(T var, const ssize_t array_size) ;

    template<typename T, ssize_t size>
    PyObject *toPyObject(T var[size], const ssize_t array_size) ;

}

#endif
