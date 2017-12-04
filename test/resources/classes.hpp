#ifndef __CLASSES__
#define __CLASSES__

namespace unused{
}

typedef char * str;
typedef const char* const const_str;


namespace outside{
    class ExternalDependency{
    };
}

namespace trial{

  class Main{
  public:

    Main();

    Main(const Main &);

    virtual ~Main();

    Main &operator=(const Main & );

    int varargs_method(double d, ...);

    int const_varargs_method(double d, ...) const;

    struct Internal{
    };

    class Inner{
    public:
          Inner();
    
          void method(outside::ExternalDependency d){
          }
    
          class Tertiary{
          public:
              float timestamp(){return 8349876.123f; }
          };
    };

    const double method(const struct Internal &) const{
        return 0.1;
    }


    //std::string string_method();

  };


  namespace incomplete{

    class Incomplete;

    class Empty{
    };

  }

}

#endif
