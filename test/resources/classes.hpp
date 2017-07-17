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

    int varargs_method(double d, ...);

    int const_varargs_method(double d, ...) const;

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
  public:

    static void static_method(const Main2 i);

    static double dbl_static_member;

    Main2():const_short_member(42){
        int_members[0] = const_short_member*2;
        int_members[1] = const_short_member*3;
        int_members[2] = const_short_member*4;
    }

    Main2(short v):const_short_member(v){
        int_members[0] = v*2;
        int_members[1] = v*3;
        int_members[2] = v*4;
    }

    static Main static_vararg_method(const char* const pattern, ...);

    const short const_short_member;
    int int_members[3];

  };

  class MultiInherited: public Main, public Main2{
  };

  template <typename T, T d = (T)0>
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
static constexpr long* null_long_ptr = (long*)0;
using ClassLongPtr = trial::TemplateClass<long*, null_long_ptr>;

typedef trial::Main& Main2;

#endif
