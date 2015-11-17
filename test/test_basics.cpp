#include <pyllars/pyllars_pointer.h>
#include <pyllars/pyllars_function_wrapper.h>
#include <pyllars/pyllars_classwrapper.h>

typedef const char* cstring;

using namespace __pyllars_internal;

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif


class Dummy{
public:
   void print(){
     fprintf (stderr, "YOU'RE IN LUCK!!!\n");
   }
   const double value = 4.242;
};


PyMODINIT_FUNC
initmod() {

  PyObject* m = Py_InitModule3("test", nullptr,
			       "Test of pyllars generation");

  init_pyllars_pointer< int>("int", m );
  init_pyllars_pointer< double>("double", m );
  init_pyllars_pointer< Dummy>( "Dummy", m );
//init_pyllars_pointer< Dummy&>( m );
//  init_pyllars_pointer< Dummy*>(m );
}

int16_t dumm( const int a, const double f, Dummy & dummy, Dummy* dummy2){
    fprintf(stdout,"Called dummy with %d, %f, %p %p", a, f, &dummy,dummy2);
    return 42;
}

template<>
const char* const PythonClassWrapper<Dummy>::name = "type_Dummy";
template<>
const char* const PythonClassWrapper<Dummy&>::name = "type_DummyRef";
template<>
const char* const PythonClassWrapper<Dummy*>::name = "type_DummyPtr";


template<>
const char* const PythonCPointerWrapper<Dummy>::name = "ptrtype_Dummy";
//template<>
//const char* const PythonCPointerWrapper<Dummy*>::name = "ptrtype_DummyPtr";
extern const char* const names []  = {"intv", "doublev", "dumm1", "dumm2",nullptr};
extern const char  funcname[] = "dummy_func";
extern const char print_name[] = "print";
extern const char member_name[] = "value";
int main(){
    Py_Initialize();
    toPyObject<int>(1);
    PythonClassWrapper<Dummy>::add_method<print_name,void>( &Dummy::print);
    PythonClassWrapper<Dummy>::add_member<member_name,const double>( &Dummy::value);
    initmod();
    auto tyobj = &PythonClassWrapper<int>::Type;
    auto pobjArgs = PyTuple_New(1);
    auto intObj = PyLong_FromLong(0);
    PyTuple_SetItem(pobjArgs, 0, intObj);
    auto obj = PyObject_CallObject( (PyObject*)tyobj, pobjArgs);
    if (!obj){
        return -1;
    }
    auto pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, obj);
    PythonClassWrapper<int>::addType("Pointer", &PythonCPointerWrapper<int>::Type);
    PyObject* o = PyObject_CallObject((PyObject*)&PythonCPointerWrapper<int>::Type, nullptr);
    if (o == nullptr){
        printf("nullptr O\n");
    }

    Dummy dumm1, dumm2;
    auto wrapper = PythonFunctionWrapper< funcname, names, int16_t, int, double, Dummy&, Dummy*>::create(dumm);
    wrapper->call(123, 3.21, dumm1, &dumm2);
    try{
        PyObject* args = PyTuple_New(4);
        PyTuple_SetItem(args, 0, PyInt_FromLong(123));
        PyTuple_SetItem(args, 1, PyFloat_FromDouble(3.21));
        PyTuple_SetItem(args, 2, toPyObject<Dummy>(dumm1));
        PyObject* dumm2_ptr = toPyObject<Dummy*>(&dumm2);
        assert( PyObject_TypeCheck(dumm2_ptr, &PythonCPointerWrapper<Dummy>::Type));
        PyTuple_SetItem(args, 3, dumm2_ptr);
        int16_t val = toCObject<int16_t>(*PyObject_CallObject( (PyObject*)wrapper, args));
        fprintf(stderr, "VALUE IS %d\n", val);
    } catch(...){
        fprintf(stderr, "Error caught on function call\n");
        return 1;
    }
    {
        Dummy obj;
        PyObject* pyobj = toPyObject(obj);
        PyObject* ret = PyObject_CallMethod(pyobj, (char*)print_name,nullptr);
        ret = PyObject_CallMethod(pyobj, (char*)(std::string("get_")+member_name).c_str(),nullptr);
        double val = toCObject<double>(*ret);
        fprintf(stderr, ".value is: %f" , val);
    }
    return 0;
}
