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
    class InitHelper {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
    };

    /**
     * Specialization for integers
     **/
    template<typename T>
    class InitHelper<T, typename std::enable_if<std::is_integral<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
    };

    /**
     * Specialization for floating point
     **/
    template<typename T>
    class InitHelper<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };

    //specialize for non-numeric fundamental types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            !std::is_void<T>::value && !std::is_arithmetic<T>::value && std::is_fundamental<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };

    //specialize for non-numeric fundamental types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<std::is_void<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };


    //specialize for pointer types:
    template<typename T>
    class InitHelper<T,  typename std::enable_if<!std::is_void<T>::value && !std::is_arithmetic<T>::value &&
                                                std::is_pointer<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };

    //specialize for copiable non-fundamental reference types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<!std::is_void<T>::value &&
                                                !std::is_integral<typename std::remove_reference<T>::type>::value &&
                                                !std::is_floating_point<typename std::remove_reference<T>::type>::value &&
                                                std::is_copy_constructible<typename std::remove_reference<T>::type>::value &&
                                                std::is_reference<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };

    //  specialize for non-copiable non-fundamental reference types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            !std::is_integral<typename std::remove_reference<T>::type>::value &&
            !std::is_floating_point<typename std::remove_reference<T>::type>::value &&
            !std::is_copy_constructible<typename std::remove_reference<T>::type>::value &&
            std::is_reference<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };

    //specialize for integral reference types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            std::is_integral<typename std::remove_reference<T>::type>::value &&
            std::is_reference<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };

    //specialize for floating point reference types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            std::is_floating_point<typename std::remove_reference<T>::type>::value &&
            std::is_reference<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
    };

    //specialize for other complex types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            !std::is_arithmetic<T>::value && !std::is_reference<T>::value &&
            !std::is_pointer<T>::value && !std::is_fundamental<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) ;
    };


    /**
     * Class to define Python wrapper to C class/type
     **/
    template<typename T>
    struct PythonClassWrapper<T, UNKNOWN_SIZE,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>
            : public CommonBaseWrapper {


        typedef CommonBaseWrapper::Base Base;
        typedef PythonClassWrapper DereferencedWrapper;
        typedef PythonClassWrapper<T const,  UNKNOWN_SIZE, void> ConstWrapper;
        typedef PythonClassWrapper<typename std::remove_const<T>::type> NonConstWrapper;
        typedef PythonClassWrapper<typename std::remove_reference<T>::type> NoRefWrapper;

        typedef typename std::remove_reference<T>::type T_NoRef;

        template<typename Z = T>
        typename std::remove_reference<T>::type *get_CObject() ;

        static PyTypeObject Type;
        typedef PyTypeObject *TypePtr_t;
        static TypePtr_t constexpr TypePtr = &Type;

        //static int initialize() { return Type.tp_name ? 0 : -1; }

        static int initialize(const char *const name, const char *const module_entry_name,
                              PyObject *module, const char *const fullname = nullptr) ;

        static PythonClassWrapper *createPy(const ssize_t arraySize, ObjContainer<T_NoRef> *const cobj, const bool isAllocated,
                                            PyObject *referencing, const size_t depth = 0) ;

        /**
         * Add a constructor to the list contained
         **/
        typedef typename ObjectLifecycleHelpers::BasicAlloc<T, PythonClassWrapper<T_NoRef *> >::ConstructorContainer ConstructorContainer;
        typedef typename ConstructorContainer::constructor constructor;

        static void addConstructor(const char *const kwlist[], constructor c) ;

        template<typename ...Args>
        static bool create(const char *const kwlist[], PyObject *args, PyObject *kwds, ObjContainer<T_NoRef> *&cobj) ;

        static PyObject *addr(PyObject *self, PyObject *args) ;

        static bool findMemberSetter(const char *const name) ;

        static void callMemberSetter(const char *const name, T_NoRef *this_, PyObject *pyval);

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


        static void addClassMember(const char *const name, PyObject *pyobj) ;

        static void addEnumValue( const char* const name, long value){
            addClassMember(name, PyInt_FromLong(value));
        }

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


        template<const char *const name, typename Type, const size_t bits>
        static void addBitField(
                typename BitFieldContainer<T_NoRef>::template Container<name, Type, bits>::getter_t &getter,
                typename BitFieldContainer<T_NoRef>::template Container<name, Type,  bits>::setter_t &setter) ;

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
        static void addAttribute(typename MemberContainer<T_NoRef>::template Container<name, Type>::member_t member,
                                 const ssize_t array_size){

            static const char *const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
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

        /* T_NoRef*/ ObjContainer<T_NoRef> *_CObject;

    private:

        static int
        _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds);

        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

        static void _free(void *self_);

        static void _dealloc(PythonClassWrapper *self);

        template<typename ...PyO>
        static bool _parsePyArgs(const char *const kwlist[], PyObject *args, PyObject *kwds, PyO *&...pyargs);

        template<typename ...Args>
        static bool _createBaseBase(ObjContainer<T_NoRef> *&cobj, Args &... args);

        template<typename ...Args,int ...S >
        static bool _createBase
                (ObjContainer<T_NoRef> *&cobj, PyObject *args, PyObject *kwds,
                 const char *const kwlist[], container<S...> unused1, Args *... unused2);

        static void _addMethod(PyMethodDef method);

        static std::string _name;
        static std::string _module_entry_name;
        static std::vector<ConstructorContainer> _constructors;
        static std::map<std::string, typename MethodContainer<T>::setter_t> _memberSettersDict;
        static std::vector<PyMethodDef> _methodCollection;
        static std::vector<PyTypeObject *> _baseClasses;

        char *_raw_storage;
        bool _allocated;

    };


    template<typename T>
    PyObject *PythonClassWrapper<T, UNKNOWN_SIZE,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            parent_module = nullptr;
    template<typename T>
    std::vector<PyMethodDef> PythonClassWrapper<T, UNKNOWN_SIZE,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            _methodCollection = std::vector<PyMethodDef>(emptyMethods, emptyMethods + 1);
    template<typename T>
    std::vector<PyTypeObject *> PythonClassWrapper<T, UNKNOWN_SIZE,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            _baseClasses = std::vector<PyTypeObject *>();
    template<typename T>
    std::map<std::string, typename MethodContainer<T>::setter_t>
            PythonClassWrapper<T, UNKNOWN_SIZE,
                    typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            _memberSettersDict = std::map<std::string, typename MethodContainer<T>::setter_t>();
    template<typename T>
    std::vector<typename PythonClassWrapper<T, UNKNOWN_SIZE, typename std::enable_if<
            !std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
    ConstructorContainer>
            PythonClassWrapper<T, UNKNOWN_SIZE,
                    typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            _constructors;


    template<typename T>
    std::string PythonClassWrapper<T, UNKNOWN_SIZE,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::_name;
    template<typename T>
    std::string PythonClassWrapper<T, UNKNOWN_SIZE,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::_module_entry_name;
    template<typename T>
    PyTypeObject PythonClassWrapper<T, UNKNOWN_SIZE,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            Type = {

                    PyObject_HEAD_INIT(nullptr)
                    0,                         /*ob_size*/
                    nullptr,             /*tp_name*/ /*filled on init*/
                    sizeof(PythonClassWrapper),             /*tp_basicsize*/
                    0,                         /*tp_itemsize*/
                    (destructor) PythonClassWrapper::_dealloc, /*tp_dealloc*/
                    nullptr,                         /*tp_print*/
                    nullptr,                         /*tp_getattr*/
                    nullptr,                         /*tp_setattr*/
                    nullptr,                         /*tp_compare*/
                    nullptr,                         /*tp_repr*/
                    nullptr,                         /*tp_as_number*/
                    nullptr,                         /*tp_as_sequence*/
                    nullptr,                         /*tp_as_mapping*/
                    nullptr,                         /*tp_hash */
                    nullptr,                         /*tp_call*/
                    nullptr,                         /*tp_str*/
                    nullptr,                         /*tp_getattro*/
                    nullptr,                         /*tp_setattro*/
                    nullptr,                         /*tp_as_buffer*/
                    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
                    "PythonClassWrapper object",           /* tp_doc */
                    nullptr,                       /* tp_traverse */
                    nullptr,                       /* tp_clear */
                    nullptr,                       /* tp_richcompare */
                    0,                               /* tp_weaklistoffset */
                    nullptr,                       /* tp_iter */
                    nullptr,                       /* tp_iternext */
                    PythonClassWrapper::_methodCollection.data(),             /* tp_methods */
                    nullptr,             /* tp_members */
                    nullptr,                         /* tp_getset */
                    Base::TypePtr,                         /* tp_base */
                    nullptr,                         /* tp_dict */
                    nullptr,                         /* tp_descr_get */
                    nullptr,                         /* tp_descr_set */
                    0,                         /* tp_dictoffset */
                    (initproc) PythonClassWrapper::_init,  /* tp_init */
                    nullptr,                         /* tp_alloc */
                    PythonClassWrapper::_new,             /* tp_new */
                    _free,                         /*tp_free*/
                    nullptr,                         /*tp_is_gc*/
                    nullptr,                         /*tp_bases*/
                    nullptr,                         /*tp_mro*/
                    nullptr,                         /*tp_cache*/
                    nullptr,                         /*tp_subclasses*/
                    nullptr,                          /*tp_weaklist*/
                    nullptr,                          /*tp_del*/
                    0,                          /*tp_version_tag*/
            };


}
#endif
