#ifndef __CLASSES__
#define __CLASSES__

namespace unused{
}

typedef char * str;
typedef const char* const const_str;


namespace outside{
    class ExternalDependency;
}

namespace trial{

    namespace attributes{

        class AttributesClass{
        public:
            int int_val;
            const float const_float_val;
            static constexpr int constexpr_int_val = 42;
            const AttributesClass* ptr;
        private:
            double private_flost_val;
        };


        struct BitFieldClass{
            BitFieldClass():size_1bit(1),
                size_2bit(0),
                const_size_11bit(3),
                size_31bit(42){
            }

            signed char size_1bit : 1;
            unsigned char size_2bit: 2;
            const int const_size_11bit: 11;
            long size_31bit: 31;
        };

    }

    namespace inheritance{
        class BaseA{
        public:
            void aMethod();
        };
    
        class BaseB{
        public:
            void bMethod() const;
        };
    
        class Inherited: public BaseA{
        };
    
        class MultipleInherited: public BaseA, public BaseB{
        };
    }

    class BasicClass{
    public:
    
        BasicClass();
    
        BasicClass(const BasicClass &);
    
        virtual ~BasicClass();
    
        BasicClass &operator=(const BasicClass & );
    
        int varargs_method(double d, ...);
    
        int const_varargs_method(double d, ...) const;

        struct Internal;

        const double method(const struct Internal &) const{
            return 0.1;
        }

        static void static_method();

        class InnerClass{
        public:
              InnerClass();
        
              void method(outside::ExternalDependency& d){
              }
        
              class InnerInnerClass{
              public:
                  float timestamp(){return 8349876.123f; }
              };

        };

  };


    namespace incomplete{

        class Incomplete;

        class Empty{
        };

    }

    namespace operators{

        class FullOperatorList{
        public:
            FullOperatorList& operator+(){
              return *this;
            }
            int operator-();
            FullOperatorList& operator~();

            FullOperatorList& operator+(const int);
            FullOperatorList& operator-(const int);
            FullOperatorList& operator*(const double);
            FullOperatorList& operator/(const float);
            FullOperatorList& operator&(const int);
            FullOperatorList& operator|(const int);
            FullOperatorList& operator^(const int);
            FullOperatorList& operator%(const int);
            FullOperatorList& operator<<(const int);
            FullOperatorList& operator>>(const int);
        };
    }
}

#endif
