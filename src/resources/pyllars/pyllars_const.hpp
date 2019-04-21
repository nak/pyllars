/**
 * holds top-level namespace fo Pyllars and common definitions
 */
#ifndef PYLLARS_CONST
#define PYLLARS_CONST

#include <vector>
#include <cstddef>
#include <Python.h>

#include "pyllars.hpp"
#include "pyllars_classwrapper.hpp"

typedef int status_t;


namespace __pyllars_internal {



    template<typename number_type>
    struct ConstNumberType;

    template<typename number_type>
    struct ConstFloatingPointType;

    struct PyConstNumberCustomBase: public CommonBaseWrapper {
        static PyTypeObject Type;
    };

    ///////////////////
    // CAUTION: Layout of Const and non-const classes in pyllars.hpp must match!!!
    ///////////////////

    template<typename number_type>
    struct PyConstNumberCustomObject: public PyConstNumberCustomBase {
    public:
        PyObject_HEAD
        typedef number_type ntype;
        typedef typename std::remove_reference<number_type>::type number_type_basic;

        typedef PythonClassWrapper<ntype const,   void> ConstWrapper;
        typedef PythonClassWrapper<typename std::remove_const<ntype>::type > NonConstWrapper;

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &Type;
        }

        static PythonClassWrapper<number_type_basic *> *alloc(PyObject *cls, PyObject *args, PyObject *kwds);

        static int initialize();

        static PyObject *richcompare(PyObject *a, PyObject *b, int op);

        static __pyllars_internal::PythonClassWrapper<number_type> *createPyReference
                ( ntype &cobj, PyObject *referencing=nullptr);

        inline static bool checkType(PyObject *const obj) {
            return PyObject_TypeCheck(obj, &Type);
        }

        static PyObject *repr(PyObject *o);

        static int create(PyObject *subtype, PyObject *args, PyObject *kwds);

        static PyObject *createPyFromAllocated(number_type_basic *cobj, PyObject *referencing=nullptr);

        PyConstNumberCustomObject() : _referenced(nullptr), _depth(0), value(0) {
        }

        inline const typename std::remove_reference<number_type>::type *get_CObject() {
            if constexpr (std::is_reference<number_type>::value){
                return value;
            } else {
                return &value;
            }
        }

        static PyMethodDef _methods[];

        std::function<__int128_t()> asLongLong;

        PyObject *_referenced;
        size_t _depth;
        typename representation<number_type>::type  value;

        class Initializer : public pyllars::Initializer {
        public:
            Initializer();

            status_t init(PyObject *const global_module);

            static Initializer *initializer;
        };

    private:
        static PyTypeObject Type;
    };


    template<>
    struct PythonClassWrapper<const char> : public PyConstNumberCustomObject<const char> {
    };

    template<>
    struct PythonClassWrapper<const short> : public PyConstNumberCustomObject<const short> {
    };

    template<>
    struct PythonClassWrapper<const int> : public PyConstNumberCustomObject<const int> {
    };

    template<>
    struct PythonClassWrapper<const long> : public PyConstNumberCustomObject<const long> {
    };

    template<>
    struct PythonClassWrapper<const long long> : public PyConstNumberCustomObject<const long long> {
    };

    template<>
    struct PythonClassWrapper<const unsigned char> : public PyConstNumberCustomObject<const unsigned char> {
    };

    template<>
    struct PythonClassWrapper<const unsigned short> : public PyConstNumberCustomObject<const unsigned short> {
    };

    template<>
    struct PythonClassWrapper<const unsigned int> : public PyConstNumberCustomObject<const unsigned int> {
    };

    template<>
    struct PythonClassWrapper<const unsigned long> : public PyConstNumberCustomObject<const unsigned long> {
    };

    template<>
    struct PythonClassWrapper<const unsigned long long> : public PyConstNumberCustomObject<const unsigned long long> {
    };



    template<>
    struct PythonClassWrapper<const char&> : public PyConstNumberCustomObject<const char&> {
    };

    template<>
    struct PythonClassWrapper<const short&> : public PyConstNumberCustomObject<const short&> {
    };

    template<>
    struct PythonClassWrapper<const int&> : public PyConstNumberCustomObject<const int&> {
    };

    template<>
    struct PythonClassWrapper<const long&> : public PyConstNumberCustomObject<const long&> {
    };

    template<>
    struct PythonClassWrapper<const long long&> : public PyConstNumberCustomObject<const long long&> {
    };

    template<>
    struct PythonClassWrapper<const unsigned char&> : public PyConstNumberCustomObject<const unsigned char&> {
    };

    template<>
    struct PythonClassWrapper<const unsigned short&> : public PyConstNumberCustomObject<const unsigned short&> {
    };

    template<>
    struct PythonClassWrapper<const unsigned int&> : public PyConstNumberCustomObject<const unsigned int&> {
    };

    template<>
    struct PythonClassWrapper<const unsigned long&> : public PyConstNumberCustomObject<const unsigned long&> {
    };

    template<>
    struct PythonClassWrapper<const unsigned long long&> : public PyConstNumberCustomObject<const unsigned long long&> {
    };


    struct PyConstFloatingPtCustomBase: public CommonBaseWrapper {
        static PyTypeObject Type;

        std::function<double()> asDouble;
    };

    template<typename number_type>
    struct PyConstFloatingPtCustomObject: PyConstFloatingPtCustomBase{
    public:
        PyObject_HEAD
        typedef number_type ntype;
        typedef typename std::remove_reference<ntype>::type ntype_basic;

        typedef PythonClassWrapper<ntype const,   void> ConstWrapper;
        typedef PythonClassWrapper<typename std::remove_const<ntype>::type > NonConstWrapper;

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &Type;
        }

        static PythonClassWrapper<ntype_basic *> *alloc(PyObject *cls, PyObject *args, PyObject *kwds);

        static int initialize();

        static PyObject *richcompare(PyObject *a, PyObject *b, int op);

        static __pyllars_internal::PythonClassWrapper<number_type> *createPyReference
                (ntype &cobj, PyObject *referencing=nullptr);

        inline static bool checkType(PyObject *const obj) {
            return PyObject_TypeCheck(obj, &Type);
        }

        static constexpr PyObject *const parent_module = nullptr;

        static PyObject *repr(PyObject *o);

        static int create(PyObject *subtype, PyObject *args, PyObject *kwds);

        static PyObject *createPyFromAllocated(ntype_basic *cobj, PyObject *referencing=nullptr);

        PyConstFloatingPtCustomObject() : _referenced(nullptr), _depth(0), value(0.0) {
        }

        inline ntype_basic *get_CObject() {
            if constexpr (std::is_reference<number_type>::value){
                return value;
            } else {
                return &value;
            }
        }

        static PyMethodDef _methods[];

        std::function<double()> asDouble;

        PyObject *_referenced;
        size_t _depth;
        typename representation<number_type>::type  value;

        class Initializer : public pyllars::Initializer {
        public:
            Initializer();

            status_t init(PyObject *const global_module);

            static Initializer *initializer;
        };

    private:
        static PyTypeObject Type;
    };

    template<>
    struct PythonClassWrapper<const float>;

    template<>
    struct PythonClassWrapper<const double>;

    template<>
    struct PythonClassWrapper<const double> : public PyConstFloatingPtCustomObject<const double> {
    };

    template<>
    struct PythonClassWrapper<const float> : public PyConstFloatingPtCustomObject<const float> {
    };

    template<>
    struct PythonClassWrapper<const double&> : public PyConstFloatingPtCustomObject<const double&> {
    };

    template<>
    struct PythonClassWrapper<const float&> : public PyConstFloatingPtCustomObject<const float&> {
    };

}

#endif