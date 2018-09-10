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
    /**
     * template function to convert python to C object
     **/
    template<typename T, bool array_allocated, typename ClassWrapper, typename E = void>
    class CObjectConversionHelper;


    /**
     * Specialization for non-fundamental, non-array, non-function types
     **/
    template<typename T, bool array_allocated, typename ClassWrapper>
    class CObjectConversionHelper<T, array_allocated, ClassWrapper,
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
        typedef smart_ptr<T_bare, array_allocated> ptr_t;

        static smart_ptr<T_bare, array_allocated> toCObject(PyObject &pyobj) ;
    };

    /**
     * Specialization for integer types
     **/
    template<typename T, bool array_allocated, typename ClassWrapper>
    class CObjectConversionHelper<T, array_allocated, ClassWrapper, typename std::enable_if<
            std::is_integral<typename std::remove_reference<T>::type>::value
            || std::is_enum<typename std::remove_reference<T>::type>::value>::type> {
    public:

        typedef typename ClassWrapper::NoRefWrapper NoRefClassWrapper;
        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper ConstClassWrapper;
        typedef smart_ptr<T, array_allocated> ptr_t;

        static smart_ptr<T, array_allocated> toCObject(PyObject &pyobj);
    };

    /**
     * Specialization for floating point types
     **/
    template<typename T, bool array_allocated, typename ClassWrapper>
    class CObjectConversionHelper<T, array_allocated, ClassWrapper,
            typename std::enable_if<std::is_floating_point<typename std::remove_reference<T>::type>::value>::type> {
    public:
        typedef typename ClassWrapper::NoRefWrapper NoRefClassWrapper;
        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper ConstClassWrapper;
        typedef smart_ptr<T, array_allocated> ptr_t;
        static smart_ptr<T, array_allocated> toCObject(PyObject &pyobj) ;
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
    * Specialization for const char*
    **/
    template<typename ClassWrapper, bool array_allocated>
    class CObjectConversionHelper<const char *, array_allocated, ClassWrapper> {
    public:

        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper ConstClassWrapper;
        typedef  smart_ptr<const char *, array_allocated> ptr_t;
        static ptr_t toCObject(PyObject &pyobj);
    };

    /**
     * Specialization for char* &
     **/
    template<typename ClassWrapper, bool array_allocated>
    class CObjectConversionHelper<const char *&, array_allocated, ClassWrapper> {
    public:

        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper ConstClassWrapper;
        typedef smart_ptr<const char *, array_allocated> ptr_t;
        static ptr_t toCObject(PyObject &pyobj);
    };

    /**
     * Specialization for char*
     **/
    template<typename ClassWrapper, bool array_allocated>
    class CObjectConversionHelper<const char *const, array_allocated, ClassWrapper> {
    public:

        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper ConstClassWrapper;
        typedef smart_ptr<const char *const, array_allocated> ptr_t;
        static ptr_t toCObject(PyObject &pyobj);
    };


    /**
     * Specialization for char*
     **/
    template<typename ClassWrapper, bool array_allocated>
    class CObjectConversionHelper<char *const, array_allocated, ClassWrapper> {
    public:
        typedef smart_ptr<char *const, array_allocated> ptr_t;
        static ptr_t toCObject(PyObject &pyobj);
    };

    /**
     * Specialization for char*
     **/
    template<typename ClassWrapper, bool array_allocated>
    class CObjectConversionHelper<char *, array_allocated, ClassWrapper> {
    public:
        typedef smart_ptr<char *, array_allocated> ptr_t;
        static ptr_t toCObject(PyObject &pyobj) ;
    };


    /**
     * Specialization for fixed-size array
     **/
    template<typename T, const size_t size, const bool array_allocated, typename ClassWrapper>
    class CObjectConversionHelper<T[size], array_allocated, ClassWrapper> {
    public:

        typedef T T_array[size];
        typedef const T Const_T_array[size];
        typedef typename std::remove_const<T>::type NonConst_T_array[size];

        typedef typename ClassWrapper::AsPtrWrapper PtrWrapper;
        // typedef typename ClassWrapper::DereferencedWrapper NonPtrWrapper;
        typedef typename ClassWrapper::NonConstWrapper NonConstArrayWrapper;
        typedef typename ClassWrapper::ConstWrapper ConstArrayWrapper;

        typedef smart_ptr<T[size], array_allocated> ptr_t;
        static ptr_t toCObject(PyObject &pyobj);
    };

    /**
     * function to convert python object to underlying C type using a class helper
     **/
    template<typename T, bool array_allocated, typename ClassWrapper>
    typename CObjectConversionHelper<typename std::remove_reference<T>::type, array_allocated, ClassWrapper>::ptr_t
     toCObject(PyObject &pyobj) {
        return CObjectConversionHelper<typename std::remove_reference<T>::type, array_allocated, ClassWrapper>::toCObject(
                pyobj);
    }

    /**
     * function to convert python object to underlying C type using a class helper
     **/
    template<typename T, const size_t size, bool array_allocated, typename ClassWrapper>
    smart_ptr<T[size], array_allocated> toCObject(PyObject &pyobj) {
        return CObjectConversionHelper<T[size], array_allocated, ClassWrapper>::toCObject(pyobj);
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
