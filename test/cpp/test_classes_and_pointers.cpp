#include <stdlib.h>
#include <Python.h>
#include <src/resources/pyllars/pyllars.hpp>
#include "gtest/gtest.h"
#include "pyllars/pyllars.hpp"
#include "pyllars/pyllars_classwrapper.impl"
#include "pyllars/pyllars_pointer.impl"
#include "pyllars/pyllars_membersemantics.impl"
#include "pyllars/pyllars_classmembersemantics.impl"
#include "pyllars/pyllars_classmethodsemantics.impl"

class InitailizerTest: public ::testing::Test{
protected:
    void SetUp() override{

    }

    void TearDown(){

    }

    pyllars::Initializer initializer;

    class SubInitializer: public pyllars::Initializer{
    public:
        bool called;
        bool init_last_called;

        SubInitializer():called(false), init_last_called(false){

        }

        int init(PyObject *const global_module) override{
            called = true;
        }

        int init_last(PyObject *const global_module) override{
            init_last_called = true;
        }

    };
};

class PythonBased: public ::testing::Test{
public:

    class BasicClass{
    public:
        BasicClass():double_ptr_member(new double(2.3)){
            int_array[0] = 1;
            int_array[1] = 2;
            int_array[2] = 3;
        }

        BasicClass(const BasicClass &obj):double_ptr_member(obj.double_ptr_member){
            int_array[0] = 1;
            int_array[1] = 2;
            int_array[2] = 3;
        }

        BasicClass(const BasicClass &&obj):double_ptr_member(obj.double_ptr_member){
            int_array[0] = 1;
            int_array[1] = 2;
            int_array[2] = 3;
        }

        int public_method(const double value){
            return (int)value;
        }

        int operator [](const char* const name){
            return atoi(name);
        }


        int operator [](const char* const name) const{
            return atoi(name);
        }

        static const char* const static_public_method(){
            static const char* const text = "I am the very model...";
            return text;
        }

        const double * const double_ptr_member;
        int int_array[3];

        static constexpr int class_const_member = 42;
        static int class_member;

    private:
        void private_method();
    };

    class ClassWithEnum{
    public:
        enum {FIRST, SECOND, THIRD};
    };

    class BitFieldContainer{
    public:
        BitFieldContainer():bit(1), char_bits7(111), const_longlong__bits3(3), long_bits5(-2){}
        unsigned char bit: 1;
        unsigned char char_bits7: 7;
        const long long const_longlong__bits3: 3;
        long long_bits5: 5;
    };

    enum Enum {ZERO, ONE, TWO};

    enum class EnumClass:unsigned char {
        E_ONE=1, E_TWO=2
    };

    class InheritanceClass: public BasicClass{
    public:
        int new_method(const char* const data) const{
            return strlen(data);
        }

    };

    class BasicClass2{
    public:
        BasicClass createBasicClass(){
            return BasicClass();
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



protected:
    void SetUp() override{
        Py_Initialize();
        PyErr_Clear();
    }

    void TearDown(){
        ASSERT_TRUE(Py_FinalizeEx() == 0);
    }

};

int PythonBased::BasicClass::class_member = 6234;
const int PythonBased::BasicClass::class_const_member;

template<>
const char* const __pyllars_internal::_Types<PythonBased::BasicClass>::type_name = "BasicClass";

template<>
const char* const __pyllars_internal::_Types<PythonBased::InheritanceClass>::type_name = "InheritanceClass";

template<>
const char* const __pyllars_internal::_Types<PythonBased::MultiInheritanceClass>::type_name ="MultiInheritanceClass";

template<>
const char* const __pyllars_internal::_Types<PythonBased::BasicClass2>::type_name = "BasicClass2";

template<>
const char* const __pyllars_internal::_Types<PythonBased::ClassWithEnum>::type_name = "ClassWithEnum";

template<>
const char* const __pyllars_internal::_Types<PythonBased::BitFieldContainer>::type_name = "BitFieldContainer";

template<>
const char* const __pyllars_internal::_Types<decltype(PythonBased::ClassWithEnum::FIRST)>::type_name = "anonymous enum";

template<>
const char* const __pyllars_internal::_Types<PythonBased::Enum>::type_name = "Enum";

template<>
const char* const __pyllars_internal::_Types<PythonBased::EnumClass>::type_name = "EnumClass";

template<>
const char* const __pyllars_internal::_Types<PythonBased::NonDestructible>::type_name = "NonDestructible";

TEST_F(InitailizerTest, TestRegisterInitializer) {
    InitailizerTest::SubInitializer subInitializer;
    ASSERT_FALSE(subInitializer.called);
    ASSERT_FALSE(subInitializer.init_last_called);
    initializer.register_init(&subInitializer);
    initializer.init(nullptr);
    ASSERT_TRUE(subInitializer.called);
    ASSERT_FALSE(subInitializer.init_last_called);
}

TEST_F(InitailizerTest, TestRegisterInitializerLast) {
    InitailizerTest::SubInitializer subInitializer;
    ASSERT_FALSE(subInitializer.called);
    ASSERT_FALSE(subInitializer.init_last_called);
    initializer.register_init_last(&subInitializer);
    initializer.init_last(nullptr);
    ASSERT_FALSE(subInitializer.called);
    ASSERT_TRUE(subInitializer.init_last_called);
}

TEST_F(PythonBased, TestPythonClassWrappperChar){
    ASSERT_EQ(PyType_Ready(__pyllars_internal::PythonClassWrapper<char>::getPyType()), 0);
    PyObject* args = PyTuple_New(0);
    PyObject* obj = PyObject_Call((PyObject*) __pyllars_internal::PythonClassWrapper<char>::getPyType(), args, nullptr);
    ASSERT_NE(obj, nullptr);
    ASSERT_NE(obj->ob_type->tp_as_number, nullptr);
    ASSERT_NE(obj->ob_type->tp_as_number->nb_add, nullptr);
    ASSERT_NE(obj->ob_type->tp_as_number->nb_subtract, nullptr);
    ASSERT_NE(obj->ob_type->tp_as_number->nb_multiply, nullptr);
    ASSERT_NE(obj->ob_type->tp_as_number->nb_remainder, nullptr);
    ASSERT_NE(obj->ob_type->tp_as_number->nb_divmod, nullptr);
    ASSERT_NE(obj->ob_type->tp_as_number->nb_power, nullptr);

}

TEST(TypesName, TestTypeNameVariants){
    ASSERT_STREQ(__pyllars_internal::Types<char>::type_name(), "c_char");
    ASSERT_STREQ(__pyllars_internal::Types<int>::type_name(), "c_int");
    ASSERT_STREQ(__pyllars_internal::Types<unsigned long long>::type_name(), "c_unsigned_long_long");
    ASSERT_STREQ(__pyllars_internal::Types<double>::type_name(), "c_double");
    ASSERT_STREQ(__pyllars_internal::Types<const double>::type_name(), "const_c_double");
}

typedef const char c_string[];

constexpr c_string method_name = "public_method";
constexpr c_string new_method_name = "new_method";
constexpr c_string create_method_name = "create_method";
constexpr c_string create_const_method_name = "create_const_method";
constexpr c_string create_bclass_method_name = "createBasicClass";
constexpr c_string create_bclass2_method_name = "createBasicClass2";
constexpr c_string static_method_name = "static_public_method";
constexpr c_string class_const_member_name = "class_const_member";
constexpr c_string class_member_name = "class_member";
constexpr c_string int_array_member_name = "int_array";
constexpr c_string bit_name = "bit";
constexpr c_string dbl_ptr_member_name = "double_ptr_member";


TEST_F(PythonBased, TestBasicClass){
    using namespace __pyllars_internal;
    const char* const kwlist[] = {"value", nullptr};
    const char* const empty_list[] = {nullptr};
    const char* const kwlist_copy_constr[] = {"obj", nullptr};
    PythonClassWrapper<PythonBased::BasicClass>::addConstructor<>(empty_list);
    PythonClassWrapper<PythonBased::BasicClass>::addConstructor<const PythonBased::BasicClass&>(kwlist_copy_constr);
    PythonClassWrapper<PythonBased::BasicClass>::addConstructor<const PythonBased::BasicClass&&>(kwlist_copy_constr);
    PythonClassWrapper<PythonBased::BasicClass>::addMethod<false, method_name, int, const double>(&PythonBased::BasicClass::public_method, kwlist);
    PythonClassWrapper<PythonBased::BasicClass>::addClassMethod<static_method_name, const char* const>(&PythonBased::BasicClass::static_public_method, kwlist);
    PythonClassWrapper<PythonBased::BasicClass>::addMapOperatorMethod<const char* const, int>(&PythonBased::BasicClass::operator[]);
    PythonClassWrapper<PythonBased::BasicClass>::addMapOperatorMethodConst<const char* const, int>(&PythonBased::BasicClass::operator[]);
    PythonClassWrapper<PythonBased::BasicClass>::addClassAttributeConst<class_const_member_name, int>(&PythonBased::BasicClass::class_const_member);
    PythonClassWrapper<PythonBased::BasicClass>::addClassAttribute<class_member_name, int>(&PythonBased::BasicClass::class_member);
    PythonClassWrapper<PythonBased::BasicClass>::addAttribute<int_array_member_name, int[3]>(&PythonBased::BasicClass::int_array);
    PythonClassWrapper<PythonBased::BasicClass>::addAttribute<dbl_ptr_member_name, const double* const>(&PythonBased::BasicClass::double_ptr_member);
    ASSERT_EQ(PyType_Ready(PythonClassWrapper<PythonBased::BasicClass>::getPyType()), 0);
    PyObject* args = PyTuple_New(0);
    PyObject* obj = PyObject_Call((PyObject*) PythonClassWrapper<PythonBased::BasicClass>::getPyType(),
            args, nullptr);
    ASSERT_NE(obj, nullptr);
    auto dbl_ptr = (PythonClassWrapper<const double* const>*) PyObject_GetAttrString(obj, dbl_ptr_member_name);
    ASSERT_NE(dbl_ptr, nullptr);
    PyObject* at = PyObject_GetAttrString((PyObject*)dbl_ptr, "at");
    ASSERT_NE(at, nullptr);
    PyObject* at_args = PyTuple_New(1);
    PyTuple_SetItem(at_args, 0, PyLong_FromLong(0));
    PyObject* dbl_value = PyObject_Call(at, at_args, nullptr);
    ASSERT_NE(dbl_value, nullptr);
    ASSERT_NEAR(PyFloat_AsDouble(dbl_value), 2.3, 0.000001);

    auto int_array = (PythonClassWrapper<int[3]>*) PyObject_GetAttrString(obj, int_array_member_name);
    ASSERT_NE(int_array, nullptr);
    at = PyObject_GetAttrString((PyObject*)int_array, "at");
    for (int i = 0; i < 3; ++i) {
        PyTuple_SetItem(at_args, 0, PyLong_FromLong(i));
        PyObject *int_value = PyObject_Call(at, at_args, nullptr);
        ASSERT_NE(int_value, nullptr);
        ASSERT_EQ(PyLong_AsLong(int_value), i+1);
    }
    PyTuple_SetItem(at_args, 0, PyLong_FromLong(3));
    PyObject *int_value = PyObject_Call(at, at_args, nullptr);
    ASSERT_EQ(int_value, nullptr);
    ASSERT_TRUE(PyErr_Occurred());
    PyErr_Clear();

    PyTuple_SetItem(at_args, 0, PyLong_FromLong(25));
    PyObject* kwds = PyDict_New();
    PyObject* new_value = PyObject_Call((PyObject*)PythonClassWrapper<int>::getPyType(), at_args, nullptr);
    ASSERT_NE(new_value, nullptr);
    PyTuple_SetItem(at_args, 0, PyLong_FromLong(1));
    PyDict_SetItemString(kwds, "set_value", new_value);
    int_value = PyObject_Call(at, at_args, kwds);
    ASSERT_NE(int_value, nullptr);
    ASSERT_EQ(PyLong_AsLong(int_value), 25);

    PyObject* mapped = PyMapping_GetItemString(obj, "123");
    ASSERT_NE(mapped, nullptr);
    ASSERT_EQ(PyLong_AsLong(mapped), 123);


    PyObject* public_method = PyObject_GetAttrString(obj, method_name);
    ASSERT_NE(public_method, nullptr);
    PyObject* dargs = PyTuple_New(1);
    PyTuple_SetItem(dargs, 0, PyFloat_FromDouble(12.3));
    PyObject* dbl = PyObject_Call((PyObject*)PythonClassWrapper<double>::getPyType(), dargs, nullptr);
    ASSERT_NE(dbl, nullptr);
    PyTuple_SetItem(dargs, 0, dbl);
    PyObject* intValue = PyObject_Call(public_method, dargs, nullptr);
    ASSERT_NE(intValue, nullptr);}

TEST_F(PythonBased, TestClassWithEnum){
    using namespace __pyllars_internal;
    static const char* const empty[] = {nullptr};
    static const char* const value[] = {"value", nullptr};
    PythonClassWrapper<decltype(PythonBased::ClassWithEnum::FIRST)>::addConstructor<decltype(PythonBased::ClassWithEnum::FIRST)>(value);
    PythonClassWrapper<PythonBased::ClassWithEnum>::addConstructor<>(empty);
    PythonClassWrapper<PythonBased::ClassWithEnum>::addEnumClassValue("FIRST", PythonBased::ClassWithEnum::FIRST);
    PythonClassWrapper<PythonBased::ClassWithEnum>::addEnumClassValue("SECOND", PythonBased::ClassWithEnum::SECOND);
    PythonClassWrapper<PythonBased::ClassWithEnum>::addEnumClassValue("THIRD", PythonBased::ClassWithEnum::THIRD);
    ASSERT_EQ(PyType_Ready(PythonClassWrapper<PythonBased::ClassWithEnum>::getPyType()), 0);
    PyObject* args = PyTuple_New(0);
    PyObject* obj = PyObject_Call((PyObject*) PythonClassWrapper<PythonBased::ClassWithEnum>::getPyType(),
                                  args, nullptr);
    ASSERT_NE(obj, nullptr);
    auto first = (PythonClassWrapper<decltype(PythonBased::ClassWithEnum::FIRST)>*) PyObject_GetAttrString(obj, "FIRST");
    ASSERT_NE(first, nullptr);
    ASSERT_EQ(*first->get_CObject(), PythonBased::ClassWithEnum::FIRST);
    auto second = (PythonClassWrapper<decltype(PythonBased::ClassWithEnum::FIRST)>*) PyObject_GetAttrString(obj, "SECOND");
    ASSERT_NE(second, nullptr);
    ASSERT_EQ(*second->get_CObject(), PythonBased::ClassWithEnum::SECOND);
}

TEST_F(PythonBased, TestBitFields){
    using namespace __pyllars_internal;
    static const char* const empty[] = {nullptr};
    PythonClassWrapper<PythonBased::BitFieldContainer>::addConstructor<>(empty);
    __pyllars_internal::BitFieldContainer<PythonBased::BitFieldContainer>::Container<bit_name, unsigned char, 1>::getter_t getter =
            [](const PythonBased::BitFieldContainer& c)->unsigned char{return c.bit;};
    __pyllars_internal::BitFieldContainer<PythonBased::BitFieldContainer>::Container<bit_name, unsigned char, 1>::setter_t setter =
            [](PythonBased::BitFieldContainer& c, const unsigned char &value)->unsigned char{c.bit = value; return value;};
    PythonClassWrapper<PythonBased::BitFieldContainer>::addBitField<bit_name, unsigned char, 1>(
            getter,
            setter);
    PyObject* args = PyTuple_New(0);
    PyObject* obj = PyObject_Call((PyObject*) PythonClassWrapper<PythonBased::BitFieldContainer>::getPyType(),
                                  args, nullptr);
    ASSERT_NE(obj, nullptr);
    PyObject* bit = PyObject_GetAttrString(obj, "bit");
    ASSERT_NE(bit, nullptr);
    ASSERT_EQ(PyLong_AsLong(bit), 1);
    PyObject_SetAttrString(obj, "bit", PyLong_FromLong(0));
    bit = PyObject_GetAttrString(obj, "bit");
    ASSERT_EQ(PyLong_AsLong(bit), 0);
}

TEST_F(PythonBased, TestEnums){
    using namespace __pyllars_internal;
    static const char* const empty[] = {nullptr};
    static const char* const kwlist[] = {"value", nullptr};
    typedef PythonClassWrapper<PythonBased::Enum> Class;
    Class::addConstructor<>(empty);
    Class::addConstructor<Enum>(kwlist);
    Class::initialize();
    Class::addEnumClassValue("ZERO", ZERO);
    PyObject* ZERO_E = PyObject_GetAttrString((PyObject*)Class::getType(), "ZERO");
    ASSERT_NE(ZERO_E, nullptr);
    ASSERT_EQ(*((Class*)ZERO_E)->get_CObject(), ZERO);
    PyObject *args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, ZERO_E);
    Class* new_value = (Class*) PyObject_Call((PyObject*) Class::getPyType(), args, nullptr);
    ASSERT_EQ(*(new_value->get_CObject()), ZERO);
}


TEST_F(PythonBased, TestClassEnums){
    using namespace __pyllars_internal;
    static const char* const empty[] = {nullptr};
    static const char* const kwlist[] = {"value", nullptr};
    typedef PythonClassWrapper<PythonBased::EnumClass> Class;
    Class::addConstructor<>(empty);
    Class::addConstructor<EnumClass>(kwlist);
    Class::initialize();
    Class::addEnumClassValue("E_ONE", PythonBased::EnumClass::E_ONE);
    PyObject* ONE_E = PyObject_GetAttrString((PyObject*)Class::getType(), "E_ONE");
    ASSERT_NE(ONE_E, nullptr);
    ASSERT_EQ(*((Class*)ONE_E)->get_CObject(), PythonBased::EnumClass::E_ONE);
    PyObject *args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, ONE_E);
    Class* new_value = (Class*) PyObject_Call((PyObject*) Class::getPyType(), args, nullptr);
    ASSERT_EQ(*(new_value->get_CObject()), PythonBased::EnumClass::E_ONE);
}

TEST_F(PythonBased, TestPointers){
    using namespace __pyllars_internal;
    typedef PythonClassWrapper<PythonBased::BasicClass> Class;

    const char* const kwlist[] = {"value", nullptr};
    const char* const empty_list[] = {nullptr};
    const char* const kwlist_copy_constr[] = {"obj", nullptr};
    Class::addConstructor<>(empty_list);
    Class::addConstructor<const PythonBased::BasicClass&>(kwlist_copy_constr);
    Class::addConstructor<const PythonBased::BasicClass&&>(kwlist_copy_constr);
    Class::addMethod<false, method_name, int, const double>(&PythonBased::BasicClass::public_method, kwlist);
    Class::addClassMethod<static_method_name, const char* const>(&PythonBased::BasicClass::static_public_method, kwlist);
    Class::addMapOperatorMethod<const char* const, int>(&PythonBased::BasicClass::operator[]);
    Class::addMapOperatorMethodConst<const char* const, int>(&PythonBased::BasicClass::operator[]);
    Class::addClassAttributeConst<class_const_member_name, int>(&PythonBased::BasicClass::class_const_member);
    Class::addClassAttribute<class_member_name, int>(&PythonBased::BasicClass::class_member);
    Class::addAttribute<int_array_member_name, int[3]>(&PythonBased::BasicClass::int_array);
    Class::addAttribute<dbl_ptr_member_name, const double* const>(&PythonBased::BasicClass::double_ptr_member);

    Class::initialize();

    PyObject* obj = PyObject_Call((PyObject*) Class::getPyType(), PyTuple_New(0), nullptr);
    auto self = PyObject_GetAttrString(obj, "this");
    ASSERT_NE(self, nullptr);
    constexpr int MAX = 1000;
    PyObject *ptrs[MAX] = {nullptr};
    PyObject* empty = PyTuple_New(0);
    ptrs[0] = self;
    ASSERT_NE(ptrs[0], nullptr);
    for (int i = 1; i < MAX; ++i){
        auto addr = PyObject_GetAttrString(ptrs[i-1], "this");
        ASSERT_NE(addr, nullptr);
        ptrs[i] = PyObject_Call(addr, empty, nullptr);
        ASSERT_NE(ptrs[i], nullptr);
    }
    auto args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, PyLong_FromLong(0));
    auto derefed = ptrs[MAX-1];
    for(int i = MAX-2; i > 0; --i){
        auto at = PyObject_GetAttrString(derefed, "at");
        ASSERT_NE(at, nullptr);
        derefed = PyObject_Call(at, args, nullptr);
        ASSERT_NE(derefed, nullptr);
        ASSERT_NE(((PythonClassWrapper<PythonBased::BasicClass*>*) derefed)->get_CObject(), nullptr);
        ASSERT_EQ(((PythonClassWrapper<PythonBased::BasicClass*>*) derefed)->get_CObject(),
                  ((PythonClassWrapper<PythonBased::BasicClass*>*)ptrs[i])->get_CObject());
    }
}

TEST_F(PythonBased, TestInheritance){
    using namespace __pyllars_internal;
    const char* const kwlist[] = {"value", nullptr};
    const char* const empty_list[] = {nullptr};
    const char* const kwlist_copy_constr[] = {"obj", nullptr};
    PythonClassWrapper<PythonBased::BasicClass>::addConstructor<>(empty_list);
    PythonClassWrapper<PythonBased::BasicClass>::addConstructor<const PythonBased::BasicClass&>(kwlist_copy_constr);
    PythonClassWrapper<PythonBased::BasicClass>::addConstructor<const PythonBased::BasicClass&&>(kwlist_copy_constr);
    PythonClassWrapper<PythonBased::BasicClass>::addMethod<false, method_name, int, const double>(&PythonBased::BasicClass::public_method, kwlist);
    PythonClassWrapper<PythonBased::BasicClass>::addClassMethod<static_method_name, const char* const>(&PythonBased::BasicClass::static_public_method, kwlist);
    PythonClassWrapper<PythonBased::BasicClass>::addMapOperatorMethod<const char* const, int>(&PythonBased::BasicClass::operator[]);
    PythonClassWrapper<PythonBased::BasicClass>::addMapOperatorMethodConst<const char* const, int>(&PythonBased::BasicClass::operator[]);
    PythonClassWrapper<PythonBased::BasicClass>::addClassAttributeConst<class_const_member_name, int>(&PythonBased::BasicClass::class_const_member);
    PythonClassWrapper<PythonBased::BasicClass>::addClassAttribute<class_member_name, int>(&PythonBased::BasicClass::class_member);
    PythonClassWrapper<PythonBased::BasicClass>::addAttribute<int_array_member_name, int[3]>(&PythonBased::BasicClass::int_array);
    PythonClassWrapper<PythonBased::BasicClass>::addAttribute<dbl_ptr_member_name, const double* const>(&PythonBased::BasicClass::double_ptr_member);

    ASSERT_EQ(PythonClassWrapper<PythonBased::BasicClass>::initialize(), 0);

    PythonClassWrapper<PythonBased::InheritanceClass>::addConstructor<>(empty_list);
    const char* const kwlist2[] = {"data", nullptr};
    PythonClassWrapper<PythonBased::InheritanceClass>::addMethod<true, new_method_name, int, const char* const>(&PythonBased::InheritanceClass::new_method, kwlist2);
    PythonClassWrapper<PythonBased::InheritanceClass>::addBaseClass(PythonClassWrapper<PythonBased::BasicClass>::getPyType());
    ASSERT_EQ(PythonClassWrapper<PythonBased::InheritanceClass>::initialize(), 0);

    PyObject* args = PyTuple_New(0);
    PyObject* obj = PyObject_Call((PyObject*) PythonClassWrapper<PythonBased::InheritanceClass>::getPyType(),
                                  args, nullptr);
    ASSERT_NE(obj, nullptr);
    auto dbl_ptr = (PythonClassWrapper<const double* const>*) PyObject_GetAttrString(obj, dbl_ptr_member_name);
    ASSERT_NE(dbl_ptr, nullptr);
    PyObject* at = PyObject_GetAttrString((PyObject*)dbl_ptr, "at");
    ASSERT_NE(at, nullptr);
    PyObject* at_args = PyTuple_New(1);
    PyTuple_SetItem(at_args, 0, PyLong_FromLong(0));
    PyObject* dbl_value = PyObject_Call(at, at_args, nullptr);
    ASSERT_NE(dbl_value, nullptr);
    ASSERT_NEAR(PyFloat_AsDouble(dbl_value), 2.3, 0.000001);

    PyObject* public_method = PyObject_GetAttrString(obj, method_name);
    ASSERT_NE(public_method, nullptr);
    PyObject* dargs = PyTuple_New(1);
    PyTuple_SetItem(dargs, 0, PyFloat_FromDouble(12.3));
    PyObject* dbl = PyObject_Call((PyObject*)PythonClassWrapper<double>::getPyType(), dargs, nullptr);
    ASSERT_NE(dbl, nullptr);
    PyTuple_SetItem(dargs, 0, dbl);
    PyObject* intValue = PyObject_Call(public_method, dargs, nullptr);
    ASSERT_NE(intValue, nullptr);
}



TEST_F(PythonBased, TestMultipleInheritance){
    using namespace __pyllars_internal;
    const char* const kwlist[] = {"value", nullptr};
    const char* const empty_list[] = {nullptr};
    const char* const kwlist_copy_constr[] = {"obj", nullptr};
    PythonClassWrapper<PythonBased::BasicClass>::addConstructor<>(empty_list);
    PythonClassWrapper<PythonBased::BasicClass>::addConstructor<const PythonBased::BasicClass&>(kwlist_copy_constr);
    PythonClassWrapper<PythonBased::BasicClass>::addConstructor<const PythonBased::BasicClass&&>(kwlist_copy_constr);
    PythonClassWrapper<PythonBased::BasicClass>::addMethod<false, method_name, int, const double>(&PythonBased::BasicClass::public_method, kwlist);
    PythonClassWrapper<PythonBased::BasicClass>::addClassMethod<static_method_name, const char* const>(&PythonBased::BasicClass::static_public_method, kwlist);
    PythonClassWrapper<PythonBased::BasicClass>::addMapOperatorMethod<const char* const, int>(&PythonBased::BasicClass::operator[]);
    PythonClassWrapper<PythonBased::BasicClass>::addMapOperatorMethodConst<const char* const, int>(&PythonBased::BasicClass::operator[]);
    PythonClassWrapper<PythonBased::BasicClass>::addClassAttributeConst<class_const_member_name, int>(&PythonBased::BasicClass::class_const_member);
    PythonClassWrapper<PythonBased::BasicClass>::addClassAttribute<class_member_name, int>(&PythonBased::BasicClass::class_member);
    PythonClassWrapper<PythonBased::BasicClass>::addAttribute<int_array_member_name, int[3]>(&PythonBased::BasicClass::int_array);
    PythonClassWrapper<PythonBased::BasicClass>::addAttribute<dbl_ptr_member_name, const double* const>(&PythonBased::BasicClass::double_ptr_member);

    ASSERT_EQ(PythonClassWrapper<PythonBased::BasicClass>::initialize(), 0);

    PythonClassWrapper<PythonBased::BasicClass2>::addConstructor<>(empty_list);
    PythonClassWrapper<PythonBased::BasicClass2>::addMethod<false, create_bclass_method_name, BasicClass>(&PythonBased::BasicClass2::createBasicClass, kwlist);
    ASSERT_EQ(PythonClassWrapper<PythonBased::BasicClass2>::initialize(), 0);

    PythonClassWrapper<PythonBased::MultiInheritanceClass>::addConstructor<>(empty_list);
    const char* const kwlist2[] = {"data", nullptr};
    PythonClassWrapper<PythonBased::MultiInheritanceClass>::addMethod<false, create_bclass2_method_name, PythonBased::BasicClass2>(&PythonBased::MultiInheritanceClass::createBasicClass2, kwlist2);
    PythonClassWrapper<PythonBased::MultiInheritanceClass>::addBaseClass(PythonClassWrapper<PythonBased::BasicClass>::getPyType());
    ASSERT_EQ(PythonClassWrapper<PythonBased::MultiInheritanceClass>::initialize(), 0);

    PyObject* args = PyTuple_New(0);
    auto* obj = PyObject_Call((PyObject*) PythonClassWrapper<PythonBased::MultiInheritanceClass>::getPyType(),
                                  args, nullptr);
    ASSERT_NE(obj, nullptr);
    auto dbl_ptr = (PythonClassWrapper<const double* const>*) PyObject_GetAttrString(obj, dbl_ptr_member_name);
    ASSERT_NE(dbl_ptr, nullptr);
    PyObject* at = PyObject_GetAttrString((PyObject*)dbl_ptr, "at");
    ASSERT_NE(at, nullptr);
    PyObject* at_args = PyTuple_New(1);
    PyTuple_SetItem(at_args, 0, PyLong_FromLong(0));
    PyObject* dbl_value = PyObject_Call(at, at_args, nullptr);
    ASSERT_NE(dbl_value, nullptr);
    ASSERT_NEAR(PyFloat_AsDouble(dbl_value), 2.3, 0.000001);

    PyObject* public_method = PyObject_GetAttrString(obj, method_name);
    ASSERT_NE(public_method, nullptr);
    PyObject* dargs = PyTuple_New(1);
    PyTuple_SetItem(dargs, 0, PyFloat_FromDouble(12.3));
    PyObject* dbl = PyObject_Call((PyObject*)PythonClassWrapper<double>::getPyType(), dargs, nullptr);
    ASSERT_NE(dbl, nullptr);
    PyTuple_SetItem(dargs, 0, dbl);
    PyObject* intValue = PyObject_Call(public_method, dargs, nullptr);
    ASSERT_NE(intValue, nullptr);

    PyObject* createBaseClass2_method = PyObject_GetAttrString(obj, create_bclass2_method_name);
    ASSERT_NE(createBaseClass2_method, nullptr);
    PyObject* bclass2 = PyObject_Call(createBaseClass2_method, PyTuple_New(0), nullptr);
    ASSERT_NE(bclass2, nullptr);
    ASSERT_TRUE(PyObject_TypeCheck(bclass2, PythonClassWrapper<PythonBased::BasicClass2>::getPyType()));
}

TEST_F(PythonBased, TestPrivateCtrDestructor){
    using namespace __pyllars_internal;
    const char* const empty_list[] = {nullptr};
    typedef PythonClassWrapper<PythonBased::NonDestructible> Class;

    Class::addClassMethod<create_method_name, PythonBased::NonDestructible*>(PythonBased::NonDestructible::create, empty_list);
    Class::addClassMethod<create_const_method_name, const PythonBased::NonDestructible*>(PythonBased::NonDestructible::create_const, empty_list);
    Class::initialize();

    PyObject* create = PyObject_GetAttrString((PyObject*)Class::getPyType(), create_method_name);
    ASSERT_NE(create, nullptr);
    PyObject* instance = PyObject_Call(create, PyTuple_New(0), nullptr);
    ASSERT_NE(instance, nullptr);
    ASSERT_TRUE(PyObject_TypeCheck(instance, PythonClassWrapper<PythonBased::NonDestructible*>::getPyType()));

    PyObject* create_const = PyObject_GetAttrString((PyObject*)Class::getPyType(), create_const_method_name);
    ASSERT_NE(create_const, nullptr);
    PyObject* instance_const = PyObject_Call(create_const, PyTuple_New(0), nullptr);
    ASSERT_NE(instance_const, nullptr);
    ASSERT_TRUE(PyObject_TypeCheck(instance_const, PythonClassWrapper<const PythonBased::NonDestructible*>::getPyType()));
}