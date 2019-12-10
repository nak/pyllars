//
// Created by jrusnak on 11/10/19.
//

#ifndef PYLLARS_PYLLARS_CLASSWRAPPER_TYPE_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSWRAPPER_TYPE_IMPL_HPP
#include <algorithm>
#include "pyllars_classwrapper.hpp"
#include "pyllars_pointer.impl.hpp"
#include "pyllars_staticfunctionsemantics.impl.hpp"
#include "pyllars_pointer-type.impl.hpp"

namespace {
    const cstring value_name = "value";
    const char* const value_kwlist[] = {"obj", nullptr};
}

namespace pyllars_internal {
    namespace{
        template <typename T>
        struct Shadow {
            char value[sizeof(T)];
        };

    }

    template <typename T>
    struct TypeInfo< Shadow<T> >{
        static constexpr cstring type_name = "?ShadowType?";
    };


    template<typename T, typename TrueType>
    PyObject *PythonClassWrapper_Base<T, TrueType>::
    addr(PyObject *self, PyObject *args) {
        if ((args && PyTuple_Size(args) > 0)) {
            PyErr_BadArgument();
            return nullptr;
        }
        PythonClassWrapper<T *>::initialize();
        PythonClassWrapper_Base *self_ = reinterpret_cast<PythonClassWrapper_Base *>(self);
        PyObject* obj;
        if(self_->get_CObject()){
            obj = PyObject_Call((PyObject*)PythonClassWrapper<T*>::getPyType(), NULL_ARGS(), nullptr);
            ((PythonClassWrapper<T*>*)obj)->set_CObject(&self_->get_CObject());
            ((PythonClassWrapper<T*>*)obj)->make_reference(self);
            PyErr_Clear();
            (reinterpret_cast<PythonClassWrapper<T *>*>(obj))->make_reference(self);
        } else {
            obj = Py_None;
        }
        return obj;
    }



    template <typename  T, typename TrueType>
    const T&
    PythonClassWrapper_Base<T, TrueType>::
    toCArgument() const{
        return *PythonBaseWrapper<T>::get_CObject();
    }


    namespace {

        template<typename T, typename ...Other>
        void for_each_init_ptrs() {
            int unused[] = {(CommonBaseWrapper::addCast<T, Other>(
                    PythonClassWrapper<T>::getRawType(),
                    PythonClassWrapper<Other>::getRawType(),
                    &CommonBaseWrapper::template interpret_cast<T, Other>), 0)...};
            (void)unused;
        }
    }

    template <typename  T, typename TrueType>
    void PythonClassWrapper_Base<T, TrueType>::_initAddCArgCasts(){
        static_assert(!std::is_reference<T>::value && !std::is_pointer<T>::value);
        typedef std::remove_cv_t <T> T_bare;
        if constexpr (!std::is_const<T>::value && !std::is_volatile<T>::value) {
            for_each_init_ptrs<T, T_bare &, const T_bare &, const T_bare>();
            for_each_init_ptrs<T&, T_bare &, const T_bare &, T_bare &&, const T_bare &&, const T_bare>();
            for_each_init_ptrs<T&&, T_bare &, const T_bare &, T_bare &&, const T_bare &&, const T_bare>();
        } else if (std::is_const<T>::value && !std::is_volatile<T>::value){
            for_each_init_ptrs<T, const T_bare &, const T_bare &&, T_bare , const T_bare>();
            for_each_init_ptrs<T&, const T_bare &, const T_bare &&, T_bare , const T_bare>();
            for_each_init_ptrs<T&&, const T_bare &, const T_bare &&, T_bare , const T_bare>();
        } else if (!std::is_const<T>::value && std::is_volatile<T>::value){
            for_each_init_ptrs<T, volatile T_bare &, const volatile T_bare &, volatile T_bare &&, const volatile T_bare &&, const volatile T_bare>();
            for_each_init_ptrs<T&, volatile T_bare &, const volatile T_bare &,volatile T_bare &&, const volatile T_bare &&, const volatile T_bare>();
            for_each_init_ptrs<T&&, volatile T_bare &, const volatile T_bare &,volatile T_bare &&, const volatile T_bare &&, const volatile T_bare>();
        } else {
            for_each_init_ptrs<T, const volatile T_bare &,const volatile T_bare &&, volatile T_bare , const volatile T_bare>();
            for_each_init_ptrs<T&, const volatile T_bare &,  const volatile T_bare &&, volatile T_bare , const volatile T_bare>();
            for_each_init_ptrs<T&&, const volatile T_bare &,  const volatile T_bare &&, volatile T_bare , const volatile T_bare>();
        }
    }


    template<typename T, typename TrueType>
    void PythonClassWrapper_Base<T, TrueType>::
    _dealloc(PyObject *self_) {
        auto * self = reinterpret_cast<PythonClassWrapper_Base*>(self_);
        if (!self) return;
        if (self->_referenced) {
            Py_DECREF(self->_referenced);
            self->_referenced = nullptr;
        }
        _free(self);
    }


    template<typename T, typename TrueType>
    void PythonClassWrapper_Base<T, TrueType>::
    _free(void *self_) {
        auto *self = (PythonClassWrapper_Base *) self_;
        if (!self->get_CObject()) return;
        if constexpr (std::is_destructible<T>::value) {
            delete self->get_CObject();
        }
        self->set_CObject(nullptr);
    }


    template<typename T, typename TrueType>
    PyObject *PythonClassWrapper_Base<T, TrueType>::
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        (void) args;
        (void) kwds;
        PythonClassWrapper_Base *self;
        self = (PythonClassWrapper_Base *) type->tp_alloc(type, 0);
        if (nullptr != self) {
            self->set_CObject(nullptr);
        }
        return (PyObject *) self;
    }

    template<typename T, typename TrueType>
    PyObject *PythonClassWrapper_Base<T, TrueType>::
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
            PyErr_SetString(PyExc_TypeError,  "new takes only one arg (an int, a tuple, or list of tuples)");
            return nullptr;
        }
        auto arg = PyTuple_GetItem(args, 0);
        if (PyLong_Check(arg)) {
            const ssize_t size = PyLong_AsLong(arg);
            if (size < 0){
                throw PyllarsException(PyExc_ValueError, "Size cannot be negative");
            }
            return (PyObject *) PythonClassWrapper<T *>::template allocateArray<>(size);
        } else if (PyTuple_Check(arg)) {
            auto list = PyList_New(1);
            PyList_SetItem(list, 0, arg);
            Py_INCREF(arg);
            auto new_arg = PyTuple_New(1);
            PyTuple_SetItem(new_arg, 0, list);
            auto obj = alloc(cls, new_arg, kwds);
            //Py_INCREF(list); // new_arg selfilshly grabs reference in SetItem call
            Py_DECREF(new_arg);
            return obj;
        } else if (PyList_Check(arg)) {

            if constexpr(!is_complete<T>::value){
                throw PyllarsException(PyExc_ValueError, "Cannot allocate multiple disparate instances using incomplete type");
            } else {
                const ssize_t size = PyList_Size(arg);
                if (size < 0) {
                    throw PyllarsException(PyExc_SystemError, "Internal error getting size of list");
                }
                typedef T * T_fixed_array;//[size];

                auto bytebucket = new unsigned char[size * sizeof(T)];
                memset(bytebucket, 0, sizeof(T)*size);
                T_fixed_array cobj_ptr = (T_fixed_array) bytebucket;
                for (ssize_t i = 0; i < size; ++i) {
                    PyObject *constructor_args = PyList_GetItem(arg, i);
                    if (!constructor_args || !PyTuple_Check(constructor_args)) {
                        PyErr_SetString(PyExc_TypeError,
                                        "Invalid constructor arguments: not a tuple as expected, or index out of range");
                        return nullptr;
                    }
                    T *cobj = nullptr;
                    for (auto const &[kwlist_, constructor_] : PythonClassWrapper_Base<T, TrueType>::_constructors()) {
                        try {
                            cobj = constructor_(kwlist_, constructor_args, nullptr, bytebucket + i);
                            if (cobj) break;
                        } catch (PyllarsException &) {
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
                        if constexpr (std::is_destructible<T>::value) {
                            for (ssize_t j = 0; j < i; ++j) {
                                cobj_ptr[j].~T();
                            }
                        }
                        PyErr_SetString(PyExc_TypeError, "Invalid constructor argsument");
                        return nullptr;
                    }
                }
                auto pyobj = PythonClassWrapper<T *>::fromInPlaceAllocation(size, bytebucket);
                return (PyObject *) pyobj;
            }
        }
        throw PyllarsException(PyExc_ValueError, "Invalid constructor arguments");
    }



    template<typename T, typename TrueType>
    int PythonClassWrapper_Base<T, TrueType>::
    _init(PythonClassWrapper_Base *self, PyObject *args, PyObject *kwds) {
        int status = 0;
        _initAddCArgCasts();


        for (auto const &ready: Base::getTypeProxy()._childrenReadyFunctions){
            if(!ready()){
                status |= -1;
            }
        }
        if (status != 0){
            PyErr_SetString(PyExc_SystemError, "Error in initializing class type");
            return -1;
        }
        if constexpr ( has_operator_compare<T, T>::value) {
            self->compare = [](CommonBaseWrapper *self_, CommonBaseWrapper *other) -> bool {
                if constexpr(has_operator_compare<T, T>::value) {
                    typedef std::remove_volatile_t <T> T_bare;
                    return PyObject_TypeCheck(other, getPyType()) &&
                           (*((T_bare*)reinterpret_cast<PythonClassWrapper_Base *>(self_)->get_CObject()) ==
                           * ((T_bare*)reinterpret_cast<PythonClassWrapper_Base *>(other)->get_CObject()));
                } else {
                    return false;
                }
            };
        } else {
            self->compare = [](CommonBaseWrapper *self_, CommonBaseWrapper *other) -> bool {
                return false;
            };
        }
        self->hash = [](CommonBaseWrapper* self)->size_t{
            static std::hash<typename std::remove_cv<T>::type*> hasher;
            return hasher(const_cast<typename std::remove_cv<T>::type*>(((PythonClassWrapper_Base*)self)->get_CObject()));
        };

        self->_referenced = nullptr;
        PyTypeObject *const coreTypePtr = PythonClassWrapper<typename core_type<T>::type>::getPyType();
        self->template populate_type_info<T>([](PyObject* o){return Base::getTypeProxy().checkType(o);}, coreTypePtr);
        if (!Base::getTypeProxy()._member_getters.count("this")) {
            Base::getTypeProxy()._member_getters["this"] = getThis;
        }
        if (!self) {
            return -1;
        }
        static const char *kwlist[] = {"_CObject", nullptr};
        self->set_CObject(nullptr);

        const bool have_args = args != NULL_ARGS();

        if (!have_args) {
            self->set_CObject((T*)kwds);
            return 0;
        }

        for (auto const &[kwlist_, constructor] : PythonClassWrapper_Base<T, TrueType>::_constructors()) {
            (void) kwlist_;
            try {
                self->set_CObject(constructor(kwlist, args, kwds, nullptr));
                if (self->get_CObject()) {
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
        if ((PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 0) {
            PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
            return -1;
        } else {
            if(!PyErr_Occurred()) {
                PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
            }
            return -1;
        }

        PyErr_SetString(PyExc_TypeError, "Unknown type for init");
        return -1;
    }


    template <typename T>
    long long enum_convert(const T& val){
        if constexpr(std::is_volatile<T>::value){
            return *reinterpret_cast<const volatile typename std::underlying_type<T>::type *>(&val);
        } else {
            return *reinterpret_cast<const typename std::underlying_type<T>::type *>(&val);
        }
    }


    template<typename T, typename TrueType>
    int
    PythonClassWrapper_Base<T, TrueType>::_initialize(PyTypeObject &Type) {
        typedef typename std::remove_cv_t<typename std::remove_reference_t<T> > basic_type;
        typedef PythonClassWrapper_Base<basic_type, basic_type> Basic;
        static bool inited = false;
        if (inited) return 0;
        _isInitialized = false;
        inited = true;
        
        if(Type.tp_basicsize == -1) {
            Type.tp_basicsize = sizeof(PythonClassWrapper_Base);
            Type.tp_dealloc = (destructor) _dealloc;
            Type.tp_getattr = _pyGetAttr;
            Type.tp_setattr = _pySetAttr;
            Type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES;
            Type.tp_init = (initproc) PythonClassWrapper_Base::_init;
            Type.tp_free = _free;
        }
        if (!Type.tp_new){
            Type.tp_new = _new;

        }
        if constexpr (std::is_enum<T >::value){
            typedef long long (*func_t)(const T&);
            Base::getTypeProxy().template addStaticMethod<value_name, value_kwlist, func_t, enum_convert<T> >();
        }
        PyMethodDef pyMethAlloc = {
                alloc_name_,
                (PyCFunction) alloc,
                METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                "allocate array of single dynamic instance of this class"
        };
        Base::getTypeProxy()._methodCollection[alloc_name_] = pyMethAlloc;
        if (!Base::getTypeProxy()._baseClasses.empty()) {
            if (Base::getTypeProxy()._baseClasses.size() > 1) {
                Type.tp_bases = PyTuple_New(Base::getTypeProxy()._baseClasses.size());
                size_t index = 0;
                std::for_each(Base::getTypeProxy()._baseClasses.begin(),
                        Base::getTypeProxy()._baseClasses.end(),
                        [&index, Type](PyTypeObject * const baseClass) {
                            PyTuple_SetItem(Type.tp_bases, index++, (PyObject *) baseClass);
                            Py_INCREF(baseClass); //SetItem steals a reference
                });
            } else if (PythonBaseWrapper<T>::getTypeProxy()._baseClasses.size() == 1) {
                Type.tp_base = PythonBaseWrapper<T>::getTypeProxy()._baseClasses[0];
            }
            for (auto& baseClass : Base::getTypeProxy()._baseClasses){
                // tp_bases not usable for inheritance of methods/members as it doesn't really do the right thing and
                // causes problems on lookup of base classes,
                // so do this manually...
                {
                    PyMethodDef *def = baseClass->tp_methods;
                    if (Base::getTypeProxy()._methodCollection.count(def->ml_name) == 0) {
                        while (def->ml_name != nullptr) {
                            Base::getTypeProxy()._methodCollection[def->ml_name] = *def;
                            ++def;
                        }
                    }
                }
                {
                    auto *def = baseClass->tp_getset;
                    while (def->name != nullptr) {
                        if (Basic::getTypeProxy()._member_getters.count(def->name) == 0) {
                            Basic::getTypeProxy()._member_setters[def->name] = def->set;
                            Basic::getTypeProxy()._member_getters[def->name] = def->get;
                        }
                        ++def;
                    }
                }
            }
        }
        Type.tp_methods = new PyMethodDef[Base::getTypeProxy()._methodCollection.size() +
                Base::getTypeProxy()._methodCollectionConst.size() + 1];
        Type.tp_methods[Base::getTypeProxy()._methodCollection.size() +
                Base::getTypeProxy()._methodCollectionConst.size()] = {nullptr};

        static PyMappingMethods methods =
                {
                nullptr,
                [](PyObject* s, PyObject* k){return Base::getTypeProxy().mapGet(s, k);},
                [](PyObject* s, PyObject* k, PyObject* v){return Base::getTypeProxy().mapSet(s, k, v,  std::is_const<T>::value);}
                };
        Type.tp_as_mapping = &methods;
        size_t index = 0;
        for (auto const&[key, methodDef]: Base::getTypeProxy()._methodCollection) {
            (void) key;
            Type.tp_methods[index] = methodDef;
            ++index;
        }
        for (auto const&[key, methodDef]:Base::getTypeProxy()._methodCollectionConst) {
            (void) key;
            Type.tp_methods[index] = methodDef;
            ++index;
        }
        Base::getTypeProxy()._member_getters = Basic::getTypeProxy()._member_getters;
        Base::getTypeProxy()._member_setters = Basic::getTypeProxy()._member_setters;

        Type.tp_getset = new PyGetSetDef[Base::getTypeProxy()._member_getters.size() + 1];
        Type.tp_getset[Base::getTypeProxy()._member_getters.size()] = {nullptr};
        index = 0;
        for (auto const&[key, getter]: Base::getTypeProxy()._member_getters) {
            auto it =Base::getTypeProxy()._member_setters.find(key);
            _setattrfunc setter = nullptr;
            if (! std::is_const<T>::value && it !=Base::getTypeProxy()._member_setters.end()) {
                setter = it->second;
            } else {
                setter = nullptr;
            }
            Type.tp_getset[index].name = (char*)key.c_str();
            Type.tp_getset[index].get = getter;
            Type.tp_getset[index].set = setter;
            Type.tp_getset[index].doc = (char*)"get/set attribute";
            Type.tp_getset[index].closure = nullptr;
            index++;
        }

        if constexpr (!std::is_const<T>::value) {
            Base::getTypeProxy()._unaryOperators = Base::getTypeProxy()._unaryOperators;
        }
        Base::getTypeProxy()._unaryOperatorsConst = Base::getTypeProxy()._unaryOperatorsConst;

        static  std::map<OpUnaryEnum, unaryfunc *> unary_mapping = {
                {OpUnaryEnum::INV, &Type.tp_as_number->nb_invert},
                {OpUnaryEnum::NEG, &Type.tp_as_number->nb_negative},
                {OpUnaryEnum::POS, &Type.tp_as_number->nb_positive}
        };

        for (auto const&[name_, func]: Base::getTypeProxy()._unaryOperators) {
            if (unary_mapping.count(name_) == 0) {
                return -1;
            }
            *unary_mapping[name_] = func;
        }
        for (auto const&[name_, func]: Base::getTypeProxy()._unaryOperatorsConst) {
            if (unary_mapping.count(name_) == 0) {
                return -1;
            }
            *unary_mapping[name_] = func;
        }
        if constexpr (!std::is_const<T>::value) {
            Base::getTypeProxy()._binaryOperators = Base::getTypeProxy()._binaryOperators;
        }
        Base::getTypeProxy()._binaryOperatorsConst = Base::getTypeProxy()._binaryOperatorsConst;

        static std::map<OpBinaryEnum , binaryfunc *> binary_mapping =
                {{OpBinaryEnum::ADD,     &Type.tp_as_number->nb_add},
                 {OpBinaryEnum::AND,     &Type.tp_as_number->nb_and},
                 {OpBinaryEnum::OR,      &Type.tp_as_number->nb_or},
                 {OpBinaryEnum::XOR,     &Type.tp_as_number->nb_xor},
                 {OpBinaryEnum::DIV,     &Type.tp_as_number->nb_true_divide},
                 {OpBinaryEnum::MOD,     &Type.tp_as_number->nb_remainder},
                 {OpBinaryEnum::MUL,     &Type.tp_as_number->nb_multiply},
                 {OpBinaryEnum::LSHIFT,  &Type.tp_as_number->nb_lshift},
                 {OpBinaryEnum::RSHIFT,  &Type.tp_as_number->nb_rshift},
                 {OpBinaryEnum::SUB,     &Type.tp_as_number->nb_subtract},
                 {OpBinaryEnum::IADD,    &Type.tp_as_number->nb_inplace_add},
                 {OpBinaryEnum::IAND,    &Type.tp_as_number->nb_inplace_and},
                 {OpBinaryEnum::IOR,     &Type.tp_as_number->nb_inplace_or},
                 {OpBinaryEnum::IXOR,    &Type.tp_as_number->nb_inplace_xor},
                 {OpBinaryEnum::IDIV,    &Type.tp_as_number->nb_inplace_true_divide},
                 {OpBinaryEnum::IMOD,    &Type.tp_as_number->nb_inplace_remainder},
                 {OpBinaryEnum::IMUL,    &Type.tp_as_number->nb_inplace_multiply},
                 {OpBinaryEnum::ILSHIFT, &Type.tp_as_number->nb_inplace_lshift},
                 {OpBinaryEnum::IRSHIFT, &Type.tp_as_number->nb_inplace_rshift},
                 {OpBinaryEnum::ISUB,    &Type.tp_as_number->nb_inplace_subtract},

                };
        for (auto const&[name_, func]: Base::getTypeProxy()._binaryOperators) {
            if (binary_mapping.count(name_) == 0) {
                throw PyllarsException(PyExc_SystemError, "Undefined operator name (internal error)");
            }
            *binary_mapping[name_] = func;
        }
        for (auto const&[name_, func]: Base::getTypeProxy()._binaryOperatorsConst) {
            if (binary_mapping.count(name_) == 0) {
                throw PyllarsException(PyExc_SystemError, "Undefined operator name (internal error)");
            }
            *binary_mapping[name_] = func;
        }
        Base::getTypeProxy()._baseClasses = Base::getTypeProxy()._baseClasses;
        if (!Type.tp_base && Base::getTypeProxy()._baseClasses.size() == 0) {
            if (PyType_Ready(CommonBaseWrapper::getRawType()) < 0) {
                PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
                return -1;
            }
            Type.tp_base = CommonBaseWrapper::getRawType();
        }


        Type.tp_dict = PyDict_New();
        Base::getTypeProxy()._classTypes = Base::getTypeProxy()._classTypes;
        for (auto const &[name, type]: Base::getTypeProxy()._classTypes){
            PyDict_SetItemString(Type.tp_dict, name.c_str(), (PyObject*)type());
        }

        if constexpr (std::is_enum<T>::value || is_scoped_enum<T>::value) {
            _classEnumValues() = Basic::_classEnumValues();
            for (auto const&[name_, value]: _classEnumValues()) {
                // can only be called after ready of Type:

                const Shadow<T> *shadowed = reinterpret_cast<const Shadow<T> *>(value);
                PythonClassWrapper<Shadow<T>>::_initAddCArgCasts();
                auto *pyval = (PythonClassWrapper<const Shadow<T> > *) PyObject_Call(
                        (PyObject *) PythonClassWrapper<const Shadow<T> >::getPyType(), NULL_ARGS(), nullptr);
                pyval->set_CObject(shadowed);
                if (pyval) {
                    PyDict_SetItemString(Type.tp_dict, name_.c_str(),
                                         (PyObject *) reinterpret_cast<PythonClassWrapper<const T> *>(pyval));
                    ((PyObject*)pyval)->ob_type = Base::getRawType();
                } else {
                    return -1;
                }
            }
        }
        Base::getTypeProxy()._classObjects = PythonBaseWrapper<T>::getTypeProxy()._classObjects;
        for (auto const&[name_, pyval]: Base::getTypeProxy()._classObjects) {
            // can only be called after ready of Type:
            if (pyval) {
                PyDict_SetItemString(Type.tp_dict, name_.c_str(), pyval);
            } else {
                return -1;
            }
        }

        if constexpr (!std::is_const<T>::value) {
            static const char *const const_name = "const";
            typedef const typename std::remove_const_t<T> T_const;
            PyDict_SetItemString(Type.tp_dict, const_name, (PyObject *) PythonClassWrapper<T_const>::getPyType());
        }
        PyDict_SetItemString(Type.tp_dict, "this", (PyObject*) PythonClassWrapper<T*>::getPyType());
        if constexpr(!std::is_volatile<T>::value) {
            static const char *const volatile_name = "volatile";
            typedef volatile typename std::remove_volatile_t<T> T_volatile;
            PyDict_SetItemString(Type.tp_dict, volatile_name, (PyObject *) PythonClassWrapper<T_volatile>::getPyType());
        }

        if (PyType_Ready(&Type) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
            return -1;
        }
        PyObject *const type = reinterpret_cast<PyObject *>(&Type);
        Py_INCREF(type);

        _isInitialized = inited;
        return 0;
    }



    template<typename T, typename TrueType>
    bool PythonClassWrapper_Base<T, TrueType>::_isInitialized = false;


    template<const char *const name, const char *const kwlist[], typename func_type, func_type method>
    void CommonBaseWrapper::TypedProxy::
    addStaticMethod() {
        static std::string doc = std::string("Call class method ") + name;
        PyMethodDef pyMeth = {
                name,
                (PyCFunction) StaticFunctionContainer<kwlist, func_type, method>::call,
                METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                doc.c_str()
        };
        _methodCollectionConst[name] = pyMeth;
    }


    template<typename Class, const char *const name, const char* const kwlist[], typename method_t, method_t method>
    void CommonBaseWrapper::TypedProxy::
    addMethod() {
        static std::string doc = std::string("Call method ") +  func_traits<method_t>::type_name();
        PyMethodDef pyMeth = {
                name,
                (PyCFunction) MethodContainer<kwlist, method_t, method>::call,
                METH_KEYWORDS | METH_VARARGS,
                doc.c_str()
        };
        if constexpr(func_traits<method_t>::is_const_method) {
            _methodCollectionConst[name]  = pyMeth;
            if constexpr (!std::is_const<Class>::value) {
                PythonClassWrapper<const Class>::_Type.template addMethod<const Class, name, kwlist, method_t, method>();
            }
        } else {
            _methodCollection[name] = pyMeth;
        }
    }

}

#endif
