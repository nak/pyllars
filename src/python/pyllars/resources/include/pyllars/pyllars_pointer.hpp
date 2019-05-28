#ifndef __PYLLARS__INTERNAL__POINTER_H
#define __PYLLARS__INTERNAL__POINTER_H

#include <type_traits>

#include <limits>
#include <vector>
#include <functional>

#include <Python.h>
#include <structmember.h>
#include <climits>
#include <sys/types.h>

#include "pyllars_classwrapper.hpp"
#include "pyllars_containment.hpp"
#include "pyllars_object_lifecycle.hpp"


namespace __pyllars_internal {
    namespace {
        template<typename T, typename E = void>
        struct Address;

    }
    extern PyTypeObject BasePtrType;

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

        template<typename Other, typename EE>
        friend class PythonClassWrapper;

        static constexpr ssize_t last = ArraySize<T>::size - 1;

        typedef typename std::remove_pointer<typename extent_as_pointer<typename std::remove_reference<T>::type>::type>::type T_base;
        typedef typename std::remove_reference<T>::type T_NoRef;

        PythonPointerWrapperBase():_max(UNKNOWN_SIZE){}

    protected:

        static PythonPointerWrapperBase *_createPyReference(PyTypeObject & Type,
                                                             T_NoRef* cobj,
                                                            const ssize_t arraySize,
                                                            PyObject *referencing = nullptr,
                                                            unsigned char* byte_bucket = nullptr);

        template<typename ...Args>
        static PythonPointerWrapperBase *_createPyFromAllocatedInstance(PyTypeObject & Type,
                                                                        Args ...args,
                                                                        ssize_t arraySize = -1);

        static int _initialize(PyTypeObject & Type);

        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

        static void _dealloc(PyObject *self_);

        static void _free(void *self);

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

        T_NoRef *_get_CObject();

        T_NoRef *_CObject;
        unsigned char* _byte_bucket;
        PyObject *_referenced_elements{};
        //ssize_t _arraySize;
        ssize_t _max;
        size_t _depth{}; //only used for classes with pointer depth > 1, kept here for consistent PyType layout
    private:
        T_NoRef* get_CObject(){
            return _CObject;
        }

        static PySequenceMethods _seqmethods;
    };

    template<typename T>
    struct PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value > 1)>::type> :
        public PythonPointerWrapperBase<T> {

        typedef PythonPointerWrapperBase<T> Base;
        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

        typedef PythonClassWrapper<T const> ConstWrapper;
        typedef PythonClassWrapper<typename std::remove_const<T>::type> NonConstWrapper;
        typedef typename std::remove_reference<T>::type T_NoRef;
        template<typename Other>
        friend class PythonPointerWrapperBase;

        PythonClassWrapper():PythonPointerWrapperBase<T>(){
           Base::_depth = ptr_depth<T>::value;
        }

        T_NoRef * get_CObject(){
            T_NoRef * value = Base::_get_CObject();
            return value;
        }

        //void setFrom(PythonClassWrapper *obj){
        //    if(!obj){obj->_free();}
        //    Py_INCREF(obj);
        //}

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &Type;
        }

        static int initialize(){return Base::_initialize(Type);}

        static bool checkType(PyObject *obj);


        static PythonClassWrapper *fromCPointer(T& cobj, const ssize_t arraySize, PyObject *referencing = nullptr, unsigned char* byte_bucket = nullptr){
            return reinterpret_cast<PythonClassWrapper*>(Base::_createPyReference(Type, &cobj, arraySize, referencing, byte_bucket));
        }

        template<typename ...Args>
        static PythonClassWrapper *allocateInstance(Args... args){

            return (PythonClassWrapper*) Base::_createPyFromAllocatedInstance(Type, args...,  -1);
        }

        template<typename ...Args>
        static PythonClassWrapper *allocateArray( Args ...args, const ssize_t arraySize) {
            return reinterpret_cast<PythonClassWrapper *>(
                    Base::_createPyFromAllocatedInstance(Type, args..., arraySize));
        }

        static PythonClassWrapper *fromInPlaceAllocation( const ssize_t arraySize, unsigned char* byte_bucket){
            return reinterpret_cast<PythonClassWrapper*>(Base::_createPyReference(Type, nullptr, arraySize, nullptr, byte_bucket));
        }

    protected:
        PythonClassWrapper *createPyReferenceToAddr();
        static PyObject *_addr(PyObject *self, PyObject *args);

        static int _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds);
        static PyTypeObject Type;
        static PyMethodDef _methods[];
    };


    template<typename T>
    struct PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value == 1) >::type> : public PythonPointerWrapperBase<T> {
    public:
        typedef PythonPointerWrapperBase<T>  Base;
        typedef typename std::remove_reference<T>::type T_NoRef;

        T_NoRef* get_CObject(){
            return PythonPointerWrapperBase<T>::_get_CObject();
        }

        void setFrom(T & value){
            if (Base::_referenced) Py_XDECREF(Base::_referenced);
            Base::_referenced = nullptr;
            Base::_CObject = ObjectLifecycleHelpers::Copy<T_NoRef>::new_copy(value);
        }

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &_Type;
        }

        static int initialize(){return Base::_initialize(_Type);}

        static bool checkType(PyObject *obj);

        static PythonClassWrapper *fromCPointer( T & cobj, const ssize_t arraySize, PyObject *referencing = nullptr){
            return reinterpret_cast<PythonClassWrapper*>(Base::_createPyReference(_Type, &cobj, arraySize, referencing, nullptr));
        }

        static PythonClassWrapper *fromInPlaceAllocation( const ssize_t arraySize, unsigned char* byte_bucket){
            return reinterpret_cast<PythonClassWrapper*>(Base::_createPyReference(_Type, nullptr, arraySize, nullptr, byte_bucket));
        }

        template<typename ...Args>
        static PythonClassWrapper *allocateInstance(Args... args){

            return (PythonClassWrapper*) Base::template _createPyFromAllocatedInstance<Args...>(_Type, args...,  -1);
        }

        template<typename ...Args>
        static PythonClassWrapper *allocateArray( Args ...args, const ssize_t arraySize) {
            return reinterpret_cast<PythonClassWrapper *>(
                    Base::template _createPyFromAllocatedInstance<Args...>(_Type, args..., arraySize));
        }

    protected:
        static PyObject *_addr(PyObject *self_, PyObject *args);
        static int _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds);
        static PyTypeObject _Type;
        static PyMethodDef _methods[];

     private:
        PythonClassWrapper<T_NoRef *> *createPyReferenceToAddr();

        struct Iter{
            PyObject_HEAD
            PythonClassWrapper* obj;
            size_t max;
            size_t i;
            static const std::string name;
            static PyTypeObject _Type;

            static PyObject * iter(PyObject* self);
            static PyObject * iternext(PyObject* self);
        };
    };


}
#endif
