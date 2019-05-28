//  PREDECLARATIONS

#ifndef __PYLLARS__INTERNAL__CLASS_WRAPPER_H
#define __PYLLARS__INTERNAL__CLASS_WRAPPER_H

#include <Python.h>
#include <structmember.h>
#include <limits.h>
#include <limits>
#include <vector>
#include <map>
#include <functional>
#include <Python.h>
#include <tupleobject.h>

#include "pyllars_type_traits.hpp"
#include "pyllars.hpp"
#include "pyllars_defns.hpp"
#include "pyllars_staticfunctionsemantics.hpp"
#include "pyllars_classmembersemantics.hpp"
#include "pyllars_membersemantics.hpp"
#include "pyllars_methodcallsemantics.hpp"
#include "pyllars_containment.hpp"
#include "pyllars_object_lifecycle.hpp"

typedef const char cstring[];
static constexpr cstring operatormapname = "operator[]";
static constexpr cstring operatormapnameconst = "operator[]const";

static constexpr cstring OP_UNARY_INV = "__invert__";
static constexpr cstring OP_UNARY_POS = "__pos__";
static constexpr cstring OP_UNARY_NEG = "__neg__";

static constexpr cstring OP_BINARY_ADD = "__add__";
static constexpr cstring OP_BINARY_SUB = "__sub__";
static constexpr cstring OP_BINARY_MUL = "__mul__";
static constexpr cstring OP_BINARY_DIV = "__div__";
static constexpr cstring OP_BINARY_AND = "__and__";
static constexpr cstring OP_BINARY_OR  = "__or__";
static constexpr cstring OP_BINARY_XOR = "__xor__";
static constexpr cstring OP_BINARY_MOD = "__mod__";
static constexpr cstring OP_BINARY_LSHIFT = "__lshift__";
static constexpr cstring OP_BINARY_RSHIFT = "__lshift__";

static constexpr cstring OP_BINARY_IADD = "__iadd__";
static constexpr cstring OP_BINARY_ISUB = "__isub__";
static constexpr cstring OP_BINARY_IMUL = "__imul__";
static constexpr cstring OP_BINARY_IDIV = "__idiv__";
static constexpr cstring OP_BINARY_IAND = "__iand__";
static constexpr cstring OP_BINARY_IOR  = "__ior__";
static constexpr cstring OP_BINARY_IXOR = "__ixor__";
static constexpr cstring OP_BINARY_IMOD = "__imod__";
static constexpr cstring OP_BINARY_ILSHIFT = "__ilshift__";
static constexpr cstring OP_BINARY_IRSHIFT = "__iRshift__";

static const char* const emptylist[] = {nullptr};

namespace __pyllars_internal {

    typedef int (*_setattrfunc)(PyObject*, PyObject*, void*);
    typedef PyObject* (*_getattrfunc)(PyObject*, void*);

    template<typename T>
    struct ObjectContainer;

    template<typename T>
    class InitHelper;

    template<typename func_t>
    struct func_traits;

    template<typename functype>
    struct PythonFunctionWrapper;

    /**
     * Class to define Python wrapper to C class/type
     **/
    template<typename T>
    struct PythonClassWrapper<T, typename std::enable_if< is_rich_class<T>::value>::type>: public CommonBaseWrapper {
        // Convenience typedefs
        typedef CommonBaseWrapper::Base Base;

        typedef typename std::remove_reference<T>::type T_NoRef;
        typedef typename core_type<T>::type CClass;
        typedef PyTypeObject *TypePtr;



        template<typename C>
        friend
        class InitHelper;

        /**
         * Initialize python type if needed
         * @return Python-based PyTypeObject associated with T
         */
        static PyTypeObject* getType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &_Type;
        }

        /**
         * Python initialization of underlying type, called to init and register type with
         * underlying Python system
         *
         * @param name: Python simple-name of the type
         **/
        static int initialize(){return _initialize(_Type);}

        /**
         * create a Python object of this class type
         **/
        static PythonClassWrapper *fromCObject(T_NoRef & cobj);

         /**
          * return Python object representing the address of the contained object
          **/
        static PyObject *addr(PyObject *self, PyObject *args) ;

        /**
         * Add a base class to this Python definition (called before initialize)
         **/
        template<typename Base>
        static void addBaseClass() ;

        /**
         * add a constructor method with given compile-time-known name to the contained collection
         *
         * @templateparams Args varidic list of templat argument types
         *
         * @param kwlist: list of keyword names of parameter names for name association
         **/
        template< typename ...Args>
        static void addConstructor(const char *const kwlist[]){
            addConstructorBase(kwlist, &create<Args...>);
        }

        /**
         * add a method with a ReturnType to be avaialable in this classes' corresponding  Python type object
         *
         * @templateparameter name: name of the method (as it will appear in Python, but should be same as C name)
         * @templateparam func_type: function signature in form ReturnType(Args...)
         * @templateparam method: pointer to method to be added
         *
         * @param method: the pointer to the metho to be added
         * @param kwlist: list of keyword names of araguments
         **/
        template<const char *const name, const char *const kwlist[], typename func_type, func_type *method>
        static void addClassMethod();

        /**
         * add a class-wide (static) member
         **/
        static void addClassMember(const char *const name, PyObject *pyobj) ;

        /**
         * Add an enum value to the class
         */
        template<typename EnumT>
        static int addEnumValue( const char* const name, EnumT value){

            addClassMember(name, PyInt_FromLong((long int)value));
            return 0;
        }

        static int addEnumClassValue( const char* const name, const T value){
            if constexpr (std::is_constructible<T>::value) {
                _classEnumValues[name] = new T(value);
            }
            return 0;
        }

        /**
         * Add a mutable bit field to this Python type definition
         **/
        template<const char *const name, typename FieldType, const size_t bits>
        static void addBitField(
                typename BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::getter_t &getter,
                typename BitFieldContainer<T_NoRef>::template Container<name, FieldType,  bits>::setter_t *setter=nullptr);

        /**
         * add a getter method for the given compile-time-known named public class member
         **/
        template<const char *const name, typename FieldType>
        static void addAttribute(typename MemberContainer<name, T_NoRef, FieldType>::member_t member);

        /**
         * add a getter method for the given compile-time-known named public static class member
         **/
        template<const char *const name, typename FieldType>
        static void addClassAttribute(FieldType *member);

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char *const name, const char* const kwlist[], typename method_t, method_t method>
        static void addMethod();

        template<typename method_t, method_t method>
        static void addInvOperator(){
            Op<OP_UNARY_INV, method_t, method>::addUnaryOperator();
        }

        template<typename method_t, method_t method>
        static void addPosOperator()
        { Op<OP_UNARY_POS, method_t, method>::addUnaryOperator();}

        template<typename method_t, method_t method>
        static void addNegOperator()
        { Op<OP_UNARY_NEG, method_t, method>::addUnaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addAddOperator(){
            BinaryOp<OP_BINARY_ADD, kwlist, method_t, method>::addBinaryOperator();
        }

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addSubOperator(){
            BinaryOp<OP_BINARY_SUB, kwlist, method_t, method>::addBinaryOperator();
        }

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addMulOperator(){
            BinaryOp<OP_BINARY_MUL, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addDivOperator(){
            BinaryOp<OP_BINARY_DIV, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addAndOperator(){
            BinaryOp<OP_BINARY_AND, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addOrOperator(){
            BinaryOp<OP_BINARY_OR, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addXorOperator(){
            BinaryOp<OP_BINARY_XOR, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addLshiftOperator(){
            BinaryOp<OP_BINARY_LSHIFT, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addRshiftOperator(){
            BinaryOp<OP_BINARY_RSHIFT, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addModOperator(){
            BinaryOp<OP_BINARY_MOD, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addInplaceAddOperator(){
            BinaryOp<OP_BINARY_IADD, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addInplaceSubOperator(){
            BinaryOp<OP_BINARY_ISUB, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addInplaceMulOperator(){
            BinaryOp<OP_BINARY_IMUL, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addInplaceModOperator(){
            BinaryOp<OP_BINARY_IMOD, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addInplaceLshiftOperator(){
            BinaryOp<OP_BINARY_ILSHIFT, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addInplaceRshiftOperator(){
            BinaryOp<OP_BINARY_IRSHIFT, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addInplaceAndOperator(){
            BinaryOp<OP_BINARY_IAND, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addInplaceOrOperator(){
            BinaryOp<OP_BINARY_IOR, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addInplaceXorOperator(){
            BinaryOp<OP_BINARY_IXOR, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2],typename method_t, method_t method>
        static void addInplaceDicOperator(){
            BinaryOp<OP_BINARY_IDIV, kwlist,method_t, method>::addBinaryOperator();}

        template<const char* const kwlist[2], typename KeyType, typename method_t, method_t method>
        static void addMapOperatorMethod(){
            typedef typename func_traits<method_t>::ReturnType ValueType;
            if constexpr(func_traits<method_t>::is_const_method) {
                _addMapOperatorMethod < kwlist, KeyType, ValueType, ValueType(CClass::*)(KeyType) const, method > ();
            } else {
                _addMapOperatorMethod < kwlist, KeyType, ValueType, ValueType(CClass::*)(KeyType), method > ();
            }
        }

        static bool checkType(PyObject *const obj);

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &_Type;
        }

        template<typename functype>
        friend struct PythonFunctionWrapper;

        template<typename Y, typename YY>
        friend class PythonClassWrapper;

        friend
        void * toFFI(PyObject*);

        constexpr PythonClassWrapper(): _CObject(nullptr){}

        /**
         * return the C-like object associated with this Python wrapper
         */
        typename PythonClassWrapper::T_NoRef *get_CObject() ;


    protected:

        static PyObject *alloc(PyObject *cls, PyObject *args, PyObject *kwds);

        static int _initialize(PyTypeObject & Type);

        static int
        _pySetAttr(PyObject* self,  char* attrname, PyObject* value){
            if (!_member_setters.count(attrname)){
                PyErr_SetString(PyExc_ValueError, "No such attribute or attempt to set const attribute");
                return -1;
            }
            return _member_setters[attrname](self, value, nullptr);
        }

        static PyObject* _pyGetAttr(PyObject* self,  char* attrname){
            if (!_member_getters.count(attrname)){
                return PyObject_GenericGetAttr(self, PyString_FromString(attrname));
            }
            return _member_getters[attrname](self, nullptr);
        }

        // must use object container, since code may have new and delete private and container
        // will shield us from that
        //ObjectContainer<T_NoRef> *_CObject;
        T_NoRef * _CObject;

    private:

        static bool _isInitialized;

        static void addAssigner(_setattrfunc func){
            if(!_member_setters.count("this"))
                _member_setters["this"] = _pyAssign;
            _assigners.push_back(func);
        }
        template<const char* const kwlist[2], typename KeyType, typename ValueType, typename method_t, method_t method>
        static void _addMapOperatorMethod();

        template<const char* const, typename method_t, method_t method>
        struct Op;

        template<const char* const name, typename ReturnType, ReturnType(CClass::*method)()>
        struct Op<name, ReturnType(CClass::*)(), method>{
            static void addUnaryOperator();
        };

        template<const char* const name, typename ReturnType, ReturnType(CClass::*method)() const>
        struct Op<name, ReturnType(CClass::*)() const, method>{
            static void addUnaryOperator();
        };

        template<const char *const name, const char* const kwlist[2], typename method_t, method_t method>
        struct BinaryOp;

        template<const char *const name, const char* const kwlist[2], typename ReturnType, typename ArgType, ReturnType(CClass::*method)(ArgType)>
        struct BinaryOp<name, kwlist, ReturnType(CClass::*)(ArgType), method>{
            static void addBinaryOperator();
        };

        template<const char *const name, const char* const kwlist[2], typename ReturnType, typename ArgType, ReturnType(CClass::*method)(ArgType) const>
        struct BinaryOp<name, kwlist, ReturnType(CClass::*)(ArgType) const, method>{
            static void addBinaryOperator();
        };

        typedef T_NoRef* (*constructor_t)(const char *const kwlist[], PyObject *args, PyObject *kwds,
                unsigned char* const location);

        /**
         * Add a constructor for this type
         *
         * @param kwlist: keyword list of items that can be used in call parameters
         * @param c: the constructor method to be added
         **/
        static void addConstructorBase(const char *const kwlist[], constructor_t c) ;

        /**
          * Create an instance of underlying class based on python arguments converting them to C to call
          * the constructor
          *
          * @param kwlist: list of keyword names of the Python parameters
          * @param args: list of Python args from the Python call into the C constructor
          * @params kwds: keywoards bassed into the copnstructor
          **/
        template<typename ...Args>
        static T_NoRef* create(const char *const kwlist[], PyObject *args, PyObject *kwds, unsigned char*) ;

        static PyObject* getThis(PyObject* self, void*){
            return addr(self, nullptr);
        }

        static int
        _pyAssign(PyObject* self, PyObject* value, void*){
            for ( _setattrfunc assigner: _assigners){
                if( assigner(self, value, nullptr) == 0){
                    return 0;
                }
            }
            PyErr_SetString(PyExc_ValueError, "Cannot assign to given _CObject type");
            return -1;
        }

        static int
        _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds);

        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

        static void _free(void *self_);

        static void _dealloc(PyObject *self);

        template<typename ...PyO>
        static bool _parsePyArgs(const char *const kwlist[], PyObject *args, PyObject *kwds, PyO *&...pyargs);



        template <typename Z>
        struct _____fake{

        };

        /**
         * in order to convert args to a parameter list of args, need to go through
         * two lower level create functions
         **/
        template<typename ...Args>
        static T_NoRef* _createBaseBase(argument_capture<Args> ... args);

        template<typename ...Args, int ...S >
        static T_NoRef* _createBase(PyObject *args, PyObject *kwds,
                 const char *const kwlist[], container<S...> unused1, _____fake<Args> *... unused2);

        /**
         * helper methods
         **/
         template<bool is_const>
        static void _addMethod(PyMethodDef method);



        static PyObject* _mapGet(PyObject* self, PyObject* key);
        static int _mapSet(PyObject* self, PyObject* key, PyObject* value);

        typedef std::pair<const char* const*, constructor_t> ConstructorContainer;
        static std::vector<ConstructorContainer> _constructors;
        static std::map<std::string, PyMethodDef> _methodCollection;
        static std::map<std::string, PyMethodDef> _methodCollectionConst;
        static std::map<std::string, std::pair<std::function<PyObject*(PyObject*, PyObject*)>,
                                     std::function<int(bool, PyObject*, PyObject*, PyObject*)>>
                          >_mapMethodCollection;
        static std::map<std::string, _getattrfunc > _member_getters;
        static std::map<std::string, _setattrfunc > _member_setters;
        static std::vector<_setattrfunc > _assigners;
        static std::vector<PyTypeObject *> _baseClasses;
        static std::vector<PyTypeObject *> _baseClassesConst;
        static std::map<std::string, const T_NoRef*> _classEnumValues;
        static std::map<std::string, unaryfunc> _unaryOperators;
        static std::map<std::string, unaryfunc> _unaryOperatorsConst;
        static std::map<std::string, binaryfunc> _binaryOperators;
        static std::map<std::string, binaryfunc> _binaryOperatorsConst;

        static PyTypeObject _Type;

    };


  template<>
  struct PythonClassWrapper<const double>;



}
#endif
