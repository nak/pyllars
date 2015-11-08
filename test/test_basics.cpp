#include <pyllars/pyllars_pointer.h>
#include <pyllars/pyllars_function_wrapper.h>

typedef const char* cstring;

using namespace __pyllars_internal;

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif


class Dummy{
};


PyMODINIT_FUNC
initmod() {

  PyObject* m = Py_InitModule3("test", nullptr,
			       "Test of pyllars generation");

  init_pyllars_pointer< int>( m );
  init_pyllars_pointer< double>( m );
  init_pyllars_pointer< Dummy>( m );
  init_pyllars_pointer< Dummy&>( m );
  init_pyllars_pointer< Dummy*>( m );
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
PyMethodDef PythonClassWrapper<Dummy>::_methods[] = {};
template<>
PyMethodDef PythonClassWrapper<Dummy&>::_methods[] = {};
template<>
PyMethodDef PythonClassWrapper<Dummy*>::_methods[] = {};

template<>
PyMemberDef PythonClassWrapper<Dummy>::_members[] = {};
template<>
PyMemberDef PythonClassWrapper<Dummy&>::_members[] = {};
template<>
PyMemberDef PythonClassWrapper<Dummy*>::_members[] = {};


template<>
const char* const PythonCPointerWrapper<Dummy>::name = "ptrtype_Dummy";
template<>
const char* const PythonCPointerWrapper<Dummy&>::name = "ptrtype_DummyRef";
template<>
const char* const PythonCPointerWrapper<Dummy*>::name = "ptrtype_DummyPtr";


int main(){
  Py_Initialize();
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
  PyObject* o = PyObject_CallObject((PyObject*)&PythonCPointerWrapper<int>::Type, pArgs);
  if (o == nullptr){
    printf("nullptr O\n");
  }

  Dummy dumm1, dumm2;
  auto wrapper = PythonFunctionWrapper< int16_t, int, double, Dummy&, Dummy*>::create(dumm);
  wrapper->call(123, 3.21, dumm1, &dumm2);
  try{
      PyObject* args = PyTuple_New(4);
      PyTuple_SetItem(args, 0, PyInt_FromLong(123));
      PyTuple_SetItem(args, 1, PyFloat_FromDouble(3.21));
      PyTuple_SetItem(args, 2, toPyObject<Dummy&>(dumm1));
      PyTuple_SetItem(args, 3, toPyObject<Dummy*>(&dumm2));
      toCObject<int16_t>(*PyObject_CallObject( (PyObject*)wrapper, args));
  } catch(...){
      printf("Error caught on function call");
  }
  return 0;
}
