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

    struct ZERO_DEPTH;

    template< typename T>
    struct rm_ptr{
        typedef ZERO_DEPTH type;
    };

    template<typename T>
    struct rm_ptr< T*>{
        typedef T type;
    };

    struct PythonBase;

    /**
     * Class to define Python wrapper to C class/type
     **/
    template<typename CClass, bool is_complete=true, const ssize_t last = -1, typename Base=PythonBase, typename Z = void>
    struct PythonClassWrapper;


    /**
     * Base class for all Pyllars C-wrapper objects
     **/
    struct PythonBase{
      PyObject_HEAD /*Per Python API docs*/
      typedef PyTypeObject * TypePtr_t;
      
      static TypePtr_t constexpr TypePtr =  &PyBaseObject_Type;
      
    };

    struct CommonBaseWrapper{

      PythonBase baseClass;

      CommonBaseWrapper():_referenced(nullptr){
      }
    
      void make_reference( PyObject *obj){
        if(_referenced){ Py_DECREF(_referenced); }
        if (obj) {Py_INCREF(obj);}
        _referenced = obj;
        
      }

    protected:
      PyObject* _referenced;
    };

}
#endif
