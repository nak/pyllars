#ifndef __PYLLARS__CONTAINMENT__H
#define __PYLLARS__CONTAINMENT__H

#include "pyllars_type_traits.hpp"

namespace __pyllars_internal{

    enum class ContainmentKind: unsigned char{
        BY_REFERENCE,
        ALLOCATED,
        CONSTRUCTED,
        CONSTRUCTED_IN_PLACE,
        RAW_BYTE_POOL
    };


    namespace {

        template<typename T, size_t size>
        struct FixedArrayHelper{
            T value[size];
        };
    }

    /////////////////////////////////////////////////////

    // DESTRUCTOR LOGIC

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


    template<typename T>
    struct ObjectContainer{
        typedef typename std::remove_reference<T>::type T_NoRef;


        virtual operator T_NoRef&() {return *fake;}

        virtual operator const T_NoRef&() const {return *fake;} // gcc complains in some cses if pure virtual :-/

        virtual T_NoRef *ptr() const{
            return nullptr;  //gcc complains if we make this virtual and use unnamed type such as an enum
        }

        virtual ~ObjectContainer(){
        }
    protected:
        ObjectContainer(){
        }

    private:
        ObjectContainer(const ObjectContainer &);
        static constexpr T* fake =  nullptr;
    };

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
        T_NoRef *ptr() const{
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

        ObjectContainerAllocated(T_NoRef* obj, const bool as_array = false):ObjectContainerReference<T>(*obj),
            _asArray(as_array){
        }

        virtual ~ObjectContainerAllocated(){
            Destructor<T_NoRef>::deallocate(ObjectContainerReference<T>::_containedP, _asArray);
        }


    protected:
        const bool _asArray;
    };


    template<typename T>
    struct ObjectContainerAllocatedArray: public ObjectContainerReference<T>{
        typedef typename std::remove_reference<T>::type T_NoRef;

        ObjectContainerAllocatedArray(T_NoRef &obj, const bool as_array = false):ObjectContainerReference<T>(obj){
            throw "Invalid call to allocate non-array type";
        }

        ~ObjectContainerAllocatedArray(){
        }

    };

    template<typename T>
    struct ObjectContainerAllocatedArray<T*>: public ObjectContainer<T*>{
        typedef typename std::remove_reference<T>::type T_NoRef;

        ObjectContainerAllocatedArray(T* obj, const bool as_array = false):_obj(obj), _asArray(as_array){
        }

        virtual ~ObjectContainerAllocatedArray(){
            Destructor<T>::deallocate(_obj, _asArray);
            _obj = nullptr;
        }

        operator T*&() override{
            if(!_obj) throw "Attempt to dereference null C object";
            return _obj;
        }

        operator T* const&() const override{
            if(!_obj) throw "Attempt to dereference null C object";
            return _obj;
        }

        T **  ptr() const override{
            return (T**) &_obj;
        }

    protected:
        T* _obj;
        const bool _asArray;
    };


    template<typename T>
    struct ObjectContainerAllocatedArray<T* const>: public ObjectContainer<T* const>{

        ObjectContainerAllocatedArray(T* const obj, const bool as_array = false):_obj(obj), _asArray(as_array){
        }

        ~ObjectContainerAllocatedArray(){
            Destructor<T>::deallocate(_obj, _asArray);
        }

        operator T* const&() override{
            if(!_obj) throw "Attempt to dereference null C object";
            return _obj;
        }
        operator T* const&() const override{
            if(!_obj) throw "Attempt to dereference null C object";
            return _obj;
        }

        T *const *  ptr() const override{
            return &_obj;
        }

    protected:
        T* const _obj;
        const bool _asArray;
    };

    template<typename T>
    struct ObjectContainerBytePool;

    template<typename T>
    struct ObjectContainerBytePool<T*>: public ObjectContainerReference<T*>{

        ObjectContainerBytePool(T*& obj, const size_t arraySize):
            ObjectContainerReference<T*>(obj), _size(arraySize){
        }

        ~ObjectContainerBytePool(){
            for(size_t i = 0; i < _size; ++i){
                Destructor<T>::inplace_destrcuct(ObjectContainerReference<T*>::_contained[i]);
            }
            unsigned char* raw_storage = (unsigned char*) ObjectContainerReference<T*>::_contained;
            delete [] raw_storage;
        }


    protected:
        const size_t _size;

    };


    template<typename T>
    struct ObjectContainerBytePool<T* const>: public ObjectContainerReference<T* const>{

        ObjectContainerBytePool(T* const& obj, const size_t arraySize):
                ObjectContainerReference<T* const>(obj), _size(arraySize){
        }

        ~ObjectContainerBytePool(){
            for(size_t i = 0; i < _size; ++i){
                Destructor<T>::inplace_destrcuct(ObjectContainerReference<T* const>::_contained[i]);
            }
            unsigned char* raw_storage = (unsigned char*) ObjectContainerReference<T* const>::_contained;
            delete [] raw_storage;
        }


    protected:
        const size_t _size;

    };

    template<typename T, size_t size>
    struct ObjectContainerBytePool<T[size]>: public ObjectContainerReference<T[size]>{

        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

        ObjectContainerBytePool(T * const&obj, const size_t arraySize):
                ObjectContainerReference<T[size]>(((FixedArrayHelper<T, size>*)obj)->value), _size(arraySize){
        }

        ~ObjectContainerBytePool(){
            for(size_t i = 0; i < _size; ++i){
                Destructor<T>::inplace_destrcuct(ObjectContainerReference<T[size]>::_contained[i]);
            }
            unsigned char* raw_storage = (unsigned char*) ObjectContainerReference<T[size]>::_contained;
            delete [] raw_storage;
        }

    protected:
        const size_t _size;

    };

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
        ObjectContainerPyReference(PyObject* obj, typename extent_as_pointer<T>::type (*convert)(PyObject*)){
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

        T **  ptr() const override{
            return (T**) &_cobj;
        }

    private:
        PyObject* _pyobj;
        T* _cobj;
    };

    template<typename T>
    struct ObjectContainerPyReference<T* const>: public ObjectContainer<T* const>{
        ObjectContainerPyReference(PyObject* obj, T*  (*convert)(PyObject*)):_cobj(convert(obj)), _pyobj(obj){
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

        T * const *  ptr() const override{
            return &_cobj;
        }

    private:
        PyObject* _pyobj;
        T* const _cobj;
    };
}
#endif