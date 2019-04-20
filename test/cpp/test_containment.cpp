
#include <stdlib.h>
#include <Python.h>
#include "gtest/gtest.h"
#include "pyllars/pyllars_containment.hpp"

template<typename T>
void test_container(T& value){
    using namespace __pyllars_internal;
    ObjectContainerReference<T> container(value);
    T& contained = container;
    ASSERT_EQ(&contained, &value);
}

class TestClass{
public:
    TestClass(double v):_v(v){

    }

    TestClass(const TestClass & t):_v(t._v){

    }

    TestClass(const TestClass && t):_v(t._v){

    }

    const double &value() const{
        return _v;
    }

    bool operator==(const TestClass v) const{
        return fabs(v.value() - value()) < 0.0000001;
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
void test_constructed_containers(T instance, T compare, Args... args){
    using namespace __pyllars_internal;
    ObjectContainerConstructed<T, Args...> objectContainer(std::forward<typename extent_as_pointer<Args>::type>(args)...);
    Assertion<T>::assert_equal(compare, *objectContainer.ptr());
}

TEST(ContainmentTestSuite, test_constructed_container) {
    using namespace __pyllars_internal;
    TestClass instance(1.2);
    test_constructed_containers<TestClass, double>(instance, 1.2,  1.2);
    int val = 0;
    int baseval = 123;
    test_constructed_containers<int, int>(val,baseval, baseval);
    test_constructed_containers<int, const int&>(val,baseval,  baseval);
    test_constructed_containers<const int&, const int&>(val,baseval,  baseval);
    TestClass cval(1234.5);
    TestClass cbaseval(1.0);
    test_constructed_containers<const TestClass, const TestClass&&>(cval,cbaseval,  1.0);

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
    test_constructed_containers<int[3], int[3]>(int_array, int_array, int_array);
    test_constructed_containers<TestClass*, TestClass*>(instanceArray, instanceArray, instanceArray);
}

template<typename T, size_t size>
void test_constructed_bytepool_containers(T &instance){
    using namespace __pyllars_internal;
    typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_element;
    using namespace __pyllars_internal;
    auto element_ctrctr =  [instance](void* address, size_t index)->void{
        new (address)T_element(instance[index]);
    };
    ObjectContainerBytePool<T> objectContainer(size, element_ctrctr);
    for (int i = 0; i < size; ++i){
        Assertion<T_element>::assert_equal(instance[i], objectContainer[i]);
    }
}


TEST(ContainmentTestSuite, test_constructed_container_bytepool_array) {
    using namespace __pyllars_internal;
    unsigned char *raw_int = new unsigned char[sizeof(int)*3];
    FixedArrayHelper<int[3]>* int_arrayP = (FixedArrayHelper<int[3]>*)raw_int;
    static double  *instanceArray = new double[99];
    test_constructed_bytepool_containers<int[3], 3>(int_arrayP->values());
    test_constructed_bytepool_containers<double*, 99>(instanceArray);
}
