#ifndef __PYLLARS__CONVERSIONS
#define __PYLLARS__CONVERSIONS

#include <memory>
#include <Python.h>

#include "pyllars_classwrapper.hpp"
#include "pyllars_pointer.hpp"
#include "pyllars_utils.hpp"
#include <utility>
/**
 * This header file defines conversions from Python to C objects
 **/
namespace __pyllars_internal {
    namespace {
        const bool PTR_IS_ALLOCATED = true;
        const bool PTR_IS_NOT_ALLOCATED = false;

        /** for use in asReference/asArgument parameters **/
        constexpr bool AS_ARGUMENT = true;
        constexpr bool AS_REFERNCE = true;
        constexpr bool AS_VARIABLE = false;
    }

    template<typename T>
    struct argument_capture{
        typedef typename std::remove_reference<T>::type T_bare;

        static constexpr auto empty_func = [](){};

        argument_capture(T_bare& value,  std::function<void()> revers_capture=empty_func):
                _reverse_capture(revers_capture), _valueP(nullptr), _value(value), _array_allocated(false){}

        argument_capture(T_bare *value, const bool array_allocated= false,
                std::function<void()> revers_capture=empty_func):
            _reverse_capture(revers_capture), _valueP(value), _value(*_valueP), _array_allocated(array_allocated){}

        ~argument_capture(){
            _reverse_capture();
            if (_valueP){
                if(_array_allocated) delete [] _valueP;
                else delete _valueP;
            }
            _valueP = nullptr;
        }

        T_bare& value(){
            return _value;
        }

        const T_bare& value() const{
            return _value;
        }

    private:
        std::function<void()> _reverse_capture;
        T_bare * _valueP;
        T_bare & _value;
        const bool _array_allocated;
    };

    /**
     * template function to convert python to C object
     **/
    template<typename T>
    class CObjectConversionHelper;


    /**
     * Specialization for function types
     **/
    template<typename T >
    class CObjectConversionHelper {
    public:
        typedef typename std::remove_reference<T>::type T_bare;

        static argument_capture<T> toCArgument(PyObject &pyobj) ;
    };

    /**
     * Specialization for callbacks
     **/
    template<typename ReturnType, typename ...Args>
    class CObjectConversionHelper<ReturnType(*)(Args...)> {
    public:
        typedef ReturnType(*callback_t)(Args...);

        static argument_capture<callback_t > toCArgument(PyObject &pyobj) ;
    };

    /**
     * Specialization for callbacks
     **/
    template<typename ReturnType, typename ...Args>
    class CObjectConversionHelper<ReturnType(*)(Args..., ...)> {
    public:
        typedef ReturnType(*callback_t)(Args..., ...);

        static argument_capture<callback_t> toCArgument(PyObject &pyobj);
    };

    /**
     * function to convert python object to underlying C type using a class helper
     **/
    template<typename T >
    argument_capture<T>
    toCArgument(PyObject &pyobj) {
        return CObjectConversionHelper<T>::toCArgument(pyobj);
    }

    /**
     * function to convert python object to underlying C type using a class helper
     **/
    template<typename T, const size_t size >
    argument_capture<T[size]>
    toCArgument(PyObject &pyobj) {
        return CObjectConversionHelper<T[size] >::toCArgument(pyobj);
    }

    /////////////////////////

    class ConversionHelpers {
    public:

        ///////////
        // Helper conversion functions
        //////////

        /**
         * Define conversion helper class, which allows easier mechanism
         * for necessary specializations
         **/
        template<typename T>
        class PyObjectConversionHelper{
        public:
            typedef PythonClassWrapper<T> ClassWrapper;
            typedef typename std::remove_reference<T>::type T_NoRef;

            static PyObject *toPyObject(T_NoRef &var, const bool asReference, const ssize_t array_size = -1);

        };


    };

    /**
     * convert C Object to python object
     * @param var: value to convert
     * @param asArgument: whether to be used as argument or not (can determine if copy is made or reference semantics used)
     **/
    template<typename T, typename E>
    PyObject *toPyObject(T &var, const bool asArgument, const ssize_t array_size) ;

    template<typename T, typename E>
    PyObject *toPyObject(const T &var, const bool asArgument, const ssize_t array_size) ;

}

#endif
