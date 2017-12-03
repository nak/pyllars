#ifndef PYLLARS
#define PYLLARS

#include <vector>
#include <cstddef>
#include <Python.h>

typedef int status_t;

namespace pyllars{

    class Initializer{
    public:

        virtual status_t init(){
            int status = 0;
            if(!_initializers) return 0;
            for (auto it = _initializers->begin(); it != _initializers->end(); ++ it){
                status |= (*it)->init();
            }
            _initializers->clear();
            return status;
        }

        int register_init( Initializer* const init){
	        if(!_initializers){
	            _initializers = new std::vector<Initializer*>();
	        }
            _initializers->push_back(init);
            return 0;
        }

        static Initializer *root;

    private:
        std::vector<Initializer*> *_initializers;
    };

    int pyllars_register( Initializer* const init);

}

#endif
//PYLLARS
