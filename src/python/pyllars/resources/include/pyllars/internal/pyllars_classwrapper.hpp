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
#include "pyllars_base.hpp"
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
    template<typename T, typename TrueType>
    struct DLLEXPORT PythonClassWrapper_Base: public PythonBaseWrapper<TrueType>{

        static_assert(!std::is_reference<T>::value);

        typedef PythonBaseWrapper<TrueType> Base;
        typedef typename core_type<T>::type CClass;

        template<typename Parent>
        static status_t ready();

        static status_t preinit(){
            return 0;
        }

        /**
         * create a Python object of this struct type
         **/
        static PythonClassWrapper_Base *fromCObject(T & cobj);

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
                typename std::function< FieldType(const T&)> &getter,
                typename std::function< FieldType(T&, const FieldType&)>  *setter=nullptr);

        template <typename FieldType>
        struct MemberPtr{;
            static_assert(!is_scoped_enum<T>::value);
            typedef FieldType T::*member_t;
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


        template<typename functype>
        friend struct PythonFunctionWrapper;

        template<typename Y, typename YY>
        friend struct PythonClassWrapper;

        friend
        void * toFFI(PyObject*);



        typename std::remove_const<T>::type& toCArgument();
        const T& toCArgument() const;


        template<OpUnaryEnum , typename method_t, method_t method>
        struct Op;

        template<OpUnaryEnum kind, typename ReturnType, ReturnType(CClass::*method)()>
        struct Op<kind, ReturnType(CClass::*)(), method>{
            static void addUnaryOperator() {
                PythonBaseWrapper<T>::getTypeProxy()._unaryOperators[kind] = (unaryfunc) MethodContainer<pyllars_empty_kwlist, ReturnType(T::*)(), method>::callAsUnaryFunc;
            }
        };

        template<OpUnaryEnum kind, typename ReturnType, ReturnType(CClass::*method)() const>
        struct Op<kind, ReturnType(CClass::*)() const, method>{
            static void addUnaryOperator() {
                PythonBaseWrapper<T>::getTypeProxy()._unaryOperatorsConst[kind] = (unaryfunc) MethodContainer<pyllars_empty_kwlist, ReturnType(T::*)() const, method>::callAsUnaryFunc;
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




        template<typename RealRepr>
        static RealRepr &reinterpretValue(PythonClassWrapper_Base* self){
            static_assert(std::is_convertible<T, RealRepr>::value);
            if (!PyObject_TypeCheck(self, PythonClassWrapper<RealRepr>::getPyType())){
                throw PyllarsException("Invalid cast of different type requested");
            }
            if constexpr (std::is_same<RealRepr, T>::value){
                return *self->get_CObject();
            } else {
                RealRepr *value = reinterpret_cast<RealRepr *>(self->get_CObject());
                return *value;
            }
        }

        static PyTypeObject* getPyType();

        static int initialize(){return _initialize(*Base::getRawType());}

    protected:


        static PyObject *alloc(PyObject *cls, PyObject *args, PyObject *kwds);

        static int _initialize(PyTypeObject & Type);

        static int
        _pySetAttr(PyObject* self,  char* attrname, PyObject* value){
            if (!PythonBaseWrapper<T>::getTypeProxy()._member_setters.count(attrname)){
                PyErr_SetString(PyExc_ValueError, "No such attribute or attempt to set const attribute");
                return -1;
            }
            return PythonBaseWrapper<T>::getTypeProxy()._member_setters[attrname](self, value, nullptr);
        }

        static PyObject* _pyGetAttr(PyObject* self,  char* attrname){
            if (!PythonBaseWrapper<T>::getTypeProxy()._member_getters.count(attrname)){
                return PyObject_GenericGetAttr(self, PyString_FromString(attrname));
            }
            return PythonBaseWrapper<T>::getTypeProxy()._member_getters[attrname](self, nullptr);
        }

        template <typename Base>
        static PyObject* cast(PyObject* self){
            static_assert(std::is_base_of<std::remove_reference_t <Base>, T >::value);
            auto self_ = (PythonClassWrapper<T>*)self;
            auto castWrapper = (PythonClassWrapper<Base&>*) PyObject_Call((PyObject*)PythonClassWrapper<Base&>::getPyType(),
                                                                          NULL_ARGS(), nullptr);
            castWrapper->set_CObject(static_cast<std::remove_reference_t<Base>*>(self_->get_CObject()));
            return (PyObject*) castWrapper;
        }

        typedef T* (*constructor_t)(const char *const kwlist[], PyObject *args, PyObject *kwds,
                                    unsigned char* const location);

        typedef std::pair<const char* const*, constructor_t> ConstructorContainer;
        static std::vector<ConstructorContainer>& _constructors(){
            static std::vector<ConstructorContainer> container;
            return container;
        }
    private:
        PythonClassWrapper_Base(){}// never invoked as Python allocates memory directly

        static void _initAddCArgCasts();

        static bool _isInitialized;

        static void addAssigner(_setattrfunc func){
            if(!PythonBaseWrapper<T>::getTypeProxy()._member_setters.count("this"))
                PythonBaseWrapper<T>::getTypeProxy()._member_setters["this"] = _pyAssign;
            PythonBaseWrapper<T>::getTypeProxy()._assigners.push_back(func);
        }

        template<typename KeyType, typename ValueType, typename method_t, method_t method>
        static void _addMapOperatorMethod();

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
        static T* create(const char *const kwlist[], PyObject *args, PyObject *kwds, unsigned char*) ;

        static PyObject* getThis(PyObject* self, void*){
            return addr(self, nullptr);
        }

        static int
        _pyAssign(PyObject* self, PyObject* value, void*){
            for ( _setattrfunc assigner: PythonBaseWrapper<T>::getTypeProxy()._assigners){
                if( assigner(self, value, nullptr) == 0){
                    return 0;
                }
            }
            PyErr_SetString(PyExc_ValueError, "Cannot assign to given _CObject type");
            return -1;
        }

        static int _init(PythonClassWrapper_Base *self, PyObject *args, PyObject *kwds);

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
        static T* _createBaseBase( unsigned char *location, argument_capture<Args> ... args);

        template<typename ...Args, int ...S >
        static T* _createBase( unsigned char *location, PyObject *args, PyObject *kwds,
                 const char *const kwlist[], container<S...> unused1, _____fake<Args> *... unused2);



        static std::map<std::string, const typename std::remove_cv<T>::type*>& _classEnumValues(){
            static std::map<std::string, const typename std::remove_cv<T>::type*> container;
            return container;
        }


    };

    /**
      * Class to define Python wrapper to C class/type
      **/
    template<typename T>
    struct DLLEXPORT PythonClassWrapper<T, typename std::enable_if< is_rich_class<T>::value>::type>: public PythonClassWrapper_Base<T, T> {
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
  struct DLLEXPORT PythonAnonymousClassWrapper: public PythonClassWrapper<T>{
  public:
       typedef PythonClassWrapper<T> Parent;
       typedef typename Parent::T T_NoRef ;

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
        static void addBitField(  typename std::function< FieldType(const T&)> &getter,
                                  typename  std::function< FieldType(T&, const FieldType&)>  *setter=nullptr){
             return Parent::addBitField(getter, setter);
         }

        /**
         * add a getter method for the given compile-time-known named public struct member
         **/
        template<const char *const name, typename FieldType>
        static void addAttribute(FieldType T::*member){
            return Parent::addAttribute(member);
        }


  };



}
#endif
