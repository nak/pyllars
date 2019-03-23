#ifndef __PYLLARS__CONTAINMENT__H
#define __PYLLARS__CONTAINMENT__H

#include <functional>
#include "pyllars_type_traits.hpp"

namespace __pyllars_internal{

    //////////////////////////////////////
    // Containers for Cobjects in varoious forms.  Containers allow for a wrapper that can
    // convert what would normally be a compil-time error in C to a more Pythonic run-time error
    /////////////////////////////////////

    namespace {

        /**
         * Helper for dealing with schizoprhenic fixed arrays in c (is it a pointer or an object with contigous memory?)
         * @tparam T: type of element in array
         * @tparam size : size of array
         */
        template<typename T>
        struct FixedArrayHelper;

        template<typename T, size_t size>
        struct FixedArrayHelper<T[size]>{
            typedef T T_array[size];

            void * operator new(const std::size_t count,  T_array& from){
                auto bytes = ::operator new(count);
                T* values = reinterpret_cast<T*>(bytes);
                for(int i = 0; i < size; ++i){
                    new (values+i) T(from[i]);
                }
                return bytes;
            }
            void * operator new(const std::size_t count,  const T* const from){
                auto bytes = ::operator new(count);
                T* values = reinterpret_cast<T*>(bytes);
                for(int i = 0; i < size; ++i){
                    new (values+i) T(from[i]);
                }
                return bytes;
            }
            T value[size];
        };

    }

    /////////////////////////////////////////////////////

    // DESTRUCTOR LOGIC
    // Converts what would normally be an exception on deletion of an object that is indestructible by the rules of C
    // to a run-time error for Python

    template <typename T>
    struct Destructor{
        static void inplace_destrcuct(T& obj){
            if constexpr (std::is_destructible<T>::value){
                if constexpr(std::is_array<T>::value){
                    if constexpr(ArraySize<T>::size > 0){
                        for(size_t i = 0; i < ArraySize<T>::size; ++i){
                            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
                            Destructor<T_element>::inplace_destrcuct(obj[i]);
                        }
                    }
                } else {
                    obj.~T();
                }
            }
        }

        static void deallocate(T* &obj, const bool asArray){
            if constexpr (std::is_destructible<T>::value){
                if (asArray){
                    delete [] obj;
                } else {
                    delete obj;
                }
            }
            obj = nullptr;
        }

        static void deallocate(T* const &obj, const bool asArray){
            if constexpr(std::is_destructible<T>::value) {
                if (asArray) {
                    delete[] obj;
                } else {
                    delete obj;
                }
            } else {
                PyErr_Warn(PyExc_RuntimeWarning, "Attempt to deallocate non-destructible instance");
            }
        }
    };


    ///////////////////////////////////

    // CONSTRUCTOR Logic
    // converts what would be a compile-time error for non-constructible occurrences into run-time erorrs
    // for compatibility in Python

    template<typename T>
    struct Constructor{
        template<typename ...Args>
        static T* inplace_allocate(T& obj, Args ...args){
            if constexpr (std::is_constructible<T>::value){
                if(std::is_array<T>::value){
                    if (ArraySize<T>::size > 0){
                        for (size_t i = 0; i < ArraySize<T>::size; ++i){
                            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
                            auto objs = (T_element*)obj;
                            Constructor<T_element>::inplace_allocate(objs[i], args...);
                        }
                        return &obj;
                    }
                    throw "Request to instantiate non-constructible object";
                } else {
                    return new ((void*)&obj)T(std::forward<typename extent_as_pointer<Args>::type>>(args)...);
                }
            }
            throw "Request to instantiate non-constructible object";
        }

        template<typename ...Args>
        static T* allocate(Args ...args){
            if constexpr (std::is_constructible<T>::value){
                if(std::is_array<T>::value){
                    if (ArraySize<T>::size > 0){
                        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
                        return new T_element[ArraySize<T>::size]{T(std::forward<typename extent_as_pointer<Args>::type>>(args)...)};
                    }
                    throw "Request to instantiate object of unknown size";
                } else {
                    return new T(std::forward<typename extent_as_pointer<Args>::type>>(args)...);
                }
            }
            throw "Request to instantiate non-constructible object";
        }


        template<typename ...Args>
        static T* allocate_array(const size_t size, Args ...args){
            if constexpr (std::is_constructible<T>::value){
                if constexpr (std::is_array<T>::value){
                    if constexpr (ArraySize<T>::size > 0){
                        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
                        return new T_element[size][ArraySize<T>::size]{{T(std::forward<typename extent_as_pointer<Args>::type>>(args)...)}};
                    }
                    throw "Request to instantiate object of unknown size";
                } else {
                    return new T[size]{T(std::forward<typename extent_as_pointer<Args>::type>>(args)...)};
                }
            }
            throw "Request to instantiate non-constructible object";
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

        virtual explicit operator T_NoRef&() {return *fake;} // gcc complains in some cses if pure virtual :-/

        virtual explicit operator const T_NoRef&() const {return *fake;} // gcc complains in some cses if pure virtual :-/

        virtual T_NoRef * ptr(){
            return nullptr;  //gcc complains if we make this virtual and use unnamed type such as an enum
        }

        virtual ~ObjectContainer() = default;

    protected:
        //substitute for not being allowed to make abstract
        ObjectContainer() = default;

    private:
        ObjectContainer(const ObjectContainer &) = delete;
        static constexpr T_NoRef* fake =  nullptr;
    };

    /**
     * Constainer for a reference-to-an-object
     * Client is responsible to ensure lifetime of given object exceeds lifetime of container
     */
    template<typename T>
    struct ObjectContainerReference: public ObjectContainer<T>{
        typedef typename std::remove_reference<T>::type T_NoRef;

        explicit ObjectContainerReference(T_NoRef & obj):ObjectContainer<T>(), _contained(obj), _containedP(&_contained){
        }

        virtual ~ObjectContainerReference(){
            _containedP = nullptr;
        }


        explicit operator T_NoRef&(){
            if(!_containedP) throw "Attempt to dereference null C object";
            return _contained;
        }

        explicit operator const T_NoRef&() const{
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


    template<typename T>
    struct ObjectContainerAllocatedInstance: public ObjectContainerReference<T>{
        typedef typename std::remove_reference<T>::type T_NoRef;

        template<typename ...Args>
        static ObjectContainerAllocatedInstance*  new_container(Args ...args){
            if constexpr (std::is_constructible<T>::value) {
                if constexpr (std::is_array<T>::value){
                    if constexpr(ArraySize<T>::size > 0){
                        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
                        typedef typename std::remove_const<T_element>::type T_nonconst_array[ArraySize<T>::size];
                        static constexpr size_t arg_length = sizeof...(Args);
                        static constexpr const bool is_same[] = {std::is_same<typename std::is_reference<Args>::type,
                                                                 typename std::remove_reference<T>::type>::value...};
                        static constexpr const bool is_sameptr[] = {std::is_same<typename extent_as_pointer<typename std::remove_reference<Args>::type>::type,
                                                                    typename extent_as_pointer<typename std::remove_reference<T>::type>::type >::value...};
                        if constexpr(arg_length==1 && (is_same[0] || is_sameptr[0])){
                            auto new_raw_value = new (args...) FixedArrayHelper<T>();
                            auto new_value = reinterpret_cast<T*>(new_raw_value);
                            return ObjectContainerAllocatedInstance::new_container(new_value);

                        } else {
                            T_nonconst_array *new_value = new T_element[1][ArraySize<T>::size]{{T(args...)}};
                            return ObjectContainerAllocatedInstance::new_container(new_value, true);
                        }
                    }
                } else {
                    T * new_value = new T(args...);
                    return ObjectContainerAllocatedInstance::new_container(new_value, false);
                }
            }
            throw "Request to allocate non-constructible type";
        }


        static ObjectContainerAllocatedInstance*  new_container(T_NoRef* already_allocated){
            return new ObjectContainerAllocatedInstance(already_allocated);
        }

    private:

        explicit ObjectContainerAllocatedInstance(T_NoRef* obj):ObjectContainerReference<T>(*obj), _obj(obj){
        }

        virtual ~ObjectContainerAllocatedInstance(){
            Destructor<T_NoRef>::deallocate(_obj, false);
        }

        T_NoRef * _obj;
    };


    template<typename T>
    struct ObjectContainerAllocated;

    template<typename T>
    struct ObjectContainerAllocated<T*>: public ObjectContainerReference<T*>{

        template<typename ...Args>
        static ObjectContainerAllocated*  new_container(Args ...args){
            if constexpr (std::is_constructible<T>::value) {
                if constexpr (std::is_array<T>::value){
                    if constexpr(ArraySize<T>::size > 0){
                        auto new_helper_value = new (args...) FixedArrayHelper<T>();
                        auto new_value = reinterpret_cast<T*>(new_helper_value);
                        return ObjectContainerAllocated::new_container(new_value, false);
                    }
                } else {
                    T * new_value = new T(args...);
                    return ObjectContainerAllocated::new_container(new_value, false);
                }
            }
            throw "Request to allocate non-constructible type";
        }


        static ObjectContainerAllocated*  new_container(T* already_allocated, const bool asArray=false){
            return new ObjectContainerAllocated(already_allocated, asArray);
        }

    private:

        explicit ObjectContainerAllocated(T* obj, const bool asArray):ObjectContainerReference<T*>(obj), _obj(obj),
        _asArray(asArray){
        }

        virtual ~ObjectContainerAllocated(){
            Destructor<T>::deallocate(_obj, _asArray);
        }

        T * _obj;
        const bool _asArray;
    };


    template<typename T>
    struct ObjectContainerAllocated<T* const>: public ObjectContainerReference<T* const>{

        template<typename ...Args>
        static ObjectContainerAllocated*  new_container(Args ...args){
            if constexpr (std::is_constructible<T>::value) {
                if constexpr (std::is_array<T>::value){
                    if constexpr(ArraySize<T>::size > 0){
                        auto new_helper_value = new (args...) FixedArrayHelper<T>();
                        auto new_value = reinterpret_cast<T* const>(new_helper_value);
                        return ObjectContainerAllocated::new_container(new_value, false);
                    }
                } else {
                    T * new_value = new T(args...);
                    return ObjectContainerAllocated::new_container(new_value, false);
                }
            }
            throw "Request to allocate non-constructible type";
        }


        static ObjectContainerAllocated*  new_container(T* const already_allocated, const bool asArray=false){
            return new ObjectContainerAllocated(already_allocated, asArray);
        }

    private:

        explicit ObjectContainerAllocated(T* const obj, const bool asArray):ObjectContainerReference<T* const>(obj),
                _obj(obj),
                _asArray(asArray){
        }

        virtual ~ObjectContainerAllocated(){
            Destructor<T>::deallocate(ObjectContainerReference<T* const>::_contained, _asArray);
        }

        T * const _obj;
        const bool _asArray;
    };

    //////////////////////////

    template<typename T>
    struct ObjectContainerBytePool;

    template<typename T>
    struct ObjectContainerBytePool<T*>: public ObjectContainer<T*>{

        ObjectContainerBytePool( const size_t arraySize, const std::function<void(void*, size_t)> &construct_element ):
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

        explicit operator T*&(){
            if(!_raw_bytes) throw "Attempt to dereference null C object";
            return _contained;
        }

        explicit operator T* const&() const{
            if(!_raw_bytes) throw "Attempt to dereference null C object";
            return _contained;
        }

        T** ptr(){
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
            T *_contained;
        };
        const size_t _size;
    };


    template<typename T>
    struct ObjectContainerBytePool<T* const>: public ObjectContainer<T* const>{

        ObjectContainerBytePool( const size_t arraySize, const std::function<void(void*, size_t)> &construct_element ):
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

        T& operator*(){
            return *_contained;
        }

        const T& operator*() const{
            return *_contained;
        }

        explicit operator T*&(){
            if(!_raw_bytes) throw "Attempt to dereference null C object";
            return _contained;
        }

        explicit operator T* const&() const{
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

        explicit ObjectContainerBytePool(const std::function<void(void*, size_t)> &construct_element ):
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

        explicit operator T*&(){
            if(!_raw_bytes) throw "Attempt to dereference null C object";
            return _contained;
        }

        explicit operator T* const&() const{
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
    struct ObjectContainerConstructed: public ObjectContainerReference<T>{
        explicit ObjectContainerConstructed(Args ...args):_constructed(std::forward<typename extent_as_pointer<Args>::type>(args)...),
        ObjectContainerReference<T>(_constructed){

        }
    private:
        T _constructed;
    };

    //Reference types should just capture the reference, so same as base class behavior:
    template<typename T, typename ...Args>
    struct ObjectContainerConstructed<T&, Args...>: public ObjectContainerReference<T>{
        explicit ObjectContainerConstructed(Args ...args):ObjectContainerReference<T>(std::forward<typename extent_as_pointer<Args>::type>(args)...){

        }
    };
    template<typename T, typename ...Args>
    struct ObjectContainerConstructed<T&&, Args...>: public ObjectContainerReference<T>{
        explicit ObjectContainerConstructed(Args ...args):ObjectContainerReference<T>(std::forward<typename extent_as_pointer<Args>::type>>(args)...){

        }
    };

    template<size_t size, typename T>
    struct ObjectContainerConstructed<T[size], T[size]>: public ObjectContainerReference<T[size]>{
        explicit ObjectContainerConstructed(T args[size]):ObjectContainerReference<T[size]>(((FixedArrayHelper<T[size]>*)&_constructed)->value){
            for(size_t i = 0; i < size; ++i){
                new (&_constructed[0] + sizeof(T)*i) T(args[i]);
            }
        }
    private:
        unsigned char _constructed[size*sizeof(T)]{};
    };

    template<typename T>
    struct ObjectContainerPyReference: public ObjectContainer<T>{

        ~ObjectContainerPyReference() = default;

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

        explicit operator T*&() override{
            return _cobj;
        }

        explicit operator T* const&() const override{
            return _cobj;
        }

        T ** ptr() override{
            return (T**) &_cobj;
        }

    private:
        PyObject* _pyobj{};
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

        explicit operator T* const&() override{
            return _cobj;
        }

        explicit operator T* const&() const override{
            return _cobj;
        }

        T * const * ptr() override{
            return &_cobj;
        }

    private:
        PyObject* _pyobj{};
        T* const _cobj;
    };
}
#endif