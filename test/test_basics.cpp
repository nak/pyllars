#include <pyllars/pyllars_pointer.h>
#include <pyllars/pyllars_function_wrapper.h>
#include <pyllars/pyllars_classwrapper.h>
#include <pyllars/pyllars_conversions.h>
#include <pyllars/pyllars_callbacks.h>

typedef const char* cstring;

using namespace __pyllars_internal;

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif


typedef const char* (*callback_t)(double);

class TestClass{
public:

   TestClass(){}
   TestClass(callback_t cb){ fprintf(stderr, "Test Class Created with Callback\n Message from CB IS:  %s\n",cb(1.2345));}

   void print(){
     fprintf (stderr, "YOU'RE IN LUCK!!!\n");
   }
   double value = 4.242;
private:
    TestClass(const TestClass &);
};


class TestClassCopiable{
public:

   TestClassCopiable(){}
   TestClassCopiable(callback_t cb){ fprintf(stderr, "Test Class Created with Callback\n Message from CB IS:  %s\n",cb(1.2345));}

   void print(){
     fprintf (stderr, "YOU'RE IN LUCK!!!\n");
   }
   double value = 4.242;
private:

};

int16_t testFunction( const int a, const double f,  int& intval, TestClass & dummy, TestClass* dummy2, callback_t cb){
    fprintf(stdout,"Called dummy with %d, %f, %p %p\n", a, f, (void*)&dummy, (void*)dummy2);
    intval = 1.234;
    dummy.value = 424242.42;
    fprintf(stdout, "Callbcack msg: %s", cb(dummy.value));
    return 42;
}

const char* message_me(double val){
    static char msg[1024];
    snprintf(msg, 1024, "LET IT SNOW! [%f]\n", val);
    return msg;
}


extern const char* const names []  = {"intv", "doublev", "intref", "dumm1", "dumm2","msg_callback",nullptr};
extern const char* const names2 []  = {"value",nullptr};
extern const char  funcname[] = "dummy_func";
extern const char  funcname2[] = "message_me";
extern const char print_name[] = "print_me";
extern const char member_name[] = "value";
extern const char* const cb_name[] = {"cb",  nullptr};
extern const char* const copy_constructor_name[] = {"from",  nullptr};

extern const char* const address_name[] = {"addr",  nullptr};

static PyObject* wrapper ;


//function to initialize the Pyllars objects we need
PyMODINIT_FUNC
initmod() {

  PyObject* m = Py_InitModule3("test", nullptr,
			       "Test of pyllars generation");

  //PythonClassWrapper< int>::initialize("int", m );
  PythonClassWrapper< int&>::initialize("int_ref", m );
  PythonClassWrapper< TestClass&>::initialize("TestClass_ref", m );
  PythonClassWrapper< TestClassCopiable&>::initialize("TestClassCopiable_ref", m );
  PythonClassWrapper< double>::initialize("double", m );
  PythonClassWrapper< TestClass>::initialize( "TestClass", m );
  PythonClassWrapper< TestClassCopiable>::initialize( "TestClassCopiable", m );
  wrapper = (PyObject*)PythonFunctionWrapper< funcname, names, int16_t, int, double, int&, TestClass&, TestClass*, callback_t>::create(testFunction);
  PyModule_AddObject(m, "testFunction", (PyObject*)wrapper);
}
#ifndef MAIN
PyMODINIT_FUNC
inittest()
#define SUCCESS
#define FAIL
#else
int main()
#define SUCCESS 0
#define FAIL 1
#endif
{
    Py_Initialize();
    toPyObject<int>(1, false);
    PythonClassWrapper<TestClass>::addConstructor( PythonClassWrapper<TestClass>::create<nullptr> );
    PythonClassWrapper<TestClass>::addConstructor( PythonClassWrapper<TestClass>::create<cb_name, callback_t> );
    PythonClassWrapper<TestClassCopiable>::addConstructor( PythonClassWrapper<TestClassCopiable>::create<nullptr> );
    PythonClassWrapper<TestClassCopiable>::addConstructor( PythonClassWrapper<TestClassCopiable>::create<cb_name, callback_t> );
    PythonClassWrapper<TestClassCopiable&>::addConstructor( PythonClassWrapper<TestClassCopiable&>::create<copy_constructor_name, TestClassCopiable> );
    PythonClassWrapper<TestClassCopiable>::addConstructor( PythonClassWrapper<TestClassCopiable>::create<copy_constructor_name, TestClassCopiable> );
    PythonClassWrapper<TestClass>::addMethod<print_name,void>( &TestClass::print);
    PythonClassWrapper<TestClass>::addMember<member_name,const double>( &TestClass::value);
    initmod();
#ifdef MAIN
    //just test code:
    {
        TestClassCopiable testObj;
        PyObject* args = PyTuple_New(1);
        PyTuple_SetItem(args, 0, PyInt_FromLong(123));

        PyObject_CallObject((PyObject*)&PythonClassWrapper< int&>::Type, args) ;
        PyTuple_SetItem(args, 0, toPyObject<TestClassCopiable>(testObj, false));
        PyObject_CallObject((PyObject*)&PythonClassWrapper< TestClassCopiable&>::Type, args) ;
    }
    auto tyobj = &PythonClassWrapper<int>::Type;
    auto pobjArgs = PyTuple_New(1);
    auto intObj = PyLong_FromLong(0);
    PyTuple_SetItem(pobjArgs, 0, intObj);
    auto obj = PyObject_CallObject( (PyObject*)tyobj, pobjArgs);
    if (!obj){
        return FAIL;
    }
    auto pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, obj);
    PythonClassWrapper<int>::addType("Pointer", &PythonCPointerWrapper<int>::Type);
    PyObject* o = PyObject_CallObject((PyObject*)&PythonCPointerWrapper<int>::Type, nullptr);
    if (o == nullptr){
        printf("nullptr O\n");
    }
    TestClass dumm1, dumm2;
     auto message_me_py = PythonFunctionWrapper<funcname2, names2, const char*, double>::create(message_me);
    PyObject *arg = PyTuple_New(1);
    PyTuple_SetItem(arg, 0, (PyObject*)message_me_py);
    PyObject_CallObject((PyObject*)&PythonClassWrapper<TestClass>::Type, arg);
    try{
        int intval = 0;
        //PyObject_Call((PyObject*)message_me_py, nullptr, nullptr);
        PyObject* args = PyTuple_New(6);
        PyTuple_SetItem(args, 0, PyInt_FromLong(123));
        PyTuple_SetItem(args, 1, PyFloat_FromDouble(3.21));
        PyTuple_SetItem(args, 2, toPyObject<int&>(intval, true));
        PyTuple_SetItem(args, 3, toPyObject<TestClass>(dumm1, true));
        PyObject* dumm2_ptr = toPyObject<TestClass*>(&dumm2, true);
        assert( PyObject_TypeCheck(dumm2_ptr, &PythonCPointerWrapper<TestClass>::Type));
        assert( dumm2_ptr != Py_None);
        PyTuple_SetItem(args, 4, dumm2_ptr);
        PyTuple_SetItem(args, 5, (PyObject*)message_me_py);
        for (int i = 0; i < 6; ++i){
            fprintf(stderr, "\n");
            //PyObject_Print( PyTuple_GetItem(args, i), stderr, 0);
            assert( PyTuple_GetItem(args,i) != Py_None);
        }
        int16_t val = toCObject<int16_t>(*PyObject_CallObject( (PyObject*)wrapper, args));
        fprintf(stderr, "VALUE IS %d\n", val);
        fprintf(stderr, "NEW INTEGRAL VALUE IS %d\n", intval);
        fprintf(stderr, "NEW DUMMY VALUE IS %f\n", dumm1.value);
    } catch(...){
        fprintf(stderr, "Error caught on function call\n");
        return FAIL;
    }
    {
        TestClass obj;
        PyObject* pyobj = toPyObject(obj, true);
        PyObject* ret = PyObject_CallMethod(pyobj, (char*)print_name,nullptr);
        ret = PyObject_CallMethod(pyobj, (char*)(std::string("get_")+member_name).c_str(),nullptr);
        double val = toCObject<double>(*ret);
        fprintf(stderr, ".value is: %f\n" , val);
    }
    fprintf(stderr, "SUCCESS!\n");
    PyErr_Clear();
    return SUCCESS;
#endif
}
