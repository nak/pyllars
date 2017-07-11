#ifndef __CLASSES__
#define __CLASSES__

namespace unused{
}

typedef char * str;
typedef const char* const const_str;

extern const char * const const_ptr_str ;
extern const char* const * const_ptr_ptr_str;

namespace outside{
    class Dep{
    };
}

namespace trial{

  class Main{
  public:
    Main();
    Main(const Main &);
    virtual ~Main();

    Main &operator=(const Main & );

    struct Implicit{
    };

    class Inner{
    public:
      Inner();

      void method(outside::Dep d){
      }

      class Tertiary{
      public:
	float timestamp();
      };
    };

    const double method(const struct Implicit &) const{
        return 0.1;
    }


    //std::string string_method();

  };

  class Inherited: public Main{
  };

  class Main2{
  };

  class MultiInherited: public Main, public Main2{
  };

  template <typename T, T d=T(0)>
  class TemplateClass{

    void method1(const float& value);

    virtual double method2(T& value);

    virtual void abstract() = 0;
  };


  using ClassInt = TemplateClass<int>;

  namespace incomplete{
    class Incomplete;

    class Empty{
    };
  }

  int some_global_function(const double & value, outside::Dep &d) throw(double );

}

using ClassLong = trial::TemplateClass<long>;
using ClassLongPtr = trial::TemplateClass<long*>;

typedef trial::Main& Main2;

#endif
