#ifndef __PYLLARS__TEMPLATES__H_
#define __PYLLARS__TEMPLATES__H_

#include <map>
#include <vector>
#include <Python.h>

namespace __pyllars_internal{

    struct PythonClassTemplate{
        PyObject_HEAD

        template<const char* const name, const char* const short_name>
        static PythonClassTemplate* get(PyObject* for_module){
                static std::map<std::string, PythonClassTemplate *> map;
                const std::string std_name = std::string(name);
                if (!map.count(std_name)) {
                    create(name, short_name, for_module, map);
                }
                return map[std_name];
        }

        int addParameterSet( PyObject* set[]);

    private:

        static
        bool _compare(PyObject** set, PyObject* tuple);

        static
        PyObject* _call(PyObject *callable_object,
                        PyObject *args,
                        PyObject *kw) ;

        static
        PyObject* _new(PyTypeObject *type, PyObject *args, PyObject *kwds) ;

        static
        int _init(PyObject *self, PyObject *args, PyObject *kwds);

        static void create(const char* const name, const char* const short_name, PyObject * const module,
                std::map<std::string, PythonClassTemplate*> & map);

        std::vector<PyObject**>* _parameterSets;
    };

}
#endif