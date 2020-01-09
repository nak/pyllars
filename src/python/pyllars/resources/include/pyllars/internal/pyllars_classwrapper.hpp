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

    template <typename T>
    struct Classes;

    template <typename T>
    struct Pointers;

    /**
     * Class to define Python wrapper to C class/type
     **/
    template<typename T >
    struct DLLEXPORT PythonClassWrapper: public PythonBaseWrapper<typename ptr_depth<T>::type_repr>{

        //handled through PythonFunctionWrapper:
        static_assert(!std::is_function<typename std::remove_pointer<T>::type>::value);

        friend
        void * toFFI(PyObject*);

        template<typename Other>
        friend class Classes;

        typedef PythonBaseWrapper<typename ptr_depth<T>::type_repr> Base;

        typedef std::remove_reference_t <T> T_NoRef;

        typedef typename ptr_depth<T>::type_repr T_repr;

        typedef typename core_type<T_NoRef>::type CClass;

        static PyTypeObject *getPyType();

        static int initialize();

        /**
         * create a Python object of this struct type
         **/
        static PythonClassWrapper *fromCObject(T_NoRef & cobj);

        /**
         *
         * @tparam Args: constructor args to use to instantiate a/an-array-of C objects to wrap
         *
         * @param Type: the Python type representing the returned object
         * @param args: constructor args for instantiation
         * @param arraySize: number of elements to allocated
         * @return Python object wrappping the instantiated C-object/array
         */
        template<typename ...Args>
        static PyObject* createAllocatedInstance(Args ...args, ssize_t arraySize = -1);

        void set_CObject(T_NoRef * obj, ssize_t array_length = -1){
            _max = array_length;
            Base::set_CObject((typename Base::storage_type) obj);
        }

        void set_CObjectFromLocation( unsigned char* location, ssize_t array_length){
            set_CObject((T_NoRef*)location, array_length);
        }

        T_NoRef* get_CObject() const{
            return (T_NoRef*) Base::get_CObject();
        }

        T_NoRef* & get_CObject(){
            return (T_NoRef*&) Base::get_CObject();
        }

        typename std::remove_const<T_NoRef>::type& toCArgument();

        const T_NoRef& toCArgument() const;

        ssize_t len(){
            return _max;
        }


    protected:

        static PyObject *alloc(PyObject *cls, PyObject *args, PyObject *kwds);


        template <typename BaseC>
        static PyObject* cast(PyObject* self){
            static_assert(std::is_base_of<std::remove_reference_t <BaseC>, T_NoRef >::value);
            auto self_ = (Base*)self;
            auto castWrapper = (PythonClassWrapper<BaseC&>*) PyObject_Call((PyObject*)PythonClassWrapper<BaseC&>::getPyType(),
                                                                          NULL_ARGS(), nullptr);
            castWrapper->set_CObject(static_cast<std::remove_reference_t<BaseC>*>(self_->get_CObject()));
            return (PyObject*) castWrapper;
        }

    private:

        PythonClassWrapper(){}// never invoked as Python allocates memory directly

        static void addAssigner(_setattrfunc func){
            if(!Base::getTypeProxy()._member_setters.count("this"))
               Base::getTypeProxy()._member_setters["this"] = _pyAssign;
            Base::getTypeProxy()._assigners.push_back(func);
        }

        static PyObject* getThis(PyObject* self, void*){
            return Base::addr(self, nullptr);
        }

        static int _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds);

        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

        static int
        _pyAssign(PyObject* self, PyObject* value, void*){
            for ( _setattrfunc assigner:Base::getTypeProxy()._assigners){
                if( assigner(self, value, nullptr) == 0){
                    return 0;
                }
            }
            PyErr_SetString(PyExc_ValueError, "Cannot assign to given _CObject type");
            return -1;
        }

        unsigned char* _byte_bucket;

        ssize_t _max;
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



    ////// Rich class methods ////////////
    template <typename T>
    struct Classes {
        static_assert(is_rich_class<T>::value);

        using T_NoRef = typename PythonClassWrapper<T>::T_NoRef ;
        using CClass = typename PythonClassWrapper<T>::CClass;
        using Base = typename PythonClassWrapper<T>::Base;

        template<typename KeyType, typename method_t, method_t method>
        static void addMapOperator();

        /**
         * Add a base struct to this Python definition (called before initialize)
         **/
        template<typename BaseClass>
        static void addBaseClass();


        /**
         * Add an enum value to the class
         */
        template<typename EnumT>
        static int addEnumValue(const char *const name, const EnumT &value);

        /**
         * Add a mutable bit field to this Python type definition
         **/
        template<const char *const name, typename FieldType, const size_t bits>
        static void addBitField(
                typename std::function<FieldType(const T_NoRef &)> &getter,
                typename std::function<FieldType(T_NoRef &, const FieldType &)> *setter = nullptr);

        template<typename FieldType>
        struct MemberPtr { ;
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
        static void addUnaryOperator() {
            Op<kind, method_t, method>::addUnaryOperator();
        }


        template<OpUnaryEnum, typename method_t, method_t method>
        struct Op;

        template<OpUnaryEnum kind, typename ReturnType, ReturnType(CClass::*method)()>
        struct Op<kind, ReturnType(CClass::*)(), method> {
            static void addUnaryOperator() {
                Base::getTypeProxy()._unaryOperators[kind] =
                        (unaryfunc) MethodContainer<pyllars_empty_kwlist, ReturnType(
                        T_NoRef::*)(), method>::callAsUnaryFunc;
            }
        };

        template<OpUnaryEnum kind, typename ReturnType, ReturnType(CClass::*method)() const>
        struct Op<kind, ReturnType(CClass::*)() const, method> {
            static void addUnaryOperator() {
                Base::getTypeProxy()._unaryOperatorsConst[kind] =
                        (unaryfunc) MethodContainer<pyllars_empty_kwlist, ReturnType(T_NoRef::*)() const, method>::callAsUnaryFunc;
            }
        };


        template<OpBinaryEnum kind, typename method_t, method_t method>
        struct BinaryOp{
            static void addBinaryOperator();
        };

        template<typename KeyType, typename ValueType, typename method_t, method_t method>
        static void _addMapOperatorMethod();

        static int
        _pySetAttr(PyObject* self,  char* attrname, PyObject* value){
            if (!Base::getTypeProxy()._member_setters.count(attrname)){
                PyErr_SetString(PyExc_ValueError, "attempt to set non-existent or const attribute");
                return -1;
            }
            return Base::getTypeProxy()._member_setters[attrname](self, value, nullptr);
        }

        static PyObject* _pyGetAttr(PyObject* self,  char* attrname){
            if (!Base::getTypeProxy()._member_getters.count(attrname)){
                return PyObject_GenericGetAttr(self, PyString_FromString(attrname));
            }
            return Base::getTypeProxy()._member_getters[attrname](self, nullptr);
        }

    };


    /////////// Pointer-type methods //////////
    template<typename T >
    struct Pointers{
        static_assert(is_pointer_like<T>::value);
        using T_NoRef = typename PythonClassWrapper<T>::T_NoRef ;
        using CClass = typename PythonClassWrapper<T>::CClass;
        using Base = typename PythonClassWrapper<T>::Base;

        /**
         * create a Python object for a pointer-like type (pointer-like types only)
         */
        static PyObject* fromCArray(typename extent_as_pointer<T_NoRef>::type &cobj,
                                    const ssize_t arraySize,
                                    PyObject *referencing = nullptr,
                                    unsigned char* byte_bucket = nullptr);

        static PyObject* fromInPlaceAllocation( const ssize_t arraySize, unsigned char* byte_bucket);

        /**
         *
         * @tparam Args: constructor args to use to instantiate a/an-array-of C objects to wrap
         *
         * @param Type: the Python type representing the returned object
         * @param args: constructor args for instantiation
         * @param arraySize: number of elements to allocated
         * @return Python object wrappping the instantiated C-object/array
         */
        template<typename ...Args>
        static PyObject* createAllocatedInstance(Args ...args, ssize_t arraySize = -1);

        //////// Pointer-type methods and supporting classes /////////////

        static PySequenceMethods _seqmethods;

        struct Iter{ // for pointer iterations
            PyObject_HEAD
            PythonClassWrapper<T>* obj;
            long long max;
            long long i;
            static const std::string name;
            static PyTypeObject _Type;

            static PyObject * iter(PyObject* self);
            static PyObject * iternext(PyObject* self);
        };

        // Pointer-type-related  public methods
        static Py_ssize_t _size(PyObject* self);

        static PyObject* _concat(PyObject* self, PyObject* other);

        static int _set_item(PyObject* self, Py_ssize_t index, PyObject* obj);

        static PyObject* _get_item(PyObject* self, Py_ssize_t item);

        static int _contains(PyObject* self, PyObject* );

        static PyObject* _at(PyObject* self, PyObject* args, PyObject* kwds);

        static PyObject *_inplace_concat(PyObject *self, PyObject *other) {
            return nullptr;
        }

        static PyObject *_inplace_repeat(PyObject *self, Py_ssize_t count){
            return nullptr;
        }

        static PyObject *_repeat(PyObject *self, Py_ssize_t count){
            return nullptr;
        }
    };


    template<typename T>
    struct References{
        using T_NoRef = typename PythonClassWrapper<T>::T_NoRef ;
        using CClass = typename PythonClassWrapper<T>::CClass;
        using Base = typename PythonClassWrapper<T>::Base;

        static PyObject* fromCObject(T cobj, PyObject* referencing = nullptr);

        static PyObject* fromCArray( T cobj, size_t array_size, PyObject* referencing = nullptr);

        static PyObject* referenceTo(PythonClassWrapper<T_NoRef>* obj);
    };

}
#endif
