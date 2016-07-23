#ifndef __PYLLARS__CONVERSIONS
#define __PYLLARS__CONVERSIONS

#include <memory>
#include <Python.h>

#include "pyllars_classwrapper.hpp"
#include "pyllars_pointer.hpp"
#include "pyllars_utils.hpp"

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
    /**
     * template function to convert python to C object
     **/
    template<typename T, bool is_array, typename ClassWrapper, typename E = void>
    class CObjectConversionHelper;


    /**
     * Specialization for non-fundamental, non-array, non-function types
     **/
    template<typename T, bool is_array, typename ClassWrapper>
    class CObjectConversionHelper<T, is_array, ClassWrapper,
            typename std::enable_if<!std::is_array<T>::value &&
                                    !std::is_function<typename std::remove_pointer<T>::type>::value &&
                                    !std::is_enum<typename std::remove_reference<T>::type>::value &&
                                    !std::is_integral<typename std::remove_reference<T>::type>::value &&
                                    !std::is_floating_point<typename std::remove_reference<T>::type>::value>::type> {
    public:

        typedef typename ClassWrapper::NoRefWrapper NoRefClassWrapper;
        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper ConstClassWrapper;

        typedef typename std::remove_reference<T>::type T_bare;

        static smart_ptr<T_bare, is_array> toCObject(PyObject &pyobj) ;
    };

    /**
     * Specialization for integer types
     **/
    template<typename T, bool is_array, typename ClassWrapper>
    class CObjectConversionHelper<T, is_array, ClassWrapper, typename std::enable_if<
            std::is_integral<typename std::remove_reference<T>::type>::value
            || std::is_enum<typename std::remove_reference<T>::type>::value>::type> {
    public:

        typedef typename ClassWrapper::NoRefWrapper NoRefClassWrapper;
        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper ConstClassWrapper;

        static smart_ptr<T, is_array> toCObject(PyObject &pyobj);
    };

    /**
     * Specialization for floating point types
     **/
    template<typename T, bool is_array, typename ClassWrapper>
    class CObjectConversionHelper<T, is_array, ClassWrapper,
            typename std::enable_if<std::is_floating_point<typename std::remove_reference<T>::type>::value>::type> {
    public:
        typedef typename ClassWrapper::NoRefWrapper NoRefClassWrapper;
        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper ConstClassWrapper;

        static smart_ptr<T> toCObject(PyObject &pyobj) ;
    };

    /**
     * Specialization for callbacks
     **/
    template<typename ClassWrapper, typename ReturnType,
            typename ...Args>
    class CObjectConversionHelper<ReturnType(*)(Args...), false, ClassWrapper, void> {
    public:
        typedef ReturnType(*callback_t)(Args...);
        typedef smart_ptr<callback_t, false> ptr_t;

        static ptr_t toCObject(PyObject &pyobj) ;
    };

    /**
     * Specialization for callbacks
     **/
    template<typename ClassWrapper, typename ReturnType,
            typename ...Args>
    class CObjectConversionHelper<ReturnType(*)(Args..., ...), false, ClassWrapper, void> {
    public:
        typedef ReturnType(*callback_t)(Args..., ...);
        typedef smart_ptr<callback_t, false> ptr_t;
        static ptr_t toCObject(PyObject &pyobj);
    };

    /**
    * Specialization for cosnt char*
    **/
    template<typename ClassWrapper>
    class CObjectConversionHelper<const char *, false, ClassWrapper> {
    public:

        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper ConstClassWrapper;
        typedef  smart_ptr<const char *> ptr_t;
        static ptr_t toCObject(PyObject &pyobj);
    };

    /**
     * Specialization for char* &
     **/
    template<typename ClassWrapper>
    class CObjectConversionHelper<const char *&, false, ClassWrapper> {
    public:

        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper ConstClassWrapper;
        typedef smart_ptr<const char *> ptr_t;
        static ptr_t toCObject(PyObject &pyobj);
    };

    /**
     * Specialization for char*
     **/
    template<typename ClassWrapper>
    class CObjectConversionHelper<const char *const, false, ClassWrapper> {
    public:

        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper ConstClassWrapper;
        typedef smart_ptr<const char *const, false> ptr_t;
        static ptr_t toCObject(PyObject &pyobj);
    };


    /**
     * Specialization for char*
     **/
    template<typename ClassWrapper>
    class CObjectConversionHelper<char *const, true, ClassWrapper> {
    public:
        typedef smart_ptr<char *const, true> ptr_t;
        static ptr_t toCObject(PyObject &pyobj);
    };

    /**
     * Specialization for char*
     **/
    template<typename ClassWrapper>
    class CObjectConversionHelper<char *, true, ClassWrapper> {
    public:
        typedef smart_ptr<char *, true> ptr_t;
        static smart_ptr<char *, true> toCObject(PyObject &pyobj) ;
    };


    /**
     * Specialization for fixed-size array
     **/
    template<typename T, const size_t size, const bool is_array, typename ClassWrapper>
    class CObjectConversionHelper<T[size], is_array, ClassWrapper> {
    public:

        typedef T T_array[size];
        typedef const T Const_T_array[size];
        typedef typename std::remove_const<T>::type NonConst_T_array[size];

        typedef typename ClassWrapper::AsPtrWrapper PtrWrapper;
        // typedef typename ClassWrapper::DereferencedWrapper NonPtrWrapper;
        typedef typename ClassWrapper::NonConstWrapper NonConstArrayWrapper;
        typedef typename ClassWrapper::ConstWrapper ConstArrayWrapper;

        typedef smart_ptr<T[size], is_array> ptr_t;
        static ptr_t toCObject(PyObject &pyobj);
    };

    /**
     * function to convert python object to underlying C type using a class helper
     **/
    template<typename T, bool is_array, typename ClassWrapper>
    smart_ptr<typename std::remove_reference<T>::type, is_array> toCObject(PyObject &pyobj) {
        return CObjectConversionHelper<typename std::remove_reference<T>::type, is_array, ClassWrapper>::toCObject(
                pyobj);
    }

    /**
     * function to convert python object to underlying C type using a class helper
     **/
    template<typename T, const size_t size, bool is_array, typename ClassWrapper>
    smart_ptr<T[size], is_array> toCObject(PyObject &pyobj) {
        return CObjectConversionHelper<T[size], is_array, ClassWrapper>::toCObject(pyobj);
    }

    class ConversionHelpers {
    public:

        template<typename T, typename E>
        friend PyObject *toPyObject(T &var, const bool asReference);
        template<typename T, typename E>
        friend PyObject *toPyObject(T &var, const bool asArgument, const ssize_t array_size, const size_t depth);
        template<typename T, typename E>
        friend PyObject *toPyObject(const T &var, const bool asArgument, const ssize_t array_size, const size_t depth);

    private:

        ///////////
        // Helper conversion functions
        //////////

        /**
         * Define conversion helper class, which allows easier mechanism
         * for necessary specializations
         **/
        template<typename T, typename PtrWrapper, typename E = void>
        class PyObjectConversionHelper;

        /**
         * specialize for non-copiable types
         **/
        template<typename T, typename ClassWrapper>
        class PyObjectConversionHelper<T, ClassWrapper,
                typename std::enable_if<!std::is_integral<T>::value &&
                                        !std::is_enum<T>::value &&
                                        !std::is_floating_point<T>::value>::type> {
        public:
            typedef typename std::remove_reference<T>::type T_NoRef;

            static PyObject *toPyObject(T_NoRef &var, const bool asReference, const ssize_t array_size = -1,
                                        const size_t depth = ptr_depth<T>::value);

        };


        /**
         * specialize for integer types
         **/
        template<typename T, typename ClassWrapper>
        class PyObjectConversionHelper<T, ClassWrapper, typename std::enable_if<
                std::is_integral<T>::value || std::is_enum<T>::value>::type> {
        public:
            static PyObject *toPyObject(const T &var, const bool asReference, const ssize_t array_size = -1,
                                        const size_t depth = 1);
        };

        /**
         * specialize for floating point types
         **/
        template<typename T, typename ClassWrapper>
        class PyObjectConversionHelper<T, ClassWrapper, typename std::enable_if<std::is_floating_point<T>::value>::type> {
        public:
            static PyObject *toPyObject(const T &var, const bool asReference, const ssize_t array_size = -1,
                                        const size_t depth = 1);
        };

        /**
         * Specialized for char*:
         **/
        template<typename ClassWrapper>
        class PyObjectConversionHelper<const char *, ClassWrapper, void> {
        public:
            static PyObject *toPyObject(const char *const &var, const bool asReference, const ssize_t array_size = -1,
                                        const size_t depth = 1);
        };

        template<typename ClassWrapper>
        class PyObjectConversionHelper<char *, ClassWrapper, void> {
        public:
            static PyObject *toPyObject(char *const &var, const bool asReference, const ssize_t array_size = -1,
                                        const size_t depth = 1);
        };

        template<typename ClassWrapper>
        class PyObjectConversionHelper<const char *const, ClassWrapper, void> {
        public:
            static PyObject *toPyObject(const char *const &var, const bool asReference, const ssize_t array_size = -1,
                                        const size_t depth = 1);

        };


    };


    /**
     * convert C Object to python object
     * @param var: value to convert
     * @param asArgument: whether to be used as argument or not (can determine if copy is made or reference semantics used)
     **/
    template<typename T, typename E>
    PyObject *toPyObject(T &var, const bool asArgument, const ssize_t array_size, const size_t depth) ;

    template<typename T, typename E>
    PyObject *toPyObject(const T &var, const bool asArgument, const ssize_t array_size, const size_t depth) ;

}

#endif
