#include "pyllars_pointer.cpp"

PyObject* __pyllars_internal::PtrWrapperBaseBase::parent_module = nullptr;

namespace __pyllars_internal{
    template<typename T,  bool is_array, const ssize_t array_size, typename std::enable_if<std::is_assignable<T, T>::value>::type>
    PyObject *set_array_values(T *values, const ssize_t size, PyObject *from, PyObject *referenced) {
        if (!PyList_Check(referenced) || PyList_Size(referenced) != size) {
            PyErr_SetString(PyExc_RuntimeError, "Internal error setting array elements");
        }
        if (PyList_Check(from)) {

            //have python list to set from
            if (PyList_Size(from) != size) {
                PyErr_SetString(PyExc_TypeError, "Invalid array size");
                return nullptr;
            }
            for (int i = 0; i < size; ++i) {
                values[i] = *toCObject<T, is_array, true>(PyList_GetItem(from, i));
            }
        } else if (PythonClassWrapper<T *>::checkType(from)) {
            //TODO: check array size on from????
            for (int i = 0; i < size; ++i) {
                values[i] = ((T *) ((PythonClassWrapper < T * > *)
                from)->_all_content._untyped_content)[i];
            }
        } else if (PythonClassWrapper<T[]>::checkType(from)) {
            //TODO: check array size on from????
            for (int i = 0; i < size; ++i) {
                values[i] = ((T *) ((PythonClassWrapper < T[] > *)
                from)->_all_content._untyped_content)[i];
            }
        } else if (array_size > 0 && size > 0 &&
                PythonClassWrapper<T[array_size]>::checkType(from)) {
            if (array_size != size) {
                PyErr_SetString(PyExc_RuntimeError, "Mistmached array lengths");
                return nullptr;
            }
            for (int i = 0; i < size; ++i) {
                values[i] = ((T *) ((PythonClassWrapper < T[array_size]> *)
                from)->_all_content._untyped_content)[i];
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid argument type");
            return nullptr;
        }
        return Py_None;
    }

    template<>
    PyObject *set_array_values<const char **, false, -1, void>(const char **values, const ssize_t size, PyObject *from,
                                                               PyObject *referenced) {
        if (!PyList_Check(referenced) || PyList_Size(referenced) != size) {
            PyErr_SetString(PyExc_RuntimeError, "Internal error setting array elements");
        }
        if (PyList_Check(from)) {

            //have python list to set from
            if (PyList_Size(from) != size) {
                PyErr_SetString(PyExc_TypeError, "Invalid array size");
                return nullptr;
            }
            for (int i = 0; i < size; ++i) {
                PyObject *item = PyList_GetItem(from, i);
                if (!PyString_Check(item)) {
                    return nullptr;
                }
                char *asstr = PyString_AsString(item);

                if (!asstr) {
                    PyErr_SetString(PyExc_TypeError, "Not a string on array elements assignment");
                    return nullptr;
                } else {
                    //make copy and keep reference
                    PyObject *pystr = PyString_FromString(asstr);
                    PyList_SetItem(referenced, i, pystr);
                    values[i] = (const char *const) PyString_AsString(pystr);
                }
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid argument type");
            return nullptr;
        }
        return Py_None;
    }
}