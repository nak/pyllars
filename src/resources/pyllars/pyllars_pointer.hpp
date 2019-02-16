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


    template<typename T>
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
        std::string get_name(){return std::string(type_name<T>());}

        ssize_t getArraySize(){return _arraySize;}

        void setArraySize(const ssize_t size){
            _arraySize = size;
            _max = size < 0 ? -1 : size - 1;
        }

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &Type;
        }

        static int initialize();

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

        T *_get_CObject();

        T_base *_raw_storage;
        ObjContainer<T> *_CObject;
        PyObject *_referenced_elements;
        ssize_t _arraySize;
        ssize_t _max;

        size_t _raw_size;

    public:
        bool _allocated;
        bool _inPlace;
        size_t _depth; //only used for classes with pointer depth > 1, kept here for consistent PyType layout

    private:
        T* get_CObject();
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


    struct _GenericDeepPointerWrapper: public PythonPointerWrapperBase<void**>{

        static PyObject *_addr(PyObject *self, PyObject *args);

        _GenericDeepPointerWrapper(const size_t depth){
            assert(depth > 1);
            _depth = depth;
        }
    private:
        void*** get_CObject();
    };


    template<typename T>
    struct PythonClassWrapper<T,typename std::enable_if<
        !std::is_function<typename std::remove_pointer<T>::type>::value &&
        !std::is_const<typename std::remove_pointer<T>::type>::value &&
        (std::is_pointer<T>::value || std::is_array<T>::value) &&
        (ptr_depth<T>::value > 1) &&
        sizeof(T) == sizeof(typename base_type<T>::type*) >::type> :public _GenericDeepPointerWrapper {

        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
        typedef PythonClassWrapper<T const> ConstWrapper;
        typedef PythonClassWrapper<typename std::remove_const<T>::type> NonConstWrapper;
        typedef PythonClassWrapper<typename std::remove_reference<T>::type> NoRefWrapper;
        typedef PythonClassWrapper<typename std::remove_const<typename std::remove_reference<T>::type>::type> NoRefNonConstWrapper;
        typedef PythonClassWrapper<typename extent_as_pointer<T>::type> AsPtrWrapper;

        static PyObject *_addr(PyObject *self, PyObject *args){
            return _GenericDeepPointerWrapper::_addr(self, args);
        }

        PythonClassWrapper():_GenericDeepPointerWrapper(ptr_depth<T>::value){
        }

        T* get_CObject(){
            static T* value = (T*)(_GenericDeepPointerWrapper::_get_CObject());
            return value;
        }

        static
        std::string get_name(){return std::string(type_name<T>());}


        static PythonClassWrapper *createPy2(const ssize_t arraySize,
                                                   T * cobj,
                                                   const bool isAllocated,
                                                   const bool inPlace,
                                                   PyObject *referencing = nullptr){
            return (PythonClassWrapper *) _GenericDeepPointerWrapper::createPy2(arraySize, (void***) cobj, isAllocated, inPlace, referencing);
        }

        static PythonClassWrapper *createPy(const ssize_t arraySize,
                                                  ObjContainer<T> *const cobj,
                                                  const bool isAllocated,
                                                  const bool inPlace,
                                                  PyObject *referencing = nullptr){
            return (PythonClassWrapper*) _GenericDeepPointerWrapper::createPy(arraySize, (ObjContainer<void**>*) cobj, isAllocated, inPlace, referencing);
        }
    };


    struct _GenericDeepPointerConstWrapper: public PythonPointerWrapperBase<const void**>{

        static PyObject *_addr(PyObject *self, PyObject *args);

        _GenericDeepPointerConstWrapper(const size_t depth){
            assert(depth > 1);
            _depth = depth;
        }

    private:
        void*** get_CObject();
    };

    template<typename T>
    struct PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            std::is_const<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value > 1) &&
            sizeof(T) == sizeof(typename base_type<T>::type*) >::type> :
            public _GenericDeepPointerConstWrapper {

        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
        typedef PythonClassWrapper<T const> ConstWrapper;
        typedef PythonClassWrapper<typename std::remove_const<T>::type> NonConstWrapper;
        typedef PythonClassWrapper<typename std::remove_reference<T>::type> NoRefWrapper;
        typedef PythonClassWrapper<typename std::remove_const<typename std::remove_reference<T>::type>::type> NoRefNonConstWrapper;
        typedef PythonClassWrapper<typename extent_as_pointer<T>::type> AsPtrWrapper;

        static PyObject *_addr(PyObject *self, PyObject *args){
            return _GenericDeepPointerConstWrapper::_addr(self, args);
        }

        PythonClassWrapper():_GenericDeepPointerConstWrapper(ptr_depth<T>::value){
        }

        T* get_CObject(){
            static T* value = (T*)(_GenericDeepPointerConstWrapper::_get_CObject());
            return value;
        }

        static
        std::string get_name(){return std::string(type_name<T>());}


        static PythonClassWrapper *createPy2(const ssize_t arraySize,
                                             T * cobj,
                                             const bool isAllocated,
                                             const bool inPlace,
                                             PyObject *referencing = nullptr){
            return (PythonClassWrapper *) _GenericDeepPointerConstWrapper::createPy2(arraySize, (const void***) cobj, isAllocated, inPlace, referencing);
        }

        static PythonClassWrapper *createPy(const ssize_t arraySize,
                                            ObjContainer<T> *const cobj,
                                            const bool isAllocated,
                                            const bool inPlace,
                                            PyObject *referencing = nullptr){
            return (PythonClassWrapper*) _GenericDeepPointerConstWrapper::createPy(arraySize, (ObjContainer<const void**>*) cobj, isAllocated, inPlace, referencing);
        }
    };


    template<typename T>
    struct PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value == 1) >::type> : public PythonPointerWrapperBase<T> {


        T* get_CObject(){
            return PythonPointerWrapperBase<T>::_get_CObject();
        }

        static PyObject *_addr(PyObject *self_, PyObject *args){
            PythonClassWrapper* self = (PythonClassWrapper*)self_;
            if (!self->_CObject || !*self->_CObject) {
                PyErr_SetString(PyExc_RuntimeError, "Cannot take address of null pointer!");
                return nullptr;
            }
            try {
                if (std::is_const<typename std::remove_pointer<T>::type>::value) {
                    const void **obj = (const void **) (self->_CObject->ptr());
                    _GenericDeepPointerConstWrapper *pyobj = reinterpret_cast<_GenericDeepPointerConstWrapper *>(
                            _GenericDeepPointerConstWrapper::createPy2(1, (const void***) &obj, false, false, (PyObject *) self));
                    pyobj->_depth = self->_depth + 1;
                    return reinterpret_cast<PyObject *>(pyobj);
                } else {
                    const void **obj = (const void **) (self->_CObject->ptr());
                    _GenericDeepPointerWrapper *pyobj = reinterpret_cast<_GenericDeepPointerWrapper *>(
                            _GenericDeepPointerWrapper::createPy2(1, (void***) &obj, false, false, (PyObject *) self));
                    pyobj->_depth = self->_depth + 1;
                    return reinterpret_cast<PyObject *>(pyobj);
                }
            } catch (const char *const msg) {
                PyErr_SetString(PyExc_RuntimeError, msg);
                return nullptr;
            }
        }
    };


}
#endif
