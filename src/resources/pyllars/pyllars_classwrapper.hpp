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


//#include "pyllars_utils.hpp"
#include "pyllars.hpp"
#include "pyllars_defns.hpp"
#include "pyllars_classmethodsemantics.hpp"
#include "pyllars_classmembersemantics.hpp"
#include "pyllars_membersemantics.hpp"
//#include "pyllars_constmethodcallsemantics.hpp"
#include "pyllars_methodcallsemantics.hpp"
#include "pyllars_object_lifecycle.hpp"
typedef const char cstring[];
static constexpr cstring operatormapname = "operator[]";

namespace __pyllars_internal {


    typedef int (*_setattrfunc)(PyObject*, PyObject*);
    typedef PyObject* (*_getattrfunc)(PyObject*);

    template<typename T, typename E=void>
    class InitHelper;

    template<class CClass>
    class ConstMemberContainer;

    template<class CClass>
    class ClassMember;

    template<class CClass>
    class ConstClassMember;

    template<bool is_base_return_complete, bool with_ellipsis, typename ReturnType, typename ...Args>
    struct PythonFunctionWrapper;


    /**
     * Class to define Python wrapper to C class/type
     **/
    template<typename T>
    struct PythonClassWrapper<T,
			      typename std::enable_if<!std::is_arithmetic<T>::value && !std::is_floating_point<T>::value && !std::is_array<T>::value && !std::is_pointer<T>::value>::type>
            : public CommonBaseWrapper {
        // Convenience typedefs
        typedef CommonBaseWrapper::Base Base;
        typedef PythonClassWrapper DereferencedWrapper;
        typedef PythonClassWrapper<T const,   void> ConstWrapper;
        typedef PythonClassWrapper<typename std::remove_const<T>::type> NonConstWrapper;
        typedef PythonClassWrapper<typename std::remove_reference<T>::type> NoRefWrapper;
        typedef PythonClassWrapper<typename std::remove_const<typename std::remove_reference<T>::type>::type> NoRefNonConstWrapper;
        typedef typename ObjectLifecycleHelpers::BasicAlloc<T>::ConstructorContainer ConstructorContainer;
        typedef typename ConstructorContainer::constructor constructor;
        typedef typename std::remove_reference<T>::type T_NoRef;
        typedef PyTypeObject *TypePtr_t;


        /**
         * return the C-likde object associated with this Python wrapper
         */
        typename PythonClassWrapper::T_NoRef *get_CObject() ;

        static PyTypeObject* getType(){
            return &Type;
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
        static PythonClassWrapper *createPy(const ssize_t arraySize, 
					                        ObjContainer<T_NoRef> *const cobj,
                                            const bool isAllocated,
                                            const bool inPlace,
                                            PyObject *referencing = nullptr) ;

        /**
         * Add a constructor for this typeEr
         **/
        static void addConstructorBase(const char *const kwlist[], constructor c) ;

        /**
         * Create an instance of underlying class based on python arguments converting them to C to call
         * the constructor
         *
         * @param kwlist: list of keyword names of the Python parameters
         * @param args: list of Python args from the Python call into the C constructor
         * @params kwds: keywoards bassed into the copnstructor
         * @param cobj: container to hold the C-like object created
         * @param inPlace: whether to create in-memory (existing allocation)
         **/
        template<typename ...Args>
        static bool create(const char *const kwlist[], PyObject *args, PyObject *kwds, ObjContainer<T_NoRef> *&cobj,
                           const bool inPlace) ;

        /**
         * return Python object representing the address of the contained object
         **/
        static PyObject *addr(PyObject *self, PyObject *args) ;

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char *const name, typename ReturnType, typename ...Args>
        static void addClassMethod(ReturnType(*method)(Args...), const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addClassMethodVarargs(ReturnType(*method)(Args... ...), const char *const kwlist[]);

       /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template< typename ...Args>
        static void addConstructor(const char *const kwlist[]){
            addConstructorBase(kwlist, &create<Args...>);
        }

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<typename _Container, const char *const name, typename ReturnType, typename ...Args>
        static void addMethodTempl(
                typename _Container::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<bool is_const, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod(
                typename MethodContainer<T_NoRef, is_const>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]){
            addMethodTempl<MethodContainer<T_NoRef, is_const>, name, ReturnType, Args...>(method, kwlist);
        }

        template<bool is_const, const char *const name, typename ReturnType, typename ...Args>
        static void addMethodVarargs(
                typename MethodContainerVarargs<T_NoRef, is_const>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]){
            addMethodTempl<MethodContainerVarargs<T_NoRef, is_const>, name, ReturnType, Args...>(method, kwlist);
         }


        template<typename _Container, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__inv__templ(
                typename _Container::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<bool is_const, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__inv__(
                typename MethodContainer<T_NoRef, is_const>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]){
            addMethod__inv__templ< MethodContainer<T_NoRef, is_const>, name, ReturnType, Args...>(method, kwlist);
        }


        template<typename _Container, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__add__templ(
                typename _Container::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<bool is_const, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__add__(
                typename MethodContainer<T_NoRef, is_const>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]){
            addMethod__add__templ<MethodContainer<T_NoRef, is_const>, name, ReturnType, Args...>(method, kwlist);
        }

        template<typename _Container, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__sub__templ(
                typename _Container::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);


        template<bool is_const, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__sub__(
                typename MethodContainer<T_NoRef, is_const>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]){
            addMethod__sub__templ<MethodContainer<T_NoRef, is_const>, name, ReturnType, Args...>(method, kwlist);
         }

        template<typename _Container, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__mul__templ(
                typename _Container::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<bool is_const, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__mul__(
                typename MethodContainer<T_NoRef, is_const>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]){
            addMethod__mul__templ<MethodContainer<T_NoRef>, name, ReturnType, Args...>(method, kwlist);
        }

        template<typename _Container, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__div__templ(
                typename _Container::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<bool is_const, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__div__(
                typename MethodContainer<T_NoRef, is_const>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]){
            addMethod__div__templ< MethodContainer<T_NoRef, is_const>, name, ReturnType, Args...>(method, kwlist);
         }

        template<typename _Container, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__and__templ(
                typename _Container::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<bool is_const, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__and__(
                typename MethodContainer<T_NoRef, is_const>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]){
            addMethod__and__templ<MethodContainer<T_NoRef, is_const>, name, ReturnType, Args...>(method, kwlist);
         }

        template<typename _Container, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__or__templ(
                typename _Container::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<bool is_const, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__or__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]){
            addMethod__and__templ<MethodContainer<T_NoRef, is_const>, name, ReturnType, Args...>(method, kwlist);
         }

        template<typename _Container, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__xor__templ(
                typename _Container::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<bool is_const, char *const name, typename ReturnType, typename ...Args>
        static void addMethod__xor__(
                typename MethodContainer<T_NoRef, is_const>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]){
            addMethod__and__templ<MethodContainer<T_NoRef, is_const>, name, ReturnType, Args...>(method, kwlist);
         }

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__lshift__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__rshift__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<typename _Container, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__mod__templ(
                typename _Container::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<bool is_const, const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__mod__(
                typename MethodContainer<T_NoRef, is_const>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]){
            addMethod__mod__templ<MethodContainer<T_NoRef, is_const>, name, ReturnType, Args...>(method, kwlist);
         }

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__iadd__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__isub__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__imul__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__imod__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__ilshift__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__irshift__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__iand__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__ior__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__ixor__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);


        template< typename KeyType, typename ValueType>
        static void addMapOperatorMethod( typename MethodContainer<T_NoRef, false>::template Container<operatormapname, ValueType, KeyType>::method_t method);

        template< typename KeyType, typename ValueType>
        static void addMapOperatorMethodConst( typename MethodContainer<T_NoRef, true>::template Container<operatormapname, ValueType, KeyType>::method_t method);

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

        template<const char* const type_name, typename EnumType>
        static int addEnumClassValue( const char* const name, EnumType value){
            const bool inited = PythonClassWrapper<EnumType>::_isInitialized;
            PyObject* pyval = toPyObject<EnumType>(value, false, 1);
            PythonClassWrapper<EnumType>::_isInitialized = inited; // chicken and egg here, only initialize to create instances that then get added back into class
            int status = pyval?0:-1;
            if (pyval){
                addClassMember(name, pyval);
            }
            return status;
        }

        /**
        * Add a base class to this Python definition (called before initialize)
        **/
        static void addBaseClass(PyTypeObject *base) ;

        /**
         * Add a mutable bit field to this Python type definition
         **/
        template<const char *const name, typename FieldType, const size_t bits>
        static void addBitField(
                typename BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::getter_t &getter,
                typename BitFieldContainer<T_NoRef>::template Container<name, FieldType,  bits>::setter_t &setter);

        /**
         * Add a constant bit field to this Python type definition
         **/
        template<const char *const name, typename FieldType, const size_t bits>
        static void addBitFieldConst(
                typename BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::getter_t &getter);

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

        void set_contents(typename std::remove_reference<T>::type *ptr, const bool allocated, const bool inPlace);

        static bool checkType(PyObject *const obj);

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &Type;
        }

        static std::string get_name();

        static bool isInitialized(){return _isInitialized;}

        template<typename C, typename E>
        friend
        class InitHelper;

        constexpr PythonClassWrapper(): _CObject(nullptr), _arraySize(0), _allocated(false), _inPlace(false){}

        template<bool is_base_return_complete, bool with_ellipsis, typename ReturnType, typename ...Args>
        friend struct PythonFunctionWrapper;

        template<typename Class, typename Z=void>
        class PyAssign ;

        template<typename Class >
        class PyAssign<Class, typename std::enable_if<!std::is_integral<typename std::remove_reference<Class>::type>::value &&
	                                              !std::is_floating_point<typename std::remove_reference<Class>::type>::value &&
	                                              !is_C_stringable<Class>::value>::type> {
        public:
            static int
            assign(PyObject *self, PyObject *value) {
                if (!PyObject_TypeCheck(value, &PythonClassWrapper<Class>::Type)) {
                    return -1;
                }
                PythonClassWrapper<T> *self_ = (PythonClassWrapper<T> *) self;
                PythonClassWrapper<Class> *value_ = (PythonClassWrapper<Class> *) value;
                *(self_->get_CObject()) = *(value_->get_CObject());
                return 0;
            }
        };

        template<typename Class>
        class PyAssign<Class,  typename std::enable_if<std::is_integral<typename std::remove_reference<Class>::type>::value >::type>{
        public:
            static int
            assign(PyObject *self, PyObject *value) {
                PythonClassWrapper<T> *self_ = (PythonClassWrapper<T> *) self;
                if (PyLong_Check(value)) {
                    Class value_ = (Class) PyLong_AsLong(value);
                    *(self_->get_CObject()->ptr()) = value_;
                } else if (PyInt_Check(value)) {
                    Class value_ = (Class) PyInt_AsLong(value);
                    *(self_->get_CObject()->ptr()) = value_;
                } else if (PyBool_Check(value)) {
                    Class value_ = (value == Py_True);
                    *(self_->get_CObject()) = value_;
                } else {
                    return -1;
                }
                return 0;
            }
        };

        template<typename Class>
        class PyAssign<Class,  typename std::enable_if<std::is_floating_point<typename std::remove_reference<Class>::type>::value>::type > {
        public:
            static int assign(PyObject *self, PyObject *value) {
                PythonClassWrapper<T> *self_ = (PythonClassWrapper<T> *) self;
                if (PyFloat_Check(value)) {
                    Class value_ = (Class) PyFloat_AsDouble(value);
                    *(self_->get_CObject()) = value_;
                } else {
                    return -1;
                }
                return 0;
            }
        };

        template<typename Class>
        class PyAssign<Class, typename std::enable_if<is_C_stringable<Class>::value>::type> {
        public:
            static int assign(PyObject *self, PyObject *value) {
                PythonClassWrapper<T> *self_ = (PythonClassWrapper<T> *) self;
                if (PyString_Check(value)) {
                    const char* const value_ = PyString_AsString(value);
                    *(self_->get_CObject()) = const_cast<Class>(value_);
                } else {
                    return -1;
                }
                return 0;
            }
        };

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
        ObjContainer<T_NoRef> *_CObject;

    private:

        static PyObject* getThis(PyObject* self){
            return addr(self, nullptr);
        }

        static int
        _pyAssign(PyObject* self, PyObject* value){
            for ( _setattrfunc assigner: _assigners){
                if( assigner(self, value) == 0){
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
            return _member_setters[attrname](self, value);
        }

        static PyObject* _pyGetAttr(PyObject* self,  char* attrname){
            if (!_member_getters.count(attrname)){
              return PyObject_GenericGetAttr(self, PyString_FromString(attrname));
            }
            return _member_getters[attrname](self);
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
        static bool _createBaseBase(ObjContainer<T_NoRef> *&cobj, Args ... args);

        template<typename ...Args, int ...S >
        static bool _createBase
                (ObjContainer<T_NoRef> *&cobj, PyObject *args, PyObject *kwds,
                 const char *const kwlist[], container<S...> unused1, _____fake<Args> *... unused2);

        /**
         * helper methods
         **/
        static void _addMethod(PyMethodDef method);



        static PyObject* _mapGet(PyObject* self, PyObject* key);
        static int _mapSet(PyObject* self, PyObject* key, PyObject* value);

        static std::string _name;
        static std::vector<ConstructorContainer> _constructors;
        static std::vector<PyMethodDef> _methodCollection;
        static std::map<std::string, std::pair<std::function<PyObject*(PyObject*, PyObject*)>,
                                     std::function<int(PyObject*, PyObject*, PyObject*)>
                                     >
                          >_mapMethodCollection;
        static std::map<std::string, _getattrfunc > _member_getters;
        static std::map<std::string, _setattrfunc > _member_setters;
        static std::vector<_setattrfunc > _assigners;
        static std::vector<PyTypeObject *> _baseClasses;
        size_t _arraySize;
        bool _allocated;
        bool _inPlace;

       private:
        static PyTypeObject Type;
        static TypePtr_t constexpr TypePtr = &Type;

    };


  template<>
  struct PythonClassWrapper<const double>;



}
#endif
