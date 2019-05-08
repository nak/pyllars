#ifndef __CLASSES__
#define __CLASSES__


namespace trial{

  template <typename T, T d = (T)0>
  class TemplateClass{
  public:

    void method1(const float& value);

    virtual double method2(T& value);

    virtual void abstract() = 0;

    double public_member;

    class Inner{
    public:
       void inner_method();
    };

    template <typename T2>
    class InnerTemplate{
    public:
       void inner_method(){
       }
    };

  };


  using AliasedTemplateClassInt = TemplateClass<int>;

  template< int n>
  using TemplateAlias = TemplateClass<int, n>;

}

using ClassLong = trial::TemplateClass<long>;
static constexpr long* null_long_ptr = (long*)0;
using ClassLongPtr = trial::TemplateClass<long*, null_long_ptr>;

#endif
