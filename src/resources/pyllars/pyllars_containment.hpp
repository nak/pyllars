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

        ObjectContainer(T_NoRef & obj):_contained(obj), _containedP(&_contained){
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
    struct ObjectContainerAllocated<T>: public ObjectContainer<T>{
        typedef typename std::remove_reference<T>::type T_NoRef;

        ObjectContainerAllocated(T_NoRef* obj, const bool as_array = false):ObjectContainer<T>(*obj),
            _asArray(as_array){
        }

        ~ObjectContainerAllocated(){
            Destructor<T_NoRef>::deallocate(ObjectContainer<T>::_containedP, _asArray);
        }


    protected:
        const bool _asArray;
    };

    template<typename T>
    struct ObjectContainerBytePool;

    template<typename T>
    struct ObjectContainerBytePool<T*>: public ObjectContainer<T*>{

        ObjectContainerBytePool(T*& obj, const size_t arraySize):
            ObjectContainer<T*>(obj), _size(arraySize){
        }

        ~ObjectContainerBytePool(){
            for(size_t i = 0; i < _size; ++i){
                Destructor<T>::inplace_destrcuct(ObjectContainer<T*>::_contained[i]);
            }
            unsigned char* raw_storage = (unsigned char*) ObjectContainer<T*>::_contained;
            delete [] raw_storage;
        }


    protected:
        const size_t _size;

    };


    template<typename T>
    struct ObjectContainerBytePool<T* const>: public ObjectContainer<T* const>{

        ObjectContainerBytePool(T* const& obj, const size_t arraySize):
                ObjectContainer<T* const>(obj), _size(arraySize){
        }

        ~ObjectContainerBytePool(){
            for(size_t i = 0; i < _size; ++i){
                Destructor<T>::inplace_destrcuct(ObjectContainer<T* const>::_contained[i]);
            }
            unsigned char* raw_storage = (unsigned char*) ObjectContainer<T* const>::_contained;
            delete [] raw_storage;
        }


    protected:
        const size_t _size;

    };

    template<typename T, size_t size>
    struct ObjectContainerBytePool<T[size]>: public ObjectContainer<T[size]>{

        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

        ObjectContainerBytePool(T * const&obj, const size_t arraySize):
                ObjectContainer<T[size]>(((FixedArrayHelper<T, size>*)obj)->value), _size(arraySize){
        }

        ~ObjectContainerBytePool(){
            for(size_t i = 0; i < _size; ++i){
                Destructor<T>::inplace_destrcuct(ObjectContainer<T[size]>::_contained[i]);
            }
            unsigned char* raw_storage = (unsigned char*) ObjectContainer<T[size]>::_contained;
            delete [] raw_storage;
        }

    protected:
        const size_t _size;

    };

    template<typename T, typename ...Args>
    struct ObjectContainerInPlace: public ObjectContainer<T>{
        typedef typename std::remove_reference<T>::type T_NoRef;

        ObjectContainerInPlace(T_NoRef& obj,  Args ...args):
                ObjectContainer<T>(*new ((void*)&obj) T_NoRef(std::forward<typename extent_as_pointer<Args>::type>(args)...)){

        }

        ~ObjectContainerInPlace(){
        }

    };

    template<typename T, size_t size>
    struct ObjectContainerInPlace<T[size], T[size]>: public ObjectContainer<T[size]>{

        typedef T T_array[size];

        ObjectContainerInPlace(T_array& obj,  T_array &arg):
                ObjectContainer<T[size]>(*Constructor<T[size]>::inplace_allocate(obj, arg)){

        }

        ObjectContainerInPlace(T_array& obj,  T* arg):
                ObjectContainer<T[size]>(*Constructor<T[size]>::inplace_allocate(obj, arg)){

        }

        ~ObjectContainerInPlace(){
        }

    };

    template<typename T, typename ...Args>
    struct ObjectContainerConstructed: public ObjectContainer<T>{
        ObjectContainerConstructed(Args ...args):_constructed(std::forward<typename extent_as_pointer<Args>::type>(args)...),
        ObjectContainer<T>(_constructed){

        }
    private:
        T _constructed;
    };

    //Reference types should just capture the reference, so same as base class behavior:
    template<typename T, typename ...Args>
    struct ObjectContainerConstructed<T&, Args...>: public ObjectContainer<T>{
        ObjectContainerConstructed(Args ...args):ObjectContainer<T>(std::forward<typename extent_as_pointer<Args>::type>(args)...){

        }
    };
    template<typename T, typename ...Args>
    struct ObjectContainerConstructed<T&&, Args...>: public ObjectContainer<T>{
        ObjectContainerConstructed(Args ...args):ObjectContainer<T>(std::forward<typename extent_as_pointer<Args>::type>>(args)...){

        }
    };

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