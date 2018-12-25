/**
 * holds top-level namespace fo Pyllars and common definitions
 */
#ifndef PYLLARS
#define PYLLARS

#include <vector>
#include <cstddef>
#include <Python.h>

typedef int status_t;

namespace pyllars{

    /**
     * class for registering a hierarchy of initializers to execute on load of library
     */
    class Initializer{
    public:

        Initializer():_initializers(nullptr){
        }

        /**
         * Call all initializers, passing in the global pyllars module
         */
        virtual status_t init(PyObject* const global_module){
            int status = 0;
            if(!_initializers) return 0;
            for (auto it = _initializers->begin(); it != _initializers->end(); ++ it){
                status |= (*it)->init(global_module);
            }
            _initializers->clear();
            return status;
        }

        virtual status_t init_last(PyObject* const global_module){
            int status = 0;
            if(!_initializers_last) return 0;
            for (auto it = _initializers_last->begin(); it != _initializers_last->end(); ++ it){
                status |= (*it)->init(global_module);
            }
            _initializers_last->clear();
            return status;
        }

        int register_init( Initializer* const init){
	        if(!_initializers){
                // allocate here as this may be called before main
                // and do not want to depend on static initailization order of files which is
                // unpredictable in C++
	            _initializers = new std::vector<Initializer*>();
	        }
            _initializers->push_back(init);
            return 0;
        }

        int register_init_last( Initializer* const init){
	        if(!_initializers_last){
                // allocate here as this may be called before main
                // and do not want to depend on static initailization order of files which is
                // unpredictable in C++
	            _initializers_last = new std::vector<Initializer*>();
	        }
            _initializers_last->push_back(init);
            return 0;
        }

        // ths root (top-level) initializer
        static Initializer *root;

    private:

        std::vector<Initializer*> *_initializers;
        std::vector<Initializer*> *_initializers_last;

    };

    int pyllars_register( Initializer* const init);

    int init(PyObject* global_mod);

}  // namespace pyllars


#if PY_MAJOR_VERSION == 3
PyObject*
#else
PyMODINIT_FUNC
int
#endif
PyllarsInit(const char* const name);

#endif
//PYLLARS
