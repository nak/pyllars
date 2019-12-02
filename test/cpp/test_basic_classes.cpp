//
// Created by jrusnak on 3/24/19.
//
#define MODE_IMPORT
#include "setup_basic_class.h"
#include "class_test_defns.h"
#include "pyllars/pyllars.hpp"
#include "pyllars/pyllars_definitions.hpp"
#include "pyllars/internal/pyllars_reference.hpp"
#include "pyllars/pyllars_namespacewrapper.hpp"

TEST_F(SetupBasicClass, TestBasicClassNew) {
    PyObject* PyBasicClass = PyObject_GetAttrString(pyllars::GlobalNS::module(), "BasicClass");
    ASSERT_NE(PyBasicClass, nullptr);
    ASSERT_TRUE(PyType_Check(PyBasicClass));
    PyObject *new_op = PyObject_GetAttrString(PyBasicClass, "new");

    ASSERT_NE(new_op, nullptr);
    ASSERT_FALSE(PyErr_Occurred());

    PyObject *args_tuple1 = PyTuple_New(1);
    PyTuple_SetItem(args_tuple1, 0, PyFloat_FromDouble(1.1));
    PyObject *args_tuple2 = PyTuple_New(2);
    PyTuple_SetItem(args_tuple2, 0, PyFloat_FromDouble(2.2));
    PyTuple_SetItem(args_tuple2, 1, PyUnicode_FromString("unused"));
    PyObject *call_args = PyTuple_New(1);

    {
        PyObject *tuple_list = PyList_New(0);
        PyList_Append(tuple_list, args_tuple1);
        PyList_Append(tuple_list, args_tuple2);

        PyTuple_SetItem(call_args, 0, tuple_list);

        PyObject *obj = PyObject_Call(new_op, call_args, nullptr);
        ASSERT_FALSE(PyErr_Occurred());
        ASSERT_NE(obj, nullptr);
        Py_DECREF(obj);
    }

    PyTuple_SetItem(call_args, 0, args_tuple2);
    PyObject* obj = PyObject_Call(new_op, call_args, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_NE(obj, nullptr);
    Py_DECREF(obj);

    PyTuple_SetItem(call_args, 0, PyLong_FromLong(100));
    //args_tuple2 no longer referenced as call_args "owned" it and first element is now replaced
    obj = PyObject_Call(new_op, call_args, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_NE(obj, nullptr);

    Py_DECREF(obj);
    Py_DECREF(call_args);
    Py_DECREF(args_tuple1);
    Py_DECREF(new_op);
}

TEST_F(SetupBasicClass, TestBasicClassConst) {
    PyObject* PyBasicClass = PyObject_GetAttrString(pyllars::GlobalNS::module(), "BasicClass");
    ASSERT_NE(PyBasicClass, nullptr);
    ASSERT_TRUE(PyType_Check(PyBasicClass));
    PyObject *const_type = PyObject_GetAttrString(PyBasicClass, "const");
    PyObject *new_op = PyObject_GetAttrString(const_type, "new");

    ASSERT_NE(new_op, nullptr);
    ASSERT_FALSE(PyErr_Occurred());

    PyObject* args_tuple1 = PyTuple_New(1);
    PyTuple_SetItem(args_tuple1, 0, PyFloat_FromDouble(1.1));
    PyObject* args_tuple2 = PyTuple_New(2);
    PyTuple_SetItem(args_tuple2, 0, PyFloat_FromDouble(2.2));
    PyTuple_SetItem(args_tuple2, 1, PyUnicode_FromString("unused"));
    PyObject *call_args = PyTuple_New(1);

    {
        PyObject *tuple_list = PyList_New(0);
        PyList_Append(tuple_list, args_tuple1);
        PyList_Append(tuple_list, args_tuple2);

        PyTuple_SetItem(call_args, 0, tuple_list);
        PyObject *obj = PyObject_Call(new_op, call_args, nullptr);
        ASSERT_FALSE(PyErr_Occurred());
        ASSERT_NE(obj, nullptr);
        Py_DECREF(obj);
    }

    PyTuple_SetItem(call_args, 0, args_tuple2);
    PyObject* obj = PyObject_Call(new_op, call_args, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_NE(obj, nullptr);
    Py_DECREF(obj);

    PyTuple_SetItem(call_args, 0, PyLong_FromLong(100));
    //args_tuple2, which was solely owned by call_args is now reference count 0
    obj = PyObject_Call(new_op, call_args, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_NE(obj, nullptr);
    Py_DECREF(obj);
    Py_DECREF(call_args);
    Py_DECREF(args_tuple1);
    Py_DECREF(new_op);
}

TEST_F(SetupBasicClass, TestBasicClass){
    PyObject* PyBasicClass = PyObject_GetAttrString(pyllars::GlobalNS::module(), "BasicClass");
    ASSERT_NE(PyBasicClass, nullptr);
    ASSERT_TRUE(PyType_Check(PyBasicClass));
    PyObject* PyBasicClassConst = PyObject_GetAttrString(PyBasicClass, "const");
    PyObject* args = PyTuple_New(0);
    PyObject* obj = PyObject_Call(PyBasicClass, args, nullptr);
    PyObject* objconst = PyObject_Call(PyBasicClassConst, args, nullptr);
    ASSERT_NE(obj, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    auto dbl_ptr = PyObject_GetAttrString(obj, dbl_ptr_member_name);
    ASSERT_NE(dbl_ptr, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    PyObject* at = PyObject_GetAttrString(dbl_ptr, "at");
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

    auto int_array = PyObject_GetAttrString(obj, int_array_member_name);
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
    auto* c_int = PyObject_GetAttrString(pyllars::GlobalNS::module(), "c_int");
    auto* new_value = PyObject_Call(c_int, at_args, nullptr);
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
    auto c_double = PyObject_GetAttrString(pyllars::GlobalNS::module(), "c_double");
    PyObject* dbl = PyObject_Call(c_double, dargs, nullptr);
    ASSERT_NE(dbl, nullptr);
    PyTuple_SetItem(dargs, 0, dbl);
    PyObject* intValue = PyObject_Call(public_method, dargs, nullptr);
    ASSERT_NE(intValue, nullptr);

    {
        int int_vals[3] = {6,7,8};
        auto tuple = PyTuple_Pack(3, PyLong_FromLong(6), PyLong_FromLong(7), PyLong_FromLong(8));
        auto c_int_new = PyObject_GetAttrString(c_int, "new");

        PyObject *vals = PyObject_Call(c_int_new, tuple, nullptr);
        ASSERT_NE(vals, nullptr);
        PyObject_SetAttrString(obj, int_array_member_name, vals);
        vals = PyObject_GetAttrString(obj, int_array_member_name);
        ASSERT_NE(vals, nullptr);
        auto c_int_ptr_at = PyObject_GetAttrString(vals, "at");
        for (size_t i = 0; i < 3; ++i) {
            int res = PyLong_AsLong(PyObject_Call(c_int_ptr_at, PyLong_FromLong(i), nullptr));
            ASSERT_EQ(res, int_vals[i]);
        }
    }/*
    {
        int *int_vals = new int[3]{6, 7, 8};

        PyObject *vals = pyllars_internal::toPyObject<int*>(int_vals, 3);
        ASSERT_NE(vals, nullptr);
        PyObject_SetAttrString(obj, int_array_member_name, vals);
        vals = PyObject_GetAttrString(obj, int_array_member_name);
        ASSERT_NE(vals, nullptr);
        for (size_t i = 0; i < 3; ++i) {
            ASSERT_EQ((*reinterpret_cast<pyllars_internal::PythonClassWrapper<int[3]> *>(vals)->get_CObject())[i], int_vals[i]);
        }
        delete[] int_vals;
    }*/
    Py_DECREF(obj);
}

TEST_F(SetupBasicClass, TestClassEnums){
    auto PyEnumClass = PyObject_GetAttrString(pyllars::GlobalNS::module(), "EnumClass");
    ASSERT_NE(PyEnumClass, nullptr);
    ASSERT_TRUE(PyType_Check(PyEnumClass));

    PyObject* ONE_E = PyObject_GetAttrString(PyEnumClass, "E_ONE");
    ASSERT_NE(ONE_E, nullptr);
    auto * value = PyObject_GetAttrString(PyEnumClass, "value");
    auto * tuple = PyTuple_Pack(1, ONE_E);
    auto * enumval = PyObject_Call(value, tuple, nullptr);
    ASSERT_NE(enumval, nullptr);
    ASSERT_EQ(PyLong_AsLong(enumval), 1);//EnumClass::E_ONE);
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
        auto * enumval = PyObject_Call(value, args, nullptr);
        auto *enumvalue = PyObject_Call(PyEnumClass, args, nullptr);
        ASSERT_NE(enumvalue, nullptr);
        ASSERT_EQ(PyLong_AsLong(enumval), 1);//EnumClass::E_ONE);
    }
    PyObject* convert = PyObject_GetAttrString(PyEnumClass, enum_convert_name);
    ASSERT_NE(convert, nullptr);
    PyObject * args2 = PyTuple_New(1);
    PyTuple_SetItem(args2, 0, ONE_E);
    PyObject* intVal = PyObject_Call(convert, args2, nullptr);
    ASSERT_NE(intVal, nullptr);
    ASSERT_EQ(PyInt_AsLong(intVal), 1);
}

TEST_F(SetupBasicClass, TestBasicClassOffNominal) {
    PyObject* PyBasicClass = PyObject_GetAttrString(pyllars::GlobalNS::module(), "BasicClass");
    ASSERT_NE(PyBasicClass, nullptr);
    ASSERT_TRUE(PyType_Check(PyBasicClass));
    {
        PyObject *args = PyTuple_New(2);
        PyTuple_SetItem(args, 0, PyLong_FromLong(1));
        PyTuple_SetItem(args, 1, PyLong_FromLong(2));
        PyObject *obj = PyObject_Call(PyBasicClass, args, nullptr);
        ASSERT_EQ(obj, nullptr);
        ASSERT_TRUE(PyErr_Occurred());
        Py_DECREF(args);
    }
    PyErr_Clear();
    static auto empty = PyTuple_New(0);
    PyObject* kwds = PyDict_New();
    PyDict_SetItemString(kwds, "ununsed", PyLong_FromLong(33));
    PyObject* obj =  PyObject_Call(PyBasicClass, empty, kwds);
    ASSERT_EQ(obj, nullptr);
    ASSERT_TRUE(PyErr_Occurred());
    PyErr_Clear();
    Py_DECREF(empty);
}
TEST_F(SetupBasicClass, TestPointers){
    PyObject* PyBasicClass = PyObject_GetAttrString(pyllars::GlobalNS::module(), "BasicClass");
    ASSERT_NE(PyBasicClass, nullptr);
    ASSERT_TRUE(PyType_Check(PyBasicClass));
    static auto empty = PyTuple_New(0);
    PyObject* obj = PyObject_Call(PyBasicClass, empty, nullptr);
    ASSERT_NE(obj, nullptr);
    auto self = PyObject_GetAttrString(obj, "this");
    ASSERT_NE(self, nullptr);

    constexpr int MAX = 10000;
    PyObject *ptrs[MAX] = {nullptr};
    ptrs[0] = self;

    for (int i = 1; i < MAX; ++i){
        auto addr = PyObject_GetAttrString(ptrs[i-1], "this");
        ASSERT_NE(addr, nullptr);
        ptrs[i] = PyObject_Call(addr, empty, nullptr);
        ASSERT_NE(ptrs[i], nullptr);
        ASSERT_EQ((void*)*((pyllars_internal::PythonClassWrapper<BasicClass*>*)ptrs[i])->get_CObject(),
                  (void*)((pyllars_internal::PythonClassWrapper<BasicClass*>*)ptrs[i-1])->get_CObject());
    }
    auto args_index_0 = PyTuple_New(1);
    PyTuple_SetItem(args_index_0, 0, PyLong_FromLong(0));
    auto derefed = ptrs[MAX-1];
    for(int i = MAX-2; i > 0; --i){
        auto at = PyObject_GetAttrString(derefed, "at");
        ASSERT_NE(at, nullptr);
        derefed = PyObject_Call(at, args_index_0, nullptr);
        if (PyErr_Occurred()){
            PyErr_Print();
        }
        ASSERT_NE(derefed, nullptr);
        ASSERT_NE(((pyllars_internal::PythonClassWrapper<BasicClass*>*)ptrs[i])->get_CObject(), nullptr);
        ASSERT_NE(((pyllars_internal::PythonClassWrapper<BasicClass*>*) derefed)->get_CObject(), nullptr);
        ASSERT_EQ(*((pyllars_internal::PythonClassWrapper<BasicClass*>*) derefed)->get_CObject(),
                  *((pyllars_internal::PythonClassWrapper<BasicClass*>*)ptrs[i])->get_CObject());
        Py_DECREF(derefed);
        Py_DECREF(at);
    }
    Py_DECREF(args_index_0);
}


TEST_F(SetupBasicClass, TestPrivateCtrDestructor){
    static const char* const empty_list[] = {nullptr};
    PyObject* PyNonDestructible = PyObject_GetAttrString(pyllars::GlobalNS::module(), "NonDestructible");
    ASSERT_NE(PyNonDestructible, nullptr);
    ASSERT_TRUE(PyType_Check(PyNonDestructible));
    PyObject* create = PyObject_GetAttrString(PyNonDestructible, create_method_name);
    ASSERT_NE(create, nullptr);
    PyObject* instance = PyObject_Call(create, PyTuple_New(0), nullptr);
    ASSERT_NE(instance, nullptr);
    ASSERT_TRUE(PyObject_TypeCheck(instance, (PyTypeObject*) PyNonDestructible));

    PyObject* create_const = PyObject_GetAttrString(PyNonDestructible, create_const_method_name);
    ASSERT_NE(create_const, nullptr);
    PyObject* instance_const = PyObject_Call(create_const, PyTuple_New(0), nullptr);
    ASSERT_NE(instance_const, nullptr);
    auto typ = PyObject_GetAttrString(PyNonDestructible, "const");
    ASSERT_TRUE(PyType_Check(typ));
    ASSERT_TRUE(PyObject_TypeCheck(instance_const, (PyTypeObject*) typ));
}
TEST_F(SetupBasicClass, TestBasicClassUnaryOperators){
    PyObject* PyBasicClass = PyObject_GetAttrString(pyllars::GlobalNS::module(), "BasicClass");
    ASSERT_NE(PyBasicClass, nullptr);
    ASSERT_TRUE(PyType_Check(PyBasicClass));
    PyTypeObject* typ = (PyTypeObject*)PyBasicClass;
    ASSERT_NE(typ->tp_as_number->nb_positive, nullptr);
    ASSERT_NE(typ->tp_as_number->nb_negative, nullptr);
    ASSERT_NE(typ->tp_as_number->nb_invert, nullptr);
    PyObject * obj = PyObject_Call(PyBasicClass, PyTuple_New(0), nullptr);
    ASSERT_NE(obj, nullptr);
    PyObject* negate = PyObject_GetAttrString(obj, "__neg__");
    ASSERT_NE(negate, nullptr);
    PyObject* negobj = PyObject_Call(negate, PyTuple_New(0), nullptr);
    ASSERT_NE(negobj, nullptr);
    ASSERT_TRUE(PyObject_TypeCheck(negobj, typ));
    auto int_array_obj = PyObject_GetAttrString(negobj, "int_array");
    ASSERT_NE(int_array_obj, nullptr);
    auto at = PyObject_GetAttrString(int_array_obj, "at");
    ASSERT_NE(at, nullptr);
    auto val = PyObject_Call(at, PyTuple_Pack(1, PyLong_FromLong(0)), nullptr);
    ASSERT_NE(val, nullptr);
    ASSERT_EQ(PyLong_AsLong(val), -1);
    val = PyObject_Call(at, PyTuple_Pack(1, PyLong_FromLong(1)), nullptr);
    ASSERT_NE(val, nullptr);
    ASSERT_EQ(PyLong_AsLong(val), -2);
    val = PyObject_Call(at, PyTuple_Pack(1, PyLong_FromLong(2)), nullptr);
    ASSERT_NE(val, nullptr);
    ASSERT_EQ(PyLong_AsLong(val), -3);
}

TEST_F(SetupBasicClass, TestBasicClassBinaryOperators){
    PyObject* PyBasicClass = PyObject_GetAttrString(pyllars::GlobalNS::module(), "BasicClass");
    ASSERT_NE(PyBasicClass, nullptr);
    ASSERT_TRUE(PyType_Check(PyBasicClass));
    auto typ = (PyTypeObject*) PyBasicClass;
    ASSERT_NE(typ->tp_as_number->nb_add, nullptr);
    ASSERT_NE(typ->tp_as_number->nb_subtract, nullptr);

    PyObject * obj = PyObject_Call(PyBasicClass, PyTuple_New(0), nullptr);
    ASSERT_NE(obj, nullptr);
    PyObject* subtract = PyObject_GetAttrString(obj, "__sub__");
    ASSERT_NE(subtract, nullptr);
    PyObject * args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, PyFloat_FromDouble(1.1));
    PyObject* subtracted = PyObject_Call(subtract, args, nullptr);
    ASSERT_NE(subtracted, nullptr);
    ASSERT_TRUE(PyObject_TypeCheck(subtracted, typ));
    auto dbl_ptr = PyObject_GetAttrString(subtracted, dbl_ptr_member_name);
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
public testing::WithParamInterface<std::pair<pyllars::OpUnaryEnum, const BasicClass*> >{

};

TEST_P(UnaryOperatorTest, InvokesOperator){
    PyObject* PyBasicClass = PyObject_GetAttrString(pyllars::GlobalNS::module(), "BasicClass");
    ASSERT_NE(PyBasicClass, nullptr);
    ASSERT_TRUE(PyType_Check(PyBasicClass));

    static const std::map<pyllars::OpUnaryEnum, const char*> opUnaryNameMap = {
            {pyllars::OpUnaryEnum::INV, "__invert__"},
            {pyllars::OpUnaryEnum::POS, "__pos__"},
            {pyllars::OpUnaryEnum::NEG, "__neg__"}
    };

    auto  [kind, expected] = GetParam();
    PyObject * obj = PyObject_Call(PyBasicClass, PyTuple_New(0), nullptr);
    ASSERT_NE(obj, nullptr);
    const char* const name = opUnaryNameMap.at(kind);
    auto func = PyObject_GetAttrString(obj, name);
    ASSERT_NE(func, nullptr);
    auto pyobj = PyObject_Call(func, PyTuple_New(0), nullptr);
    ASSERT_NE(pyobj, nullptr);
    const BasicClass& cobj = *((pyllars_internal::PythonClassWrapper<BasicClass>*)pyobj)->get_CObject();
    ASSERT_EQ(cobj, *expected);
}

static const BasicClass val_pos = BasicClass();
static const BasicClass val_neg = -val_pos;
static const BasicClass val_inv = ~val_pos;

#ifndef INSTANTIATE_TEST_SUITE_P
#define INSTANTIATE_TEST_SUITE_P INSTANTIATE_TEST_CASE_P
#endif


INSTANTIATE_TEST_SUITE_P(UnaryOperatorTestSuite, UnaryOperatorTest, ::testing::Values(std::pair<pyllars::OpUnaryEnum, const BasicClass*>(pyllars::OpUnaryEnum::POS, &val_pos),
        std::pair<pyllars::OpUnaryEnum, const BasicClass*>(pyllars::OpUnaryEnum::NEG, &val_neg),
        std::pair<pyllars::OpUnaryEnum, const BasicClass*>(pyllars::OpUnaryEnum::INV, &val_inv)
        ));


class BinaryOperatorTest : public SetupBasicClass,
public testing::WithParamInterface<std::tuple<pyllars::OpBinaryEnum, const BasicClass*, std::function<void(PyObject*)> > >{

};

TEST_P(BinaryOperatorTest, InvokeOperator){

    static const std::map<pyllars::OpBinaryEnum, const char*> opBinaryNameMap= {
            {pyllars::OpBinaryEnum::ADD, "__add__"},
            {pyllars::OpBinaryEnum::SUB, "__sub__"},
            {pyllars::OpBinaryEnum::MUL, "__mul__"},
            {pyllars::OpBinaryEnum::DIV, "__div__"},
            {pyllars::OpBinaryEnum::AND, "__and__"},
            {pyllars::OpBinaryEnum::OR, "__or__"},
            {pyllars::OpBinaryEnum::XOR, "__xor__"},
            {pyllars::OpBinaryEnum::MOD, "__mod__"},
            {pyllars::OpBinaryEnum::LSHIFT, "__lshift__"},
            {pyllars::OpBinaryEnum::RSHIFT, "__rshift__"},

            {pyllars::OpBinaryEnum::IADD, "__iadd__"},
            {pyllars::OpBinaryEnum::ISUB, "__isub__"},
            {pyllars::OpBinaryEnum::IMUL, "__imul__"},
            {pyllars::OpBinaryEnum::IDIV, "__idiv__"},
            {pyllars::OpBinaryEnum::IAND, "__iand__"},
            {pyllars::OpBinaryEnum::IOR, "__ior__"},
            {pyllars::OpBinaryEnum::IXOR, "__ixor__"},
            {pyllars::OpBinaryEnum::IMOD, "__imod__"},
            {pyllars::OpBinaryEnum::ILSHIFT, "__ilshift__"},
            {pyllars::OpBinaryEnum::IRSHIFT, "__irshift__"}
    };
    PyObject* PyBasicClass = PyObject_GetAttrString(pyllars::GlobalNS::module(), "BasicClass");
    ASSERT_NE(PyBasicClass, nullptr);
    ASSERT_TRUE(PyType_Check(PyBasicClass));
    auto typ = (PyTypeObject*) PyBasicClass;
    auto  [kind, val1, expectation] = GetParam();
    PyObject * obj = PyObject_Call(PyBasicClass, PyTuple_New(0), nullptr);
    ASSERT_NE(obj, nullptr);
    auto func = PyObject_GetAttrString(obj, (const char* const)opBinaryNameMap.at(kind));
    ASSERT_NE(func, nullptr);
    ASSERT_TRUE(PyCallable_Check(func));
    auto args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, (PyObject*)pyllars_internal::PythonClassWrapper<const BasicClass&>::fromCObject(*val1));
    auto pyobj = PyObject_Call(func, args, nullptr);
    ASSERT_NE(pyobj, nullptr);
    expectation(pyobj);
}

static const BasicClass val_add = val_pos;
static const BasicClass expected_add = BasicClass(val_pos + val_pos);
static std::function<void(PyObject*)> expectation_add = [](PyObject* result){
    ASSERT_TRUE(PyObject_TypeCheck(result, pyllars_internal::PythonClassWrapper<double>::getPyType()));
    ASSERT_DOUBLE_EQ(*reinterpret_cast<pyllars_internal::PythonClassWrapper<double>*>(result)->get_CObject(), *expected_add.double_ptr_member);
};

INSTANTIATE_TEST_SUITE_P(BinaryOperatorTestSuite, BinaryOperatorTest, ::testing::Values(
        std::tuple<pyllars::OpBinaryEnum, const BasicClass*, std::function<void(PyObject*)>>(pyllars::OpBinaryEnum::ADD, &val_pos, expectation_add)
));
