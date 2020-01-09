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
#include "pyllars_pointer-createAllocatedInstance.impl.hpp"

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


    template <typename T>
    const std::remove_reference_t<T>&
    PythonClassWrapper<T>::
    toCArgument() const{
        return *Base::get_CObject();
    }



    template<typename T>
    PyObject *PythonClassWrapper<T>::
    _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        (void) args;
        (void) kwds;
        PythonClassWrapper *self;
        self = (PythonClassWrapper *) type->tp_alloc(type, 0);
        if (nullptr != self) {
            self->set_CObject(nullptr);
        }
        return (PyObject *) self;
    }

    template<typename T>
    PyObject *PythonClassWrapper<T>::
    alloc(PyObject *cls, PyObject *args, PyObject *kwds) {
        /**
         * Takes no kwds
         * args is of length 1
         * if arg is int value, allocate array of that size based on default constructor (or raise exc if no such constructor)
         * If arg is a tuple, use that as the arguments to a constructor for allocation of single item
         * If arg is a list of tuples, cosntrcut an array of that size of objects using each tuple as cosntructor args
         *  (allocation will be through generic byte buffer and then in-place-memory allocation)
         */
         if constexpr(std::is_reference<T>::value){
             throw PyllarsException(PyExc_TypeError, "cannot allocate a reference type");
         }
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
            auto*  obj = Pointers<typename ptr_depth<T_NoRef*>::type_repr>::template createAllocatedInstance<>(size);
            ((PythonBaseWrapper<typename ptr_depth<T_NoRef>::type_repr*>*)obj)->set_ptr_depth(ptr_depth<T_NoRef>::value + 1);
            return reinterpret_cast<PyObject*>(obj);
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

            if constexpr(!is_complete<T_NoRef>::value){
                throw PyllarsException(PyExc_ValueError, "Cannot allocate multiple disparate instances using incomplete type");
            } else if constexpr(std::is_same<Base, PythonBaseWrapper<T> >::value){
                throw PyllarsException(PyExc_ValueError, "Cannot allocate reference type");
            } else {
                const ssize_t size = PyList_Size(arg);
                if (size < 0) {
                    throw PyllarsException(PyExc_SystemError, "Internal error getting size of list");
                }
                typedef T_NoRef * T_fixed_array;//[size];

                auto bytebucket = new unsigned char[size * sizeof(T_NoRef)];
                memset(bytebucket, 0, sizeof(T_NoRef)*size);
                T_fixed_array cobj_ptr = (T_fixed_array) bytebucket;
                for (ssize_t i = 0; i < size; ++i) {
                    PyObject *constructor_args = PyList_GetItem(arg, i);
                    if (!constructor_args || !PyTuple_Check(constructor_args)) {
                        PyErr_SetString(PyExc_TypeError,
                                "Invalid constructor arguments: not a tuple as expected, or index out of range");
                        return nullptr;
                    }
                    T_NoRef *cobj = (T_NoRef*) Base::allocate(constructor_args, nullptr, bytebucket + i);
                    if (!cobj) {
                        if constexpr (std::is_destructible<T>::value  && !has_finite_extent<T_NoRef>::value) {
                            for (ssize_t j = 0; j < i; ++j) {
                                cobj_ptr[j].~T();
                            }
                        }
                        PyErr_SetString(PyExc_TypeError, "Invalid constructor argsument");
                        return nullptr;
                    }
                }
                auto * pyobj = Pointers<T_NoRef*>::fromInPlaceAllocation(size, bytebucket);
                return (PyObject *) pyobj;
            }

        }
        throw PyllarsException(PyExc_ValueError, "Invalid constructor arguments");
    }



    template<typename T>
    int PythonClassWrapper<T>::
    _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
        int status = 0;

        if (!self) {
            return -1;
        }
        self->set_ptr_depth(ptr_depth<T>::value);

        if (status != 0){
            PyErr_SetString(PyExc_SystemError, "Error in initializing class type");
            return -1;
        }
        self->_referenced = nullptr;
        self->template populate_type_info<T>([](PyObject* o){return Base::getTypeProxy().checkType(o);});

        if constexpr ( has_operator_compare<T, T>::value) {
            self->compare = [](CommonBaseWrapper *self_, CommonBaseWrapper *other) -> bool {
                if constexpr(has_operator_compare<T, T>::value) {
                    typedef std::remove_volatile_t <T_NoRef> T_bare;
                    return PyObject_TypeCheck(other, PythonClassWrapper::getPyType()) &&
                           (*((T_bare*)reinterpret_cast<PythonClassWrapper *>(self_)->get_CObject()) ==
                           * ((T_bare*)reinterpret_cast<PythonClassWrapper *>(other)->get_CObject()));
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
            static std::hash<std::remove_cv_t<T_NoRef>*> hasher;
            return hasher(const_cast<std::remove_cv_t<T_NoRef>*>(((PythonClassWrapper*)self)->get_CObject()));
        };

        if (!Base::getTypeProxy()._member_getters.count("this")) {
            Base::getTypeProxy()._member_getters["this"] = getThis;
        }
        static const char *kwlist[] = {"_CObject", nullptr};
        self->set_CObject(nullptr);

        const bool have_args = args != NULL_ARGS();

        if (!have_args) {
            if (std::is_reference<T>::value) {
                auto * obj = reinterpret_cast<PythonClassWrapper*>(kwds);
                obj->set_CObject(obj->get_CObject());
                obj->make_reference(reinterpret_cast<PyObject*>(obj));
            } else {
                self->set_CObject((T_NoRef *)kwds); //kwds can be used to indicate the actual C object to set to; this is an internal hack for now
            }
            return 0;
        }
        if constexpr (!std::is_reference<T>::value){
            auto *obj = (T_NoRef*) Base::allocate(args, kwds);
            if (obj) {
                self->set_CObject(obj);
                return 0;
            } else {
                return -1;
            }
        } else {
            PyObject * arg = PyTuple_GetItem(args, 0);
            if (!arg || (!PythonClassWrapper::checkType(arg) &&
                !PyObject_TypeCheck(arg, getPyType()))) {
                PyErr_SetString(PyExc_ValueError, "Type mismatch create wrapper to reference to C object");
                return -1;
            }
            self->set_CObject(((PythonClassWrapper *) arg)->get_CObject());
            if (!self->get_CObject()) {
                PyErr_SetString(PyExc_ValueError, "Cannot create C reference to null object");
                return -1;
            }
            self->make_reference(arg);
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


    template<typename T>
    int
    PythonClassWrapper<T>::initialize() {
        auto Type = *Base::getRawType();
        typedef typename std::remove_cv_t<T_NoRef> basic_type;
        typedef PythonClassWrapper<basic_type> Basic;
        static bool inited = false;
        if (inited) return 0;
        inited = true;

        if (PyType_Ready(CommonBaseWrapper::getBaseType()) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to set_up type!");
            return -1;
        }

        if (Type.tp_basicsize == -1) {
            Type.tp_basicsize = sizeof(PythonClassWrapper) + (is_pointer_like<T>::value?8:0);
            Type.tp_dealloc = (destructor) Base::_dealloc;
            Type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES;
            Type.tp_init = (initproc) PythonClassWrapper::_init;
            Type.tp_free = Base::_free;
        }
        if constexpr (is_rich_class<T>::value){
            Type.tp_getattr = Classes<T>::_pyGetAttr;
            Type.tp_setattr =  Classes<T>::_pySetAttr;
        }
        if constexpr(std::is_reference<T>::value) {
            Type.tp_base = PythonClassWrapper<T_NoRef>::getPyType();
        } else if (std::is_const<T>::value || std::is_volatile<T>::value){
            Type.tp_base = PythonClassWrapper<std::remove_cv_t <T>>::getPyType();
        } else if constexpr (is_pointer_like<T>::value){
            Pointers<T>::_seqmethods.sq_length = Pointers<T>::_size;
            Pointers<T>::_seqmethods.sq_concat = Pointers<T>::_concat;
            Pointers<T>::_seqmethods.sq_repeat = Pointers<T>::_repeat;
            Pointers<T>::_seqmethods.sq_item = Pointers<T>::_get_item;
            Pointers<T>::_seqmethods.sq_ass_item = Pointers<T>::_set_item;
            Pointers<T>::_seqmethods.sq_contains = Pointers<T>::_contains;
            Pointers<T>::_seqmethods.sq_inplace_concat = Pointers<T>::_inplace_concat;
            Pointers<T>::_seqmethods.sq_inplace_repeat = Pointers<T>::_inplace_repeat;

            Type.tp_as_sequence = &Pointers<T>::_seqmethods;
            Type.tp_flags =  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE|  Py_TPFLAGS_HAVE_ITER;
            Type.tp_iter = Pointers<T>::Iter::iter;
            Type.tp_iternext = Pointers<T>::Iter::iter;

        } else  {
            if (!Type.tp_new) {
                Type.tp_new = _new;
            }

            if constexpr (std::is_enum<T_NoRef>::value) {
                //add enum conversion method to convert to int; TODO: make this an instance method
                typedef long long (*func_t)(const T_NoRef &);
                Base::getTypeProxy().template addStaticMethod<value_name, value_kwlist, func_t, enum_convert<T_NoRef> >();
            }
            PyMethodDef pyMethAlloc = {
                    alloc_name_,
                    (PyCFunction) alloc,
                    METH_KEYWORDS | METH_CLASS | METH_VARARGS,
                    "allocate array of single dynamic instance of this class"
            };

            //setup inheritance
            if (!Base::getTypeProxy()._baseClasses.empty()) {
                if (Base::getTypeProxy()._baseClasses.size() > 1) {
                    Type.tp_bases = PyTuple_New(Base::getTypeProxy()._baseClasses.size());
                    size_t index = 0;
                    std::for_each(Base::getTypeProxy()._baseClasses.begin(),
                                  Base::getTypeProxy()._baseClasses.end(),
                                  [&index, Type](PyTypeObject *const baseClass) {
                                      PyTuple_SetItem(Type.tp_bases, index++, (PyObject *) baseClass);
                                      Py_INCREF(baseClass); //SetItem steals a reference
                                  });
                } else if (Base::getTypeProxy()._baseClasses.size() == 1) {
                    Type.tp_base = Base::getTypeProxy()._baseClasses[0];
                }
                /*??? TODO: verify if this is needed
                for (auto &baseClass : Base::getTypeProxy()._baseClasses) {
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
                }*/
            } else {
                if (!Type.tp_base) {
                    Type.tp_base = CommonBaseWrapper::getBaseType();
                }
            }

            assert(Type.tp_base != &Type);

            {//set up methods
                Base::getTypeProxy()._methodCollection[alloc_name_] = pyMethAlloc;

                Type.tp_methods = new PyMethodDef[Base::getTypeProxy()._methodCollection.size() +
                                                  Base::getTypeProxy()._methodCollectionConst.size() + 1];
                Type.tp_methods[Base::getTypeProxy()._methodCollection.size() +
                                Base::getTypeProxy()._methodCollectionConst.size()] = {nullptr}; //sentinel

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
            }
            {
                //set up mapping operator
                static PyMappingMethods methods =
                        {
                                nullptr,
                                [](PyObject *s, PyObject *k) { return Base::getTypeProxy().mapGet(s, k); },
                                [](PyObject *s, PyObject *k, PyObject *v) {
                                    return Base::getTypeProxy().mapSet(s, k, v, std::is_const<T_NoRef>::value);
                                }
                        };


                Type.tp_as_mapping = &methods;
            }

            {// set up attribute setter/getters

                Type.tp_getset = new PyGetSetDef[Base::getTypeProxy()._member_getters.size() + 1];
                Type.tp_getset[Base::getTypeProxy()._member_getters.size()] = {nullptr};
                size_t index = 0;
                for (auto const&[key, getter]: Base::getTypeProxy()._member_getters) {
                    auto it = Base::getTypeProxy()._member_setters.find(key);
                    _setattrfunc setter = nullptr;
                    if (!std::is_const<T_NoRef>::value && it != Base::getTypeProxy()._member_setters.end()) {
                        setter = it->second;
                    } else {
                        setter = nullptr;
                    }
                    Type.tp_getset[index].name = (char *) key.c_str();
                    Type.tp_getset[index].get = getter;
                    Type.tp_getset[index].set = setter;
                    Type.tp_getset[index].doc = (char *) "get/set attribute";
                    Type.tp_getset[index].closure = nullptr;
                    index++;
                }
            }

            {//add unary operators
                static std::map<OpUnaryEnum, unaryfunc *> unary_mapping = {
                        {OpUnaryEnum::INV, &Type.tp_as_number->nb_invert},
                        {OpUnaryEnum::NEG, &Type.tp_as_number->nb_negative},
                        {OpUnaryEnum::POS, &Type.tp_as_number->nb_positive}
                };
                if constexpr (!std::is_const<T_NoRef>::value) {//no non-const operators for const types
                    for (auto const&[name_, func]: Base::getTypeProxy()._unaryOperators) {
                        if (unary_mapping.count(name_) == 0) {
                            return -1;
                        }
                        *unary_mapping[name_] = func; //sets Type.tp_as_number associated function
                    }
                }
                for (auto const&[name_, func]: Base::getTypeProxy()._unaryOperatorsConst) {
                    if (unary_mapping.count(name_) == 0) {
                        return -1;
                    }
                    *unary_mapping[name_] = func; //sets Type.tp_as_number associated function
                }
            }

            {//set binary operators

                static std::map<OpBinaryEnum, binaryfunc *> binary_mapping =
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
                if constexpr (!std::is_const<T_NoRef>::value) {//no non-const operators for const types
                    for (auto const&[name_, func]: Base::getTypeProxy()._binaryOperators) {
                        if (binary_mapping.count(name_) == 0) {
                            throw PyllarsException(PyExc_SystemError, "Undefined operator name (internal error)");
                        }
                        *binary_mapping[name_] = func; //sets Type.tp_as_number associated function
                    }
                }
                for (auto const&[name_, func]: Base::getTypeProxy()._binaryOperatorsConst) {
                    if (binary_mapping.count(name_) == 0) {
                        throw PyllarsException(PyExc_SystemError, "Undefined operator name (internal error)");
                    }
                    *binary_mapping[name_] = func; //sets Type.tp_as_number associated function
                }
            }

            { //set Python type attributes for this class's PyType
                Type.tp_dict = PyDict_New();
                Base::getTypeProxy()._classTypes = Base::getTypeProxy()._classTypes;
                for (auto const &[name, type]: Base::getTypeProxy()._classTypes) {
                    PyTypeObject *typ = type();
                    PyType_Ready(typ);
                    PyDict_SetItemString(Type.tp_dict, name.c_str(), (PyObject *) typ);
                }


                if constexpr ((std::is_enum<T_NoRef>::value || is_scoped_enum<T_NoRef>::value)) {
                    //Base::_classEnumValues() = Basic::_classEnumValues();
                    for (auto const&[name_, value]: Base::_classEnumValues()) {
                        // can only be called after ready of Type:

                        const Shadow<T_NoRef> *shadowed = reinterpret_cast<const Shadow<T_NoRef> *>(value);
                       // PythonBaseWrapper<Shadow<T_NoRef>>::_addReinterpretations();
                        auto *pyval = (PythonClassWrapper<const Shadow<T_NoRef> > *) PyObject_Call(
                                (PyObject *) PythonClassWrapper<const Shadow<T_NoRef> >::getPyType(), NULL_ARGS(), nullptr);
                        pyval->set_CObject(shadowed);
                        if (pyval) {
                            PyDict_SetItemString(Type.tp_dict, name_.c_str(),
                                                 (PyObject *) reinterpret_cast<PythonClassWrapper<const T_NoRef> *>(pyval));
                            ((PyObject *) pyval)->ob_type = Base::getRawType();
                        } else {
                            return -1;
                        }
                    }
                }
                Base::getTypeProxy()._classObjects = Base::getTypeProxy()._classObjects;
                for (auto const&[name_, pyval]: Base::getTypeProxy()._classObjects) {
                    if (pyval) {
                        PyDict_SetItemString(Type.tp_dict, name_.c_str(), pyval);
                    } else {
                        return -1;
                    }
                }

            }
        }

        if constexpr (is_rich_class<T>::value || is_pointer_like<T>::value) {
            if constexpr (!std::is_const<T_NoRef>::value) {
                static const char *const const_name = "const";
                auto const_Type = (PyObject *) PythonClassWrapper<const T>::getPyType();
                PyDict_SetItemString(Type.tp_dict, const_name, const_Type);
            }
            if constexpr(!std::is_volatile<T_NoRef>::value) {
                static const char *const volatile_name = "volatile";
                auto *typ = (PyObject *) PythonClassWrapper<volatile T>::getPyType();
                PyDict_SetItemString(Type.tp_dict, volatile_name, typ);
            }
        }

        //make sure bases are ready
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
        // finally, ready this type
        if (PyType_Ready(&Type) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to ready type!");
            return -1;
        }


        PyObject *const type = reinterpret_cast<PyObject *>(&Type);
        Py_INCREF(type);
        return 0;
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
            //if constexpr (!std::is_const<Class>::value) {
            //    PythonClassWrapper<const Class>::getTypeProxy().template addMethod<const Class, name, kwlist, method_t, method>();
            // }
        } else {
            _methodCollection[name] = pyMeth;
        }
    }

}

#endif
