#ifndef __PYLLARS__INTERNAL__POINTER_H
#define __PYLLARS__INTERNAL__POINTER_H

#include <type_traits>

#include <limits>
#include <vector>
#include <functional>

#include <Python.h>
#include <structmember.h>
#include <climits>

#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_containment.hpp"


namespace __pyllars_internal {

    extern PyTypeObject BasePtrType;

    /**
     * A class that wraps a c-pointer-like type to represent in Python
     *
     * @tparam T a pointer-like type to create a Python-wrapper-to-C-pointer
     */
    template<typename T>
    struct PythonPointerWrapperBase: public CommonBaseWrapper{

        template<typename Other, typename EE>
        friend struct PythonClassWrapper;

        template<typename Other>
        friend struct PythonPointerWrapperBase;

        typedef typename std::remove_pointer<typename extent_as_pointer<typename std::remove_reference<T>::type>::type>::type T_base;

        // last element of array, or -1 if unknown extent
        static constexpr ssize_t last = ArraySize<T>::size - 1;

    protected:

        /**
         *
         * @param Type: python type to be initialized
         * @param cobj: non-null C object to wrap as Python construct
         * @param arraySize: size behind the pointer (number of elements), or -1 if unknown
         * @param referencing: which Python object this pointer is referencing into, or nullptr if none
         * @param byte_bucket: a pointer to raw bytes in memory where an in-place allocated instance/array is found,
         *   or nullptr if not allocated in-place
         * @return Python object wrapping given C pointer object
         */
        static PythonPointerWrapperBase *fromCPointer(PyTypeObject & Type,
                T* cobj,
                const ssize_t arraySize,
                PyObject *referencing = nullptr,
                unsigned char* byte_bucket = nullptr);

        /**
         *
         * @tparam Args: constructor args to use to instantiate a/an-array-of C objects to wrap
         *
         * @param Type: the Python type representing the returned object
         * @param args: constructor args for instantiation
         * @param arraySize: number of elements to allocated
         * @return Python object wrappping the instantiated C-object/array
         */
        template<typename ...Args>
        static PythonPointerWrapperBase *createAllocatedInstance(PyTypeObject & Type,
                Args ...args,
                ssize_t arraySize = -1);

        /**
         *
         * @param Type: Python type to initialize to represent a C point in Python
         * @return 0 on success, negative value otherwise
         */
        static int _initialize(PyTypeObject & Type);

        /**
         * @return a pointer to the pointer-object being wrapped
         */
        T *_get_CObject() const;

        size_t _depth{}; //only used for classes with pointer depth > 1, kept here for consistent PyType layout

    private:

        // map a set of calls to the equivalents in PyObjectType tp_* attreiutes
        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

        static void _dealloc(PyObject *self_);

        static void _free(void *self);

        /**
         * @return: mapped to a Python method to return number of elements associated with wrapped pointer, or -1 if not known
         */
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

/**
         * Concatenate two objects representing C arrays into a single array and return
         * @param self: object to do in-place concat
         * @param other: object to be concatenated with
         * @return new concatenated object
         */
        static PyObject *_concat(PyObject *self, PyObject *other);

        /**
         * Set an item within the underlying C-array
         * @param self: Python object wrapping the object to be changed
         * @param index: which index to set
         * @param obj : Python wrapper of a value to set the item to
         * @return 0 on success, negative value otherwise
         */
        static int _set_item(PyObject *self, Py_ssize_t index, PyObject *obj);

        /**
         *
         * @param self: Python object wrapping a C array to get an item out of
         * @param index: index into pointer to get out of the array
         * @return Python wrapper for the item requested
         */
        static PyObject *_get_item(PyObject *self, Py_ssize_t index);

        /**
         * Python wrapper to provide a method to the client to get an item at a given index
         * @return Python wrapper to the item at the given index
         */
        static PyObject *_at(PyObject *self, PyObject *args, PyObject *kwds);

        /**
         * Ptyhon method to detrmine if given object is contained in self
         * @return 1 if contained, 0 otherwise
         */
        static int _contains(PyObject *self, PyObject *obj);

        static int _initbase(PythonPointerWrapperBase *self, PyObject *args, PyObject *kwds, PyTypeObject *pytypeobj);

        static PySequenceMethods _seqmethods;

        T *_CObject;
        unsigned char* _byte_bucket;
        ssize_t _max;

        T* get_CObject() const{
            return _CObject;
        }

        //should never get called as Python uses malloc and such to allocate memory
        PythonPointerWrapperBase();

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
        friend struct PythonPointerWrapperBase;

        PythonClassWrapper():PythonPointerWrapperBase<T>(){
           Base::_depth = ptr_depth<T>::value;
        }

        T_NoRef * get_CObject() const{
            T_NoRef * value = Base::_get_CObject();
            return value;
        }

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &_Type;
        }

        static int initialize(){
            return Base::_initialize(_Type);
        }

        static bool checkType(PyObject *obj);


        static PythonClassWrapper *fromCPointer(T& cobj, const ssize_t arraySize, PyObject *referencing = nullptr){
            return reinterpret_cast<PythonClassWrapper*>(Base::fromCPointer(_Type, &cobj, arraySize, referencing, nullptr));
        }

        /**
         * Allocate an instance based on a set of constructor args
         * @tparam Args types of arguments to pass to constructor
         *
         * @param args: the argument values to pass to constructor
         * @return Python wrapper to an allocated instance, or nullptr on error
         */
        template<typename ...Args>
        static PythonClassWrapper *allocateInstance(Args... args){

            return (PythonClassWrapper*) Base::_createPyFromAllocatedInstance(_Type, args...,  -1);
        }

        template<typename ...Args>
        static PythonClassWrapper *allocateArray( Args ...args, const ssize_t arraySize) {
            return reinterpret_cast<PythonClassWrapper *>(
                    Base::_createPyFromAllocatedInstance(_Type, args..., arraySize));
        }

        static PythonClassWrapper *fromInPlaceAllocation( const ssize_t arraySize, unsigned char* byte_bucket){
            return reinterpret_cast<PythonClassWrapper*>(Base::_createPyReference(_Type, nullptr, arraySize, nullptr, byte_bucket));
        }


        typename std::remove_const<T>::type& toCArgument();
        const T& toCArgument() const;

    protected:
        PythonClassWrapper *createPyReferenceToAddr();
        static PyObject *_addr(PyObject *self, PyObject *args);

        static int _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds);
        static PyTypeObject _Type;
        static PyMethodDef _methods[];
    };


    template<typename T>
    struct PythonClassWrapper<T, typename std::enable_if<is_pointer_like<T>::value && (ptr_depth<T>::value == 1) >::type> : public PythonPointerWrapperBase<T> {
    public:
        typedef PythonPointerWrapperBase<T>  Base;
        typedef typename std::remove_reference<T>::type T_NoRef;

        T_NoRef* get_CObject() const{
            return PythonPointerWrapperBase<T>::_get_CObject();
        }

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &_Type;
        }

        static int initialize(){
            return Base::_initialize(_Type);
        }

        static bool checkType(PyObject *obj);

        static PythonClassWrapper *fromCPointer( T & cobj, const ssize_t arraySize, PyObject *referencing = nullptr){
            return reinterpret_cast<PythonClassWrapper*>(Base::fromCPointer(_Type, &cobj, arraySize, referencing, nullptr));
        }

        static PythonClassWrapper *fromInPlaceAllocation( const ssize_t arraySize, unsigned char* byte_bucket){
            return reinterpret_cast<PythonClassWrapper*>(Base::fromCPointer(_Type, nullptr, arraySize, nullptr, byte_bucket));
        }

        template<typename ...Args>
        static PythonClassWrapper *allocateInstance(Args... args){

            return (PythonClassWrapper*) Base::template createAllocatedInstance<Args...>(_Type, args...,  -1);
        }

        template<typename ...Args>
        static PythonClassWrapper *allocateArray( Args ...args, const ssize_t arraySize) {
            return reinterpret_cast<PythonClassWrapper *>(
                    Base::template createAllocatedInstance<Args...>(_Type, args..., arraySize));
        }


        typename std::remove_const<T>::type& toCArgument();
        const T& toCArgument() const;

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
            long long max;
            long long i;
            static const std::string name;
            static PyTypeObject _Type;

            static PyObject * iter(PyObject* self);
            static PyObject * iternext(PyObject* self);
        };


    };


}
#endif
