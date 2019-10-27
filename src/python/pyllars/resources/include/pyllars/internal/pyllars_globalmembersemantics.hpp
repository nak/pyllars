#ifndef GLOBALMEMERCONTAINER_H
#define GLOBALMEMERCONTAINER_H

#include <Python.h>

#include "pyllars/internal/pyllars_defns.hpp"
#include "pyllars.hpp"
#include "pyllars/internal/pyllars_classwrapper.hpp"

namespace __pyllars_internal {


    /**
     * struct to hold a global variable, mapping to Python semantics
     */
    struct GlobalVariable {
        template<typename T>
        static
        PyObject *createGlobalVariable(const char *const name,
                                       T *variable,
                                       PyObject *module,
                                       const size_t size);

    protected:

        /**
         * Container for the variable, which is a Python Type
         * @tparam T: the type of variable to contain
         */
        template<typename T>
        class Container{
        public:
            
            typedef typename std::remove_reference<T>::type T_NoRef;
            typedef T *member_t;

            PyObject_HEAD

            static PyTypeObject Type;

            
            static int _init(PyObject *self, PyObject *args, PyObject *kwds);

            static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

            static PyObject *call(Container *callable, PyObject *args, PyObject *kwds);

            member_t member;
            size_t array_size;
        };

    private:
    };


}
#endif // GLOBALMEMERCONTAINER_H
