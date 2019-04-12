//
// Created by jrusnak on 3/24/19.
//

#ifndef PYLLARS_CLASS_TEST_DEFNS_H
#define PYLLARS_CLASS_TEST_DEFNS_H

#include<string>
#include<map>

class BasicClass{
public:
    explicit BasicClass(const double val=2.3):double_ptr_member(new double(val)){
        int_array[0] = 1;
        int_array[1] = 2;
        int_array[2] = 3;
        _mapped_values["123"] = 123;
    }

    BasicClass(const double val, const char* const unused):double_ptr_member(new double(val)){
        int_array[0] = 1;
        int_array[1] = 2;
        int_array[2] = 3;
        _mapped_values["123"] = 123;
    }

    BasicClass(const BasicClass &obj):double_ptr_member(new double(*obj.double_ptr_member)){
        int_array[0] = obj.int_array[0];
        int_array[1] = obj.int_array[1];
        int_array[2] = obj.int_array[2];
        _mapped_values["123"] = 123;
    }

    BasicClass(const BasicClass &&obj):double_ptr_member(new double(*obj.double_ptr_member)){
        int_array[0] = obj.int_array[0];
        int_array[1] = obj.int_array[1];
        int_array[2] = obj.int_array[2];
        _mapped_values["123"] = 123;
    }

    int public_method(const double value){
        return (int)value;
    }

    int &operator [](const char* const name){
        return _mapped_values[std::string(name)];
    }


    const int &operator [](const char* const name) const{
        return _mapped_values[std::string(name)];
    }

    BasicClass operator +() const{
        return *this;
    }

    double operator + (const BasicClass & val) const{
        return *val.double_ptr_member + *double_ptr_member;
    }

    BasicClass operator -() const{
        BasicClass copy = *this;
        copy.int_array[0] = -int_array[0];
        copy.int_array[1] = -int_array[1];
        copy.int_array[2] = -int_array[2];
        *const_cast<double*>(copy.double_ptr_member) = -*double_ptr_member;
        return copy;
    }

    BasicClass operator-(const double val){
        return BasicClass(*double_ptr_member - val);
    }

    BasicClass operator ~() const{
        BasicClass copy = *this;
        copy.int_array[0] = ~int_array[0];
        copy.int_array[1] = ~int_array[1];
        copy.int_array[2] = ~int_array[2];
        *const_cast<double*>(copy.double_ptr_member) = -*double_ptr_member;
        return copy;
    }

    static const char* const static_public_method(){
        static const char* const text = "I am the very model...";
        return text;
    }

    const double * const double_ptr_member;
    int int_array[3];

    static constexpr int class_const_member = 42;
    static int class_member;


    bool operator==(const BasicClass& obj) const{
        return int_array[0] == obj.int_array[0] &&
            int_array[1] == obj.int_array[1] &&
            int_array[2] == obj.int_array[2] &&
            *double_ptr_member ==  *obj.double_ptr_member;
    }


private:
    void private_method();
    mutable std::map<std::string, int> _mapped_values;
};


class BasicClass2{
public:
    BasicClass createBasicClass(){
        return BasicClass();
    }
};

class BitFieldContainerClass{
public:
    BitFieldContainerClass():bit(1), char_bits7(111), const_longlong__bits3(3), long_bits5(-2){}
    unsigned char bit: 1;
    unsigned char char_bits7: 7;
    const long long const_longlong__bits3: 3;
    long long_bits5: 5;
};


enum class EnumClass:unsigned char {
    E_ONE=1, E_TWO=2
};



class InheritanceClass: public BasicClass{
public:
    int new_method(const char* const data) const{
        return strlen(data);
    }

};

class MultiInheritanceClass: public BasicClass, public BasicClass2{
public:
    BasicClass2 createBasicClass2(){
        return BasicClass2();
    }
};

class NonDestructible{
public:
    static NonDestructible * create(){ return new NonDestructible();}
    static const NonDestructible * create_const(){ return new const NonDestructible();}
private:
    NonDestructible() = default;
    ~NonDestructible() = default;
};




#endif //PYLLARS_CLASS_TEST_DEFNS_H
