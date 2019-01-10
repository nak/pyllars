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
        PyObject *createGlobalVariable(const char *const name, const char *const tp_name,
                                       //typename extent_as_pointer<T>::type *variable,
                                       T *variable,
                                       PyObject *module,
                                       const size_t size);

        template<typename T, typename E= void>
        class Container;

        template<typename T>
        class Container<T, typename std::enable_if<
                !std::is_array<T>::value && !std::is_const<T>::value>::type> {
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

        template<size_t size, typename T>
        class Container<T[size]> {
        public:
            typedef typename std::remove_reference<T>::type T_NoRef;
            PyObject_HEAD

            static PyTypeObject
                    Type;
            //typedef T *member_t[size];
            typedef T val_t[size];
            typedef val_t *member_t;

            member_t member;
            size_t array_size;

            static int _init(PyObject *self, PyObject *args, PyObject *kwds);

            static PyObject *
            _new(PyTypeObject *type, PyObject *args, PyObject *kwds);

            static PyObject *call(Container *callable, PyObject *args, PyObject *kwds);

            static void setFromPyObject(Container *callable, PyObject *pyobj);
        };

        template<typename T>
        class Container<T[], typename std::enable_if<!std::is_const<T>::value>::type> {
        public:
            typedef typename std::remove_reference<T>::type T_NoRef;
            PyObject_HEAD

            static PyTypeObject
                    Type;

            typedef T **member_t;
            member_t member;
            size_t array_size;

            static int _init(PyObject *self, PyObject *args, PyObject *kwds);

            static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

            static PyObject *call(Container *callable, PyObject *args, PyObject *kwds);

        };

        template<typename T>
        class Container<const T, typename std::enable_if<!std::is_array<T>::value>::type> {
        public:
            typedef typename std::remove_reference<T>::type T_NoRef;
            typedef const T *member_t;

            PyObject_HEAD

            static PyTypeObject
                    Type;
            member_t member;
            size_t array_size;

            static int _init(PyObject *self, PyObject *args, PyObject *kwds);

            static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

            static PyObject *call(Container *callable, PyObject *args, PyObject *kwds);

        };

        template<size_t size, typename T>
        class Container<const T[size], void> {
        public:
            typedef typename std::remove_reference<T>::type T_NoRef;
            PyObject_HEAD

            static PyTypeObject Type;
            typedef const T **member_t;

            member_t member;
            size_t array_size;

            static int _init(PyObject *self, PyObject *args, PyObject *kwds);

            static PyObject *
            _new(PyTypeObject *type, PyObject *args, PyObject *kwds);

            static PyObject *call(Container *callable, PyObject *args, PyObject *kwds);

        };

        template<typename T>
        class Container<const T[], void> {
        public:
            typedef typename std::remove_reference<T>::type T_NoRef;
            PyObject_HEAD

            static PyTypeObject
                    Type;
            typedef const T **member_t;

            member_t member;
            size_t array_size;

            static int _init(PyObject *self, PyObject *args, PyObject *kwds);

            static PyObject *
            _new(PyTypeObject *type, PyObject *args, PyObject *kwds);

            static PyObject *call(Container *callable, PyObject *args, PyObject *kwds);
        };

    protected:
    private:
    };


}
#endif // GLOBALMEMERCONTAINER_H
