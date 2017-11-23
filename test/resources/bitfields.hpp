#ifndef __BITFIELDS__
#define __BITFIELDS__

namespace unused{
}

typedef char * str;
typedef const char* const const_str;

extern const char * const const_ptr_str ;
extern const char* const * const_ptr_ptr_str;

namespace outside{
    class ExternalDependency{
    };
}

namespace trial{

  class Container{
  public:

    struct BitFields{
        BitFields():size_1bit(1),
            size_2bit(0),
            const_size_11bit(3),
            size_31bit(42){
        }

        signed char size_1bit : 1;
        unsigned char size_2bit: 2;
        const int const_size_11bit: 11;
        long size_31bit: 31;
    };

  };

}

#endif
