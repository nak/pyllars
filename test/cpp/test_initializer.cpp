//
// Created by jrusnak on 3/24/19.
//

#include "pyllars/pyllars.hpp"
#include "gtest/gtest.h"
#include "pyllars/pyllars_reference.impl.hpp"

class InitailizerTest: public ::testing::Test{
protected:
    void SetUp() override{

    }

    void TearDown(){

    }

    pyllars::Initializer initializer;

    class SubInitializer: public pyllars::Initializer{
    public:
        bool setup_called;
        bool ready_called;

        SubInitializer():setup_called(false), ready_called(false){

        }

        int set_up() override{
            setup_called = true;
            return 0;
        }

        int ready(PyObject *const top_level_module) override{
            ready_called = true;
            return 0;
        }

    };

    static void SetUpTestSuite() {
        Py_Initialize();
    }



    static void TearDownTestSuite(){
        PyErr_Clear();
    }

};



TEST_F(InitailizerTest, TestRegisterInitializer) {
    InitailizerTest::SubInitializer subInitializer;
    ASSERT_FALSE(subInitializer.setup_called);
    ASSERT_FALSE(subInitializer.ready_called);
    initializer.register_init(&subInitializer);
    initializer.set_up();
    ASSERT_TRUE(subInitializer.setup_called);
    ASSERT_FALSE(subInitializer.ready_called);
}


TEST_F(InitailizerTest, TestRegisterInitializerLast) {
    InitailizerTest::SubInitializer subInitializer;
    ASSERT_FALSE(subInitializer.setup_called);
    ASSERT_FALSE(subInitializer.ready_called);
    initializer.register_init(&subInitializer);
    initializer.ready(nullptr);
    ASSERT_FALSE(subInitializer.setup_called);
    ASSERT_TRUE(subInitializer.ready_called);
}