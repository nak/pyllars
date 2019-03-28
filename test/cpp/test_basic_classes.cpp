//
// Created by jrusnak on 3/24/19.
//

#include "setup_basic_class.h"
#include "class_test_defns.h"
#include "pyllars/pyllars.hpp"

TEST_F(SetupBasicClass, TestBasicClassNew) {
    using namespace __pyllars_internal;
    PyObject *args = PyTuple_New(0);

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
    static const char* const kwlist[] = {"value", nullptr};
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
    ASSERT_EQ(int_value, nullptr);
    PyErr_Clear();

    PyTuple_SetItem(at_args, 0, PyLong_FromLong(25));
    PyObject* kwds = PyDict_New();
    PythonClassWrapper<int>* new_value = (PythonClassWrapper<int>*) PyObject_Call((PyObject*)PythonClassWrapper<int>::getPyType(), at_args, nullptr);
    ASSERT_FALSE(PyErr_Occurred());
    ASSERT_NE(new_value, nullptr);
    PyTuple_SetItem(at_args, 0, PyLong_FromLong(1));
    PyDict_SetItemString(kwds, "set_value", (PyObject*) new_value);
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
        PyObject *vals = toPyObject(int_vals, 3);
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

        PyObject *vals = toPyObject(int_vals, 3);
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
    PyObject *args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, ONE_E);
    Class* new_value = (Class*) PyObject_Call((PyObject*) Class::getPyType(), args, nullptr);
    ASSERT_NE((new_value->get_CObject()), nullptr);
    ASSERT_EQ(*(new_value->get_CObject()), EnumClass::E_ONE);

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
    static const char *const kwlist[] = {"value", nullptr};
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

    PyObject* kwds = PyDict_New();
    PyDict_SetItemString(kwds, "ununsed", PyLong_FromLong(33));
    obj =  PyObject_Call((PyObject *) PythonClassWrapper<BasicClass>::getPyType(),
                         nullptr, kwds);
    ASSERT_EQ(obj, nullptr);
    ASSERT_TRUE(PyErr_Occurred());
    PyErr_Clear();
}

TEST_F(SetupBasicClass, TestPointers){
    using namespace __pyllars_internal;
    typedef PythonClassWrapper<BasicClass> Class;
    PyObject* obj = PyObject_Call((PyObject*) Class::getPyType(), PyTuple_New(0), nullptr);
    auto self = PyObject_GetAttrString(obj, "this");
    ASSERT_NE(self, nullptr);

    constexpr int MAX = 1000;
    PyObject *ptrs[MAX] = {nullptr};
    PyObject* empty = PyTuple_New(0);
    ptrs[0] = self;

    for (int i = 1; i < MAX; ++i){
        auto addr = PyObject_GetAttrString(ptrs[i-1], "this");
        ASSERT_NE(addr, nullptr);
        ptrs[i] = PyObject_Call(addr, empty, nullptr);
        ASSERT_NE(ptrs[i], nullptr);
        ASSERT_EQ((void*)*((PythonClassWrapper<BasicClass*>*)ptrs[i])->get_CObject(),
                  (void*)((PythonClassWrapper<BasicClass*>*)ptrs[i-1])->get_CObject());
    }
    auto args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, PyLong_FromLong(0));
    auto derefed = ptrs[MAX-1];
    for(int i = MAX-2; i > 0; --i){
        auto at = PyObject_GetAttrString(derefed, "at");
        ASSERT_NE(at, nullptr);
        derefed = PyObject_Call(at, args, nullptr);
        ASSERT_NE(derefed, nullptr);
        ASSERT_NE(((PythonClassWrapper<BasicClass*>*) derefed)->get_CObject(), nullptr);
        ASSERT_EQ(((PythonClassWrapper<BasicClass*>*) derefed)->get_CObject(),
                  ((PythonClassWrapper<BasicClass*>*)ptrs[i])->get_CObject());
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
