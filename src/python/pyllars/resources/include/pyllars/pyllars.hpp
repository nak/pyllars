/**
 * holds top-level namespace fo Pyllars and common definitions
 */
#ifndef PYLLARS
#define PYLLARS

#include <vector>
#include <cstddef>
#include <Python.h>
#include <functional>

#include <pyllars/internal/pyllars_defns.hpp>
typedef int status_t;

#ifdef _MSC_VER
__declspec(dllexport)
#endif
#if PY_MAJOR_VERSION == 3
PyObject*
#else
PyMODINIT_FUNC
#endif
PyllarsInit(const char *name);

/**
 * The 'pyllars' namespace holds all the public definitions for a client used to statically construct a mapping
 * from C constructs into Python constructs.
 */
namespace pyllars{

}

namespace pyllars_internal {

    /**
     * Class to hold registered init functions (called first upon init of a Pyllars' module)
     * as well as ready functions (called after all registered init's)  -- basically, the init
     * calls populate statically the members of the PyType struct and ready calls PyType_Ready to ready
     * the type once it has the full definition.
     */
    class DLLEXPORT Init {
    public:
        typedef status_t (*ready_func_t)();

        static void registerReady(ready_func_t func);

        static void registerInit(ready_func_t func);

        static status_t ready();

        static status_t init();
    private:
        static std::vector<ready_func_t> *_initFuncs;
        static std::vector<ready_func_t> *_readyFuncs;

    };

    static const char * const pyllars_empty_kwlist[] = {nullptr};
}
#endif
