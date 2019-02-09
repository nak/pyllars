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
    namespace {
        template<typename T, typename E = void>
        struct Address;

    }

    class ObjectLifecycleHelpers;


    template<typename T, int variant = 0>
    struct PythonPointerWrapperBase: public CommonBaseWrapper{
        template<typename T2>
        using ObjectContent = ObjectLifecycleHelpers::template ObjectContent<T2, void>;

        template<typename PtrWrapper>
        friend
        struct ObjectLifecycleHelpers::BasicAlloc;

        template<typename PtrWrapper>
        friend
        struct ObjectLifecycleHelpers::BasicDeallocation;

        template<typename T2, bool is_array, typename ClassWrapper, typename E>
        friend
        class CObjectConversionHelper;


        template<typename T2, typename PtrWrapper, typename E>
        friend
        struct ObjectLifecycleHelpers::Deallocation;

        template<typename T2, typename PtrWrapper, typename E>
        friend
        struct ObjectLifecycleHelpers::Alloc;


        static constexpr ssize_t last = ArraySize<T>::size - 1;

        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
        typedef PythonClassWrapper<T const> ConstWrapper;
        typedef PythonClassWrapper<typename std::remove_const<T>::type> NonConstWrapper;
        typedef PythonClassWrapper<typename std::remove_reference<T>::type> NoRefWrapper;
        typedef PythonClassWrapper<typename std::remove_const<typename std::remove_reference<T>::type>::type> NoRefNonConstWrapper;
        typedef PythonClassWrapper<typename extent_as_pointer<T>::type> AsPtrWrapper;

        static
        std::string get_name(){return Types<T>::type_name();}

        ssize_t getArraySize(){return _arraySize;}

        void setArraySize(const ssize_t size){
            _arraySize = size;
            _max = size < 0 ? -1 : size - 1;
        }

        static PyTypeObject *getPyType(){
            if(initialize(Types<T>::type_name()) != 0){
                return nullptr;
            }
            return &Type;
        }

        static int initialize(const char *const name = nullptr);

        static bool checkType(PyObject *const obj);

        static bool checkTypeDereferenced(PyObject *const obj);

        static PythonClassWrapper<T> *createPy2(const ssize_t arraySize,
                                             T * cobj,
                                             const bool isAllocated,
                                             const bool inPlace,
                                             PyObject *referencing = nullptr);

        static PythonClassWrapper<T> *createPy(const ssize_t arraySize,
                                            ObjContainer<T> *const cobj,
                                            const bool isAllocated,
                                            const bool inPlace,
                                            PyObject *referencing = nullptr);


        PythonPointerWrapperBase():_arraySize(UNKNOWN_SIZE), _max(0), _raw_storage(nullptr){
        }

        T *get_CObject();

    protected:
        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

        static void _dealloc(PyObject *self_);

        static Py_ssize_t _size(PyObject *self);

        static PyObject *_inplace_concat(PyObject *self, PyObject *other) {
            return nullptr;
        }

        static PyObject *_inplace_repeat(PyObject *self, Py_ssize_t count){
            return nullptr;
        }

        static PyObject *_repeat(PyObject *self, Py_ssize_t count){
            return nullptr;
        }

        static PyObject *_concat(PyObject *self, PyObject *other);

        static int _set_item(PyObject *self, Py_ssize_t index, PyObject *obj);

        static PyObject *_get_item(PyObject *self, Py_ssize_t index);

        static PyObject *_get_item2(PyObject *self, Py_ssize_t index, const bool make_copy);

        static PyObject *_at(PyObject *self, PyObject *args, PyObject *kwds);

        static int _contains(PyObject *self, PyObject *obj);

        void set_raw_storage(T_base *const storage, const size_t size);

        void delete_raw_storage();

        static int _init(PythonPointerWrapperBase *self, PyObject *args, PyObject *kwds);

        static int _initbase(PythonPointerWrapperBase *self, PyObject *args, PyObject *kwds, PyTypeObject *pytypeobj);

        static Py_ssize_t get_array_index(PythonPointerWrapperBase *const self, PyObject *args, PyObject *kwargs);

        static PyTypeObject Type;
        static PyMethodDef _methods[];
        static PySequenceMethods _seqmethods;

        T_base *_raw_storage;
        ObjContainer<T> *_CObject;
        PyObject *_referenced_elements;
        ssize_t _arraySize;
        ssize_t _max;

        size_t _raw_size;

    public:
        bool _allocated;
        bool _inPlace;
    };


    template<typename T, typename E=void>
    struct base_type;

    template<typename T>
    struct base_type<T, typename std::enable_if<ptr_depth<T>::value == 1>::type>{
        typedef typename std::remove_pointer<T>::type type;
    };

    template<typename T>
    struct base_type<T, typename std::enable_if<(ptr_depth<T>::value > 1)>::type>{
        typedef typename base_type<typename std::remove_pointer<T>::type>::type type;
    };

    template<typename T>
    struct PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value == 1) >::type> : public PythonPointerWrapperBase<T> {


        static PyObject *_addr(PyObject *self, PyObject *args);

    };


    template<typename T>
    struct PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            !std::is_const<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value > 1) &&
            sizeof(T) == sizeof(typename base_type<T>::type*) >::type> :
            public PythonPointerWrapperBase<typename base_type<T>::type*, 1> {


        static PyObject *_addr(PyObject *self, PyObject *args);

        T* get_CObject(){
            static T* value = (T*)(PythonPointerWrapperBase<typename base_type<T>::type*, 1>::get_CObject());
            return value;
        }

        static
        std::string get_name(){return Types<T>::type_name();}


        static PythonClassWrapper *createPy2(const ssize_t arraySize,
                                                   T * cobj,
                                                   const bool isAllocated,
                                                   const bool inPlace,
                                                   PyObject *referencing = nullptr){
            return (PythonClassWrapper *) PythonPointerWrapperBase<typename base_type<T>::type*, 1>::createPy2(arraySize, (typename base_type<T>::type**) cobj, isAllocated, inPlace, referencing);
        }

        static PythonClassWrapper *createPy(const ssize_t arraySize,
                                                  ObjContainer<T> *const cobj,
                                                  const bool isAllocated,
                                                  const bool inPlace,
                                                  PyObject *referencing = nullptr){
            return (PythonClassWrapper*) PythonPointerWrapperBase<typename base_type<T>::type*, 1>::createPy(arraySize, (ObjContainer<typename base_type<T>::type*>*) cobj, isAllocated, inPlace, referencing);
        }
    };


    template<typename T>
    struct PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            std::is_const<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value > 1) &&
            sizeof(T) == sizeof(typename base_type<T>::type*) >::type> :
            public PythonPointerWrapperBase<const typename base_type<T>::type*, 1> {


        static PyObject *_addr(PyObject *self, PyObject *args);

        T* get_CObject(){
            static T* value = (T*)(PythonPointerWrapperBase<typename base_type<T>::type*, 1>::get_CObject());
            return value;
        }

        static
        std::string get_name(){return Types<T>::type_name();}


        static PythonClassWrapper *createPy2(const ssize_t arraySize,
                                             T * cobj,
                                             const bool isAllocated,
                                             const bool inPlace,
                                             PyObject *referencing = nullptr){
            return (PythonClassWrapper *) PythonPointerWrapperBase<typename base_type<T>::type*, 1>::createPy2(arraySize, (typename base_type<T>::type**) cobj, isAllocated, inPlace, referencing);
        }

        static PythonClassWrapper *createPy(const ssize_t arraySize,
                                            ObjContainer<T> *const cobj,
                                            const bool isAllocated,
                                            const bool inPlace,
                                            PyObject *referencing = nullptr){
            return (PythonClassWrapper*) PythonPointerWrapperBase<typename base_type<T>::type*, 1>::createPy(arraySize, (ObjContainer<typename base_type<T>::type*>*) cobj, isAllocated, inPlace, referencing);
        }
    };

}
#endif
