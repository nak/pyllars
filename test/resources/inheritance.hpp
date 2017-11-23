#ifndef __INHERITANCE__
#define __INHERITANCE__

namespace trial{

  class ClassA{
  public:

    ClassA();

    ClassA(const ClassA &);

    virtual ~ClassA();

    ClassA &operator=(const ClassA & );

    const double method(const struct Internal &) const{
        return 0.1;
    }



  };

  class Inherited: public ClassA{
  public:
    const double* double_ptr(){
        static const double* var = new double(0.1f);
        return var;
    }
  };

  class ClassB{
  public:


    ClassB():const_short_member(42){
    }

    ClassB(short v):const_short_member(v){
    }

    const short const_short_member;

  };

  class MultiInherited: public ClassA, public ClassB{
  };


}

#endif
