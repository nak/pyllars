#ifndef __PYLLARS_INTERNAL_DEFNS_H_
#define __PYLLARS_INTERNAL_DEFNS_H_

#include <vector>
#include <cstddef>
#include <Python.h>
#include <functional>

#include <pyllars/internal/pyllars_defns.hpp>
#ifdef _MSC_VER
#pragma warning(disable:4251)
#pragma warning(disable:4661)
#endif

#include <type_traits>
#include <map>

#include <Python.h>

#include "pyllars_type_traits.hpp"

#ifdef MSVC
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT
#define MSVC
#else
#define DLLEXPORT
#define DLLIMPORT
#endif

namespace pyllars_internal {
    DLLEXPORT PyObject* NULL_ARGS();

    constexpr size_t ct_strlen( const char* s ) noexcept{
        return *s ? 1 + ct_strlen(s + 1) : 0;
    }

    typedef const char *const cstring;

    namespace {
        constexpr int ERROR_TYPE_MISMATCH = -2;
        constexpr ssize_t UNKNOWN_SIZE = -1;
        //basic constants (names and such)
        extern const char address_name[] = "this";
        extern const char alloc_name_[] = "new";
    }


    template<typename T>
    struct DLLEXPORT TypeInfo {
        static const char type_name[];
    };

    template<>
    struct DLLEXPORT TypeInfo<double> {
        static constexpr cstring type_name = "c_double";
    };

    template<>
    struct DLLEXPORT TypeInfo<float> {
        static constexpr cstring type_name = "c_float";
    };

    template<>
    struct DLLEXPORT TypeInfo<signed char> {
        static constexpr cstring type_name = "c_signed_char";
    };

    template<>
    struct DLLEXPORT TypeInfo<char> {
        static constexpr cstring type_name = "c_char";
    };

    template<>
    struct DLLEXPORT TypeInfo<short> {
        static constexpr cstring type_name = "c_short";
    };

    template<>
    struct DLLEXPORT TypeInfo<int> {
        static constexpr cstring type_name = "c_int";
    };

    template<>
    struct DLLEXPORT TypeInfo<long> {
        static constexpr cstring type_name = "c_long";
    };

    template<>
    struct DLLEXPORT TypeInfo<long long> {
        static constexpr cstring type_name = "c_long_long";
    };

    template<>
    struct DLLEXPORT TypeInfo<unsigned char> {
        static constexpr cstring type_name = "c_unsigned_char";
    };

    template<>
    struct DLLEXPORT TypeInfo<unsigned short> {
        static constexpr cstring type_name = "c_unsigned_short";
    };

    template<>
    struct DLLEXPORT TypeInfo<unsigned int> {
        static constexpr cstring type_name = "c_unsigned_int";
    };

    template<>
    struct DLLEXPORT TypeInfo<unsigned long> {
        static constexpr cstring type_name = "c_unsigned_long";
    };

    template<>
    struct DLLEXPORT TypeInfo<unsigned long long> {
        static constexpr cstring type_name = "c_unsigned_long_long";
    };

    template<typename T>
    struct DLLEXPORT Types {
        static const char *const type_name() {
            if constexpr(std::is_function<T>::value){
                static const char* const name = "<function>";
                return name;
            } else {
                return TypeInfo<T>::type_name;
            }
        }
    };

    template<typename T>
    struct DLLEXPORT Types<const T> {
        static const char *const type_name() {
            static std::string name("const_");
            static bool init = false;
            if (!init) {
                name += Types<typename std::remove_const<T>::type>::type_name();
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<volatile T> {
        static const char *const type_name() {

            static std::string name("volatile_");
            static bool init = false;
            if (!init) {
                name += Types<typename std::remove_const<T>::type>::type_name();
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<const volatile T> {
        static const char *const type_name() {

            static std::string name("const_volatile_");
            static bool init = false;
            if (!init) {
                name += Types<typename std::remove_const<T>::type>::type_name();
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<T *> {
        static const char *const type_name() {

            static std::string name(Types<typename std::remove_const<T>::type>::type_name());
            static bool init = false;
            if (!init) {
                name += "*";
            }
            init = true;
            return name.c_str();

        }
    };

    template<typename T>
    struct DLLEXPORT Types<const T *> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_") + Types<typename std::remove_const<T>::type>::type_name() + "*";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<volatile T *> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("volatile_") + Types<typename std::remove_const<T>::type>::type_name() + "*";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<const volatile T *> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_volatile_") + Types<typename std::remove_const<T>::type>::type_name() + "*";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<T &> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string(Types<typename std::remove_const<T>::type>::type_name()) + "&";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<const T &> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_") + Types<typename std::remove_const<T>::type>::type_name() + "&";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<volatile T &> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("volatile") + Types<typename std::remove_const<T>::type>::type_name() + "&";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<const volatile T &> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_volatile_") + Types<typename std::remove_const<T>::type>::type_name() + "&";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<T[]> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string(Types<typename std::remove_const<T>::type>::type_name()) + "[]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<const T[]> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_") + Types<typename std::remove_const<T>::type>::type_name() + "[]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<volatile T[]> {
        static const char *const type_name() {


            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("volatile_") + Types<typename std::remove_const<T>::type>::type_name() + "[]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<const volatile T[]> {
        static const char *const type_name() {


            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_volatile") + Types<typename std::remove_const<T>::type>::type_name() + "[]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T, size_t size>
    struct DLLEXPORT Types<T[size]> {
        static const char *const type_name() {
            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string(Types<typename std::remove_const<T>::type>::type_name()) + "[" +
                  std::to_string(size) + "]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T, size_t size>
    struct DLLEXPORT Types<const T[size]> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_") + Types<typename std::remove_const<T>::type>::type_name() + "[" +
                    std::to_string(size) + "]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T, size_t size>
    struct DLLEXPORT Types<volatile T[size]> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("volatile_") + Types<typename std::remove_const<T>::type>::type_name() + "[" +
                        std::to_string(size) + "]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T, size_t size>
    struct DLLEXPORT Types<const volatile T[size]> {
        static const char *const type_name() {


            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_volatile") + Types<typename std::remove_const<T>::type>::type_name() + "[" +
                        std::to_string(size) + "]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<T &&> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string(Types<typename std::remove_const<T>::type>::type_name()) + "&&";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<const T &&> {
        static const char *const type_name(){
            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_") + Types<typename std::remove_const<T>::type>::type_name() + "&&";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<volatile T &&> {
        static const char *const type_name() {
            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("volatile_") + Types<typename std::remove_const<T>::type>::type_name() + "&&";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct DLLEXPORT Types<const volatile T &&> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_volatile_") + Types<typename std::remove_const<T>::type>::type_name() + "&&";
            }
            init = true;
            return name.c_str();
        }
    };


    template<>
    struct DLLEXPORT Types<void> {
        static const char *const type_name() {
            static constexpr cstring name = "void";
            return name;
        }
    };

    template<>
    struct DLLEXPORT Types<bool> {
        static const char *const type_name() {
            static constexpr cstring name = "c_bool";
            return name;
        }
    };

    template<typename T>
    const char *const type_name() {
        return Types<T>::type_name();
    }

    template<typename ReturnType, typename ...Args>
    struct DLLEXPORT Types<ReturnType(Args...)>{
        static const char* const type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(pyllars_internal::type_name<ReturnType>()) + std::string("(*)(");

                std::string arg_names[] = {Types<Args>::type_name()...};
                for (unsigned long i = 0; i < sizeof...(Args); ++i) {
                    n += arg_names[i] + std::string(",");
                }
                n += std::string(")");
            }
            return n.c_str();
        }
    };

    template<typename ReturnType, typename ...Args>
    struct DLLEXPORT Types<ReturnType(Args..., ...)>{
        static const char* type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(pyllars_internal::type_name<ReturnType>()) + std::string("(*)(");

                std::string arg_names[] = {Types<Args>::type_name()...};
                for (unsigned int i = 0; i < sizeof...(Args); ++i) {
                    n += arg_names[i] + std::string(",");
                }
                n += std::string(" ...)");
            }
            return n.c_str();
        }
    };


    template<typename CClass, typename ReturnType, typename ...Args>
    struct DLLEXPORT Types<ReturnType(CClass::*)(Args...)>{
        static const char* type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(pyllars_internal::type_name<ReturnType>()) + std::string("(") +
                    pyllars_internal::type_name<CClass>() + std::string("*)(");

                std::string arg_names[] = {Types<Args>::type_name()...};
                for (unsigned int i = 0; i < sizeof...(Args); ++i) {
                    n += arg_names[i] + std::string(",");
                }
                n += std::string(")");
            }
            return n.c_str();
        }
    };

    template<typename CClass, typename ReturnType, typename ...Args>
    struct DLLEXPORT Types<ReturnType(CClass::*)(Args..., ...)>{
        static const char* type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(pyllars_internal::type_name<ReturnType>()) + std::string("(") +
                    pyllars_internal::type_name<CClass>() + std::string("*)(");

                std::string arg_names[] = {Types<Args>::type_name()...};
                for (unsigned int i = 0; i < sizeof...(Args); ++i) {
                    n += arg_names[i] + std::string(",");
                }
                n += std::string(" ...)");
            }
            return n.c_str();
        }
    };

    template<typename CClass, typename ReturnType, typename ...Args>
    struct DLLEXPORT Types<ReturnType(CClass::*)(Args...) const>{
        static const char* type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(pyllars_internal::type_name<ReturnType>()) + std::string("(") +
                    pyllars_internal::type_name<CClass>() + std::string("*)(");

                std::string arg_names[] = {Types<Args>::type_name()...};
                for (unsigned int i = 0; i < sizeof...(Args); ++i) {
                    n += arg_names[i] + std::string(",");
                }
                n += std::string(") const");
            }
            return n.c_str();
        }
    };

    template<typename CClass, typename ReturnType, typename ...Args>
    struct DLLEXPORT Types<ReturnType(CClass::*)(Args..., ...) const>{
        static const char* type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(pyllars_internal::type_name<ReturnType>()) + std::string("(") +
                    pyllars_internal::type_name<CClass>() + std::string("*)(");

                std::string arg_names[] = {Types<Args>::type_name()...};
                for (unsigned int i = 0; i < sizeof...(Args); ++i) {
                    n += arg_names[i] + std::string(",");
                }
                n += std::string(" ...) const");
            }
            return n.c_str();
        }
    };

    ///////////
    // Helper conversion functions
    //////////


    template<typename T>
    PyObject *toPyObject(T var, const ssize_t array_size) ;

    template<typename T, ssize_t size>
    PyObject *toPyObject(T var[size], const ssize_t array_size) ;


    template<typename T>
    struct DLLEXPORT argument_capture;

    template<typename T>
    struct DLLEXPORT PythonClassWrapper;


    template<typename T>
    struct DLLEXPORT PythonFunctionWrapper;

    enum class DLLEXPORT OpUnaryEnum : unsigned char{
        INV,
        POS,
        NEG
    };

    enum class DLLEXPORT OpBinaryEnum : unsigned char{
        ADD = 3,
        SUB,
        MUL,
        DIV,
        AND,
        OR,
        XOR,
        MOD,
        LSHIFT,
        RSHIFT,

        IADD,
        ISUB,
        IMUL,
        IDIV,
        IAND,
        IOR,
        IXOR,
        IMOD,
        ILSHIFT,
        IRSHIFT
    };

    typedef int (*_setattrfunc)(PyObject*, PyObject*, void*);
    typedef PyObject* (*_getattrfunc)(PyObject*, void*);

    template<bool varargs, typename ReturnType, typename ...Args>
    struct DLLEXPORT FuncContainer;

    template<typename ReturnType, typename ...Args>
    struct DLLEXPORT FuncContainer<false, ReturnType, Args...> {
    public:
        template<int unused, typename ...Throws>
        struct DLLEXPORT Type {
        public:
            ReturnType (*_cfunc)(Args...);
            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct DLLEXPORT Type<unused, void> {
            ReturnType (*_cfunc)(Args...);

            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct DLLEXPORT Type<unused> {
            ReturnType (*_cfunc)(Args...);

            typedef decltype(_cfunc) func_type;
        };
    };

    template<typename ReturnType, typename ...Args>
    struct DLLEXPORT FuncContainer<true, ReturnType, Args...> {

        template<int unused, typename ...Throws>
        struct DLLEXPORT Type {
            ReturnType (*_cfunc)(Args... ...);

            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct DLLEXPORT Type<unused, void> {
            ReturnType (*_cfunc)(Args... ...);

            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct DLLEXPORT Type<unused> {
            ReturnType (*_cfunc)(Args... ...);

            typedef decltype(_cfunc) func_type;
        };
    };



    template<typename const_or_nonconst_char>
    DLLEXPORT const_or_nonconst_char* fromPyStringLike(PyObject* obj);


    class PyllarsException{
    public:
        explicit PyllarsException(PyObject* excType, const char* const msg):_msg(msg), _excType(excType){
        }

        inline const char* const msg() const{
            return _msg.c_str();
        }

        inline PyObject * type() const{
            return _excType;
        }

        void raise(){
            if(PyErr_Occurred()){
                PyErr_Print();
            }
            PyErr_SetString(_excType, _msg.c_str());
        }

        static void raise_internal_cpp(const char* const msg = nullptr){
            if(!msg){
                PyllarsException(PyExc_RuntimeError, "internal c++ exception thrown:  ").raise();
            } else {
                PyllarsException(PyExc_RuntimeError, msg).raise();
            }
        }

    private:
        const std::string _msg;
        PyObject* const _excType;
    };


    constexpr int ERR_PYLLARS_ON_CREATE = -1;
}
#endif
