#ifndef PYLLARS__CLASSWRAPPER_IMPL
#define PYLLARS__CLASSWRAPPER_IMPL

#include <limits>
#include <string.h>
#include <utility>

#include "pyllars_classwrapper.hpp"
#include "pyllars_utils.hpp"

#include "pyllars_classmethodsemantics.impl.hpp"
#include "pyllars_classmembersemantics.impl.hpp"
#include "pyllars_methodcallsemantics.impl.hpp"
#include "pyllars_object_lifecycle.impl.hpp"
#include "pyllars_conversions.impl.hpp"


namespace __pyllars_internal {

    //static PyMethodDef emptyMethods[] = {{nullptr, nullptr, 0, nullptr}};

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
    std::vector<PyTypeObject *> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _baseClasses = std::vector<PyTypeObject *>();

    template<typename T>
    std::map<std::string, const typename std::remove_reference<T>::type *> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _classEnumValues = std::map<std::string, const T_NoRef *>();

    template<typename T>
    std::map<std::string, unaryfunc> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _unaryOperators = std::map<std::string, unaryfunc>();

    template<typename T>
    std::map<std::string, binaryfunc> PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
            _binaryOperators = std::map<std::string, binaryfunc>();

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
            std::function<int(PyObject *, PyObject *, PyObject *)>
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
                    nullptr,             /*tp_name*/ /*filled on init*/
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
        typedef typename std::remove_reference<T>::type T_NoRef;
        if (!self) {
            return -1;
        }
        static const char *kwlist[] = {"value", nullptr};
        self->_CObject = nullptr;
            for (auto const &[kwlist_, constructor] : PythonClassWrapper<T>::_constructors) {
                try {
                    if ((self->_CObject = constructor(kwlist_, args, kwds, nullptr))) {
                        self->_isInitialized = true;
                        return 0;
                    }
                } catch (...) {
                }
                PyErr_Clear();
            }
            if ((!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
                if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
                    PyErr_Clear();
                } else {
                    PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                    return -1;
                }
            } else {
                PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
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
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::get_CObject() {
        return _CObject ? _CObject->ptr() : nullptr;
    }

    template<typename T>
    int PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    initialize() {
        typedef typename std::remove_volatile<
                typename std::remove_const<
                        typename std::remove_reference<T>::type
                >::type
        >::type basic_type;
        typedef PythonClassWrapper<basic_type> Basic;
        static bool inited = false;
        if (inited) return 0;
        _isInitialized = false;
        inited = true;
        if constexpr (std::is_const<T_NoRef >::value){
            for ( auto & element : Basic::_constructors){
                _constructors.push_back(*reinterpret_cast<ConstructorContainer*>(&element));
            }
        } else {
            _constructors = Basic::_constructors;
        }
        const char *const name = type_name<T>();
        if (!name || strlen(name) == 0) return -1;
        int status = 0;
        if (_Type.tp_name) {/*already initialized*/ return status; }
        char *tp_name = new char[strlen(name) + 1 + tp_name_prefix_len];
        strcpy(tp_name, tp_name_prefix);
        strcpy(tp_name + strlen(tp_name_prefix), name);
        _Type.tp_name = tp_name;

        PyMethodDef pyMethAlloc = {
                alloc_name_,
                (PyCFunction) alloc,
                METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                "allocate array of single dynamic instance of this class"
        };
        _methodCollection = Basic::_methodCollection; // make this same as type with no garnishment
        _methodCollection[alloc_name_] = pyMethAlloc;
        if (!Basic::_baseClasses.empty()) {
            _Type.tp_bases = PyTuple_New(Basic::_baseClasses.size());
            Py_ssize_t index = 0;
            for (auto const &baseClass: Basic::_baseClasses) {
                PyTuple_SetItem(_Type.tp_bases, index, (PyObject *) baseClass);
                // tp_bases not usable for inheritance of methods/membser as it doesn't really do the right thing and
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
                index++;
            }
        }
        _Type.tp_methods = new PyMethodDef[_methodCollection.size() + 1];
        _Type.tp_methods[_methodCollection.size()] = {nullptr};
        size_t index = 0;
        for (auto const&[key, methodDef]: _methodCollection) {
            (void) key;
            _Type.tp_methods[index] = methodDef;
            ++index;
        }
        _member_getters = Basic::_member_getters;
        _member_setters = Basic::_member_setters;

        _Type.tp_getset = new PyGetSetDef[_member_getters.size() + 1];
        _Type.tp_getset[_member_getters.size()] = {nullptr};
        index = 0;
        for (auto const&[key, getter]: Basic::_member_getters) {
            auto it = _member_setters.find(key);
            _setattrfunc setter = nullptr;
            if (not std::is_const<T>::value && it != _member_setters.end()) {
                setter = it->second;
            } else {
                setter = nullptr;
            }
            _Type.tp_getset[index].name = key.c_str();
            _Type.tp_getset[index].get = getter;
            _Type.tp_getset[index].set = setter;
            _Type.tp_getset[index].doc = "get/set attribute";
            _Type.tp_getset[index].closure = nullptr;
            index++;
        }

        _unaryOperators = Basic::_unaryOperators;
        for (auto const&[name_, func]: _unaryOperators) {
            static std::map<std::string, unaryfunc *> unary_mapping =
                    {{std::string(OP_UNARY_INV), &_Type.tp_as_number->nb_invert},
                     {std::string(OP_UNARY_NEG), &_Type.tp_as_number->nb_negative},
                     {std::string(OP_UNARY_POS), &_Type.tp_as_number->nb_positive}};

            if (unary_mapping.count(name_) == 0) {
               return -1;
            }
            *unary_mapping[name_] = func;
        }

        _binaryOperators = Basic::_binaryOperators;
        for (auto const&[name_, func]: _binaryOperators) {
            static std::map<std::string, binaryfunc *> binary_mapping =
                    {{OP_BINARY_ADD,     &_Type.tp_as_number->nb_add},
                     {OP_BINARY_AND,     &_Type.tp_as_number->nb_and},
                     {OP_BINARY_OR,      &_Type.tp_as_number->nb_or},
                     {OP_BINARY_XOR,     &_Type.tp_as_number->nb_xor},
                     {OP_BINARY_DIV,     &_Type.tp_as_number->nb_true_divide},
                     {OP_BINARY_MOD,     &_Type.tp_as_number->nb_remainder},
                     {OP_BINARY_MUL,     &_Type.tp_as_number->nb_multiply},
                     {OP_BINARY_LSHIFT,  &_Type.tp_as_number->nb_lshift},
                     {OP_BINARY_RSHIFT,  &_Type.tp_as_number->nb_rshift},
                     {OP_BINARY_SUB,     &_Type.tp_as_number->nb_subtract},
                     {OP_BINARY_IADD,    &_Type.tp_as_number->nb_inplace_add},
                     {OP_BINARY_IAND,    &_Type.tp_as_number->nb_inplace_and},
                     {OP_BINARY_IOR,     &_Type.tp_as_number->nb_inplace_or},
                     {OP_BINARY_IXOR,    &_Type.tp_as_number->nb_inplace_xor},
                     {OP_BINARY_IDIV,    &_Type.tp_as_number->nb_inplace_true_divide},
                     {OP_BINARY_IMOD,    &_Type.tp_as_number->nb_inplace_remainder},
                     {OP_BINARY_IMUL,    &_Type.tp_as_number->nb_inplace_multiply},
                     {OP_BINARY_ILSHIFT, &_Type.tp_as_number->nb_inplace_lshift},
                     {OP_BINARY_IRSHIFT, &_Type.tp_as_number->nb_inplace_rshift},
                     {OP_BINARY_ISUB,    &_Type.tp_as_number->nb_inplace_subtract},

                    };
            if (binary_mapping.count(name_) == 0) {
                throw "Undefined operator name (internal error)";
            }
            *binary_mapping[name_] = func;
        }

        _baseClasses = Basic::_baseClasses;
        if (!_Type.tp_base && _baseClasses.size() == 0) {
            if (PyType_Ready(&CommonBaseWrapper::_BaseType) < 0) {
                PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
                PyErr_Print();
                return -1;
            }
            _Type.tp_base = &CommonBaseWrapper::_BaseType;
        }
        if (PyType_Ready(&_Type) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
            PyErr_Print();
           return -1;
        }

        _classEnumValues = Basic::_classEnumValues;
        for (auto const&[name_, value]: _classEnumValues) {
            // can only be called after ready of _Type:
            PyObject *pyval = toPyObject<T>(*const_cast<T_NoRef *>(value), 1);
            if (pyval) {
                PyDict_SetItemString(_Type.tp_dict, name_.c_str(), pyval);
            } else {
                return -1;
            }
        }

        if(status == 0){
            PyObject *const type = reinterpret_cast<PyObject *>(&_Type);
            Py_INCREF(type);
        }
        _isInitialized = (status == 0);
        return status;
    }


    template<typename T>
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type> *
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    createPyReference(T_NoRef &cobj, PyObject *referencing) {
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
            pyobj->_CObject = new ObjectContainerReference<T_NoRef>(cobj);
            if (referencing) pyobj->make_reference(referencing);
        }
        return pyobj;
    }


    template<typename T>
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type> *
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    createPyFromAllocated(T_NoRef *cobj, PyObject *referencing) {
        static PyObject *kwds = PyDict_New();
        static PyObject *emptyargs = PyTuple_New(0);

        PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);
        PyTypeObject *type_ = getPyType();

        if (!type_->tp_name) {
            PyErr_SetString(PyExc_RuntimeError, "Uninitialized type when creating object");
            return nullptr;
        }
        PythonClassWrapper *pyobj = (PythonClassWrapper *) PyObject_Call(
                reinterpret_cast<PyObject *>(type_), emptyargs, kwds);
        if (pyobj) {
            pyobj->_CObject = ObjectContainerAllocatedInstance<T_NoRef>::new_container(cobj);
            if (referencing) pyobj->make_reference(referencing);
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
    ObjectContainer<T> *
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    create(const char *const kwlist[], PyObject *args, PyObject *kwds,
           unsigned char *location) {
        try {
            return _createBase<Args...>(args, kwds, kwlist, typename argGenerator<sizeof...(Args)>::type(),
                                        (_____fake<Args> *) nullptr...);

        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            return nullptr;
        }
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
            obj = toPyObject<T_NoRef *>(self_->_CObject->ptr(), 1); // by reference? pointer -- so probably no need
            PyErr_Clear();
            (reinterpret_cast<PythonClassWrapper<T_NoRef *>*>(obj))->make_reference(self);
        } else {
            obj = Py_None;
        }
        return obj;
    }

    template<typename T>
    template<const char *const name, const char *const kwlist[], typename ReturnType, typename ...Args>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addClassMethod(ReturnType(*method)(Args...)) {
        static const char *const doc = "Call class method ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);

        PyMethodDef pyMeth = {
                name,
                (PyCFunction) ClassMethodContainer<T_NoRef>::template Container<false, name, kwlist, ReturnType, Args...>::call,
                METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                doc_string
        };

        ClassMethodContainer<T>::template Container<false, name, kwlist, ReturnType, Args...>::method = method;
        _addMethod(pyMeth);
    }

    template<typename T>
    template<const char *const name, const char *const kwlist[], typename ReturnType, typename ...Args>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addClassMethodVarargs(ReturnType(*method)(Args... ...)) {
        static const char *const doc = "Call class method ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);

        PyMethodDef pyMeth = {
                name,
                (PyCFunction) ClassMethodContainer<T_NoRef>::template Container<true, name, kwlist, ReturnType, Args...>::call,
                METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                doc_string
        };

        ClassMethodContainer<T>::template Container<true, name, ReturnType, Args...>::method = method;
        _addMethod(pyMeth);
    }


    template<typename T>
    template<typename _Container, bool is_const, const char *const kwlist[], typename ReturnType, typename ...Args>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addMethodTempl(typename _Container::template Container<is_const, kwlist, ReturnType, Args...>::method_t method) {
        static const char *const doc = "Call method ";
        char *doc_string = new char[strlen(_Container::name_) + strlen(doc) + 1];
        snprintf(doc_string, strlen(_Container::name_) + strlen(doc) + 1, "%s%s", doc, _Container::name_);

        PyMethodDef pyMeth = {
                _Container::name_,
                (PyCFunction) _Container::call,
                METH_KEYWORDS | METH_VARARGS,
                doc_string
        };

        _Container::template Container<is_const, kwlist, ReturnType, Args...>::setMethod(method);
        _addMethod(pyMeth);
    }


    template<typename T>
    template<const char *const name, bool is_const, typename Arg>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addUnaryOperator(
            typename MethodContainer<T_NoRef, name>::template Container<is_const, emptylist, Arg>::method_t method) {
        MethodContainer<T_NoRef, name>::template Container<is_const, emptylist, Arg>::setMethod(method);
        _unaryOperators[name] = (unaryfunc) MethodContainer<T_NoRef, name>::callAsUnaryFunc;
    }


    template<typename T>
    template<const char *const name, bool is_const, const char *const kwlist[2], typename ReturnType, typename Arg>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addBinaryOperator(
            typename MethodContainer<T_NoRef, name>::template Container<is_const, kwlist, ReturnType, Arg>::method_t method) {
        MethodContainer<T_NoRef, name>::template Container<is_const, kwlist, ReturnType, Arg>::setMethod(method);
        _binaryOperators[name] = (binaryfunc) MethodContainer<T_NoRef, name>::callAsBinaryFunc;
    }

    template<typename T>
    PyObject *
    PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _mapGet(PyObject *self, PyObject *key) {
        PyObject *value = nullptr;
        for (auto const &[_, method]: _mapMethodCollection) {
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
            if ((status = method.second(self, key, value)) == 0) {
                PyErr_Clear();
                break;
            }
        }
        return status;
    }

    namespace {


        template<typename K, typename V>
        class Name {
        public:
            static const std::string name;
        };

        template<typename K, typename V>
        const std::string
                Name<K, V>::name = std::string(typeid(K).name()) + std::string(typeid(V).name());
    }


#include <functional>

    template<typename T>
    template<const char *const kwlist[], typename KeyType, typename ValueType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addMapOperatorMethod(
            typename MethodContainer<T_NoRef, operatormapname>::template Container<false, kwlist, ValueType, KeyType>::method_t method) {
        std::function<PyObject *(PyObject *, PyObject *)> getter = [method](PyObject *self,
                                                                            PyObject *item) -> PyObject * {
            PythonClassWrapper *self_ = (PythonClassWrapper *) self;
            try {
                auto c_key = toCArgument<KeyType>(*item);
                return toPyObject((self_->get_CObject()->*method)(c_key.value()), 1);
            } catch (const char *const msg) {
                PyErr_SetString(PyExc_TypeError, msg);
                return nullptr;
            }
        };
        std::function<int(PyObject *, PyObject *, PyObject *)> setter = [method](PyObject *self, PyObject *item,
                                                                                 PyObject *value) -> int {
            PythonClassWrapper *self_ = (PythonClassWrapper *) self;
            try {
                if constexpr (!std::is_const<T>::value && std::is_reference<ValueType>::value) {
                    auto c_value = toCArgument<ValueType>(*value);
                    auto c_key = toCArgument<KeyType>(*item);
                    Assignment<ValueType>::assign((self_->get_CObject()->*method)(c_key.value()), c_value.value());
                } else {
                    PyErr_SetString(PyExc_TypeError, "Cannot set const or non-reference returned item");
                    return -1;
                }
            } catch (const char *const msg) {
                PyErr_SetString(PyExc_TypeError, "Cannot assign to value of unrelated type.");
                return -1;
            }
            return 0;
        };

        const std::string name = Name<ValueType, KeyType>::name;
        _mapMethodCollection[name] = std::pair<std::function<PyObject *(PyObject *, PyObject *)>,
                std::function<int(PyObject *, PyObject *, PyObject *)>
        >(getter, setter);

        static PyMappingMethods methods = {nullptr, _mapGet, _mapSet};
        _Type.tp_as_mapping = &methods;
    }

    template<typename T>
    template<const char *const kwlist[], typename KeyType, typename ValueType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addMapOperatorMethodConst(
            typename MethodContainer<T_NoRef, operatormapname>::template Container<true, kwlist, ValueType, KeyType>::method_t method) {
        std::function<PyObject *(PyObject *, PyObject *)> getter = [method](PyObject *self,
                                                                            PyObject *item) -> PyObject * {
            PythonClassWrapper *self_ = (PythonClassWrapper *) self;
            try {
                auto c_key = toCArgument<KeyType>(*item);
                return toPyObject((self_->get_CObject()->*method)(c_key.value()), 1);
            } catch (const char *const msg) {
                PyErr_SetString(PyExc_TypeError, msg);
                return nullptr;
            }
        };

        std::function<int(PyObject *, PyObject *, PyObject *)> setter = [method](PyObject *self, PyObject *item,
                                                                                 PyObject *value) -> int {
            PyErr_SetString(PyExc_TypeError, "Unable to set value of const mapping");
            return 1;
        };

        const std::string name = Name<ValueType, KeyType>::name;
        //do not override a non-const with const version of operator[]
        if (!_mapMethodCollection.count(name)) {
            _mapMethodCollection[name] = std::pair<std::function<PyObject *(PyObject *, PyObject *)>,
                    std::function<int(PyObject *, PyObject *, PyObject *)>
            >(getter, setter);
        }
        static PyMappingMethods methods = {nullptr, _mapGet, _mapSet};
        _Type.tp_as_mapping = &methods;
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
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addBaseClass(PyTypeObject *base) {
        if (!base) return;
        if (!_Type.tp_base && _baseClasses.empty()) {
            _Type.tp_base = base;
        } else {
            if (_Type.tp_base) {
                _baseClasses.push_back(_Type.tp_base);
                _Type.tp_base = nullptr;
            }
            _baseClasses.insert(_baseClasses.begin(), base);
        }
    }

    template<typename T>
    bool PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    checkType(PyObject *const obj) {
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
            throw "new takes not key words";
        }
        if (PyTuple_Size(args) != 1) {
            throw "new takes only one arg (and int, a tuple, or list of tuples)";
        }
        auto arg = PyTuple_GetItem(args, 0);
        if (PyLong_Check(arg)) {
            const ssize_t size = PyLong_AsLong(arg);
            if (size < 0){
                throw "Size cannot be negative";
            }
            T_NoRef *values = Constructor<T_NoRef >::allocate_array((size_t)size);
            return (PyObject *) PythonClassWrapper<T_NoRef *>::createPyFromAllocatedInstance(values, size);
        } else if (PyTuple_Check(arg)) {
            auto list = PyList_New(1);
            PyList_SetItem(list, 0, arg);
            auto new_arg = PyTuple_New(1);
            PyTuple_SetItem(new_arg, 0, list);
            auto obj = alloc(cls, new_arg, kwds);
            Py_DECREF(new_arg);
            return obj;
        } else if (PyList_Check(arg)) {
            const ssize_t size = PyList_Size(arg);
            if(size < 0){
                throw "Internal error getting size of list";
            }
            std::function<void(void*, size_t)> constructor = [arg, kwds](void * location, size_t index){
                ObjectContainer<T_NoRef > *cobj = nullptr;
                PyObject* constructor_args = PyList_GetItem(arg, index);
                if (!constructor_args || !PyTuple_Check(constructor_args)){
                    throw "Invalid constructor arguments: not a tuple as expected, or index out of range";
                }
                for (auto const &[kwlist_, constructor_] : PythonClassWrapper<T>::_constructors) {
                    try {
                        cobj = constructor_(kwlist_, constructor_args, nullptr, (unsigned char*)location);
                        if (cobj) break;
                    } catch (...) {
                    }
                    PyErr_Clear();
                }
                if (!cobj) {
                    throw  "No matching constructor";
                }
            };
            return (PyObject *) PythonClassWrapper<T_NoRef *>::createPyUsingBytePool((size_t) size, constructor);
        }
        throw "Invalid constructor arguments";
    }

    template<typename T>
    int PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
        if (_Type.tp_base && Base::TypePtr->tp_init) {
            PyObject *empty = PyTuple_New(0);
            Base::TypePtr->tp_init((PyObject *) &self->baseClass, empty, nullptr);
            PyErr_Clear();
            Py_DECREF(empty);
        }
        self->_referenced = nullptr;
        PyTypeObject *const coreTypePtr = PythonClassWrapper<typename core_type<T>::type>::getPyType();
        self->populate_type_info<T>(&checkType, coreTypePtr);
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
        delete self->_CObject;
        self->_CObject = nullptr;
    }

    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _dealloc(PythonClassWrapper *self) {
        if (!self) return;
        if (self->_referenced) {
            Py_XDECREF(self->_referenced);
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
        return sizeof...(PyO) == 0 || PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...);
    }

    template<typename T>
    template<typename ...Args>
    ObjectContainer<T> *
    PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _createBaseBase(argument_capture<Args> ... args) {
        return new ObjectContainerConstructed<T, Args...>(
                std::forward<typename extent_as_pointer<Args>::type>(args.value())...);
    }


    template<typename T>
    template<typename ...Args, int ...S>
    ObjectContainer<T> *
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

        return _createBaseBase<Args...>(toCArgument<Args>(*pyobjs[S])...);
    }


    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _addMethod(PyMethodDef method) {
        //insert at beginning to keep null sentinel at end of list:
        _methodCollection[method.ml_name] = method;
    }

    template<typename T>
    template<const char *const name, typename FieldType, const size_t bits>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addBitField(
            typename BitFieldContainer<typename std::remove_reference<T>::type>::template Container<name, FieldType, bits>::getter_t &getter,
            typename BitFieldContainer<typename std::remove_reference<T>::type>::template Container<name, FieldType, bits>::setter_t &setter) {
        static const char *const doc = "Get bit-field attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::_getter = getter;
        BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::_setter = setter;
        _member_getters[name] = BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::get;
        _member_setters[name] = BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::set;

    }

    template<typename T>
    template<const char *const name, typename FieldType, const size_t bits>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addBitFieldConst(
            typename BitFieldContainer<T_NoRef>::template Container<name, FieldType, bits>::getter_t &getter) {
        static const char *const doc = "Get bit-field attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        BitFieldContainer<T_NoRef>::template ConstContainer<name, FieldType, bits>::_getter = getter;
        _member_getters[name] = BitFieldContainer<T_NoRef>::template ConstContainer<name, FieldType, bits>::get;
    }


    template<typename T>
    template<const char *const name, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addAttribute(typename MemberContainer<T_NoRef>::template Container<name, FieldType>::member_t member) {

        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        const ssize_t array_size = ArraySize<FieldType>::size;
        MemberContainer<T_NoRef>::template Container<name, FieldType>::member = member;
        MemberContainer<T_NoRef>::template Container<name, FieldType>::array_size = array_size;
        _member_getters[name] = MemberContainer<T_NoRef>::template Container<name, FieldType>::get;
        _member_setters[name] = MemberContainer<T_NoRef>::template Container<name, FieldType>::set;
    }

    template<typename T>
    template<const char *const name, ssize_t size, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addConstAttribute(
            typename MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::member_t member,
            const ssize_t array_size) {
        assert(array_size == size);
        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::member = member;
        _member_getters[name] = MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::get;
    }

    template<typename T>
    template<const char *const name, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addAttributeConst(
            typename ConstMemberContainer<T_NoRef>::template Container<name, FieldType>::member_t member) {

        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        ConstMemberContainer<T_NoRef>::template Container<name, FieldType>::member = member;
        _member_getters[name] = ConstMemberContainer<T_NoRef>::template Container<name, FieldType>::get;
    }

    template<typename T>
    template<const char *const name, ssize_t size, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addArrayAttribute(
            typename MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::member_t member,
            const ssize_t array_size) {
        assert(array_size == size);
        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::member = member;
        _member_getters[name] = MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::get;
        _member_setters[name] = MemberContainer<T_NoRef>::template Container<name, FieldType[size]>::set;
    }

    template<typename T>
    template<const char *const name, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addClassAttribute(FieldType *member) {

        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        ClassMember<T_NoRef>::template Container<name, FieldType>::member = member;
        PyMethodDef pyMeth = {name,
                              (PyCFunction) ClassMember<T_NoRef>::template Container<name, FieldType>::call,
                              METH_VARARGS | METH_KEYWORDS | METH_CLASS,
                              doc_string
        };
        _addMethod(pyMeth);
    }

    template<typename T>
    template<const char *const name, typename FieldType>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addClassAttributeConst(FieldType const *member) {

        static const char *const doc = "Get attribute ";
        char *doc_string = new char[strlen(name) + strlen(doc) + 1];
        snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
        ConstClassMember<T_NoRef>::template Container<name, FieldType>::member = member;
        PyMethodDef pyMeth = {name,
                              (PyCFunction) ConstClassMember<T_NoRef>::template Container<name, FieldType>::call,
                              METH_VARARGS | METH_KEYWORDS | METH_CLASS,
                              doc_string
        };
        _addMethod(pyMeth);
    }
}

#endif