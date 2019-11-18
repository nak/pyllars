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

    namespace attributestesting{

        class AttributesClass{
        public:
            AttributesClass():int_val(22), const_float_val(39872.55), ptr(this){}
            int int_val;
            const float const_float_val;
            static constexpr int constexpr_int_val = 42;
            const AttributesClass* ptr;
        private:
            double private_float_val;
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

    namespace inheritancetesting{

        class BaseA{
        public:
            BaseA():int_val(0){}
            BaseA(int val):int_val(val){}

            static void set(BaseA & a, const int val){
                a.int_val = val;
            }

            attributestesting::AttributesClass* aMethod(){
                return new attributestesting::AttributesClass();
            }

            int int_val;

        };
    
        class BaseB{
        public:
            const attributestesting::BitFieldsClass &bMethod() const{
                return _bitFields;
            };
        private:
            attributestesting::BitFieldsClass _bitFields;
        };
    
        class Inherited: public BaseA{
        public:
            BaseA createBase(const int val){
                return BaseA(val);
            }
        };
    
        class MultipleInherited: public BaseA, public BaseB{
            BaseB createBase(){
                return BaseB();
            }
        };
    }

    class BasicClass{
    public:
    
        BasicClass(){}
    
        BasicClass(const BasicClass &){}
    
        virtual ~BasicClass(){}
    
        BasicClass &operator=(const BasicClass & ){return *this;}
    
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

}

#endif
