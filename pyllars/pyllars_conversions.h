
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
    template < typename C_type, bool is_array, bool is_complete,typename E = void>
    class CObjectConversionHelper;


    /**
     * Specialization for classes, references and pointer types (except function ptr typedefs)
     **/
    template< typename C_type, bool is_array, bool is_complete>
    class CObjectConversionHelper< C_type , is_array,  is_complete,
                                  typename std::enable_if< std::is_class<C_type>::value ||
                                                          (std::is_reference<C_type>::value && !
                                                           std::is_const<typename std::remove_reference<C_type>::type>::value)|| (std::is_pointer<C_type>::value && std::is_convertible<C_type, const void*>::value)>::type>{
    public:

        static smart_ptr<C_type> toCObject( PyObject& pyobj){
            if(&pyobj == nullptr){
                throw "Invalid argument for conversion";
            }
            typedef typename std::remove_reference<C_type>::type C_bare;
            typedef typename std::remove_reference<typename std::remove_pointer<C_type>::type>::type C_base;
            if (PyObject_TypeCheck(&pyobj, (&PythonCPointerWrapper<C_base, is_complete>::Type))){
              return smart_ptr<C_type>((C_bare*)reinterpret_cast<PythonCPointerWrapper<C_base, is_complete> *>(&pyobj)->get_CObject(), false);
            }

            if (PyObject_TypeCheck(&pyobj, (&PythonClassWrapper<typename std::remove_const<C_bare>::type , true>::Type))){
              return smart_ptr<C_type>((typename std::remove_const<C_bare>::type*)reinterpret_cast<PythonClassWrapper<typename std::remove_const<C_bare>::type, true> *>(&pyobj)->get_CObject(), false);
            }
            if (!PyObject_TypeCheck(&pyobj, (&PythonClassWrapper< C_bare, true >::Type)) && !PyObject_TypeCheck(&pyobj, (&PythonClassWrapper< C_type, true >::Type))){
                fprintf(stderr, "\n");
                PyObject_Print( &pyobj, stderr, 0);
                fprintf(stderr, "\n");
                PyObject_Print(  PyObject_Type(&pyobj), stderr, 0);
                fprintf(stderr, "\n");
                throw "Invalid type converting to C object";
            } else {
                return smart_ptr<C_type>((typename std::remove_reference<C_type>::type*)reinterpret_cast<PythonClassWrapper< C_bare, is_complete>* >(&pyobj)->get_CObject(), false);
            }
            return smart_ptr<C_type>((typename std::remove_reference<C_type>::type*)reinterpret_cast<PythonClassWrapper< C_type, is_complete >* >(&pyobj)->get_CObject(), false);
        }
    };

    /**
     * Specialization for integer types
     **/
    template<typename T, bool is_array, bool is_complete>
    class CObjectConversionHelper<T, is_array, is_complete, typename std::enable_if< std::is_integral<T>::value ||
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
            } else if (!PyObject_TypeCheck(&pyobj, (&PythonClassWrapper<T_bare, true>::Type))){
                throw "Invalid type converting to C object";
            }
            return smart_ptr<T>(reinterpret_cast<PythonClassWrapper<T_bare, true>* >(&pyobj)->get_CObject(), false);
      }
    };

    /**
     * Specialization for floating point types
     **/
    template<typename T, bool is_array, bool is_complete>
    class CObjectConversionHelper<T, is_array, is_complete, typename std::enable_if< std::is_floating_point<T>::value ||
       ( std::is_floating_point<typename std::remove_reference<T>::type>::value &&
         std::is_reference<T>::value &&
         std::is_const<typename std::remove_reference<T>::type>::value)>::type >{
    public:
      static smart_ptr<T> toCObject( PyObject& pyobj){
        typedef typename std::remove_reference< T>::type T_bare;
        if (PyFloat_Check(&pyobj)){
            return smart_ptr<T>( new T_bare(PyFloat_AsDouble( &pyobj )), true);
        } else if (!PyObject_TypeCheck(&pyobj, (&PythonClassWrapper<T_bare, true>::Type))){
            PyObject_Print(&pyobj,stderr, 0);
            throw "Invalid type converting to C object";
        }
        T_bare* retval = (reinterpret_cast<PythonClassWrapper<T_bare, true>* >(&pyobj)->get_CObject());
        return smart_ptr<T>(retval, false);
      }
    };

    /**
     * Specialization for callbacks
     **/
     template< typename ReturnType,
               typename ...Args >
     class CObjectConversionHelper< ReturnType(*)(Args...), false, true, void>{
     public:
        typedef ReturnType(*callback_t)(Args...);

        static smart_ptr<callback_t, false> toCObject( PyObject& pyobj){
            if (!PyCallable_Check(&pyobj)){
                throw "Python callback is not callable!";
            }
            callback_t * retval = new callback_t(PyCallbackWrapper<ReturnType, Args...>(&pyobj).get_C_callback());
            return smart_ptr<callback_t, false>(retval, true);
        }
     };

     /**
     * Specialization for char*
     **/
     template<>
     class CObjectConversionHelper<const char*, false, true>{
     public:
       static smart_ptr<const char*> toCObject( PyObject& pyobj){
            const char* name = nullptr;
            if (PyObject_Type(&pyobj)== (PyObject*)&PythonClassWrapper<const char*, true>::Type){
                PythonClassWrapper<char* const, true>* const self_ = ((PythonClassWrapper<char* const, true>* const)&pyobj);
                name = (const char*)self_->get_CObject();
            } else if (PyObject_Type(&pyobj)== (PyObject*)&PythonCPointerWrapper<const char, true>::Type){
                PythonCPointerWrapper<const char, true>* const self_ = ((PythonCPointerWrapper<const char, true>* const)&pyobj);
                name = (const char*)self_->get_CObject();
            }else if (PyString_Check(&pyobj)){
              name =  (const char*)PyString_AS_STRING( &pyobj);
            } else {
                PyObject_Print(&pyobj, stderr, 0);
                throw "Conversion to C stgring from non-string Python object!";
            }
            if (!name){ throw "Error converting string: null pointer encountered";}

            return smart_ptr<const char*, false>((const char* *)&name, false);
        }
     };

    /**
     * Specialization for char*
     **/
     template<>
     class CObjectConversionHelper<const char* const, false, true>{
     public:
       static smart_ptr<const char* const, false> toCObject( PyObject& pyobj){
	    const char* name = nullptr;
            if (PyObject_Type(&pyobj)== (PyObject*)&PythonClassWrapper<const char*, true>::Type){
                PythonClassWrapper<char* const, true>* const self_ = ((PythonClassWrapper<char* const, true>* const)&pyobj);
                name = (const char*)self_->get_CObject();
            } else if (PyObject_Type(&pyobj)== (PyObject*)&PythonCPointerWrapper<const char, true>::Type){
                PythonCPointerWrapper<const char, true>* const self_ = ((PythonCPointerWrapper<const char, true>* const)&pyobj);
                name = (const char*)self_->get_CObject();
            }else if (PyString_Check(&pyobj)){
              name =  (const char*)PyString_AS_STRING( &pyobj);
            } else {
	        name = nullptr;
                PyObject_Print(&pyobj, stderr, 0);
                throw "Conversiont o C stgring from non-string Python object!";
            }
            if (!name){ throw "Error converting string: null pointer encountered";}

            return smart_ptr<const char* const, false>((const char* const *)&name, false);
        }
     };


    /**
     * Specialization for char*
     **/
     template<>
     class CObjectConversionHelper< char* const, false, true>{
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
     class CObjectConversionHelper< char* , true, true>{
     public:
       static smart_ptr< char*, true> toCObject( PyObject& pyobj){
            const char* name = nullptr;
            if (PyString_Check(&pyobj)){
              name =  (const char*)PyString_AS_STRING( &pyobj);
            } else {
                throw "Conversiont o C stgring from non-string Python object!";
            }
            if (!name){ throw "Error converting string: null pointer encountered";}
            char *new_name = new char[strlen(name)+1];
            strcpy(new_name, name);
            return smart_ptr< char*, true>(&new_name, true);
        }
     };


    /**
     * Specialization for char*
     **/
     template<typename T, const size_t size, const bool is_array>
     class CObjectConversionHelper< T[size] , is_array, true>{
     public:

        typedef T T_array[size];

        static smart_ptr< T[size], is_array> toCObject( PyObject& pyobj){

             if (PyObject_TypeCheck(&pyobj, &(PythonCPointerWrapper<T, true, size>::Type))){
                T* val_ = reinterpret_cast<PythonCPointerWrapper<T, true, size>* >(&pyobj)->get_CObject();
                T_array * val = (T_array*)&val_;
                return smart_ptr< T[size], is_array>(val, true);
             } else if(PyObject_TypeCheck(&pyobj, (&PythonClassWrapper<T_array, true>::Type))){
                return smart_ptr<T_array, is_array>(reinterpret_cast<PythonClassWrapper<T_array, true>* >(&pyobj)->get_CObject(), false);
             } else if (PyList_Check(&pyobj)){
                if (PyList_Size(&pyobj) != size){
                    throw "Inconsistent sizes in array assignment";
                }
                if (!is_array){
                    throw "Invalid attempt on non-array allocation of fixed-size array";
                }
                T_array *val = new T_array[1];
                for(size_t i = 0; i < size; ++i){
                    PyObject* listitem = PyList_GetItem(&pyobj, i);
                    if (PyObject_TypeCheck(listitem, (&PythonClassWrapper<T ,true>::Type))){
                        (*val)[i] = *reinterpret_cast< PythonClassWrapper<T, true>* >(listitem)->get_CObject();
                    }else {
                        delete [] val;
                        throw "Invalid type in array element assignment";
                    }
                }

                return smart_ptr<T_array, is_array>(val, true);
             } else {
                        throw "Conversiont o C stgring from non-string Python object!";
            }
            return smart_ptr<T_array, is_array>(nullptr, false);
        }

     };

   /**
     * Specialization for char*
     **/
     template<typename T, const size_t size, const bool is_base_complete>
     class CObjectConversionHelper< const T[size] , false, is_base_complete>{
     public:

       typedef const T T_array[size];

       static smart_ptr< T_array, false> toCObject( PyObject& pyobj){

         if (PyObject_TypeCheck(&pyobj, &(PythonCPointerWrapper<T, is_base_complete, size>::Type))){
               T_array * const val = reinterpret_cast<PythonCPointerWrapper<T, is_base_complete, size>* >(&pyobj)->get_CObject();

               return smart_ptr< T_array, false>(val, true);
            } else if(PyObject_TypeCheck(&pyobj, (&PythonClassWrapper<T_array, is_base_complete>(&pyobj)))){
                return smart_ptr<T, false>(reinterpret_cast<PythonClassWrapper<T_array, is_base_complete>* >(&pyobj)->get_CObject(), false);
            }else {
                throw "Conversiont o C stgring from non-string Python object!";
            }
            return smart_ptr<T_array, false>(nullptr, false);
        }
     };


    /**
     * function to convert python object to underlying C type using a class helper
     **/
     template< typename T, bool is_array, bool is_complete>
     smart_ptr<T, is_array> toCObject( PyObject& pyobj){
        return CObjectConversionHelper<T, is_array, is_complete>::toCObject(pyobj);
    }

    /**
     * function to convert python object to underlying C type using a class helper
     **/
     template< typename T, const size_t size,  bool is_array, bool is_complete>
     smart_ptr<T[size], is_array> toCObject( PyObject& pyobj){
        return CObjectConversionHelper<T[size], is_array, is_complete>::toCObject(pyobj);
    }

}

#endif
