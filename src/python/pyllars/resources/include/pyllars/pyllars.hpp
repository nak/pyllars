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
#include <pyllars/internal/pyllars_containment.hpp>

typedef int status_t;


#if PY_MAJOR_VERSION == 3
PyObject*
#else
PyMODINIT_FUNC
#endif
PyllarsInit(const char *name);


#include <pyllars/internal/pyllars_classwrapper.hpp>
#include <pyllars/internal/pyllars_pointer.hpp>
#include <pyllars/internal/pyllars_integer.hpp>
#include <pyllars/internal/pyllars_floating_point.hpp>
namespace __pyllars_internal {
    class Init {
    public:
        typedef status_t (*ready_func_t)();

        static void registerReady(ready_func_t func){
            static std::vector<ready_func_t> _funcs;
            _readyFuncs = &_funcs;
            if (func) _readyFuncs->push_back(func);
        }

        static status_t ready(){
            static status_t status = 0;
            static bool inited = false;
            if (inited) return status;
            inited = true;
            if(_readyFuncs) {
                for (auto &func: *_readyFuncs) {
                    if(func) {status |= func();}
                }
            }
            return status;
        }

        static std::vector<ready_func_t> *_readyFuncs;

    };
}
#endif
