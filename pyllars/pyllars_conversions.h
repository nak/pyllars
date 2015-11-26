namespace __pyllars_internal{
    template< typename C_type>
    C_type toCObject( PyObject& pyobj);
}

#ifndef __PYLLARS__CONVERSIONS
#define __PYLLARS__CONVERSIONS

#include "pyllars_classwrapper.h"

namespace __pyllars_internal{

    template <typename C_type, typename E = void>
    class CObjectConversionHelper{
    public:
        static C_type toCObject( PyObject& obj);
    };

    /**
    * template fnction to convert python to C object
    **/
    template< typename C_type>
    class CObjectConversionHelper< C_type , typename std::enable_if<std::is_class<C_type>::value || std::is_reference<C_type>::value || std::is_pointer<C_type>::value >::type>{
    public:
        static C_type toCObject( PyObject& pyobj){
            if(&pyobj == nullptr){
                throw "Invalid argument for conversion";
            }
            PyObject_Print(&pyobj, stderr, 0);
            typedef typename std::remove_reference<C_type>::type C_bare;
            typedef typename std::remove_reference<typename std::remove_pointer<C_type>::type>::type C_base;
            if (PyObject_TypeCheck(&pyobj, &PythonCPointerWrapper<C_base>::Type)){
              return *(C_bare*)reinterpret_cast<PythonCPointerWrapper<C_bare> *>(&pyobj)->ptr();
            }
            if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper< C_bare >::Type) && !PyObject_TypeCheck(&pyobj, &PythonClassWrapper< C_type >::Type)){
               throw "Invalid type converting to C object";
            } else {
                return * (typename std::remove_reference<C_type>::type*)reinterpret_cast<PythonClassWrapper< C_bare >* >(&pyobj)->get_CObject();
            }
            return * (typename std::remove_reference<C_type>::type*)reinterpret_cast<PythonClassWrapper< C_type >* >(&pyobj)->get_CObject();
        }
    };

    /**
     * Specialization for integer types
     **/
    template<typename T>
    class CObjectConversionHelper<T, typename std::enable_if< std::is_integral<T>::value >::type >{
    public:
        static T toCObject( PyObject& pyobj){
        if (PyInt_Check( &pyobj)){
          return PyInt_AsLong( &pyobj );
        } else if (PyLong_Check(&pyobj)){
            return PyLong_AsLong( &pyobj );
        } else if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper<long int>::Type)){
           throw "Invalid type converting to C object";
        }
        return *reinterpret_cast<PythonClassWrapper<long int>* >(&pyobj)->get_CObject();
      }
    };

    /**
     * Specialization for floating point types
     **/
    template<typename T>
    class CObjectConversionHelper<T, typename std::enable_if< std::is_floating_point<T>::value >::type >{
    public:
        static T toCObject( PyObject& pyobj){
        if (PyFloat_Check(&pyobj)){
           return PyFloat_AsDouble( &pyobj );
        } else if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper<double>::Type)){
           throw "Invalid type converting to C object";
        }
        return *reinterpret_cast<PythonClassWrapper<double>* >(&pyobj)->get_CObject();
      }
    };

    /**
     * function to convert python object to underlyin C type
     **/
    template< typename T>
    T toCObject( PyObject& pyobj){
        return CObjectConversionHelper<T>::toCObject(pyobj);
    }

}

#endif
