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

    Initializer(const initializer_t initializer);

    static status_t init();

  private:
    static std::vector< initializer_t> *_initializers;
  };

}

#endif 
//PYLLARS
