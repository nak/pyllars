#ifndef __PYLLARS__TEMPLATES__H_
#define __PYLLARS__TEMPLATES__H_

#include <map>
#include <vector>
#include <Python.h>
#include "pyllars_classwrapper.hpp"



template<>
struct std::hash<PyObject*> {
    auto operator() (PyObject* const &key) const {
        std::hash<void*> hasher;
        size_t result = 0;
        size_t h;
        using namespace __pyllars_internal;
        if (PyTuple_Check(key)) {
            for (ssize_t i = 0; i < PyTuple_Size(key); ++i) {
                auto item = PyTuple_GetItem(key, i);

                if (PyType_Check(item)) {
                    h = hasher((void *) item);
                } else if (PyObject_TypeCheck(item, __pyllars_internal::CommonBaseWrapper::getPyType())) {
                    if (!reinterpret_cast<CommonBaseWrapper *>(item)->hash) {
                        throw PyllarsException(PyExc_SystemError, "System Error: null hash function encountered");
                    }
                    h = reinterpret_cast<CommonBaseWrapper *>(item)->hash((CommonBaseWrapper *) item);
                }
                result = (result << 1) ^ h;
            }
        } else {
            auto item = key;

            if (PyType_Check(item)) {
                h = hasher((void *) item);
            } else if (PyObject_TypeCheck(item, __pyllars_internal::CommonBaseWrapper::getPyType())) {
                if (!reinterpret_cast<CommonBaseWrapper *>(item)->hash) {
                    throw PyllarsException(PyExc_SystemError, "System Error: null hash function encountered");
                }
                h = reinterpret_cast<CommonBaseWrapper *>(item)->hash((CommonBaseWrapper *) item);
            }
            result = (result << 1) ^ h;
        }
        return result;
    }
};

namespace __pyllars_internal{




    template<typename ...Args>
    struct BareArgBundle {
        static PyObject** params() {
                static PyObject* p[sizeof...(Args) + 1] = {(PyObject*)PythonClassWrapper<Args>::getPyType()..., nullptr};
                return p;
        }

        static constexpr ssize_t size = sizeof...(Args);
    };

    template <typename T, T value, typename ...Args>
    struct ArgBundle{
        static PyObject** params(){
                PyObject * p[sizeof...(Args) + 2] = {toPyObject<T>(value), (PyObject*) PythonClassWrapper<Args>::getPyType()..., nullptr};
                return p;
        }
        static constexpr ssize_t size = sizeof...(Args) + 1;

    };

    struct TemplateDict{
        PyObject_HEAD

        static int _init(TemplateDict* self, PyObject* , PyObject*){
        }

        static PyObject* _new(PyObject* type_, PyObject*, PyObject*){
            PyTypeObject * type = (PyTypeObject*) type_;
            TemplateDict* self = (TemplateDict*)type->tp_alloc(type, 0);
            self->dictionary = new   std::unordered_map<PyObject*, PyObject*, std::hash<PyObject*>, TemplateDict::EqualTo>();
        }

        static Py_ssize_t _mp_lengh(TemplateDict* self){
            return self->dictionary->size();
        }

        static PyObject* _mp_subscript(TemplateDict* self, PyObject* tuple_key){
            if (!PyTuple_Check(tuple_key)){
                PyObject* tuple = PyTuple_New(1);
                PyTuple_SetItem(tuple, 0, tuple_key);
                Py_INCREF(tuple_key);  //Tuple steals a reference
                auto item = (*self->dictionary)[tuple];
                Py_DECREF(tuple);
                return item;
            } else {
                return (*self->dictionary)[tuple_key];
            }
        }

        static int _mp_ass_subscript(TemplateDict *self, PyObject* key, PyObject* value){
            (*self->dictionary)[key] = value;
            return 0;
        }

        struct EqualTo {
             bool operator()(PyObject * const &item1, PyObject * const &item2) const{
                 return compare(item1, item2);
             }

            bool compare(PyObject * const &item1, PyObject * const &item2) const{
                if (!item1 || !item2) {
                    return false;
                }
                if PyTuple_Check(item1) {
                    if (PyTuple_Check(item2) && PyTuple_Size(item2) == PyTuple_Size(item1)) {
                        for (ssize_t i = 0; i < PyTuple_Size(item2); ++i) {
                            auto subitem1 = PyTuple_GetItem(item1, i);
                            auto subitem2 = PyTuple_GetItem(item2, i);
                            if (!compare(subitem1, subitem2))
                            {
                                return false;
                            }
                        }
                        return true;
                    }
                    return false;
                } else if (PyType_Check(item1)) {
                    return item1 == item2;
                } else if (!PyObject_TypeCheck(item1, CommonBaseWrapper::getPyType()) ||
                           !PyObject_TypeCheck(item2, CommonBaseWrapper::getPyType())) {
                    return false;
                } else {
                    return ((CommonBaseWrapper *) item1)->compare((CommonBaseWrapper *) item1,
                                                                  (CommonBaseWrapper *) item2);
                }
                return false;
            }
        };

        static PyTypeObject _Type;

    private:
        std::unordered_map<PyObject *, PyObject *, std::hash<PyObject *>, TemplateDict::EqualTo> *dictionary;

    };

    template <const char* const template_name, typename T>
    struct TemplateClassInstantiation{


        template <typename ...ArgBundleT>
        struct Params{

            static int  instantiate(){
                static auto empty_args = PyTuple_New(0);
                if(!TemplateClassInstantiation::_dictionary){
                    if(PyType_Ready(&TemplateDict::_Type) != 0){
                        PyErr_SetString(PyExc_RuntimeError, "Unable to initialize TemplateDict Python Type");
                        return -1;
                    }
                    TemplateClassInstantiation::_dictionary = PyObject_Call((PyObject*)&TemplateDict::_Type, empty_args, nullptr);
                    if (!TemplateClassInstantiation::_dictionary){
                        PyErr_SetString(PyExc_RuntimeError, "Unable to initialize TemplateDict Python object");
                        return -1;
                    }
                }

                ssize_t sizes[sizeof...(ArgBundleT)] = {ArgBundleT::size...};
                PyObject * *params[sizeof...(ArgBundleT)] = {ArgBundleT::params()...};
                ssize_t size = 0;
                for (ssize_t i = 0; i < sizeof...(ArgBundleT); ++i) {
                    size += sizes[i];
                }
                PyObject * tuple = PyTuple_New(size);
                ssize_t index = 0;
                for (ssize_t i = 0; i < sizeof...(ArgBundleT); ++i) {
                    PyObject** obj = params[i];
                    while (*obj) {
                        PyTuple_SetItem(tuple, index++, *(obj++));
                    }
                }
                return PyObject_SetItem(TemplateClassInstantiation::_dictionary, tuple, (PyObject*)PythonClassWrapper<T>::getPyType());
            }
        };

        static PyObject* addDefinition(PyObject* module){
            if (module) {
                if(PyModule_AddObject(module, template_name, TemplateClassInstantiation::_dictionary) == 0){
                    return TemplateClassInstantiation::_dictionary;
                } else {
                    return nullptr;
                }
            }
            return TemplateClassInstantiation::_dictionary;
        }

    private:

        static PyObject* _dictionary;

    };

    template< const char* const template_name, typename T>
    PyObject* TemplateClassInstantiation<template_name, T>::_dictionary  = nullptr;

}


#endif