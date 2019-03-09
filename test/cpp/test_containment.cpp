
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
    using namespace __pyllars_internal;
    ObjectContainerConstructed<TestClass, double> objectContainer(1.2);
    TestClass &instance = objectContainer;
    ASSERT_DOUBLE_EQ(instance.value(), 1.2);
}


TEST(ContainmentTestSuite, test_constructed_container_array) {
    using namespace __pyllars_internal;

    static int int_array[3] = {0,10,20};
    static TestClass *testClassArray = new TestClass[99];
    ObjectContainerConstructed<int[3], int[3]> objectContainer(int_array);
    int *val = *objectContainer.ptr();
    ASSERT_EQ(val[0], 0);
    ASSERT_EQ(val[1], 10);
    ASSERT_EQ(val[2], 20);
}