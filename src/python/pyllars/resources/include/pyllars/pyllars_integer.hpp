//
// Created by jrusnak on 5/8/19.
//

#ifndef PYLLARS_PYLLARS_INTEGER_H
#define PYLLARS_PYLLARS_INTEGER_H

#include "pyllars_defns.hpp"
#include "pyllars_classwrapper.hpp"

namespace  __pyllars_internal {

    /**
     Struct (non-template) to hold a common number (integer) base Type that is not instantiable,
     but provide for common reference base type
    */
    struct PyNumberCustomBase : public CommonBaseWrapper {
        static PyTypeObject Type;
         __int128_t(*toInt)(PyObject* obj);
    };

    namespace{

        __int128_t toLongLong(PyObject *obj) {
            if (PyLong_Check(obj)) {
                return PyLong_AsLongLong(obj);
            } else if (PyObject_TypeCheck(obj, &PyNumberCustomBase::Type)) {
                auto self = (PyNumberCustomBase*)obj;
                if (!self->toInt){
                    PyErr_SetString(PyExc_SystemError, "Uninitialized integer conversion function pointer encountered!!");
                    return -1;
                }
                return  self->toInt(obj);
            } else {
                PyErr_SetString(PyExc_SystemError, "System error: invalid type encountered");
                return -1;
            }
        }

        bool isIntegerObject(PyObject *obj) {
            return bool(PyLong_Check(obj)) || bool(PyObject_TypeCheck(obj, &PyNumberCustomBase::Type));
        }

    }

    template<typename number_type>
    struct NumberType{
        typedef typename std::remove_reference<number_type>::type number_type_basic;

        static PyNumberMethods *instance();


        static constexpr number_type_basic min = std::numeric_limits<number_type_basic>::min();
        static constexpr number_type_basic max = std::numeric_limits<number_type_basic>::max();

        static bool is_out_of_bounds_add(__int128_t value1, __int128_t value2) {
            return ((value1 > 0 && value1 > max - value2) ||
                    (value1 < 0 && value1 < min - value2));
        }

        static bool is_out_of_bounds_subtract(__int128_t value1, __int128_t value2) {
            return ((value1 > 0 && value1 > max + value2) ||
                    (value1 < 0 && value1 < min + value2));
        }

        template<__int128_t(*func)(__int128_t, __int128_t, const bool check)>
        static PyObject *_baseBinaryFunc(PyObject *v1, PyObject *v2) ;

        template<void(*func)(__int128_t &, number_type_basic)>
        static PyObject *_baseInplaceBinaryFunc(PyObject *v1, PyObject *v2);

        template<number_type_basic (*func)(__int128_t)>
        static PyObject *_baseUnaryFunc(PyObject *obj);

        static __int128_t add(__int128_t value1, __int128_t value2, const bool check) {
            return value1 + value2;
        }

        static __int128_t subtract(__int128_t value1, __int128_t value2, const bool check) {
            return value1 - value2;
        }

        static __int128_t multiply(__int128_t value1, __int128_t value2, const bool check) {
            const number_type_basic result = value1 * value2;
            if (check && value1 != 0 && result / value1 != value2) {
                PyErr_SetString(PyExc_ValueError, "multiplication of values is out of range");
            }
            return result;
        }

        static __int128_t pow(__int128_t base, __int128_t exp){
            __int128_t result = 1;
            for (;;) {
                if (exp & 1)
                    result *= base;
                exp >>= 1;
                if (!exp)
                    break;
                base *= base;
            }
            return result;
        }

        static PyObject *power(PyObject *v1, PyObject *v2, PyObject *v3);

        static __int128_t remainder(__int128_t value1, __int128_t value2, const bool check){
            __int128_t result = value1 % value2;
            if (((value1 < 0 and value2 > 0) || (value1 > 0 && value2 < 0)) && (value1 % value2 != 0)) {
                result = value1 - (floor_div(value1, value2, false) * value2);
            }
            if (check && (result > max || result < min)) {
                PyErr_SetString(PyExc_ValueError, "Result is out of range");
            }
            return result;
        }


        static PyObject *positive(PyObject *v1){
            Py_INCREF(v1);
            return v1;
        }

        static number_type_basic absolute(__int128_t value1){
            if (value1 == min) {
                PyErr_SetString(PyExc_ValueError, "Result is out of bounds");
            }
            return value1 > 0 ? value1 : -value1;
        }

        static number_type_basic negative(__int128_t value){
            const __int128_t result = -value;
            if (result < min || result > max) {
                PyErr_SetString(PyExc_ValueError, "Result is out of range");
            }
            return result;
        }

        static PyObject *divmod(PyObject *v1, PyObject *v2);

        static number_type_basic invert(__int128_t value) {
            return ~(number_type) value;
        }

        static __int128_t lshift(__int128_t value1, __int128_t value2, const bool) {
            return ((number_type) value1) << ((number_type) value2);
        }

        static __int128_t rshift(__int128_t value1, __int128_t value2, const bool) {
            return ((number_type) value1) >> ((number_type) value2);
        }

        static __int128_t and_(__int128_t value1, __int128_t value2, const bool) {
            return (number_type) value1 & (number_type) value2;
        }

        static __int128_t or_(__int128_t value1, __int128_t value2, const bool) {
            return ((number_type) value1) | ((number_type) value2);
        }

        static __int128_t xor_(__int128_t value1, __int128_t value2, const bool) {
            return ((number_type) value1) ^ ((number_type) value2);
        }

        static PyObject *to_pyint(PyObject *value) {
            return PyLong_FromLong(static_cast<long>(toLongLong(value)));
        }

        static PyObject *to_pyfloat(PyObject *value) {
            return PyFloat_FromDouble((double) toLongLong(value));
        }

        static void inplace_add(__int128_t &value1, number_type_basic value2) {
            if (is_out_of_bounds_add(value1, value2)) {
                PyErr_SetString(PyExc_ValueError, "Values out of range for in place addition");
            }
            value1 += value2;
        }

        static void inplace_subtract(__int128_t &value1, number_type_basic value2) {
            if (is_out_of_bounds_subtract(value1, value2)) {
                PyErr_SetString(PyExc_ValueError, "Values out of range for in place subtraction");
            }
            value1 -= value2;
        }

        static void inplace_multiply(__int128_t &value1, number_type_basic value2) {
            const number_type orig = value1;
            value1 *= value2;
            if (value2 != 0 && value1 / value2 != orig) {
                PyErr_SetString(PyExc_ValueError, "multiplication of values is out of range");
            }
        }

        static void inplace_remainder(__int128_t &value1, number_type_basic value2) {
            value1 %= value2;
        }

        static void inplace_lshift(__int128_t &value1, number_type_basic value2) {
            value1 <<= value2;
        }

        static void inplace_rshift(__int128_t &value1, number_type_basic value2) {
            value1 >>= value2;
        }

        static void inplace_and(__int128_t &value1, number_type_basic value2) {
            value1 &= value2;
        }

        static void inplace_or(__int128_t &value1, number_type_basic value2) {
            value1 |= value2;
        }

        static void inplace_xor(__int128_t &value1, number_type_basic value2) {
            value1 ^= value2;
        }

        static void inplace_floor_div(__int128_t &value1, number_type_basic value2) {
            value1 /= value2;
            if (((value1 < 0 and value2 > 0) || (value1 > 0 && value2 < 0)) && (value1 % value2 != 0)) {
                value1 -= 1;
            }
        }

        static __int128_t floor_div(__int128_t value1, __int128_t value2, const bool check) {
            if (((value1 < 0 and value2 > 0) || (value1 > 0 && value2 < 0)) && (value1 % value2 != 0)) {
                return value1 / value2 - 1;
            }
            return value1 / value2;
        }

        static PyObject *divide(PyObject *v1, PyObject *v2) ;

        static PyObject *inplace_divide(PyObject *v1, PyObject *v2);
    };


    /**
     Templated class representing a c-type integer in Python based on a number-class-type that implements
     Python's number methods
    */
    template<typename number_type>
    struct PyNumberCustomObject : public PyNumberCustomBase {
    public:
        PyObject_HEAD
        typedef typename std::remove_reference<number_type>::type number_type_basic;

        static PyTypeObject Type;

        /**
         * Initialize the Python Type associated with this class
         * @return 0 on success, negative integer otherwise
         */
        static int initialize();

        /**
         * Create Python Object that wrapps the given C object instance
         * @param cobj: C-like object instance to be wrapped
         * @param referencing: Python object from which the cobject is held (and thereby this will hold a reference to
         *    that for its lifetime) or nullptr if not from another Python Wrapper object
         * @return newly created Python object wrapping the given C object instance
         */
        static PyObject *createPyReference(number_type &cobj, PyObject *referencing = nullptr);

        /**
         * Create a Python Wrapper for the given allocated pointer-to-C-object and assume ownership of the pointer
         * (pointer will be deallocated at end of the PyObject's lifetime)
         * @param cobj: pointer-to-C-instnace to be wrapped and owned
         * @param referencing:  Python object from which the cobject is held (and thereby this will hold a reference to
         *    that for its lifetime) or nullptr if not from another Python Wrapper object
         * @return newly created Python object owning/wrapping the given pointer-to-C-object
         */
        static PyObject *createPyFromAllocated(number_type_basic *cobj, PyObject *referencing = nullptr);

        /**
         *
         * @param obj PyObject to check if type matches this class
         * @return true if obj matches the same type as this class's Type, false otherwise
         */
        inline static bool checkType(PyObject *const obj) {
            return PyObject_TypeCheck(obj, getPyType());
        }

        explicit PyNumberCustomObject() : _referenced(nullptr), _depth(0), value(0) {
        }

        /**
         * @return pointer to the C instnace this class is wrapping
         */
        inline number_type_basic *get_CObject() {
            return representation<number_type>::addr(value);
        }

        /**
         * @return the PyTypeObject associated with this class;  initialize the type if not already initialized
         */
        static PyTypeObject *getPyType() {
            if (initialize() != 0) { return nullptr; }
            return &Type;
        }

        friend struct NumberType<number_type>;

    private:
        static PyMethodDef _methods[];

        static int create(PyObject *subtype, PyObject *args, PyObject *kwds);

        static PyObject *richcompare(PyObject *a, PyObject *b, int op);

        static PyObject *repr(PyObject *o);

        static PyObject *alloc(PyObject *cls, PyObject *args, PyObject *kwds);

        static PyObject *to_int(PyObject *self, PyObject *args, PyObject *kwds);

        std::function<__int128_t()> asLongLong;
        PyObject *_referenced;
        size_t _depth;
        typename representation<number_type>::type value;

        class Initializer : public pyllars::Initializer {
        public:
            Initializer();

            status_t set_up() override;

            static Initializer *initializer;
        };
    };

    template<>
    class PythonClassWrapper<char> : public PyNumberCustomObject<char> {
    };

    template<>
    class PythonClassWrapper<char &> : public PyNumberCustomObject<char &> {
    };

    template<>
    class PythonClassWrapper<short> : public PyNumberCustomObject<short> {
    };

    template<>
    class PythonClassWrapper<short &> : public PyNumberCustomObject<short &> {
    };

    template<>
    class PythonClassWrapper<int> : public PyNumberCustomObject<int> {
    };

    template<>
    class PythonClassWrapper<int &> : public PyNumberCustomObject<int &> {
    };

    template<>
    class PythonClassWrapper<long> : public PyNumberCustomObject<long> {
    };

    template<>
    class PythonClassWrapper<long &> : public PyNumberCustomObject<long &> {
    };

    template<>
    class PythonClassWrapper<long long> : public PyNumberCustomObject<long long> {
    };

    template<>
    class PythonClassWrapper<long long &> : public PyNumberCustomObject<long long &> {
    };

    template<>
    class PythonClassWrapper<unsigned char> : public PyNumberCustomObject<unsigned char> {
    };

    template<>
    class PythonClassWrapper<unsigned char &> : public PyNumberCustomObject<unsigned char &> {
    };

    template<>
    class PythonClassWrapper<unsigned short> : public PyNumberCustomObject<unsigned short> {
    };

    template<>
    class PythonClassWrapper<unsigned short &> : public PyNumberCustomObject<unsigned short &> {
    };

    template<>
    class PythonClassWrapper<unsigned int> : public PyNumberCustomObject<unsigned int> {
    };

    template<>
    class PythonClassWrapper<unsigned int &> : public PyNumberCustomObject<unsigned int &> {
    };

    template<>
    class PythonClassWrapper<unsigned long> : public PyNumberCustomObject<unsigned long> {
    };

    template<>
    class PythonClassWrapper<unsigned long &> : public PyNumberCustomObject<unsigned long &> {
    };

    template<>
    class PythonClassWrapper<unsigned long long> : public PyNumberCustomObject<unsigned long long> {
    };

    template<>
    class PythonClassWrapper<unsigned long long &> : public PyNumberCustomObject<unsigned long long &> {
    };



    template<>
    class PythonClassWrapper<const char> : public PyNumberCustomObject<const char> {
    };

    template<>
    class PythonClassWrapper<const char &> : public PyNumberCustomObject<const char &> {
    };

    template<>
    class PythonClassWrapper<const short> : public PyNumberCustomObject<const short> {
    };

    template<>
    class PythonClassWrapper<const short &> : public PyNumberCustomObject<const short &> {
    };

    template<>
    class PythonClassWrapper<const int> : public PyNumberCustomObject<const int> {
    };

    template<>
    class PythonClassWrapper<const int &> : public PyNumberCustomObject<const int &> {
    };

    template<>
    class PythonClassWrapper<const long> : public PyNumberCustomObject<const long> {
    };

    template<>
    class PythonClassWrapper<const long &> : public PyNumberCustomObject<const long &> {
    };

    template<>
    class PythonClassWrapper<const long long> : public PyNumberCustomObject<const long long> {
    };

    template<>
    class PythonClassWrapper<const long long &> : public PyNumberCustomObject<const long long &> {
    };

    template<>
    class PythonClassWrapper<const unsigned char> : public PyNumberCustomObject<const unsigned char> {
    };

    template<>
    class PythonClassWrapper<const unsigned char &> : public PyNumberCustomObject<const unsigned char &> {
    };

    template<>
    class PythonClassWrapper<const unsigned short> : public PyNumberCustomObject<const unsigned short> {
    };

    template<>
    class PythonClassWrapper<const unsigned short &> : public PyNumberCustomObject<const unsigned short &> {
    };

    template<>
    class PythonClassWrapper<const unsigned int> : public PyNumberCustomObject<const unsigned int> {
    };

    template<>
    class PythonClassWrapper<const unsigned int &> : public PyNumberCustomObject<const unsigned int &> {
    };

    template<>
    class PythonClassWrapper<const unsigned long> : public PyNumberCustomObject<const unsigned long> {
    };

    template<>
    class PythonClassWrapper<const unsigned long &> : public PyNumberCustomObject<const unsigned long &> {
    };

    template<>
    class PythonClassWrapper<const unsigned long long> : public PyNumberCustomObject<const unsigned long long> {
    };

    template<>
    class PythonClassWrapper<const unsigned long long &> : public PyNumberCustomObject<const unsigned long long &> {
    };

}
#endif //PYLLARS_PYLLARS_INTEGER_H
