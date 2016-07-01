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

namespace __pyllars_internal {

    static PyMethodDef emptyMethods[] = {{nullptr, nullptr, 0, nullptr}};

    template<typename T, typename E=void>
    class InitHelper;


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
        typedef typename ObjectLifecycleHelpers::BasicAlloc<T, PythonClassWrapper<T_NoRef *> >::ConstructorContainer
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
        static PythonClassWrapper *createPy(const ssize_t arraySize, ObjContainer<T_NoRef> *const cobj, const bool isAllocated,
                                            PyObject *referencing, const size_t depth = 0) ;

        /**
         * Add a constructor to the list contained
         **/
        static void addConstructor(const char *const kwlist[], constructor c) ;

        /**
         * Create an instance of underlying class based on python arguments converting them to C to call
         * the constructor
         **/
        template<typename ...Args>
        static bool create(const char *const kwlist[], PyObject *args, PyObject *kwds, ObjContainer<T_NoRef> *&cobj) ;

        /**
         * return python object representing the address of the contained object
         **/
        static PyObject *addr(PyObject *self, PyObject *args) ;

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char *const name, typename ReturnType, typename ...Args>
        static void addClassMethod(ReturnType(*method)(Args...), const char *const kwlist[]);

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char *const name, typename ReturnType, typename ...Args>
        static void addConstMethod(
                typename ConstMethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]);

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
            static const char *const getter_prefix = "get_";
            //char *getter_name = new char[strlen(name) +strlen(getter_prefix)+1];
            //snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
            MemberContainer<T_NoRef>::template Container<name, Type[size]>::member = member;
            PyMethodDef pyMeth = {name,
                                  (PyCFunction) MemberContainer<T_NoRef>::template Container<name, Type[size]>::call,
                                  METH_KEYWORDS,
                                  doc_string
            };
            _addMethod(pyMeth);
            _memberSettersDict[name] = MemberContainer<T>::template Container<name, Type[size]>::setFromPyObject;
        }

        /**
         * Add a mutable bit field to this Python type definition
         **/
        template<const char *const name, typename Type, const size_t bits>
        static void addBitField(
                typename BitFieldContainer<T_NoRef>::template Container<name, Type, bits>::getter_t &getter,
                typename BitFieldContainer<T_NoRef>::template Container<name, Type,  bits>::setter_t &setter) ;

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
            PyMethodDef pyMeth = {name,
                                  (PyCFunction) BitFieldContainer<T_NoRef>::template ConstContainer<name, Type, bits>::call,
                                  METH_KEYWORDS,
                                  doc_string
            };
            _addMethod(pyMeth);
            _memberSettersDict[name] = BitFieldContainer<T>::template ConstContainer<name, Type, bits>::setFromPyObject;
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
            PyMethodDef pyMeth = {name,
                                  (PyCFunction) MemberContainer<T_NoRef>::template Container<name, Type>::call,
                                  METH_KEYWORDS,
                                  doc_string
            };
            _addMethod(pyMeth);
            _memberSettersDict[name] = MemberContainer<T>::template Container<name, Type>::setFromPyObject;
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
            PyMethodDef pyMeth = {name,
                                  (PyCFunction) ConstMemberContainer<T_NoRef>::template Container<name, Type>::call,
                                  METH_KEYWORDS,
                                  doc_string
            };
            _addMethod(pyMeth);
        }


        /**
         * add a getter method for the given compile-time-known named public static class member
         **/
        template<const char *const name, typename Type>
        static void addClassAttribute(Type *member){

            static const char *const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
            static const char *const getter_prefix = "get_";
            //char *getter_name = new char[strlen(name) +strlen(getter_prefix)+1];
            //snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
            static const char *const kwlist[] = {"value", nullptr};
            ClassMemberContainer<T_NoRef>::template Container<name, Type>::kwlist = kwlist;
            ClassMemberContainer<T_NoRef>::template Container<name, Type>::member = member;
            PyMethodDef pyMeth = {name,
                                  (PyCFunction) ClassMemberContainer<T_NoRef>::template Container<name, Type>::call,
                                  METH_KEYWORDS | METH_CLASS,
                                  doc_string
            };
            _addMethod(pyMeth);
            _memberSettersDict[name] = ClassMemberContainer<T>::template Container<name, Type>::setFromPyObject;
        }


        /**
         * add a getter method for the given compile-time-known named public static class member
         **/
        template<const char *const name, typename Type>
        static void addConstClassAttribute(Type const *member){

            static const char *const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
            static const char *const getter_prefix = "get_";
            //char *getter_name = new char[strlen(name) +strlen(getter_prefix)+1];
            //snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
            static const char *const kwlist[] = {"value", nullptr};
            ConstClassMemberContainer<T_NoRef>::template Container<name, Type>::kwlist = kwlist;
            ConstClassMemberContainer<T_NoRef>::template Container<name, Type>::member = member;
            PyMethodDef pyMeth = {name,
                                  (PyCFunction) ConstClassMemberContainer<T_NoRef>::template Container<name, Type>::call,
                                  METH_KEYWORDS | METH_CLASS,
                                  doc_string
            };
            _addMethod(pyMeth);
        }

        void set_contents(typename std::remove_reference<T>::type *ptr, bool allocated);

        static bool checkType(PyObject *const obj);

        static PyTypeObject *getType(const size_t unused_depth = 0);

        static std::string get_name();

        static std::string get_module_entry_name();

        static PyObject *parent_module;

        template<typename C, const ssize_t size, typename depth>
        friend
        struct PythonCPointerWrapper;

        template<typename C, typename E>
        friend
        class InitHelper;

    protected:

        static PyObject *alloc(PyObject *cls, PyObject *args, PyObject *kwds);

        // must use object container, since code may have new and delete private and container
        // will shield us from that
        ObjContainer<T_NoRef> *_CObject;

    private:

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

        static bool _findMemberSetter(const char *const name) ;

        static std::string _name;
        static std::string _module_entry_name;
        static std::vector<ConstructorContainer> _constructors;
        static std::map<std::string, typename MethodContainer<T>::setter_t> _memberSettersDict;
        static std::vector<PyMethodDef> _methodCollection;
        static std::vector<PyTypeObject *> _baseClasses;

        char *_raw_storage;
        bool _allocated;

    };

}
#endif
