#ifndef PYLLARS__CLASSWRAPPER_IMPL
#define PYLLARS__CLASSWRAPPER_IMPL

#include "pyllars_classwrapper.hpp"
#include "pyllars_base.impl.hpp"

namespace pyllars_internal {

//Implementations are in subfile pyllars_classwrapper-*.impl.hpp

    template<typename T, typename TrueType>
    template<typename Parent>
    status_t
    PythonClassWrapper_Base<T, TrueType>::ready(){
        int status = 0;
        for (const auto& ready_fnctn: PythonBaseWrapper<T>::getTypeProxy()._childrenReadyFunctions){
            status |= (ready_fnctn() == nullptr);
        }

        return status;
    }

    template<typename T>
    PyTypeObject*
    PythonClassWrapper<T, typename std::enable_if< is_rich_class<T>::value>::type>::getPyType(){
        return (initialize() == 0)?Base::getRawType():nullptr;
    }

    template<typename T, typename TrueType>
    template<typename BaseClass>
    void PythonClassWrapper_Base<T, TrueType>::
    addBaseClass() {
        PyTypeObject * base = PythonClassWrapper<BaseClass>::getRawType();
        if (!base) return;
        PythonClassWrapper<T>::getTypeProxy()._baseClasses.insert(PythonClassWrapper<T>::getTypeProxy()._baseClasses.begin(), base);
        PythonClassWrapper<T>::getTypeProxy()._childrenReadyFunctions.insert(
                PythonClassWrapper<T>::getTypeProxy()._childrenReadyFunctions.begin(), 
                &PythonClassWrapper<BaseClass>::getPyType);
        auto baseTyp = PythonClassWrapper<BaseClass>::getRawType();
        auto key = std::pair{baseTyp, PythonBaseWrapper<T>::getRawType()};
        CommonBaseWrapper::castMap()[key] = &cast<BaseClass>;
    }


    template <typename T, typename TrueType>
    typename std::remove_const<T>::type & //TODO: verify remove_const not needed
    PythonClassWrapper_Base<T, TrueType>::
    toCArgument(){
        if constexpr (std::is_const<T>::value){
            throw PyllarsException(PyExc_TypeError, "Invalid conversion from non const reference to const reference");
        } else {
            return *PythonClassWrapper<T>::get_CObject();
        }
    }


    template<typename T, typename TrueType>
    PythonClassWrapper_Base<T, TrueType> *
    PythonClassWrapper_Base<T, TrueType>::
    fromCObject(T &cobj) {
        if (!PythonClassWrapper<TrueType>::getTypeProxy().type().tp_name) {
            PyErr_SetString(PyExc_RuntimeError, "Uninitialized type when creating object");
            return nullptr;
        }
        auto pyobj = (PythonClassWrapper_Base *) PyObject_Call(
                reinterpret_cast<PyObject *>(PythonClassWrapper<TrueType>::getPyType()), NULL_ARGS(), nullptr);
        if (pyobj) {
            if constexpr (std::is_reference<T>::value){
                pyobj->set_CObject(&cobj);
            } else if constexpr (std::is_copy_constructible<T>::value){
                pyobj->set_CObject(new T(cobj));
            } else {
                PyErr_Format(PyExc_RuntimeError, "Attempt to make copy of non-copiable type %s", Types<T>::type_name());
                return nullptr;
            }
        }
        return pyobj;
    }

    template<typename T, typename TrueType>
    typename PythonClassWrapper_Base<T, TrueType>::Initializer
    PythonClassWrapper_Base<T, TrueType>::initializer;

}
#endif
