#ifndef __PYLLARS__CONTAINMENT__H
#define __PYLLARS__CONTAINMENT__H

#include <functional>
#include "pyllars_type_traits.hpp"

namespace __pyllars_internal{

    //////////////////////////////////////
    // Containers for Cobjects in varoious forms.  Containers allow for a wrapper that can
    // convert what would normally be a compil-time error in C to a more Pythonic run-time error
    /////////////////////////////////////

    /**
     * Variou kinds of containers
     */
    enum class ContainmentKind: unsigned char{
        BY_REFERENCE,
        ALLOCATED,
        CONSTRUCTED,
        CONSTRUCTED_IN_PLACE
    };


    namespace {

        /**
         * Helper for dealing with schizoprhenic fixed arrays in c (is it a pointer or an object with contigous memory?)
         * @tparam T: type of element in array
         * @tparam size : size of array
         */
        template<typename T, size_t size>
        struct FixedArrayHelper{
            T value[size];
        };
    }

    /////////////////////////////////////////////////////

    // DESTRUCTOR LOGIC
    // Converts what would normally be an exception on deletion of an object that is indestructible by the rules of C
    // to a run-time error for Python

    template <typename T, typename Z=void>
    struct Destructor;

    template<typename T>
    struct Destructor<T, typename std::enable_if<std::is_destructible<T>::value && !std::is_array<T>::value>::type>{
        static void inplace_destrcuct(T& obj){
            obj.~T();
        }

        static void deallocate(T* &obj, const bool asArray){
            if (asArray){
                delete [] obj;
            } else {
                delete obj;
            }
            obj = nullptr;
        }

        static void deallocate(T* const &obj, const bool asArray){
            if (asArray){
                delete [] obj;
            } else {
                delete obj;
            }
        }
    };

    template<typename T, size_t size>
    struct Destructor<T[size], typename std::enable_if<std::is_destructible<T>::value>::type>{

        typedef T T_array[size];

        static void inplace_destrcuct(T_array& obj){
            for(size_t i = 0; i < size; ++i){
                obj[i].~T();
            }
        }

        static void deallocate(T_array* &obj, const bool asArray){
            if (asArray){
                delete [] obj;
            } else {
                delete obj;
            }
            obj = nullptr;
        }

        static void deallocate(T_array* const &obj, const bool asArray){
            if (asArray){
                delete [] obj;
            } else {
                delete obj;
            }
        }
    };

    template<typename T>
    struct Destructor<T, typename std::enable_if<!std::is_destructible<T>::value>::type>{
        static void inplace_destrcuct(T& obj){
            // nothing to be done
        }

        static void deallocate(T* &obj, const bool asArray){
           obj = nullptr;
           throw "attempt to deallocate indestructible type";
        }

        static void deallocate(T* const &obj, const bool asArray){
            throw "attempt to deallocate indestructible type";
        }
    };


    template<typename T, size_t size>
    struct Destructor<T[size], typename std::enable_if<!std::is_destructible<T>::value>::type>{
        static void inplace_destrcuct(T& obj){
            // nothing to be done
        }

        static void deallocate(T* &obj, const bool asArray){
            obj = nullptr;
            throw "attempt to deallocate indestructible type";
        }

        static void deallocate(T* const &obj, const bool asArray){
            throw "attempt to deallocate indestructible type";
        }
    };


    ///////////////////////////////////

    // CONSTRUCTOR Logic
    // converts what would be a compile-time error for non-constructible occurrences into run-time erorrs
    // for compatibility in Python

    template<typename T, typename Z=void>
    struct Constructor;

    template<typename T>
    struct Constructor<T, typename std::enable_if<std::is_constructible<T>::value && !std::is_array<T>::value>::type>{
        template<typename ...Args>
        static T* inplace_allocate(T& obj, Args ...args){
            return new ((void*)&obj)T(std::forward<typename extent_as_pointer<Args>::type>>(args)...);
        }

        template<typename ...Args>
        static T* allocate(Args ...args){
            return new T(std::forward<typename extent_as_pointer<Args>::type>>(args)...);
        }


        template<typename ...Args>
        static T* allocate_array(const size_t size, Args ...args){
            return new T[size]{T(std::forward<typename extent_as_pointer<Args>::type>>(args)...)};
        }
    };


    template<typename T, size_t size>
    struct Constructor<T[size], typename std::enable_if<std::is_constructible<T>::value>::type>{
        typedef T T_array[size];

        static T_array* inplace_allocate(T_array& obj, T_array & from){
            for(size_t i = 0; i < size; ++i){
                new ((void*)&obj[i])T(from[i]);
            }
            return &obj;
        }

        static T_array* inplace_allocate(T_array& obj, T* const from){
            for(size_t i = 0; i < size; ++i){
                new ((void*)&obj[i])T(from[i]);
            }
            return &obj;
        }

        template<typename ...Args>
        static T* allocate(Args ...args){
            return new T[size]{T(std::forward<typename extent_as_pointer<Args>::type>>(args)...)};
        }

        template<typename ...Args>
        static T* allocate_array(const size_t _size, Args ...args){
            throw "Cannot instantiate object of given type per rules of C++";
        }
    };


    template<typename T>
    struct Constructor<T, typename std::enable_if<!std::is_constructible<T>::value>::type>{
        template<typename ...Args>
        static T* inplace_allocate(T& obj, Args ...args){
            throw "Cannot instantiate object of given type per rules of C++";
        }

        template<typename ...Args>
        static T* allocate(Args ...args){
            throw "Cannot instantiate object of given type per rules of C++";
        }

        template<typename ...Args>
        static T* allocate_array(const size_t size, Args ...args){
            throw "Cannot instantiate object of given type per rules of C++";
        }
    };

    ///////////////////////////////////

    /**
     * "Abstract" base class for an object container holding an instance or refernce to a C object
     * @tparam T : Type of C object
     */
    template<typename T>
    struct ObjectContainer{
        typedef typename std::remove_reference<T>::type T_NoRef;

        virtual operator T_NoRef&() {return *fake;} // gcc complains in some cses if pure virtual :-/

        virtual operator const T_NoRef&() const {return *fake;} // gcc complains in some cses if pure virtual :-/

        virtual T_NoRef * ptr(){
            return nullptr;  //gcc complains if we make this virtual and use unnamed type such as an enum
        }

        virtual ~ObjectContainer(){
        }

    protected:
        //substitute for not being allowed to make abstract
        ObjectContainer(){
        }

    private:
        ObjectContainer(const ObjectContainer &);
        static constexpr T* fake =  nullptr;
    };

    /**
     * Constainer for a reference-to-an-object
     * Client is responsible to ensure lifetime of given object exceeds lifetime of container
     */
    template<typename T>
    struct ObjectContainerReference: public ObjectContainer<T>{
        typedef typename std::remove_reference<T>::type T_NoRef;

        ObjectContainerReference(T_NoRef & obj):ObjectContainer<T>(), _contained(obj), _containedP(&_contained){
        }

        virtual ~ObjectContainerReference(){
            _containedP = nullptr;
        }


        operator T_NoRef&(){
            if(!_containedP) throw "Attempt to dereference null C object";
            return _contained;
        }

        operator const T_NoRef&() const{
            if(!_containedP) throw "Attempt to dereference null C object";
            return _contained;
        }

        T_NoRef *  ptr(){
            return _containedP;
        }

    protected:
        T_NoRef & _contained;
        T_NoRef * _containedP;

    };



    template<typename T, typename Z=void>
    struct ObjectContainerAllocated;

    template<typename T>
    struct ObjectContainerAllocated<T>: public ObjectContainerReference<T>{
        typedef typename std::remove_reference<T>::type T_NoRef;

        template<typename ...Args>
        static ObjectContainerAllocated*  new_container(Args ...args){
            return new ObjectContainerAllocated(new T(args...));
        }

    private:

        ObjectContainerAllocated(T_NoRef* obj):ObjectContainerReference<T>(*_obj), _obj(obj){
        }

        virtual ~ObjectContainerAllocated(){
            Destructor<T_NoRef>::deallocate(ObjectContainerReference<T>::_containedP, false);
        }

        T * _obj;
    };

    template<typename T, size_t size>
    struct ObjectContainerAllocated<T[size]>: public ObjectContainerReference<T[size]> {
        typedef typename std::remove_reference<T>::type T_NoRef;
        typedef T T_array[size];

        template<typename ...Args>
        static ObjectContainerAllocated*  new_container(T_array&  value){
            typedef typename std::remove_const<T>::type T_nonconst_array[size];
            T_nonconst_array *new_value = new T_nonconst_array[1];
            for (size_t i = 0; i < size; ++i) new_value[0][i] = value[i];
            return new ObjectContainerAllocated(new_value);
        }

    private:

        ObjectContainerAllocated(T_array* obj):ObjectContainerReference<T[size]>(*_obj), _obj(obj){

        }

        virtual ~ObjectContainerAllocated(){
            Destructor<T[size]>::deallocate(ObjectContainerReference<T[size]>::_containedP, true);
        }
        T_array* _obj;
    };


    //////////////////////////


    template<typename T>
    struct ObjectContainerBytePool;

    template<typename T>
    struct ObjectContainerBytePool<T*>: public ObjectContainer<T*>{

        ObjectContainerBytePool( const size_t arraySize, std::function<void(void*, size_t)> construct_element ):
                _raw_bytes(malloc(arraySize*sizeof(T))), _size(arraySize){
            memset(_raw_bytes, 0, arraySize*sizeof(T));
            for(size_t i = 0; i < arraySize; ++i){
                construct_element((void*)(((T*)_raw_bytes) + i), i);
            }
        }

        T& operator [](const size_t index){
            if (index < 0 || index >=_size){
                throw "Index out of range";
            }
            return _contained[index];
        }

        operator T*&(){
            if(!_raw_bytes) throw "Attempt to dereference null C object";
            return _contained;
        }

        operator T* const&() const{
            if(!_raw_bytes) throw "Attempt to dereference null C object";
            return _contained;
        }

        T** ptr(){
            return (T**)&_contained;
        }

        ~ObjectContainerBytePool(){
            for(size_t i = 0; i < _size; ++i){
                Destructor<T>::inplace_destrcuct((*this)[i]);
            }
            free(_raw_bytes);
        }


    protected:
        union {
            void *_raw_bytes;
            T *_contained;
        };
        const size_t _size;
    };


    template<typename T>
    struct ObjectContainerBytePool<T* const>: public ObjectContainer<T* const>{

        ObjectContainerBytePool( const size_t arraySize, std::function<void(void*, size_t)> construct_element ):
                _raw_bytes(malloc(arraySize*sizeof(T))), _size(arraySize){
            memset(_raw_bytes, 0, arraySize*sizeof(T));
            for(size_t i = 0; i < arraySize; ++i){
                construct_element((void*)(((T*)_raw_bytes) +i), i);
            }
        }

        T& operator [](const size_t index){
            if (index < 0 || index >=_size){
                throw "Index out of range";
            }
            return _contained[index];
        }


        operator T*&(){
            if(!_raw_bytes) throw "Attempt to dereference null C object";
            return _contained;
        }

        operator T* const&() const{
            if(!_raw_bytes) throw "Attempt to dereference null C object";
            return _contained;
        }

        T* const * ptr(){
            return &_contained;
        }

        ~ObjectContainerBytePool(){
            for(size_t i = 0; i < _size; ++i){
                Destructor<T>::inplace_destrcuct((*this)[i]);
            }
            free(_raw_bytes);
        }

    protected:
        union {
            void *_raw_bytes;
            T* const _contained;
        };
        const size_t _size;

    };

    template<typename T, size_t size>
    struct ObjectContainerBytePool<T[size]>: public ObjectContainerReference<T[size]>{

        ObjectContainerBytePool( std::function<void(void*, size_t)> construct_element ):
                _raw_bytes(malloc(size*sizeof(T))){
            memset(_raw_bytes, 0, size*sizeof(T));
            for(size_t i = 0; i < size; ++i){
                construct_element((void*)(((T*)_raw_bytes) + i), i);
            }
        }

        T& operator [](const size_t index){
            if (index < 0 || index >=size){
                throw "Index out of range";
            }
            return _contained[index];
        }

        operator T*&(){
            if(!_raw_bytes) throw "Attempt to dereference null C object";
            return _contained;
        }

        operator T* const&() const{
            if(!_raw_bytes) throw "Attempt to dereference null C object";
            return _contained;
        }

        T** ptr(){
            return &_contained;
        }


        ~ObjectContainerBytePool(){
            for(size_t i = 0; i < size; ++i){
                Destructor<T>::inplace_destrcuct((*this)[i]);
            }
            free(_raw_bytes);
        }


    protected:
        union {
            void *_raw_bytes;
            T* _contained;
        };

    };

    /////////////////////////////

    template<typename T, typename ...Args>
    struct ObjectContainerInPlace: public ObjectContainerReference<T>{
        typedef typename std::remove_reference<T>::type T_NoRef;

        ObjectContainerInPlace(T_NoRef& obj,  Args ...args):
                ObjectContainerReference<T>(*new ((void*)&obj) T_NoRef(std::forward<typename extent_as_pointer<Args>::type>(args)...)){

        }

        ~ObjectContainerInPlace(){
        }

    };

    template<typename T, size_t size>
    struct ObjectContainerInPlace<T[size], T[size]>: public ObjectContainerReference<T[size]>{

        typedef T T_array[size];

        ObjectContainerInPlace(T_array& obj,  T_array &arg):
                ObjectContainerReference<T[size]>(*Constructor<T[size]>::inplace_allocate(obj, arg)){

        }

        ObjectContainerInPlace(T_array& obj,  T* arg):
                ObjectContainer<T[size]>(*Constructor<T[size]>::inplace_allocate(obj, arg)){

        }

        ~ObjectContainerInPlace(){
        }

    };

    template<typename T, typename ...Args>
    struct ObjectContainerConstructed: public ObjectContainerReference<T>{
        ObjectContainerConstructed(Args ...args):_constructed(std::forward<typename extent_as_pointer<Args>::type>(args)...),
        ObjectContainerReference<T>(_constructed){

        }
    private:
        T _constructed;
    };

    //Reference types should just capture the reference, so same as base class behavior:
    template<typename T, typename ...Args>
    struct ObjectContainerConstructed<T&, Args...>: public ObjectContainerReference<T>{
        ObjectContainerConstructed(Args ...args):ObjectContainerReference<T>(std::forward<typename extent_as_pointer<Args>::type>(args)...){

        }
    };
    template<typename T, typename ...Args>
    struct ObjectContainerConstructed<T&&, Args...>: public ObjectContainerReference<T>{
        ObjectContainerConstructed(Args ...args):ObjectContainerReference<T>(std::forward<typename extent_as_pointer<Args>::type>>(args)...){

        }
    };

    template<size_t size, typename T>
    struct ObjectContainerConstructed<T[size], T[size]>: public ObjectContainerReference<T[size]>{
        ObjectContainerConstructed(T args[size]):ObjectContainerReference<T[size]>(((FixedArrayHelper<T,size>*)&_constructed)->value){
            for(size_t i = 0; i < size; ++i){
                new (&_constructed[0] + sizeof(T)*i) T(args[i]);
            }
        }
    private:
        unsigned char _constructed[size*sizeof(T)];
    };

    template<typename T>
    struct ObjectContainerPyReference: public ObjectContainer<T>{

        ~ObjectContainerPyReference(){
        }

    private:
        ObjectContainerPyReference(PyObject* obj, typename extent_as_pointer<T>::type const (*convert)(PyObject*)){
        }


    };

    template<typename T>
    struct ObjectContainerPyReference<T*>: public ObjectContainer<T*>{
        ObjectContainerPyReference(PyObject* obj, T* (*convert)(PyObject*)):_cobj(convert(obj)), _pyobj(obj){
            if(!&_cobj){throw "Invalid conversion from python to c object";}
            Py_INCREF(_pyobj);
        }

        ~ObjectContainerPyReference(){
            Py_DECREF(_pyobj);
        }

        operator T*&() override{
            return _cobj;
        }
        operator T* const&() const override{
            return _cobj;
        }

        T ** ptr() override{
            return (T**) &_cobj;
        }

    private:
        PyObject* _pyobj;
        T* _cobj;
    };

    template<typename T>
    struct ObjectContainerPyReference<T* const>: public ObjectContainer<T* const>{
        ObjectContainerPyReference(PyObject* obj, T* const (*convert)(PyObject*)):_cobj(convert(obj)), _pyobj(obj){
            if(!&_cobj){throw "Invalid conversion from python to c object";}
            Py_INCREF(_pyobj);
        }

        ~ObjectContainerPyReference(){
            Py_DECREF(_pyobj);
        }

        operator T* const&() override{
            return _cobj;
        }
        operator T* const&() const override{
            return _cobj;
        }

        T * const * ptr() override{
            return &_cobj;
        }

    private:
        PyObject* _pyobj;
        T* const _cobj;
    };
}
#endif