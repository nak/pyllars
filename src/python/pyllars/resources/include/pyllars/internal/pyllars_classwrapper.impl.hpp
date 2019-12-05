#ifndef PYLLARS__CLASSWRAPPER_IMPL
#define PYLLARS__CLASSWRAPPER_IMPL

#include "pyllars_classwrapper.hpp"

namespace pyllars_internal {

//Implementations are in subfile pyllars_classwrapper-*.impl.hpp

    template<typename T>
    template<typename Parent>
    status_t
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::ready(){
        int status = 0;
        for (const auto& ready_fnctn: _Type._childrenReadyFunctions){
            status |= (ready_fnctn() == nullptr);
        }

        return status;
    }

    template<typename T>
    PyTypeObject*
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::getRawType(){
        return &_Type.type();
    }

    template<typename T>
    PyTypeObject*
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::getPyType(){
        return (initialize() == 0)?&_Type.type():nullptr;
    }

    template<typename T>
    template<typename Base>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addBaseClass() {
        PyTypeObject * base = PythonClassWrapper<Base>::getRawType();
        if (!base) return;
        _Type._baseClasses.insert(_Type._baseClasses.begin(), base);
        _Type._childrenReadyFunctions.insert(_Type._childrenReadyFunctions.begin(), &PythonClassWrapper<Base>::getPyType);
        auto baseTyp = PythonClassWrapper<Base>::getRawType();
        auto key = std::pair{baseTyp, getRawType()};
        castMap()[key] = &cast<Base>;
    }


    template <typename  T>
    typename std::remove_const<T>::type & //TODO: verify remove_const not needed
    PythonClassWrapper<T,typename std::enable_if<is_rich_class<T>::value>::type>::
    toCArgument(){
        if constexpr (std::is_const<T>::value){
            throw PyllarsException(PyExc_TypeError, "Invalid conversion from non const reference to const reference");
        } else {
            return *get_CObject();
        }
    }


    template<typename T>
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type> *
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    fromCObject(T_NoRef &cobj) {
        if (!_Type.type().tp_name) {
            PyErr_SetString(PyExc_RuntimeError, "Uninitialized type when creating object");
            return nullptr;
        }
        PythonClassWrapper *pyobj = (PythonClassWrapper *) PyObject_Call(
                reinterpret_cast<PyObject *>(getPyType()), NULL_ARGS(), nullptr);
        if (pyobj) {
            if constexpr (std::is_reference<T>::value){
                pyobj->_CObject = &cobj;
            } else if constexpr (std::is_copy_constructible<T_NoRef>::value){
                pyobj->_CObject = new T_NoRef(cobj);//ObjectLifecycleHelpers::Copy<T>::new_copy(cobj);
            } else {
                PyErr_Format(PyExc_RuntimeError, "Attempt to make copy of non-copiable type %s", Types<T>::type_name());
                return nullptr;
            }
        }
        return pyobj;
    }
}
#endif
