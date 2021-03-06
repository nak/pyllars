//
// Created by jrusnak on 3/24/19.
//
#include "setup_basic_class.h"
#include "class_test_defns.h"
#include "pyllars/pyllars.hpp"
#include <gtest-param-test.h>

#include "pyllars/pyllars_classmethod.hpp"
#include "pyllars/pyllars_classstaticmethod.hpp"
#include "pyllars/pyllars_classconstructor.hpp"
#include "pyllars/pyllars_enum.hpp"
#include "pyllars/pyllars_classoperator.hpp"
#include "pyllars/pyllars_classstaticmember.hpp"
#include "pyllars/pyllars_classmember.hpp"
#include "pyllars/pyllars_classbitfield.hpp"
#include "pyllars/pyllars_class.hpp"

TEST_F(SetupBasicClass, TestBasicClassNew) {
    using namespace __pyllars_internal;
    PyObject *new_op = PyObject_GetAttrString((PyObject*)PythonClassWrapper<BasicClass>::getPyType(), "new");

    ASSERT_NE(new_op, nullptr);
    ASSERT_FALSE(PyErr_Occurred());

    PyObject* args_tuple1 = PyTuple_New(1);
    PyTuple_SetItem(args_tuple1, 0, PyFloat_FromDouble(1.1));
    PyObject* args_tuple2 = PyTuple_New(2);
    PyTuple_SetItem(args_tuple2, 0, PyFloat_FromDouble(2.2));
    PyTuple_SetItem(args_tuple2, 1, PyUnicode_FromString("unused"));

    PyObject* tuple_list = PyList_New(0);
    PyList_Append(tuple_list, args_tuple1);
    PyList_Append(tuple_list, args_tuple2);

    PyObject* call_args = PyTuple_New(1);
    PyTuple_SetItem(call_args, 0, tuple_list);
    PyObject* obj = PyObject_Call(new_op, call_args, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_NE(obj, nullptr);
    Py_DECREF(obj);

    PyTuple_SetItem(call_args, 0, args_tuple2);
    obj = PyObject_Call(new_op, call_args, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_NE(obj, nullptr);
    Py_DECREF(obj);

    PyTuple_SetItem(call_args, 0, PyLong_FromLong(100));
    obj = PyObject_Call(new_op, call_args, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_NE(obj, nullptr);

    Py_DECREF(obj);
}

TEST_F(SetupBasicClass, TestBasicClass){
    using namespace __pyllars_internal;
    static const char* const kwlist[] = {"_CObject", nullptr};
    static const char* const empty_list[] = {nullptr};
    static const char* const kwlist_copy_constr[] = {"obj", nullptr};
    PyObject* args = PyTuple_New(0);
    PyObject* obj = PyObject_Call((PyObject*) PythonClassWrapper<BasicClass>::getPyType(), args, nullptr);
    PyObject* objconst = PyObject_Call((PyObject*) PythonClassWrapper<const BasicClass>::getPyType(), args, nullptr);
    ASSERT_NE(obj, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    auto dbl_ptr = (PythonClassWrapper<const double* const>*) PyObject_GetAttrString(obj, dbl_ptr_member_name);
    ASSERT_NE(dbl_ptr, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    PyObject* at = PyObject_GetAttrString((PyObject*)dbl_ptr, "at");
    ASSERT_NE(at, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    PyObject* at_args = PyTuple_New(1);
    PyTuple_SetItem(at_args, 0, PyLong_FromLong(0));
    PyObject* dbl_value = PyObject_Call(at, at_args, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_NE(dbl_value, nullptr);
    ASSERT_NEAR(PyFloat_AsDouble(dbl_value), 2.3, 0.000001);

    auto const_int = PyObject_GetAttrString(obj, const_int_member_name);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_NE(const_int, nullptr);
    long value = PyLong_AsLong(const_int);
    ASSERT_EQ(value, -2348);

    auto int_array = (PythonClassWrapper<int[3]>*) PyObject_GetAttrString(obj, int_array_member_name);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_NE(int_array, nullptr);
    at = PyObject_GetAttrString((PyObject*)int_array, "at");
    for (int i = 0; i < 3; ++i) {
        PyTuple_SetItem(at_args, 0, PyLong_FromLong(i));
        PyObject *int_value = PyObject_Call(at, at_args, nullptr);
        ASSERT_FALSE(PyErr_Occurred());
        ASSERT_NE(int_value, nullptr);
        ASSERT_EQ(PyLong_AsLong(int_value), i+1);
    }
    PyTuple_SetItem(at_args, 0, PyLong_FromLong(3));
    PyObject *int_value = PyObject_Call(at, at_args, nullptr);
    ASSERT_TRUE(PyErr_Occurred()); // index out of range
    PyErr_Clear();
    ASSERT_EQ(int_value, nullptr);

    PyTuple_SetItem(at_args, 0, PyLong_FromLong(25));
    PyObject* kwds = PyDict_New();
    PythonClassWrapper<int>* new_value = (PythonClassWrapper<int>*) PyObject_Call((PyObject*)PythonClassWrapper<int>::getPyType(), at_args, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_NE(new_value, nullptr);
    PyTuple_SetItem(at_args, 0, PyLong_FromLong(1));
    PyDict_SetItemString(kwds, "set_value", (PyObject*) new_value);
    ASSERT_FALSE(PyErr_Occurred());
    int_value = PyObject_Call(at, at_args, kwds);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_NE(int_value, nullptr);
    ASSERT_EQ(PyLong_AsLong(int_value), 25);

    {
        PyObject *mapped = PyMapping_GetItemString(obj, "123");
        ASSERT_FALSE(PyErr_Occurred());
        ASSERT_NE(mapped, nullptr);
        ASSERT_EQ(PyLong_AsLong(mapped), 123);
        PyMapping_SetItemString(obj, "123", PyLong_FromLong(99));
        ASSERT_FALSE(PyErr_Occurred());
        mapped = PyMapping_GetItemString(obj, "123");
        ASSERT_FALSE(PyErr_Occurred());
        ASSERT_NE(mapped, nullptr);
        ASSERT_EQ(PyLong_AsLong(mapped), 99);
    }
    {
        PyObject *mapped = PyMapping_GetItemString(objconst, "123");
        ASSERT_FALSE(PyErr_Occurred());
        ASSERT_NE(mapped, nullptr);
        ASSERT_EQ(PyLong_AsLong(mapped), 123);
        typedef const char* cstring;
        ASSERT_EQ( PyMapping_SetItemString(objconst, "123", PyLong_FromLong(99)), -1);
        ASSERT_TRUE(PyErr_Occurred());
        PyErr_Clear();
    }
    PyObject* public_method = PyObject_GetAttrString(obj, method_name);
    ASSERT_NE(public_method, nullptr);
    PyObject* dargs = PyTuple_New(1);
    PyTuple_SetItem(dargs, 0, PyFloat_FromDouble(12.3));
    PyObject* dbl = PyObject_Call((PyObject*)PythonClassWrapper<double>::getPyType(), dargs, nullptr);
    ASSERT_NE(dbl, nullptr);
    PyTuple_SetItem(dargs, 0, dbl);
    PyObject* intValue = PyObject_Call(public_method, dargs, nullptr);
    ASSERT_NE(intValue, nullptr);

    {
        int int_vals[3] = {6, 7, 8};
        PyObject *vals = toPyObject<int[3]>(int_vals, 3);
        ASSERT_NE(vals, nullptr);
        PyObject_SetAttrString(obj, int_array_member_name, vals);
        vals = PyObject_GetAttrString(obj, int_array_member_name);
        ASSERT_NE(vals, nullptr);
        for (size_t i = 0; i < 3; ++i) {
            ASSERT_EQ((*reinterpret_cast<PythonClassWrapper<int[3]> *>(vals)->get_CObject())[i], int_vals[i]);
        }
    }
    {
        int *int_vals = new int[3]{6, 7, 8};

        PyObject *vals = toPyObject<int*>(int_vals, 3);
        ASSERT_NE(vals, nullptr);
        PyObject_SetAttrString(obj, int_array_member_name, vals);
        vals = PyObject_GetAttrString(obj, int_array_member_name);
        ASSERT_NE(vals, nullptr);
        for (size_t i = 0; i < 3; ++i) {
            ASSERT_EQ((*reinterpret_cast<PythonClassWrapper<int[3]> *>(vals)->get_CObject())[i], int_vals[i]);
        }
        delete[] int_vals;
    }
    Py_DECREF(obj);
}

TEST_F(SetupBasicClass, TestClassEnums){
    using namespace __pyllars_internal;
    typedef PythonClassWrapper< EnumClass> Class;

    PyObject* ONE_E = PyObject_GetAttrString((PyObject*)Class::getType(), "E_ONE");
    ASSERT_NE(ONE_E, nullptr);
    ASSERT_NE((((Class*)ONE_E)->get_CObject()), nullptr);
    ASSERT_EQ(*(((Class*)ONE_E)->get_CObject()), EnumClass::E_ONE);
    {
        PyObject *value_callable = PyObject_GetAttrString(ONE_E, "value");
        ASSERT_NE(value_callable, nullptr);
        ASSERT_FALSE(PyErr_Occurred());
        PyObject *args = PyTuple_New(1);
        PyTuple_SetItem(args, 0, ONE_E);
        PyObject *int_value_py = PyObject_Call(value_callable, args, nullptr);
        ASSERT_FALSE(PyErr_Occurred());
        ASSERT_NE(int_value_py, nullptr);
        ASSERT_EQ(PyLong_AsLong(int_value_py), 1);
    }
    {
        PyObject *args = PyTuple_New(1);
        PyTuple_SetItem(args, 0, ONE_E);
        Class *new_value = (Class *) PyObject_Call((PyObject *) Class::getPyType(), args, nullptr);
        ASSERT_NE((new_value->get_CObject()), nullptr);
        ASSERT_EQ(*(new_value->get_CObject()), EnumClass::E_ONE);
    }
    PyObject* convert = PyObject_GetAttrString((PyObject*)Class::getPyType(), enum_convert_name);
    ASSERT_NE(convert, nullptr);
    PyObject * args2 = PyTuple_New(1);
    PyTuple_SetItem(args2, 0, ONE_E);
    PyObject* intVal = PyObject_Call(convert, args2, nullptr);
    ASSERT_NE(intVal, nullptr);
    ASSERT_EQ(PyInt_AsLong(intVal), 1);
}

TEST_F(SetupBasicClass, TestBasicClassOffNominal) {
    using namespace __pyllars_internal;
    static const char *const kwlist[] = {"_CObject", nullptr};
    static const char *const empty_list[] = {nullptr};
    static const char *const kwlist_copy_constr[] = {"obj", nullptr};
    PyObject *args = PyTuple_New(2);
    PyTuple_SetItem(args, 0, PyLong_FromLong(1));
    PyTuple_SetItem(args, 1, PyLong_FromLong(2));
    PyObject *obj = PyObject_Call((PyObject *) PythonClassWrapper<BasicClass>::getPyType(),
                                  args, nullptr);
    ASSERT_EQ(obj, nullptr);
    ASSERT_TRUE(PyErr_Occurred());
    PyErr_Clear();
    static auto empty = PyTuple_New(0);
    PyObject* kwds = PyDict_New();
    PyDict_SetItemString(kwds, "ununsed", PyLong_FromLong(33));
    obj =  PyObject_Call((PyObject *) PythonClassWrapper<BasicClass>::getPyType(), empty, kwds);
    ASSERT_EQ(obj, nullptr);
    ASSERT_TRUE(PyErr_Occurred());
    PyErr_Clear();
}

TEST_F(SetupBasicClass, TestPointers){
    using namespace __pyllars_internal;
    typedef PythonClassWrapper<BasicClass> Class;
    static auto empty = PyTuple_New(0);
    PyObject* obj = PyObject_Call((PyObject*) Class::getPyType(), empty, nullptr);
    ASSERT_NE(obj, nullptr);
    auto self = PyObject_GetAttrString(obj, "this");
    ASSERT_NE(self, nullptr);

    constexpr int MAX = 1000;
    PyObject *ptrs[MAX] = {nullptr};
    ptrs[0] = self;

    for (int i = 1; i < MAX; ++i){
        auto addr = PyObject_GetAttrString(ptrs[i-1], "this");
        ASSERT_NE(addr, nullptr);
        ptrs[i] = PyObject_Call(addr, empty, nullptr);
        ASSERT_NE(ptrs[i], nullptr);
        ASSERT_EQ((void*)*((PythonClassWrapper<BasicClass*>*)ptrs[i])->get_CObject(),
                  (void*)((PythonClassWrapper<BasicClass*>*)ptrs[i-1])->get_CObject());
    }
    auto args_index_0 = PyTuple_New(1);
    PyTuple_SetItem(args_index_0, 0, PyLong_FromLong(0));
    auto derefed = ptrs[MAX-1];
    for(int i = MAX-2; i > 0; --i){
        auto at = PyObject_GetAttrString(derefed, "at");
        ASSERT_NE(at, nullptr);
        derefed = PyObject_Call(at, args_index_0, nullptr);
        ASSERT_NE(derefed, nullptr);
        ASSERT_NE(((PythonClassWrapper<BasicClass*>*)ptrs[i])->get_CObject(), nullptr);
        ASSERT_NE(((PythonClassWrapper<BasicClass*>*) derefed)->get_CObject(), nullptr);
        ASSERT_EQ(*((PythonClassWrapper<BasicClass*>*) derefed)->get_CObject(),
                  *((PythonClassWrapper<BasicClass*>*)ptrs[i])->get_CObject());
    }
}

TEST_F(SetupBasicClass, TestPrivateCtrDestructor){
    using namespace __pyllars_internal;
    static const char* const empty_list[] = {nullptr};
    typedef PythonClassWrapper<NonDestructible> Class;


    PyObject* create = PyObject_GetAttrString((PyObject*)Class::getPyType(), create_method_name);
    ASSERT_NE(create, nullptr);
    PyObject* instance = PyObject_Call(create, PyTuple_New(0), nullptr);
    ASSERT_NE(instance, nullptr);
    ASSERT_TRUE(PyObject_TypeCheck(instance, PythonClassWrapper<NonDestructible*>::getPyType()));

    PyObject* create_const = PyObject_GetAttrString((PyObject*)Class::getPyType(), create_const_method_name);
    ASSERT_NE(create_const, nullptr);
    PyObject* instance_const = PyObject_Call(create_const, PyTuple_New(0), nullptr);
    ASSERT_NE(instance_const, nullptr);
    ASSERT_TRUE(PyObject_TypeCheck(instance_const, PythonClassWrapper<const NonDestructible*>::getPyType()));
}

TEST_F(SetupBasicClass, TestBasicClassUnaryOperators){
    using namespace __pyllars_internal;
    typedef PythonClassWrapper<BasicClass> Class;


    ASSERT_NE(Class::getPyType()->tp_as_number->nb_positive, nullptr);
    ASSERT_NE(Class::getPyType()->tp_as_number->nb_negative, nullptr);
    ASSERT_NE(Class::getPyType()->tp_as_number->nb_invert, nullptr);
    PyObject * obj = PyObject_Call((PyObject*)Class::getPyType(), PyTuple_New(0), nullptr);
    ASSERT_NE(obj, nullptr);
    PyObject* negate = PyObject_GetAttrString(obj, "__neg__");
    ASSERT_NE(negate, nullptr);
    PyObject* negobj = PyObject_Call(negate, PyTuple_New(0), nullptr);
    ASSERT_NE(negobj, nullptr);
    ASSERT_TRUE(PyObject_TypeCheck(negobj, Class::getPyType()));
    ASSERT_NE(((Class*)negobj)->get_CObject(), nullptr);
    ASSERT_EQ(((Class*)negobj)->get_CObject()->int_array[0], -1);
    ASSERT_EQ(((Class*)negobj)->get_CObject()->int_array[1], -2);
    ASSERT_EQ(((Class*)negobj)->get_CObject()->int_array[2], -3);
}


TEST_F(SetupBasicClass, TestBasicClassBinaryOperators){
    using namespace __pyllars_internal;
    typedef PythonClassWrapper<BasicClass> Class;

    ASSERT_NE(Class::getPyType()->tp_as_number->nb_add, nullptr);
    ASSERT_NE(Class::getPyType()->tp_as_number->nb_subtract, nullptr);

    PyObject * obj = PyObject_Call((PyObject*)Class::getPyType(), PyTuple_New(0), nullptr);
    ASSERT_NE(obj, nullptr);
    PyObject* subtract = PyObject_GetAttrString(obj, "__sub__");
    ASSERT_NE(subtract, nullptr);
    PyObject * args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, PyFloat_FromDouble(1.1));
    PyObject* subtracted = PyObject_Call(subtract, args, nullptr);
    ASSERT_NE(subtracted, nullptr);
    ASSERT_TRUE(PyObject_TypeCheck(subtracted, Class::getPyType()));
    auto dbl_ptr = (PythonClassWrapper<const double* const>*) PyObject_GetAttrString(subtracted, dbl_ptr_member_name);
    ASSERT_NE(dbl_ptr, nullptr);
    PyObject* at = PyObject_GetAttrString((PyObject*)dbl_ptr, "at");
    ASSERT_NE(at, nullptr);
    PyObject* at_args = PyTuple_New(1);
    PyTuple_SetItem(at_args, 0, PyLong_FromLong(0));
    PyObject* dbl_value = PyObject_Call(at, at_args, nullptr);
    ASSERT_NE(dbl_value, nullptr);
    ASSERT_DOUBLE_EQ(PyFloat_AsDouble(dbl_value), 1.2);
}

class UnaryOperatorTest : public SetupBasicClass,
public testing::WithParamInterface<std::pair<__pyllars_internal::OpUnaryEnum, const BasicClass*> >{

};

TEST_P(UnaryOperatorTest, InvokesOperator){
    using namespace __pyllars_internal;
    typedef PythonClassWrapper<BasicClass> Class;


    static const std::map<OpUnaryEnum, const char*> opUnaryNameMap = {
            {OpUnaryEnum::INV, "__invert__"},
            {OpUnaryEnum::POS, "__pos__"},
            {OpUnaryEnum::NEG, "__neg__"}
    };

    auto  [kind, expected] = GetParam();
    PyObject * obj = PyObject_Call((PyObject*)Class::getPyType(), PyTuple_New(0), nullptr);
    ASSERT_NE(obj, nullptr);
    const char* const name = opUnaryNameMap.at(kind);
    auto func = PyObject_GetAttrString(obj, name);
    ASSERT_NE(func, nullptr);
    auto pyobj = PyObject_Call(func, PyTuple_New(0), nullptr);
    ASSERT_NE(pyobj, nullptr);
    const BasicClass& cobj = toCArgument<BasicClass>(*pyobj).value();
    ASSERT_EQ(cobj, *expected);
}

static const BasicClass val_pos = BasicClass();
static const BasicClass val_neg = -val_pos;
static const BasicClass val_inv = ~val_pos;

#ifndef INSTANTIATE_TEST_SUITE_P
#define INSTANTIATE_TEST_SUITE_P INSTANTIATE_TEST_CASE_P
#endif


INSTANTIATE_TEST_SUITE_P(UnaryOperatorTestSuite, UnaryOperatorTest, ::testing::Values(std::pair<__pyllars_internal::OpUnaryEnum, const BasicClass*>(__pyllars_internal::OpUnaryEnum::POS, &val_pos),
        std::pair<__pyllars_internal::OpUnaryEnum, const BasicClass*>(__pyllars_internal::OpUnaryEnum::NEG, &val_neg),
        std::pair<__pyllars_internal::OpUnaryEnum, const BasicClass*>(__pyllars_internal::OpUnaryEnum::INV, &val_inv)
        ));


class BinaryOperatorTest : public SetupBasicClass,
public testing::WithParamInterface<std::tuple<__pyllars_internal::OpBinaryEnum, const BasicClass*, std::function<void(PyObject*)> > >{

};

TEST_P(BinaryOperatorTest, InvokeOperator){
    using namespace __pyllars_internal;

    static const std::map<OpBinaryEnum, const char*> opBinaryNameMap= {
            {OpBinaryEnum::ADD, "__add__"},
            {OpBinaryEnum::SUB, "__sub__"},
            {OpBinaryEnum::MUL, "__mul__"},
            {OpBinaryEnum::DIV, "__div__"},
            {OpBinaryEnum::AND, "__and__"},
            {OpBinaryEnum::OR, "__or__"},
            {OpBinaryEnum::XOR, "__xor__"},
            {OpBinaryEnum::MOD, "__mod__"},
            {OpBinaryEnum::LSHIFT, "__lshift__"},
            {OpBinaryEnum::RSHIFT, "__rshift__"},

            {OpBinaryEnum::IADD, "__iadd__"},
            {OpBinaryEnum::ISUB, "__isub__"},
            {OpBinaryEnum::IMUL, "__imul__"},
            {OpBinaryEnum::IDIV, "__idiv__"},
            {OpBinaryEnum::IAND, "__iand__"},
            {OpBinaryEnum::IOR, "__ior__"},
            {OpBinaryEnum::IXOR, "__ixor__"},
            {OpBinaryEnum::IMOD, "__imod__"},
            {OpBinaryEnum::ILSHIFT, "__ilshift__"},
            {OpBinaryEnum::IRSHIFT, "__irshift__"}
    };
    typedef PythonClassWrapper<BasicClass> Class;

    auto  [kind, val1, expectation] = GetParam();
    PyObject * obj = PyObject_Call((PyObject*)Class::getPyType(), PyTuple_New(0), nullptr);
    ASSERT_NE(obj, nullptr);
    auto func = PyObject_GetAttrString(obj, (const char* const)opBinaryNameMap.at(kind));
    ASSERT_NE(func, nullptr);
    ASSERT_TRUE(PyCallable_Check(func));
    auto args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, (PyObject*)PythonClassWrapper<const BasicClass&>::fromCObject(*val1));
    auto pyobj = PyObject_Call(func, args, nullptr);
    ASSERT_NE(pyobj, nullptr);
    expectation(pyobj);
}

static const BasicClass val_add = val_pos;
static const BasicClass expected_add = BasicClass(val_pos + val_pos);
static std::function<void(PyObject*)> expectation_add = [](PyObject* result){
    using namespace __pyllars_internal;
    ASSERT_TRUE(PyObject_TypeCheck(result, PythonClassWrapper<double>::getPyType()));
    ASSERT_FLOAT_EQ(*reinterpret_cast<PythonClassWrapper<double>*>(result)->get_CObject(), *expected_add.double_ptr_member);
};

INSTANTIATE_TEST_SUITE_P(BinaryOperatorTestSuite, BinaryOperatorTest, ::testing::Values(
        std::tuple<__pyllars_internal::OpBinaryEnum, const BasicClass*, std::function<void(PyObject*)>>(__pyllars_internal::OpBinaryEnum::ADD, &val_pos, expectation_add)
));
