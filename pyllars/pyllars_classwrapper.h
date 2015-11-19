//  PREDECLARATIONS
namespace __pyllars_internal{
    /*********
     * Class to define Python wrapper to C class/type
     **/
    template< typename CClass, typename ...Bases>
    struct PythonClassWrapper;

    ///////////
    // Helper conversion functions
    //////////
    template< typename T,  typename E = void>
    PyObject* toPyObject( T &var, const bool asArgument);

    template< typename C_type>
    C_type toCObject( PyObject& pyobj);



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



    ///////////
    // Helper conversion functions
    //////////

    //  There seems to be a bug in g++ compaining about
    //  non-type specialization when functions are specialized
    //  even with only type parameters, so use helper class :-(


    template< typename T,  typename E = void>
    class PyObjectConversionHelper{
    public:
        static PyObject* toPyObject(const T &var, const bool asArgument);
        static PyObject* toPyObject(T &var, const bool asArgument);
    };

    /**
    * template to convert c object to python
    **/
    template< typename T>
    class PyObjectConversionHelper<T,  typename  std::enable_if<!std::is_copy_constructible<T>::value && !std::is_integral<T>::value && !std::is_floating_point<T>::value && !std::is_pointer<T>::value>::type >{
    public:


        static PyObject* toPyObject(  T & var, const bool asArgument ){
            //create the object of the desired type and do some checks
            PyObject* pyobj = PyObject_CallObject(  (PyObject*)&PythonClassWrapper<T>::Type, nullptr);
            if ( !pyobj || !PyObject_TypeCheck(pyobj,&PythonClassWrapper<T>::Type)){
                PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                goto onerror;
            }

            reinterpret_cast<PythonClassWrapper<T>*>(pyobj)->set_content(&var);
            return pyobj;
        onerror:
            return Py_None;
        }
    };

    template<typename T>
    class PyObjectConversionHelper<T,  typename  std::enable_if< std::is_copy_constructible<T>::value && !std::is_integral<T>::value && !std::is_floating_point<T>::value && !std::is_pointer<T>::value >::type >{
    public:

        static PyObject* toPyObject(  T & var, const bool asArgument ){
            //create the object of the desired type and do some checks
            PyObject* pyobj = PyObject_CallObject(  (PyObject*)&PythonClassWrapper<T>::Type, nullptr);
            if ( !pyobj || !PyObject_TypeCheck(pyobj,&PythonClassWrapper<T>::Type)){
                PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                goto onerror;
            }

            reinterpret_cast<PythonClassWrapper<T>*>(pyobj)->set_content( asArgument?&var:new typename std::remove_reference<T>::type(var));
            return pyobj;
        onerror:
            return Py_None;

        }
    };



    template<typename T>
    class PyObjectConversionHelper<T,  typename  std::enable_if< std::is_integral<T>::value >::type >{
    public:
        static PyObject* toPyObject( const T & var, const bool asArgument ){
            return PyInt_FromLong( var);
        }
    };

    template<typename T>
    class PyObjectConversionHelper<T,  typename  std::enable_if< std::is_floating_point<T>::value >::type >{
    public:
        static PyObject* toPyObject( const T & var, const bool asArgument ){
            return PyFloat_FromDouble( var);
        }
    };

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
            return toPyObject( call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type(args)), false);
        }

    private:

        /**
         * call that invokes method a la C:
         **/
        template< typename ...PyO>
        static T call_methodC( T (CClass::*method)(Args...),
                                        typename std::remove_reference<CClass>::type &self,
                                        PyObject *pytuple, PyObject *kwds, PyO* ...pyargs){
            return  (self.*method)(toCObject<Args>(*pyargs)...);
        }

        /**
         * call that converts python given arguments to make C call:
         **/
        template<int ...S, const char* const... names>
        static T call_methodBase( T (CClass::*method)(Args...),
                                        typename std::remove_reference<CClass>::type &self,
                                        PyObject *args, PyObject *kwds, container<S...> s) {
            return call_methodC(method, self, args, kwds, PyTuple_GetItem(s.pyobjs,S)...);
        }

    };

    /**
     * specialize for void returns:
     **/
    template<typename CClass, typename ...Args>
    class MethodCallSemantics<CClass,void, Args...>{
    public:
        typedef void(CClass::*method_t)(Args...);
        typedef void* member_t;
        static member_t member;
        static PyObject* toPyObj(CClass & self){
            return Py_None;
        }
        static PyObject* call( method_t method, CClass & self, PyObject* args, PyObject* kwds){
            call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type(args));
            return Py_None;
        }

    private:
        template< typename ...PyO>
        static void call_methodC( void (CClass::*method)(Args...),
                                        typename std::remove_reference<CClass>::type &self,
                                        PyObject *pytuple, PyObject *kwds, PyO* ...pyargs){
              (self.*method)(toCObject<Args>(*pyargs)...);
        }

        template<int ...S, const char* const... names>
        static void call_methodBase( void (CClass::*method)(Args...),
                                        typename std::remove_reference<CClass>::type &self,
                                        PyObject *args, PyObject *kwds, container<S...> s) {
             call_methodC(method, self, args, kwds, PyTuple_GetItem(s.pyobjs,S)...);
        }

     };
    template< class CClass, typename T, typename ...Args>
    typename MethodCallSemantics<CClass, T, Args...>::member_t
    MethodCallSemantics<CClass, T, Args...>::member;
    template< class CClass, typename ...Args>
    typename MethodCallSemantics<CClass, void, Args...>::member_t
    MethodCallSemantics<CClass, void, Args...>::member;


    /**
     * This class is needed to prevent ambiguities and compiler issues in add_method
     * It holds the method call/member getter method and allows specialization based on
     * underlying CClass type
     **/
    template<  class CClass, const char* const name, typename E = void>
    class MethodContainer{
    public:
        template<typename ReturnType, typename ...Args>
        class Container{
           typedef ReturnType(CClass::*method_t)(Args...);
           typedef ReturnType CClass::* member_t;
           static PyObject* call(PyObject* self, PyObject* args, PyObject* kwds);
        };
    };

    /**
     * Specialization for class types
     **/
    template<class CClass, const char* const name>
    class MethodContainer<CClass, name, typename std::enable_if< std::is_class<CClass>::value>::type>{
    public:

        template<typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef ReturnType(CClass::*method_t)(Args...);
            typedef typename MethodCallSemantics<CClass, ReturnType>::member_t member_t;

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
        };
    };
    template< class CClass, const char* const name>
    template< typename ReturnType, typename ...Args>
    typename MethodContainer<CClass, name, typename std::enable_if< std::is_class<CClass>::value>::type >::template Container<ReturnType, Args...>::method_t
     MethodContainer< CClass, name, typename std::enable_if< std::is_class<CClass>::value>::type >::Container<ReturnType, Args...>::method;


    /**
     * Specialization for integral types
     **/
    template<typename CClass, const char* const name>
    class MethodContainer< CClass, name, typename std::enable_if< std::is_integral<CClass>::value>::type>{
    public:
       template<  typename ReturnType, typename ...Args>
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
   template< typename CClass, const char* const name>
    class MethodContainer<  CClass,name, typename std::enable_if< std::is_floating_point<CClass>::value>::type>{
    public:
        template<  typename ReturnType, typename ...Args>
        class Container{
           typedef int member_t;
           static PyObject* call(PyObject*, PyObject*, PyObject*){
            return nullptr;
           }
        };
    };

    static PyMethodDef emptyMethods[] = {nullptr};
    static PyMemberDef emptyMembers[] = {nullptr};

       //
    /////////////////////////



    template<typename CClass, typename E=void>
    class InitHelper{
    public:
        static int init(PythonClassWrapper<CClass> *self, PyObject* args, PyObject*kwds);
    };

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

    //specialize for non-primitive types:
    template<typename T>
    class InitHelper<T , typename std::enable_if<!std::is_integral<T>::value && !std::is_floating_point<T>::value >::type>{
    public:
        static int init(PythonClassWrapper<T> *self, PyObject* args, PyObject*kwds){
          if (!self) {
            return -1;
          }
          for (auto it = PythonClassWrapper<T>::_constructors.begin(); it != PythonClassWrapper<T>::_constructors.end(); ++it){
              if ((self->_CObject = (*it)(args, kwds)) != nullptr){
                  break;
              }
          }

          //if (self->_CObject == nullptr) return -1;
          return 0;
        }
    };

    /**
     * Class to define Python wrapper to C class/type
     **/
    template<typename CClass, typename ...Bases>
    struct PythonClassWrapper {
        PyObject_HEAD /*Per Python API docs*/

        typedef typename std::remove_reference<CClass>::type CClass_NoRef;

        typename std::remove_reference<CClass>::type* get_CObject() {
            return _CObject;
        }

        static PyTypeObject Type;

        static const char* const name;

        //// Define interim constructor function types /////
        template< typename ...Args, const char* const ...names, typename ...PyO>
        static std::remove_reference<CClass>* constructorC( PyObject *pytuple, PyObject *kwds, PyO* ...pyargs);

        template<typename ...Args, int ...S, const char* const... names>
        static typename std::remove_reference<CClass>::type* constructBase(PyObject *args, PyObject *kwds, container<S...> s) {
            return constructorC<Args...,names...>(args, kwds, PyTuple_GetItem(s.pyobjs,S)...);
        }

        template<typename ...Args>
        static typename std::remove_reference<CClass>::type* construct( PyObject *args, PyObject *kwds) {
            try{
                return constructBase(args, kwds, typename container<sizeof...(Args)>::type(args));
            } catch(...){
                return nullptr;
            }
        }

        /**
         * Add a constructor to the list contained
         **/
        typedef typename std::remove_reference<CClass>::type* (*constructor)( PyObject *args, PyObject *kwds);
        static void add_constructor( constructor c){
            _constructors.push(c);
        }

         /**
         * Add a type to be contained within this class (presumably a pointer-to-this-class-type)
         * @return: 0 on success, negative otherwise
         **/
         static int addType( const char* const name, PyTypeObject * const contained_type){

            if (PyType_Ready(contained_type) < 0)
                return -1;
            Py_INCREF((PyObject*)contained_type);
            return PyObject_SetAttrString( (PyObject*)&PythonClassWrapper<CClass, Bases...>::Type,
                                            name, (PyObject*)contained_type);
         }

         /**
         * Add a type definition to the given module
         * @return: 0 on success, negative otherwise
         **/
        static int addToModule( const char* const name, PyObject* const to_module) {
            Type.tp_name = name;
            if (PyType_Ready(&Type) < 0)
                return -1;


            Py_INCREF( (&Type) );
            PyModule_AddObject(to_module, name, (PyObject *)&Type);

            if (PyType_Ready(&Type) < 0)
                return -1;

            return 0;
        }

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char* const name,  typename ReturnType, typename ...Args>
        static void add_method( typename MethodContainer<CClass, name>::template Container<ReturnType, Args...>::method_t method) {
            static const char* const doc = "Call method ";
            char *doc_string = new char[strlen(name) +strlen(doc)]+1;
            snprintf(doc_string, strlen(name) +strlen(doc), "%s%s",doc,name);

            PyMethodDef pyMeth = {
              name,
              (PyCFunction)MethodContainer<CClass_NoRef, name>::template Container<ReturnType, Args...>::call,
              METH_KEYWORDS,
              doc_string
            };

            MethodContainer<CClass, name>::template Container<ReturnType, Args...>::method = method;
            _add_method(pyMeth);
        }


        /**
         * add a getter method for the given compile-time-known named public class member
         **/
        template< const char* const name, typename Type>
        static void add_member( typename MethodContainer<CClass_NoRef, name>::template Container<Type>::member_t member){

            static const char* const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) +strlen(doc)]+1;
            snprintf(doc_string, strlen(name) +strlen(doc)+1, "%s%s",doc,name);
            static const char* const getter_prefix = "get_";
            char *getter_name = new char[strlen(name) +strlen(getter_prefix)]+1;
            snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
            MethodContainer< CClass_NoRef, name>::template Container<Type>::member = member;
            PyMethodDef pyMeth = {getter_name,
                    (PyCFunction)MethodContainer<CClass_NoRef, name>::template Container<Type>::call,
                    METH_KEYWORDS,
                    doc_string
              };
            _add_method(pyMeth);
        }

        void set_content(typename std::remove_reference<CClass>::type * ptr){
            _CObject = ptr;
        }

        static PyObject * parent_module;
        friend class PythonCPointerWrapper<CClass, Bases...>;

        template<typename C, typename E>
        friend  class InitHelper;


    private:
        CClass_NoRef* _CObject;

        static int
        _init( PythonClassWrapper *self, PyObject *args, PyObject *kwds){
            return InitHelper<CClass>::init(self, args, kwds);
        }


        static PyObject* _new( PyTypeObject* type, PyObject* args, PyObject*kwds) {
            PythonClassWrapper  *self;
            self = (PythonClassWrapper*)type ->tp_alloc(type, 0);
            if ( nullptr != self) {
                self->_CObject = nullptr;
            }
            return (PyObject*) self;
        }

        static void _dealloc( PythonClassWrapper* self) {
            if(!self) return;
            delete self->_CObject;
            self->_CObject = nullptr;
            self->ob_type->tp_free((PyObject*)self);
        }


        static std::vector<constructor> _constructors;
        static std::vector<PyMemberDef> _memberCollection;
        static std::vector<PyMethodDef> _methodCollection;

        static void _add_method( PyMethodDef method){
            //insert at beginning to keep null sentinel at end of list:
            _methodCollection.insert(_methodCollection.begin(), method);
            Type.tp_methods = _methodCollection.data();
        }


        static void _add_member( const char* const name, PyMemberDef member){
            //insert at beginning to keep null sentinel at end of list:
            _memberCollection.insert(_memberCollection.begin(), member);
            Type.tp_members = _memberCollection.data();
        }

    };

    template< typename CClass, typename ...Bases>
    PyObject* PythonClassWrapper<CClass, Bases...>::parent_module = nullptr;


    template< typename CClass, typename ...Bases>
    const char* const PythonClassWrapper<CClass, Bases...>::name = nullptr;/*filled on init*/

    template< typename CClass, typename... Bases >
    std::vector<PyMethodDef> PythonClassWrapper<CClass,  Bases...>::_methodCollection = std::vector<PyMethodDef>(emptyMethods, emptyMethods+1);

    template< typename CClass, typename... Bases >
    std::vector<PyMemberDef> PythonClassWrapper<CClass,  Bases...>::_memberCollection = std::vector<PyMemberDef>(emptyMembers, emptyMembers+1);

    template<typename CClass, typename ...Bases>
    std::vector<typename PythonClassWrapper<CClass, Bases...>::constructor > PythonClassWrapper<CClass, Bases...>::_constructors;


    template< typename CClass, typename ...Bases>
    PyTypeObject PythonClassWrapper<CClass, Bases...>::Type = {

        PyObject_HEAD_INIT(nullptr)
        0,                         /*ob_size*/
        PythonClassWrapper::name,             /*tp_name*/
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
        PythonClassWrapper::_memberCollection.data(),             /* tp_members */
        nullptr,                         /* tp_getset */
        nullptr,                         /* tp_base */
        nullptr,                         /* tp_dict */
        nullptr,                         /* tp_descr_get */
        nullptr,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        (initproc)PythonClassWrapper::_init,  /* tp_init */
        nullptr,                         /* tp_alloc */
        PythonClassWrapper::_new,             /* tp_new */
    };


///////////////////////////////////////////////////

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
            }
            if (PyObject_TypeCheck(&pyobj, &PythonClassWrapper< C_bare >::Type)){
                return * (typename std::remove_reference<C_type>::type*)reinterpret_cast<PythonClassWrapper< C_bare >* >(&pyobj)->get_CObject();
            }
            return * (typename std::remove_reference<C_type>::type*)reinterpret_cast<PythonClassWrapper< C_type >* >(&pyobj)->get_CObject();
        }
    };


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

    template< typename T>
    T toCObject( PyObject& pyobj){
        return CObjectConversionHelper<T>::toCObject(pyobj);
    }

}
#endif
