#ifndef __PYLLARS_INTERNAL_DEFNS
#define __PYLLARS_INTERNAL_DEFNS

#include <type_traits>
#include <sys/types.h>
#include <Python.h>

#include "pyllars_utils.hpp"
#include <map>

namespace __pyllars_internal {

    namespace {
        constexpr size_t MAX_PTR_DEPTH = 5;
        constexpr int ERROR_TYPE_MISMATCH = -2;
        constexpr ssize_t UNKNOWN_SIZE = -1;
        static constexpr Py_ssize_t INVALID_INDEX = LONG_LONG_MIN;
        //basic constants (names and such)
        extern const char address_name[] = "this";
        extern const char alloc_name_[] = "new";
    }



    template<typename T>
    struct _Types{
        static const char* const type_name();
    };

    template<>
    struct _Types<double>{
        static const char* const type_name();
    };

    template<>
    struct _Types<float>{
        static const char* const type_name();
    };

    template<>
    struct _Types<char>{
        static const char* const type_name();
    };

    template<>
    struct _Types<short>{
        static const char* const type_name();
    };

    template<>
    struct _Types<int>{
        static const char* const type_name();
    };

    template<>
    struct _Types<long>{
        static const char* const type_name();
    };



    template<>
    struct _Types<long long>{
        static const char* const type_name();
    };


    template<>
    struct _Types<unsigned char>{
        static const char* const type_name();
    };

    template<>
    struct _Types<unsigned short>{
        static const char* const type_name();
    };

    template<>
    struct _Types<unsigned int>{
        static const char* const type_name();
    };

    template<>
    struct _Types<unsigned long>{
        static const char* const type_name();
    };

    template<>
    struct _Types<unsigned long long>{
        static const char* const type_name();
    };

    template<typename T>
    struct Types{
        static const char* const type_name(){
            return _Types<T>::type_name();
        }
    };

    template<typename T>
    struct Types<const T>{
        static const char* const type_name(){
            static const std::string namebase = std::string("const_") + std::string(Types<typename std::remove_const<T>::type>::type_name());
            return namebase.c_str();
        }
    };

    template<typename T>
    struct Types<volatile T>{
        static const char* const type_name(){
            static const std::string namebase = std::string("volatile_") + std::string(Types<typename std::remove_volatile<T>::type>::type_name());
            return namebase.c_str();
        }
    };

    template<typename T>
    struct Types<const volatile T>{
        static const char* const type_name(){
            static const std::string namebase = std::string("const_volatile_") + std::string(Types<typename std::remove_volatile<T>::type>::type_name());
            return namebase.c_str();
        }
    };

    template<typename T>
    struct Types<T*>{
        static const char* const type_name(){
	  static std::string namebase = std::string(Types<typename std::remove_pointer<T>::type>::type_name()) + '*';
            return namebase.c_str();
        }
    };

    template<typename T>
    struct Types<const T*>{
        static const char* const type_name(){
            static std::string namebase = std::string("const_") + std::string(Types<typename std::remove_pointer<T>::type>::type_name()) + '*';
            return namebase.c_str();
        }
    };

    template<typename T>
    struct Types<volatile T*>{
        static const char* const type_name(){
            static std::string namebase = std::string("volatile_") + std::string(Types<typename std::remove_pointer<T>::type>::type_name()) + '*';
            return namebase.c_str();
        }
    };

    template<typename T>
    struct Types<const volatile T*>{
        static const char* const type_name(){
            static std::string namebase = std::string("const_volatile") + std::string(Types<typename std::remove_pointer<T>::type>::type_name()) + '*';
            return namebase.c_str();
        }
    };

    template<typename T>
    struct Types<T&>{
        static const char* const type_name(){
	  static std::string namebase = std::string(Types<typename std::remove_reference<T>::type>::type_name())+ '&';
            return namebase.c_str();
        }
    };

    template<typename T>
    struct Types<const T&>{
        static const char* const type_name(){
            static std::string namebase = std::string("const_") + std::string(Types<typename std::remove_reference<T>::type>::type_name())+ '&';
            return namebase.c_str();
        }
    };

    template<typename T>
    struct Types<volatile T&>{
        static const char* const type_name(){
            static std::string namebase = std::string("volatile_") + std::string(Types<typename std::remove_reference<T>::type>::type_name())+ '&';
            return namebase.c_str();
        }
    };

    template<typename T>
    struct Types<const volatile T&>{
        static const char* const type_name(){
            static std::string namebase = std::string("const_volatile_") + std::string(Types<typename std::remove_reference<T>::type>::type_name())+ '&';
            return namebase.c_str();
        }
    };

    template<typename T>
    struct Types<T[]>{
        static const char* const type_name(){
            static std::string namebase = std::string(Types<typename std::remove_reference<T>::type>::type_name())+ "[]";
            return namebase.c_str();
        }
    };

    template<typename T>
    struct Types<const T[]>{
        static const char* const type_name(){
            static std::string namebase = std::string("const_") + std::string(Types<typename std::remove_reference<T>::type>::type_name())+ "[]";
            return namebase.c_str();
        }
    };

    template<typename T>
    struct Types<volatile T[]>{
        static const char* const type_name(){
            static std::string namebase = std::string("volatile_") + std::string(Types<typename std::remove_reference<T>::type>::type_name())+ "[]";
            return namebase.c_str();
        }
    };

    template<typename T>
    struct Types<const volatile T[]>{
        static const char* const type_name(){
            static std::string namebase = std::string("const_volatile_") + std::string(Types<typename std::remove_reference<T>::type>::type_name())+ "[]";
            return namebase.c_str();
        }
    };

    template<typename T, size_t size>
    struct Types<T[size]>{
        static const char* const type_name(){
            static std::string namebase = std::string(Types<typename std::remove_reference<T>::type>::type_name())+ '[' + std::to_string(size) + ']';
            return namebase.c_str();
        }
    };

    template<typename T, size_t size>
    struct Types<const T[size]>{
        static const char* const type_name(){
            static std::string namebase = std::string("const_") + std::string(Types<typename std::remove_reference<T>::type>::type_name())+ '[' + std::to_string(size) + ']';
            return namebase.c_str();
        }
    };

    template<typename T, size_t size>
    struct Types<volatile T[size]>{
        static const char* const type_name(){
            static std::string namebase = std::string("volatile_") + std::string(Types<typename std::remove_reference<T>::type>::type_name())+ '[' + std::to_string(size) + ']';
            return namebase.c_str();
        }
    };

    template<typename T, size_t size>
    struct Types<const volatile T[size]>{
        static const char* const type_name(){
            static std::string namebase = std::string("const_volatile_") + std::string(Types<typename std::remove_reference<T>::type>::type_name())+ '[' + std::to_string(size) + ']';
            return namebase.c_str();
        }
    };

    template<typename T>
    struct Types<T&&>{
        static const char* const type_name(){
	  static std::string namebase = std::string(Types<typename std::remove_reference<T>::type>::type_name()) + "&&";
            return namebase.c_str();
        }
     };

    template<typename T>
    struct Types<const T&&>{
        static const char* const type_name(){
            static std::string namebase = std::string("const_") + std::string(Types<typename std::remove_reference<T>::type>::type_name()) + "&&";
            return namebase.c_str();
        }
    };
    template<typename T>
    struct Types<volatile T&&>{
        static const char* const type_name(){
            static std::string namebase = std::string("volatile_") + std::string(Types<typename std::remove_reference<T>::type>::type_name()) + "&&";
            return namebase.c_str();
        }
    };
    template<typename T>
    struct Types<const volatile T&&>{
        static const char* const type_name(){
            static std::string namebase = std::string("const_volatile_") + std::string(Types<typename std::remove_reference<T>::type>::type_name()) + "&&";
            return namebase.c_str();
        }
    };


    template<>
    struct Types<void>{
        static const char* const type_name(){
            static const char* const name = "void";
            return name;
        }
    };

    template<>
    struct Types<bool> {
        static const char *const type_name(){
            static const char* const name = "c_bool";
            return name;
        }
    };

    template<typename T>
    const char* const type_name(){
        return Types<T>::type_name();
    }

    ///////////
    // Helper conversion functions
    //////////
    template<typename T, typename E = void>
    PyObject *toPyObject(T &var, const bool asArgument, const ssize_t array_size);

    template<typename T,  typename E = void>
    PyObject *toPyObject(const T &var, const bool asArgument, const ssize_t array_size);

  //  template<typename T, bool array_allocated, typename ClassWrapper>
  //  smart_ptr<typename std::remove_reference<T>::type, array_allocated> toCObject(PyObject &pyobj);

    struct ZERO_DEPTH;

    template<typename T>
    struct rm_ptr {
        typedef ZERO_DEPTH type;
    };

    template<typename T>
    struct rm_ptr<T *> {
        typedef T type;
    };


    template <typename T, typename E = void>
    struct PythonClassWrapper;

  template<>
  class PythonClassWrapper<const float>;


  template<>
  class PythonClassWrapper<const double>;

    /**
     * Class common to all C++ wrapper classes
     **/
    struct CommonBaseWrapper {

        struct Base {
            PyObject_HEAD
            /*Per Python API docs*/

            typedef PyTypeObject *TypePtr_t;

            static TypePtr_t constexpr TypePtr = &PyBaseObject_Type;

        } baseClass;

        constexpr CommonBaseWrapper() : baseClass(), _referenced(nullptr) {
        }
        typedef const char* const cstring;
        static constexpr cstring tp_name_prefix = "[*pyllars*] ";
        static constexpr size_t tp_name_prefix_len = strlen(tp_name_prefix);

        static bool IsClassType( PyObject* obj){
            PyTypeObject* pytype = (PyTypeObject*) PyObject_Type(obj);
            return strncmp( pytype->tp_name, tp_name_prefix, tp_name_prefix_len) == 0;
        }

        static constexpr cstring ptrtp_name_prefix = "[*pyllars:ptr*] ";
        static constexpr size_t ptrtp_name_prefix_len = strlen(ptrtp_name_prefix);

        static bool IsCFunctionType( PyObject* obj){
            PyTypeObject* pytype = (PyTypeObject*) PyObject_Type(obj);
            return strncmp( pytype->tp_name, ptrtp_name_prefix, ptrtp_name_prefix_len) == 0;
        }

        void make_reference(PyObject *obj) {
            if (_referenced) { Py_DECREF(_referenced); }
            if (obj) { Py_INCREF(obj); }
            _referenced = obj;
        }

        PyObject *getReferenced() {
            return _referenced;
        }

    protected:
        PyObject *_referenced;

    };



    template<typename T, bool is_array, const ssize_t array_size, typename E = void>
    PyObject *set_array_values(T values, const ssize_t size, PyObject *fromTuple, PyObject *referenced);

    template<typename T,  bool is_array, const ssize_t array_size, typename std::enable_if<std::is_assignable<T, T>::value>::type>
    PyObject *set_array_values(T *values, const ssize_t size, PyObject *from, PyObject *referenced);

    template<>
    PyObject *set_array_values<const char **, false, -1, void>(const char **values, const ssize_t size, PyObject *from,
                                                               PyObject *referenced) ;

    template<bool varargs, typename ReturnType, typename ...Args>
    struct FuncContainer;

    template<typename ReturnType, typename ...Args>
    struct FuncContainer<false, ReturnType, Args...> {
    public:
        template<int unused, typename ...Throws>
        struct Type{
        public:
            ReturnType(*_cfunc)(Args...) throw(Throws...);
            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct Type<unused, void>{
            ReturnType(*_cfunc)(Args...) throw();
            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct Type<unused> {
            ReturnType(*_cfunc)(Args...) throw();
            typedef decltype(_cfunc) func_type;
        };
    };

    template<typename ReturnType, typename ...Args>
    struct FuncContainer<true, ReturnType, Args...> {

        template<int unused, typename ...Throws>
        struct Type{
            ReturnType(*_cfunc)(Args... ...) throw(Throws...);
            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct Type<unused, void>{
            ReturnType(*_cfunc)(Args... ...) throw();
            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct Type<unused> {
            ReturnType(*_cfunc)(Args... ...);
            typedef decltype(_cfunc) func_type;
        };
    };

    template<typename T, typename E = void>
    struct Factory{
        PyObject* create_single_instance(T& obj, const bool inPlace);
    };

    template<typename T>
    struct Factory<T, typename std::enable_if<std::is_integral<T>::value>::type >{
        PyObject* create_single_instance(const T& value, const bool inPlace){
            return PyLong_FromLong(value);
        }
    };

    template<typename T>
    struct Factory<T, typename std::enable_if<std::is_floating_point<T>::value>::type >{
        PyObject* create_single_instance(T& value, const bool inPlace){
            return PyFloat_FromDouble(value);
        }
    };

    template<typename T>
    struct Factory<T, typename std::enable_if<!std::is_fundamental<T>::value>::type>{
        PyObject* create_single_instance(T& value, const bool inPlace){
         return PythonClassWrapper<T>::createPy(1, value, inPlace);
         }
    };
}
#endif
