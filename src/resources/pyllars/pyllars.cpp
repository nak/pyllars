#include "pyllars.hpp"

#include "pyllars_varargs.cpp"
#include "pyllars_templates.cpp"
pyllars::Initializer *pyllars::Initializer::root = nullptr;


extern "C"{
    #if PY_MAJOR_VERSION == 3
    PyMODINIT_FUNC
    PyInit_pyllars(void){
        static const char* const name = "pyllars";
        static const char* const doc = "Pyllars top-level module";
        static PyModuleDef moduleDef;
        memset(&moduleDef, 0, sizeof(moduleDef));
        moduleDef.m_name = name;
        moduleDef.m_doc = doc;
        moduleDef.m_size = -1;
        return PyModule_Create(&moduleDef);
    }
    #else
    int _initpyllars(){
        PyObject *pyllars_mod = Py_InitModule3("pyllars", nullptr, "Pyllars top-level module");
        if(!pyllars_mod) { return -1;}
        return pyllars::Initializer::root?pyllars::Initializer::root->init():0;
    }
    #endif
}

int pyllars::pyllars_register( Initializer* const init){
    // ensure root is "clean" and no static initizlied as this function
    // may be called during static initialization before root has been assigend
    // a static value
    static Initializer _root;
    Initializer::root = &_root;

    return Initializer::root->register_init(init);
}

