#ifndef __PYLLARS__INTERNAL__UTILS_H
#define __PYLLARS__INTERNAL__UTILS_H

#include <structmember.h>
#include <memory>


#undef NULL
#define NULL nullptr
/**
 * Some (perhaps odd) utility classes to aide in converting calls with python tuples
 * to full-blown argument lists used in C function calls
 **/
namespace __pyllars_internal {

    //type trait to determine if types is a function pointer
    template<typename T>
    struct is_function_ptr {
        static constexpr bool value = false;
    };


    template<typename T, typename...Args>
    struct is_function_ptr<T(*)(Args...)> {
        static constexpr bool value = true;
    };
    template<typename T, typename...Args>
    struct is_function_ptr<T(*const)(Args...)> {
        static constexpr bool value = true;
    };

    //type trait to determine depth of pointer
    // ptr_depth<int> = 0
    // ptr_depth<int*> = ptr_depth<int[]> = ptr_depth<int[size]> = 1
    // ptr_depth<int**> = 2
    // ...
    template<typename T>
    struct ptr_depth {
        static constexpr size_t value = 0;
    };

    template<typename T>
    struct ptr_depth<T *> {
        static constexpr size_t value = ptr_depth<T>::value + 1;
    };

    template<typename T>
    struct ptr_depth<T *const> {
        static constexpr size_t value = ptr_depth<T>::value + 1;
    };

    template<typename T, size_t size>
    struct ptr_depth<T[size]> {
        static constexpr size_t value = ptr_depth<T>::value + 1;
    };

    template<typename T, size_t depth>
    struct ptr_of_depth {
        typedef typename ptr_of_depth<T, depth - 1>::type *type;
    };

    template<typename T>
    struct ptr_of_depth<T, 0> {
        typedef T type;
    };

    template<typename T, size_t depth, size_t size>
    struct ptr_of_depth<T[size], depth> {
        typedef typename ptr_of_depth<T, depth - 1>::type type[size];
    };


    //convertion of type with extent to pointer
    // extent_as_pointer<T[]> = extent_as_pointer<T[size]> = T*
    template<typename T>
    struct extent_as_pointer {
        typedef T type;
    };

    template<typename T>
    struct extent_as_pointer<T *> {
        //typedef typename extent_as_pointer<T>::type *type;
        typedef T *type;
    };

    template<typename T>
    struct extent_as_pointer<T[]> {
        // typedef typename extent_as_pointer<T>::type *type;
        typedef T *type;
    };
    template<typename T>
    struct extent_as_pointer<const T[]> {
        // typedef const typename extent_as_pointer<T>::type *type;
        typedef const T *type;
    };

    template<typename T, const size_t max>
    struct extent_as_pointer<T[max]> {
        // typedef typename extent_as_pointer<T>::type *type;
        typedef T *type;
    };

    template<typename T, const size_t max>
    struct extent_as_pointer<const T[max]> {
        //  typedef const typename extent_as_pointer<T>::type *type;
        typedef const T *type;
    };

    /**
     * container class for holding a tuple, to be expanded to an argument list
    **/
    template<int ...S>
    struct container {
    };

    /**
     * Counter class to expand arguments from a tuple object
     * to a set of arguments passed to a function
     **/
    template<int N, int ...S>
    struct argGenerator : argGenerator<N - 1, N - 1, S...> {

    };

    /**
     * 0th order specialization
    **/
    template<int ...S>
    struct argGenerator<0, S...> {
        typedef container<S...> type;

    };


    /**
     *  Types used to define a smart pointer that knows if contained
     *  element is dynamically allocated and should be deleted (or not)
     */
    template<typename T, bool array_allocated, typename E = void>
    struct smart_delete;

    template<typename T, bool array_allocated>
    struct smart_delete<T, array_allocated, typename std::enable_if<!std::is_destructible<T>::value>::type> {

        typedef typename std::remove_reference<T>::type T_base;

        smart_delete(const bool deleteable) : _deleteable(deleteable) {
        }

        void operator()(T_base *ptr) const {
            // if (_deleteable) delete ptr;
        }

        const bool _deleteable;
    };


    template<typename T>
    struct smart_delete<T, false, typename std::enable_if<std::is_destructible<T>::value>::type> {

        typedef typename std::remove_reference<T>::type T_base;

        smart_delete(const bool deleteable) : _deleteable(deleteable) {
        }

        void operator()(T_base *ptr) const {
            if (_deleteable) {
                delete ptr;
            }
        }

        const bool _deleteable;
    };

    template<typename T>
    struct smart_delete<T, true, typename std::enable_if<std::is_destructible<T>::value > ::type >{

        typedef typename std::remove_reference<T>::type T_base;

        smart_delete(const bool deleteable) : _deleteable(deleteable) {
        }

        void operator()(T_base *ptr) const {
             if(_deleteable){
                delete[] ptr;
            }
        }

        const bool _deleteable;
    };


    template<typename T, bool array_allocated>
    using smart_ptr = std::unique_ptr<typename std::remove_reference<T>::type, smart_delete<T, array_allocated> >;


    /**
     *  HACK(?) to determine if type is complete or not
     */
    struct char256 {
        char x[256];
    };


    template<typename T>
    char256 is_complete_helper(int(*)[sizeof(T)]);

    template<typename>
    char is_complete_helper(...);

    template<typename T>
    struct is_complete {
        enum {
            value = sizeof(is_complete_helper<T>(0)) != 1
        };
    };

    template<>
    struct is_complete<void> {
        enum {
            value = 1
        };
    };

    template<typename T>
    struct ArraySize {
        static const int size = 1;
    };

    template<typename T>
    struct ArraySize<T *> {
        static const int size = -1;
    };

    template<typename T>
    struct ArraySize<T[]> {
        static const int size = -1;
    };

    template<typename T_base, size_t arsize>
    struct ArraySize<T_base[arsize]> {
        static const int size = arsize;
    };

    template<typename T, typename Z = void>
    struct Sizeof;

    template<typename T>
    struct Sizeof<T, typename std::enable_if<is_complete<T>::value>::type> {
        static constexpr size_t value = sizeof(T);
    };

    template<typename T>
    struct Sizeof<T, typename std::enable_if<!is_complete<T>::value>::type> {
        static constexpr size_t value = 0;
    };

    template<typename T, const size_t bits, typename E = void>
    struct BitFieldLimits;

    template<typename T, const size_t bits>
    struct BitFieldLimits<T, bits, typename std::enable_if<
            std::numeric_limits<T>::is_signed && std::is_integral<T>::value>::type> {
        static constexpr int64_t lower = -(1ull << bits);
        static constexpr int64_t upper = -lower + 1;

        static bool is_in_bounds(const T &value) { return (value >= lower) && (value <= upper); }
    };

    template<typename T, const size_t bits>
    struct BitFieldLimits<T, bits, typename std::enable_if<
            !std::numeric_limits<T>::is_signed && std::is_integral<T>::value>::type> {
        static constexpr uint64_t lower = 0;
        static constexpr uint64_t upper = (1ull << bits) - 1;

        static bool is_in_bounds(const T &value) { return (value >= lower) && (value <= upper); }
    };

    template<typename T, const size_t bits>
    struct BitFieldLimits<T, bits, typename std::enable_if<!std::is_integral<T>::value>::type> {
        static bool is_in_bounds(const T &value) { return true;/*no meaning here, so always return true*/}
    };
}


namespace __pyllars_internal {


    template<typename T, typename E =void>
    struct Deallocator {
        static void dealloc(T *const ptr, const bool in_place, const size_t size);
    };

    template<typename T>
    struct Deallocator<T, typename std::enable_if<!is_function_ptr<T>::value && !std::is_reference<T>::value &&
                                                        std::is_destructible<T>::value && !std::is_array<T>::value>::type > {
        static void dealloc(T *const ptr, const bool in_place, const size_t size) {
            delete ptr;
        }
    };


    template<typename T>
    struct Deallocator<T, typename std::enable_if<!is_function_ptr<T>::value &&
                                                  !std::is_reference<T>::value &&
                                                  std::is_destructible<T>::value &&
                                                  std::is_array<T>::value &&
                                                  !std::is_array<typename std::remove_pointer<T>::type>::value
                                                  >::type > {

        static void dealloc(T *const ptr, const bool in_place, const size_t size ) {
            if (in_place){
                for(size_t i = 0; i < size; ++i){
                    if(ptr[i]) ptr[i].~T();
                }
            } else {
                delete[] ptr;
            }
        }
    };

    template<typename T>
    struct Deallocator<T, typename std::enable_if<!is_function_ptr<T>::value &&
                                                  !std::is_reference<T>::value &&
                                                  std::is_destructible<T>::value &&
                                                  std::is_array<T>::value &&
                                                  std::is_array<typename std::remove_pointer<T>::type>::value
                                                 >::type > {

        static void dealloc(T *const ptr, const bool in_place, const size_t size ) {
                delete[] ptr;
        }
    };


    template<typename T>
    struct Deallocator<T, typename std::enable_if<
            !is_function_ptr<T>::value && !std::is_reference<T>::value &&
            !std::is_destructible<T>::value>::type> {
        static void dealloc(T *const ptr, const bool in_place, const size_t size) {
        }
    };

    template<typename T>
    struct Deallocator<T, typename std::enable_if<
            is_function_ptr<T>::value || std::is_reference<T>::value>::type> {
        static void dealloc(T *const ptr, const bool in_place, const size_t size) {
        }
    };

    template<typename T>
    struct ObjContainer {


        operator T &() {
            return contained;
        }


        virtual ~ObjContainer() {
        }


        T *ptr() {
            return &contained;
        }

        T **ptrptr() {
            return &containedP;
        }

        template< typename ...Args>
        void set_contents(Args... args){
            new (&contained) T(args...);
        }

    protected:

      ObjContainer(T &obj, const size_t size, const bool isAllocated) :
                contained(obj),
                containedP(&obj),
                _size(size),
                _isAllocated(isAllocated){
        }

        T &contained;
        T *containedP;
        const size_t _size;
        const bool _isAllocated;
    };

    template<>
    struct ObjContainer<void> {
      ObjContainer(void* const ptr, const size_t size, const bool isAllocated) :
	containedP(ptr), _size(size),_isAllocated(isAllocated){

        }

        virtual void *ptr() {
            return nullptr;
        }

        virtual void **ptrptr() {
            static void *NO_PTR = nullptr;
            return &NO_PTR;
        }
    protected:
        void * containedP;
        size_t _size;
        bool _isAllocated;
    };


    template<>
    struct ObjContainer<const void> {
      ObjContainer(const void* ptr, const size_t size, const bool isAllocated) :containedP(ptr),_size(size), _isAllocated(isAllocated){

        }

        virtual const void *ptr() {
            return nullptr;
        }

        virtual const void **ptrptr() {
            static const void *NO_PTR = nullptr;
            return &NO_PTR;
        }

    protected:
        const void * containedP;
        size_t _size;
        bool _isAllocated;
    };

    template<typename T, bool is_allocated_, bool in_place = false>
    struct ObjContainerPtrProxy : public ObjContainer<T> {
        ObjContainerPtrProxy(T *const ptr, const size_t size, const bool isAllocated = is_allocated_) :
	  ObjContainer<T>(*ptr, size, isAllocated)
                //containedPtr(ptr)
      {
      }

        virtual ~ObjContainerPtrProxy() {
	  if (ObjContainer<T>::_isAllocated ) { Deallocator<T>::dealloc(ObjContainer<T>::containedP, in_place, ObjContainer<T>::_size); }
        }



    private:
    };

    template<bool b>
    struct ObjContainerPtrProxy<void, b> : public ObjContainer<void> {

      ObjContainerPtrProxy(void *const ptr, const size_t size, const bool isAllocated = false) :
	    ObjContainer<void>(ptr, 0, isAllocated){
        }

        virtual ~ObjContainerPtrProxy() {
        }

        virtual void *ptr() {
            return containedP;
        }

        virtual void **ptrptr() {
            return const_cast<void **>(&containedP);
        }

    private:
    };


    template<bool b>
    struct ObjContainerPtrProxy<const void, b> : public ObjContainer<const void> {
      ObjContainerPtrProxy(const void *const ptr, const size_t size, const bool isAllocated = false):
	ObjContainer<const void>(ptr, size, isAllocated){
      }

        virtual ~ObjContainerPtrProxy() {
        }

        virtual const void *ptr() {
            return containedP;
        }

        virtual const void **ptrptr() {
            return const_cast<const void **>(&containedP);
        }

    private:
     };

    template<typename T, typename ...Args>
    struct ObjContainerProxy : ObjContainer<T> {

        ObjContainerProxy(Args ...args) :
	        ObjContainer<T>(contained2, 0, false),
            contained2(args...) {
        }


    private:
        T contained2;
    };

    template<typename t, const size_t size>
    struct ObjContainerProxy<t[size], t[size]> : ObjContainer<t[size]> {

        ObjContainerProxy(t arg[size]) :
	     ObjContainer<t[size]>(contained2, size, false),
                contained2{arg[0]} {
            typedef typename std::remove_const<t>::type unconst_t;
            unconst_t *unconst_contained = const_cast<unconst_t *>(arg);
            for (size_t i = 0; i < size; ++i) {
                unconst_contained[i] = arg[i];
            }
        }

    private:
        t contained2[size];
    };

    template<typename Type>
    struct defaults_to_string{
        static constexpr bool value = std::is_array<Type>::value;
    };

    template<>
    struct defaults_to_string<char*>{
        static constexpr bool value = false;
    };

    template<>
    struct defaults_to_string<const char*>{
        static constexpr bool value = false;
    };

    template<>
    struct defaults_to_string<char* const>{
        static constexpr bool value = false;
    };

    template<>
    struct defaults_to_string<const char* const>{
        static constexpr bool value = false;
    };
}


#endif
