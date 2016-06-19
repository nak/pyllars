#include "pyllars.hpp"
#include "pyllars/test/module.hpp"


static PyMethodDef methods[] = {
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
inittest(void)
{
  PyObject* mod = Py_InitModule("test", methods);
  pyllars::test::init();
  pyllars::Initializer::init();  
}
