//
// Created by jrusnak on 7/22/16.
//

#ifndef __PYLLARS_INTERNAL__POINTER_CAI_CPP_
#define __PYLLARS_INTERNAL__POINTER_CAI_CPP_

#include "pyllars_pointer.hpp"

namespace pyllars_internal{

    template<typename T>
    template<typename ...Args>
    PyObject*
    Pointers<T>::createAllocatedInstance(Args... args,  ssize_t arraySize) {
        static_assert(is_pointer_like<T>::value);
        typedef std::remove_pointer_t <typename extent_as_pointer<T>::type> T_element;
        if constexpr (!std::is_constructible<T_element, Args...>::value){
            PyErr_SetString(PyExc_TypeError, "Type is not constructible base on provided arguments");
            return nullptr;
        } else {
            if (ArraySize<T>::size > 0 && arraySize != -1) {
                PyErr_SetString(PyExc_SystemError, "Attempt to allocate dynamically sized array of fixed-sized-array types");
                return nullptr;
            } else if (ArraySize<T>::size > 0) {
                arraySize = ArraySize<T>::size;
            }

            static PyObject *pyargs = PyTuple_New(0);
            static PyObject *kwds = PyDict_New();
            auto *pyobj = (PythonClassWrapper<T> *) PyObject_Call((PyObject *) PythonClassWrapper<T>::getPyType(), pyargs, kwds);
            if (!pyobj) {
                PyErr_SetString(PyExc_RuntimeError, "Unable to create Python Object");
                return nullptr;
            }
            assert(pyobj->get_CObject() == nullptr);
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
            if (arraySize >= 0) {
                try {
                    typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
                    if constexpr (std::is_volatile<T>::value || std::is_volatile<T_element>::value){
                        PyErr_SetString(PyExc_TypeError, "Unable to allocate array of volatile instances");
                        return nullptr;
                    } else {
                        pyobj->set_CObject (new T());
                        auto  * cobj =  new (T_element*)(Constructor<T_element>::template allocate_array<Args...>((size_t) arraySize,
                                args...));
                        pyobj->set_CObject(cobj);
                    }
                } catch (PyllarsException &e) {
                    e.raise();
                    return nullptr;
                } catch(std::exception const & e) {
                    PyllarsException::raise_internal_cpp(e.what());
                    return nullptr;
                } catch(...) {
                    PyllarsException::raise_internal_cpp();
                    return nullptr;
                }
            } else {
                pyobj->set_CObject(nullptr);
                T_element *new_value = (T_element*) new T_element(args...);
                pyobj->set_CObject(&new_value, arraySize -1);
            }
            return (PyObject*) pyobj;
        }
    }



}

#endif
