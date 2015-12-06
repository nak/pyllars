
#ifndef __PYLLARS__CONVERSIONS
#define __PYLLARS__CONVERSIONS
#include <memory>

#include "pyllars/pyllars_classwrapper.h"
#include "pyllars/pyllars_callbacks.h"
#include "pyllars/pyllars_utils.h"

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
        static smart_ptr<C_type> toCObject( PyObject& obj);
    };


    /**
     * Specialization for classes, references and pointer types (except function ptr typedefs)
     **/
    template< typename C_type>
    class CObjectConversionHelper< C_type , typename std::enable_if< std::is_class<C_type>::value || (std::is_reference<C_type>::value && ! std::is_const<typename std::remove_reference<C_type>::type>::value)|| (std::is_pointer<C_type>::value && std::is_convertible<C_type, const void*>::value)>::type>{
    public:

        static smart_ptr<C_type> toCObject( PyObject& pyobj){
           if(&pyobj == nullptr){
                throw "Invalid argument for conversion";
            }
            typedef typename std::remove_reference<C_type>::type C_bare;
            typedef typename std::remove_reference<typename std::remove_pointer<C_type>::type>::type C_base;
            static constexpr ssize_t depth = ptr_depth<C_base>::value;
             if (PyObject_TypeCheck(&pyobj, &PythonCPointerWrapper<C_base>::Type[depth])){
              return smart_ptr<C_type>((C_bare*)reinterpret_cast<PythonCPointerWrapper<C_base> *>(&pyobj)->ptr(), false);
            }

            if (PyObject_TypeCheck(&pyobj, &PythonClassWrapper<typename std::remove_const<C_bare>::type >::Type)){
              return smart_ptr<C_type>((typename std::remove_const<C_bare>::type*)reinterpret_cast<PythonCPointerWrapper<C_bare> *>(&pyobj)->ptr(), false);
            }
            if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper< C_bare >::Type) && !PyObject_TypeCheck(&pyobj, &PythonClassWrapper< C_type >::Type)){
                fprintf(stderr, "\n");
                PyObject_Print( &pyobj, stderr, 0);
                fprintf(stderr, "\n");
                PyObject_Print(  PyObject_Type(&pyobj), stderr, 0);
                fprintf(stderr, "\n");
                throw "Invalid type converting to C object";
            } else {
                return smart_ptr<C_type>((typename std::remove_reference<C_type>::type*)reinterpret_cast<PythonClassWrapper< C_bare >* >(&pyobj)->get_CObject(), false);
            }
            return smart_ptr<C_type>((typename std::remove_reference<C_type>::type*)reinterpret_cast<PythonClassWrapper< C_type >* >(&pyobj)->get_CObject(), false);
        }
    };

    /**
     * Specialization for integer types
     **/
    template<typename T>
    class CObjectConversionHelper<T, typename std::enable_if< std::is_integral<T>::value ||
            ( std::is_integral<typename std::remove_reference<T>::type>::value &&
                std::is_reference<T>::value &&
                std::is_const<typename std::remove_reference<T>::type>::value )>::type >{
    public:
      static smart_ptr<T> toCObject( PyObject& pyobj){
            typedef typename std::remove_reference< typename std::remove_const<T>::type>::type T_bare;
            if (PyInt_Check( &pyobj)){
               T_bare* value = new T_bare(PyInt_AsLong(&pyobj));
               return smart_ptr<T>(value, true);
            } else if (PyLong_Check(&pyobj)){
                T_bare* value = new T_bare( PyLong_AsLongLong( &pyobj ) );
                return smart_ptr<T>(value, false);
            } else if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper<T_bare>::Type)){
                throw "Invalid type converting to C object";
            }
            return smart_ptr<T>(reinterpret_cast<PythonClassWrapper<T_bare>* >(&pyobj)->get_CObject(), false);
      }
    };

    /**
     * Specialization for floating point types
     **/
    template<typename T>
    class CObjectConversionHelper<T, typename std::enable_if< std::is_floating_point<T>::value ||
       ( std::is_floating_point<typename std::remove_reference<T>::type>::value &&
         std::is_reference<T>::value &&
         std::is_const<typename std::remove_reference<T>::type>::value)>::type >{
    public:
      static smart_ptr<T> toCObject( PyObject& pyobj){
        typedef typename std::remove_reference< T>::type T_bare;
        if (PyFloat_Check(&pyobj)){
            return smart_ptr<T>( new T_bare(PyFloat_AsDouble( &pyobj )), true);
        } else if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper<T_bare>::Type)){
            PyObject_Print(&pyobj,stderr, 0);
            throw "Invalid type converting to C object";
        }
        T_bare* retval = (reinterpret_cast<PythonClassWrapper<T_bare>* >(&pyobj)->get_CObject());
        return smart_ptr<T>(retval, false);
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

        static smart_ptr<callback_t> toCObject( PyObject& pyobj){
            if (!PyCallable_Check(&pyobj)){
                throw "Python callback is not callable!";
            }
            callback_t * retval = new callback_t(PyCallbackWrapper<ReturnType, Args...>(&pyobj).get_C_callback());
            return smart_ptr<callback_t>(retval, true);
        }
     };

     /**
     * Specialization for char*
     **/
     template<>
     class CObjectConversionHelper<const char*>{
     public:
       static smart_ptr<const char*> toCObject( PyObject& pyobj){
            const char* name = nullptr;
            if (PyObject_Type(&pyobj)== (PyObject*)&PythonClassWrapper<const char*>::Type){
                PythonClassWrapper<char* const>* const self_ = ((PythonClassWrapper<char* const>* const)&pyobj);
                name = (const char*)self_->get_CObject();
            } else if (PyObject_Type(&pyobj)== (PyObject*)&PythonCPointerWrapper<const char>::Type){
                PythonCPointerWrapper<const char>* const self_ = ((PythonCPointerWrapper<const char>* const)&pyobj);
                name = (const char*)self_->ptr();
            }else if (PyString_Check(&pyobj)){
              name =  (const char*)PyString_AS_STRING( &pyobj);
            } else {
                PyObject_Print(&pyobj, stderr, 0);
                throw "Conversiont o C stgring from non-string Python object!";
            }
            if (!name){ throw "Error converting string: null pointer encountered";}

            return smart_ptr<const char*>((const char* *)&name, false);
        }
     };

    /**
     * Specialization for char*
     **/
     template<>
     class CObjectConversionHelper<const char* const>{
     public:
       static smart_ptr<const char* const > toCObject( PyObject& pyobj){
           const char* name = nullptr;
            if (PyObject_Type(&pyobj)== (PyObject*)&PythonClassWrapper<const char*>::Type){
                PythonClassWrapper<char* const>* const self_ = ((PythonClassWrapper<char* const>* const)&pyobj);
                name = (const char*)self_->get_CObject();
            } else if (PyObject_Type(&pyobj)== (PyObject*)&PythonCPointerWrapper<const char>::Type){
                PythonCPointerWrapper<const char>* const self_ = ((PythonCPointerWrapper<const char>* const)&pyobj);
                name = (const char*)self_->ptr();
            }else if (PyString_Check(&pyobj)){
              name =  (const char*)PyString_AS_STRING( &pyobj);
            } else {
                PyObject_Print(&pyobj, stderr, 0);
                throw "Conversiont o C stgring from non-string Python object!";
            }
            if (!name){ throw "Error converting string: null pointer encountered";}

            return smart_ptr<const char* const>((const char* const *)&name, false);
        }
     };


    /**
     * Specialization for char*
     **/
     template<>
     class CObjectConversionHelper< char* const>{
     public:
       static smart_ptr< char* const> toCObject( PyObject& pyobj){
            const char* name = nullptr;
            if (PyString_Check(&pyobj)){
              name =  (const char*)PyString_AS_STRING( &pyobj);
            } else {
                throw "Conversiont o C stgring from non-string Python object!";
            }
            if (!name){ throw "Error converting string: null pointer encountered";}

            return smart_ptr<char* const>((char* const*)&name, false);
        }
     };

    /**
     * Specialization for char*
     **/
     template<>
     class CObjectConversionHelper< char* >{
     public:
       static smart_ptr<const char*> toCObject( PyObject& pyobj){
            const char* name = nullptr;
            if (PyString_Check(&pyobj)){
              name =  (const char*)PyString_AS_STRING( &pyobj);
            } else {
                throw "Conversiont o C stgring from non-string Python object!";
            }
            if (!name){ throw "Error converting string: null pointer encountered";}

            return smart_ptr<const char*>((const char**)&name, true);
        }
     };
    /**
     * function to convert python object to underlying C type using a class helper
     **/
    template< typename T>
    smart_ptr<T> toCObject( PyObject& pyobj){
        return CObjectConversionHelper<T>::toCObject(pyobj);
    }

}

#endif
