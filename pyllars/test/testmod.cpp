#include "pyllars.hpp"
#include "pyllars/test_pyllars/module.hpp"


static PyMethodDef methods[] = {
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
inittest_pyllars(void)
{
  PyObject* mod = Py_InitModule("test_pyllars", methods);
  pyllars::test_pyllars::init();
  pyllars::Initializer::init();  
}
