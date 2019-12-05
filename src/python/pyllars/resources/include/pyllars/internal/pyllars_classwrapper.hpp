//  PREDECLARATIONS

#ifndef __PYLLARS__INTERNAL__CLASSWRAPPER_H
#define __PYLLARS__INTERNAL__CLASSWRAPPER_H

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
#include "pyllars_reference.hpp"
#include "pyllars_type_traits.hpp"
#include "pyllars_methodcallsemantics.hpp"

typedef const char cstring[];
static constexpr cstring operatormapname = "operator[]";
static constexpr cstring operatormapnameconst = "operator[]const";

static const char* const emptylist[] = {nullptr};

namespace pyllars{
    struct CommonNamespaceWrapper;
}

namespace pyllars_internal {



    template<typename functype>
    struct DLLEXPORT PythonFunctionWrapper;

    /**
     * Class to define Python wrapper to C class/type
     **/
    template<typename T>
    struct DLLEXPORT PythonClassWrapper<T, typename std::enable_if< is_rich_class<T>::value>::type>: public CommonBaseWrapper {
        // Convenience typedefs
        typedef T WrappedType;
        //typedef CommonBaseWrapper::Base Base;

        typedef typename std::remove_reference<T>::type T_NoRef;
        typedef typename core_type<T>::type CClass;

        template<typename C>
        friend
        struct InitHelper;


        /**
         * Python initialization of underlying type, called to init and register type with
         * underlying Python system
         *
         * @param name: Python simple-name of the type
         **/
        static int initialize(){return _initialize(*getRawType());}

        template<typename Parent>
        static status_t ready();

        static status_t preinit(){
            return 0;
        }

        /**
         * create a Python object of this struct type
         **/
        static PythonClassWrapper *fromCObject(T_NoRef & cobj);

         /**
          * return Python object representing the address of the contained object
          **/
        static PyObject *addr(PyObject *self, PyObject *args) ;

        /**
         * Add a base struct to this Python definition (called before initialize)
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
        template<const char*  const kwlist[], typename ...Args>
        static void addConstructor();

        /**
         * Add an enum value to the class
         */
        template<typename EnumT>
        static int addEnumValue( const char* const name, const EnumT& value);

        /**
         * Add a mutable bit field to this Python type definition
         **/
        template<const char *const name, typename FieldType, const size_t bits>
        static void addBitField(
                typename std::function< FieldType(const T_NoRef&)> &getter,
                typename std::function< FieldType(T_NoRef&, const FieldType&)>  *setter=nullptr);

        template <typename FieldType>
        struct MemberPtr{;
            static_assert(!is_scoped_enum<T_NoRef>::value);
            typedef FieldType T_NoRef::*member_t;
        };
        /**
         * add a getter method for the given compile-time-known named public struct member
         **/
        template<const char *const name, typename FieldType>
        static void addAttribute(typename MemberPtr<FieldType>::member_t member);

        /**
         * add a getter method for the given compile-time-known named public static struct member
         **/
        template<const char *const name, typename FieldType>
        static void addStaticAttribute(FieldType *member);


        template<OpUnaryEnum kind, typename method_t, method_t method>
        static void addUnaryOperator(){
            Op<kind, method_t, method>::addUnaryOperator();
        }

        template<typename KeyType, typename method_t, method_t method>
        static void addMapOperator();

        static PyTypeObject *getPyType();

        /**
         * Return underlying PyTypeObject, possibly uninitialized (no call to PyType_Ready is guaranteed)
         * Use sparingly
         *
         * @return  PyTypeObject associated with this class
         */
        static PyTypeObject * getRawType();

        template<typename functype>
        friend struct PythonFunctionWrapper;

        template<typename Y, typename YY>
        friend struct PythonClassWrapper;

        friend
        void * toFFI(PyObject*);


        /**
         * return the C-like object associated with this Python wrapper
         */
        typename PythonClassWrapper::T_NoRef *get_CObject() const{
            return _CObject;
        }

        typename std::remove_const<T>::type& toCArgument();
        const T& toCArgument() const;


        template<OpUnaryEnum , typename method_t, method_t method>
        struct Op;

        template<OpUnaryEnum kind, typename ReturnType, ReturnType(CClass::*method)()>
        struct Op<kind, ReturnType(CClass::*)(), method>{
            static void addUnaryOperator() {
                PythonClassWrapper<T>::getTypeProxy()._unaryOperators[kind] = (unaryfunc) MethodContainer<pyllars_empty_kwlist, ReturnType(T::*)(), method>::callAsUnaryFunc;
            }
        };

        template<OpUnaryEnum kind, typename ReturnType, ReturnType(CClass::*method)() const>
        struct Op<kind, ReturnType(CClass::*)() const, method>{
            static void addUnaryOperator() {
                PythonClassWrapper<T>::getTypeProxy()._unaryOperatorsConst[kind] = (unaryfunc) MethodContainer<pyllars_empty_kwlist, ReturnType(T::*)() const, method>::callAsUnaryFunc;
            }
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

        friend struct CommonBaseWrapper;

        inline void set_CObject(T_NoRef * value ){_CObject = value;}

        inline static constexpr TypedProxy & getTypeProxy(){return _Type;}

        inline static bool checkType(PyObject* o){return _Type.checkType(o);}

    protected:

        PythonClassWrapper();// never invoked as Python allocates memory directly

        static PyObject *alloc(PyObject *cls, PyObject *args, PyObject *kwds);

        static int _initialize(PyTypeObject & Type);

        static int
        _pySetAttr(PyObject* self,  char* attrname, PyObject* value){
            if (!_Type._member_setters.count(attrname)){
                PyErr_SetString(PyExc_ValueError, "No such attribute or attempt to set const attribute");
                return -1;
            }
            return _Type._member_setters[attrname](self, value, nullptr);
        }

        static PyObject* _pyGetAttr(PyObject* self,  char* attrname){
            if (!_Type._member_getters.count(attrname)){
                return PyObject_GenericGetAttr(self, PyString_FromString(attrname));
            }
            return _Type._member_getters[attrname](self, nullptr);
        }

        template <typename Base>
        static PyObject* cast(PyObject* self){
            static_assert(std::is_base_of<std::remove_reference_t <Base>, T_NoRef >::value);
            auto self_ = (PythonClassWrapper<T>*)self;
            auto castWrapper = (PythonClassWrapper<Base&>*) PyObject_Call((PyObject*)PythonClassWrapper<Base&>::getPyType(),
                                                                          NULL_ARGS(), nullptr);
            castWrapper->set_CObject(static_cast<std::remove_reference_t<Base>*>(self_->get_CObject()));
            return (PyObject*) castWrapper;
        }

        T_NoRef * _CObject;


    private:

        static void _initAddCArgCasts();

        static bool _isInitialized;

        static void addAssigner(_setattrfunc func){
            if(!_Type._member_setters.count("this"))
                _Type._member_setters["this"] = _pyAssign;
            _Type._assigners.push_back(func);
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
            for ( _setattrfunc assigner: _Type._assigners){
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


        typedef std::pair<const char* const*, constructor_t> ConstructorContainer;
        static std::vector<ConstructorContainer>& _constructors(){
            static std::vector<ConstructorContainer> container;
            return container;
        }

        static std::map<std::string, const typename std::remove_cv<T_NoRef>::type*>& _classEnumValues(){
            static std::map<std::string, const typename std::remove_cv<T_NoRef>::type*> container;
            return container;
        }

    private:

        static TypedProxy _Type;

    };



  /**
   * for inner structs like:
   * struct Outer{
   *    struct {
   *      int value;
   *    }; // note: no type name AND no attribute name
   * };
   */
  template<typename T>
  struct DLLEXPORT PythonAnonymousClassWrapper: protected PythonClassWrapper<T>{
  public:
       typedef PythonClassWrapper<T> Parent;
       typedef typename Parent::T_NoRef T_NoRef ;

        /**
         * Initialize python type if needed
         * @return Python-based PyTypeObject associated with T
        */
        static PyTypeObject* getPyType(){
           return Parent::getPyType();
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
         * add a getter method for the given compile-time-known named public struct member
         **/
        template<const char *const name, typename FieldType>
        static void addAttribute(FieldType T_NoRef::*member){
            return Parent::addAttribute(member);
        }

  };



}
#endif
