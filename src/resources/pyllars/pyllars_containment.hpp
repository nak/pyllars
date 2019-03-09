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

    template<typename T>
    struct ObjectContainer{
        typedef typename std::remove_reference<T>::type T_NoRef;

        ObjectContainer(T_NoRef& obj):_contained(obj), _containedP(&_contained){
        }

        operator T_NoRef&(){
            if(!_containedP) throw "Attempt to dereference null C object";
            return _contained;
        }

        operator const T_NoRef&() const{
            if(!_containedP) throw "Attempt to dereference null C object";
            return _contained;
        }

        virtual ~ObjectContainer(){
            _containedP = nullptr;
        }

        T_NoRef *ptr() const{
            return _containedP;
        }
    protected:
        T_NoRef & _contained;
        T_NoRef * _containedP;

    private:
        ObjectContainer(const ObjectContainer &);
    };



    template<typename T, typename Z=void>
    struct ObjectContainerAllocated;

    template<typename T>
    struct ObjectContainerAllocated<T, typename std::enable_if<std::is_destructible<T>::value>::type>: public ObjectContainer<T>{
        typedef typename std::remove_reference<T>::type T_NoRef;

        ObjectContainerAllocated(T_NoRef* obj, const bool as_array = false):ObjectContainer<T>(*obj),
            _asArray(as_array){
        }

        ~ObjectContainerAllocated(){
            if (_asArray){
                delete [] ObjectContainer<T>::_containedP;
            } else {
                delete ObjectContainer<T>::_containedP;
            }
        }


    protected:
        const bool _asArray;
    };


    template<typename T>
    struct ObjectContainerAllocated<T, typename std::enable_if<!std::is_destructible<T>::value>::type>: public ObjectContainer<T>{
        typedef typename std::remove_reference<T>::type T_NoRef;

        ObjectContainerAllocated(T_NoRef* obj, const bool as_array = false):ObjectContainer<T>(*obj),
                                                                            _asArray(as_array){
        }

        ~ObjectContainerAllocated(){
            throw "Attempt to deallocate indestructible typed object";
        }


    protected:
        const bool _asArray;
    };


    template<typename T, typename Z=void>
    struct ObjectContainerBytePool;

    template<typename T>
    struct ObjectContainerBytePool<T, typename std::enable_if<std::is_pointer<T>::value && std::is_destructible<typename std::remove_pointer<T>::type>::value>::type>: public ObjectContainer<T>{

        typedef typename std::remove_pointer<T>::type T_base;

        ObjectContainerBytePool(T obj, const size_t arraySize):
            ObjectContainer<T>(obj), _size(arraySize){
        }

        ~ObjectContainerBytePool(){
            for(size_t i = 0; i < _size; ++i){
                ObjectContainer<T>::_contained[i].~T_base();
            }
            unsigned char* raw_storage = (unsigned char*) ObjectContainer<T>::_contained;
            delete [] raw_storage;
        }


    protected:
        const size_t _size;

    };


    template<typename T>
    struct ObjectContainerBytePool<T, typename std::enable_if<std::is_pointer<T>::value && !std::is_destructible<typename std::remove_pointer<T>::type>::value>::type>: public ObjectContainer<T>{

        ObjectContainerBytePool(T obj, const size_t arraySize):
                ObjectContainer<T>(obj), _size(arraySize){
        }

        ~ObjectContainerBytePool(){
            unsigned char* raw_storage = (unsigned char*) ObjectContainer<T>::_contained;
            delete [] raw_storage;
        }

    protected:
        const size_t _size;

    };


    template<typename T, typename ...Args>
    struct ObjectContainerInPlace: public ObjectContainer<T>{
        typedef typename std::remove_reference<T>::type T_NoRef;

        ObjectContainerInPlace(T_NoRef& obj,  Args ...args):
           ObjectContainer<T>(*new ((void*)&obj) T_NoRef(args...)){

        }

        ~ObjectContainerInPlace(){
        }

    };

    template<typename T, typename ...Args>
    struct ObjectContainerConstructed: public ObjectContainer<T>{
        ObjectContainerConstructed(Args ...args):_constructed(args...),
        ObjectContainer<T>(_constructed){

        }
    private:
        T _constructed;
    };

    namespace {

        template<typename T, size_t size>
        struct FixedArrayHelper{
            T value[size];
        };
    }

    template<size_t size, typename T>
    struct ObjectContainerConstructed<T[size], T[size]>: public ObjectContainer<T[size]>{
        ObjectContainerConstructed(T args[size]):ObjectContainer<T[size]>(((FixedArrayHelper<T,size>*)&_constructed)->value){
            for(size_t i = 0; i < size; ++i){
                new (&_constructed[0] + sizeof(T)*i) T(args[i]);
            }
        }
    private:
        unsigned char _constructed[size*sizeof(T)];
    };
}
#endif