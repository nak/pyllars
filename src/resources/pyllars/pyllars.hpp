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

typedef int status_t;

namespace pyllars {

    /**
     * class for registering a hierarchy of initializers to execute on load of library
     */
    class Initializer {
    public:

        Initializer() : _initializers(nullptr), _initializers_last(nullptr){
        }

        /**
         * Call all initializers, passing in the global pyllars module
         */
        virtual status_t init(PyObject *const global_module) {
            int status = 0;
            if (!_initializers) return 0;
            for (auto &_initializer : *_initializers) {
                status |= _initializer->init(global_module);
            }
            _initializers->clear();
            return status;
        }

        virtual status_t init_last(PyObject *const global_module) {
            int status = 0;
            if (!_initializers_last) return 0;
            for (auto &it : *_initializers_last) {
                status |= it->init(global_module);
            }
            _initializers_last->clear();
            return status;
        }

        int register_init(Initializer *const init) {
            if (!_initializers) {
                // allocate here as this may be called before main
                // and do not want to depend on static initailization order of files which is
                // unpredictable in C++
                _initializers = new std::vector<Initializer *>();
            }
            _initializers->push_back(init);
            return 0;
        }

        int register_init_last(Initializer *const init) {
            if (!_initializers_last) {
                // allocate here as this may be called before main
                // and do not want to depend on static initailization order of files which is
                // unpredictable in C++
                _initializers_last = new std::vector<Initializer *>();
            }
            _initializers_last->push_back(init);
            return 0;
        }

        // ths root (top-level) initializer
        static Initializer *root;

    private:

        std::vector<Initializer *> *_initializers;
        std::vector<Initializer *> *_initializers_last{};

    };

    int pyllars_register(Initializer *const init);

    int pyllars_register_last(Initializer *const init);

    int init(PyObject *global_mod);

}  // namespace pyllars

namespace __pyllars_internal {


    /**
     Struct (non-template) to hold a common number (integer) base Type that is not instantiable,
     but provide for common reference base type
    */
    struct PyNumberCustomBase {
        static PyTypeObject Type;
    };

    template<typename number_type>
    struct PyNumberCustomObject {
    public:
        /**
        Templated class representing a c-type integer in Python based on a number-class-type that implements
        Python's number methods
        */
        PyObject_HEAD
        typedef number_type ntype;
        typedef PythonClassWrapper<number_type const, void> ConstWrapper;
        typedef PythonClassWrapper<typename std::remove_const<number_type>::type> NonConstWrapper;
        typedef PythonClassWrapper<typename std::remove_reference<number_type>::type> NoRefWrapper;
        typedef PythonClassWrapper<typename std::remove_const<typename std::remove_reference<number_type>::type>::type> NoRefNonConstWrapper;

        static PyTypeObject Type;

        inline static std::string get_name() {
            return std::string(__pyllars_internal::type_name<ntype>());
        }

        inline static std::string get_full_name() {
            return std::string(__pyllars_internal::type_name<ntype>());
        }

        inline static std::string get_module_entry_name() {
            return std::string(__pyllars_internal::type_name<ntype>());
        }

        static PythonClassWrapper<number_type *> *alloc(PyObject *cls, PyObject *args, PyObject *kwds);

        static int initialize(const char *const name, const char *const module_entry_name,
                              PyObject *module, const char *const fullname = nullptr);

        static PyObject *richcompare(PyObject *a, PyObject *b, int op);

        static __pyllars_internal::PythonClassWrapper<number_type> *createPy
                (const ssize_t arraySize,
                 __pyllars_internal::ObjContainer<ntype> *const cobj,
                 const bool isAllocated,
                 const bool inPlace, PyObject *referencing, const size_t depth = 0);


        void make_reference(PyObject *obj);

        inline static bool checkType(PyObject *const obj) {
            return PyObject_TypeCheck(obj, &Type);
        }

        static constexpr PyObject *const parent_module = nullptr;

        static PyObject *repr(PyObject *o);

        static int create(PyObject *subtype, PyObject *args, PyObject *kwds);

        PyNumberCustomObject() : _referenced(nullptr), _depth(0) {
        }

        template<typename t=number_type>
        inline t *get_CObject() {
            return &value;
        }


        static PyMethodDef _methods[];

        std::function<__int128_t()> asLongLong;

        PyObject *_referenced;
        size_t _depth;
        number_type value;

        class Initializer : public pyllars::Initializer {
        public:
            Initializer();

            status_t init(PyObject *const global_module) override;

            static Initializer *initializer;
        };
    };


    template<>
    class PythonClassWrapper<char> : public PyNumberCustomObject<char> {
    };

    template<>
    class PythonClassWrapper<short> : public PyNumberCustomObject<short> {
    };

    template<>
    class PythonClassWrapper<int> : public PyNumberCustomObject<int> {
    };

    template<>
    class PythonClassWrapper<long> : public PyNumberCustomObject<long> {
    };

    template<>
    class PythonClassWrapper<long long> : public PyNumberCustomObject<long long> {
    };

    template<>
    class PythonClassWrapper<unsigned char> : public PyNumberCustomObject<unsigned char> {
    };

    template<>
    class PythonClassWrapper<unsigned short> : public PyNumberCustomObject<unsigned short> {
    };

    template<>
    class PythonClassWrapper<unsigned int> : public PyNumberCustomObject<unsigned int> {
    };

    template<>
    class PythonClassWrapper<unsigned long> : public PyNumberCustomObject<unsigned long> {
    };

    template<>
    class PythonClassWrapper<unsigned long long> : public PyNumberCustomObject<unsigned long long> {
    };


    struct PyFloatingPtCustomBase {
        static PyTypeObject Type;

        std::function<double()> asDouble;
    };

    template<typename number_type>
    struct PyFloatingPtCustomObject {
    public:
        PyObject_HEAD
        typedef number_type
        ntype;
        typedef PythonClassWrapper<number_type const, void> ConstWrapper;
        typedef PythonClassWrapper<typename std::remove_const<number_type>::type> NonConstWrapper;
        typedef PythonClassWrapper<typename std::remove_reference<number_type>::type> NoRefWrapper;

        static PyTypeObject Type;

        inline static std::string get_name() {
            return std::string(__pyllars_internal::type_name<ntype>());
        }

        inline static std::string get_full_name() {
            return std::string(__pyllars_internal::type_name<ntype>());
        }

        inline static std::string get_module_entry_name() {
            return std::string(__pyllars_internal::type_name<ntype>());
        }

        static PythonClassWrapper<number_type *> *alloc(PyObject *cls, PyObject *args, PyObject *kwds);

        static int initialize(const char *const name, const char *const module_entry_name,
                              PyObject *module, const char *const fullname = nullptr);

        static PyObject *richcompare(PyObject *a, PyObject *b, int op);

        static __pyllars_internal::PythonClassWrapper<number_type> *createPy
                (const ssize_t arraySize,
                 __pyllars_internal::ObjContainer<ntype> *const cobj,
                 const bool isAllocated,
                 const bool inPlace, PyObject *referencing, const size_t depth = 0);


        void make_reference(PyObject *obj);

        inline static bool checkType(PyObject *const obj) {
            return PyObject_TypeCheck(obj, &Type);
        }

        static constexpr PyObject *const parent_module = nullptr;

        static PyObject *repr(PyObject *o);

        static int create(PyObject *subtype, PyObject *args, PyObject *kwds);

        PyFloatingPtCustomObject() : _referenced(nullptr), _depth(0) {
        }

        template<typename t=number_type>
        inline t *get_CObject() {
            return &value;
        }


        static PyMethodDef _methods[];

        std::function<double()> asDouble;

        PyObject *_referenced;
        size_t _depth;
        number_type value;

        class Initializer : public pyllars::Initializer {
        public:
            Initializer();

            status_t init(PyObject *const global_module);

            static Initializer *initializer;
        };
    };


    template<>
    class PythonClassWrapper<float> : public PyFloatingPtCustomObject<float> {
    };

    template<>
    class PythonClassWrapper<double> : public PyFloatingPtCustomObject<double> {
    };


}

#include "pyllars_const.hpp"

#if PY_MAJOR_VERSION == 3
PyObject*
#else
PyMODINIT_FUNC
int
#endif
PyllarsInit(const char *const name);

#endif
//PYLLARS
