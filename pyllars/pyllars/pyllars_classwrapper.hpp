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
#include "pyllars_defns.hpp"
#include "pyllars_classmethodsemantics.hpp"
#include "pyllars_constmethodcallsemantics.hpp"
#include "pyllars_methodcallsemantics.hpp"
#include "pyllars_object_lifecycle.hpp"
//#include "pyllars_conversions.hpp"
typedef const char cstring[];
static constexpr cstring operatormapname = "operator[]";

namespace __pyllars_internal {
    typedef int (*_setattrfunc)(PyObject*, PyObject*);
    typedef PyObject* (*_getattrfunc)(PyObject*);

    template<typename T, typename E=void>
    class InitHelper;

    template<class CClass>
    class ConstMethodContainer;

    template<class CClass>
    class ConstMemberContainer;

    template<class CClass>
    class ClassMemberContainer;

    template<class CClass>
    class ConstClassMemberContainer;

    template<bool is_base_return_complete, bool with_ellipsis, typename ReturnType, typename ...Args>
    struct PythonFunctionWrapper;
    /**
     * Class to define Python wrapper to C class/type
     **/
    template<typename T>
    struct PythonClassWrapper<T,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>
            : public CommonBaseWrapper {

        typedef CommonBaseWrapper::Base Base;
        typedef PythonClassWrapper DereferencedWrapper;
        typedef PythonClassWrapper<T const,   void> ConstWrapper;
        typedef PythonClassWrapper<typename std::remove_const<T>::type> NonConstWrapper;
        typedef PythonClassWrapper<typename std::remove_reference<T>::type> NoRefWrapper;

        typedef typename std::remove_reference<T>::type T_NoRef;
        typedef typename ObjectLifecycleHelpers::BasicAlloc<T>::ConstructorContainer
        ConstructorContainer;
        typedef typename ConstructorContainer::constructor constructor;
        typedef PyTypeObject *TypePtr_t;


        template<typename Z = T>
        typename std::remove_reference<T>::type *get_CObject() ;

        static PyTypeObject Type;
        static TypePtr_t constexpr TypePtr = &Type;

         /**
         * Python initialization of underlying type, called to init and register type with
         * underlying Python system
         **/
        static int initialize(const char *const name, const char *const module_entry_name,
                              PyObject *module, const char *const fullname = nullptr) ;

        /**
         * create a python object of this class type
         **/
        static PythonClassWrapper *createPy(const ssize_t arraySize, 
					                        ObjContainer<T_NoRef> *const cobj, const bool isAllocated,
                                            const bool inPlace,
                                            PyObject *referencing = nullptr, const size_t depth = 0) ;

        /**
         * Add a constructor to the list contained
         **/
        static void addConstructor(const char *const kwlist[], constructor c) ;

        /**
         * Create an instance of underlying class based on python arguments converting them to C to call
         * the constructor
         **/
        template<typename ...Args>
        static bool create(const char *const kwlist[], PyObject *args, PyObject *kwds, ObjContainer<T_NoRef> *&cobj,
                           const bool inPlace) ;

        /**
         * return python object representing the address of the contained object
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
        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__inv__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__add__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__sub__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__mul__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__div__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__and__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__or__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__xor__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__lshift__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__rshift__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod__mod__(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

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

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char *const name, typename ReturnType, typename ...Args>
        static void addConstMethod(
                typename ConstMethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        template< typename KeyType, typename ValueType>
        static void addMapOperatorMethod( typename MethodContainer<T_NoRef>::template Container<operatormapname, ValueType, KeyType>::method_t method);

        template< typename KeyType, typename ValueType>
        static void addMapOperatorMethodConst( typename ConstMethodContainer<T_NoRef>::template Container<operatormapname, ValueType, KeyType>::method_t method);

        /**
         * add a class-wide (static) member
         **/
        static void addClassMember(const char *const name, PyObject *pyobj) ;

        /**
         * Add an enum value to the class
         */
        static void addEnumValue( const char* const name, long value){
            addClassMember(name, PyInt_FromLong(value));
        }

        /**
        * Add a base class to this Python definition (called before initialize)
        **/
        static void addBaseClass(PyTypeObject *base) ;

        /**
         * add a getter method for the given compile-time-known named public class member
         **/
        template<const char *const name, ssize_t size, typename Type>
        static void addAttribute(
                typename MemberContainer<T_NoRef>::template Container<name, Type[size]>::member_t member,
                const ssize_t array_size){
            assert(array_size == size);
            static const char *const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
            //static const char *const getter_prefix = "get_";
            //char *getter_name = new char[strlen(name) +strlen(getter_prefix)+1];
            //snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
            MemberContainer<T_NoRef>::template Container<name, Type[size]>::member = member;
            _member_getters[name] = MemberContainer<T_NoRef>::template Container<name, Type[size]>::get;
            _member_setters[name] = MemberContainer<T_NoRef>::template Container<name, Type[size]>::set;
        }

        template<const char *const name, ssize_t size, typename Type>
        static void addConstAttribute(
                typename MemberContainer<T_NoRef>::template Container<name, Type[size]>::member_t member,
                const ssize_t array_size){
            assert(array_size == size);
            static const char *const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
            MemberContainer<T_NoRef>::template Container<name, Type[size]>::member = member;
            _member_getters[name] = MemberContainer<T_NoRef>::template Container<name, Type[size]>::get;
        }
        /**
         * Add a mutable bit field to this Python type definition
         **/
        template<const char *const name, typename Type, const size_t bits>
        static void addBitField(
                typename BitFieldContainer<T_NoRef>::template Container<name, Type, bits>::getter_t &getter,
                typename BitFieldContainer<T_NoRef>::template Container<name, Type,  bits>::setter_t &setter){
            static const char *const doc = "Get bit-field attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
            BitFieldContainer<T_NoRef>::template Container<name, Type, bits>::_getter = getter;
            BitFieldContainer<T_NoRef>::template Container<name, Type, bits>::_setter = setter;
            _member_getters[name] =  BitFieldContainer<T_NoRef>::template Container<name, Type, bits>::get;
            _member_setters[name] =  BitFieldContainer<T_NoRef>::template Container<name, Type, bits>::set;

        }

        /**
         * Add a constant bit field to this Python type definition
         **/
        template<const char *const name, typename Type, const size_t bits>
        static void addConstBitField(
                typename BitFieldContainer<T_NoRef>::template Container<name, Type, bits>::getter_t &getter){
            static const char *const doc = "Get bit-field attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
            BitFieldContainer<T_NoRef>::template ConstContainer<name, Type, bits>::_getter = getter;
            _member_getters[name] = BitFieldContainer<T_NoRef>::template ConstContainer<name, Type, bits>::get;
        }

        /**
         * add a getter method for the given compile-time-known named public class member
         **/
        template<const char *const name, typename Type>
        static void addAttribute(typename MemberContainer<T_NoRef>::template Container<name, Type>::member_t member){

            static const char *const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
            const ssize_t array_size = ArraySize<Type>::size;
            MemberContainer<T_NoRef>::template Container<name, Type>::member = member;
            MemberContainer<T_NoRef>::template Container<name, Type>::array_size = array_size;
            _member_getters[name] =  MemberContainer<T_NoRef>::template Container<name, Type>::get;
            _member_setters[name] =   MemberContainer<T_NoRef>::template Container<name, Type>::set;
        }


        /**
         * add a getter method for the given compile-time-known named public class member
         **/
        template<const char *const name, typename Type>
        static void addConstAttribute(
                typename ConstMemberContainer<T_NoRef>::template Container<name, Type>::member_t member){

            static const char *const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
            ConstMemberContainer<T_NoRef>::template Container<name, Type>::member = member;
            _member_getters[name] = ConstMemberContainer<T_NoRef>::template Container<name, Type>::get;
        }


        /**
         * add a getter method for the given compile-time-known named public static class member
         **/
        template<const char *const name, typename Type>
        static void addClassAttribute(Type *member){

            static const char *const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
            //static const char *const getter_prefix = "get_";
            //char *getter_name = new char[strlen(name) +strlen(getter_prefix)+1];
            //snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
            //static const char *const kwlist[] = {"value", nullptr};
	    //            ClassMemberContainer<T_NoRef>::template Container<name, Type>::kwlist = kwlist;
            ClassMemberContainer<T_NoRef>::template Container<name, Type>::member = member;
            PyMethodDef pyMeth = {name,
                                  (PyCFunction) ClassMemberContainer<T_NoRef>::template Container<name, Type>::call,
                                  METH_KEYWORDS | METH_CLASS,
                                  doc_string
            };
            _addMethod(pyMeth);
	    //UPDATE?? _memberSettersDict[name] = ClassMemberContainer<T>::template Container<name, Type>::setFromPyObject;
        }


        /**
         * add a getter method for the given compile-time-known named public static class member
         **/
        template<const char *const name, typename Type>
        static void addConstClassAttribute(Type const *member){

            static const char *const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
            //static const char *const getter_prefix = "get_";
            //char *getter_name = new char[strlen(name) +strlen(getter_prefix)+1];
            //snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
            //static const char *const kwlist[] = {"value", nullptr};
            //ConstClassMemberContainer<T_NoRef>::template Container<name, Type>::kwlist = kwlist;
            ConstClassMemberContainer<T_NoRef>::template Container<name, Type>::member = member;
            PyMethodDef pyMeth = {name,
                                  (PyCFunction) ConstClassMemberContainer<T_NoRef>::template Container<name, Type>::call,
                                  METH_KEYWORDS | METH_CLASS,
                                  doc_string
            };
            _addMethod(pyMeth);
        }

        void set_contents(typename std::remove_reference<T>::type *ptr, const bool allocated, const bool inPlace);

        static bool checkType(PyObject *const obj);

        static PyTypeObject *getType(const size_t unused_depth = 0);

        static std::string get_name();

        static std::string get_module_entry_name();

        static std::string get_full_name();

        static PyObject *parent_module;

        static bool isInitialized(){return _isInitialized;}

        template<typename C, const ssize_t size, typename depth>
        friend
        struct PythonCPointerWrapper;

        template<typename C, typename E>
        friend
        class InitHelper;

        constexpr PythonClassWrapper(): _CObject(nullptr), _arraySize(0), _allocated(false), _inPlace(false), _depth(0){}

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

        /**
         * in order to convert args to a parameter list of args, need to go through
         * two lower level create functions
         **/
        template<typename ...Args>
        static bool _createBaseBase(ObjContainer<T_NoRef> *&cobj, Args &... args);

        template<typename ...Args,int ...S >
        static bool _createBase
                (ObjContainer<T_NoRef> *&cobj, PyObject *args, PyObject *kwds,
                 const char *const kwlist[], container<S...> unused1, Args *... unused2);

        /**
         * helper methods
         **/
        static void _addMethod(PyMethodDef method);



        static PyObject* _mapGet(PyObject* self, PyObject* key);
        static int _mapSet(PyObject* self, PyObject* key, PyObject* value);

        static std::string _name;
        static std::string _module_entry_name;
        static std::string _full_name;
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
        static bool _isInitialized;
        size_t _arraySize;
        bool _allocated;
        bool _inPlace;
      public:
        size_t _depth;
    };

}
#endif
