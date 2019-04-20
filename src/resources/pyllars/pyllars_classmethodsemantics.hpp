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
    **/
    template<typename func_type>
    class StaticFunctionContainerBase{
    public:

        static PyObject *call(const char* const kwlist[], func_type function, PyObject *cls, PyObject *args, PyObject *kwds) ;
    };

    /**
     * Container for a static function (global function or static class method)
    **/
    template<typename func_type>
    class GlobalFunctionContainer : public StaticFunctionContainerBase<func_type>{
    public:
        GlobalFunctionContainer(func_type *function=nullptr, const char* const kwlist[]=nullptr):_function(function),
        _kwlist(kwlist){}

        PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds) {
            if (!_kwlist || !_function){
                PyErr_SetString(PyExc_SystemError, "Missing initialization of function and/or keyword list when invoking function");
                return nullptr;
            }
            return StaticFunctionContainerBase<func_type>::call(_kwlist, _function, cls, args, kwds);
        }
    private:
        func_type *_function;
        const char* const *_kwlist;

    };

    /**
      * Container for a static function (global function or static class method)
      *
      * @param: CClass: to keep instanitation unique among like methods, different classes
      * @param: name: to keep unique among different methods, same signature
     **/
    template<class CClass, const char* const name, const char* const kwlist[], typename func_type>
    class ClassMethodContainer : public StaticFunctionContainerBase<func_type>{
    public:
        static func_type *function;
        inline static PyObject *call(PyObject *cls, PyObject *args, PyObject *kwds){
            return StaticFunctionContainerBase<func_type>::call(kwlist, function, cls, args, kwds);
        }
    };

}

#endif // __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2

