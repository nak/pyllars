#ifndef __PYLLARS__INTERNAL__POINTER_H
#define __PYLLARS__INTERNAL__POINTER_H

#include <type_traits>

#include <limits>
#include <vector>
#include <functional>

#include <Python.h>
#include <structmember.h>
#include <limits.h>
#include <sys/types.h>

#include "pyllars_classwrapper.hpp"
#include "pyllars_object_lifecycle.hpp"

// TODO (jrusnak#1#): All adding of bases, but not through template parameter....

namespace __pyllars_internal {

    class ObjectLifecycleHelpers;

    template<typename T>
    struct PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value)>::type> :
            public CommonBaseWrapper {

        static constexpr ssize_t last = ArraySize<T>::size - 1;

        static PyObject *parent_module;

        template<typename T2>
        using ObjectContent = ObjectLifecycleHelpers::template ObjectContent<T2, void>;

        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
        typedef PythonClassWrapper<T const> ConstWrapper;
        typedef PythonClassWrapper<typename std::remove_const<T>::type> NonConstWrapper;
        typedef PythonClassWrapper<typename std::remove_reference<T>::type> NoRefWrapper;
        typedef PythonClassWrapper<typename std::remove_const<typename std::remove_reference<T>::type>::type> NoRefNonConstWrapper;
        typedef PythonClassWrapper<typename extent_as_pointer<T>::type> AsPtrWrapper;

        static Py_ssize_t _size(PyObject *self);

        static PyObject *_inplace_concat(PyObject *self, PyObject *other) {
            return nullptr;
        }


        ssize_t getArraySize();

        void setArraySize(const ssize_t size);

        template<typename T2, bool is_array, typename ClassWrapper, typename E>
        friend
        class CObjectConversionHelper;

        static std::string get_name(const size_t depth = 1);

        static std::string get_full_name(const size_t depth = 1) { return _full_name; }

        static std::string get_module_entry_name(const size_t depth = 1);

        static int initialize(const size_t depth = ptr_depth<T>::value);

        static int initialize(const char *const name, const char *const module_entry_name,
                              PyObject *module, const char *const fullname = nullptr,
                              const size_t depth = ptr_depth<T>::value);


        static PythonClassWrapper *createPy2(const ssize_t arraySize,
                                             T *const cobj,
                                             const bool isAllocated,
                                             const bool inPlace,
                                             PyObject *referencing = nullptr, const size_t depth = ptr_depth<T>::value);

        static PythonClassWrapper *createPy(const ssize_t arraySize,
                                            ObjContainer<T> *const cobj,
                                            const bool isAllocated,
                                            const bool inPlace,
                                            PyObject *referencing = nullptr, const size_t depth = ptr_depth<T>::value);


        static int _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds);

        static int _initbase(PythonClassWrapper *self, PyObject *args, PyObject *kwds, PyTypeObject *pytypeobj);

        T *get_CObject();

        static PyObject *_addr(PyObject *self, PyObject *args);

        static void _dealloc(PyObject *self_);

        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

        static bool checkType(PyObject *const obj);

        static bool checkTypeDereferenced(PyObject *const obj);

        static PyTypeObject *getType(const size_t depth = 1,
                                     const char *const fullname = nullptr);

        void set_raw_storage(T_base *const storage, const size_t size);

        void delete_raw_storage();


        // FREIND DEFINITIONS //


        template<typename PtrWrapper>
        friend
        struct ObjectLifecycleHelpers::BasicAlloc;

        template<typename PtrWrapper>
        friend
        struct ObjectLifecycleHelpers::BasicDeallocation;

        template<typename T2, typename PtrWrapper, typename E>
        friend
        struct ObjectLifecycleHelpers::Deallocation;

    protected:

        static Py_ssize_t get_array_index(PythonClassWrapper *const self, PyObject *args, PyObject *kwargs);

        static PyMethodDef _methods[];
        static PySequenceMethods _seqmethods;
        static std::string _full_name;
        ObjContainer<T> *_CObject;
        //use objcontainer only for compatability with non-pointer defn
        T_base *_raw_storage;
        PyObject *_referenced_elements;
        ssize_t _max;
        ssize_t _arraySize;
        size_t _raw_size;

    private:
        static PyObject *_concat(PyObject *self, PyObject *other);

        static PyObject *_inplace_repeat(PyObject *self, Py_ssize_t count);

        static PyObject *_repeat(PyObject *self, Py_ssize_t count);

        static int _set_item(PyObject *self, Py_ssize_t index, PyObject *obj);

        static int _contains(PyObject *self, PyObject *obj);

        static PyObject *_at(PyObject *self, PyObject *args, PyObject *kwds);

        static PyObject *_get_item(PyObject *self, Py_ssize_t index);

        static PyObject *_get_item2(PyObject *self, Py_ssize_t index, const bool make_copy);

    public:
        size_t _depth;
        bool _allocated;
        bool _inPlace;
    };


}
#endif
