namespace __pyllars_internal{
    template< typename C_type>
    C_type toCObject( PyObject& pyobj);
}

#ifndef __PYLLARS__CONVERSIONS
#define __PYLLARS__CONVERSIONS

#include "pyllars/pyllars_classwrapper.h"
#include "pyllars/pyllars_callbacks.h"

/**
 * This header file defines conversions from Python to C objects
 **/
namespace __pyllars_internal{


    /**
     * template fnction to convert python to C object
     **/
    template <typename C_type, typename E = void>
    class CObjectConversionHelper{
    public:
        static C_type toCObject( PyObject& obj);
    };

    /**
     * Specialization for classes, references and pointer types (except function ptr typedefs)
     **/
    template< typename C_type>
    class CObjectConversionHelper< C_type , typename std::enable_if< std::is_class<C_type>::value || std::is_reference<C_type>::value || (std::is_pointer<C_type>::value && std::is_convertible<C_type, const void*>::value)>::type>{
    public:
        static C_type toCObject( PyObject& pyobj){
            if(&pyobj == nullptr){
                throw "Invalid argument for conversion";
            }
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
    class CObjectConversionHelper<T, typename std::enable_if< std::is_integral<T>::value || (std::is_integral<typename std::remove_reference<T>::type>::value && std::is_reference<T>::value && std::is_const<T>::value)>::type >{
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
    class CObjectConversionHelper<T, typename std::enable_if< std::is_floating_point<T>::value || (std::is_floating_point<typename std::remove_reference<T>::type>::value && std::is_reference<T>::value && std::is_const<T>::value)>::type >{
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
     * Specialization for callbacks
     **/
     template< typename ReturnType,
               typename ...Args >
     class CObjectConversionHelper< ReturnType(*)(Args...), void>{
     public:
        typedef ReturnType(*callback_t)(Args...);

        static callback_t toCObject( PyObject& pyobj){
            if (!PyCallable_Check(&pyobj)){
                throw "Python callback is not callable!";
            }
            return PyCallbackWrapper<ReturnType, Args...>(&pyobj).get_C_callback();
        }
     };

     /**
     * Specialization for char*
     **/
     template<>
     class CObjectConversionHelper<const char*>{
     public:
          static const char* toCObject( PyObject& pyobj){
            if (!PyString_Check(&pyobj)){
                throw "Conversiont o C stgring from non-string Python object!";
            }
            return (const char*)PyString_AS_STRING( &pyobj);
        }
     };

    /**
     * Specialization for char*
     **/
     template<>
     class CObjectConversionHelper<const char* const>{
     public:
          static const char* toCObject( PyObject& pyobj){
            if (!PyString_Check(&pyobj)){
                throw "Conversiont o C stgring from non-string Python object!";
            }
            return (const char*)PyString_AS_STRING( &pyobj);
        }
     };


    /**
     * Specialization for char*
     **/
     template<>
     class CObjectConversionHelper< char* const>{
     public:
          static const char* toCObject( PyObject& pyobj){
            if (!PyString_Check(&pyobj)){
                throw "Conversiont o C stgring from non-string Python object!";
            }
            return (const char*)PyString_AS_STRING( &pyobj);
        }
     };

    /**
     * Specialization for char*
     **/
     template<>
     class CObjectConversionHelper< char* >{
     public:
          static const char* toCObject( PyObject& pyobj){
            if (!PyString_Check(&pyobj)){
                throw "Conversiont o C stgring from non-string Python object!";
            }
            return (const char*)PyString_AS_STRING( &pyobj);
        }
     };
    /**
     * function to convert python object to underlying C type using a class helper
     **/
    template< typename T>
    T toCObject( PyObject& pyobj){
        return CObjectConversionHelper<T>::toCObject(pyobj);
    }

}

#endif
