#ifndef __PYLLARS_INTERNAL_DEFNS
#define __PYLLARS_INTERNAL_DEFNS
#include <type_traits>
#include <sys/types.h>

#include "pyllars_utils.h"

namespace __pyllars_internal{

    ///////////
    // Helper conversion functions
    //////////
    template< typename T, bool is_complete, const ssize_t max = -1,  typename E = void>
    PyObject* toPyObject( T &var, const bool asArgument);

    template< typename T, bool is_complete, const ssize_t max = -1,  typename E = void>
    PyObject* toPyObject( const T &var, const bool asArgument);

    template< typename T, bool is_array, bool is_complete>
    smart_ptr<T, is_array> toCObject( PyObject& pyobj );

    struct PythonBase;

    /**
     * Class to define Python wrapper to C class/type
     **/
    template<typename CClass, bool is_complete=true, typename Base=PythonBase, typename Z = void>
    struct PythonClassWrapper;


    template< typename CClass, bool is_complete = true, const ssize_t max=-1,
              typename content_type = typename std::remove_reference<CClass>::type* >
    struct PythonCPointerWrapper;
}
#endif
