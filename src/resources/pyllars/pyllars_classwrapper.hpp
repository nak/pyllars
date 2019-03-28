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
#include "pyllars_classmethodsemantics.hpp"
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

    template<bool is_base_return_complete, bool with_ellipsis, typename ReturnType, typename ...Args>
    struct PythonFunctionWrapper;

    /**
     * Class to define Python wrapper to C class/type
     **/
    template<typename T>
    struct PythonClassWrapper<T, typename std::enable_if< is_rich_class<T>::value>::type>: public CommonBaseWrapper {
        // Convenience typedefs
        typedef CommonBaseWrapper::Base Base;

        typedef typename std::remove_reference<T>::type T_NoRef;
        typedef PyTypeObject *TypePtr;


        /**
         * return the C-likde object associated with this Python wrapper
         */
        typename PythonClassWrapper::T_NoRef *get_CObject() ;

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
        static int initialize();

        /**
         * create a Python object of this class type
         **/
        static PythonClassWrapper *createPyReference(T_NoRef & cobj, PyObject *referencing = nullptr);

        static PythonClassWrapper *createPyFromAllocated(T_NoRef* cobj, PyObject *referencing = nullptr) ;

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
         * @templateparam ReturnType: return type of the method being added
         * @templteparam Args: variadic list of types for the method's arumgnets
         *
         * @param method: the pointer to the metho to be added
         * @param kwlist: list of keyword names of araguments
         **/
        template<const char *const name, const char *const kwlist[], typename ReturnType, typename ...Args>
        static void addClassMethod(ReturnType(*method)(Args...));

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char *const name, const char *const kwlist[], typename ReturnType, typename ...Args>
        static void addClassMethodVarargs(ReturnType(*method)(Args... ...));


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

        static int addEnumClassValue( const char* const name, const T& value){
            _classEnumValues[name] = &value;
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
        static void addAttribute(typename MemberContainer<T_NoRef>::template Container<name, FieldType>::member_t member);

        template<const char *const name, ssize_t size, typename FieldType>
        static void addConstAttribute(
                typename MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::member_t member,
                const ssize_t array_size);

        /**
         * add a getter method for the given compile-time-known named public class member
         **/
        template<const char *const name, typename FieldType>
        static void addAttributeConst(
                typename ConstMemberContainer<T_NoRef>::template Container<name, FieldType>::member_t member);

        /**
          * add a getter method for the given compile-time-known named public class member
          **/
        template<const char *const name, ssize_t size, typename FieldType>
        static void addArrayAttribute(
                typename MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::member_t member,
                const ssize_t array_size);

        /**
         * add a getter method for the given compile-time-known named public static class member
         **/
        template<const char *const name, typename FieldType>
        static void addClassAttribute(FieldType *member);

        /**
         * add a getter method for the given compile-time-known named public static class member
         **/
        template<const char *const name, typename FieldType>
        static void addClassAttributeConst(FieldType const *member);

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<typename _MContainer,bool is_const, const char* const kwlist[], typename ReturnType, typename ...Args>
        static void addMethodTempl(
                typename _MContainer::template Container<is_const, kwlist, ReturnType, Args...>::method_t method);

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<bool is_const, const char *const name, const char* const kwlist[], typename ReturnType, typename ...Args>
        static void addMethod(
                typename MethodContainer<T_NoRef, name>::template Container<is_const, kwlist, ReturnType, Args...>::method_t method){
            addMethodTempl<MethodContainer<T_NoRef, name>, is_const, kwlist, ReturnType, Args...>(method);
        }

        template<bool is_const, const char *const name, const char* const kwlist[], typename ReturnType, typename ...Args>
        static void addMethodVarargs(
                typename MethodContainerVarargs<T_NoRef, name>::template Container<is_const, kwlist, ReturnType, Args...>::method_t method){
            addMethodTempl<MethodContainerVarargs<T_NoRef, name>, name, kwlist, ReturnType, Args...>(method);
         }



        template<typename Arg = T_NoRef, bool is_const = true>
        static void addInvOperator(typename MethodContainer<T_NoRef, OP_UNARY_INV>::template Container<is_const, emptylist, Arg>::method_t method)
        { addUnaryOperator<OP_UNARY_INV, is_const, Arg>(method);}

        template<typename Arg = T_NoRef, bool is_const = true>
        static void addPosOperator(typename MethodContainer<T_NoRef, OP_UNARY_POS>::template Container<is_const, emptylist, Arg>::method_t method)
        { addUnaryOperator<OP_UNARY_POS, is_const, Arg>(method);}

        template<typename Arg = T_NoRef, bool is_const = true>
        static void addNegOperator(typename MethodContainer<T_NoRef, OP_UNARY_NEG>::template Container<is_const, emptylist, Arg>::method_t method)
        { addUnaryOperator<OP_UNARY_NEG, is_const, Arg>(method);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addAddOperator(typename MethodContainer<T_NoRef, OP_BINARY_ADD>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){
            addBinaryOperator<OP_BINARY_ADD, is_const,  kwlist, ReturnType, Arg>(method);
        }

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addSubOperator(typename MethodContainer<T_NoRef, OP_BINARY_SUB>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){
            addBinaryOperator<OP_BINARY_SUB, is_const,  kwlist, ReturnType, Arg>(method);
        }

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addMulOperator(typename MethodContainer<T_NoRef, OP_BINARY_MUL>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){
            addBinaryOperator<OP_BINARY_MUL, is_const, kwlist, ReturnType, Arg>(method);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addDivOperator(typename MethodContainer<T_NoRef, OP_BINARY_DIV>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){
            addBinaryOperator<OP_BINARY_DIV, is_const, kwlist, ReturnType, Arg>(method);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addAndOperator(typename MethodContainer<T_NoRef, OP_BINARY_AND>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){
            addBinaryOperator<OP_BINARY_AND, is_const, kwlist, ReturnType, Arg>(method);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addOrOperator(typename MethodContainer<T_NoRef, OP_BINARY_OR>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){
            addBinaryOperator<OP_BINARY_OR, is_const, kwlist, ReturnType, Arg>(method);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addXorOperator(typename MethodContainer<T_NoRef, OP_BINARY_XOR>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){
            addBinaryOperator<OP_BINARY_XOR, is_const,  ReturnType, Arg>(method);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addLshiftOperator(typename MethodContainer<T_NoRef, OP_BINARY_LSHIFT>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){
            addBinaryOperator<OP_BINARY_LSHIFT, is_const,  kwlist, ReturnType, Arg>(method);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addRshiftOperator(typename MethodContainer<T_NoRef, OP_BINARY_RSHIFT>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){
            addBinaryOperator<OP_BINARY_RSHIFT, is_const, kwlist, ReturnType, Arg>(method);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addModOperator(typename MethodContainer<T_NoRef, OP_BINARY_MOD>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){ addBinaryOperator<OP_BINARY_MOD, is_const,  ReturnType, Arg>(method, kwlist);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addInplaceAddOperator(typename MethodContainer<T_NoRef, OP_BINARY_IADD>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){ addBinaryOperator<OP_BINARY_IADD, is_const,  ReturnType, Arg>(method, kwlist);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addInplaceSubOperator(typename MethodContainer<T_NoRef, OP_BINARY_ISUB>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){ addBinaryOperator<OP_BINARY_ISUB, is_const,  ReturnType, Arg>(method, kwlist);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addInplaceMulOperator(typename MethodContainer<T_NoRef, OP_BINARY_IMUL>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){ addBinaryOperator<OP_BINARY_IMUL, is_const,  ReturnType, Arg>(method, kwlist);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addInplaceModOperator(typename MethodContainer<T_NoRef, OP_BINARY_IMOD>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){ addBinaryOperator<OP_BINARY_IMOD, is_const,  ReturnType, Arg>(method, kwlist);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addInplaceLshiftOperator(typename MethodContainer<T_NoRef, OP_BINARY_ILSHIFT>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){ addBinaryOperator<OP_BINARY_ILSHIFT, is_const,  ReturnType, Arg>(method, kwlist);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addInplaceRshiftOperator(typename MethodContainer<T_NoRef, OP_BINARY_IRSHIFT>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){ addBinaryOperator<OP_BINARY_IRSHIFT, is_const,  ReturnType, Arg>(method, kwlist);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addInplaceAndOperator(typename MethodContainer<T_NoRef, OP_BINARY_IAND>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){ addBinaryOperator<OP_BINARY_IAND, is_const,  ReturnType, Arg>(method, kwlist);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addInplaceOrOperator(typename MethodContainer<T_NoRef, OP_BINARY_IOR>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){ addBinaryOperator<OP_BINARY_IOR, is_const,  ReturnType, Arg>(method, kwlist);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addInplaceXorOperator(typename MethodContainer<T_NoRef, OP_BINARY_IXOR>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){ addBinaryOperator<OP_BINARY_IXOR, is_const,  ReturnType, Arg>(method, kwlist);}

        template<const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg = T_NoRef, bool is_const = true>
        static void addInplaceDicOperator(typename MethodContainer<T_NoRef, OP_BINARY_IDIV>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method){ addBinaryOperator<OP_BINARY_IDIV, is_const,  ReturnType, Arg>(method, kwlist);}

        template<bool is_const, const char* const kwlist[2], typename KeyType, typename ValueType>
        static void addMapOperatorMethod( typename MethodContainer<T_NoRef, operatormapname>::template Container<is_const, kwlist, ValueType, KeyType>::method_t method);

        static bool checkType(PyObject *const obj);

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &_Type;
        }

        constexpr PythonClassWrapper(): _CObject(nullptr){}

        template<bool is_base_return_complete, bool with_ellipsis, typename ReturnType, typename ...Args>
        friend struct PythonFunctionWrapper;

        template<typename Y, typename YY>
        friend class PythonClassWrapper;

        static void addAssigner(_setattrfunc func){
            if(!_member_setters.count("this"))
                _member_setters["this"] = _pyAssign;
            _assigners.push_back(func);
        }

        static bool _isInitialized;

    protected:

        static PyObject *alloc(PyObject *cls, PyObject *args, PyObject *kwds);

        // must use object container, since code may have new and delete private and container
        // will shield us from that
        ObjectContainer<T_NoRef> *_CObject;

    private:

        template<const char *const name, bool is_const, typename Arg=T_NoRef>
        static void addUnaryOperator(
                typename MethodContainer<T_NoRef, name>::template Container<is_const, emptylist, Arg>::method_t method);

        template<const char *const name, bool is_const, const char* const kwlist[2], typename ReturnType=T_NoRef, typename Arg=T_NoRef>
        static void addBinaryOperator(
                typename MethodContainer<T_NoRef, name>::template Container<is_const, kwlist,  ReturnType, Arg>::method_t method);

        typedef ObjectContainer<T_NoRef>* (*constructor_t)(const char *const kwlist[], PyObject *args, PyObject *kwds,
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
        static ObjectContainer<T>* create(const char *const kwlist[], PyObject *args, PyObject *kwds, unsigned char*) ;

        static PyObject* getThis(PyObject* self, void*){
            return addr(self, nullptr);
        }

        static int
        _pyAssign(PyObject* self, PyObject* value){
            for ( _setattrfunc assigner: _assigners){
                if( assigner(self, value, nullptr) == 0){
                    return 0;
                }
            }
            PyErr_SetString(PyExc_ValueError, "Cannot assign to given value type");
            return -1;
        }

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

        static int
        _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds);

        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

        static void _free(void *self_);

        static void _dealloc(PythonClassWrapper *self);

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
        static ObjectContainer<T>* _createBaseBase(argument_capture<Args> ... args);

        template<typename ...Args, int ...S >
        static ObjectContainer<T>* _createBase
                (PyObject *args, PyObject *kwds,
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
