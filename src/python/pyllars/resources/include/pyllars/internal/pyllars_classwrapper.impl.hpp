#ifndef PYLLARS__CLASSWRAPPER_IMPL
#define PYLLARS__CLASSWRAPPER_IMPL

#include <limits>
#include <string.h>
#include <utility>

#include "pyllars_classwrapper.hpp"
#include "pyllars_defns.hpp"
#include "pyllars_utils.hpp"
#include "pyllars_type_traits.hpp"

#include "pyllars_staticfunctionsemantics.impl.hpp"
#include "pyllars_conversions.impl.hpp"
#include "pyllars_classwrapper-mapop.impl.hpp"
#include "pyllars_classwrapper-methods.impl.hpp"
#include "pyllars_classwrapper-type.impl.hpp"

#include "pyllars_classmembersemantics.hpp"
#include "pyllars_membersemantics.hpp"
#include "pyllars_methodcallsemantics.hpp"

namespace __pyllars_internal {

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
    template<typename Parent>
    status_t
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::ready(){
        int status = 0;
        for (const auto& ready_fnctn: _childrenReadyFunctions()){
            status |= (ready_fnctn() == nullptr);
        }
      
        return status;
    }


    template<typename T>
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type> *
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    fromCObject(T_NoRef &cobj) {
        PyTypeObject *type_ = getPyType();

        if (!type_ || !type_->tp_name) {
            PyErr_SetString(PyExc_RuntimeError, "Uninitialized type when creating object");
            return nullptr;
        }
        PythonClassWrapper *pyobj = (PythonClassWrapper *) PyObject_Call(
                reinterpret_cast<PyObject *>(type_), NULL_ARGS(), nullptr);
        if (pyobj) {
            pyobj->_CObject = new T(cobj);//ObjectLifecycleHelpers::Copy<T>::new_copy(cobj);
        }
        return pyobj;
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
    template<typename Base>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addBaseClass() {
        PyTypeObject * base = PythonClassWrapper<Base>::getPyType();
        if (!base) return;
        _baseClasses().insert(_baseClasses().begin(), base);
        _childrenReadyFunctions().insert(_childrenReadyFunctions().begin(), &PythonClassWrapper<Base>::getPyType);
        auto baseTyp = PythonClassWrapper<Base>::getPyType();
        auto key = std::pair{baseTyp, getPyType()};
        castMap()[key] = &cast<Base>;
    }

    template<typename T>
    bool PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    checkType(PyObject * obj) {
        return PyObject_TypeCheck(obj, &_Type);
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

    template<typename T>
    std::map<std::string, std::pair<std::function<PyObject *(PyObject *, PyObject *)>,
            std::function<int(bool is_const, PyObject *, PyObject *, PyObject *)> >
    >
            PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::_mapMethodCollection;

}

#endif
