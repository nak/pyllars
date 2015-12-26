#ifndef __PYLLARS__CONVERSIONS
#define __PYLLARS__CONVERSIONS

#include <memory>
#include <Python.h>

#include "pyllars_pointer.hpp"
#include "pyllars_callbacks.hpp"
#include "pyllars_utils.hpp"

/**
 * This header file defines conversions from Python to C objects
 **/
namespace __pyllars_internal {

    /**
     * template fnction to convert python to C object
     **/
    template<typename C_type, bool is_array, typename ClassWrapper, typename E = void>
    class CObjectConversionHelper;


    /**
     * Specialization for classes, references and pointer types (except function ptr typedefs)
     **/
    template<typename C_type, bool is_array, typename PtrWrapper>
    class CObjectConversionHelper<C_type, is_array, PtrWrapper,
            typename std::enable_if<std::is_class<C_type>::value ||
                                    (std::is_reference<C_type>::value &&
                                     !std::is_const<typename std::remove_reference<C_type>::type>::value) ||
                                    (std::is_pointer<C_type>::value &&
                                     std::is_convertible<C_type, const void *>::value)>::type> {
    public:

        typedef typename PtrWrapper::NoRefWrapper NoRefClassWrapper;

        static smart_ptr<C_type> toCObject(PyObject &pyobj) {
            if (&pyobj == nullptr) {
                throw "Invalid argument for conversion";
            }
            typedef typename std::remove_reference<C_type>::type C_bare;
            // typedef typename std::remove_reference<typename std::remove_pointer<C_type>::type>::type C_base;

            if (PyObject_TypeCheck(&pyobj, (&PtrWrapper::Type))) {
                return smart_ptr<C_type>((C_bare *) reinterpret_cast<PtrWrapper *>
                (&pyobj)->template get_CObject<C_bare>(), false);
            } else if (PyObject_TypeCheck(&pyobj, (&NoRefClassWrapper::Type))) {
                return smart_ptr<C_type>(reinterpret_cast<NoRefClassWrapper *>(&pyobj)->
                        template get_CObject<typename std::remove_const<C_bare>::type>(), false);
            } else if (PyObject_TypeCheck(&pyobj, (&PtrWrapper::Type))) {
                return smart_ptr<C_type>(reinterpret_cast<PtrWrapper *>(&pyobj)->
                        template get_CObject<typename std::remove_const<C_bare>::type>(), false);
            }
            if (!PyObject_TypeCheck(&pyobj, (&PtrWrapper::Type)) &&
                !PyObject_TypeCheck(&pyobj, (&NoRefClassWrapper::Type))) {
                fprintf(stderr, "\n");
                PyObject_Print(&pyobj, stderr, 0);
                fprintf(stderr, "\n");
                PyObject_Print(PyObject_Type(&pyobj), stderr, 0);
                fprintf(stderr, "\n");
                throw "Invalid type converting to C object";
            } else {
                return smart_ptr<C_type>(reinterpret_cast<PtrWrapper * >(&pyobj)->
                        template get_CObject<C_bare>(), false);
            }
            return smart_ptr<C_type>(reinterpret_cast<PtrWrapper * >(&pyobj)->
                    template get_CObject<C_type>(), false);
        }
    };

    /**
     * Specialization for integer types
     **/
    template<typename T, bool is_array, typename ClassWrapper>
    class CObjectConversionHelper<T, is_array, ClassWrapper, typename std::enable_if<
            std::is_integral<T>::value || std::is_enum<T>::value ||
            (std::is_integral<typename std::remove_reference<T>::type>::value &&
             std::is_reference<T>::value &&
             std::is_const<typename std::remove_reference<T>::type>::value) ||
            (std::is_enum<typename std::remove_reference<T>::type>::value &&
             std::is_reference<T>::value &&
             std::is_const<typename std::remove_reference<T>::type>::value)>::type> {
    public:
        static smart_ptr<T> toCObject(PyObject &pyobj) {
            typedef typename std::remove_reference<typename std::remove_const<T>::type>::type T_bare;
            if (PyInt_Check(&pyobj)) {
                T_bare *value = new T_bare((T_bare) PyInt_AsLong(&pyobj));
                return smart_ptr<T>(value, true);
            } else if (PyLong_Check(&pyobj)) {
                T_bare *value = new T_bare((T_bare) PyLong_AsLongLong(&pyobj));
                return smart_ptr<T>(value, false);
            } else if (!PyObject_TypeCheck(&pyobj, (&ClassWrapper::Type))) {
                throw "Invalid type converting to C object";
            }
            return smart_ptr<T>(reinterpret_cast<ClassWrapper * >(&pyobj)->template get_CObject<T_bare>(), false);
        }
    };

    /**
     * Specialization for floating point types
     **/
    template<typename T, bool is_array, typename ClassWrapper>
    class CObjectConversionHelper<T, is_array, ClassWrapper, typename std::enable_if<
            std::is_floating_point<T>::value ||
            (std::is_floating_point<typename std::remove_reference<T>::type>::value &&
             std::is_reference<T>::value &&
             std::is_const<typename std::remove_reference<T>::type>::value)>::type> {
    public:
        static smart_ptr<T> toCObject(PyObject &pyobj) {
            typedef typename std::remove_reference<T>::type T_bare;
            if (PyFloat_Check(&pyobj)) {
                return smart_ptr<T>(new T_bare(PyFloat_AsDouble(&pyobj)), true);
            } else if (!PyObject_TypeCheck(&pyobj, (&ClassWrapper::Type))) {
                PyObject_Print(&pyobj, stderr, 0);
                throw "Invalid type converting to C object";
            }
            T_bare *retval = (reinterpret_cast<ClassWrapper * >(&pyobj)->template get_CObject<T_bare>());
            return smart_ptr<T>(retval, false);
        }
    };

    /**
     * Specialization for callbacks
     **/
    template<typename ClassWrapper, typename ReturnType,
            typename ...Args>
    class CObjectConversionHelper<ReturnType(*)(Args...), false, ClassWrapper, void> {
    public:
        typedef ReturnType(*callback_t)(Args...);

        static smart_ptr<callback_t, false> toCObject(PyObject &pyobj) {
            if (!PyCallable_Check(&pyobj)) {
                throw "Python callback is not callable!";
            }
            callback_t *retval = new callback_t(PyCallbackWrapper<ReturnType, Args...>(&pyobj).get_C_callback());
            return smart_ptr<callback_t, false>(retval, true);
        }
    };

    /**
    * Specialization for char*
    **/
    template<typename ClassWrapper>
    class CObjectConversionHelper<const char *, false, ClassWrapper> {
    public:
        typedef typename ClassWrapper::ConstWrapper ConstClassWrapper;

        static smart_ptr<const char *> toCObject(PyObject &pyobj) {
            const char *name = nullptr;
            if (PyObject_Type(&pyobj) == (PyObject * ) & ConstClassWrapper::Type) {
                ConstClassWrapper *const self_ = (ConstClassWrapper *const) &pyobj;
                name = (const char *) self_->template get_CObject<const char *const>();
            } else if (PyObject_Type(&pyobj) == (PyObject * ) & ClassWrapper::Type) {
                ClassWrapper *const self_ = (ClassWrapper *const) &pyobj;
                name = (const char *) self_->template get_CObject<const char *>();
            } else if (PyString_Check(&pyobj)) {
                name = (const char *) PyString_AS_STRING(&pyobj);
            } else {
                PyObject_Print(&pyobj, stderr, 0);
                throw "Conversion to C stgring from non-string Python object!";
            }
            if (!name) { throw "Error converting string: null pointer encountered"; }

            return smart_ptr<const char *, false>((const char **) &name, false);
        }
    };

    /**
     * Specialization for char*
     **/
    template<typename ClassWrapper>
    class CObjectConversionHelper<const char *const, false, ClassWrapper> {
    public:

        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;

        static smart_ptr<const char *const, false> toCObject(PyObject &pyobj) {
            const char *name = nullptr;
            if (PyObject_Type(&pyobj) == (PyObject * ) & NonConstClassWrapper::Type) {
                NonConstClassWrapper *const self_ = ((NonConstClassWrapper *const) &pyobj);
                name = (const char *) self_->template get_CObject<const char *>();
            } else if (PyObject_Type(&pyobj) == (PyObject * ) & ClassWrapper::Type) {
                ClassWrapper *const self_ = ((ClassWrapper *const) &pyobj);
                name = (const char *) self_->template get_CObject<const char *const>();
            } else if (PyString_Check(&pyobj)) {
                name = (const char *) PyString_AS_STRING(&pyobj);
            } else {
                name = nullptr;
                PyObject_Print(&pyobj, stderr, 0);
                throw "Conversiont o C stgring from non-string Python object!";
            }
            if (!name) { throw "Error converting string: null pointer encountered"; }

            return smart_ptr<const char *const, false>((const char *const *) &name, false);
        }
    };


    /**
     * Specialization for char*
     **/
    template<typename ClassWrapper>
    class CObjectConversionHelper<char *const, false, ClassWrapper> {
    public:
        static smart_ptr<char *const> toCObject(PyObject &pyobj) {
            const char *name = nullptr;
            if (PyString_Check(&pyobj)) {
                name = (const char *) PyString_AS_STRING(&pyobj);
            } else {
                throw "Conversiont o C stgring from non-string Python object!";
            }
            if (!name) { throw "Error converting string: null pointer encountered"; }

            return smart_ptr<char *const>((char *const *) &name, false);
        }
    };

    /**
     * Specialization for char*
     **/
    template<typename ClassWrapper>
    class CObjectConversionHelper<char *, true, ClassWrapper> {
    public:
        static smart_ptr<char *, true> toCObject(PyObject &pyobj) {
            const char *name = nullptr;
            if (PyString_Check(&pyobj)) {
                name = (const char *) PyString_AS_STRING(&pyobj);
            } else {
                throw "Conversiont o C stgring from non-string Python object!";
            }
            if (!name) { throw "Error converting string: null pointer encountered"; }
            char *new_name = new char[strlen(name) + 1];
            strcpy(new_name, name);
            return smart_ptr<char *, true>(&new_name, true);
        }
    };


    /**
     * Specialization for char*
     **/
    template<typename T, const size_t size, const bool is_array, typename ClassWrapper>
    class CObjectConversionHelper<T[size], is_array, ClassWrapper> {
    public:

        typedef T T_array[size];
        typedef typename ClassWrapper::AsPtrWrapper PtrWrapper;
        typedef typename ClassWrapper::DereferencedWrapper NonPtrWrapper;

        static smart_ptr<T[size], is_array> toCObject(PyObject &pyobj) {

            if (PyObject_TypeCheck(&pyobj, &(PtrWrapper::Type))) {
                T *val_ = *reinterpret_cast<PtrWrapper * >(&pyobj)->template get_CObject<T *>();
                T_array *val = (T_array *) &val_;
                return smart_ptr<T[size], is_array>(val, true);
            } else if (PyObject_TypeCheck(&pyobj, (&ClassWrapper::Type))) {
                return smart_ptr<T_array, is_array>(
                        reinterpret_cast<ClassWrapper * >(&pyobj)->template get_CObject<T_array>(), false);
            } else if (PyList_Check(&pyobj)) {
                if (PyList_Size(&pyobj) != size) {
                    throw "Inconsistent sizes in array assignment";
                }
                if (!is_array) {
                    throw "Invalid attempt on non-array allocation of fixed-size array";
                }
                T_array *val = new T_array[1];
                for (size_t i = 0; i < size; ++i) {
                    PyObject *listitem = PyList_GetItem(&pyobj, i);
                    if (PyObject_TypeCheck(listitem, (&NonPtrWrapper::Type))) {
                        (*val)[i] = *reinterpret_cast< NonPtrWrapper * >(listitem)->template get_CObject<T>();
                    } else {
                        delete[] val;
                        throw "Invalid type in array element assignment";
                    }
                }

                return smart_ptr<T_array, is_array>(val, true);
            } else {
                throw "Conversiont o C stgring from non-string Python object!";
            }
            return smart_ptr<T_array, is_array>(nullptr, false);
        }

    };

    /**
      * Specialization for const array
      **/
    template<typename T, const size_t size, typename ClassWrapper>
    class CObjectConversionHelper<const T[size], false, ClassWrapper> {
    public:

        typedef const T T_array[size];
        typedef typename ClassWrapper::AsPtrWrapper PtrWrapper;//TODO MAKE SURE THIS HAS SIZE IN PARAM LIST!!


        static smart_ptr<T_array, false> toCObject(PyObject &pyobj) {

            if (PyObject_TypeCheck(&pyobj, &(PtrWrapper::Type))) {
                T_array *const val = reinterpret_cast<PtrWrapper * >
                (&pyobj)->template get_CObject<T *>();

                return smart_ptr<T_array, false>(val, true);
            } else if (PyObject_TypeCheck(&pyobj, (&ClassWrapper(&pyobj)))) {
                return smart_ptr<T_array, false>(
                        reinterpret_cast<ClassWrapper * >(&pyobj)->template get_CObject<T_array>(), false);
            } else {
                throw "Conversiont o C stgring from non-string Python object!";
            }
            return smart_ptr<T_array, false>(nullptr, false);
        }
    };


    /**
     * function to convert python object to underlying C type using a class helper
     **/
    template<typename T, bool is_array, typename ClassWrapper>
    smart_ptr<T, is_array> toCObject(PyObject &pyobj) {
        return CObjectConversionHelper<T, is_array, ClassWrapper>::toCObject(pyobj);
    }

    /**
     * function to convert python object to underlying C type using a class helper
     **/
    template<typename T, const size_t size, bool is_array, typename ClassWrapper>
    smart_ptr<T[size], is_array> toCObject(PyObject &pyobj) {
        return CObjectConversionHelper<T[size], is_array, ClassWrapper>::toCObject(pyobj);
    }

    class ConversionHelpers {
    public:

        template<typename T, const ssize_t max, typename E>
        friend PyObject *toPyObject(T &var, const bool asReference);

    private:

        ///////////
        // Helper conversion functions
        //////////

        /**
         * Define conversion helper class, which allows easier mechanism
         * for necessary specializations
         **/
        template<typename T, typename PtrWrapper, const ssize_t max, typename E = void>
        class PyObjectConversionHelper;

        /**
         * specialize for non-copiable types
         **/
        template<typename T, typename ClassWrapper, const ssize_t max>
        class PyObjectConversionHelper<T, ClassWrapper, max,
                typename std::enable_if<!std::is_array<T>::value &&
                                        !std::is_copy_constructible<T>::value>::type> {
        public:
            typedef typename std::remove_reference<T>::type T_NoRef;
            static PyObject *toPyObject(T_NoRef &var, const bool asReference, const ssize_t array_size = -1) {
                //must pass in keyword to inform Python it is ok to create a Python C Wrapper object
                //with a null content, which will be set here
                PyObject *kw = PyDict_New();
                PyDict_SetItemString(kw, "__internal_allow_null", PyBool_FromLong(asReference));
                //create the object of the desired type and do some checks
                PyObject *emptyTuple = PyTuple_New(0);
                PyObject *pyobj = (PyObject*) ClassWrapper::createPy(array_size, &var, !asReference, nullptr);//
                        //PyObject_Call((PyObject * ) & ClassWrapper::Type,
                        //                        emptyTuple, kw);
                Py_DECREF(kw);
                Py_DECREF(emptyTuple);
                if (!pyobj || !PyObject_TypeCheck(pyobj, (&ClassWrapper::Type))) {
                    PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                    goto onerror;
                }
                //here is where the pointer content is set:
                reinterpret_cast<ClassWrapper *>(pyobj)->set_contents(&var, false);
                return pyobj;
                onerror:
                return Py_None;
            }

        };


        /**
         * specialize for non-trivial copiable types (non-pointer too)
         **/
        template<typename T, typename ClassWrapper, const ssize_t max>
        class PyObjectConversionHelper<T, ClassWrapper, max,
                typename std::enable_if<std::is_copy_constructible<T>::value &&
                                        !std::is_integral<T>::value &&
                                        !std::is_enum<T>::value &&
                                        !std::is_floating_point<T>::value &&
                                        !std::is_pointer<T>::value>::type> {
        public:
            typedef typename std::remove_reference<T>::type T_NoRef;

            static PyObject *toPyObject(T &var, const bool asReference, const ssize_t array_size) {
                PyObject *kw = PyDict_New();
                PyDict_SetItemString(kw, "__internal_allow_null", PyBool_FromLong(asReference));
                //create the object of the desired type and do some checks
                PyObject *emptyTuple = PyTuple_New(0);
                PyObject *pyobj = PyObject_Call((PyObject * ) & ClassWrapper::Type, emptyTuple, kw);
                Py_DECREF(kw);
                Py_DECREF(emptyTuple);
                if (!pyobj || !PyObject_TypeCheck(pyobj, (&ClassWrapper::Type))) {
                    PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                    goto onerror;
                }

                reinterpret_cast<ClassWrapper *>(pyobj)->set_contents(asReference ? &var : new T_NoRef(var),
                                                                      !asReference);
                return pyobj;
                onerror:
                PyErr_Print();
                return Py_None;

            }
        };

        /**
          * specialize for non-extent array types (non-pointer too and non-const-char too)
          **/
        template<typename T, const size_t size,typename ClassWrapper, const ssize_t max>
        class PyObjectConversionHelper<T[size], ClassWrapper, max, void> {
        public:
            typedef typename std::remove_reference<T>::type T_NoRef;
            typedef T T_array[size];

            static PyObject *toPyObject(T_array &var, const bool asReference, const ssize_t array_size) {

                //create the object of the desired type and do some checks
                PyObject *pyobj = (PyObject*) ClassWrapper::template createPy<T_array>(array_size, &var, !asReference,
                                                                                  nullptr);//
                if (!pyobj || !PyObject_TypeCheck(pyobj, (&ClassWrapper::Type))) {
                    PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                    pyobj = nullptr;
                }
                return pyobj;

            }
        };

        /**
         * specialize for fixed-size array types (non-pointer too and non-const-char too)
         **/
        template<typename T, const size_t size, typename ClassWrapper, const ssize_t max>
        class PyObjectConversionHelper<const T[size], ClassWrapper, max, void> {
        public:
            typedef typename std::remove_reference<T>::type T_NoRef;
            typedef const T T_array[];

            static PyObject *toPyObject(T var[], const bool asReference, const ssize_t array_size) {
                if (!asReference) {
                    PyErr_SetString(PyExc_RuntimeError, "Attempt to copy array of unknown extent");
                    return nullptr;
                }
                //create the object of the desired type and do some checks
                PyObject *pyobj = ClassWrapper::template createPy<T_array>(array_size, &var, false, nullptr);//

                if (!pyobj || !PyObject_TypeCheck(pyobj, (&ClassWrapper::Type))) {
                    PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                    pyobj = nullptr;
                }

                return pyobj;

            }
        };


        /**
         * specialize for const non-extent array types (non-pointer too and non-const-char too)
         **/
        template<typename T, typename ClassWrapper, const ssize_t max>
        class PyObjectConversionHelper<T const[], ClassWrapper, max, void> {
        public:
            typedef typename std::remove_reference<T>::type T_NoRef;
            typedef const T T_array[];

            static PyObject *toPyObject(T const *const &var, const bool asReference, const ssize_t array_size) {
                if (!asReference) {
                    PyErr_SetString(PyExc_RuntimeError, "Attempt to copy array of unknown extent");
                    return nullptr;
                }

                //create the object of the desired type and do some checks
                // PyObject* emptyTuple = PyTuple_New(0);
                PythonClassWrapper<T const[], max>::initialize();

                PyObject *pyobj = ClassWrapper::template createPy<const T[]>(array_size, &var, false,
                                                                                        nullptr);//PyObject_Call(  (PyObject*)&ClassWrapper::Type,
                //emptyTuple, nullptr);
                // Py_DECREF(emptyTuple);

                if (!pyobj || !PyObject_TypeCheck(pyobj, (&ClassWrapper::Type))) {
                    PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                    pyobj = nullptr;
                }/*
            {
              reinterpret_cast<ClassWrapper*>(pyobj)->set_contents( (void**)(&var), false);
            }*/
                return pyobj;

            }
        };


        /**
         * specialize for integer types
         **/
        template<typename T, typename ClassWrapper, const ssize_t max>
        class PyObjectConversionHelper<T, ClassWrapper, max, typename std::enable_if<
                std::is_integral<T>::value || std::is_enum<T>::value>::type> {
        public:
            static PyObject *toPyObject(const T &var, const bool asReference, const ssize_t array_size = -1) {
                (void) asReference;
                return PyInt_FromLong((long) var);
            }
        };

        /**
         * specialize for floating point types
         **/
        template<typename T, typename ClassWrapper, const ssize_t max>
        class PyObjectConversionHelper<T, ClassWrapper, max, typename std::enable_if<std::is_floating_point<T>::value>::type> {
        public:
            static PyObject *toPyObject(const T &var, const bool asReference, const ssize_t array_size = -1) {
                (void) asReference;
                return PyFloat_FromDouble(var);
            }
        };

        /**
         * specialize for cosnt integer reference types
         **/
        template<typename T, typename ClassWrapper, const ssize_t max>
        class PyObjectConversionHelper<T, ClassWrapper, max, typename std::enable_if<
                std::is_const<T>::value && std::is_reference<T>::value &&
                (std::is_integral<typename std::remove_reference<T>::type>::value ||
                 std::is_enum<typename std::remove_reference<T>::type>::value
                )>::type> {
        public:
            static PyObject *toPyObject(const T &var, const bool asReference, const ssize_t array_size = -1) {
                if (!asReference) {
                    PyErr_SetString(PyExc_RuntimeError, "cannot create const reference variable from basic type");
                    return Py_None;
                }
                PyObject *args = PyTuple_New(1);
                PyTuple_SET_ITEM(args, 0, PyLong_FromLong(var));
                PyObject *retval = PyObject_CallObject(&ClassWrapper::Type, args);
                Py_DECREF(PyTuple_GetItem(args, 0));
                Py_DECREF(args);
                return retval;
            }
        };

        /**
         * specialize for const non-basic reference types
         **/
        template<typename T, typename ClassWrapper, const ssize_t max>
        class PyObjectConversionHelper<T, ClassWrapper, max,
                typename std::enable_if<std::is_const<T>::value &&
                                        std::is_reference<T>::value &&
                                        !std::is_integral<typename std::remove_reference<T>::type>::value &&
                                        std::is_floating_point<typename std::remove_reference<T>::type>::value>::type> {
        public:
            static PyObject *toPyObject(const T &var, const bool asReference, const ssize_t array_size = -1) {
                if (!asReference) {
                    PyErr_SetString(PyExc_RuntimeError, "cannot create const reference variable from basic type");
                    return Py_None;
                }
                PyObject *args = PyTuple_New(1);
                PyTuple_SET_ITEM(args, 0, toPyObject(var));
                PyObject *retval = PyObject_CallObject(&ClassWrapper::Type, args);
                Py_DECREF(PyTuple_GetItem(args, 0));
                Py_DECREF(args);
                return retval;
            }
        };

        /**
         * specialize for const floating point reference types
         **/
        template<typename T, typename ClassWrapper, const ssize_t max>
        class PyObjectConversionHelper<T, ClassWrapper, max,
                typename std::enable_if<std::is_const<T>::value &&
                                        std::is_reference<T>::value &&
                                        !std::is_floating_point<typename std::remove_reference<T>::type>::value>::type> {
        public:
            static PyObject *toPyObject(const T &var, const bool asReference, const ssize_t array_size = -1) {
                if (!asReference) {
                    PyErr_SetString(PyExc_RuntimeError, "cannot create const reference variable from basic type");
                    return Py_None;
                }
                PyObject *args = PyTuple_New(1);
                PyTuple_SET_ITEM(args, 0, PyFloat_FromDouble(var));
                PyObject *retval = PyObject_CallObject(&ClassWrapper::Type, args);
                Py_DECREF(PyTuple_GetItem(args, 0));
                Py_DECREF(args);
                return retval;
            }
        };

        /**
         * Specialized for non-const pointers:
         **/
        template<typename T, typename ClassWrapper, const ssize_t max>
        class PyObjectConversionHelper<T *, ClassWrapper, max, void/*typename  std::enable_if< !is_function_ptr<typename std::remove_pointer<T>::type >::value >::type*/> {
        public:
            static PyObject *toPyObject(T *const &var, const bool asReference, const ssize_t array_size) {
                (void) asReference;
                PyObject *pyobj = nullptr;

                typedef ClassWrapper PtrWrapper;

                if (!PtrWrapper::Type.tp_name) {
                    PtrWrapper::initialize();


                    if (PtrWrapper::parent_module) {
                        PyModule_AddObject(PtrWrapper::parent_module,
                                           PtrWrapper::Type.tp_name,
                                           (PyObject * ) & PtrWrapper::Type);
                    }
                }
                {
                    PyObject *emptyTuple = PyTuple_New(0);
                    pyobj = PyObject_CallObject((PyObject * ) & PtrWrapper::Type, emptyTuple);
                    Py_DECREF(emptyTuple);
                }
                if (!pyobj || !PyObject_TypeCheck(pyobj, &PtrWrapper::Type)) {
                    PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                    goto onerror;
                }
                reinterpret_cast<PtrWrapper *>(pyobj)->set_contents((void **) var, false);
                return pyobj;

                onerror:
                return Py_None;
            }
        };

        /**
         * Specialized for  pointers:
         **/
        template<typename T, typename PtrWrapper, const ssize_t max>
        class PyObjectConversionHelper<T *const, PtrWrapper, max, void/*typename  std::enable_if< !is_function_ptr<typename std::remove_pointer<T>::type >::value>::type*/> {
        public:
            static PyObject *toPyObject(T *const &var, const bool asReference, const ssize_t array_size) {
                (void) asReference;
                PyObject *pyobj = nullptr;

                //typedef PythonClassWrapper<T, max> ClassWrapper;
                if (!PtrWrapper::Type.tp_name) {
                    PtrWrapper::initialize();
                }
                if (PtrWrapper::parent_module) {
                    PyModule_AddObject(PtrWrapper::parent_module,
                                       PtrWrapper::Type.tp_name,
                                       (PyObject * ) & PtrWrapper::Type);
                }

                {
                    PyObject *emptyTuple = PyTuple_New(0);
                    pyobj = PyObject_CallObject((PyObject * ) & PtrWrapper::Type, emptyTuple);
                    Py_DECREF(emptyTuple);
                }
                if (!pyobj || !PyObject_TypeCheck(pyobj, &PtrWrapper::Type)) {
                    PyErr_SetString(PyExc_TypeError, "Unable to convert C type object to Python object");
                    goto onerror;
                }
                reinterpret_cast<PtrWrapper *>(pyobj)->set_contents((void **) var, false);
                return pyobj;

                onerror:
                return Py_None;
            }
        };

        /**
         * Specialized for char*:
         **/
        template<const ssize_t max, typename ClassWrapper>
        class PyObjectConversionHelper<const char *, ClassWrapper, max, void> {
        public:
            static PyObject *toPyObject(const char *const &var, const bool asReference, const ssize_t array_size = -1) {
                (void) asReference;
                if (!var) {
                    throw "NULL CHAR* encountered";
                }
                return PyString_FromString(var);

            }
        };

        template<const ssize_t max, typename ClassWrapper>
        class PyObjectConversionHelper<char *, ClassWrapper, max, void> {
        public:
            static PyObject *toPyObject(char *const &var, const bool asReference, const ssize_t array_size = -1) {
                (void) asReference;
                if (!var) {
                    throw "NULL CHAR* encountered";
                }
                return PyString_FromString(var);

            }

        };

        template<const ssize_t max, typename ClassWrapper>
        class PyObjectConversionHelper<const char *const, ClassWrapper, max, void> {
        public:
            static PyObject *toPyObject(const char *const &var, const bool asReference, const ssize_t array_size = -1) {
                (void) asReference;
                if (!var) {
                    throw "NULL CHAR* encountered";
                }
                return PyString_FromString(var);

            }

        };


    };


    /**
  * convert C Object to python object
  * @param var: value to convert
  * @param asArgument: whether to be used as argument or not (can determine if copy is made or reference semantics used)
  **/
    template<typename T, const ssize_t max, typename E>
    PyObject *toPyObject(T &var, const bool asArgument, const ssize_t array_size) {
        return ConversionHelpers::PyObjectConversionHelper<T, PythonClassWrapper<T, max, E>, max>::toPyObject(
                var, asArgument, array_size);
    }

    template<typename T, const ssize_t max, typename E>
    PyObject *toPyObject(const T &var, const bool asArgument, const ssize_t array_size) {
        return ConversionHelpers::PyObjectConversionHelper<const T, PythonClassWrapper<T, max, E>, max>::toPyObject(
                var, asArgument, array_size);
    }

}

#endif
