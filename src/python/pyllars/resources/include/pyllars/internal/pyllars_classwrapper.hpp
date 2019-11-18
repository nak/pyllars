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

#include "pyllars/pyllars_namespacewrapper.hpp"
#include "pyllars/pyllars.hpp"
#include "pyllars_containment.hpp"
#include "pyllars_type_traits.hpp"

typedef const char cstring[];
static constexpr cstring operatormapname = "operator[]";
static constexpr cstring operatormapnameconst = "operator[]const";

static const char* const emptylist[] = {nullptr};

namespace pyllars{
    class CommonNamespaceWrapper;
}

namespace __pyllars_internal {


    enum class OpUnaryEnum : unsigned char{
        INV,
        POS,
        NEG
    };

    enum class OpBinaryEnum : unsigned char{
        ADD = 3,
        SUB,
        MUL,
        DIV,
        AND,
        OR,
        XOR,
        MOD,
        LSHIFT,
        RSHIFT,

        IADD,
        ISUB,
        IMUL,
        IDIV,
        IAND,
        IOR,
        IXOR,
        IMOD,
        ILSHIFT,
        IRSHIFT
    };

    typedef int (*_setattrfunc)(PyObject*, PyObject*, void*);
    typedef PyObject* (*_getattrfunc)(PyObject*, void*);

    template<typename T>
    struct ObjectContainer;

    template<typename T>
    class InitHelper;

    template<typename functype>
    struct PythonFunctionWrapper;

    /**
     * Class to define Python wrapper to C class/type
     **/
    template<typename T>
    struct PythonClassWrapper<T, typename std::enable_if< is_rich_class<T>::value>::type>: public CommonBaseWrapper {
        // Convenience typedefs
        typedef T WrappedType;
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

        template<typename Parent>
        static status_t ready();

        static status_t preinit(){
            return 0;
        }

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
        static void addConstructor(const char *const kwlist[]);

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
        template<const char *const name, const char *const kwlist[], typename func_type, func_type method>
        static void addStaticMethod();

        /**
         *  Adda a named type (a class-within-this-class), where typeobj may not have yet been
         *  readied, but will be so during call to this's ready.  This allows us to handle a type
         *  of chicken-and-egg situation
         * @param name   name of type to add
         * @param typeobj the type to add
         */
        static void addStaticType( const char* const name, PyTypeObject* (*typeobj)());

        /**
         * Add an enum value to the class
         */
        template<typename EnumT>
        static int addEnumValue( const char* const name, const EnumT& value);

        static void addClassObject(const char* const name, PyObject* const obj);
        /**
         * Add a mutable bit field to this Python type definition
         **/
        template<const char *const name, typename FieldType, const size_t bits>
        static void addBitField(
                typename std::function< FieldType(const T_NoRef&)> &getter,
                typename std::function< FieldType(T_NoRef&, const FieldType&)>  *setter=nullptr);

        template <typename FieldType, typename E= void>
        struct MemberPtr;

        template <typename FieldType>
        struct MemberPtr<FieldType, std::enable_if_t<!is_scoped_enum<T_NoRef>::value > >{
            typedef FieldType T_NoRef::*member_t;
        };
        /**
         * add a getter method for the given compile-time-known named public class member
         **/
        template<const char *const name, typename FieldType>
        static void addAttribute(typename MemberPtr<FieldType>::member_t member);//MemberContainer<name, T_NoRef, FieldType>::member_t member);

        /**
         * add a getter method for the given compile-time-known named public static class member
         **/
        template<const char *const name, typename FieldType>
        static void addStaticAttribute(FieldType *member);

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char *const name, const char* const kwlist[], typename method_t, method_t method>
        static void addMethod();

        template<OpUnaryEnum kind, typename method_t, method_t method>
        static void addUnaryOperator(){
            Op<kind, method_t, method>::addUnaryOperator();
        }

        template<typename KeyType, typename method_t, method_t method>
        static void addMapOperator();

        static bool checkType(PyObject * obj);

        static PyTypeObject *getPyType(){
            return (initialize() == 0)?&_Type:nullptr;
        }

        /**
         * Return underlying PyTypeObject, possibly uninitialized (no call to PyType_Ready is guaranteed)
         * Use sparingly
         *
         * @return  PyTypeObject associated with this class
         */
        static PyTypeObject * getRawType(){
            return &_Type;
        }

        template<typename functype>
        friend struct PythonFunctionWrapper;

        template<typename Y, typename YY>
        friend class PythonClassWrapper;

        friend
        void * toFFI(PyObject*);


        /**
         * return the C-like object associated with this Python wrapper
         */
        typename PythonClassWrapper::T_NoRef *get_CObject() const;

        typename std::remove_const<T>::type& toCArgument();
        const T& toCArgument() const;


        template<OpUnaryEnum , typename method_t, method_t method>
        struct Op;

        template<OpUnaryEnum kind, typename ReturnType, ReturnType(CClass::*method)()>
        struct Op<kind, ReturnType(CClass::*)(), method>{
            static void addUnaryOperator();
        };

        template<OpUnaryEnum kind, typename ReturnType, ReturnType(CClass::*method)() const>
        struct Op<kind, ReturnType(CClass::*)() const, method>{
            static void addUnaryOperator();
        };


        template<OpBinaryEnum kind, typename method_t, method_t method>
        struct BinaryOp;

        template<OpBinaryEnum kind, typename ReturnType, typename ArgType, ReturnType(CClass::*method)(ArgType)>
        struct BinaryOp<kind, ReturnType(CClass::*)(ArgType), method>{
            static void addBinaryOperator();
        };

        template<OpBinaryEnum kind, typename ReturnType, typename ArgType, ReturnType(CClass::*method)(ArgType) const>
        struct BinaryOp<kind, ReturnType(CClass::*)(ArgType) const, method>{
            static void addBinaryOperator();
        };

    protected:

        void set_CObject(T_NoRef * value ){
            _CObject = value;
        }

        PythonClassWrapper();// never invoked as Python allocates memory directly

        static PyObject *alloc(PyObject *cls, PyObject *args, PyObject *kwds);

        static int _initialize(PyTypeObject & Type);

        static int
        _pySetAttr(PyObject* self,  char* attrname, PyObject* value){
            if (!_member_setters().count(attrname)){
                PyErr_SetString(PyExc_ValueError, "No such attribute or attempt to set const attribute");
                return -1;
            }
            return _member_setters()[attrname](self, value, nullptr);
        }

        static PyObject* _pyGetAttr(PyObject* self,  char* attrname){
            if (!_member_getters().count(attrname)){
                return PyObject_GenericGetAttr(self, PyString_FromString(attrname));
            }
            return _member_getters()[attrname](self, nullptr);
        }

        template <class Base>
        static PyObject* cast(PyObject* self){
            static_assert(std::is_base_of<std::remove_reference_t <Base>, T_NoRef >::value);
            auto self_ = (PythonClassWrapper<T>*)self;
            auto castWrapper = (PythonClassWrapper<Base&>*) PyObject_Call((PyObject*)PythonClassWrapper<Base&>::getPyType(),
                    NULL_ARGS(), nullptr);
            castWrapper->set_CObject(static_cast<std::remove_reference_t<Base>*>(self_->get_CObject()));
            return (PyObject*) castWrapper;
        }

        // must use object container, since code may have new and delete private and container
        // will shield us from that
        //ObjectContainer<T_NoRef> *_CObject;
        T_NoRef * _CObject;

    private:

        static bool _isInitialized;

        static void addAssigner(_setattrfunc func){
            if(!_member_setters().count("this"))
                _member_setters()["this"] = _pyAssign;
            _assigners().push_back(func);
        }
        template<typename KeyType, typename ValueType, typename method_t, method_t method>
        static void _addMapOperatorMethod();

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
            for ( _setattrfunc assigner: _assigners()){
                if( assigner(self, value, nullptr) == 0){
                    return 0;
                }
            }
            PyErr_SetString(PyExc_ValueError, "Cannot assign to given _CObject type");
            return -1;
        }

        static int _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds);

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
        static std::vector<ConstructorContainer>& _constructors(){
            static std::vector<ConstructorContainer> container;
            return container;
        }

        static std::map<std::string, PyMethodDef>& _methodCollection(){
            static std::map<std::string, PyMethodDef> container;
            return container;
        }

        static std::map<std::string, PyMethodDef>& _methodCollectionConst(){
            static std::map<std::string, PyMethodDef> container;
            return container;
        }

        static std::map<std::string, std::pair<std::function<PyObject*(PyObject*, PyObject*)>,
                                     std::function<int(bool, PyObject*, PyObject*, PyObject*)>>
                          >_mapMethodCollection;
        static std::map<std::string, _getattrfunc >& _member_getters(){
            static std::map<std::string, _getattrfunc > container;
            return container;
        }

        static std::map<std::string, _setattrfunc >& _member_setters(){
            static std::map<std::string, _setattrfunc > container;
            return container;
        }

        static std::vector<_setattrfunc >& _assigners(){
            static std::vector<_setattrfunc > container;
            return container;
        }

        static std::vector<PyTypeObject *>& _baseClasses(){
            static std::vector<PyTypeObject *> container;
            return container;
        }

        static std::vector<PyTypeObject*(*)()>& _childrenReadyFunctions(){
            static std::vector<PyTypeObject*(*)()> container;
            return container;
        }

        static std::map<std::string, const typename std::remove_cv<T_NoRef>::type*>& _classEnumValues(){
            static std::map<std::string, const typename std::remove_cv<T_NoRef>::type*> container;
            return container;
        }

        static std::map<std::string, PyObject*>& _classObjects(){
            static std::map<std::string, PyObject*> container;
            return container;
        }

        static std::map<std::string, PyTypeObject* (*)()>& _classTypes(){
            static std::map<std::string, PyTypeObject*(*)()> container;
            return container;
        }

        static std::map<OpUnaryEnum, unaryfunc>& _unaryOperators(){
            static std::map<OpUnaryEnum , unaryfunc> container;
            return container;
        }

        static std::map<OpUnaryEnum, unaryfunc>& _unaryOperatorsConst(){
            static std::map<OpUnaryEnum , unaryfunc> container;
            return container;
        }

        static std::map<OpBinaryEnum, binaryfunc>& _binaryOperators(){
            static std::map<OpBinaryEnum, binaryfunc> container;
            return container;
        }

        static std::map<OpBinaryEnum, binaryfunc>& _binaryOperatorsConst(){
            static std::map<OpBinaryEnum , binaryfunc> container;
            return container;
        }

        static PyTypeObject _Type;

    };


  template<>
  struct PythonClassWrapper<const double>;

  /**
   * for inner structs like:
   * struct Outer{
   *    struct {
   *      int value;
   *    }; // note: no type name AND no attribute name
   * };
   */
  template<typename T>
  class PythonAnonymousClassWrapper: protected PythonClassWrapper<T>{
  public:
       typedef PythonClassWrapper<T> Parent;
       typedef typename Parent::T_NoRef T_NoRef ;

        /**
         * Initialize python type if needed
         * @return Python-based PyTypeObject associated with T
        */
        static PyTypeObject* getType(){
           return Parent::getType();
        }

        /**
         * Python initialization of underlying type, called to init and register type with
         * underlying Python system
         *
         * @param name: Python simple-name of the type
         **/
        static int initialize(){return Parent::initialize();}

           /**
         * Add an enum value to the class
         */
        template<typename EnumT>
        static int addEnumValue( const char* const name, EnumT value){
            return Parent::addEnumValue(name, value);
        }

        static void addClassObject( const char* const name, PyObject* obj){
            return Parent::addClassObject(name, obj);
        }

        /**
         * Add a mutable bit field to this Python type definition
         **/
        template<const char *const name, typename FieldType, const size_t bits>
        static void addBitField(  typename std::function< FieldType(const T_NoRef&)> &getter,
                                  typename  std::function< FieldType(T_NoRef&, const FieldType&)>  *setter=nullptr){
             return Parent::addBitField(getter, setter);
         }

        /**
         * add a getter method for the given compile-time-known named public class member
         **/
        template<const char *const name, typename FieldType>
        static void addAttribute(FieldType T_NoRef::*member){
            return Parent::addAttribute(member);
        }

  };



}
#endif
