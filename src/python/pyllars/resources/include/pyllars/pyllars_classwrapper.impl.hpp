#ifndef PYLLARS__CLASSWRAPPER_IMPL
#define PYLLARS__CLASSWRAPPER_IMPL

#include <limits>
#include <string.h>
#include <utility>

#include "pyllars_classwrapper.hpp"
#include "pyllars_utils.hpp"

#include "pyllars_staticfunctionsemantics.impl.hpp"
#include "pyllars_classmembersemantics.impl.hpp"
#include "pyllars_methodcallsemantics.impl.hpp"
#include "pyllars_conversions.impl.hpp"

namespace {
    constexpr cstring value_name = "_CObject";
}

namespace __pyllars_internal {

    template <typename T>
    long long enum_convert(const T& val){
        if constexpr(std::is_volatile<T>::value){
            return *reinterpret_cast<const volatile typename std::underlying_type<T>::type *>(&val);
        } else {
            return *reinterpret_cast<const typename std::underlying_type<T>::type *>(&val);
        }
    }

    template<typename T>
    class InitHelper {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
    };

    template<typename T>
    std::map<std::string, PyMethodDef> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _methodCollection = std::map<std::string, PyMethodDef>();


    template<typename T>
    std::map<std::string, PyMethodDef> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _methodCollectionConst = std::map<std::string, PyMethodDef>();

    template<typename T>
    std::vector<PyTypeObject *> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _baseClasses = std::vector<PyTypeObject *>();

    template<typename T>
    std::vector<PyTypeObject *> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _baseClassesConst = std::vector<PyTypeObject *>();

    template<typename T>
    std::map<std::string,  const typename std::remove_cv<typename std::remove_reference<T>::type>::type *> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _classEnumValues = std::map<std::string, const typename std::remove_cv<T_NoRef>::type *>();

    template<typename T>
    std::map<std::string, unaryfunc> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _unaryOperators = std::map<std::string, unaryfunc>();
    template<typename T>
    std::map<std::string, unaryfunc> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _unaryOperatorsConst = std::map<std::string, unaryfunc>();

    template<typename T>
    std::map<std::string, binaryfunc> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _binaryOperators = std::map<std::string, binaryfunc>();

    template<typename T>
    std::map<std::string, binaryfunc> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _binaryOperatorsConst = std::map<std::string, binaryfunc>();

    template<typename T>
    std::vector<typename PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::ConstructorContainer>
            PythonClassWrapper<T,
                    typename std::enable_if<is_rich_class<T>::value>::type>::
            _constructors;

    template<typename T>
    std::map<std::string, _getattrfunc>
            PythonClassWrapper<T,
                    typename std::enable_if<is_rich_class<T>::value>::type>::
            _member_getters;
    template<typename T>
    std::map<std::string, _setattrfunc>
            PythonClassWrapper<T,
                    typename std::enable_if<is_rich_class<T>::value>::type>::
            _member_setters;
    template<typename T>
    std::vector<_setattrfunc>
            PythonClassWrapper<T,
                    typename std::enable_if<is_rich_class<T>::value>::type>::
            _assigners;

    template<typename T>
    bool PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::_isInitialized = false;

    template<typename T>
    std::map<std::string, std::pair<std::function<PyObject *(PyObject *, PyObject *)>,
            std::function<int(bool is_const, PyObject *, PyObject *, PyObject *)>
    >
    >
            PythonClassWrapper<T,
                    typename std::enable_if<is_rich_class<T>::value>::type>::_mapMethodCollection;

    template<typename T>
    PyTypeObject PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _Type = {
#if PY_MAJOR_VERSION == 3
                    PyVarObject_HEAD_INIT(NULL, 0)
#else
            PyObject_HEAD_INIT(nullptr)
            0,                         /*ob_size*/
#endif
                    __pyllars_internal::type_name<T>(),             /*tp_name*/
                    sizeof(PythonClassWrapper),             /*tp_basicsize*/
                    0,                         /*tp_itemsize*/
                    (destructor) PythonClassWrapper::_dealloc, /*tp_dealloc*/
                    nullptr,                         /*tp_print*/
                    _pyGetAttr,                         /*tp_getattr*/
                    _pySetAttr,                         /*tp_setattr*/
                    nullptr,                         /*tp_compare*/
                    nullptr,                         /*tp_repr*/
                    new PyNumberMethods{0},          /*tp_as_number*/
                    nullptr,                         /*tp_as_sequence*/
                    nullptr,                         /*tp_as_mapping*/
                    nullptr,                         /*tp_hash */
                    nullptr,                         /*tp_call*/
                    nullptr,                         /*tp_str*/
                    nullptr,                         /*tp_getattro*/
                    nullptr,                         /*tp_setattro*/
                    nullptr,                         /*tp_as_buffer*/
                    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES, /*tp_flags*/
                    "PythonClassWrapper object",           /* tp_doc */
                    nullptr,                       /* tp_traverse */
                    nullptr,                       /* tp_clear */
                    nullptr,                       /* tp_richcompare */
                    0,                               /* tp_weaklistoffset */
                    nullptr,                       /* tp_iter */
                    nullptr,                       /* tp_iternext */
                    nullptr,             /* tp_methods */
                    nullptr,             /* tp_members */
                    nullptr,                         /* tp_getset */
                    nullptr,                         /* tp_base */
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


    template<typename T>
    int InitHelper<T>::init
            (PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
        if (!self) {
            return -1;
        }
        static const char *kwlist[] = {"_CObject", nullptr};
        self->_CObject = nullptr;
            for (auto const &[kwlist_, constructor] : PythonClassWrapper<T>::_constructors) {
                (void) kwlist_;
                try {
                    if ((self->_CObject = constructor(kwlist, args, kwds, nullptr))) {
                        self->_isInitialized = true;
                        return 0;
                    }
                } catch (PyllarsException &) {
                    //try next one
                } catch(std::exception const & e) {
                    PyllarsException::raise_internal_cpp(e.what());
                    return -1;
                } catch (...){
                    PyllarsException::raise_internal_cpp();
                    return -1;
                }
                PyErr_Clear();
            }
            if ((!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
                if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
                    PyErr_Clear();
                    return 0;
                } else {
                    PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                    return -1;
                }
            } else {
                if(!PyErr_Occurred()) {
                    PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
                }
                return -1;
            }

        PyErr_SetString(PyExc_TypeError, "Unknown type for init");
        return -1;
    }



    /**
     * Class to define Python wrapper to C class/type
     *
    */
    template<typename T>
    typename std::remove_reference<T>::type *
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::get_CObject() const{
        return _CObject;
    }

    template<typename T>
    int
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::_initialize(PyTypeObject &Type) {
        typedef typename std::remove_const<
                        typename std::remove_reference<T>::type
        >::type basic_type;
        typedef PythonClassWrapper<basic_type> Basic;
        static bool inited = false;
        if (inited) return 0;
        _isInitialized = false;
        inited = true;

        if constexpr (std::is_enum<T_NoRef >::value){
            static const char* const kwlist[] = {"obj", nullptr};
            addClassMethod<value_name, kwlist, long long(const T_NoRef&), enum_convert<T_NoRef> >();
        }
        if constexpr (std::is_const<T_NoRef >::value){
            for ( auto & element : Basic::_constructors){
                _constructors.push_back(*reinterpret_cast<ConstructorContainer*>(&element));
            }
        } else {
            _constructors = Basic::_constructors;
        }

        PyMethodDef pyMethAlloc = {
                alloc_name_,
                (PyCFunction) alloc,
                METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                "allocate array of single dynamic instance of this class"
        };
        _methodCollectionConst = Basic::_methodCollectionConst; // make this same as type with no garnishment
        if constexpr(std::is_const<T>::value) {
            _methodCollection = Basic::_methodCollection; // make this same as type with no garnishment
        }
        _methodCollection[alloc_name_] = pyMethAlloc;
        if (!Basic::_baseClasses.empty()) {
            if (Basic::_baseClasses.size() > 1) {
                Type.tp_bases = PyTuple_New(Basic::_baseClasses.size());
            } else if (Basic::_baseClasses.size() == 1) {
                if constexpr (std::is_const<T>::value) {
                    Type.tp_base = Basic::_baseClassesConst[0];
                } else {
                    Type.tp_base = Basic::_baseClasses[0];
                }
            }
            for (size_t index = 0; index < Basic::_baseClasses.size(); ++ index){
                auto baseClass = std::is_const<T>::value?_baseClassesConst[index]:_baseClasses[index];
                if (Basic::_baseClasses.size() > 1)
                    PyTuple_SetItem(Type.tp_bases, index, (PyObject *) baseClass);
                // tp_bases not usable for inheritance of methods/members as it doesn't really do the right thing and
                // causes problems on lookup of base classes,
                // so do this manually...
                {
                    PyMethodDef *def = baseClass->tp_methods;
                    if (_methodCollection.count(def->ml_name) == 0) {
                        while (def->ml_name != nullptr) {
                            _methodCollection[def->ml_name] = *def;
                            ++def;
                        }
                    }
                }
                {
                    auto *def = baseClass->tp_getset;
                    while (def->name != nullptr) {
                        if (Basic::_member_getters.count(def->name) == 0) {
                            Basic::_member_setters[def->name] = def->set;
                            Basic::_member_getters[def->name] = def->get;
                        }
                        ++def;
                    }
                }
            }
        }
        Type.tp_methods = new PyMethodDef[_methodCollection.size() +_methodCollectionConst.size() + 1];
        Type.tp_methods[_methodCollection.size() + _methodCollectionConst.size()] = {nullptr};
        // for const-style T, this converts calls to pass in a const T pointer in the std::function call,
        // important for disallowing setting of a const *_CObject
        _mapMethodCollection = Basic::_mapMethodCollection;
        static PyMappingMethods methods = {nullptr, _mapGet, _mapSet};
        Type.tp_as_mapping = &methods;
        size_t index = 0;
        for (auto const&[key, methodDef]: _methodCollection) {
            (void) key;
            Type.tp_methods[index] = methodDef;
            ++index;
        }
        for (auto const&[key, methodDef]: _methodCollectionConst) {
            (void) key;
            Type.tp_methods[index] = methodDef;
            ++index;
        }
        _member_getters = Basic::_member_getters;
        _member_setters = Basic::_member_setters;

        Type.tp_getset = new PyGetSetDef[_member_getters.size() + 1];
        Type.tp_getset[_member_getters.size()] = {nullptr};
        index = 0;
        for (auto const&[key, getter]: Basic::_member_getters) {
            auto it = _member_setters.find(key);
            _setattrfunc setter = nullptr;
            if (not std::is_const<T>::value && it != _member_setters.end()) {
                setter = it->second;
            } else {
                setter = nullptr;
            }
            Type.tp_getset[index].name = key.c_str();
            Type.tp_getset[index].get = getter;
            Type.tp_getset[index].set = setter;
            Type.tp_getset[index].doc = "get/set attribute";
            Type.tp_getset[index].closure = nullptr;
            index++;
        }
        if constexpr (!std::is_const<T>::value) {
            _unaryOperators = Basic::_unaryOperators;
        }
        _unaryOperatorsConst = Basic::_unaryOperatorsConst;

        static std::map<std::string, unaryfunc *> unary_mapping =
                {{std::string(OP_UNARY_INV), &Type.tp_as_number->nb_invert},
                 {std::string(OP_UNARY_NEG), &Type.tp_as_number->nb_negative},
                 {std::string(OP_UNARY_POS), &Type.tp_as_number->nb_positive}};
        for (auto const&[name_, func]: _unaryOperators) {
            if (unary_mapping.count(name_) == 0) {
               return -1;
            }
            *unary_mapping[name_] = func;
        }
        for (auto const&[name_, func]: _unaryOperatorsConst) {
            if (unary_mapping.count(name_) == 0) {
                return -1;
            }
            *unary_mapping[name_] = func;
        }
        if constexpr (!std::is_const<T>::value) {
            _binaryOperators = Basic::_binaryOperators;
        }
        _binaryOperatorsConst = Basic::_binaryOperatorsConst;

        static std::map<std::string, binaryfunc *> binary_mapping =
                {{OP_BINARY_ADD,     &Type.tp_as_number->nb_add},
                 {OP_BINARY_AND,     &Type.tp_as_number->nb_and},
                 {OP_BINARY_OR,      &Type.tp_as_number->nb_or},
                 {OP_BINARY_XOR,     &Type.tp_as_number->nb_xor},
                 {OP_BINARY_DIV,     &Type.tp_as_number->nb_true_divide},
                 {OP_BINARY_MOD,     &Type.tp_as_number->nb_remainder},
                 {OP_BINARY_MUL,     &Type.tp_as_number->nb_multiply},
                 {OP_BINARY_LSHIFT,  &Type.tp_as_number->nb_lshift},
                 {OP_BINARY_RSHIFT,  &Type.tp_as_number->nb_rshift},
                 {OP_BINARY_SUB,     &Type.tp_as_number->nb_subtract},
                 {OP_BINARY_IADD,    &Type.tp_as_number->nb_inplace_add},
                 {OP_BINARY_IAND,    &Type.tp_as_number->nb_inplace_and},
                 {OP_BINARY_IOR,     &Type.tp_as_number->nb_inplace_or},
                 {OP_BINARY_IXOR,    &Type.tp_as_number->nb_inplace_xor},
                 {OP_BINARY_IDIV,    &Type.tp_as_number->nb_inplace_true_divide},
                 {OP_BINARY_IMOD,    &Type.tp_as_number->nb_inplace_remainder},
                 {OP_BINARY_IMUL,    &Type.tp_as_number->nb_inplace_multiply},
                 {OP_BINARY_ILSHIFT, &Type.tp_as_number->nb_inplace_lshift},
                 {OP_BINARY_IRSHIFT, &Type.tp_as_number->nb_inplace_rshift},
                 {OP_BINARY_ISUB,    &Type.tp_as_number->nb_inplace_subtract},

                };
        for (auto const&[name_, func]: _binaryOperators) {
            if (binary_mapping.count(name_) == 0) {
                throw PyllarsException(PyExc_SystemError, "Undefined operator name (internal error)");
            }
            *binary_mapping[name_] = func;
        }
        for (auto const&[name_, func]: _binaryOperatorsConst) {
            if (binary_mapping.count(name_) == 0) {
                throw PyllarsException(PyExc_SystemError, "Undefined operator name (internal error)");
            }
            *binary_mapping[name_] = func;
        }

        _baseClasses = Basic::_baseClasses;
        if (!Type.tp_base && _baseClasses.size() == 0) {
            if (PyType_Ready(&CommonBaseWrapper::_BaseType) < 0) {
                PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
                return -1;
            }
            Type.tp_base = &CommonBaseWrapper::_BaseType;
        }
        if (PyType_Ready(&Type) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
           return -1;
        }

        _classEnumValues = Basic::_classEnumValues;
        for (auto const&[name_, value]: _classEnumValues) {
            // can only be called after ready of Type:
            PyObject *pyval = toPyObject<const T_NoRef&>(*const_cast<const T_NoRef *>(value), 1);
            if (pyval) {
                PyDict_SetItemString(Type.tp_dict, name_.c_str(), pyval);
            } else {
                return -1;
            }
        }

        PyObject *const type = reinterpret_cast<PyObject *>(&Type);
        Py_INCREF(type);
        _isInitialized = inited;
        return 0;
    }


    template<typename T>
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type> *
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    fromCObject(T_NoRef &cobj) {
        static PyObject *kwds = PyDict_New();
        static PyObject *emptyargs = PyTuple_New(0);
        PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);
        PyTypeObject *type_ = getPyType();

        if (!type_ || !type_->tp_name) {
            PyErr_SetString(PyExc_RuntimeError, "Uninitialized type when creating object");
            return nullptr;
        }
        PythonClassWrapper *pyobj = (PythonClassWrapper *) PyObject_Call(
                reinterpret_cast<PyObject *>(type_), emptyargs, kwds);
        if (pyobj) {
            pyobj->_CObject = new T(cobj);//ObjectLifecycleHelpers::Copy<T>::new_copy(cobj);
        }
        return pyobj;
    }


    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addConstructorBase(const char *const kwlist[], constructor_t c) {
        _constructors.push_back(ConstructorContainer(kwlist, c));
    }

    template<typename T>
    template<typename ...Args>
    typename std::remove_reference<T>::type*
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    create(const char *const kwlist[], PyObject *args, PyObject *kwds,
           unsigned char *location) {
            return _createBase<Args...>(args, kwds, kwlist, typename argGenerator<sizeof...(Args)>::type(),
                                        (_____fake<Args> *) nullptr...);

    }

    template<typename T>
    PyObject *PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addr(PyObject *self, PyObject *args) {
        if ((args && PyTuple_Size(args) > 0)) {
            PyErr_BadArgument();
            return nullptr;
        }
        PythonClassWrapper<T_NoRef *>::initialize();
        PythonClassWrapper *self_ = reinterpret_cast<PythonClassWrapper *>(self);
        PyObject*obj;
        if(self_->_CObject){
            obj = toPyObject<T_NoRef *>(self_->_CObject, 1); // by reference? pointer -- so probably no need
            PyErr_Clear();
            (reinterpret_cast<PythonClassWrapper<T_NoRef *>*>(obj))->make_reference(self);
        } else {
            obj = Py_None;
        }
        return obj;
    }

    template<typename T>
    template<const char *const name, const char *const kwlist[], typename func_type, func_type *method>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addClassMethod() {
        static const char *const doc = "Call class method ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);

        PyMethodDef pyMeth = {
                name,
                (PyCFunction) StaticFunctionContainer<kwlist, func_type, method>::call,
                METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                doc_string
        };

        _addMethod<true>(pyMeth);
    }

   
    template<typename T>
    template<const char *const name, const char* const kwlist[], typename method_t, method_t method>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addMethod() {
        static const char *const doc = "Call method ";
        char *doc_string = new char[func_traits<method_t>::type_name().size() + strlen(doc) + 1];
        snprintf(doc_string, func_traits<method_t>::type_name().size() + strlen(doc) + 1, "%s%s", doc, func_traits<method_t>::type_name().c_str());
        PyMethodDef pyMeth = {
                name,
                (PyCFunction) MethodContainer<kwlist, method_t, method>::call,
                METH_KEYWORDS | METH_VARARGS,
                doc_string
        };

        _addMethod<func_traits<method_t>::is_const_method>(pyMeth);
    }


    template<typename T>
    template<const char* const name, typename ReturnType, ReturnType( core_type<T>::type::*method)()>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    Op<name, ReturnType( core_type<T>::type::*)(), method>::addUnaryOperator() {
        static const char* const kwlist[1] = {nullptr};
        _unaryOperators[name] = (unaryfunc) MethodContainer<kwlist, ReturnType(CClass::*)(), method>::callAsUnaryFunc;
    }


    template<typename T>
    template<const char* const name, typename ReturnType, ReturnType(core_type<T>::type::*method)() const>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    Op<name, ReturnType(core_type<T>::type::*)() const, method>::addUnaryOperator() {
        static const char* const kwlist[1] = {nullptr};
        _unaryOperatorsConst[name] = (unaryfunc) MethodContainer<kwlist, ReturnType(CClass::*)() const, method>::callAsUnaryFunc;
    }

    template<typename T>
    template<const char *const name, const char* const kwlist[2], typename ReturnType, typename ArgType,
            ReturnType(core_type<T>::type::*method)(ArgType)>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    BinaryOp<name, kwlist, ReturnType(core_type<T>::type::*)(ArgType), method>::addBinaryOperator() {
        _binaryOperators[name] = (binaryfunc) MethodContainer<kwlist,  ReturnType(core_type<T>::type::*)(ArgType), method>::callAsBinaryFunc;
    }



    template<typename T>
    template<const char *const name, const char* const kwlist[2], typename ReturnType, typename ArgType,
            ReturnType(core_type<T>::type::*method)(ArgType) const>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    BinaryOp<name, kwlist, ReturnType(core_type<T>::type::*)(ArgType) const, method>::addBinaryOperator() {
        _binaryOperatorsConst[name] = (binaryfunc) MethodContainer< kwlist,  ReturnType(core_type<T>::type::*)(ArgType) const,
           method>::callAsBinaryFunc;
    }

    template<typename T>
    PyObject *
    PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _mapGet(PyObject *self, PyObject *key) {
        PyObject *value = nullptr;
        for (auto const &[_, method]: _mapMethodCollection) {
            (void) _;
            if ((value = method.first(self, key))) {
                PyErr_Clear();
                break;
            }
        }
        return value;
    }


    template<typename T>
    int PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _mapSet(PyObject *self, PyObject *key, PyObject *value) {
        int status = -1;
        for (auto const &[_, method]: _mapMethodCollection) {
            (void) _;
            try {
                if ((status = method.second(std::is_const<T_NoRef>::value, self, key, value)) == 0) {
                    PyErr_Clear();
                    break;
                }
            } catch (PyllarsException &e){
                //just try the next one, as most likely an argumnet conversion exception thrown
            } catch(std::exception const & e) {
                PyllarsException::raise_internal_cpp(e.what());
                return -1;
            } catch (...){
                PyllarsException::raise_internal_cpp();
                status = -1;
                break;
            }
        }
        return status;
    }



#include <functional>

    template<typename T>
    template<const char *const kwlist[2],  typename KeyType, typename ValueType, typename method_t, method_t method>
    void PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    _addMapOperatorMethod() {
        constexpr bool method_is_const = func_traits<method_t>::is_const_method;
        std::function<PyObject *(PyObject *, PyObject *)> getter = [](PyObject *self, PyObject *item) -> PyObject * {
            PythonClassWrapper *self_ = (PythonClassWrapper *) self;
            try {
                auto c_key = __pyllars_internal::toCArgument<KeyType>(*item);
                return toPyObject<ValueType>((self_->get_CObject()->*method)(c_key.value()), 1);
            } catch (PyllarsException &e) {
                e.raise();
                return nullptr;
            } catch(std::exception const & e) {
                PyllarsException::raise_internal_cpp(e.what());
                return nullptr;
            } catch (...){
                PyllarsException::raise_internal_cpp();
                return nullptr;
            }
        };
        // since elements can be mutable, even const map operators must allow for setters
        std::function<int(bool, PyObject *, PyObject *, PyObject *)> setter =
                [](bool obj_is_const, PyObject *self, PyObject *item, PyObject *value) -> int {
            PythonClassWrapper *self_ = (PythonClassWrapper *) self;
            auto cobj = self_->get_CObject();
            if (!cobj){
                PyErr_SetString(PyExc_TypeError, "Cannot operate on nullptr");
                return -1;
            }
            try {
                if constexpr (std::is_reference<ValueType>::value) {
                    //the value here is something we will be assigning TO and NOT FROM.  So make const
                    //in order to avoid type conversion issue as is it not really an argument to a function call
                    typedef typename to_const<ValueType>::type safe_value_type;
                    argument_capture<safe_value_type > c_value = __pyllars_internal::template toCArgument<safe_value_type >(*value);
                    argument_capture<KeyType> c_key = __pyllars_internal::template toCArgument<KeyType>(*item);
                    if (obj_is_const){
                        if constexpr(method_is_const) {
                            //mutable fields are still settable against const-ness of owning object
                            //NOTE: we re-use this std::function for PythonClassWrapper<const T>, so need
                            //   to get const bool to determine if this really should be a const-C object
                            auto const_cobj = reinterpret_cast<const T_NoRef *>(cobj);
                            try {
                                Assignment<ValueType>::assign((const_cobj->*method)(c_key.value()), c_value.value());
                            } catch (PyllarsException &e) {
                                e.raise();
                                return -1;
                            } catch(std::exception const & e) {
                                PyllarsException::raise_internal_cpp(e.what());
                                return -1;
                            } catch (...){
                                PyllarsException::raise_internal_cpp();
                                return -1;
                            }
                        } else {

                            PyErr_SetString(PyExc_TypeError, "Cannot call non-const method with const this");
                            return -1;
                        }
                    } else {
                        try{
                            Assignment<ValueType>::assign((cobj->*method)(c_key.value()), c_value.value());
                        } catch (PyllarsException &e){
                            e.raise();
                            return -1;
                        } catch(std::exception const & e) {
                            PyllarsException::raise_internal_cpp(e.what());
                            return -1;
                        } catch (...){
                            PyllarsException::raise_internal_cpp();
                            return -1;
                        }
                    }
                } else {
                    PyErr_SetString(PyExc_TypeError, "Cannot set non-reference returned item");
                    return -1;
                }
            } catch (PyllarsException &e) {
                e.raise();
                return -1;
            } catch(std::exception const & e) {
                PyllarsException::raise_internal_cpp(e.what());
                return -1;
            } catch (...){
                PyllarsException::raise_internal_cpp();
                return -1;
            }
            return 0;
        };

        const std::string name = type_name<ValueType>() + std::string(":") + type_name<KeyType>();
        _mapMethodCollection[name] = std::pair<std::function<PyObject *(PyObject *, PyObject *)>,
                std::function<int(bool, PyObject *, PyObject *, PyObject *)>
        >(getter, setter);

    }


    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addClassMember(const char *const name, PyObject *pyobj) {
        if (!_Type.tp_dict) {
            _Type.tp_dict = PyDict_New();
        }
        PyDict_SetItemString(_Type.tp_dict, name, pyobj);
    }

    template<typename T>
    template<typename Base>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addBaseClass() {
        PyTypeObject * base = PythonClassWrapper<Base>::getPyType();
        PyTypeObject * const_base = PythonClassWrapper<const Base>::getPyType();
        if (!base) return;
        _baseClasses.insert(_baseClasses.begin(), base);
        _baseClassesConst.insert(_baseClassesConst.begin(), const_base);
    }

    template<typename T>
    bool PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    checkType(PyObject * obj) {
        return PyObject_TypeCheck(obj, &_Type);
    }

    template<typename T>
    PyObject *PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    alloc(PyObject *cls, PyObject *args, PyObject *kwds) {
        /**
         * Takes no kwds
         * args is of length 1
         * if arg is int value, allocate array of that size based on default constructor (or raise exc if no such constructor)
         * If arg is a tuple, use that as the arguments to a constructor for allocation of single item
         * If arg is a list of tuples, cosntrcut an array of that size of objects using each tuple as cosntructor args
         *  (allocation will be through generic byte buffer and then in-place-memory allocation)
         */
        if (kwds && PyDict_Size(kwds) != 0) {
            throw PyllarsException(PyExc_ValueError, "new takes not key words");
        }
        if (PyTuple_Size(args) != 1) {
            throw PyllarsException(PyExc_TypeError, "new takes only one arg (and int, a tuple, or list of tuples)");
        }
        auto arg = PyTuple_GetItem(args, 0);
        if (PyLong_Check(arg)) {
            const ssize_t size = PyLong_AsLong(arg);
            if (size < 0){
                throw PyllarsException(PyExc_ValueError, "Size cannot be negative");
            }
            return (PyObject *) PythonClassWrapper<T_NoRef *>::template allocateArray<>(size);
        } else if (PyTuple_Check(arg)) {
            auto list = PyList_New(1);
            PyList_SetItem(list, 0, arg);
            auto new_arg = PyTuple_New(1);
            PyTuple_SetItem(new_arg, 0, list);
            auto obj = alloc(cls, new_arg, kwds);
            Py_INCREF(list); // new_arg selfilshly grabs reference in SetItem call
            Py_DECREF(new_arg);
            return obj;
        } else if (PyList_Check(arg)) {

            if constexpr(!is_complete<T_NoRef>::value){
                throw PyllarsException(PyExc_ValueError, "Cannot allocate multiple disparate instances using incomplete type");
            } else {
                const ssize_t size = PyList_Size(arg);
                if (size < 0) {
                    throw PyllarsException(PyExc_SystemError, "Internal error getting size of list");
                }
                typedef T_NoRef  T_fixed_array[size];
                typedef T_fixed_array *T_fixed_array_ptr;

                auto bytebucket = new unsigned char[size * sizeof(T_NoRef)];
                T_fixed_array_ptr cobj_ptr = (T_fixed_array_ptr) bytebucket;
                for (ssize_t i = 0; i < size; ++i) {
                    PyObject *constructor_args = PyList_GetItem(arg, i);
                    if (!constructor_args || !PyTuple_Check(constructor_args)) {
                        PyErr_SetString(PyExc_TypeError,
                                        "Invalid constructor arguments: not a tuple as expected, or index out of range");
                        return nullptr;
                    }
                    T_NoRef *cobj = nullptr;
                    for (auto const &[kwlist_, constructor_] : PythonClassWrapper<T>::_constructors) {
                        try {
                            cobj = constructor_(kwlist_, constructor_args, nullptr, bytebucket + i);
                            if (cobj) break;
                        } catch (PyllarsException &e) {
                            //try next one
                        } catch(std::exception const & e) {
                            PyllarsException::raise_internal_cpp(e.what());
                            return nullptr;
                        } catch (...){
                            PyllarsException::raise_internal_cpp();
                            return nullptr;
                        }
                        PyErr_Clear();
                    }
                    if (!cobj) {
                        if constexpr (std::is_destructible<T_NoRef>::value) {
                            for (ssize_t j = 0; j < i; ++j) {
                                (*cobj_ptr)[j].~T_NoRef();
                            }
                        }
                        PyErr_SetString(PyExc_TypeError, "Invalid constructor argsument");
                        return nullptr;
                    }
                }
                auto pyobj = PythonClassWrapper<T_NoRef *>::fromInPlaceAllocation(size, bytebucket);
                return (PyObject *) pyobj;
            }
        }
        throw PyllarsException(PyExc_ValueError, "Invalid constructor arguments");
    }

    template<typename T>
    int PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds) {

        self->compare = [](CommonBaseWrapper* self_, CommonBaseWrapper* other)->bool{
            if constexpr (has_operator_compare<T&, T>::value) {
                return PyObject_TypeCheck(other, getPyType()) &&
                       (*reinterpret_cast<PythonClassWrapper *>(self_)->get_CObject() ==
                        *reinterpret_cast<PythonClassWrapper *>(other)->get_CObject());
            } else {
                return false;
            }
        };

        self->hash = [](CommonBaseWrapper* self)->size_t{
            static std::hash<typename std::remove_cv<T>::type*> hasher;
            return hasher(const_cast<typename std::remove_cv<T>::type*>(((PythonClassWrapper*)self)->get_CObject()));
        };

        if (_Type.tp_base && Base::TypePtr->tp_init) {
            PyObject *empty = PyTuple_New(0);
            Base::TypePtr->tp_init((PyObject *) &self->baseClass, empty, nullptr);
            PyErr_Clear();
            Py_DECREF(empty);
        }
        self->_referenced = nullptr;
        PyTypeObject *const coreTypePtr = PythonClassWrapper<typename core_type<T>::type>::getPyType();
        self->template populate_type_info<T>(&checkType, coreTypePtr);
        if (!_member_getters.count("this")) {
            _member_getters["this"] = getThis;
        }
        return InitHelper<T>::init(self, args, kwds);
    }

    template<typename T>
    PyObject *PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        (void) args;
        (void) kwds;
        PythonClassWrapper *self;
        self = (PythonClassWrapper *) type->tp_alloc(type, 0);
        if (nullptr != self) {
            self->_CObject = nullptr;
        }
        return (PyObject *) self;
    }

    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _free(void *self_) {
        PythonClassWrapper *self = (PythonClassWrapper *) self_;
        if (!self->_CObject) return;
        if constexpr (std::is_destructible<T>::value) {
            delete self->_CObject;
        }
        self->_CObject = nullptr;
    }

    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _dealloc(PyObject *self_) {
        PythonClassWrapper* self = reinterpret_cast<PythonClassWrapper*>(self_);
        if (!self) return;
        if (self->_referenced) {
            Py_DECREF(self->_referenced);
            self->_referenced = nullptr;
        }
        _free(self);
    }


    template<typename T>
    template<typename ...PyO>
    bool PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _parsePyArgs(const char *const kwlist[], PyObject *args, PyObject *kwds, PyO *&...pyargs) {
        char format[sizeof...(PyO) + 1] = {0};
        if (sizeof...(PyO) > 0)
            memset(format, 'O', sizeof...(PyO));
        return (args && !kwds && sizeof...(PyO) == 0) || PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...);
    }

    template<typename T>
    template<typename ...Args>
    typename std::remove_reference<T>::type *
    PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _createBaseBase(argument_capture<Args> ... args) {
        return new T_NoRef(std::forward<typename extent_as_pointer<Args>::type>(args.value())...);
    }


    template<typename T>
    template<typename ...Args, int ...S>
    typename std::remove_reference<T>::type*
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    _createBase(PyObject *args, PyObject *kwds,
                const char *const kwlist[], container<S...>, _____fake<Args> *...) {
        if (args && PyTuple_Size(args) != sizeof...(Args)) {
            return nullptr;
        }
        PyObject *pyobjs[sizeof...(Args) + 1];
        (void) pyobjs;
        if (!_parsePyArgs(kwlist, args, kwds, pyobjs[S]...)) {
            PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
            return nullptr;
        }

        return _createBaseBase<Args...>(__pyllars_internal::toCArgument<Args>(*pyobjs[S])...);
    }


    template<typename T>
    template<bool is_const>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _addMethod(PyMethodDef method) {
        //insert at beginning to keep null sentinel at end of list:
        if constexpr(is_const){
            _methodCollectionConst[method.ml_name] = method;
        } else {
            _methodCollection[method.ml_name] = method;
        }
    }

    template<typename T>
    template<const char *const name, typename FieldType, const size_t bits>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addBitField(
            typename BitFieldContainer<typename std::remove_reference<T>::type>::template Container<name, FieldType, bits>::getter_t &getter,
            typename BitFieldContainer<typename std::remove_reference<T>::type>::template Container<name, FieldType, bits>::setter_t *setter) {
        static const char *const doc = "Get bit-field attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::_getter = getter;
        _member_getters[name] = BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::get;
        if (setter) {
            BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::_setter = *setter;
            _member_setters[name] = BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::set;
        }
    }

    template<typename T>
    template<const char *const name, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addAttribute(typename MemberContainer<name, T_NoRef, FieldType>::member_t member) {

        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        const ssize_t array_size = ArraySize<FieldType>::size;
        MemberContainer<name, T_NoRef, FieldType>::member = member;
        MemberContainer<name, T_NoRef, FieldType>::array_size = array_size;
        _member_getters[name] = MemberContainer<name, T_NoRef, FieldType>::get;
        if constexpr (!std::is_const<FieldType>::value) {
            _member_setters[name] = MemberContainer<name, T_NoRef, FieldType>::set;
        }
    }

    template<typename T>
    template<const char *const name, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addClassAttribute(FieldType *member) {

        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        ClassMember<name, T_NoRef, FieldType>::member = member;
        PyMethodDef pyMeth = {name,
                              (PyCFunction) ClassMember<name, T_NoRef, FieldType>::call,
                              METH_VARARGS | METH_KEYWORDS | METH_CLASS,
                              doc_string
        };
        _addMethod<true>(pyMeth);
    }

    template <typename  T>
    typename std::remove_const<T>::type &
    PythonClassWrapper<T,typename std::enable_if<is_rich_class<T>::value>::type>::
    toCArgument(){
        if constexpr (std::is_const<T>::value){
            throw PyllarsException(PyExc_TypeError, "Invalid conversion from non const reference to const reference");
        } else {
            return *get_CObject();
        }
    }


    template <typename  T>
    const T&
    PythonClassWrapper<T,typename std::enable_if<is_rich_class<T>::value>::type>::
    toCArgument() const{
        return *get_CObject();
    }

}

#endif