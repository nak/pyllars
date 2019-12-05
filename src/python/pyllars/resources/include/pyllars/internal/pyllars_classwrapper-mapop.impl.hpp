//
// Created by jrusnak on 11/10/19.
//

#ifndef PYLLARS_PYLLARS_CLASSWRAPPER_MAPOP_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSWRAPPER_MAPOP_IMPL_HPP
#include "pyllars_classwrapper.hpp"
#include "pyllars_funttraits.hpp"

#include <functional>

namespace pyllars_internal {
    template<typename T>
    template<typename KeyType, typename method_t, method_t method>
    void PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    addMapOperator() {
        typedef typename func_traits<method_t>::ReturnType ValueType;
        if constexpr(func_traits<method_t>::is_const_method) {
            _addMapOperatorMethod <KeyType, ValueType, ValueType(CClass::*)(KeyType) const, method > ();
        } else {
            _addMapOperatorMethod <KeyType, ValueType, ValueType(CClass::*)(KeyType), method > ();
        }
    }

    template<typename T>
    template<typename KeyType, typename ValueType, typename method_t, method_t method>
    void PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    _addMapOperatorMethod() {

        std::function<PyObject *(PyObject *, PyObject *)> getter = [](PyObject *self, PyObject *item) -> PyObject * {
            PythonClassWrapper *self_ = (PythonClassWrapper *) self;
            try {
                auto c_key = pyllars_internal::toCArgument<KeyType>(*item);
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
                            if constexpr(std::is_const<ValueType>::value){
                                PyErr_SetString(PyExc_TypeError, "Cannot call assign to const return value");
                                return -1;
                            } else {
                                //the value here is something we will be assigning TO and NOT FROM.  So make const
                                //in order to avoid type conversion issue as is it not really an argument to a function call
                                typedef typename to_const<ValueType>::type safe_value_type;
                                argument_capture <safe_value_type> c_value = pyllars_internal::template toCArgument<safe_value_type>(
                                        *value);
                                argument_capture <KeyType> c_key = pyllars_internal::template toCArgument<KeyType>(
                                        *item);

                                try {
                                    Assignment<ValueType>::assign((cobj->*method)(c_key.value()), c_value.value());
                                } catch (PyllarsException &e) {
                                    e.raise();
                                    return -1;
                                } catch (std::exception const &e) {
                                    PyllarsException::raise_internal_cpp(e.what());
                                    return -1;
                                } catch (...) {
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
        _Type._mapMethodCollection[name] = std::pair<std::function<PyObject *(PyObject *, PyObject *)>,
                std::function<int(bool, PyObject *, PyObject *, PyObject *)>
        >(getter, setter);

    }


}


#endif //PYLLARS_PYLLARS_CLASSWRAPPER_METHODS_IMPL_HPP
