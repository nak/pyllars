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
    template<typename T, typename TrueType >
    struct DLLEXPORT PythonClassWrapper_Base: public PythonBaseWrapper<TrueType>{

        typedef PythonBaseWrapper<TrueType> Base;

        typedef std::remove_reference_t <TrueType> T_NoRef;

        typedef typename core_type<T>::type CClass;

        template<typename Parent>
        static status_t ready();

        static status_t preinit(){
            return 0;
        }

        /**
         * create a Python object of this struct type
         **/
        static PythonClassWrapper_Base *fromCObject(T_NoRef & cobj);

         /**
          * return Python object representing the address of the contained object
          **/
        static PyObject *addr(PyObject *self, PyObject *args) ;

        /**
         * Add a base struct to this Python definition (called before initialize)
         **/
        template<typename BaseClass>
        static void addBaseClass() ;


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
                typename std::function< FieldType(T&, const FieldType&)>  *setter=nullptr);

        template <typename FieldType>
        struct MemberPtr{;
            static_assert(!is_scoped_enum<T_NoRef>::value);
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

        template<typename Y, typename YY>
        friend struct PythonClassWrapper_Base;

        friend
        void * toFFI(PyObject*);

        friend struct PythonClassWrapper_Base<std::remove_cv_t<T_NoRef >, std::remove_cv_t<T_NoRef> >;

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

        template <typename BaseC>
        static PyObject* cast(PyObject* self){
            static_assert(std::is_base_of<std::remove_reference_t <BaseC>, T_NoRef >::value);
            auto self_ = (PythonClassWrapper<T>*)self;
            auto castWrapper = (PythonClassWrapper<BaseC&>*) PyObject_Call((PyObject*)PythonClassWrapper<BaseC&>::getPyType(),
                                                                          NULL_ARGS(), nullptr);
            castWrapper->set_CObject(static_cast<std::remove_reference_t<BaseC>*>(self_->get_CObject()));
            return (PyObject*) castWrapper;
        }

    private:
        PythonClassWrapper_Base(){}// never invoked as Python allocates memory directly

        static void addAssigner(_setattrfunc func){
            if(!Base::getTypeProxy()._member_setters.count("this"))
               Base::getTypeProxy()._member_setters["this"] = _pyAssign;
            Base::getTypeProxy()._assigners.push_back(func);
        }

        template<typename KeyType, typename ValueType, typename method_t, method_t method>
        static void _addMapOperatorMethod();

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

        static int readyType();

        struct Initializer{
            Initializer(){
                Init::registerInit(&PythonClassWrapper_Base::initialize);
                Init::registerReady(&PythonClassWrapper_Base::readyType);
            }

        };
        static Initializer initializer;
    };

    /**
      * Class to define Python wrapper to C class/type
      **/
    template<typename T>
    struct DLLEXPORT PythonClassWrapper<T, typename std::enable_if< is_rich_class<T>::value>::type>: public PythonClassWrapper_Base<T, T> {

        typedef  PythonClassWrapper_Base<T, T> Base;

        static PyTypeObject* getPyType();


        static int initialize(){return Base::_initialize(*Base::getRawType());}
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
