#ifndef GLOBALMEMERCONTAINER_H
#define GLOBALMEMERCONTAINER_H

#include <Python.h>

#include "pyllars_defns.hpp"
#include "pyllars.hpp"
#include "pyllars_classwrapper.hpp"

namespace __pyllars_internal {


    struct GlobalVariable {
        template<typename T>
        static
        PyObject *createGlobalVariable(const char *const name,
                                       T *variable,
                                       PyObject *module,
                                       const size_t size);


        template<typename T>
        class Container{
        public:
            typedef typename std::remove_reference<T>::type T_NoRef;
            typedef T *member_t;

            PyObject_HEAD

            static PyTypeObject Type;
            member_t member;
            size_t array_size;

            static int _init(PyObject *self, PyObject *args, PyObject *kwds);

            static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

            static PyObject *call(Container *callable, PyObject *args, PyObject *kwds);

        };

    protected:
    private:
    };


}
#endif // GLOBALMEMERCONTAINER_H
