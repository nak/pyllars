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
            typedef PythonClassWrapper<T*> PtrWrapper;
            typedef typename ptr_depth<T*>::type_repr  T_repr;
            obj = PyObject_Call((PyObject*)PtrWrapper::getPyType(), NULL_ARGS(), nullptr);
            ((PtrWrapper *) obj)->set_CObject(&self_->get_CObject());
            ((PtrWrapper*)obj)->make_reference(self);
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
        if constexpr (std::is_destructible<T>::value && !std::is_reference<TrueType>::value) {
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
            } else if constexpr(std::is_same<Base, PythonBaseWrapper<TrueType> >::value){
                throw PyllarsException(PyExc_ValueError, "Cannot allocate reference type");
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
                    T *cobj = Base::allocate(constructor_args, nullptr, bytebucket + i);
                    if (!cobj) {
                        if constexpr (std::is_destructible<T>::value  && !has_finite_extent<T>::value) {
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
                    return PyObject_TypeCheck(other, PythonClassWrapper<TrueType>::getPyType()) &&
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
        if constexpr (!std::is_reference<TrueType>::value){
            auto obj = Base::allocate(args, kwds);
            if (obj) {
                self->set_CObject(obj);
                return 0;
            } else {
                return -1;
            }
        } else {
            return 0;
        }
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
        inited = true;
        Base::_addReinterpretations();
        if( Type.tp_basicsize == -1) {
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
            Type.tp_base = CommonBaseWrapper::getBaseType();
        }
        assert(Type.tp_base != &Type);
        Type.tp_dict = PyDict_New();
        Base::getTypeProxy()._classTypes = Base::getTypeProxy()._classTypes;
        for (auto const &[name, type]: Base::getTypeProxy()._classTypes){
            PyTypeObject* typ = type();
            PyType_Ready(typ);
            PyDict_SetItemString(Type.tp_dict, name.c_str(), (PyObject*) typ);
        }

        if constexpr ((std::is_enum<TrueType>::value || is_scoped_enum<TrueType>::value) &&
                      !std::is_same<Base, PythonBaseWrapper<T> >::value){
            Base::_classEnumValues() = Basic::_classEnumValues();
            for (auto const&[name_, value]: Base::_classEnumValues()) {
                // can only be called after ready of Type:

                const Shadow<T> *shadowed = reinterpret_cast<const Shadow<T> *>(value);
                PythonBaseWrapper<Shadow<T>>::_addReinterpretations();
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
            typedef const typename std::remove_const_t<TrueType> T_const;
            auto const_Type =  (PyObject *) PythonClassWrapper<T_const>::getPyType();
            PyDict_SetItemString(Type.tp_dict, const_name, const_Type);
        }
        PyDict_SetItemString(Type.tp_dict, "this", (PyObject*) PythonClassWrapper<T*>::getPyType());
        if constexpr(!std::is_volatile<T>::value) {
            static const char *const volatile_name = "volatile";
            typedef volatile typename std::remove_volatile_t<TrueType> T_volatile;
            auto * typ = (PyObject *) PythonClassWrapper<T_volatile>::getPyType();
            PyDict_SetItemString(Type.tp_dict, volatile_name, typ);
        }

        if (Type.tp_base) {
            if (PyType_Ready(Type.tp_base) < 0) {
                PyErr_SetString(PyExc_RuntimeError, "Failed to ready base type");
                return -1;
            }
        }
        if (Type.tp_bases){
            for (int i = 0; i < PyTuple_Size(Type.tp_bases); ++i){
                if (PyType_Ready((PyTypeObject*) PyTuple_GetItem(Type.tp_bases, i)) < 0){
                    PyErr_SetString(PyExc_RuntimeError, "Failed to ready base type");
                    return -1;
                }
            }
        }
        if (PyType_Ready(&Type) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to ready type!");
            return -1;
        }


        PyObject *const type = reinterpret_cast<PyObject *>(&Type);
        Py_INCREF(type);

        return 0;
    }


    template<typename T, typename TrueType>
    int
    PythonClassWrapper_Base<T, TrueType>::readyType() {

    }


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
                PythonClassWrapper<const Class>::getTypeProxy().template addMethod<const Class, name, kwlist, method_t, method>();
            }
        } else {
            _methodCollection[name] = pyMeth;
        }
    }

}

#endif
