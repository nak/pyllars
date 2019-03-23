/**
 * holds top-level namespace fo Pyllars and common definitions
 */
#ifndef PYLLARS
#define PYLLARS

#include <vector>
#include <cstddef>
#include <Python.h>
#include <functional>

#include "pyllars_defns.hpp"
#include "pyllars_containment.hpp"

typedef int status_t;

namespace pyllars {

    /**
     * class for registering a hierarchy of initializers to execute on load of library
     */
    class Initializer {
    public:

        Initializer() : _initializers(nullptr){
        }

        /**
         * Call all initializers, passing in the global pyllars module
         */
        virtual status_t set_up() {
            static int status = 0;
            static bool called = false;
            if (called) return status;
            called = true;
            if (!_initializers) return 0;
            for (auto &_initializer : *_initializers) {
                status |= _initializer->set_up();
            }
            return status;
        }

        /**
         * Ready all PyTypeObjects, adding to each types parent,
         * @param top_level_module: Top level module for holding global type objects
         */
         virtual status_t ready(PyObject* top_level_module){
             static int status = 0;
             static bool called = false;
             if (called) return status;
             called = true;
             if (!_initializers) return 0;
             for (auto &_initializer : *_initializers) {
                 status |= _initializer->ready(top_level_module);
             }
             return status;
         }

        int register_init(Initializer *const init) {
            if (!_initializers) {
                static std::vector<Initializer*> initializers;
                // allocate here as this may be called before main
                // and do not want to depend on static initialization order of files which is
                // unpredictable in C++
                _initializers = &initializers;
            }
            _initializers->push_back(init);
            return 0;
        }

        // ths root (top-level) initializer
        static Initializer *root;

    private:

        std::vector<Initializer *> *_initializers;

    };

    int pyllars_register(Initializer *init);

    int pyllars_register_last(Initializer *init);

    int init(PyObject *global_mod);

}  // namespace pyllars

namespace __pyllars_internal {


    /**
     Struct (non-template) to hold a common number (integer) base Type that is not instantiable,
     but provide for common reference base type
    */
    struct PyNumberCustomBase: public CommonBaseWrapper {
        static PyTypeObject Type;
    };

    /**
     Templated class representing a c-type integer in Python based on a number-class-type that implements
     Python's number methods
    */
    template<typename number_type>
    struct PyNumberCustomObject : public PyNumberCustomBase{
    public:
        PyObject_HEAD
        typedef number_type ntype;
        typedef typename std::remove_reference<number_type>::type number_type_basic;

        static PyTypeObject Type;

        static PythonClassWrapper<number_type_basic *> *alloc(PyObject *cls, PyObject *args, PyObject *kwds);
        static PyObject *to_int(PyObject *self, PyObject *args, PyObject *kwds);

        static int initialize();

        static PyObject *richcompare(PyObject *a, PyObject *b, int op);

        static __pyllars_internal::PythonClassWrapper<number_type> *createPyReference
                (ntype & cobj, PyObject *referencing=nullptr);

        inline static bool checkType(PyObject *const obj) {
            return PyObject_TypeCheck(obj, getPyType());
        }

        static PyObject *repr(PyObject *o);

        static int create(PyObject *subtype, PyObject *args, PyObject *kwds);

        explicit PyNumberCustomObject() : _referenced(nullptr), _depth(0) {
        }

        template<typename t=number_type>
        inline typename std::remove_reference<t>::type *get_CObject() {
            return &value;
        }

        static PyTypeObject* getPyType(){
            if(initialize() != 0){ return nullptr;}
            return &Type;
        }

        static PyMethodDef _methods[];

        std::function<__int128_t()> asLongLong;

        PyObject *_referenced;
        size_t _depth;
        number_type value;

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
    class PythonClassWrapper<char&> : public PyNumberCustomObject<char&> {
    };

    template<>
    class PythonClassWrapper<short> : public PyNumberCustomObject<short> {
    };

    template<>
    class PythonClassWrapper<short&> : public PyNumberCustomObject<short&> {
    };

    template<>
    class PythonClassWrapper<int> : public PyNumberCustomObject<int> {
    };
    template<>
    class PythonClassWrapper<int&> : public PyNumberCustomObject<int&> {
    };

    template<>
    class PythonClassWrapper<long> : public PyNumberCustomObject<long> {
    };

    template<>
    class PythonClassWrapper<long&> : public PyNumberCustomObject<long&> {
    };

    template<>
    class PythonClassWrapper<long long> : public PyNumberCustomObject<long long> {
    };

    template<>
    class PythonClassWrapper<long long&> : public PyNumberCustomObject<long long&> {
    };

    template<>
    class PythonClassWrapper<unsigned char> : public PyNumberCustomObject<unsigned char> {
    };

    template<>
    class PythonClassWrapper<unsigned char&> : public PyNumberCustomObject<unsigned char&> {
    };

    template<>
    class PythonClassWrapper<unsigned short> : public PyNumberCustomObject<unsigned short> {
    };

    template<>
    class PythonClassWrapper<unsigned short&> : public PyNumberCustomObject<unsigned short&> {
    };

    template<>
    class PythonClassWrapper<unsigned int> : public PyNumberCustomObject<unsigned int> {
    };

    template<>
    class PythonClassWrapper<unsigned int&> : public PyNumberCustomObject<unsigned int&> {
    };

    template<>
    class PythonClassWrapper<unsigned long> : public PyNumberCustomObject<unsigned long> {
    };

    template<>
    class PythonClassWrapper<unsigned long&> : public PyNumberCustomObject<unsigned long&> {
    };

    template<>
    class PythonClassWrapper<unsigned long long> : public PyNumberCustomObject<unsigned long long> {
    };

    template<>
    class PythonClassWrapper<unsigned long long&> : public PyNumberCustomObject<unsigned long long&> {
    };


    struct PyFloatingPtCustomBase: public CommonBaseWrapper {

        std::function<double()> asDouble;

        static PyTypeObject Type;
    };

    template<typename number_type>
    struct PyFloatingPtCustomObject: public PyFloatingPtCustomBase{
    public:
        PyObject_HEAD
        typedef number_type ntype;
        typedef typename std::remove_reference<ntype>::type ntype_basic;

        static PythonClassWrapper<ntype_basic *> *alloc(PyObject *cls, PyObject *args, PyObject *kwds);
        static PyObject* to_float(PyObject *cls, PyObject *args, PyObject *kwds);

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &Type;
        }

        static int initialize();

        static PyObject *richcompare(PyObject *a, PyObject *b, int op);

        static __pyllars_internal::PythonClassWrapper<number_type> *createPyReference
                ( ntype & cobj, PyObject *referencing = nullptr);

        inline static bool checkType(PyObject *const obj) {
            return PyObject_TypeCheck(obj, &Type);
        }

        static constexpr PyObject *const parent_module = nullptr;

        static PyObject *repr(PyObject *o);

        static int create(PyObject *subtype, PyObject *args, PyObject *kwds);

        explicit PyFloatingPtCustomObject() : _referenced(nullptr), _depth(0){
        }

        template<typename t=number_type>
        inline t *get_CObject() {
            return &value;
        }
        std::function<double()> asDouble;

        PyObject *_referenced;
        size_t _depth;
        number_type value;

        class Initializer : public pyllars::Initializer {
        public:
            Initializer();

            status_t set_up() override;

            static Initializer *initializer;
        };

    private:
        static PyMethodDef _methods[];

        static PyTypeObject Type;
    };


    template<>
    class PythonClassWrapper<float> : public PyFloatingPtCustomObject<float> {
    };

    template<>
    class PythonClassWrapper<double> : public PyFloatingPtCustomObject<double> {
    };

    template<>
    class PythonClassWrapper<float&> : public PyFloatingPtCustomObject<float&> {
    };

    template<>
    class PythonClassWrapper<double&> : public PyFloatingPtCustomObject<double&> {
    };

}

#include "pyllars_const.hpp"

#if PY_MAJOR_VERSION == 3
PyObject*
#else
PyMODINIT_FUNC
#endif
PyllarsInit(const char *name);

#endif
