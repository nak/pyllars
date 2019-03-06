
#include <stdlib.h>
#include <Python.h>
#include "gtest/gtest.h"
#include "pyllars/pyllars_containment.hpp"

template<typename T>
void test_container(T& value){
    using namespace __pyllars_test;
    ObjectContainer<T> container(value);
    T& contained = container;
    ASSERT_EQ(&contained, &value);
}

class TestClass{
public:
    TestClass(double v=0.0):_v(v){

    }

    const double &value() const{
        return _v;
    }

private:
    double _v;
};

TEST(ContainmentTestSuite, test_basics){
    static int int_value = 92;
    static TestClass testClass(1.234);
    test_container(int_value);
    test_container(testClass);
}

TEST(ContainmentTestSuite, test_array_and_ptr){
    static int int_array[3] = {0,1,2};
    static TestClass *testClassArray = new TestClass[99];
    test_container(int_array);
    test_container(testClassArray);
}

TEST(ContainmentTestSuite, test_constructed_container) {
    using namespace __pyllars_test;
    ObjectContainerConstructed<TestClass, double> objectContainer(1.2);
    TestClass &instance = objectContainer;
    ASSERT_DOUBLE_EQ(instance.value(), 1.2);
}