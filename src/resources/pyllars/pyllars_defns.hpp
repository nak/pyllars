#ifndef __PYLLARS_INTERNAL_DEFNS
#define __PYLLARS_INTERNAL_DEFNS

#include <type_traits>
#include <sys/types.h>
#include <Python.h>

#include "pyllars_utils.hpp"
#include <map>

namespace __pyllars_internal {

    typedef const char *const cstring;

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
            return _Types<T>::type_name;
        }
    };

    template<typename T>
    struct Types<const T> {
        static const char *const type_name() {
            static char * namebase = new char[7 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "const_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            return namebase;
        }
    };

    template<typename T>
    struct Types<volatile T> {
        static const char *const type_name() {
            static char * namebase = new char[10 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "volatile_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            return namebase;
        }
    };

    template<typename T>
    struct Types<const volatile T> {
        static const char *const type_name() {
            static char * namebase = new char[16 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "const_volatile_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            return namebase;
        }
    };

    template<typename T>
    struct Types<T *> {
        static const char *const type_name() {
            static char * namebase = new char[2 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "*");
            return namebase;
        }
    };

    template<typename T>
    struct Types<const T *> {
        static const char *const type_name() {
            static char * namebase = new char[8 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "const_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "*");
            return namebase;
        }
    };

    template<typename T>
    struct Types<volatile T *> {
        static const char *const type_name() {
            static char * namebase = new char[11 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "volatile_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "*");
            return namebase;
        }
    };

    template<typename T>
    struct Types<const volatile T *> {
        static const char *const type_name() {
            static char * namebase = new char[17 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "const_volatile_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "*");
            return namebase;
        }
    };

    template<typename T>
    struct Types<T &> {
        static const char *const type_name() {
            static char * namebase = new char[2 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "&");
            return namebase;
        }
    };

    template<typename T>
    struct Types<const T &> {
        static const char *const type_name() {
            static char * namebase = new char[8 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "const_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "&");
            return namebase;
        }
    };

    template<typename T>
    struct Types<volatile T &> {
        static const char *const type_name() {
            static char * namebase = new char[11 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "volatile_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "&");
            return namebase;
        }
    };

    template<typename T>
    struct Types<const volatile T &> {
        static const char *const type_name() {
            static char * namebase = new char[17 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "const_volatile_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "&");
            return namebase;
        }
    };

    template<typename T>
    struct Types<T[]> {
        static const char *const type_name() {
            static char * namebase = new char[3 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "[]");
            return namebase;
        }
    };

    template<typename T>
    struct Types<const T[]> {
        static const char *const type_name() {
            static char * namebase = new char[9 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "const_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "[]");
            return namebase;
        }
    };

    template<typename T>
    struct Types<volatile T[]> {
        static const char *const type_name() {
            static char * namebase = new char[12 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "volatile_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "[]");
            return namebase;
        }
    };

    template<typename T>
    struct Types<const volatile T[]> {
        static const char *const type_name() {
            static char * namebase = new char[18 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
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
            static char * namebase = new char[44 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "[");
            strcat(namebase, intstr);
            strcat(namebase, "]");
            return namebase;
        }
    };

    template<typename T, size_t size>
    struct Types<const T[size]> {
        static const char *const type_name() {
            char intstr[32];
            sprintf(intstr, "%ld", (long)size);
            static char * namebase = new char[50 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "const_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "[");
            strcat(namebase, intstr);
            strcat(namebase, "]");
            return namebase;
        }
    };

    template<typename T, size_t size>
    struct Types<volatile T[size]> {
        static const char *const type_name() {
            char intstr[32];
            sprintf(intstr, "%ld", (long)size);
            static char * namebase = new char[53 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "volatile");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "[");
            strcat(namebase, intstr);
            strcat(namebase, "]");
            return namebase;
        }
    };

    template<typename T, size_t size>
    struct Types<const volatile T[size]> {
        static const char *const type_name() {
            char intstr[32];
            sprintf(intstr, "%ld", (long)size);
            static char * namebase = new char[59 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "const_volatile");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "[");
            strcat(namebase, intstr);
            strcat(namebase, "]");
            return namebase;
        }
    };

    template<typename T>
    struct Types<T &&> {
        static const char *const type_name() {
            static char * namebase = new char[3 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "&&");
            return namebase;
        }
    };

    template<typename T>
    struct Types<const T &&> {
        static const char *const type_name() {
            static char * namebase = new char[9 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "const_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "&&");
            return namebase;
        }
    };

    template<typename T>
    struct Types<volatile T &&> {
        static const char *const type_name() {
            static char * namebase = new char[12 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "volatile_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "&&");
            return namebase;
        }
    };

    template<typename T>
    struct Types<const volatile T &&> {
        static const char *const type_name() {
            static char * namebase = new char[18 + strlen(Types<typename std::remove_const<T>::type>::type_name())];
            strcpy(namebase, "const_volatile_");
            strcat(namebase, Types<typename std::remove_const<T>::type>::type_name());
            strcat(namebase, "&&");
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

    ///////////
    // Helper conversion functions
    //////////
    template<typename T, typename E = void>
    PyObject *toPyObject(T &var, const bool asArgument, const ssize_t array_size);

    template<typename T, typename E = void>
    PyObject *toPyObject(const T &var, const bool asArgument, const ssize_t array_size);

    //  template<typename T, bool array_allocated, typename ClassWrapper>
    //  smart_ptr<typename std::remove_reference<T>::type, array_allocated> toCObject(PyObject &pyobj);

    struct ZERO_DEPTH;

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

        struct Base {
            PyObject_HEAD
            /*Per Python API docs*/

            typedef PyTypeObject *TypePtr_t;

            static TypePtr_t constexpr TypePtr = &PyBaseObject_Type;

        } baseClass;

        constexpr CommonBaseWrapper() : baseClass(), _referenced(nullptr) {
        }

        typedef const char *const cstring;
        static constexpr cstring tp_name_prefix = "[*pyllars*] ";
        static constexpr size_t tp_name_prefix_len = strlen(tp_name_prefix);

        static bool IsClassType(PyObject *obj) {
            PyTypeObject *pytype = (PyTypeObject *) PyObject_Type(obj);
            return strncmp(pytype->tp_name, tp_name_prefix, tp_name_prefix_len) == 0;
        }

        static constexpr cstring ptrtp_name_prefix = "[*pyllars:ptr*] ";
        static constexpr size_t ptrtp_name_prefix_len = strlen(ptrtp_name_prefix);

        static bool IsCFunctionType(PyObject *obj) {
            PyTypeObject *pytype = (PyTypeObject *) PyObject_Type(obj);
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

    protected:
        PyObject *_referenced;

    };


    template<typename T, bool is_array, const ssize_t array_size, typename E = void>
    PyObject *set_array_values(T values, const ssize_t size, PyObject *fromTuple, PyObject *referenced);

    template<typename T, bool is_array, const ssize_t array_size, typename std::enable_if<std::is_assignable<T, T>::value>::type>
    PyObject *set_array_values(T *values, const ssize_t size, PyObject *from, PyObject *referenced);

    template<>
    PyObject *set_array_values<const char **, false, -1, void>(const char **values, const ssize_t size, PyObject *from,
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

    template<typename T, typename E = void>
    struct Factory {
        PyObject *create_single_instance(T &obj, const bool inPlace);
    };

    template<typename T>
    struct Factory<T, typename std::enable_if<std::is_integral<T>::value>::type> {
        PyObject *create_single_instance(const T &value, const bool inPlace) {
            return PyLong_FromLong(value);
        }
    };

    template<typename T>
    struct Factory<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
        PyObject *create_single_instance(T &value, const bool inPlace) {
            return PyFloat_FromDouble(value);
        }
    };

    template<typename T>
    struct Factory<T, typename std::enable_if<!std::is_fundamental<T>::value>::type> {
        PyObject *create_single_instance(T &value, const bool inPlace) {
            return PythonClassWrapper<T>::createPy(1, value, inPlace);
        }
    };
}
#endif
