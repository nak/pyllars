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
         * Call all initializer, passing in the global pyllars module
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

        // ths root (top-level) initializer
        static Initializer *root;

    private:

        std::vector<Initializer*> *_initializers;

    };

    int pyllars_register( Initializer* const init);

}  // namespace pyllars

#endif
//PYLLARS
