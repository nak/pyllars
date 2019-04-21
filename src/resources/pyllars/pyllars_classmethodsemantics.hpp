#ifndef __PYLLARS_INTERNAL__CLASSMETHODCALLSEMANTICS_H
#define __PYLLARS_INTERNAL__CLASSMETHODCALLSEMANTICS_H

#include </usr/include/python2.7/Python.h>
#include </usr/include/string.h>
#include </usr/include/sys/types.h>

#include "pyllars_utils.hpp"
#include "pyllars_defns.hpp"
#include "pyllars_conversions.hpp"
#include "pyllars_varargs.hpp"

#include <ffi.h>
/**
* This unit defines template classes needed to contain method pointers and
* define Python-to-C call semantics for invoking class instance methods
**/


namespace __pyllars_internal {

    /**
     * class to hold reference to a global or static class method and define
     * method call semantics
     *
     * @param with ellipsis: whether method has ellipsis
     * @param ReturnType:  ReturnType of the method (can be "void")
     * @param Args: template variable arg list of method arguments (can be empty)
     **/
    template<typename func_type>
    class StaticCallSemantics {
    public:

        static constexpr bool has_ellipsis = __pyllars_internal::func_traits<func_type>::has_ellipsis;

        typedef typename __pyllars_internal::func_traits<func_type>::ReturnType ReturnType;


        /**
         * Call the C given (static) function, transforming Python arguments as well as return value
         * @param method : static function to be called
         * @param args : Python args to transform as arguments to C call
         * @param kwds  : Python keyword args to transform as arguments to C call
         * @return Python object transformed from return of C call (oy Py_None if return is void)
         */
        static PyObject *call(func_type method, const char* const kwlist[], PyObject *args, PyObject *kwds) ;

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
      * Container for a static function (global function or static class method)
      *
      * @param: kwlist: list of char strings that are the parameter names of the arguments underlying the C function
      * @param: function: the C function to call (pointer-to)
     **/
    template<const char* const kwlist[], typename func_type, func_type* function>
    class StaticFunctionContainer{
    public:
        static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds);

    };

}

#endif // __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2

