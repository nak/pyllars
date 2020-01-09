#ifndef PYLLARS__CLASSWRAPPER_IMPL
#define PYLLARS__CLASSWRAPPER_IMPL

#include "pyllars_classwrapper.hpp"
#include "pyllars_base.impl.hpp"

namespace pyllars_internal {

    //Implementations are in subfile pyllars_classwrapper-*.impl.hpp

    template<typename T>
    template<typename ...Args>
    PyObject*
    PythonClassWrapper<T>::createAllocatedInstance(Args... args, ssize_t arraySize) {
        return Pointers<T>::template createAllocatedInstance<Args...>(args..., arraySize);
    }


    template<typename T>
    PyTypeObject*
    PythonClassWrapper<T>::getPyType(){
        return (initialize() == 0)?Base::getRawType():nullptr;
    }

    template <typename T>
    typename std::remove_const_t<std::remove_reference_t <T> > & //TODO: verify remove_const not needed
    PythonClassWrapper<T>::
    toCArgument(){
        if constexpr (std::is_const<T_NoRef>::value){
            throw PyllarsException(PyExc_TypeError, "Invalid conversion from non const reference to const reference");
        } else {
            return *(( std::remove_const_t<std::remove_reference_t <T> > *)Base::get_CObject());
        }
    }


    template<typename T>
    PythonClassWrapper<T> *
    PythonClassWrapper<T>::
    fromCObject(T_NoRef &cobj) {
        if (!getPyType()->tp_name) {
            PyErr_SetString(PyExc_RuntimeError, "Uninitialized type when creating object");
            return nullptr;
        }
        auto pyobj = (PythonClassWrapper *) PyObject_Call(
                reinterpret_cast<PyObject *>(getPyType()), NULL_ARGS(), nullptr);
        if (pyobj) {
            if constexpr (std::is_reference<T>::value){
                pyobj->set_CObject(&cobj);
            } else if constexpr (std::is_copy_constructible<T>::value){
                pyobj->set_CObject(new T_NoRef(cobj));
            } else {
                PyErr_Format(PyExc_RuntimeError, "Attempt to make copy of non-copiable type %s", Types<T>::type_name());
                return nullptr;
            }
        }
        return pyobj;
    }


#if PY_MAJOR_VERSION >= 3
#  define Py_TPFLAGS_HAVE_ITER 0
#endif

    template<typename T>
    template<typename BaseClass>
    void  Classes<T>::
    addBaseClass() {
        PyTypeObject * base = PythonClassWrapper<BaseClass>::getRawType();
        if (!base) return;
        Base::getTypeProxy()._baseClasses.insert(Base::getTypeProxy()._baseClasses.begin(), base);
        //Base::getTypeProxy()._childrenReadyFunctions.insert(
        //        Base::getTypeProxy()._childrenReadyFunctions.begin(),
        //        &PythonClassWrapper<BaseClass>::getPyType);
        auto baseTyp = PythonClassWrapper<BaseClass>::getRawType();
        auto key = std::pair{baseTyp, Base::getRawType()};
        CommonBaseWrapper::castMap()[key] = &PythonClassWrapper<T>::template cast<BaseClass>;
    }

    template<typename T>
    PyObject *References<T>::fromCObject(T cobj, PyObject *referencing) {
        auto * pyobj = reinterpret_cast<PythonClassWrapper<T>*>(PyObject_Call((PyObject*)PythonClassWrapper<T>::getPyType(), NULL_ARGS(),
                                                                              referencing));
        return reinterpret_cast<PyObject*>(pyobj);
    }

    template<typename T>
    PyObject *References<T>::fromCArray(T cobj, size_t array_size, PyObject *referencing) {
        static_assert(is_pointer_like<T>::value);
        auto * pyobj = reinterpret_cast<PythonClassWrapper<T>*>(PyObject_Call((PyObject*)PythonClassWrapper<T>::getPyType(), NULL_ARGS(),
                                                                              nullptr));
        pyobj->set_CObject(&cobj, array_size);
        pyobj->make_reference(referencing);
        return reinterpret_cast<PyObject*>(pyobj);
    }

    template<typename T>
    PyObject *References<T>::referenceTo(PythonClassWrapper<T_NoRef> *obj) {
        auto * pyobj = reinterpret_cast<PythonClassWrapper<T>*>(PyObject_Call((PyObject*)PythonClassWrapper<T>::getPyType(), NULL_ARGS(),
                                                                              nullptr));
        pyobj->set_CObject(obj->get_CObject());
        pyobj->make_reference(obj);
        return reinterpret_cast<PyObject*>(pyobj);
    }

}
#endif
