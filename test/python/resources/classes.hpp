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
            AttributesClass():int_val(22), const_float_val(39872.55), ptr(this){}
            int int_val;
            const float const_float_val;
            static constexpr int constexpr_int_val = 42;
            const AttributesClass* ptr;
        private:
            double private_flost_val;
        };


        struct BitFieldsClass{
            BitFieldsClass():size_1bit(1),
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
            attributes::AttributesClass* aMethod(){
                return new attributes::AttributesClass();
            }
        };
    
        class BaseB{
        public:
            const attributes::BitFieldsClass &bMethod() const{
                return _bitFields;
            };
        private:
            attributes::BitFieldsClass _bitFields;
        };
    
        class Inherited: public BaseA{
        };
    
        class MultipleInherited: public BaseA, public BaseB{
        };
    }

    class BasicClass{
    public:
    
        BasicClass(){}
    
        BasicClass(const BasicClass &){}
    
        virtual ~BasicClass(){}
    
        BasicClass &operator=(const BasicClass & ){}
    
        int varargs_method(double d, ...){
            return int(d);
        }
    
        int const_varargs_method(double d, ...) const{
            return int(d) + 5;
        }

        struct InnnerClass_Incomplete;

        const double method(const struct Internal &) const{
            return 0.1;
        }

        static float static_method(){
            return 123.456789f;
        }

        class InnerClass{
        public:
              InnerClass(){}
        
              outside::ExternalDependency* method(outside::ExternalDependency& d){
                return &d;
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

            FullOperatorList(double d=0.134):value(d){}

            FullOperatorList& operator+(){
                return *this;
            }

            int operator-() const{
                return -int(value);
            }

            FullOperatorList operator~() const{
                return FullOperatorList(-value - 1.2);
            }

            FullOperatorList operator+(const int i) const{
                return FullOperatorList(value + i);
            }
            FullOperatorList operator-(const int i) const{
                return FullOperatorList(value - i);
            }
            FullOperatorList operator*(const double factor) const{
                return FullOperatorList(value * factor);
            }

            FullOperatorList operator/(const float div) const{
                return FullOperatorList(value/double(div));
            }

            FullOperatorList operator&(const int i) const{
                return FullOperatorList( int(value) & i );
            }

            FullOperatorList operator|(const int i) const{
                return FullOperatorList( int(value) | i );
            }

            FullOperatorList operator^(const int i) const{
                return FullOperatorList( int(value) ^ i );
            }

            FullOperatorList operator%(const int i) const{
                return FullOperatorList( int(value) % i);
            }

            FullOperatorList operator<<(const int i) const{
                return FullOperatorList( int(value) << i );
            }

            FullOperatorList operator>>(const int i){
                return FullOperatorList( int(value) >> i );
            }

            FullOperatorList& operator+=(const int i){
                value += i;
                return *this;
            }

            FullOperatorList& operator-(const int i){
                value -= i;
                return *this;
            }

        private:
            double value;
        };
    }
}

#endif
