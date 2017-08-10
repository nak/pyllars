#ifndef __CLASSES__
#define __CLASSES__

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

    struct Internal{
    };

    class Inner{
    public:
          Inner();
    
          void method(outside::ExternalDependency d){
          }
    
          class Tertiary{
          public:
              float timestamp();
          };
    };

    const double method(const struct Internal &) const{
        return 0.1;
    }


    //std::string string_method();

  };

  class Inherited: public Main{
  };

  class PublicMembers{
  public:

    static void static_method(const PublicMembers i);

    static double dbl_static_member;

    PublicMembers():const_short_member(42){
        int_members[0] = const_short_member*2;
        int_members[1] = const_short_member*3;
        int_members[2] = const_short_member*4;
    }

    PublicMembers(short v):const_short_member(v){
        int_members[0] = v*2;
        int_members[1] = v*3;
        int_members[2] = v*4;
    }

    static Main static_vararg_method(const char* const pattern, ...);

    const short const_short_member;
    int int_members[3];

  };

  class MultiInherited: public Main, public PublicMembers{
  };

  template <typename T, T d = (T)0>
  class TemplateClass{
   public:

    void method1(const float& value);

    virtual double method2(T& value);

    virtual void abstract() = 0;

    double public_member;
  };


  using AliasedTemplateClassInt = TemplateClass<int>;

  namespace incomplete{

    class Incomplete;

    class Empty{
    };

  }

    namespace AnonymousInnerTypes{

        class AnonInner{
        public:
            enum {FIRST=1, SECOND=2, FOURTH=4};

            struct {
               unsigned int word1: 4;
               unsigned word2: 4;
            } anon_struct_instance;

        };


    }

  int some_global_function(const double & value, outside::ExternalDependency &d) throw(double );

}

using ClassLong = trial::TemplateClass<long>;
static constexpr long* null_long_ptr = (long*)0;
using ClassLongPtr = trial::TemplateClass<long*, null_long_ptr>;

typedef trial::Main& PublicMembers;

#endif
