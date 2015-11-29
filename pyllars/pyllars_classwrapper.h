//  PREDECLARATIONS
#include <memory>
#include "pyllars_utils.h"

namespace __pyllars_internal{

    ///////////
    // Helper conversion functions
    //////////
    template< typename T,  typename E = void>
    PyObject* toPyObject( T &var, const bool asArgument);

    template< typename T>
    smart_ptr<T> toCObject( PyObject& pyobj );

}

#ifndef __PYLLARS__INTERNAL__CLASS_WRAPPER_H
#define __PYLLARS__INTERNAL__CLASS_WRAPPER_H

#include <Python.h>
#include <structmember.h>
#include <limits.h>
#include <limits>
#include <vector>
#include <map>
#include <functional>

#include "pyllars/pyllars_utils.h"

namespace __pyllars_internal{
   struct PythonBase{
        PyObject_HEAD /*Per Python API docs*/

        typedef PyTypeObject * TypePtr_t;
        static TypePtr_t constexpr TypePtr =  &PyBaseObject_Type;
    };

    /*********
     * Class to define Python wrapper to C class/type
     **/
    template< typename CClass, typename Base=PythonBase>
    struct PythonClassWrapper;

    namespace{
                extern const char address_name[] = "addr";
    }

    ///////////
    // Helper conversion functions
    //////////

    /**
     * Define conversion helper class, which allows easier mechanism
     * for necessary specializations
     **/
    template< typename T,  typename E = void>
    class PyObjectConversionHelper{
    public:
        static PyObject* toPyObject(const T &var, const bool asArgument);
        static PyObject* toPyObject(T &var, const bool asArgument);
    };

    /**
    * specialize for non-copiable types
    **/
    template< typename T>
    class PyObjectConversionHelper<T,  typename  std::enable_if<!std::is_copy_constructible<T>::value >::type >{
    public:


        static PyObject* toPyObject(  T & var, const bool asArgument ){
            //must pass in keyword to inform Python it is ok to create a Python C Wrapper object
            //with a null content, which will be set here
            PyObject* kw = PyDict_New();
            PyDict_SetItemString(kw, "__internal_allow_null", PyBool_FromLong(asArgument));
            //create the object of the desired type and do some checks
            PyObject* emptyTuple = PyTuple_New(0);
            PyObject* pyobj = PyObject_Call(  (PyObject*)&PythonClassWrapper<T>::Type, emptyTuple, kw);
            Py_DECREF(kw);
            Py_DECREF(emptyTuple);
            if ( !pyobj || !PyObject_TypeCheck(pyobj,&PythonClassWrapper<T>::Type)){
                PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                goto onerror;
            }
            //here is where the pointer content is set:
            reinterpret_cast<PythonClassWrapper<T>*>(pyobj)->set_content(&var);
            return pyobj;
        onerror:
            return Py_None;
        }
    };

    /**
     * specialize for non-trivial copiable types
     **/
    template<typename T>
    class PyObjectConversionHelper<T,  typename  std::enable_if< std::is_copy_constructible<T>::value && !std::is_integral<T>::value && !std::is_floating_point<T>::value && !std::is_pointer<T>::value >::type >{
    public:

        static PyObject* toPyObject(  T & var, const bool asArgument ){
            PyObject* kw = PyDict_New();
            PyDict_SetItemString(kw, "__internal_allow_null", PyBool_FromLong(asArgument));
            //create the object of the desired type and do some checks
            PyType_Ready(&PythonClassWrapper<T>::Type);
            PyObject* emptyTuple = PyTuple_New(0);
            PyObject* pyobj = PyObject_Call(  (PyObject*)&PythonClassWrapper<T>::Type, emptyTuple, kw);
            Py_DECREF(kw);
            Py_DECREF(emptyTuple);
            if ( !pyobj || !PyObject_TypeCheck(pyobj,&PythonClassWrapper<T>::Type)){
                PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                goto onerror;
            }

            reinterpret_cast<PythonClassWrapper<T>*>(pyobj)->set_content( asArgument?&var:new typename std::remove_reference<T>::type(var));
            return pyobj;
        onerror:
            PyErr_Print();
            return Py_None;

        }
    };

    /**
     * specialize for integer types
     **/
    template<typename T>
    class PyObjectConversionHelper<T,  typename  std::enable_if< std::is_integral<T>::value >::type >{
    public:
        static PyObject* toPyObject( const T & var, const bool asArgument ){
            (void)asArgument;
            return PyInt_FromLong( var);
        }
    };

    /**
     * specialize for floating point types
     **/
    template<typename T>
    class PyObjectConversionHelper<T,  typename  std::enable_if< std::is_floating_point<T>::value >::type >{
    public:
        static PyObject* toPyObject( const T & var, const bool asArgument ){
            (void)asArgument;
            return PyFloat_FromDouble( var);
        }
    };

   /**
     * specialize for cosnt integer reference types
     **/
    template<typename T>
    class PyObjectConversionHelper<T,  typename  std::enable_if< std::is_const<T>::value && std::is_reference<T>::value && std::is_integral<typename std::remove_reference<T>::type>::value >::type >{
    public:
        static PyObject* toPyObject( const T & var, const bool asArgument ){
             if(!asArgument){
                PyErr_SetString( PyExc_RuntimeError, "cannot create const reference variable from basic type");
                return Py_None;
            }
            PyObject* args = PyTuple_New(1);
            PyTuple_SET_ITEM(args, 0, PyLong_FromLong(var));
            PyObject* retval =  PyObject_CallObject(&PythonClassWrapper<T>::Type, args);
            Py_DECREF( PyTuple_GetItem(args,0));
            Py_DECREF( args );
            return retval;
        }
    };

    /**
     * specialize for const non-basic reference types
     **/
    template<typename T>
    class PyObjectConversionHelper<T,  typename  std::enable_if< std::is_const<T>::value && std::is_reference<T>::value && !std::is_integral<typename std::remove_reference<T>::type>::value && std::is_floating_point<typename std::remove_reference<T>::type>::value >::type >{
    public:
        static PyObject* toPyObject( const T & var, const bool asArgument ){
            if(!asArgument){
                PyErr_SetString( PyExc_RuntimeError, "cannot create const reference variable from basic type");
                return Py_None;
            }
            PyObject* args = PyTuple_New(1);
            PyTuple_SET_ITEM(args, 0, toPyObject(var));
            PyObject* retval = PyObject_CallObject(&PythonClassWrapper<T>::Type, args);
            Py_DECREF( PyTuple_GetItem(args,0));
            Py_DECREF( args );
            return retval;
        }
    };

   /**
     * specialize for const floating point reference types
     **/
    template<typename T>
    class PyObjectConversionHelper<T,  typename  std::enable_if< std::is_const<T>::value && std::is_reference<T>::value && !std::is_floating_point<typename std::remove_reference<T>::type>::value >::type >{
    public:
        static PyObject* toPyObject( const T & var, const bool asArgument ){
            if(!asArgument){
                PyErr_SetString( PyExc_RuntimeError, "cannot create const reference variable from basic type");
                return Py_None;
            }
            PyObject* args = PyTuple_New(1);
            PyTuple_SET_ITEM(args, 0, PyFloat_FromDouble(var));
            PyObject* retval = PyObject_CallObject(&PythonClassWrapper<T>::Type, args);
            Py_DECREF( PyTuple_GetItem(args,0));
            Py_DECREF( args );
            return retval;
        }
    };

    /**
     * convert C Object to python object
     * @param var: value to convert
     * @param asArgument: whether to be used as argument or not (can determine if copy is made or reference semantics used)
     **/
    template< typename T, typename E = void>
    PyObject* toPyObject(  T & var, const bool asArgument ){
        return PyObjectConversionHelper<T>::toPyObject(var, asArgument);
    }

   template< typename T, typename E = void>
    PyObject* toPyObject(  const T & var, const bool asArgument ){
        return PyObjectConversionHelper<T>::toPyObject(var, asArgument);
    }

    /////////////////////////////////////////

    /**
     * class to hold referecne to a class method and define
     * method call semantics
     **/
    template<typename CClass, typename T, typename ... Args>
    class MethodCallSemantics{
    public:
        typedef T(CClass::*method_t)(Args...);
        typedef T CClass::* member_t;
        static member_t member;
        static const char* const *kwlist;

        /**
         * if call is a "get_" member element call:
         **/
        static PyObject* toPyObj(CClass &self){
            return toPyObject<T>(self.*member, false);
        }

        /**
         * Used for regular methods:
         */
        static PyObject* call( method_t method, CClass & self, PyObject* args, PyObject* kwds){
	  try{
              return toPyObject( call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type()), false);
	  } catch( const char* const msg){
	    PyErr_SetString( PyExc_RuntimeError, msg);
	    return  nullptr;
	  }
        }

    private:

        /**
         * call that invokes method a la C:
         **/
        template< typename ...PyO>
        static T call_methodC( T (CClass::*method)(Args...),
                                        typename std::remove_reference<CClass>::type &self,
                                        PyObject *args, PyObject *kwds, PyO* ...pyargs){
            static char format[sizeof...(Args)+1] = {0};
            if (sizeof...(Args)>0)
                memset( format, 'O', sizeof...(Args));

            if(!PyArg_ParseTupleAndKeywords(args, kwds, format, (char**)kwlist, &pyargs...)){
	      throw "Invalid arguments to method call";
	    }
            T retval =  (self.*method)(*toCObject<Args>(*pyargs)...);
            return retval;
        }

        /**
         * call that converts python given arguments to make C call:
         **/
        template<int ...S>
        static T call_methodBase( T (CClass::*method)(Args...),
                                  typename std::remove_reference<CClass>::type &self,
                                  PyObject* args, PyObject* kwds, container<S...> s) {
            (void)s;
            PyObject pyobjs[sizeof...(Args)+1];
            return call_methodC(method, self, args, kwds, &pyobjs[S]...);
            (void)pyobjs;
        }

    };

    template< class CClass, typename ReturnType, typename ...Args>
    const char* const *
    MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;


    /**
     * specialize for void returns:
     **/
    template<typename CClass, typename ...Args>
    class MethodCallSemantics<CClass,void, Args...>{
    public:
        typedef void(CClass::*method_t)(Args...);
        typedef void* member_t;
        static member_t member;
        static const char* const * kwlist;

        static PyObject* toPyObj(CClass & self){
            (void)self;
            return Py_None;
        }
        static PyObject* call( method_t method, CClass & self, PyObject* args, PyObject* kwds){
	  call_methodBase(method, self, args, kwds, typename argGenerator< sizeof...(Args) >::type());
            return Py_None;
        }

    private:

        template< typename ...PyO>
        static void call_methodC( void (CClass::*method)(Args...),
                                        typename std::remove_reference<CClass>::type &self,
                                        PyObject* args, PyObject* kwds,
                                        PyO* ...pyargs){

            char format[sizeof...(Args)+1]={0};
            if (sizeof...(Args) > 0)
                memset(format,'O',sizeof...(Args));
            if(!PyArg_ParseTupleAndKeywords(args, kwds, format, (char**)kwlist, &pyargs...)){
                PyErr_SetString( PyExc_RuntimeError, "Failed to parse argument on method call");
            } else {
                (self.*method)(*toCObject<Args>(*pyargs)...);
            }
        }

        template<int ...S>
        static void call_methodBase( void (CClass::*method)(Args...),
                                     typename std::remove_reference<CClass>::type &self,
                                     PyObject *args, PyObject *kwds,
				     container<S...> unused) {
	     (void)unused;
             PyObject pyobjs[sizeof...(Args)+1];
             call_methodC(method, self, args, kwds, &pyobjs[S]...);
             (void)pyobjs;
        }

     };

    template< class CClass, typename T, typename ...Args>
    typename MethodCallSemantics<CClass, T, Args...>::member_t
    MethodCallSemantics<CClass, T, Args...>::member;

    template< class CClass, typename ...Args>
    typename MethodCallSemantics<CClass, void, Args...>::member_t
    MethodCallSemantics<CClass, void, Args...>::member;

    template< class CClass, typename ...Args>
    const char* const *
    MethodCallSemantics<CClass, void, Args...>::kwlist;


    /**
     * This class is needed to prevent ambiguities and compiler issues in add_method
     * It holds the method call/member getter method and allows specialization based on
     * underlying CClass type
     **/
    template<  class CClass,  typename E = void>
    class MethodContainer{
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type * , PyObject*);

        template<const char* const name, typename ReturnType, typename ...Args>
        class Container{
           typedef ReturnType(CClass::*method_t)(Args...);
           typedef ReturnType CClass::* member_t;

            typedef const char* const * kwlist_t;
           static constexpr  kwlist_t &kwlist = MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;

           static PyObject* call(PyObject* self, PyObject* args, PyObject* kwds);

        };
    };


    template <class CClass, class T>
    class AttributeSetter{
    public:
        static void setFromPyObject( typename std::remove_reference<CClass>::type * self, PyObject* pyobj,typename MethodCallSemantics<CClass, T>::member_t member){
             self->*member = *toCObject<T>(*pyobj);
        }
    };

    template <class CClass, class T>
    class AttributeSetter< CClass, const T>{
    public:
        static void setFromPyObject( typename std::remove_reference<CClass>::type * self, PyObject* pyobj,typename MethodCallSemantics<CClass, const T>::member_t member){
             (void)self; (void)pyobj;(void)member;
             PyErr_SetString(PyExc_TypeError, "Attempt to set const field in C class object");
             PyErr_Print();
         }
    };

    /**
     * Specialization for non-const class types
     **/
    template<class CClass>
    class MethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && !std::is_const<CClass>::value >::type>{
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type * , PyObject*);

        template<const char* const name, typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef ReturnType(CClass::*method_t)(Args...);
            typedef typename MethodCallSemantics<CClass, ReturnType>::member_t member_t;

            typedef const char* const * kwlist_t;
            static constexpr kwlist_t &kwlist = MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;
            static method_t method;
            static member_t constexpr &member = MethodCallSemantics<CClass, ReturnType, Args...>::member;

            static PyObject* call(PyObject* self, PyObject* args, PyObject* kwds){
                if(!self) return nullptr;
                PythonClassWrapper<CClass>* _this = (PythonClassWrapper<CClass>*)self;
                if(_this->get_CObject()){
                    if (method){
                        try{
                            return MethodCallSemantics<CClass, ReturnType, Args...>::call(method, *_this->get_CObject(),args, kwds);
                        } catch(...){
                            return nullptr;
                        }
                    } else if(member){
                        return MethodCallSemantics<CClass, ReturnType, Args...>::toPyObj(*_this->get_CObject());
                    }
                }
                return nullptr;
            }
            static void setFromPyObject( typename std::remove_reference<CClass>::type * self, PyObject* pyobj){
                AttributeSetter< CClass, ReturnType>::setFromPyObject(self, pyobj, member);
            }
        };
    };


    /**
     * Specialization for const class types
     **/
    template<class CClass>
    class MethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && std::is_const<CClass>::value >::type>{
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type * , PyObject*);

        template<const char* const name, typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef ReturnType(CClass::*method_t)(Args...);
            typedef typename MethodCallSemantics<CClass, ReturnType>::member_t member_t;

            typedef const char* const * kwlist_t;
            static constexpr kwlist_t &kwlist = MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;
            static method_t method;
            static constexpr member_t &member = MethodCallSemantics<CClass, ReturnType, Args...>::member;

            static PyObject* call(PyObject* self, PyObject* args, PyObject* kwds){
                if(!self) return nullptr;
                PythonClassWrapper<CClass>* _this = (PythonClassWrapper<CClass>*)self;
                if(_this->get_CObject()){
                    if (method){
                        try{
                            return MethodCallSemantics<CClass, ReturnType, Args...>::call(method, *_this->get_CObject(),args, kwds);
                        } catch(...){
                            return nullptr;
                        }
                    } else if(member){
                        return MethodCallSemantics<CClass, ReturnType, Args...>::toPyObj(*_this->get_CObject());
                    }
                }
                return nullptr;
            }
            static void setFromPyObject( typename std::remove_reference<CClass>::type * self, PyObject* pyobj){
               PyErr_SetString(PyExc_TypeError, "Attempt to set const field in C class object");
               PyErr_Print();
            }
        };
    };

    template< class CClass>
    template< const char* const name, typename ReturnType, typename ...Args>
    typename MethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && !std::is_const<CClass>::value >::type>::template Container<name, ReturnType, Args...>::method_t
     MethodContainer< CClass, typename std::enable_if< std::is_class<CClass>::value && !std::is_const<CClass>::value >::type>::Container<name, ReturnType, Args...>::method;


    template< class CClass>
    template< const char* const name, typename ReturnType, typename ...Args>
    typename MethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && std::is_const<CClass>::value >::type>::template Container<name, ReturnType, Args...>::method_t
     MethodContainer< CClass, typename std::enable_if< std::is_class<CClass>::value && std::is_const<CClass>::value >::type>::Container<name, ReturnType, Args...>::method;


    /**
     * Specialization for integral types
     **/
    template<typename CClass>
    class MethodContainer< CClass, typename std::enable_if< std::is_integral<CClass>::value>::type>{
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type * , PyObject*);

        template< const char* const name, typename ReturnType, typename ...Args>
        class Container{
           typedef int member_t;
           static PyObject* call(PyObject*, PyObject*, PyObject*){
                return nullptr;
           }
        };
    };

    /**
     * Specialization for floating point types
     **/
   template< typename CClass>
    class MethodContainer<  CClass, typename std::enable_if< std::is_floating_point<CClass>::value>::type>{
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type * , PyObject*);

        template< const char* const, typename ReturnType, typename ...Args>
        class Container{
           typedef int member_t;
           static PyObject* call(PyObject*, PyObject*, PyObject*){
                return nullptr;
           }
        };
    };

    static PyMethodDef emptyMethods[] = {{nullptr, nullptr, 0, nullptr}};
    //
    /////////////////////////

    template<typename CClass, typename E=void>
    class InitHelper{
    public:
        static int init(PythonClassWrapper<CClass> *self, PyObject* args, PyObject*kwds);
    };

    /**
     * Specialization for integers
     **/
    template<typename T>
    class InitHelper<T, typename std::enable_if<std::is_integral<T>::value>::type >{
    public:
        static int init(PythonClassWrapper<T> *self, PyObject* args, PyObject*kwds){
            if (!self) {
                return -1;
            }
            static const char* kwlist[] = {"value",nullptr};
            if (std::is_unsigned<T>::value){
                unsigned long long value;
                if (! PyArg_ParseTupleAndKeywords(args, kwds, "L", (char**)kwlist,
                                                  &value))
                    return -1;
                if (value < (unsigned long long)std::numeric_limits<T>::min() || value >(unsigned long long)std::numeric_limits<T>::max() ) {
                    PyErr_SetString(PyExc_OverflowError,"Integer value out of range of int");
                    return -1;
                }
                self->_CObject = new T((T)value);
            } else {
                long long value;
                fprintf(stderr, "%ld", PyLong_AsLong(PyTuple_GetItem(args,0)));
                if (! PyArg_ParseTupleAndKeywords(args, kwds, "K", (char**)kwlist,
                                                  &value))
                    return -1;
                if (value < std::numeric_limits<T>::min() || value >std::numeric_limits<T>::max() ) {
                    PyErr_SetString(PyExc_OverflowError,"Integer value out of range of int");
                    return -1;
                }
                self->_CObject = new T((T)value);
            }
            return 0;
        }
    };

    /**
     * Specialization for floating point
     **/
    template<typename T>
    class InitHelper<T, typename std::enable_if<std::is_floating_point<T>::value >::type>{
    public:
        static int init(PythonClassWrapper<T> *self, PyObject* args, PyObject*kwds){
            static const char* kwlist[] = {"value",nullptr};
            if (!self) {
                return -1;
            }

            double value;
            if (! PyArg_ParseTupleAndKeywords(args, kwds, "d", (char**)kwlist,
                                              &value))
                return -1;
            if (value < std::numeric_limits<T>::min() || value >std::numeric_limits<T>::max() ) {
                PyErr_SetString(PyExc_OverflowError,"Integer value out of range of int");
                return -1;
            }
            self->_CObject = new T((T)value);


            return 0;
        }
    };

    //specialize for non-numeric fundamental types:
    template<typename T>
    class InitHelper<T , typename std::enable_if<!std::is_arithmetic<T>::value && std::is_fundamental<T>::value >::type>{
    public:
        static int init(PythonClassWrapper<T> *self, PyObject* args, PyObject*kwds){
            if (!self) {
                return -1;
            }
            for (auto it = PythonClassWrapper<T>::_constructors.begin(); it != PythonClassWrapper<T>::_constructors.end(); ++it){
              try{
                    if ((self->_CObject = (*it)(args, kwds)) != nullptr){
                              break;
                    }
              } catch(...){
              }
              PyErr_Clear();
            }
            if ( self->_CObject == nullptr){
                static const char * kwdlist[] = {"value", nullptr};
                PyObject* pyobj;
                if (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwdlist, pyobj) ) {
                    if(!PyObject_TypeCheck(pyobj, &PythonClassWrapper<T>::Type)){
                        PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                        return -1;
                    }
                    self->_CObject = new typename std::remove_reference<T>::type(*(reinterpret_cast<PythonClassWrapper<T>*>(pyobj)->get_CObject()));
                } else if ((!kwds || PyDict_Size(kwds)==0) && (!args || PyTuple_Size(args)==0)){
                    self->_CObject = new typename std::remove_reference<T>::type();
                    memset(self->_CObject, 0, sizeof(typename std::remove_reference<T>::type));
                }
            } else if((!args || PyTuple_Size(args)==0) && kwds && PyDict_Size(kwds)==1 ){
                    if( PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True){
                        PyErr_Clear();
                    } else {
                        PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                        return -1;
                    }
            }
            if (self->_CObject == nullptr) {
                PyErr_SetString(PyExc_TypeError, "Invalid argment(s) to constructor");
                return -1;
            }
            return 0;
        }
    };

    //specialize for pointer types:
    template<typename T>
    class InitHelper<T , typename std::enable_if<!std::is_arithmetic<T>::value && std::is_pointer<T>::value >::type>{
    public:
        static int init(PythonClassWrapper<T> *self, PyObject* args, PyObject*kwds){
            if (!self) {
                return -1;
            }
            self->_CObject = nullptr;
            for (auto it = PythonClassWrapper<T>::_constructors.begin(); it != PythonClassWrapper<T>::_constructors.end(); ++it){
                try{
                    if ((self->_CObject = (*it)(args, kwds)) != nullptr){
                        break;
                    }
                } catch(...){
                }
                PyErr_Clear();
            }

            if (self->_CObject == nullptr) {
                static const char * kwdlist[] = {"value", nullptr};
                PyObject pyobj;
                PythonClassWrapper<T>* pyclass = reinterpret_cast<PythonClassWrapper<T> *>(&pyobj);
                if (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwdlist, &pyobj)) {
                    if(!PyObject_TypeCheck(&pyobj, &PythonClassWrapper<T>::Type)){
                        PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                        return -1;
                    }
                    self->_CObject = pyclass->get_CObject();
                } else if((!args || PyTuple_Size(args)==0) && kwds && PyDict_Size(kwds)==1 ){
                    if( PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True){
                        PyErr_Clear();
                    } else {
                        PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                        return -1;
                    }
                } else {
                    PyErr_SetString(PyExc_TabError, "Invalid constructor argument(s)");
                    return -1;
                }
            }
            return 0;
        }
    };

    //specialize for copiable non-fundamental reference types:
    template<typename T>
    class InitHelper<T , typename std::enable_if<!std::is_integral< typename std::remove_reference<T>::type >::value &&
                                                 !std::is_floating_point< typename std::remove_reference<T>::type >::value &&
                                                 std::is_copy_constructible<typename std::remove_reference<T>::type >::value &&
                                                 std::is_reference<T>::value >::type>{
    public:
        static int init(PythonClassWrapper<T> * self, PyObject *args, PyObject *kwds){
            typedef typename std::remove_reference<T>::type T_NoRef;
            if (!self) {
                return -1;
            }
            self->_CObject = nullptr;
            for (auto it = PythonClassWrapper<T>::_constructors.begin(); it != PythonClassWrapper<T>::_constructors.end(); ++it){
	      try{
		if ((self->_CObject = (*it)(args, kwds)) != nullptr){
                  break;
		}
	      } catch(...){
	      }
	      PyErr_Clear();
            }

            if (self->_CObject == nullptr) {
                static const char * kwdlist[] = {"value", nullptr};
                PyObject *pyobj = nullptr;

                if (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwdlist, &pyobj)) {
                    PythonCPointerWrapper<T_NoRef> *pyptr = reinterpret_cast<PythonCPointerWrapper<T_NoRef> *>(pyobj);
                     if(PyObject_TypeCheck(&pyobj, &PythonCPointerWrapper<T_NoRef>::Type)){
                        self->_CObject = (T_NoRef*) pyptr->ptr();
                    }  else {
                        PyObject_Print(pyobj, stderr, 0);
                        PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                        return -1;
                    }
                 }else if((!args || PyTuple_Size(args)==0) && kwds && PyDict_Size(kwds)==1 ){
                    if( PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True){
                        PyErr_Clear();
                    } else {
                        PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                        return -1;
                    }
                } else {
                    PyErr_SetString(PyExc_TabError, "Invalid constructor argument(s)");
                    return -1;
                }
            }
            return 0;
        }
    };

   //specialize for non-copiable non-fundamental reference types:
    template<typename T>
    class InitHelper<T , typename std::enable_if<!std::is_integral< typename std::remove_reference<T>::type >::value &&
                                                 !std::is_floating_point< typename std::remove_reference<T>::type >::value &&
                                                 !std::is_copy_constructible<typename std::remove_reference<T>::type >::value &&
                                                 std::is_reference<T>::value >::type>{
    public:
        static int init(PythonClassWrapper<T> * self, PyObject *args, PyObject *kwds){
            typedef typename std::remove_reference<T>::type T_NoRef;
            if (!self) {
                return -1;
            }
            self->_CObject = nullptr;
            for (auto it = PythonClassWrapper<T>::_constructors.begin(); it != PythonClassWrapper<T>::_constructors.end(); ++it){
                try{
                    if ((self->_CObject = (*it)(args, kwds)) != nullptr){
                        break;
                    }
                } catch(...){
                }
                PyErr_Clear();
            }

            if (self->_CObject == nullptr) {
                static const char * kwdlist[] = {"value", nullptr};
                PyObject *pyobj = nullptr;

                if (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwdlist, &pyobj)) {
                    PythonCPointerWrapper<T_NoRef> *pyptr = reinterpret_cast<PythonCPointerWrapper<T_NoRef> *>(pyobj);
                    if(PyObject_TypeCheck(&pyobj, &PythonCPointerWrapper<T_NoRef>::Type)){
                        self->_CObject = (T_NoRef*) pyptr->ptr();
                    } else {
                        PyErr_SetString(PyExc_TypeError, "Invalid type to construct from;  NOTE: type is not copy constructible");
                        return -1;
                    }
                 }else if((!args || PyTuple_Size(args)==0) && kwds && PyDict_Size(kwds)==1 ){
                    if( PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True){
                        PyErr_Clear();
                    } else {
                        PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                        return -1;
                    }
                } else {
                    PyErr_SetString(PyExc_TabError, "Invalid constructor argument(s)");
                    return -1;
                }
            }
            return 0;
        }
    };


    //specialize for integral reference types:
    template<typename T>
    class InitHelper<T , typename std::enable_if<std::is_integral< typename std::remove_reference<T>::type >::value && std::is_reference<T>::value >::type>{
    public:
        static int init(PythonClassWrapper<T> * self, PyObject *args, PyObject *kwds){
            typedef typename std::remove_reference<T>::type T_NoRef;
            if (!self) {
                return -1;
            }
            self->_CObject = nullptr;
            int status = 0;
            if (self->_CObject == nullptr) {
                static const char * kwdlist[] = {"value", nullptr};
                if (std::is_signed< T_NoRef>::value){
                    long long intval  = 0;

                    if (!PyArg_ParseTupleAndKeywords(args, kwds, "L", (char**)kwdlist, &intval)) {
                        PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                        status =  -1;
                        goto onerror;
                    }
                    if (intval < (long long) std::numeric_limits<T_NoRef>::min() || intval > (long long)std::numeric_limits<T_NoRef>::max()){
                        PyErr_SetString(PyExc_TypeError, "Argument value out of range");
                        status = -1;
                        goto onerror;
                    }
                    self->_CObject = new T_NoRef((T_NoRef)intval);

                 } else {
                     unsigned long long intval  = 0;

                    if (!PyArg_ParseTupleAndKeywords(args, kwds, "K", (char**)kwdlist, &intval)) {
                        PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                        status = -1;
                        goto onerror;
                    }
                    if (intval < (unsigned long long )std::numeric_limits<T_NoRef>::min() || intval > (unsigned long long )std::numeric_limits<T_NoRef>::max()){
                        PyErr_SetString(PyExc_TypeError, "Argument value out of range");
                        status =  -1;
                        goto onerror;
                    }
                    self->_CObject = new T_NoRef((T_NoRef)intval);

                 }
            }
        onerror:
            if(status != 0 && (!args || PyTuple_Size(args)==0) && kwds && PyDict_Size(kwds)==1 ){
                if( PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True){
                    PyErr_Clear();
                    status = 0;
                } else {
                    if (( !args|| PyTuple_Size(args)==0)  && (!kwds || PyDict_Size(kwds)==0))
                        PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                    status = -1;
                }
            }
            return status;
        }
    };

  //specialize for floating point reference types:
    template<typename T>
    class InitHelper<T , typename std::enable_if<std::is_floating_point< typename std::remove_reference<T>::type >::value && std::is_reference<T>::value >::type>{
    public:
        static int init(PythonClassWrapper<T> * self, PyObject *args, PyObject *kwds){
            typedef typename std::remove_reference<T>::type T_NoRef;
            if (!self) {
                return -1;
            }


            if (self->_CObject == nullptr) {
                static const char * kwdlist[] = {"value", nullptr};
                double intval  = 0;

                if (!PyArg_ParseTupleAndKeywords(args, kwds, "d", (char**)kwdlist, &intval)) {
                    PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                    return -1;
                }
                if (intval < (double) std::numeric_limits<T_NoRef>::min() || intval > (double)std::numeric_limits<T_NoRef>::max()){
                    PyErr_SetString(PyExc_TypeError, "Argument value out of range");
                    return -1;
                }
                self->_CObject = new T_NoRef((T_NoRef)intval);

            }
            return 0;
        }
    };

    //specialize for other complex types:
    template<typename T>
    class InitHelper<T , typename std::enable_if<!std::is_arithmetic<T>::value && !std::is_reference<T>::value &&
                                                 !std::is_pointer<T>::value && !std::is_fundamental<T>::value >::type>{
    public:
        static int init(PythonClassWrapper<T> *self, PyObject* args, PyObject*kwds){
            if (!self) {
                return -1;
            }
            self->_CObject = nullptr;
            for (auto it = PythonClassWrapper<T>::_constructors.begin(); it != PythonClassWrapper<T>::_constructors.end(); ++it){
                try{
                    if ((self->_CObject = (*it)(args, kwds)) != nullptr){
                        return 0;
                    }
                } catch( ...) {
                }
                PyErr_Clear();
            }
            if((!args || PyTuple_Size(args)==0) && kwds && PyDict_Size(kwds)==1 ){
                if( PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True){
                    PyErr_Clear();
                } else {
                    PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                    return -1;
                }
            } else {
                    PyErr_SetString(PyExc_TabError, "Invalid constructor argument(s)");
                    return -1;
            }

            return 0;
        }
    };


        template <  const char* const names[], bool spacer, typename ...Args  >
        static void* cccreate( PyObject* args, PyObject* kwds){
           return nullptr;
        }



    /**
     * Class to define Python wrapper to C class/type
     **/
    template<typename CClass, typename Base>
    struct PythonClassWrapper {
        Base baseClass;

        typedef typename std::remove_reference<CClass>::type CClass_NoRef;

        typename std::remove_reference<CClass>::type* get_CObject() {
            return _CObject;
        }

        static PyTypeObject Type;
        typedef PyTypeObject* TypePtr_t;
        static TypePtr_t constexpr TypePtr = &Type;

        static void initialize(const char* const name, PyObject* module){

            if(Type.tp_name){/*already initialize*/ return;}
            Type.tp_name = name;

            PyMethodDef pyMeth = {
              address_name,
              addr,
              METH_KEYWORDS,
              nullptr
            };
            _methodCollection.insert(_methodCollection.begin(), pyMeth);
            if(!_memberNames.empty() && !std::is_const<CClass_NoRef>::value){
                PyMethodDef pyMeth = {
                  "set_fields",
                  (PyCFunction)set_fields,
                  METH_KEYWORDS,
                  nullptr
                };
                _methodCollection.insert(_methodCollection.begin(), pyMeth);
            }
            Type.tp_methods = _methodCollection.data();
	    if (!_baseClasses.empty()){
	      Type.tp_bases = PyTuple_New(_baseClasses.size());
	      Py_ssize_t index = 0;
	      for(auto it = _baseClasses.begin(); it != _baseClasses.end(); ++it){
                  PyTuple_SET_ITEM( Type.tp_bases, index++, (PyObject*) *it);
	      }
	    }
	    if (PyType_Ready(&Type) < 0){
                PyErr_SetString(PyExc_RuntimeError,"Failed to ready type!");
                PyErr_Print();
                return ;
            }
            PyObject* const type = reinterpret_cast<PyObject*>(&Type);
            Py_INCREF(type);
            if (module != nullptr){
                if ( PyModule_AddObject(module, name, type) == 0){
                    parent_module = module;
                } else {
                    PyErr_SetString(PyExc_RuntimeError,"Failed to add type to module!");
                    PyErr_Print();
                }
            }
        }


        /**
         * Add a constructor to the list contained
         **/
        typedef typename std::remove_reference<CClass>::type* (*constructor)( PyObject *args, PyObject *kwds);
        static void addConstructor( constructor c){
            _constructors.push_back(c);
        }


        template < const char*  const kwlist[], typename ...Args >
        static CClass_NoRef* create( PyObject* args, PyObject* kwds){
            try{
                return _createBase( args, kwds, kwlist, typename argGenerator<sizeof...(Args)>::type(),(typename std::remove_reference<Args>::type*)nullptr...);
            } catch (const char* const msg){
                PyErr_SetString(PyExc_RuntimeError, msg);
                //PyErr_Print();
                return nullptr;
            }
        }


         /**
         * Add a type to be contained within this class (presumably a pointer-to-this-class-type)
         * @return: 0 on success, negative otherwise
         **/
         static int addType( const char* const name, PyTypeObject * const contained_type){

            if (PyType_Ready(contained_type) < 0)
                return -1;
            Py_INCREF((PyObject*)contained_type);
            return PyObject_SetAttrString( (PyObject*)&PythonClassWrapper<CClass>::Type,
                                           name, (PyObject*)contained_type);
         }

        static PyObject* addr(PyObject* self, PyObject *args){
            if( (args&&PyTuple_Size(args)>0)){
                PyErr_BadArgument();
                return nullptr;
            }
            PyObject* obj= toPyObject( reinterpret_cast<PythonClassWrapper*>(self)->_CObject, false);
            PyErr_Clear();
            return obj;
        }

        static PyObject* set_fields(PyObject* self, PyObject *args, PyObject* kwds){
            if( (args&&PyTuple_Size(args)!=0)){
                PyErr_BadArgument();
                return nullptr;
            }
            if(!kwds || PyDict_Size(kwds)){
                PyErr_BadArgument();
                return nullptr;
            }
            int index = 0;
            for( auto it = _memberNames.begin(); it != _memberNames.end(); ++it){
                PyObject* item = PyDict_GetItemString(kwds, *it);
                if(item){
                    PythonClassWrapper* self_ = reinterpret_cast<PythonClassWrapper*>(self);
                    if (self_ && self_->_CObject)
                        _memberSetters[index](self_->_CObject, item);
                }
                ++index;
            }
            PyObject* obj= toPyObject( reinterpret_cast<PythonClassWrapper*>(self)->_CObject, false);
            PyErr_Clear();
            return obj;
        }

         /**
         * Add a type definition to the given module
         * @return: 0 on success, negative otherwise
         **/
        static int _addToModule( const char* const name, PyObject* const to_module) {


            return 0;
        }

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char* const name, typename ReturnType, typename ...Args>
	  static void addMethod( typename MethodContainer<CClass_NoRef>::template Container<name,ReturnType, Args...>::method_t method, const char * const kwlist[]) {
            static const char* const doc = "Call method ";
            char *doc_string = new char[strlen(name) +strlen(doc)+1];
            snprintf(doc_string, strlen(name) +strlen(doc)+1, "%s%s",doc,name);

            PyMethodDef pyMeth = {
              name,
              (PyCFunction)MethodContainer<CClass_NoRef>::template Container<name, ReturnType, Args...>::call,
              METH_KEYWORDS,
              doc_string
            };

            MethodContainer<CClass>::template Container<name, ReturnType, Args...>::method = method;
            MethodContainer<CClass>::template Container<name, ReturnType, Args...>::kwlist = kwlist;
            _addMethod(pyMeth);
        }


        static void addClassMember( const char* const name, PyObject* pyobj){
            if (!Type.tp_dict){
                Type.tp_dict = PyDict_New();
            }
            PyDict_SetItemString(Type.tp_dict, name , pyobj);
        }

        static void addExtraBaseClass( PyTypeObject* base){
            if(!base) return;
            if(!Type.tp_base && _baseClasses.empty()){
                Type.tp_base = base;
            } else {
                if (Type.tp_base){
                    _baseClasses.push_back(Type.tp_base);
                    Type.tp_base = nullptr;
                }
                _baseClasses.insert(_baseClasses.begin(),base);
            }
        }

        /**
         * add a getter method for the given compile-time-known named public class member
         **/
        template< const char* const name, typename Type>
        static void addMember( typename MethodContainer<CClass_NoRef>::template Container<name, Type>::member_t member){

            static const char* const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) +strlen(doc)+1];
            snprintf(doc_string, strlen(name) +strlen(doc)+1, "%s%s",doc,name);
            static const char* const getter_prefix = "get_";
            char *getter_name = new char[strlen(name) +strlen(getter_prefix)+1];
            snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
            MethodContainer< CClass_NoRef>::template Container<name, Type>::member = member;
            PyMethodDef pyMeth = {getter_name,
                    (PyCFunction)MethodContainer<CClass_NoRef>::template Container<name,Type>::call,
                    METH_KEYWORDS,
                    doc_string
              };
              static const char* const kwlist[] = {"value",nullptr};
               MethodContainer< CClass_NoRef>::template Container<name, Type>::kwlist = kwlist;
            _addMethod(pyMeth);
            //TODO: can probably now make _memberSetters a dictionary
            _memberNames.push_back(name);
            _memberSetters.push_back(MethodContainer<CClass>::template Container<name, Type>::setFromPyObject);
        }

        void set_content(typename std::remove_reference<CClass>::type * ptr){
            _CObject = ptr;
        }

        static PyObject * parent_module;

        friend class PythonCPointerWrapper<CClass>;

        template<typename C, typename E>
        friend  class InitHelper;


    private:
        CClass_NoRef* _CObject;

        static int
        _init( PythonClassWrapper *self, PyObject *args, PyObject *kwds){
            if( Type.tp_base && Base::TypePtr->tp_init)
                Base::TypePtr->tp_init( (PyObject*)&self->baseClass, args, kwds);
            return InitHelper<CClass>::init(self, args, kwds);
        }


        static PyObject* _new( PyTypeObject* type, PyObject* args, PyObject*kwds) {
            (void)args;
            (void)kwds;
            PythonClassWrapper  *self;
            self = (PythonClassWrapper*)type ->tp_alloc(type, 0);
            if( Type.tp_base && Base::TypePtr->tp_new)
                Base::TypePtr->tp_new( Type.tp_base, args, kwds);
            if ( nullptr != self) {
                self->_CObject = nullptr;
            }
            return (PyObject*) self;
        }

        static void _dealloc( PythonClassWrapper* self) {
            if(!self) return;
            //delete self->_CObject;
            self->_CObject = nullptr;
            self->baseClass.ob_type->tp_free((PyObject*)self);
        }


        template <typename ...PyO>
        static bool _parsePyArgs( const char* const kwlist[],  PyObject* args, PyObject* kwds, PyO*& ...pyargs){
          char format[sizeof...(PyO)+1] = {0};
          if(sizeof...(PyO)> 0)
            memset(format, 'O', sizeof...(PyO));
          return sizeof...(PyO)== 0 || PyArg_ParseTupleAndKeywords(args, kwds, format, (char**)kwlist, &pyargs... );
        }

        template< typename ...Args>
        static typename std::remove_reference<CClass>::type* _createBaseBase(Args... args){
            return new typename std::remove_reference<CClass>::type(args...);
        }

        template< typename ...Args, int ...S>
        static typename std::remove_reference<CClass>::type* _createBase( PyObject* args, PyObject* kwds, const char* const kwlist[], container<S...> unused1, Args*... unused2){
            (void)unused1;
            void* unused = {(void*)unused2...}; (void)unused;
            if (args && PyTuple_Size(args)!= sizeof...(Args)){
                return nullptr;
            }
            PyObject* pyobjs[sizeof...(Args)+1];
            (void)pyobjs;
            if (!_parsePyArgs(kwlist, args, kwds, pyobjs[S]...)){
                PyErr_SetString(PyExc_TypeError, "Invalid constgructor arguments");
                PyErr_Print();
                return nullptr;
            }

	     return _createBaseBase<Args...>( *toCObject<Args>(*pyobjs[S])...);
        }


        static std::vector<constructor> _constructors;
        static std::vector<const char*> _memberNames;
        static std::vector<typename MethodContainer<CClass>::setter_t> _memberSetters;
        static std::vector<PyMethodDef> _methodCollection;
        static std::vector<PyTypeObject*> _baseClasses;
        static void _addMethod( PyMethodDef method){
            //insert at beginning to keep null sentinel at end of list:
            _methodCollection.insert(_methodCollection.begin(), method);
            Type.tp_methods = _methodCollection.data();
        }



    };



    template< typename CClass, typename Base>
    PyObject* PythonClassWrapper<CClass, Base>::parent_module = nullptr;

    template< typename CClass, typename Base>
    std::vector<PyMethodDef> PythonClassWrapper<CClass, Base>::_methodCollection = std::vector<PyMethodDef>(emptyMethods, emptyMethods+1);

    template< typename CClass, typename Base>
    std::vector<const char* > PythonClassWrapper<CClass, Base>::_memberNames = std::vector<const char* >();

    template< typename CClass, typename Base>
    std::vector< PyTypeObject* > PythonClassWrapper<CClass, Base>::_baseClasses = std::vector<PyTypeObject * >();

    template< typename CClass, typename Base>
    std::vector<typename MethodContainer<CClass>::setter_t> PythonClassWrapper<CClass, Base>::_memberSetters = std::vector< typename MethodContainer<CClass>::setter_t>();

    template<typename CClass, typename Base>
    std::vector<typename PythonClassWrapper<CClass, Base>::constructor > PythonClassWrapper<CClass, Base>::_constructors;

    template< typename CClass, typename Base>
    PyTypeObject PythonClassWrapper<CClass, Base>::Type = {

        PyObject_HEAD_INIT(nullptr)
        0,                         /*ob_size*/
        nullptr,             /*tp_name*/ /*filled on init*/
        sizeof(PythonClassWrapper),             /*tp_basicsize*/
        0,                         /*tp_itemsize*/
        (destructor)PythonClassWrapper::_dealloc, /*tp_dealloc*/
        nullptr,                         /*tp_print*/
        nullptr,                         /*tp_getattr*/
        nullptr,                         /*tp_setattr*/
        nullptr,                         /*tp_compare*/
        nullptr,                         /*tp_repr*/
        nullptr,                         /*tp_as_number*/
        nullptr,                         /*tp_as_sequence*/
        nullptr,                         /*tp_as_mapping*/
        nullptr,                         /*tp_hash */
        nullptr,                         /*tp_call*/
        nullptr,                         /*tp_str*/
        nullptr,                         /*tp_getattro*/
        nullptr,                         /*tp_setattro*/
        nullptr,                         /*tp_as_buffer*/
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
        "PythonClassWrapper object",           /* tp_doc */
        nullptr,		               /* tp_traverse */
        nullptr,		               /* tp_clear */
        nullptr,		               /* tp_richcompare */
        0,		                       /* tp_weaklistoffset */
        nullptr,		               /* tp_iter */
        nullptr,		               /* tp_iternext */
        PythonClassWrapper::_methodCollection.data(),             /* tp_methods */
        nullptr,             /* tp_members */
        nullptr,                         /* tp_getset */
        Base::TypePtr,                         /* tp_base */
        nullptr,                         /* tp_dict */
        nullptr,                         /* tp_descr_get */
        nullptr,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        (initproc)PythonClassWrapper::_init,  /* tp_init */
        nullptr,                         /* tp_alloc */
        PythonClassWrapper::_new,             /* tp_new */
        nullptr,                         /*tp_free*/ //TODO: Implement a free??
        nullptr,                         /*tp_is_gc*/
        nullptr,                         /*tp_bases*/
        nullptr,                         /*tp_mro*/
        nullptr,                         /*tp_cache*/
        nullptr,                         /*tp_subclasses*/
        nullptr,                          /*tp_weaklist*/
        nullptr,                          /*tp_del*/
        0,                          /*tp_version_tag*/
    };



}
#endif
