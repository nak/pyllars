//  PREDECLARATIONS

#ifndef __PYLLARS__INTERNAL__CLASS_WRAPPER_H
#define __PYLLARS__INTERNAL__CLASS_WRAPPER_H

#include <Python.h>
#include <structmember.h>
#include <limits.h>
#include <limits>
#include <vector>
#include <map>
#include <functional>
#include <Python.h>
#include <tupleobject.h>


#include "pyllars_utils.hpp"
#include "pyllars_defns.hpp"
#include "pyllars_classmethodsemantics.hpp"
#include "pyllars_constmethodcallsemantics.hpp"
#include "pyllars_methodcallsemantics.hpp"
#include "pyllars_object_lifecycle.hpp"
#include "pyllars_conversions.hpp"

namespace __pyllars_internal {

    namespace {
        //basic constants (names and such)
        extern const char address_name[] = "PYL_addr";
        extern const char alloc_name[] = "PYL_alloc";
    }


    static PyMethodDef emptyMethods[] = {{nullptr, nullptr, 0, nullptr}};

    template<typename T, typename E=void>
    class InitHelper {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds);
    };

    /**
     * Specialization for integers
     **/
    template<typename T>
    class InitHelper<T, typename std::enable_if<std::is_integral<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
            if (!self) {
                return -1;
            }
            static const char *kwlist[] = {"value", nullptr};
            if (std::is_unsigned<T>::value) {
                unsigned long long value;
                if (!PyArg_ParseTupleAndKeywords(args, kwds, "L", (char **) kwlist,
                                                 &value))
                    return -1;
                if (value < (unsigned long long) std::numeric_limits<T>::min() ||
                    value > (unsigned long long) std::numeric_limits<T>::max()) {
                    PyErr_SetString(PyExc_OverflowError, "Integer value out of range of int");
                    return -1;
                }
                self->_CObject = new T((T) value);
            } else {
                long long value;
                fprintf(stderr, "%ld", PyLong_AsLong(PyTuple_GetItem(args, 0)));
                if (!PyArg_ParseTupleAndKeywords(args, kwds, "K", (char **) kwlist,
                                                 &value))
                    return -1;
                if (value < (long long) std::numeric_limits<T>::min() ||
                    value > (long long) std::numeric_limits<T>::max()) {
                    PyErr_SetString(PyExc_OverflowError, "Integer value out of range of int");
                    return -1;
                }
                self->_CObject = new T((T) value);
            }
            return 0;
        }
    };

    /**
     * Specialization for floating point
     **/
    template<typename T>
    class InitHelper<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
            static const char *kwlist[] = {"value", nullptr};
            if (!self) {
                return -1;
            }

            double value;
            if (!PyArg_ParseTupleAndKeywords(args, kwds, "d", (char **) kwlist,
                                             &value))
                return -1;
            if (value < std::numeric_limits<T>::min() || value > std::numeric_limits<T>::max()) {
                PyErr_SetString(PyExc_OverflowError, "Integer value out of range of int");
                return -1;
            }
            self->_CObject = new T((T) value);


            return 0;
        }
    };

    //specialize for non-numeric fundamental types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            !std::is_void<T>::value && !std::is_arithmetic<T>::value && std::is_fundamental<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
            if (!self) {
                return -1;
            }
            for (auto it = PythonClassWrapper<T>::_constructors.begin();
                 it != PythonClassWrapper<T>::_constructors.end(); ++it) {
                try {
                    if ((*it)(args, kwds, self->_CObject)) {
                        break;
                    }
                } catch (...) {
                }
                PyErr_Clear();
            }
            if (self->_CObject == nullptr) {
                static const char *kwdlist[] = {"value", nullptr};
                PyObject *pyobj;
                if (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char **) kwdlist, pyobj)) {
                    if (!PythonClassWrapper<T>::checkType(pyobj)) {
                        PyErr_Print();
                        PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                        return -1;
                    }
                    self->_CObject = new typename std::remove_reference<T>::type >
                                     (*(reinterpret_cast<PythonClassWrapper<T> *>(pyobj)->template get_CObject<T>()));
                } else if ((!kwds || PyDict_Size(kwds) == 0) && (!args || PyTuple_Size(args) == 0)) {
                    self->_CObject = new typename std::remove_reference<T>::type();
                    memset(self->_CObject, 0, sizeof(typename std::remove_reference<T>::type));
                }
            } else if ((!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
                if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
                    PyErr_Clear();
                } else {
                    PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                    return -1;
                }
            }
            if (self->_CObject == nullptr) {
                PyErr_SetString(PyExc_TypeError, "Invalid argment(s) to constructor");
                return -1;
            }
            return 0;
        }
    };

    //specialize for non-numeric fundamental types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<std::is_void<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
            if (!self) {
                return -1;
            }
            for (auto it = PythonClassWrapper<T>::_constructors.begin();
                 it != PythonClassWrapper<T>::_constructors.end(); ++it) {
                try {
                    if ((*it)(args, kwds, self->_CObject)) {
                        break;
                    }
                } catch (...) {
                }
                PyErr_Clear();
            }
            if (self->_CObject == nullptr) {
                static const char *kwdlist[] = {"value", nullptr};
                PyObject *pyobj = Py_None;
                if (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char **) kwdlist, pyobj)) {
                    if (!PythonClassWrapper<T>::checkType(pyobj)) {
                        PyErr_Print();
                        PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                        return -1;
                    }
                    self->_CObject = nullptr;
                } else if ((!kwds || PyDict_Size(kwds) == 0) && (!args || PyTuple_Size(args) == 0)) {
                    self->_CObject = nullptr;

                }
            } else if ((!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
                if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
                    PyErr_Clear();
                } else {
                    PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                    return -1;
                }
            }
            if (self->_CObject == nullptr) {
                PyErr_SetString(PyExc_TypeError, "Invalid argment(s) to constructor");
                return -1;
            }
            return 0;
        }
    };


    //specialize for pointer types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<!std::is_void<T>::value && !std::is_arithmetic<T>::value &&
                                                             std::is_pointer<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
            if (!self) {
                return -1;
            }
            self->_CObject = nullptr;
            for (auto it = PythonClassWrapper<T>::_constructors.begin();
                 it != PythonClassWrapper<T>::_constructors.end(); ++it) {
                try {
                    if ((*it)(args, kwds, self->_CObject)) {
                        break;
                    }
                } catch (...) {
                }
                PyErr_Clear();
            }

            if (self->_CObject == nullptr) {
                static const char *kwdlist[] = {"value", nullptr};
                PyObject pyobj;
                PythonClassWrapper<T> *pyclass = reinterpret_cast<PythonClassWrapper<T> *>(&pyobj);
                if (PyArg_ParseTupleAndKeywords(args, kwds, "O", (char **) kwdlist, &pyobj)) {
                    if (!PythonClassWrapper<T>::checkType(&pyobj)) {
                        PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                        return -1;
                    }
                    self->_CObject = pyclass->template get_CObject<T>();
                } else if ((!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
                    if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
                        PyErr_Clear();
                    } else {
                        PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                        return -1;
                    }
                } else {
                    PyErr_Print();
                    PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
                    return -1;
                }
            }
            return 0;
        }
    };

    //specialize for copiable non-fundamental reference types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<!std::is_void<T>::value &&
                                                             !std::is_integral<typename std::remove_reference<T>::type>::value &&
                                                             !std::is_floating_point<typename std::remove_reference<T>::type>::value &&
                                                             std::is_copy_constructible<typename std::remove_reference<T>::type>::value &&
                                                             std::is_reference<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
            if (!self) {
                return -1;
            }
            self->_CObject = nullptr;
            for (auto it = PythonClassWrapper<T>::_constructors.begin();
                 it != PythonClassWrapper<T>::_constructors.end(); ++it) {
                try {
                    if ((*it)(args, kwds, self->_CObject)) {
                        break;
                    }
                } catch (...) {
                }
                PyErr_Clear();
            }

            if (self->_CObject == nullptr) {
                if ((!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
                    if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
                        PyErr_Clear();
                    } else {
                        PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                        return -1;
                    }
                } else {
                    PyErr_Print();
                    PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
                    return -1;
                }
            }
            return 0;
        }
    };

    //  specialize for non-copiable non-fundamental reference types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            !std::is_integral<typename std::remove_reference<T>::type>::value &&
            !std::is_floating_point<typename std::remove_reference<T>::type>::value &&
            !std::is_copy_constructible<typename std::remove_reference<T>::type>::value &&
            std::is_reference<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
            if (!self) {
                return -1;
            }
            self->_CObject = nullptr;
            for (auto it = PythonClassWrapper<T>::_constructors.begin();
                 it != PythonClassWrapper<T>::_constructors.end(); ++it) {
                try {
                    if ((*it)(args, kwds, self->_CObject)) {
                        break;
                    }
                } catch (...) {
                }
                PyErr_Clear();
            }

            if (self->_CObject == nullptr) {

                if ((!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
                    if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
                        PyErr_Clear();
                    } else {
                        PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                        return -1;
                    }
                } else {
                    PyErr_Print();
                    PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
                    return -1;
                }
            }
            return 0;
        }
    };

    //specialize for integral reference types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            std::is_integral<typename std::remove_reference<T>::type>::value &&
            std::is_reference<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
            typedef typename std::remove_reference<T>::type T_NoRef;
            if (!self) {
                return -1;
            }
            self->_CObject = nullptr;
            int status = 0;
            if (self->_CObject == nullptr) {
                static const char *kwdlist[] = {"value", nullptr};
                if (std::is_signed<T_NoRef>::value) {
                    long long intval = 0;

                    if (!PyArg_ParseTupleAndKeywords(args, kwds, "L", (char **) kwdlist, &intval)) {
                        PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                        status = -1;
                        goto onerror;
                    }
                    if (intval < (long long) std::numeric_limits<T_NoRef>::min() ||
                        intval > (long long) std::numeric_limits<T_NoRef>::max()) {
                        PyErr_SetString(PyExc_TypeError, "Argument value out of range");
                        status = -1;
                        goto onerror;
                    }
                    self->_CObject = new T_NoRef((T_NoRef) intval);

                } else {
                    unsigned long long intval = 0;

                    if (!PyArg_ParseTupleAndKeywords(args, kwds, "K", (char **) kwdlist, &intval)) {
                        PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                        status = -1;
                        goto onerror;
                    }
                    if (intval < (unsigned long long) std::numeric_limits<T_NoRef>::min() ||
                        intval > (unsigned long long) std::numeric_limits<T_NoRef>::max()) {
                        PyErr_SetString(PyExc_TypeError, "Argument value out of range");
                        status = -1;
                        goto onerror;
                    }
                    self->_CObject = new T_NoRef((T_NoRef) intval);

                }
            }
            onerror:
            if (status != 0 && (!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
                if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
                    PyErr_Clear();
                    status = 0;
                } else {
                    if ((!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0))
                        PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                    status = -1;
                }
            }
            return status;
        }
    };

    //specialize for floating point reference types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            std::is_floating_point<typename std::remove_reference<T>::type>::value &&
            std::is_reference<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
            typedef typename std::remove_reference<T>::type T_NoRef;
            if (!self) {
                return -1;
            }


            if (self->_CObject == nullptr) {
                static const char *kwdlist[] = {"value", nullptr};
                double intval = 0;

                if (!PyArg_ParseTupleAndKeywords(args, kwds, "d", (char **) kwdlist, &intval)) {
                    PyErr_SetString(PyExc_TypeError, "Invalid type to construct from");
                    return -1;
                }
                if (intval < (double) std::numeric_limits<T_NoRef>::min() ||
                    intval > (double) std::numeric_limits<T_NoRef>::max()) {
                    PyErr_SetString(PyExc_TypeError, "Argument value out of range");
                    return -1;
                }
                self->_CObject = new T_NoRef((T_NoRef) intval);

            }
            return 0;
        }
    };

    //specialize for other complex types:
    template<typename T>
    class InitHelper<T, typename std::enable_if<
            !std::is_arithmetic<T>::value && !std::is_reference<T>::value &&
            !std::is_pointer<T>::value && !std::is_fundamental<T>::value>::type> {
    public:
        static int init(PythonClassWrapper<T> *self, PyObject *args, PyObject *kwds) {
            if (!self) {
                return -1;
            }
            self->_CObject = nullptr;
            for (auto it = PythonClassWrapper<T>::_constructors.begin();
                 it != PythonClassWrapper<T>::_constructors.end(); ++it) {
                try {
                    if ((*it)(args, kwds, self->_CObject)) {
		        self->_allocated = (self->_CObject != nullptr);
                        return 0;
                    }
                } catch (...) {
                }
                PyErr_Clear();
            }
            if ((!args || PyTuple_Size(args) == 0) && kwds && PyDict_Size(kwds) == 1) {
                if (PyDict_GetItemString(kwds, "__internal_allow_null") == Py_True) {
                    PyErr_Clear();
                } else {
                    PyErr_SetString(PyExc_RuntimeError, "Creation of null C object not allowed");
                    return -1;
                }
            } else {
                PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
                return -1;
            }

            return 0;
        }
    };


    /**
     * Class to define Python wrapper to C class/type
     **/
    template<typename T>
    struct PythonClassWrapper<T, UNKNOWN_SIZE,
            typename std::enable_if< !std::is_array<T>::value && !std::is_pointer<T>::value>::type> : public CommonBaseWrapper {
        typedef CommonBaseWrapper::Base Base;
        typedef PythonClassWrapper DereferencedWrapper;
        typedef PythonClassWrapper<T const, UNKNOWN_SIZE,void> ConstWrapper;
        typedef PythonClassWrapper<typename std::remove_const<T>::type> NonConstWrapper;
        typedef PythonClassWrapper<typename std::remove_reference<T>::type> NoRefWrapper;

        typedef typename std::remove_reference<T>::type T_NoRef;

        template<typename Z = T>
        typename std::remove_reference<T>::type *get_CObject() {
            return _CObject;
        }

        static PyTypeObject Type;
        typedef PyTypeObject *TypePtr_t;
        static TypePtr_t constexpr TypePtr = &Type;

        static int initialize() { return Type.tp_name ? 0 : -1; }

        static int initialize(const char *const name, const char *const module_entry_name,
                              PyObject *module, const char *const fullname = nullptr) {
            if (!name || strlen(name) == 0) return -1;
            if (!module_entry_name || strlen(module_entry_name) == 0) return -1;
            int status = 0;
            _name = name;
            _module_entry_name = module_entry_name;
            if (Type.tp_name) {/*already initialize*/ return status; }
            char *tp_name = new char[strlen(fullname ? fullname : name) + 1];
            strcpy(tp_name, fullname ? fullname : name);
            Type.tp_name = tp_name;

            PyMethodDef pyMeth = {
                    address_name,
                    addr,
                    METH_KEYWORDS,
                    "C address of object"
            };
            _methodCollection.insert(_methodCollection.begin(), pyMeth);
            PyMethodDef pyMethAlloc = {
                    alloc_name,
                    (PyCFunction) alloc,
                    METH_KEYWORDS | METH_CLASS,
                    "allocate arry of single dynamic instance of this class"
            };
            _methodCollection.insert(_methodCollection.begin(), pyMethAlloc);
            Type.tp_methods = _methodCollection.data();
            if (!_baseClasses.empty()) {
                Type.tp_bases = PyTuple_New(_baseClasses.size());
                Py_ssize_t index = 0;
                for (auto it = _baseClasses.begin(); it != _baseClasses.end(); ++it) {
                    PyTuple_SET_ITEM(Type.tp_bases, index++, (PyObject * ) * it);
                }
            }
            if (PyType_Ready(&Type) < 0) {
                PyErr_SetString(PyExc_RuntimeError, "Failed to ready type!");
                PyErr_Print();
                status = -1;
                goto onerror;
            }
            {
                PyObject *const type = reinterpret_cast<PyObject *>(&Type);
                Py_INCREF(type);
                if (module != nullptr) {
                    if (PyModule_AddObject(module, module_entry_name, type) == 0) {
                        parent_module = module;
                    } else {
                        PyErr_Print();
                        PyErr_SetString(PyExc_RuntimeError, "Failed to add type to module!");
                    }
                }
            }
            onerror:
            return status;
        }


        static PythonClassWrapper* createPy(const ssize_t arraySize, T_NoRef * const cobj, const bool isAllocated, PyObject* referencing, const size_t depth = 0){
            static PyObject* kwds = PyDict_New();
            static PyObject* emptyargs = PyTuple_New(0);
            PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);
            typedef PythonClassWrapper< T, -1> PyWrapper;
            PyWrapper* pyobj = (PyWrapper*)PyObject_Call((PyObject*)&Type, emptyargs, kwds);
            pyobj->_CObject = cobj;
            pyobj->_allocated = isAllocated;
            if(referencing) pyobj->_referenced = referencing;
            return pyobj;
        }
        /**
         * Add a constructor to the list contained
         **/
        typedef typename ObjectLifecycleHelpers::BasicAlloc<T, PythonClassWrapper<T_NoRef *> >::ConstructorContainer ConstructorContainer;
        typedef typename ConstructorContainer::constructor constructor;

        static void addConstructor(const char *const kwlist[], constructor c) {
            _constructors.push_back(ConstructorContainer(kwlist, c));
        }


        template<typename ...Args>
        static bool create(const char *const kwlist[], PyObject *args, PyObject *kwds, T_NoRef *&cobj) {
            try {
                return _createBase(cobj, args, kwds, kwlist, typename argGenerator<sizeof...(Args)>::type(),
                                   (typename std::remove_reference<Args>::type *) nullptr...);

            } catch (const char *const msg) {
                PyErr_SetString(PyExc_RuntimeError, msg);
                PyErr_Print();
                return false;
            }
        }

        static PyObject *addr(PyObject *self, PyObject *args) {
            if ((args && PyTuple_Size(args) > 0)) {
                PyErr_BadArgument();
                return nullptr;
            }
            std::string ptr_name = std::string(Type.tp_name) + "*";
            std::string module_ptr_name = std::string(Type.tp_name) + "_ptr";
            PythonClassWrapper<T_NoRef *>::initialize();
	    PythonClassWrapper* self_ = reinterpret_cast<PythonClassWrapper *>(self);
            PyObject *obj = toPyObject<T_NoRef *>(self_->_CObject, true, 1);
            PyErr_Clear();
            ((PythonClassWrapper<T_NoRef *> *) obj)->make_reference(self);
            return obj;
        }


        static bool findMemberSetter(const char *const name) {
            return _memberSettersDict.find(name) != _memberSettersDict.end();
        }

        static void callMemberSetter(const char *const name, T_NoRef *this_, PyObject *pyval) {
            if (findMemberSetter(name))
                _memberSettersDict[name](this_, pyval);
        }

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char *const name, typename ReturnType, typename ...Args>
        static void addClassMethod(ReturnType(*method)(Args...), const char *const kwlist[]) {
            static const char *const doc = "Call class method ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);

            PyMethodDef pyMeth = {
                    name,
                    (PyCFunction) ClassMethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::call,
                    METH_KEYWORDS | METH_CLASS,
                    doc_string
            };

            ClassMethodContainer<T>::template Container<name, ReturnType, Args...>::method = method;
            ClassMethodContainer<T>::template Container<name, ReturnType, Args...>::kwlist = kwlist;
            _addMethod(pyMeth);
        }

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char *const name, typename ReturnType, typename ...Args>
        static void addMethod(
                typename MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]) {
            static const char *const doc = "Call method ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);

            PyMethodDef pyMeth = {
                    name,
                    (PyCFunction) MethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::call,
                    METH_KEYWORDS,
                    doc_string
            };

            MethodContainer<T>::template Container<name, ReturnType, Args...>::method = method;
            MethodContainer<T>::template Container<name, ReturnType, Args...>::kwlist = kwlist;
            _addMethod(pyMeth);
        }

        /**
         * add a method with given compile-time-known name to the contained collection
         **/
        template<const char *const name, typename ReturnType, typename ...Args>
        static void addConstMethod(
                typename ConstMethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::method_t method,
                const char *const kwlist[]) {
            static const char *const doc = "Call method ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);

            PyMethodDef pyMeth = {
                    name,
                    (PyCFunction) ConstMethodContainer<T_NoRef>::template Container<name, ReturnType, Args...>::call,
                    METH_KEYWORDS,
                    doc_string
            };

            ConstMethodContainer<T>::template Container<name, ReturnType, Args...>::method = method;
            ConstMethodContainer<T>::template Container<name, ReturnType, Args...>::kwlist = kwlist;
            _addMethod(pyMeth);
        }


        static void addClassMember(const char *const name, PyObject *pyobj) {
            if (!Type.tp_dict) {
                Type.tp_dict = PyDict_New();
            }
            PyDict_SetItemString(Type.tp_dict, name, pyobj);
        }

        static void addExtraBaseClass(PyTypeObject *base) {
            if (!base) return;
            if (!Type.tp_base && _baseClasses.empty()) {
                Type.tp_base = base;
            } else {
                if (Type.tp_base) {
                    _baseClasses.push_back(Type.tp_base);
                    Type.tp_base = nullptr;
                }
                _baseClasses.insert(_baseClasses.begin(), base);
            }
        }

        /**
         * add a getter method for the given compile-time-known named public class member
         **/
        template<const char *const name, ssize_t size, typename Type>
        static void addAttribute(
                typename MemberContainer<T_NoRef>::template Container<name, Type[size]>::member_t member,
                const ssize_t array_size) {
            assert(array_size == size);
            static const char *const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
            static const char *const getter_prefix = "get_";
            //char *getter_name = new char[strlen(name) +strlen(getter_prefix)+1];
            //snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
            MemberContainer<T_NoRef>::template Container<name, Type[size]>::member = member;
            PyMethodDef pyMeth = {name,
                                  (PyCFunction) MemberContainer<T_NoRef>::template Container<name, Type[size]>::call,
                                  METH_KEYWORDS,
                                  doc_string
            };
            _addMethod(pyMeth);
            _memberSettersDict[name] = MemberContainer<T>::template Container<name, Type[size]>::setFromPyObject;
        }

        /**
         * add a getter method for the given compile-time-known named public class member
         **/
        template<const char *const name, typename Type>
        static void addAttribute(typename MemberContainer<T_NoRef>::template Container<name, Type>::member_t member,
                                 const ssize_t array_size) {

            static const char *const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
            MemberContainer<T_NoRef>::template Container<name, Type>::member = member;
            MemberContainer<T_NoRef>::template Container<name, Type>::array_size = array_size;
            PyMethodDef pyMeth = {name,
                                  (PyCFunction) MemberContainer<T_NoRef>::template Container<name, Type>::call,
                                  METH_KEYWORDS,
                                  doc_string
            };
            _addMethod(pyMeth);
            _memberSettersDict[name] = MemberContainer<T>::template Container<name, Type>::setFromPyObject;
        }


        /**
         * add a getter method for the given compile-time-known named public class member
         **/
        template<const char *const name, typename Type>
        static void addConstAttribute(
                typename ConstMemberContainer<T_NoRef>::template Container<name, Type>::member_t member) {

            static const char *const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
            ConstMemberContainer<T_NoRef>::template Container<name, Type>::member = member;
            PyMethodDef pyMeth = {name,
                                  (PyCFunction) ConstMemberContainer<T_NoRef>::template Container<name, Type>::call,
                                  METH_KEYWORDS,
                                  doc_string
            };
            _addMethod(pyMeth);
        }


        /**
         * add a getter method for the given compile-time-known named public static class member
         **/
        template<const char *const name, typename Type>
        static void addClassAttribute(Type *member) {

            static const char *const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
            static const char *const getter_prefix = "get_";
            //char *getter_name = new char[strlen(name) +strlen(getter_prefix)+1];
            //snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
            static const char *const kwlist[] = {"value", nullptr};
            ClassMemberContainer<T_NoRef>::template Container<name, Type>::kwlist = kwlist;
            ClassMemberContainer<T_NoRef>::template Container<name, Type>::member = member;
            PyMethodDef pyMeth = {name,
                                  (PyCFunction) ClassMemberContainer<T_NoRef>::template Container<name, Type>::call,
                                  METH_KEYWORDS | METH_CLASS,
                                  doc_string
            };
            _addMethod(pyMeth);
            _memberSettersDict[name] = ClassMemberContainer<T>::template Container<name, Type>::setFromPyObject;
        }

        /**
         * add a getter method for the given compile-time-known named public static class member
         **/
        template<const char *const name, typename Type>
        static void addConstClassAttribute(Type const *member) {

            static const char *const doc = "Get attribute ";
            char *doc_string = new char[strlen(name) + strlen(doc) + 1];
            snprintf(doc_string, strlen(name) + strlen(doc) + 1, "%s%s", doc, name);
            static const char *const getter_prefix = "get_";
            //char *getter_name = new char[strlen(name) +strlen(getter_prefix)+1];
            //snprintf(getter_name, strlen(name) +strlen(getter_prefix)+1, "%s%s_",getter_prefix,name);
            static const char *const kwlist[] = {"value", nullptr};
            ConstClassMemberContainer<T_NoRef>::template Container<name, Type>::kwlist = kwlist;
            ConstClassMemberContainer<T_NoRef>::template Container<name, Type>::member = member;
            PyMethodDef pyMeth = {name,
                                  (PyCFunction) ConstClassMemberContainer<T_NoRef>::template Container<name, Type>::call,
                                  METH_KEYWORDS | METH_CLASS,
                                  doc_string
            };
            _addMethod(pyMeth);
        }

        void set_contents(typename std::remove_reference<T>::type *ptr, bool allocated) {
            _allocated = allocated;
            _CObject = ptr;
        }

        static bool checkType( PyObject* const obj){
            return PyObject_TypeCheck(obj, &Type);
        }

        static PyTypeObject* getType(){
            return &Type;
        }

        static std::string get_name() { return _name; }

        static std::string get_module_entry_name() { return _module_entry_name; }

        static PyObject *parent_module;

        template<typename C, const ssize_t size, typename depth>
        friend
        class PythonCPointerWrapper;

        template<typename C, typename E>
        friend
        class InitHelper;

    protected:

        static PyObject *alloc(PyObject *cls, PyObject *args, PyObject *kwds) {
            return ObjectLifecycleHelpers::Alloc<T, PythonClassWrapper<T_NoRef *>, PythonClassWrapper>::allocbase
                    (cls, args, kwds, _constructors);
        }

        T_NoRef *_CObject;

    private:

        static int
        _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds) {
            if (Type.tp_base && Base::TypePtr->tp_init) {
                PyObject *empty = PyTuple_New(0);
                Base::TypePtr->tp_init((PyObject * ) & self->baseClass, empty, nullptr);
                Py_DECREF(empty);
                PyErr_Clear();
            }
            self->_allocated = false;
            self->_raw_storage = nullptr;
            return InitHelper<T>::init(self, args, kwds);
        }

        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
            (void) args;
            (void) kwds;
            PythonClassWrapper *self;
            self = (PythonClassWrapper *) type->tp_alloc(type, 0);
            /*if( Type.tp_base && Base::TypePtr->tp_new){
              PyObject* empty = PyTuple_New(0);
              Base::TypePtr->tp_new( Type.tp_base, empty, nullptr);
              Py_DECREF(empty);
              }*/
            if (nullptr != self) {
                self->_CObject = nullptr;
            }
            return (PyObject *) self;
        }

        static void _free(void *self_) {
            PythonClassWrapper *self = (PythonClassWrapper *) self_;
            T_NoRef *ptr = (T_NoRef *) self->_CObject;
            if (self->_raw_storage) {
                delete self->_raw_storage;
            } else if (self->_allocated) {
                ObjectLifecycleHelpers::Alloc<T_NoRef, PythonClassWrapper<T_NoRef *>, PythonClassWrapper>::dealloc(
                        ptr);
            }
            self->_raw_storage = nullptr;
            if(self->_allocated) self->_CObject = nullptr;
        }

        static void _dealloc(PythonClassWrapper *self) {
            if (!self) return;
            if (self->_referenced) {
                Py_XDECREF(self->_referenced);
                self->_referenced = nullptr;
            }
            _free(self);
        }


        template<typename ...PyO>
        static bool _parsePyArgs(const char *const kwlist[], PyObject *args, PyObject *kwds, PyO *&...pyargs) {
            char format[sizeof...(PyO) + 1] = {0};
            if (sizeof...(PyO) > 0)
                memset(format, 'O', sizeof...(PyO));
            return sizeof...(PyO) == 0 || PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...);
        }

        template<typename ...Args>
        static bool _createBaseBase(T_NoRef *&cobj, Args... args) {
            if (!cobj) {
                cobj = new T_NoRef(args...);
                return cobj != nullptr;
            }
            new(cobj) T_NoRef(args...);
            return true;
        }

        template<typename ...Args, int ...S>
        static bool _createBase
                (T_NoRef *&cobj, PyObject *args, PyObject *kwds,
                 const char *const kwlist[], container<S...> unused1, Args *... unused2) {
            (void) unused1;
            void *unused[] = {(void *) unused2..., nullptr};
            (void) unused;
            if (args && PyTuple_Size(args) != sizeof...(Args)) {
                return false;
            }
            PyObject *pyobjs[sizeof...(Args) + 1];
            (void) pyobjs;
            if (!_parsePyArgs(kwlist, args, kwds, pyobjs[S]...)) {
                PyErr_SetString(PyExc_TypeError, "Invalid constructor arguments");
                return false;
            }

            return _createBaseBase<Args...>(cobj,
                                            *toCObject<Args, false, PythonClassWrapper<Args> >(
                                                    *pyobjs[S])...);
        }

        static void _addMethod(PyMethodDef method) {
            //insert at beginning to keep null sentinel at end of list:
            _methodCollection.insert(_methodCollection.begin(), method);
            Type.tp_methods = _methodCollection.data();
        }

        static std::string _name;
        static std::string _module_entry_name;
        static std::vector <ConstructorContainer> _constructors;
        static std::map<std::string, typename MethodContainer<T>::setter_t> _memberSettersDict;
        static std::vector <PyMethodDef> _methodCollection;
        static std::vector<PyTypeObject *> _baseClasses;

        char *_raw_storage;
        bool _allocated;

    };


    template<typename T>
    PyObject *PythonClassWrapper<T, UNKNOWN_SIZE,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            parent_module = nullptr;
    template<typename T>
    std::vector <PyMethodDef> PythonClassWrapper<T, UNKNOWN_SIZE,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            _methodCollection = std::vector<PyMethodDef>(emptyMethods, emptyMethods + 1);
    template<typename T>
    std::vector<PyTypeObject *> PythonClassWrapper<T, UNKNOWN_SIZE,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            _baseClasses = std::vector<PyTypeObject *>();
    template<typename T>
    std::map<std::string, typename MethodContainer<T>::setter_t>
            PythonClassWrapper<T, UNKNOWN_SIZE,
                    typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            _memberSettersDict = std::map<std::string, typename MethodContainer<T>::setter_t>();
    template<typename T>
    std::vector< typename PythonClassWrapper<T, UNKNOWN_SIZE, typename std::enable_if< !std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
                          ConstructorContainer>
            PythonClassWrapper<T, UNKNOWN_SIZE,
                    typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            _constructors;


    template<typename T>
    std::string PythonClassWrapper<T, UNKNOWN_SIZE,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::_name;
    template<typename T>
    std::string PythonClassWrapper<T, UNKNOWN_SIZE,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::_module_entry_name;
    template<typename T>
    PyTypeObject PythonClassWrapper<T, UNKNOWN_SIZE,
            typename std::enable_if<!std::is_array<T>::value && !std::is_pointer<T>::value>::type>::
            Type = {

                    PyObject_HEAD_INIT(nullptr)
                    0,                         /*ob_size*/
                    nullptr,             /*tp_name*/ /*filled on init*/
                    sizeof(PythonClassWrapper),             /*tp_basicsize*/
                    0,                         /*tp_itemsize*/
                    (destructor)PythonClassWrapper::_dealloc, /*tp_dealloc*/
                    nullptr,                         /*tp_print*/
                    nullptr,                         /*tp_getattr*/
                    nullptr,                         /*tp_setattr*/
                    nullptr,                         /*tp_compare*/
                    nullptr,                         /*tp_repr*/
                    nullptr,                         /*tp_as_number*/
                    nullptr,                         /*tp_as_sequence*/
                    nullptr,                         /*tp_as_mapping*/
                    nullptr,                         /*tp_hash */
                    nullptr,                         /*tp_call*/
                    nullptr,                         /*tp_str*/
                    nullptr,                         /*tp_getattro*/
                    nullptr,                         /*tp_setattro*/
                    nullptr,                         /*tp_as_buffer*/
                    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
                    "PythonClassWrapper object",           /* tp_doc */
                    nullptr,                       /* tp_traverse */
                    nullptr,                       /* tp_clear */
                    nullptr,                       /* tp_richcompare */
                    0,                               /* tp_weaklistoffset */
                    nullptr,                       /* tp_iter */
                    nullptr,                       /* tp_iternext */
                    PythonClassWrapper::_methodCollection.data(),             /* tp_methods */
                    nullptr,             /* tp_members */
                    nullptr,                         /* tp_getset */
                    Base::TypePtr,                         /* tp_base */
                    nullptr,                         /* tp_dict */
                    nullptr,                         /* tp_descr_get */
                    nullptr,                         /* tp_descr_set */
                    0,                         /* tp_dictoffset */
                    (initproc)PythonClassWrapper::_init,  /* tp_init */
                    nullptr,                         /* tp_alloc */
                    PythonClassWrapper::_new,             /* tp_new */
                    _free,                         /*tp_free*/
                    nullptr,                         /*tp_is_gc*/
                    nullptr,                         /*tp_bases*/
                    nullptr,                         /*tp_mro*/
                    nullptr,                         /*tp_cache*/
                    nullptr,                         /*tp_subclasses*/
                    nullptr,                          /*tp_weaklist*/
                    nullptr,                          /*tp_del*/
                    0,                          /*tp_version_tag*/
            };


}
#endif
