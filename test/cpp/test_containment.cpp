
#include <stdlib.h>
#include <Python.h>
#include "gtest/gtest.h"
#include "pyllars/pyllars_containment.hpp"

template<typename T>
void test_container(T& value){
    using namespace __pyllars_internal;
    ObjectContainer<T> container(value);
    T& contained = container;
    ASSERT_EQ(&contained, &value);
}

class TestClass{
public:
    TestClass(double v):_v(v){

    }

    const double &value() const{
        return _v;
    }

    bool operator==(const TestClass v) const{
        return v.value() == value();
    }

private:
    double _v;
};

class Indestructible{
public:
    static Indestructible* create(){return new Indestructible();}
private:
    Indestructible(){}
    ~Indestructible();
};

TEST(ContainmentTestSuite, test_basics){
    static int int_value = 92;
    static TestClass testClass(1.234);
    static Indestructible* ind = Indestructible::create();
    test_container(int_value);
    test_container(testClass);
    test_container(*ind);
}

TEST(ContainmentTestSuite, test_array_and_ptr){
    static int int_array[3] = {0,1,2};
    static TestClass  instanceArray[] = {TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0)};
    static TestClass *testClassArray = &instanceArray[0];
    test_container(int_array);
    test_container(testClassArray);
}


template<typename T>
struct Assertion{
    static void assert_equal(const T& v1, const T& v2){
        ASSERT_EQ(v1, v2);
    }
};

template<>
struct Assertion<double>{
    static void assert_equal(const double &v1, const double& v2){
        ASSERT_DOUBLE_EQ(v1, v2);
    }
};

template<typename T, size_t size>
struct Assertion<T[size]>{

    static void assert_equal(T*const v1, T* const v2){
        for (size_t i = 0; i < size; ++i){
            ASSERT_EQ(v1[i], v2[i]);
        }
    }
};


template<typename T, typename ...Args>
void test_constructed_containers(T instance, Args... args){
    using namespace __pyllars_internal;
    ObjectContainerConstructed<T, Args...> objectContainer(args...);
    Assertion<T>::assert_equal(instance, *objectContainer.ptr());
}

TEST(ContainmentTestSuite, test_constructed_container) {
    using namespace __pyllars_internal;
    TestClass instance(1.2);
    test_constructed_containers<TestClass, double>(instance, 1.2);
    int val = 123;
    test_constructed_containers<int, int>(val, 123);

}


TEST(ContainmentTestSuite, test_constructed_container_array) {
    using namespace __pyllars_internal;

    static int int_array[3] = {0,10,20};
    static TestClass  instanceArray[] = {TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0)};
    static TestClass *testClassArray = &instanceArray[0];
    test_constructed_containers<int[3], int[3]>(int_array, int_array);
    test_constructed_containers<TestClass*, TestClass*>(instanceArray, instanceArray);
}


template<typename T, typename ...Args>
void test_constructed_inplace_containers(T instance, Args ...args){
    using namespace __pyllars_internal;
    static unsigned char raw[sizeof(T)];
    T* instanceP = (T*) raw;
    ObjectContainerInPlace<T, Args...> objectContainer(*instanceP, args...);
    Assertion<T>::assert_equal(instance, *objectContainer.ptr());
}


TEST(ContainmentTestSuite, test_constructed_iplace_container) {
    using namespace __pyllars_internal;
    TestClass instance(1.2);
    test_constructed_inplace_containers<TestClass, double>(instance, 1.2);
    int val = 123;
    test_constructed_containers<int, int>(val, 123);

}


TEST(ContainmentTestSuite, test_constructed_container_inplace_array) {
    using namespace __pyllars_internal;

    static int int_array[3] = {0,10,20};
    static TestClass  instanceArray[] = {TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0),
                                         TestClass(0.0)};
    static TestClass *testClassArray = &instanceArray[0];
    test_constructed_inplace_containers<int[3], int[3]>(int_array, int_array);
    test_constructed_inplace_containers<TestClass*, TestClass*>(instanceArray, instanceArray);
}
