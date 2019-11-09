//
// Created by jrusnak on 10/13/19.
//
#include "pyllars/internal/pyllars_classwrapper.hpp"

#ifndef PYLLARS_PYLLARS_CLASSMAPOPERATOR_HPP
#define PYLLARS_PYLLARS_CLASSMAPOPERATOR_HPP

namespace pyllars{
    template<typename method_t, method_t method>
    class PyllarsClassMapOperator;

    template<typename Class, typename ValueType, typename KeyType, ValueType (Class::*method)(KeyType)>
    class PyllarsClassMapOperator<ValueType (Class::*)(KeyType), method>{
    private:
        class Initializer{
        public:
            Initializer(){
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace __pyllars_internal;
                typedef ValueType (Class::*method_t)(KeyType);
                PythonClassWrapper<Class>::template addMapOperator<KeyType, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;

    };


    template<typename Class, typename ValueType, typename KeyType, ValueType (Class::*method)(KeyType)>
    typename PyllarsClassMapOperator<ValueType (Class::*)(KeyType), method>::Initializer * const
            PyllarsClassMapOperator<ValueType (Class::*)(KeyType), method>::initializer  = new
                    PyllarsClassMapOperator<ValueType (Class::*)(KeyType), method>::Initializer();

    template<typename Class, typename ValueType, typename KeyType, ValueType (Class::*method)(KeyType) const>
    class PyllarsClassMapOperator<ValueType (Class::*)(KeyType) const, method>{
    private:
        class Initializer{
        public:
            Initializer(){
                __pyllars_internal::Init::registerInit(init);
            }

            static status_t init(){
                using namespace __pyllars_internal;
                typedef ValueType (Class::*method_t)(KeyType) const;
                PythonClassWrapper<Class>::template addMapOperator<KeyType, method_t, method>();
                return 0;
            }
        };

        static Initializer* const initializer;

    };


    template<typename Class, typename ValueType, typename KeyType, ValueType (Class::*method)(KeyType) const>
    typename PyllarsClassMapOperator<ValueType (Class::*)(KeyType) const, method>::Initializer * const
            PyllarsClassMapOperator<ValueType (Class::*)(KeyType) const, method>::initializer  = new
                    PyllarsClassMapOperator<ValueType (Class::*)(KeyType) const, method>::Initializer();
}

#include <functional>

namespace __pyllars_internal {


    template<typename T>
    template<typename KeyType, typename ValueType, typename method_t, method_t method>
    void PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    _addMapOperatorMethod() {
        constexpr bool method_is_const = func_traits<method_t>::is_const_method;
        std::function<PyObject *(PyObject *, PyObject *)> getter = [](PyObject *self, PyObject *item) -> PyObject * {
            PythonClassWrapper *self_ = (PythonClassWrapper *) self;
            try {
                auto c_key = __pyllars_internal::toCArgument<KeyType>(*item);
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
                            //the value here is something we will be assigning TO and NOT FROM.  So make const
                            //in order to avoid type conversion issue as is it not really an argument to a function call
                            typedef typename to_const<ValueType>::type safe_value_type;
                            argument_capture<safe_value_type > c_value = __pyllars_internal::template toCArgument<safe_value_type >(*value);
                            argument_capture<KeyType> c_key = __pyllars_internal::template toCArgument<KeyType>(*item);
                            if (obj_is_const){
                                if constexpr(method_is_const) {
                                    //mutable fields are still settable against const-ness of owning object
                                    //NOTE: we re-use this std::function for PythonClassWrapper<const T>, so need
                                    //   to get const bool to determine if this really should be a const-C object
                                    auto const_cobj = reinterpret_cast<const T_NoRef *>(cobj);
                                    try {
                                        Assignment<ValueType>::assign((const_cobj->*method)(c_key.value()), c_value.value());
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
                                } else {

                                    PyErr_SetString(PyExc_TypeError, "Cannot call non-const method with const this");
                                    return -1;
                                }
                            } else {
                                try{
                                    Assignment<ValueType>::assign((cobj->*method)(c_key.value()), c_value.value());
                                } catch (PyllarsException &e){
                                    e.raise();
                                    return -1;
                                } catch(std::exception const & e) {
                                    PyllarsException::raise_internal_cpp(e.what());
                                    return -1;
                                } catch (...){
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
        _mapMethodCollection[name] = std::pair<std::function<PyObject *(PyObject *, PyObject *)>,
                std::function<int(bool, PyObject *, PyObject *, PyObject *)>
        >(getter, setter);

    }



}

#endif
