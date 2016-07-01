#ifndef PYLLARS
#define PYLLARS

#include <vector>
#include <cstddef>
#include <Python.h>

typedef int status_t;

namespace pyllars{



  class Initializer{
  public:

    typedef int(*initializer_t)(void);

    Initializer(const initializer_t initializer){
      if(!_initializers){
	    _initializers = new std::vector<initializer_t>();
      }
      _initializers->push_back(initializer);
    }

    static status_t init(){
      int status = 0;
      if(!_initializers) return 0;
      for (auto it = _initializers->begin(); it != _initializers->end(); ++ it){
	status |= (*it)();
      }
      return status;
    }

  private:
    static std::vector< initializer_t> *_initializers;
  };

}

#endif 
//PYLLARS
