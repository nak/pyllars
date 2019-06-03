//
// Created by jrusnak on 3/24/19.
//

#ifndef PYLLARS_CLASS_TEST_DEFNS_H
#define PYLLARS_CLASS_TEST_DEFNS_H

#include<string>
#include<map>

template<typename T>
class SingleTypeParamTemplateClass{
public:
    SingleTypeParamTemplateClass(const T & d):_value(d){}

    const T & val(){return _value;}

private:
    const T _value;
};

template<long value>
class SingleValueParamTemplateClass{
public:
    SingleValueParamTemplateClass(const long l):_value(l){
    }

    const long & val(){return _value;}

private:
    const long _value;
};


template<typename ...Args>
class MultTypeParamClass{
public:
};


template<typename T1, typename T2, int val, typename ...Args2>
class MultiMixedParamClass{
public:
};

#endif //PYLLARS_CLASS_TEST_DEFNS_H
