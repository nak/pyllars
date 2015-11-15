namespace __pyllars_internal{
/*********
 * Class to define Python wrapper to C class/type
 **/
template< typename CClass, typename ...Bases>
struct PythonClassWrapper;


template< typename CClass,
          typename... Bases>
struct PythonCPointerWrapper;

}

#ifndef __PYLLARS__INTERNAL__POINTER_H
#define __PYLLARS__INTERNAL__POINTER_H

#include <Python.h>
#include <structmember.h>
#include <limits.h>
#include <limits>
#include <vector>
#include <map>
#include <functional>

// TODO (jrusnak#1#): All adding of bases, but not through template parameter....
namespace __pyllars_internal {

    ////////////////////////////////
    // Helper functions

    template< typename c_type>
    PyObject* toPyObject( const c_type &var);

    template<int ...S>
    struct container {
        container( PyObject* const objs):pyobjs(objs){}
        PyObject* const pyobjs;
    };

    template<int N, int ...S>
    struct argGenerator : argGenerator<N-1, N-1, S...> {

    };

    template<int ...S>
    struct argGenerator<0, S...> {
      typedef container<S...> type;

    };


    template< typename CClass, typename ReturnType, typename ...Args, typename ...PyO>
    static ReturnType call_methodC( ReturnType (CClass::*method)(Args...),
                                    typename std::remove_reference<CClass>::type &self,
                                    PyObject *pytuple, PyObject *kwds, PyO* ...pyargs);

    template<typename CClass,typename ReturnType, typename ...Args, int ...S, const char* const... names>
    static ReturnType call_methodBase( ReturnType (CClass::*method)(Args...),
                                        typename std::remove_reference<CClass>::type &self,
                                        PyObject *args, PyObject *kwds, container<S...> s) {
        return call_methodC(method, self, args, kwds, PyTuple_GetItem(s.pyobjs,S)...);
    }

    template<typename CClass,typename ReturnType, typename ...Args>
    static ReturnType call_method( ReturnType ( CClass::*method)(Args...),
                                   typename std::remove_reference<CClass>::type &self,
                                   PyObject *args, PyObject *kwds );

     /**
     * This class is needed to prevent ambiguities in add_method
     **/
    template< const char* const name, class CClass, typename ReturnType, typename ...Args>
    class NonPrimitive{
    public:
       typedef ReturnType(CClass::*method_t)(Args...);
       typedef ReturnType CClass::* member_t;

       static method_t method;
       static member_t member;

       static PyObject* call(PyObject* self, PyObject* args, PyObject* kwds){
            if(!self) return nullptr;
            PythonClassWrapper<CClass>* _this = (PythonClassWrapper<CClass>*)self;
            if(_this->_CObject){
                if (method){
                    try{
                        return toPyObject<ReturnType>(call_method<CClass, ReturnType, Args...>
                            (method, *_this->_CObject,args, kwds));
                    } catch(...){
                        return nullptr;
                    }
                } else if(member){
                    return toPyObject<ReturnType>(*(_this->_CObject).*member);
                }
            }
            return nullptr;
       }
    };
     template< const char* const name, class CClass, typename ReturnType, typename ...Args>
     typename NonPrimitive<name, CClass, ReturnType, Args...>::method_t NonPrimitive<name, CClass, ReturnType, Args...>::method;
    template< const char* const name, class CClass, typename ReturnType, typename ...Args>
     typename NonPrimitive<name, CClass, ReturnType, Args...>::member_t NonPrimitive<name, CClass, ReturnType, Args...>::member;

   template<typename CClass,typename ReturnType, typename ...Args>
    static ReturnType call_method( ReturnType ( CClass::*method)(Args...),
                                   typename std::remove_reference<CClass>::type &self,
                                   PyObject *args, PyObject *kwds ){

        return call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type(args));

    }


    static PyMethodDef emptyMethods[] = {nullptr};
    static PyMemberDef emptyMembers[] = {nullptr};
    //
    /////////////////////////

    /*********
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

        typedef typename std::remove_reference<CClass>::type* (*constructor)( PyObject *args, PyObject *kwds);
        static void add_constructor( constructor c){
            _constructors.push(c);
        }

    ////////////////////////////////////////////////////

         /**
         * Add a type definition to the given module
         * @return: 0 on success, negative otherwise
         **/
        static int addType( PyObject* const to_module) {
            if (PyType_Ready(&Type) < 0)
                return -1;


            Py_INCREF( (&Type) );
            PyModule_AddObject(to_module, name, (PyObject *)&Type);

            if (PyType_Ready(&Type) < 0)
                return -1;

            return 0;
        }


        template<const char* const name,  typename ReturnType, typename ...Args>
        static void add_method( typename NonPrimitive<name, CClass, ReturnType, Args...>::method_t method) {
            static const char* const doc = "Call method ";
            char *doc_string = new char[strlen(name) +strlen(doc)]+1;
            snprintf(doc_string, strlen(name) +strlen(doc), "%s%s",doc,name);

            PyMethodDef pyMeth = {
              name,
              (PyCFunction)NonPrimitive<name, CClass_NoRef, ReturnType, Args...>::call,
              METH_KEYWORDS,
              doc_string
            };

            NonPrimitive<name, CClass, ReturnType, Args...>::method = method;
            _add_method(pyMeth);
        }



        template< const char* const name, typename Type>
        static void add_member( typename NonPrimitive<name, CClass_NoRef, Type>::member_t member){
            static const char* const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) +strlen(doc)]+1;
            snprintf(doc_string, strlen(name) +strlen(doc)+1, "%s%s",doc,name);
            static const char* const getter_prefix = "get_";
            char *getter_name = new char[strlen(name) +strlen(getter_prefix)]+1;
            snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
            NonPrimitive<name, CClass_NoRef, Type>::member = member;
            PyMethodDef pyMeth = {getter_name,
                    (PyCFunction)NonPrimitive<name, CClass_NoRef, Type>::call,
                    METH_KEYWORDS,
                    doc_string
              };
            _add_method(pyMeth);
        }


        friend class PythonCPointerWrapper<CClass, Bases...>;

    //private:
        CClass_NoRef* _CObject;

        static int
        _init( PythonClassWrapper *self, PyObject *args, PyObject *kwds);


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


    private:

        static size_t s_methodIndex;
        static size_t s_memberIndex;

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


template<const char* const name>
class NonPrimitive<name, long int, long int>{
public:
   typedef int member_t;
   static PyObject* call(PyObject*, PyObject*, PyObject*){
    return nullptr;
   }
};
template<const char* const name>
class NonPrimitive<name, long long int, long long int>{
public:
   typedef int member_t;
   static PyObject* call(PyObject*, PyObject*, PyObject*){
    return nullptr;
   }
};

template<const char* const name>
class NonPrimitive<name, int,int>{
public:
   typedef int member_t;
   static PyObject* call(PyObject*, PyObject*, PyObject*){
    return nullptr;
   }
};


template<const char* const name>
class NonPrimitive<name, short int, short int>{
public:
   typedef int member_t;
   static PyObject* call(PyObject*, PyObject*, PyObject*){
    return nullptr;
   }
};

template<const char* const name>
class NonPrimitive<name, char,char>{
public:
   typedef int member_t;
   static PyObject* call(PyObject*, PyObject*, PyObject*){
    return nullptr;
   }
};

template<const char* const name>
class NonPrimitive<name, long unsigned int,long unsigned int>{
public:
   typedef int member_t;
   static PyObject* call(PyObject*, PyObject*, PyObject*){
    return nullptr;
   }
};
template<const char* const name>
class NonPrimitive<name, long long unsigned int,long long unsigned int>{
public:
   typedef int member_t;
   static PyObject* call(PyObject*, PyObject*, PyObject*){
    return nullptr;
   }
};
template<const char* const name>
class NonPrimitive< name, unsigned int, unsigned int>{
public:
   typedef int member_t;
   static PyObject* call(PyObject*, PyObject*, PyObject*){
    return nullptr;
   }
};
template<const char* const name>
class NonPrimitive<name, short unsigned int,short unsigned int>{
public:
   typedef int member_t;
   static PyObject* call(PyObject*, PyObject*, PyObject*){
    return nullptr;
   }
};
template<const char* const name>
class NonPrimitive< name, unsigned char, unsigned char>{
public:
   typedef int member_t;
   static PyObject* call(PyObject*, PyObject*, PyObject*){
    return nullptr;
   }
};
template<const char* const name>
class NonPrimitive<name, double, double>{
public:
   typedef int member_t;
   static PyObject* call(PyObject*, PyObject*, PyObject*){
    return nullptr;
   }
};
template<const char* const name>
class NonPrimitive<name, float, float>{
public:
   typedef int member_t;
   static PyObject* call(PyObject*, PyObject*, PyObject*){
    return nullptr;
   }
};
template<const char* const name, typename CClass, typename ...Args>
class NonPrimitive<name, CClass, void, Args...>{
public:
    typedef void(CClass::*method_t)(Args...);
    typedef int member_t;
    static method_t method;
    static PyObject* call(PyObject* self, PyObject* args, PyObject* kwds){
         if(!self) return nullptr;
        PythonClassWrapper<CClass>* _this = (PythonClassWrapper<CClass>*)self;
        if(_this->_CObject){
            if (method){
                try{
                    call_method<CClass, void, Args...>
                        (method, *_this->_CObject,args, kwds);
                    return Py_None;
                } catch(...){
                    return nullptr;
                }
            }
        }
        return nullptr;
    }
};
template< const char* const name, class CClass, typename ...Args>
typename NonPrimitive<name, CClass, void, Args...>::method_t NonPrimitive<name, CClass, void, Args...>::method;

template<const char* const name, typename T>
class NonPrimitive<name, T*, T*>{
public:
   typedef int member_t;
   static PyObject* call(PyObject*, PyObject*, PyObject*){
    return nullptr;
   }
};
template<const char* const name, typename T>
class NonPrimitive<name, T&, T&>{
public:
   typedef int member_t;
   static PyObject* call(PyObject*, PyObject*, PyObject*){
    return nullptr;
   }
};
template< typename CClass,
          typename... Bases >
std::vector<PyMethodDef> PythonClassWrapper<CClass,  Bases...>::_methodCollection = std::vector<PyMethodDef>(emptyMethods, emptyMethods+1);

template< typename CClass,
          typename... Bases >
std::vector<PyMemberDef> PythonClassWrapper<CClass,  Bases...>::_memberCollection = std::vector<PyMemberDef>(emptyMembers, emptyMembers+1);

#include "pyllars/pyllars_utils.h"


  /**
   * template fnction to convert python to C object
   **/

  template< typename C_type>
  C_type toCObject( PyObject& pyobj){
    if(&pyobj == nullptr){
        throw "Invalid argument for conversion";
    }
    if (PyObject_TypeCheck(&pyobj, &PythonCPointerWrapper<C_type>::Type)){
      return *((typename std::remove_reference<C_type>::type*)(reinterpret_cast<PythonCPointerWrapper<C_type> *>(&pyobj)->ptr()));
    }
    if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper< C_type >::Type)){
       throw "Invalid type converting to C object";
    }
    return * (typename std::remove_reference<C_type>::type*)reinterpret_cast<PythonClassWrapper< C_type >* >(&pyobj)->get_CObject();
  }

  /// explicit instantiations for fundamental types
  template<>
  long int toCObject< long int>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    } else if (PyLong_Check(&pyobj)){
        return PyLong_AsLong( &pyobj );
    } else if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper<long int>::Type)){
       throw "Invalid type converting to C object";
    }
    return *reinterpret_cast<PythonClassWrapper<long int>* >(&pyobj)->get_CObject();
  }

  template<>
  int toCObject< int>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    }else if (PyLong_Check(&pyobj)){
        return PyLong_AsLong( &pyobj );
    } else if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper< int >::Type)){
       throw "Invalid type converting to C object";
    }
    return *reinterpret_cast<PythonClassWrapper< int >* >(&pyobj)->get_CObject();
  }

  template<>
  short int toCObject< short int>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    }else if (PyLong_Check(&pyobj)){
        return PyLong_AsLong( &pyobj );
    } else if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper< short int >::Type)){
       throw "Invalid type converting to C object";
    }
    return *reinterpret_cast<PythonClassWrapper< short int >* >(&pyobj)->get_CObject();
  }

  template<>
  char toCObject< char>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    }else if (PyLong_Check(&pyobj)){
        return PyLong_AsLong( &pyobj );
    } else if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper<char>::Type)){
       throw "Invalid type converting to C object";
    }
    return *reinterpret_cast<PythonClassWrapper<char>* >(&pyobj)->get_CObject();
  }

  template<>
  unsigned long int toCObject< unsigned long int>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    }else if (PyLong_Check(&pyobj)){
        return PyLong_AsLong( &pyobj );
    } else if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper<unsigned long int>::Type)){
       throw "Invalid type converting to C object";
    }
    return *reinterpret_cast<PythonClassWrapper<unsigned long int>* >(&pyobj)->get_CObject();
  }

  template<>
  unsigned int toCObject< unsigned int>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    }else if (PyLong_Check(&pyobj)){
        return PyLong_AsLong( &pyobj );
    } else if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper<unsigned short int>::Type)){
       throw "Invalid type converting to C object";
    }
    return *reinterpret_cast<PythonClassWrapper<unsigned int>* >(&pyobj)->get_CObject();
  }

  template<>
  unsigned short int toCObject< unsigned short int>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    }else if (PyLong_Check(&pyobj)){
        return PyLong_AsLong( &pyobj );
    } else if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper<unsigned short int>::Type)){
       throw "Invalid type converting to C object";
    }
    return *reinterpret_cast<PythonClassWrapper<unsigned short int >* >(&pyobj)->get_CObject();
  }


  template<>
  unsigned char toCObject< unsigned char>( PyObject& pyobj){
    if (PyInt_Check( &pyobj)){
      return PyInt_AsLong( &pyobj );
    }else if (PyLong_Check(&pyobj)){
        return PyLong_AsLong( &pyobj );
    } else if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper<unsigned char >::Type)){
       throw "Invalid type converting to C object";
    }
    return *reinterpret_cast<PythonClassWrapper< unsigned char>* >(&pyobj)->get_CObject();
  }

  template<>
  float toCObject< float>( PyObject& pyobj){
    if (PyFloat_Check(&pyobj)){
       return PyFloat_AsDouble( &pyobj );
    } else if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper<double>::Type)){
       throw "Invalid type converting to C object";
    }
    return *reinterpret_cast<PythonClassWrapper<double>* >(&pyobj)->get_CObject();
  }

  template<>
  double toCObject< double>( PyObject& pyobj){
    if (PyFloat_Check(&pyobj)){
        return PyFloat_AsDouble( &pyobj );
    } else if (!PyObject_TypeCheck(&pyobj, &PythonClassWrapper<double>::Type)){
       throw "Invalid type converting to C object";
    }
    return *reinterpret_cast<PythonClassWrapper<double>* >(&pyobj)->get_CObject();
  }

template<typename CClass, typename ...Bases>
std::vector<typename PythonClassWrapper<CClass, Bases...>::constructor > PythonClassWrapper<CClass, Bases...>::_constructors;

template<>
int
PythonClassWrapper<int>::_init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
    if (!self) {
        return -1;
    }
    static const char* kwlist[] = {"value",nullptr};
    long long value;
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "i", (char**)kwlist,
                                      &value))
        return -1;
    if (value < INT_MIN or value > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError,"Integer value out of range of int");
        return -1;
    }
    self->_CObject = new int((int)value);
    return 0;
}

template<>
int
PythonClassWrapper<char>::_init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
    if (!self) {
        return -1;
    }
    static const char* kwlist[] = {"value",nullptr};
    long long value;
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "i", (char**)kwlist,
                                      &value))
        return -1;
    if (value > CHAR_MAX or value < CHAR_MIN) {
        PyErr_SetString(PyExc_OverflowError,"Integer value out of range of char");
        return -1;
    }
    self->_CObject = new char((char)value);
    return 0;
}

template<>
int
PythonClassWrapper<short>::_init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
    if (!self) {
        return -1;
    }
    static const char* kwlist[] = {"value",nullptr};
    long long value;
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "i", (char**)kwlist,
                                      &value))
        return -1;
    if (value > SHRT_MAX or value < SHRT_MIN) {
        PyErr_SetString(PyExc_OverflowError,"Integer value out of range of short");
        return -1;
    }
    self->_CObject = new short((short)value);
    return 0;
}


template<>
int
PythonClassWrapper<long>::_init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
    if (!self) {
        return -1;
    }
    static const char* kwlist[] = {"value",nullptr};
    long long value;
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "i", (char**)kwlist,
                                      &value))
        return -1;
    if (value > LONG_MAX or value < LONG_MIN) {
        PyErr_SetString(PyExc_OverflowError,"Integer value out of range of long");
        return -1;
    }
    self->_CObject = new long((long)value);
    return 0;
}

template<>
int
PythonClassWrapper<long long>::_init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
    if (!self) {
        return -1;
    }
    static const char* kwlist[] = {"value",nullptr};
    long long value;
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "i", (char**)kwlist,
                                      &value))
        return -1;

    self->_CObject = new long long((long long)value);
    return 0;
}


template<>
int
PythonClassWrapper<float>::_init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
    if (!self) {
        return -1;
    }
    static const char* kwlist[] = {"value",nullptr};
    double value;
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "f", (char**)kwlist,
                                      &value))
        return -1;
    if (fabs(value) > std::numeric_limits<float>::max() ) {
        PyErr_SetString(PyExc_OverflowError,"Integer value out of range of float");
        return -1;
    }
    self->_CObject = new float((float)value);
    return 0;
}

template<>
int
PythonClassWrapper<double>::_init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
    if (!self) {
        return -1;
    }
    static const char* kwlist[] = {"value",nullptr};
    double value;
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "f", (char**)kwlist,
                                      &value))
        return -1;
    if (value > std::numeric_limits<double>::max() || value < std::numeric_limits<double>::min() ) {
        PyErr_SetString(PyExc_OverflowError,"Integer value out of range of float");
        return -1;
    }
    self->_CObject = new double((double)value);
    return 0;
}

template<typename CClass, typename ...Bases>
int
PythonClassWrapper<CClass, Bases...>::_init( PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
      if (!self) {
        return -1;
      }
      for (auto it = _constructors.begin(); it != _constructors.end(); ++it){
          if ((self->_CObject = (*it)(args, kwds)) != nullptr){
              break;
          }
      }

      //if (self->_CObject == nullptr) return -1;
      return 0;
}


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


  template< typename CClass, typename ReturnType, typename ...Args, typename ...PyO>
    static ReturnType call_methodC( ReturnType (CClass::*method)(Args...),
                                    typename std::remove_reference<CClass>::type &self,
                                    PyObject *pytuple, PyObject *kwds, PyO* ...pyargs){
       return  (self.*method)(toCObject<Args>(*pyargs)...);
    }

////////////////////////////


template< typename CClass,
          typename... Bases>
struct PythonCPointerWrapper {

    PyObject_HEAD

    static int addType( PyObject* const to_module) {
        if (PyType_Ready(&Type) < 0)
            return -1;


        Py_INCREF( (&Type) );
        PyModule_AddObject(to_module, name, (PyObject *)&Type);

        if (PyType_Ready(&Type) < 0)
            return -1;

        return 0;
    }

    static void _dealloc( PythonCPointerWrapper* self) {
        if( self != nullptr ) {
            self->_content = nullptr;
            self->ob_type->tp_free((PyObject*)self);
        }
    }

    static PyObject *
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        PythonCPointerWrapper *self;

        self = (PythonCPointerWrapper*)type->tp_alloc(type, 0);
        if (self != nullptr) {
            self->_content = nullptr;
            self->_depth = 1;
        }
        return (PyObject *)self;

    }

    static int
    _init( PythonCPointerWrapper *self, PyObject *args, PyObject *kwds) {
        PyObject *target=nullptr;

        static const char *kwlist[] = {"target", nullptr};

        int status = -1;
        if (self != nullptr &&
                PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwlist,
                                            &target)) {

            if (target && target != Py_None) {
                if (PyObject_IsInstance( target, (PyObject*)&PythonCPointerWrapper::Type)) {
                    self->_depth = ((PythonCPointerWrapper*)target)->_depth +1;
                    PyObject* obj  = PyObject_CallObject((PyObject*)&Type, Py_None);
                    self->_content = &(reinterpret_cast<PythonCPointerWrapper*>(obj)->_content);
                    status = 0;
                } else if ( PyObject_IsInstance( target, (PyObject*)&PythonClassWrapper<CClass>::Type )) {
                    self->_depth = 1;
                    typename std::remove_reference<CClass>::type* obj = reinterpret_cast< PythonClassWrapper<CClass> *>(target)->get_CObject();
                    self->_content = (void*)obj;
                    status = 0;
                }


            } else if (target==Py_None) {
                self->_content = nullptr;
                status = 0;
            }
        }
        return status;
    }


    static PyTypeObject Type;

    static const char* const name;

    void* ptr(){
        return _content;
    }

private:
    static PyObject* _at( PyObject * self, PyObject* args, PyObject* kwargs) {
        static const char* kwlist[] = {"index",nullptr};

        PyObject* result = Py_None;
        int index = -1;

        if( !PyArg_ParseTupleAndKeywords( args,kwargs, "i", (char**)kwlist, &index)) {
            return nullptr;
        }
        if (reinterpret_cast<PythonCPointerWrapper*>(self)->_depth == 1) {

            result =  PyObject_CallObject( (PyObject*)&PythonClassWrapper<CClass>::Type,
                                           Py_None);
            if(result)
                reinterpret_cast<PythonClassWrapper<CClass>* >(result)->_CObject = &((typename std::remove_reference<CClass>::type*)reinterpret_cast<PythonCPointerWrapper*>(self)->_content)[index];

        } else {

            result = PyObject_CallObject( (PyObject*)&PythonCPointerWrapper::Type,
                                          Py_None);
            if(result) {
                reinterpret_cast<PythonCPointerWrapper*>(result)->_content =
                    reinterpret_cast<void**>(reinterpret_cast<PythonCPointerWrapper*>(self)->_content)[index];
            }
        }

        return result;
    }

    static std::vector<PyMethodDef> s_methodCollection;
    static std::vector<PyMemberDef> s_memberCollection;
    static PyMethodDef s_methodList[];
    static PyMemberDef s_memberList[];

    //  !!!!!!CAUTION: If you change the layout you must consider changing
    //  !!!!!!The offset computation below
    void* _content;
    size_t _depth;
};



template<typename CClass, typename ...Bases>
template< typename ...Args, const char * const ...names, typename ...PyO>
 std::remove_reference<CClass>* PythonClassWrapper<CClass, Bases...>::constructorC( PyObject *pytuple, PyObject *kwds, PyO*... pyargs){
    static char* kwlist[] = {names..., nullptr};
    char format[sizeof...(Args)+1] = {'O'};
    format[sizeof...(Args)] = 0;

    if(!PyArg_ParseTupleAndKeywords(pytuple, kwds, format, kwlist, pyargs...)){
      return nullptr;
    }
    return new CClass(toCObject<Args>(pyargs)...);
}


template< typename CClass,
          typename... Bases >
PyTypeObject PythonCPointerWrapper<CClass,  Bases...>::Type = {
    PyObject_HEAD_INIT(nullptr)
    0,                         /*ob_size*/
    PythonCPointerWrapper::name,             /*tp_name*/
    sizeof(PythonCPointerWrapper),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PythonCPointerWrapper<CClass,  Bases...>::_dealloc, /*tp_dealloc*/
    nullptr,                         /*tp_print*/
    nullptr,                         /*tp_getattr*/
    nullptr,                         /*tp_setattr*/
    nullptr,                         /*tp_compare*/
    nullptr,                         /*tp_repr*/
    nullptr,                         /*tp_as_number*/
    nullptr,                         /*tp_as_containeruence*/
    nullptr,                         /*tp_as_mapping*/
    nullptr,                         /*tp_hash */
    nullptr,                         /*tp_call*/
    nullptr,                         /*tp_str*/
    nullptr,                         /*tp_getattro*/
    nullptr,                         /*tp_setattro*/
    nullptr,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PythonCPointerWrapper object",           /* tp_doc */
    nullptr,		               /* tp_traverse */
    nullptr,		               /* tp_clear */
    nullptr,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    nullptr,		               /* tp_iter */
    nullptr,		               /* tp_iternext */
    PythonCPointerWrapper::s_methodCollection.data(),             /* tp_methods */
    PythonCPointerWrapper::s_memberCollection.data(),             /* tp_members */
    nullptr,                         /* tp_getset */
    nullptr,                         /* tp_base */
    nullptr,                         /* tp_dict */
    nullptr,                         /* tp_descr_get */
    nullptr,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PythonCPointerWrapper<CClass,  Bases...>::_init,      /* tp_init */
    nullptr,                         /* tp_alloc */
    PythonCPointerWrapper<CClass,  Bases...>::_new,                 /* tp_new */
};

template<typename CClass, typename ...Bases>
PyMethodDef PythonCPointerWrapper<CClass, Bases...>::s_methodList[] = {
    {
        "at", (PyCFunction)PythonCPointerWrapper::_at,
        METH_VARARGS,
        "Return the dereferenced item at provided index"
    },
    {nullptr}  /* Sentinel */
};
template<typename CClass, typename ...Bases>
std::vector<PyMethodDef> PythonCPointerWrapper<CClass, Bases...>::s_methodCollection =
   std::vector<PyMethodDef>(PythonCPointerWrapper::s_methodList, PythonCPointerWrapper::s_methodList+2);

struct UNUSED{
  PyObject_HEAD
};
template<typename CClass, typename ...Bases>
PyMemberDef PythonCPointerWrapper<CClass, Bases...>::s_memberList[] = { {
            //  !!!!!!!!!CAUTION : CANNOT USE OFFSETOF HERE, SO HAVE TO COMPUTE, BUT IS
            //  !!!!!!!!!DEPENDENT ON LAYOUT
              (char*)"depth", T_OBJECT_EX, sizeof(PythonCPointerWrapper) - sizeof(UNUSED) + sizeof(void*), 0,
              (char*)"depth of pointer"
            },
            {nullptr}/*Sentinel*/
};
template<typename CClass, typename ...Bases>
std::vector<PyMemberDef> PythonCPointerWrapper<CClass, Bases...>::s_memberCollection=
   std::vector<PyMemberDef>(PythonCPointerWrapper::s_memberList, PythonCPointerWrapper::s_memberList+2);;


template<>
const char* const PythonClassWrapper<int>::name = "type_int";

template<>
const char* const PythonCPointerWrapper<int>::name = "ptr_int";


template<>
const char* const PythonClassWrapper<long>::name = "type_long";

template<>
const char* const PythonCPointerWrapper<long>::name = "ptr_long";

template<>
const char* const PythonClassWrapper<short>::name = "type_short";

template<>
const char* const PythonCPointerWrapper<short>::name = "ptr_short";

template<>
const char* const PythonClassWrapper<char>::name = "type_char";

template<>
const char* const PythonCPointerWrapper<char>::name = "ptr_char";


template<>
const char* const PythonClassWrapper<unsigned long>::name = "type_unsigned_long";

template<>
const char* const PythonCPointerWrapper<unsigned long>::name = "ptr_unsigned_long";



template<>
const char* const PythonClassWrapper<unsigned short>::name = "type_unsigned_short";

template<>
const char* const PythonCPointerWrapper<unsigned short>::name = "ptr_unsigned_short";


template<>
const char* const PythonClassWrapper<unsigned char>::name = "type_unsigned_char";

template<>
const char* const PythonCPointerWrapper<unsigned char>::name = "ptr_unsigned_char";

template<>
const char* const PythonClassWrapper<double>::name = "type_unsigned_long";

template<>
const char* const PythonCPointerWrapper<double>::name = "ptr_unsigned_long";


template<>
const char* const PythonClassWrapper<float>::name = "type_unsigned_long";

template<>
const char* const PythonCPointerWrapper<float>::name = "ptr_unsigned_long";


////


template<>
const char* const PythonClassWrapper<int const>::name = "type_int";

template<>
const char* const PythonCPointerWrapper<int const>::name = "ptr_int";


template<>
const char* const PythonClassWrapper<long const>::name = "type_long";

template<>
const char* const PythonCPointerWrapper<long const>::name = "ptr_long";

template<>
const char* const PythonClassWrapper<short const>::name = "type_short";

template<>
const char* const PythonCPointerWrapper<short const>::name = "ptr_short";

template<>
const char* const PythonClassWrapper<char const>::name = "type_char";

template<>
const char* const PythonCPointerWrapper<char const>::name = "ptr_char";


template<>
const char* const PythonClassWrapper<unsigned long const>::name = "type_unsigned_long";

template<>
const char* const PythonCPointerWrapper<unsigned long const>::name = "ptr_unsigned_long";



template<>
const char* const PythonClassWrapper<unsigned short const>::name = "type_unsigned_short";

template<>
const char* const PythonCPointerWrapper<unsigned short const>::name = "ptr_unsigned_short";


template<>
const char* const PythonClassWrapper<unsigned char const>::name = "type_unsigned_char";

template<>
const char* const PythonCPointerWrapper<unsigned char const>::name = "ptr_unsigned_char";

template<>
const char* const PythonClassWrapper<double const>::name = "type_unsigned_long";

template<>
const char* const PythonCPointerWrapper<double const>::name = "ptr_unsigned_long";


template<>
const char* const PythonClassWrapper<float const>::name = "type_unsigned_long";

template<>
const char* const PythonCPointerWrapper<float const>::name = "ptr_unsigned_long";


template< typename CClass,
          typename... Bases >
void init_pyllars_pointer( PyObject* m ) {
    if (m == nullptr)
        return;
    if (PythonCPointerWrapper<CClass, Bases...>::addType( m ) < 0)
        return;
    if (PythonClassWrapper<CClass>::addType(m) < 0)
        return;

}

}

#endif
