#ifndef __PYLLARS_INTERNAL__CLASSMETHODCALLSEMANTICS_H
#define __PYLLARS_INTERNAL__CLASSMETHODCALLSEMANTICS_H

#include <Python.h>
#include <string.h>

#include "pyllars_utils.hpp"
#include "pyllars/internal/pyllars_defns.hpp"
#include "pyllars/internal/pyllars_conversions.hpp"
#include "pyllars_varargs.hpp"

#include <ffi.h>

/**
 * This unit defines template classes needed to contain pointers to static functions and
 * define Python-to-C call semantics for invoking class instance methods.  This can be a global function
 * or a static method of  class.
**/


namespace __pyllars_internal {

    /**
     * class to hold reference to a global or static class method and define
     * method call semantics.  This template is instnatiated solely on the function type and not the
     * (static pointer to) a function instance, so that functions of similar signature share a common instantiation,
     * reducing generated object code.
     *
     * @tparam func_type: the function type (signature), typically specified in form ReturnType(Args...)
     **/
    template<typename func_type>
    class StaticCallSemantics {
    public:

        static constexpr bool has_ellipsis = __pyllars_internal::func_traits<func_type>::has_ellipsis;

        typedef typename __pyllars_internal::func_traits<func_type>::ReturnType ReturnType;


        /**
         * Call the C given (static) function, transforming Python arguments as well as return value
         * @param function : static function to be called
         * @param args : Python args to transform as arguments to C call
         * @param kwds  : Python keyword args to transform as arguments to C call
         * @return Python object transformed from return of C call (oy Py_None if return is void)
         */
        static PyObject *call(func_type function, const char* const kwlist[], PyObject *args, PyObject *kwds) ;

    private:


        /**
         * template call that converts python tuple/dic args into a set of python parameters to make call to
         * invoke given C function
         * @param f: function to invokd
         * @param args: tuple args to convert to C arguments when invoking given function
         * @param kwds: dict args to convert to C arguments when invoking given function
         * @param s: a trick to use to expand the tuple/kwds into a set of Pyobjects to convert to c arguments in an API call
         **/
        template<int ...S>
        static ReturnType invoke(func_type f, const char* const kwlist[],  PyObject *args, PyObject *kwds, container<S...> s) ;

    };


    /**
      * Container for a static function (global function or static class method).  It is instantiated based on
      * function type and function instance, to enforce a one-to-one mapping of the function to a Python call
      * method.
      *
      * @tparam kwlist: a nullptr-terminated list of argument names for the function
      * @tparam func_type: the function type (signature), typically specified in form ReturnType(Args...)
      * @tparam function: the instance of the function this instantiation is associated with
     **/
    template<const char* const kwlist[], typename func_type, func_type function>
    class StaticFunctionContainer{
    public:
        static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

    };

}

#endif // __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2

