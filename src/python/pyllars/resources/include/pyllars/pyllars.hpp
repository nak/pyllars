/**
 * holds top-level namespace fo Pyllars and common definitions
 */
#ifndef PYLLARS
#define PYLLARS
#include <vector>
#include <cstddef>
#include <Python.h>
#include <functional>

#include <pyllars/pyllars_defns.hpp>
#include <pyllars/pyllars_containment.hpp>

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

#if PY_MAJOR_VERSION == 3
PyObject*
#else
PyMODINIT_FUNC
#endif
PyllarsInit(const char *name);


#include <pyllars/pyllars_classwrapper.hpp>
#include <pyllars/pyllars_pointer.hpp>
#include <pyllars/pyllars_integer.hpp>
#include <pyllars/pyllars_floating_point.hpp>
#endif
