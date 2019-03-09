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
#include "pyllars_containment.hpp"
#include "pyllars_object_lifecycle.hpp"

// TODO (jrusnak#1#): All adding of bases, but not through template parameter....

namespace __pyllars_internal {
    namespace {
        template<typename T, typename E = void>
        struct Address;

    }

    class ObjectLifecycleHelpers;


    template <typename T> class remove_all_pointers{
    public:
        typedef T type;
    };

    template <typename T> class remove_all_pointers<T*>{
    public:
        typedef typename remove_all_pointers<T>::type type;
    };

    template <typename T> class remove_all_pointers<T* const>{
    public:
        typedef typename remove_all_pointers<T>::type type;
    };

    template <typename T> class remove_all_pointers<T* volatile>{
    public:
        typedef typename remove_all_pointers<T>::type type;
    };

    template <typename T> class remove_all_pointers<T* const volatile >{
    public:
        typedef typename remove_all_pointers<T>::type type;
    };


    template<typename T>
    struct PythonPointerWrapperBase: public CommonBaseWrapper{

        template<typename T2>
        using ObjectContent = ObjectLifecycleHelpers::template ObjectContent<T2, void>;

        template<typename T2, bool is_array, typename E>
        friend
        class CObjectConversionHelper;

        template<typename Other, typename EE>
        friend class PythonClassWrapper;

        static constexpr ssize_t last = ArraySize<T>::size - 1;

        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
        typedef PythonClassWrapper<T const> ConstWrapper;
        typedef PythonClassWrapper<typename std::remove_const<T>::type> NonConstWrapper;
        typedef PythonClassWrapper<typename std::remove_reference<T>::type> NoRefWrapper;
        typedef PythonClassWrapper<typename std::remove_const<typename std::remove_reference<T>::type>::type> NoRefNonConstWrapper;
        typedef PythonClassWrapper<typename extent_as_pointer<T>::type> AsPtrWrapper;

        PythonPointerWrapperBase():_arraySize(UNKNOWN_SIZE){
        }


    protected:



        static PythonPointerWrapperBase *_createPy(PyTypeObject & Type,
                                                   const size_t arraySize,
                                                   T& cobj,
                                                   const ContainmentKind containmentKind,
                                                   PyObject *referencing = nullptr);

        static int _initialize(PyTypeObject & Type);

        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

        static void _dealloc(PyObject *self_);

        static void _free(PythonPointerWrapperBase *self);

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

        static PyObject *_at(PyObject *self, PyObject *args, PyObject *kwds);

        static int _contains(PyObject *self, PyObject *obj);

        static int _initbase(PythonPointerWrapperBase *self, PyObject *args, PyObject *kwds, PyTypeObject *pytypeobj);

        static Py_ssize_t get_array_index(PythonPointerWrapperBase *const self, PyObject *args, PyObject *kwargs);

        static PySequenceMethods _seqmethods;

        T *_get_CObject();

        ObjectContainer<T> *_CObject;
        PyObject *_referenced_elements;
        ssize_t _arraySize;
        size_t _max;
        size_t _depth; //only used for classes with pointer depth > 1, kept here for consistent PyType layout
        bool _allocated;
    private:
        T* get_CObject(){
            return _CObject?_CObject->ptr():nullptr;
        }
    };


    template<typename T, typename E=void>
    struct base_type;

    template<typename T>
    struct base_type<T, typename std::enable_if<ptr_depth<T>::value == 1>::type>{
        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type type;
    };

    template<typename T>
    struct base_type<T, typename std::enable_if<(ptr_depth<T>::value > 1)>::type>{
        typedef typename base_type<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::type type;
    };

    template<typename T>
    struct PythonClassWrapper<T, typename std::enable_if<
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value > 1) &&
            sizeof(typename extent_as_pointer<T>::type) == sizeof(typename base_type<T>::type*) >::type> :
        public PythonPointerWrapperBase<T> {
        typedef PythonPointerWrapperBase<T> Base;
        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
        typedef PythonClassWrapper<T const> ConstWrapper;
        typedef PythonClassWrapper<typename std::remove_const<T>::type> NonConstWrapper;
        typedef PythonClassWrapper<typename std::remove_reference<T>::type> NoRefWrapper;
        typedef PythonClassWrapper<typename std::remove_const<typename std::remove_reference<T>::type>::type> NoRefNonConstWrapper;
        typedef PythonClassWrapper<typename extent_as_pointer<T>::type> AsPtrWrapper;

        template<typename Other>
        friend class PythonPointerWrapperBase;

        PythonClassWrapper():PythonPointerWrapperBase<T>(){
           Base::_depth = ptr_depth<T>::value;
        }

        T* get_CObject(){
            T* value = Base::_get_CObject();
            return value;
        }

        void setFrom(PythonClassWrapper *obj){
            if(!obj){
               obj->_free();
            }
            Py_INCREF(obj);
        }

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &Type;
        }

        static int initialize(){return Base::_initialize(Type);}

        static bool checkType(PyObject *const obj);


        static PythonClassWrapper *createPy(const ssize_t arraySize,
                                            T &cobj,
                                            const ContainmentKind containmentKind,
                                            PyObject *referencing = nullptr){
            return reinterpret_cast<PythonClassWrapper*>(Base::_createPy(Type, arraySize, cobj,containmentKind, referencing));
        }

    protected:
        static PyObject *_addr(PyObject *self, PyObject *args);

        static int _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds);
        static PyTypeObject Type;
        static PyMethodDef _methods[];
    };


    template<typename T>
    struct PythonClassWrapper<T, typename std::enable_if<//!std::is_pointer<typename std::remove_pointer<T>::type>::value &&
            !std::is_function<typename std::remove_pointer<T>::type>::value &&
            (std::is_pointer<T>::value || std::is_array<T>::value) &&
            (ptr_depth<T>::value == 1) >::type> : public PythonPointerWrapperBase<T> {

        typedef PythonPointerWrapperBase<T>  Base;
        T* get_CObject(){
            return PythonPointerWrapperBase<T>::_get_CObject();
        }

        void setFrom(T & value){
            if (Base::_referenced) Py_XDECREF(Base::_referenced);
            Base::_referenced = nullptr;
            Base::_CObject = new ObjectContainer<T>(value);
        }

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &Type;
        }

        static int initialize(){return Base::_initialize(Type);}

        static bool checkType(PyObject *const obj);

        static PythonClassWrapper *createPy(const ssize_t arraySize,
                                            T & cobj,
                                            const ContainmentKind containmentKind,
                                            PyObject *referencing = nullptr){
            return reinterpret_cast<PythonClassWrapper*>(Base::_createPy(Type, arraySize, cobj, containmentKind, referencing));
        }

    protected:
        static PyObject *_addr(PyObject *self_, PyObject *args);
        static int _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds);
        static PyTypeObject Type;
        static PyMethodDef _methods[];

     private:
        struct Iter{
            PyObject_HEAD
            PythonClassWrapper* obj;
            size_t max;
            size_t i;
            static const std::string name;
            static PyTypeObject Type;

            static PyObject * iter(PyObject* self);
            static PyObject * iternext(PyObject* self);
        };
    };


}
#endif
