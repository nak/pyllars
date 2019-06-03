#ifndef __PYLLARS_INTERNAL_DEFNS_H_
#define __PYLLARS_INTERNAL_DEFNS_H_

#include <type_traits>
#include <sys/types.h>
#include <map>

#include <Python.h>

#include "pyllars_type_traits.hpp"

namespace __pyllars_internal {


    typedef const char *const cstring;

    namespace {
        constexpr int ERROR_TYPE_MISMATCH = -2;
        constexpr ssize_t UNKNOWN_SIZE = -1;
        //basic constants (names and such)
        extern const char address_name[] = "this";
        extern const char alloc_name_[] = "new";
    }


    template<typename T>
    struct _Types {
        static cstring type_name;
    };

    template<>
    struct _Types<double> {
        static constexpr cstring type_name = "c_double";
    };

    template<>
    struct _Types<float> {
        static constexpr cstring type_name = "c_float";
    };

    template<>
    struct _Types<char> {
        static constexpr cstring type_name = "c_char";
    };

    template<>
    struct _Types<short> {
        static constexpr cstring type_name = "c_short";
    };

    template<>
    struct _Types<int> {
        static constexpr cstring type_name = "c_int";
    };

    template<>
    struct _Types<long> {
        static constexpr cstring type_name = "c_long";
    };


    template<>
    struct _Types<long long> {
        static constexpr cstring type_name = "c_long_long";
    };


    template<>
    struct _Types<unsigned char> {
        static constexpr cstring type_name = "c_unsigned_char";
    };

    template<>
    struct _Types<unsigned short> {
        static constexpr cstring type_name = "c_unsigned_short";
    };

    template<>
    struct _Types<unsigned int> {
        static constexpr cstring type_name = "c_unsigned_int";
    };

    template<>
    struct _Types<unsigned long> {
        static constexpr cstring type_name = "c_unsigned_long";
    };

    template<>
    struct _Types<unsigned long long> {
        static constexpr cstring type_name = "c_unsigned_long_long";
    };

    template<typename T>
    struct Types {
        static const char *const type_name() {
            if constexpr(std::is_function<T>::value){
                return 1;
            } else {
                return _Types<T>::type_name;
            }
        }
    };

    template<typename T>
    struct Types<const T> {
        static const char *const type_name() {
            static auto * namebase = new char[7 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 7 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "const_");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<volatile T> {
        static const char *const type_name() {
            static auto * namebase = new char[10 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 10 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "volatile_");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<const volatile T> {
        static const char *const type_name() {
            static auto * namebase = new char[16 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 16 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "const_volatile_");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<T *> {
        static const char *const type_name() {
            static auto * namebase = new char[2 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 2 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "*");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<const T *> {
        static const char *const type_name() {
            static auto * namebase = new char[8 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 8 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "const_");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "*");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<volatile T *> {
        static const char *const type_name() {
            static auto * namebase = new char[11 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 11 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "volatile_");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "*");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<const volatile T *> {
        static const char *const type_name() {
            static auto * namebase = new char[17 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 17 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "const_volatile_");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "*");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<T &> {
        static const char *const type_name() {
            static auto * namebase = new char[2 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 2 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "&");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<const T &> {
        static const char *const type_name() {
            static auto * namebase = new char[8 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 8 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "const_");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "&");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<volatile T &> {
        static const char *const type_name() {
            static auto * namebase = new char[11 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 11 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "volatile_");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "&");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<const volatile T &> {
        static const char *const type_name() {
            static auto * namebase = new char[17 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 17 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "const_volatile_");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "&");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<T[]> {
        static const char *const type_name() {
            static auto * namebase = new char[3 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 3 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "[]");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<const T[]> {
        static const char *const type_name() {
            static auto * namebase = new char[9 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 9 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "const_");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "[]");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<volatile T[]> {
        static const char *const type_name() {
            static auto * namebase = new char[12 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 12 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "volatile_");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "[]");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<const volatile T[]> {
        static const char *const type_name() {
            static auto * namebase = new char[18 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "const_volatile_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "[]");
            return namebase;
        }
    };

    template<typename T, size_t size>
    struct Types<T[size]> {
        static const char *const type_name() {
            char intstr[32];
            sprintf(intstr, "%ld", (long)size);
            static auto * namebase = new char[44 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 44 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "[");
                strcat(namebase, intstr);
                strcat(namebase, "]");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T, size_t size>
    struct Types<const T[size]> {
        static const char *const type_name() {
            char intstr[32];
            sprintf(intstr, "%ld", (long)size);
            static auto * namebase = new char[50 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 50 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "const_");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "[");
                strcat(namebase, intstr);
                strcat(namebase, "]");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T, size_t size>
    struct Types<volatile T[size]> {
        static const char *const type_name() {
            char intstr[32];
            sprintf(intstr, "%ld", (long)size);
            static auto * namebase = new char[53 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 53 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "volatile");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "[");
                strcat(namebase, intstr);
                strcat(namebase, "]");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T, size_t size>
    struct Types<const volatile T[size]> {
        static const char *const type_name() {
            char intstr[32];
            sprintf(intstr, "%ld", (long)size);
            static auto * namebase = new char[59 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 59 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "const_volatile");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "[");
                strcat(namebase, intstr);
                strcat(namebase, "]");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<T &&> {
        static const char *const type_name() {
            static auto * namebase = new char[3 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 3 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "&&");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<const T &&> {
        static const char *const type_name() {
            static auto * namebase = new char[9 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 9 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "const_");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "&&");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<volatile T &&> {
        static const char *const type_name() {
            static auto * namebase = new char[12 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 12 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "volatile_");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "&&");
                init = true;
            }
            return namebase;
        }
    };

    template<typename T>
    struct Types<const volatile T &&> {
        static const char *const type_name() {
            static auto * namebase = new char[18 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            static bool init = false;
            if(!init) {
                memset(namebase, 0, 18 + strlen(Types<typename std::remove_const<T>::type>::type_name()));
                strcpy(namebase, "const_volatile_");
                strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
                strcat(namebase, "&&");
                init = true;
            }
            return namebase;
        }
    };


    template<>
    struct Types<void> {
        static const char *const type_name() {
            static constexpr cstring name = "void";
            return name;
        }
    };

    template<>
    struct Types<bool> {
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
    struct Types<ReturnType(Args...)>{
        static const char* const type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(*)(");

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
    struct Types<ReturnType(Args..., ...)>{
        static const char* type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(*)(");

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
    struct Types<ReturnType(CClass::*)(Args...)>{
        static const char* type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(") +
                        __pyllars_internal::type_name<CClass>() + std::string("*)(");

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
    struct Types<ReturnType(CClass::*)(Args..., ...)>{
        static const char* type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(") +
                        __pyllars_internal::type_name<CClass>() + std::string("*)(");

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
    struct Types<ReturnType(CClass::*)(Args...) const>{
        static const char* type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(") +
                    __pyllars_internal::type_name<CClass>() + std::string("*)(");

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
    struct Types<ReturnType(CClass::*)(Args..., ...) const>{
        static const char* type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(") +
                    __pyllars_internal::type_name<CClass>() + std::string("*)(");

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
    struct argument_capture;

    template<typename T, typename E = void>
    struct PythonClassWrapper;

    template<>
    class PythonClassWrapper<const float>;


    template<>
    class PythonClassWrapper<const double>;

    /**
     * Class common to all C++ wrapper classes
     **/
    struct CommonBaseWrapper {
        PyObject_HEAD

        static PyTypeObject* getPyType(){return &_BaseType;}

        typedef bool (*comparison_func_t)(CommonBaseWrapper*, CommonBaseWrapper*);
        typedef size_t (*hash_t)(CommonBaseWrapper*);

        comparison_func_t compare;
        hash_t hash;

        struct Base{
            PyObject_HEAD
            /*Per Python API docs*/

            typedef PyTypeObject *TypePtr_t;

            static auto constexpr TypePtr = &PyBaseObject_Type;

        } baseClass;



        typedef const char *const cstring;
        static constexpr cstring tp_name_prefix = "[*pyllars*] ";
        static constexpr size_t tp_name_prefix_len = strlen(tp_name_prefix);

        static bool IsClassType(PyObject *obj) {
            auto *pytype = (PyTypeObject *) PyObject_Type(obj);
            return strncmp(pytype->tp_name, tp_name_prefix, tp_name_prefix_len) == 0;
        }

        static constexpr cstring ptrtp_name_prefix = "[*pyllars:ptr*] ";
        static constexpr size_t ptrtp_name_prefix_len = strlen(ptrtp_name_prefix);

        static bool IsCFunctionType(PyObject *obj) {
            auto *pytype = (PyTypeObject *) PyObject_Type(obj);
            return strncmp(pytype->tp_name, ptrtp_name_prefix, ptrtp_name_prefix_len) == 0;
        }

        void make_reference(PyObject *obj) {
            if (_referenced) { Py_DECREF(_referenced); }
            if (obj) { Py_INCREF(obj); }
            _referenced = obj;
        }

        PyObject *getReferenced() {
            return _referenced;
        }

        /**
             * Check for valid conversion to type T from given Python object in use as passing as argument to C function
             * (after conversion)
             * @tparam T type to convert to
             * @param obj Python with underlying C object to convert from
             * @return true if such a conversion allowed, false otherwise
             */
        template<typename T>
        static bool checkImplicitArgumentConversion(PyObject *obj){
            static constexpr bool to_is_const = std::is_const<T>::value;
            static constexpr bool to_is_reference = std::is_reference<T>::value;
            typedef typename core_type<T>::type core_t;
            auto const self = reinterpret_cast<CommonBaseWrapper*>(obj);
            return (bool) PyObject_TypeCheck(obj, &CommonBaseWrapper::_BaseType) && // is truly wrapping a C object
                   (self->_coreTypePtr == PythonClassWrapper<core_t>::getPyType()) && //core type match
                   (to_is_const || !to_is_reference || !self->_is_const); // logic for conversion-is-allowed
        }

    protected:
        static PyTypeObject _BaseType;

        static int
        __init(PyObject *self, PyObject *args, PyObject *kwds){
            return 0;
        }

        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds){
            (void) args;
            (void) kwds;
            CommonBaseWrapper *self;
            self = (CommonBaseWrapper *) type->tp_alloc(type, 0);
            return reinterpret_cast<PyObject*>(self);
        }


        template<typename T>
        void populate_type_info(bool(*checkType)(PyObject* const),
                PyTypeObject* const coreTypePtr){
            _is_const = std::is_const<T>::value;
            _is_volatile = std::is_volatile<T>::value;
            _is_reference = std::is_reference<T>::value;
            _is_pointer = std::is_pointer<T>::value;
            __checkType = checkType;
            _coreTypePtr = coreTypePtr;
        }

        bool _is_const;
        bool _is_reference;
        bool _is_volatile;
        bool _is_pointer;
        PyObject *_referenced;
        bool (*__checkType)(PyObject * typ);
        PyTypeObject* _coreTypePtr;
        CommonBaseWrapper(); // never invoked as Python allocates memory directly
    };

    template<typename T, bool is_array, const ssize_t array_size, typename E = void>
    PyObject *set_array_values(T values, ssize_t size, PyObject *fromTuple, PyObject *referenced);

    template<typename T, bool is_array, const ssize_t array_size, typename std::enable_if<std::is_assignable<T, T>::value>::type>
    PyObject *set_array_values(T *values, ssize_t size, PyObject *from, PyObject *referenced);

    template<>
    PyObject *set_array_values<const char **, false, -1, void>(const char **values, ssize_t size, PyObject *from,
                                                               PyObject *referenced);

    template<bool varargs, typename ReturnType, typename ...Args>
    struct FuncContainer;

    template<typename ReturnType, typename ...Args>
    struct FuncContainer<false, ReturnType, Args...> {
    public:
        template<int unused, typename ...Throws>
        struct Type {
        public:
            ReturnType (*_cfunc)(Args...)
#if __cplusplus < 201703L
    throw(Throws...)
#endif
;
            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct Type<unused, void> {
            ReturnType (*_cfunc)(Args...)
#if __cplusplus < 201703L
    throw()
#endif
;

            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct Type<unused> {
            ReturnType (*_cfunc)(Args...)
#if __cplusplus < 201703L
    throw()
#endif
;

            typedef decltype(_cfunc) func_type;
        };
    };

    template<typename ReturnType, typename ...Args>
    struct FuncContainer<true, ReturnType, Args...> {

        template<int unused, typename ...Throws>
        struct Type {
            ReturnType (*_cfunc)(Args... ...)
#if __cplusplus < 201703L
    throw(Throws...)
#endif
;

            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct Type<unused, void> {
            ReturnType (*_cfunc)(Args... ...)
#if __cplusplus < 201703L
    throw()
#endif
;

            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct Type<unused> {
            ReturnType (*_cfunc)(Args... ...);

            typedef decltype(_cfunc) func_type;
        };
    };



    template<typename const_or_nonconst_char>
    const_or_nonconst_char* fromPyStringLike(PyObject* obj);


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
}
#endif
