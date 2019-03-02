#ifndef ___PYLLARS__FUNCTION_WRAPPER
#define ___PYLLARS__FUNCTION_WRAPPER

#include <limits>
#include <functional>
#include <Python.h>
#include <structmember.h>
#include <limits.h>

#include "pyllars_utils.hpp"
#include <ffi.h>
#include "pyllars_varargs.hpp"

namespace __pyllars_internal {

    namespace{
        struct Arbitrary{
            float fdata;
            int idata;
        };
    }



    template<bool is_base_return_complete, bool with_ellipsis, typename ReturnType, typename ...Args>
    struct PythonFunctionWrapper : public CommonBaseWrapper {

        template<typename ...Throws>
        struct Wrapper {
            PyObject_HEAD
            typedef typename FuncContainer<with_ellipsis, ReturnType, Args...>::template Type<0, Throws...> FuncContainer;

            struct FunctType {

                static ReturnType call(const  FuncContainer func,
                                       Args... args, PyObject *extra_args) {
                    typedef typename std::remove_reference<ReturnType>::type ReturnType_NoRef;
                    if (!extra_args || PyTuple_Size(extra_args) == 0) {
                        return func._cfunc(args...);
                    } else {
                        const ssize_t extra_args_size = PyTuple_Size(extra_args);

                        ffi_cif cif;
                        ffi_type *arg_types[sizeof...(Args) + extra_args_size] = {FFIType<Args>::type()...};
                        void *arg_values[sizeof...(Args) + extra_args_size] = {(void *) &args...};
                        ffi_status status;

                        // Because the return value from foo() is smaller than sizeof(long), it
                        // must be passed as ffi_arg or ffi_sarg.
                        ffi_arg result_small;
                        unsigned char result_big[Sizeof<ReturnType_NoRef>::value];
                        void *result = (Sizeof<ReturnType_NoRef>::value >= sizeof(ffi_arg)) ? (void *) &result_big[0]
                                                                                     : (void *) &result_small;

                        // Specify the data type of each argument. Available types are defined
                        // in <ffi/ffi.h>.
                        union ArgType {
                            int intvalue;
                            long longvalue;
                            double doublevalue;
                            bool boolvalue;
                            void *ptrvalue;
                        };
                        ArgType extra_arg_values[sizeof...(Args) + extra_args_size];
                        for (size_t i = sizeof...(Args); i < sizeof...(Args) + extra_args_size; ++i) {
                            PyObject * nextArg = PyTuple_GetItem(extra_args, i - sizeof...(Args));
                            const int subtype = getType(nextArg, arg_types[i]);
                            switch (arg_types[i]->type) {
                                case FFI_TYPE_SINT32:
                                    extra_arg_values[i].intvalue = PyInt_AsLong(nextArg);
                                    arg_values[i] = &extra_arg_values[i].intvalue;
                                    break;
                                case FFI_TYPE_SINT64:
                                    extra_arg_values[i].longvalue = PyLong_AsLong(nextArg);
                                    arg_values[i] = &extra_arg_values[i].longvalue;
                                    break;
                                case FFI_TYPE_UINT8:
                                    extra_arg_values[i].boolvalue = (nextArg == Py_True);
                                    arg_values[i] = &extra_arg_values[i].boolvalue;
                                    break;
                                case FFI_TYPE_DOUBLE:
                                    extra_arg_values[i].doublevalue = PyFloat_AsDouble(nextArg);
                                    arg_values[i] = &extra_arg_values[i].doublevalue;
                                    break;
                                case FFI_TYPE_POINTER:
                                    if (STRING_TYPE == subtype) {
                                        const char* const string_value = PyString_AsString(nextArg);
                                        extra_arg_values[i].ptrvalue = (void*) string_value;
                                        arg_values[i] = &extra_arg_values[i].ptrvalue;
                                    } else if (COBJ_TYPE == subtype) {
                                        static const size_t offset = offset_of<ObjContainer<Arbitrary> *, PythonClassWrapper<Arbitrary> >
                                                (&PythonClassWrapper<Arbitrary>::_CObject);
                                        ObjContainer<void *> **ptrvalue =
                                                (ObjContainer<void *> **) (((char *) nextArg) + offset);
                                        extra_arg_values[i].ptrvalue = ptrvalue ? (*ptrvalue)->ptr() : nullptr;
                                    } else if (FUNC_TYPE == subtype) {
                                        typedef typename PythonFunctionWrapper<true, with_ellipsis, int, int>::template Wrapper<> wtype;
                                        typedef typename PythonFunctionWrapper<true, with_ellipsis, int, int>::template Wrapper<>::FuncContainer ftype;
                                        static const size_t offset = offset_of<ftype, wtype >(&wtype::_cfunc);
                                        void **ptrvalue = (void **) (((char *) nextArg) + offset);
                                        extra_arg_values[i].ptrvalue = *ptrvalue;
                                    } else {
                                        throw "Unable to convert Python object to C Object";
                                    }
                                    arg_values[i] = &extra_arg_values[i].ptrvalue;
                                    break;
                                default:
                                    throw "Python object cannot be converted to C object";
                                    break;
                            }
                        }

                        ffi_type *return_type = FFIType<ReturnType>::type();
                        // Prepare the ffi_cif structure.
                        if ((status = ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI,
                                                       sizeof...(Args), sizeof...(Args) + extra_args_size,
                                                       return_type, arg_types)) != FFI_OK) {
                            throw "FFI error calling variadic function";
                        }

                        // Invoke the function.
                        ffi_call(&cif, FFI_FN(func._cfunc), result, arg_values);

                        return *((ReturnType_NoRef *) result);

                    }
                }
            };

            static PyTypeObject Type;

            static PyObject *_call(PyObject *callable_object, PyObject *args, PyObject *kw);

            static int _init(PyObject *self, PyObject *args, PyObject *kwds);

            static PyObject *
            _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
                (void) args;
                (void) kwds;
                Wrapper *self = (Wrapper *) type->tp_alloc(type, 0);
                return (PyObject *) self;
            }

            static void initialize_type(const char *const name) {
                type_name = name;
                char *newname = new char[strlen(name) + 1 + ptrtp_name_prefix_len];
                strcpy(newname, ptrtp_name_prefix);
                strcpy(newname + ptrtp_name_prefix_len, name);
                Type.tp_name = newname;
            }

            static bool checkType(PyObject *const obj) {
                return PyObject_TypeCheck(obj, &Type);
            }

            static
            Wrapper *create(const char *const func_name,
                            FuncContainer func,
                            const char *const names[]) {
                static bool inited = false;
                Py_ssize_t index = 0;
                char *name = new char[strlen(func_name) + 1 + ptrtp_name_prefix_len];
                 memset(name, 0, strlen(func_name) + 1 + ptrtp_name_prefix_len);
                strcpy(name, ptrtp_name_prefix);
                strcpy(name + ptrtp_name_prefix_len, func_name);
                Type.tp_name = name;
                if (!inited && (PyType_Ready(&Type) < 0)) {
                    throw "Unable to initialize python object for c function wrapper";
                } else {
                    inited = true;
                    PyTypeObject *type = new PyTypeObject(Type);
                    Py_INCREF(type);
                    type->tp_name = name;
		    PyType_Ready(type);
                    auto pyfuncobj = (Wrapper *) PyObject_CallObject((PyObject *) type, nullptr);
		    if (!pyfuncobj){
		      PyErr_Print();
		      throw "Unable to create function callable";
		    }
                    pyfuncobj->_cfunc = func;
                    while (names[index]) {
                        pyfuncobj->_kwlist.push_back(names[index++]);
                    }
                    pyfuncobj->_kwlist.push_back(nullptr);
                    if (!PyCallable_Check((PyObject *) pyfuncobj)) {
                        throw "Python object is not callbable as expected!";
                    }
                    return pyfuncobj;
                }
            }

            Wrapper(){ _cfunc._cfunc = nullptr; }

            ~Wrapper() { }

        private:


            static std::string type_name;

            template<typename ...PyO>
            ReturnType callFuncBase(PyObject *args, PyObject *kwds, PyO *...pyargs) {
                if (!_cfunc._cfunc) {
                    PyErr_SetString(PyExc_RuntimeError, "Uninitialized C callable!");
                    PyErr_Print();
                    throw "Uninitialized C callable";
                }
                char format[sizeof...(Args) + 1] = {0};
                memset(format, 'O', sizeof...(Args));
                const size_t arg_count = kwds ? PyDict_Size(kwds) : 0 + args ? PyTuple_Size(args) : 0;
                const size_t kwd_count = kwds ? PyDict_Size(kwds) : 0;
                PyObject *extra_args = nullptr;
                PyObject *tuple = nullptr;

                if ((arg_count > sizeof...(Args)) && with_ellipsis) {
                    tuple = PyTuple_GetSlice(args, 0, sizeof...(Args) - kwd_count);
                    extra_args = PyTuple_GetSlice(args, sizeof...(Args) - kwd_count, arg_count);
                } else {
                    tuple = args;
                }
                if (sizeof...(Args) > 0) {
                    if (!PyArg_ParseTupleAndKeywords(tuple, kwds, format, (char **) _kwlist.data(), &pyargs...)) {
                        PyErr_Print();
                        throw "Illegal arumgnet(s)";
                    }
                }
                if (sizeof...(Args) - kwd_count - arg_count) {
                    return FunctType::call(_cfunc, *toCArgument<Args, false, PythonClassWrapper<Args> >(*pyargs)...,
                                           extra_args);
                } else {
                    return _cfunc._cfunc(*toCArgument<Args, false, PythonClassWrapper<Args> >(*pyargs)...);
                }
            }

            template<int ...S>
            ReturnType callFunc(PyObject *const tuple, PyObject *kw, container<S...> s) {
                (void) s;//only used for unpacking arguments into a list and determine the int... args to this template
                PyObject pyobjs[sizeof...(S)];
                (void) pyobjs;
                return callFuncBase(tuple, kw, &pyobjs[S]...);
            }

        public:
            FuncContainer _cfunc;
        private:
            std::vector<const char *> _kwlist;
        };
    };

    //Python definition of Type for this function wrapper
    template<bool is_base_return_complete, bool with_ellipsis, typename ReturnType, typename... Args>
    template <typename ...Throws>
    PyTypeObject PythonFunctionWrapper<is_base_return_complete, with_ellipsis, ReturnType, Args...>::Wrapper<Throws...>::Type = {
    #if PY_MAJOR_VERSION == 3
            PyVarObject_HEAD_INIT(NULL, 0)
    #else
            PyObject_HEAD_INIT(nullptr)
            0,                               /*ob_size*/
    #endif
            nullptr,                         /*tp_name*/
            sizeof(Wrapper),   /*tp_basicsize*/
            0,                               /*tp_itemsize*/
            nullptr,                         /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            nullptr,                         /*tp_getattr*/
            nullptr,                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            nullptr,                         /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            _call,                           /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT, /*tp_flags*/
            "PythonFunctionWrapper object",  /* tp_doc */
            nullptr,                         /* tp_traverse */
            nullptr,                         /* tp_clear */
            nullptr,                         /* tp_richcompare */
            0,                                 /* tp_weaklistoffset */
            nullptr,                         /* tp_iter */
            nullptr,                         /* tp_iternext */
            nullptr,                         /* tp_methods */
            nullptr,                         /* tp_members */
            nullptr,                         /* tp_getset */
            nullptr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                               /* tp_dictoffset */
            _init,  /* tp_init */
            nullptr,                         /* tp_alloc */
            _new,     /* tp_new */
            nullptr,                         /*tp_free*/ //TODO: Implement a free??
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bass*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };

    template<bool is_base_return_complete, bool with_ellipsis, typename ReturnType, typename... Args>
    template< typename ...Throws>
    std::string PythonFunctionWrapper<is_base_return_complete, with_ellipsis, ReturnType, Args...>::Wrapper<Throws...>::type_name;

    template<bool is_base_return_complete, bool with_ellispsis, typename ReturnType, typename... Args>
    template< typename ...Throws>
    int PythonFunctionWrapper<is_base_return_complete, with_ellispsis, ReturnType, Args...>::Wrapper<Throws...>::_init(PyObject *self,
                                                                                                   PyObject *args,
                                                                                                   PyObject *kwds) {
        //avoid compiler warnings (including reinterpret cast to avoid type-punned warning)
        (void) self;
        (void) args;
        (void) kwds;
        if (PyType_Ready(&Type) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to initialize Python type");
            return -1;
        }
        PyObject *const type = reinterpret_cast<PyObject *>(&Type);
        Py_INCREF(type);
        return 0;
    }

    template<bool is_base_return_complete, bool with_ellipsis, typename ReturnType, typename... Args>
    template< typename ...Throws>
    PyObject *PythonFunctionWrapper<is_base_return_complete, with_ellipsis, ReturnType, Args...>::
    Wrapper<Throws...>::_call(
            PyObject *callable_object,
            PyObject *args, PyObject *kw) {
        try {
            Wrapper &wrapper = *reinterpret_cast<Wrapper *const>(callable_object);
            typedef typename std::remove_pointer<typename extent_as_pointer<ReturnType>::type>::type BaseType;
            ReturnType result = wrapper.callFunc(args, kw, typename argGenerator<sizeof...(Args)>::type());
            const ssize_t array_size = Sizeof<BaseType>::value ? Sizeof<ReturnType>::value / Sizeof<BaseType>::value
                                                               : 1;
            return toPyObject<ReturnType>(result, false, array_size);
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);

            return nullptr;
        }
    }

    /** specialize for void returns **/
    template<bool with_ellipsis, typename ...Args>
    struct PythonFunctionWrapper<true, with_ellipsis, void, Args...> : public CommonBaseWrapper {
         template<typename ...Throws>
        struct Wrapper {
            PyObject_HEAD
            typedef typename FuncContainer<with_ellipsis, void, Args...>::template Type<0, Throws...> FuncContainer;
            static PyTypeObject Type;

            static PyObject *_call(PyObject *callable_object, PyObject *args, PyObject *kw);

            static int _init(PyObject *self, PyObject *args, PyObject *kwds);

            static PyObject *
            _new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
                (void) args;
                (void) kwds;
                PythonFunctionWrapper * self = (PythonFunctionWrapper *) type->tp_alloc(type, 0);
                return (PyObject *) self;
            }

            static void initialize_type(const char *const name) {
                type_name = name;
            }

            static bool checkType(PyObject *const obj) {
                return PyObject_TypeCheck(obj, &Type);
            }

            static
            Wrapper *create(const char *const func_name,
                            FuncContainer func,
                            const char *const names[]) {
                static bool inited = false;
                Py_ssize_t index = 0;
                PyTypeObject *type = new PyTypeObject(Type);
                Py_INCREF(type);
                char *name = new char[strlen(func_name) + 1 + ptrtp_name_prefix_len];
                strcpy(name, ptrtp_name_prefix);
                strcpy(name + ptrtp_name_prefix_len, func_name);
                type->tp_name = name;
                if (!inited && (PyType_Ready(type) < 0)) {
                    throw "Unable to initialize python object for c function wrapper";
                } else {
                    inited = true;
                    auto pyfuncobj = (Wrapper *) PyObject_CallObject((PyObject *) type, nullptr);
		    if(!pyfuncobj){
		      throw "Unable to create function wrapper";
		    }
                    pyfuncobj->_cfunc = func;
                    while (names[index]) {
                        pyfuncobj->_kwlist.push_back(names[index++]);
                    }
                    pyfuncobj->_kwlist.push_back(nullptr);
                    if (!PyCallable_Check((PyObject *) pyfuncobj)) {
                        throw "Python object is not callbable as expected!";
                    }
                    return pyfuncobj;
                }
            }

            template<typename T1, typename T2>
            friend size_t offset_of(T1 T2::*member);

        private:


            struct FunctType {

                static void call(FuncContainer func,
                                 Args... args,
                                 PyObject *extra_args) {
                    if (!extra_args || PyTuple_Size(extra_args) == 0) {
                        return func._cfunc(args...);
                    } else {
                        const ssize_t extra_args_size = PyTuple_Size(extra_args);

                        ffi_cif cif;
                        ffi_type *arg_types[sizeof...(Args) + extra_args_size] = {FFIType<Args>::type()...};
                        void *arg_values[sizeof...(Args) + extra_args_size] = {(void *) &args...};

                        // Specify the data type of each argument. Available types are defined
                        // in <ffi/ffi.h>.
                        union ArgType {
                            int intvalue;
                            long longvalue;
                            double doublevalue;
                            bool boolvalue;
                            void *ptrvalue;
                        };
                        ArgType extra_arg_values[sizeof...(Args) + extra_args_size];
                        for (size_t i = sizeof...(Args); i < sizeof...(Args) + extra_args_size; ++i) {
                            PyObject *const nextArg = PyTuple_GetItem(extra_args, i - sizeof...(Args));
                            const int subtype = getType(nextArg, arg_types[i]);
                            switch (arg_types[i]->type) {
                                case FFI_TYPE_SINT32:
                                    extra_arg_values[i].intvalue = PyInt_AsLong(nextArg);
                                    arg_values[i] = &extra_arg_values[i].intvalue;
                                    break;
                                case FFI_TYPE_SINT64:
                                    extra_arg_values[i].longvalue = PyLong_AsLong(nextArg);
                                    arg_values[i] = &extra_arg_values[i].longvalue;
                                    break;
                                case FFI_TYPE_UINT8:
                                    extra_arg_values[i].boolvalue = (nextArg == Py_True);
                                    arg_values[i] = &extra_arg_values[i].boolvalue;
                                    break;
                                case FFI_TYPE_DOUBLE:
                                    extra_arg_values[i].doublevalue = PyFloat_AsDouble(nextArg);
                                    arg_values[i] = &extra_arg_values[i].doublevalue;
                                    break;
                                case FFI_TYPE_POINTER:
                                    if (STRING_TYPE == subtype) {
                                        extra_arg_values[i].ptrvalue = const_cast<char*>(PyString_AsString(nextArg));
                                        arg_values[i] = &extra_arg_values[i].ptrvalue;
                                    } else if (COBJ_TYPE == subtype) {
                                        static const size_t offset = offset_of<ObjContainer<Arbitrary> *, PythonClassWrapper<Arbitrary> >
                                                (&PythonClassWrapper<Arbitrary>::_CObject);
                                        ObjContainer<int *> **ptrvalue =
                                                (ObjContainer<int *> **) (((char *) nextArg) + offset);
                                        extra_arg_values[i].ptrvalue = ptrvalue ? (*ptrvalue)->ptr() : nullptr;
                                    } else if (FUNC_TYPE == subtype) {
                                        typedef typename PythonFunctionWrapper<true, with_ellipsis, int, int>::template Wrapper<> wtype;
                                        typedef typename PythonFunctionWrapper<true, with_ellipsis, int, int>::template Wrapper<>::FuncContainer ftype;
                                        static const size_t offset =  offset_of<ftype,  wtype >( &wtype::_cfunc);

                                        void **ptrvalue = (void **) (((char *) nextArg) + offset);
                                        extra_arg_values[i].ptrvalue = *ptrvalue;
                                    } else {
                                        throw "Unable to convert Python object to C Object";
                                    }
                                    arg_values[i] = &extra_arg_values[i].ptrvalue;
                                    break;
                                default:
                                    throw "Python object cannot be converted to C object";
                                    break;
                            }
                        }

                        // Prepare the ffi_cif structure.
                        if ((ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI, sizeof...(Args), sizeof...(Args) + extra_args_size,
                                              &ffi_type_sint, arg_types)) != FFI_OK) {
                            throw "FFI error calling variadic function";
                        }

                        // Invoke the function.
                        ffi_arg result_small;
                        ffi_call(&cif, FFI_FN(func._cfunc), &result_small, arg_values);

                    }
                }
            };

            Wrapper() {
                _cfunc._cfunc = nullptr;
            }

            ~Wrapper() { }

            template<typename ...PyO>
            void callFuncBase(PyObject *args, PyObject *kwds, PyO *...pyargs) {
                if (!_cfunc._cfunc) {
                    PyErr_SetString(PyExc_RuntimeError, "Uninitialized C callable!");
                    PyErr_Print();
                    throw "Uninitialized C callable";
                }
                char format[sizeof...(Args) + 1] = {0};
                memset(format, 'O', sizeof...(Args));
                const size_t arg_count = kwds ? PyDict_Size(kwds) : 0 + args ? PyTuple_Size(args) : 0;
                const size_t kwd_count = kwds ? PyDict_Size(kwds) : 0;
                PyObject *extra_args = nullptr;
                PyObject *tuple = nullptr;

                if ((arg_count > sizeof...(Args)) && with_ellipsis) {
                    tuple = PyTuple_GetSlice(args, 0, sizeof...(Args) - kwd_count);
                    extra_args = PyTuple_GetSlice(args, sizeof...(Args) - kwd_count, arg_count);
                } else {
                    tuple = args;
                }
                if (!PyArg_ParseTupleAndKeywords(tuple, kwds, format, (char **) _kwlist.data(), &pyargs...)) {
                    PyErr_Print();
                    throw "Illegal arumgnet(s)";
                }
                if (sizeof...(Args) - kwd_count - arg_count) {
                    FunctType::call(_cfunc, *toCArgument<Args, false, PythonClassWrapper<Args> >(*pyargs)...,
                                    extra_args);
                } else {
                    _cfunc._cfunc(*toCArgument<Args, false, PythonClassWrapper<Args> >(*pyargs)...);
                }
            }

            template<int ...S>
            void callFunc(PyObject *const tuple, PyObject *kw, container<S...> s) {
                (void) s;//only used for unpacking arguments into a list and determine the int... args to this template
                PyObject pyobjs[sizeof...(S)];
                (void) pyobjs;
                callFuncBase(tuple, kw, &pyobjs[S]...);
            }

        public:
            FuncContainer _cfunc;
        private:
            std::vector<const char *> _kwlist;
            static std::string type_name;
        };
    };

    //Python definition of Type for this function wrapper
    template<bool with_ellipsis, typename... Args>
    template <typename ...Throws>
    PyTypeObject PythonFunctionWrapper<true, with_ellipsis, void, Args...>:: Wrapper<Throws...>::Type = {

        #if PY_MAJOR_VERSION == 3
                PyVarObject_HEAD_INIT(NULL, 0)
        #else
                PyObject_HEAD_INIT(nullptr)
                0,                         /*ob_size*/
        #endif
            nullptr,                         /*tp_name*/
            sizeof(Wrapper),   /*tp_basicsize*/
            0,                               /*tp_itemsize*/
            nullptr,                         /*tp_dealloc*/
            nullptr,                         /*tp_print*/
            nullptr,                         /*tp_getattr*/
            nullptr,                         /*tp_setattr*/
            nullptr,                         /*tp_compare*/
            nullptr,                         /*tp_repr*/
            nullptr,                         /*tp_as_number*/
            nullptr,                         /*tp_as_sequence*/
            nullptr,                         /*tp_as_mapping*/
            nullptr,                         /*tp_hash */
            _call,                           /*tp_call*/
            nullptr,                         /*tp_str*/
            nullptr,                         /*tp_getattro*/
            nullptr,                         /*tp_setattro*/
            nullptr,                         /*tp_as_buffer*/
            Py_TPFLAGS_DEFAULT, /*tp_flags*/
            "PythonFunctionWrapper object",  /* tp_doc */
            nullptr,                         /* tp_traverse */
            nullptr,                         /* tp_clear */
            nullptr,                         /* tp_richcompare */
            0,                                 /* tp_weaklistoffset */
            nullptr,                         /* tp_iter */
            nullptr,                         /* tp_iternext */
            nullptr,                         /* tp_methods */
            nullptr,                         /* tp_members */
            nullptr,                         /* tp_getset */
            nullptr,                         /* tp_base */
            nullptr,                         /* tp_dict */
            nullptr,                         /* tp_descr_get */
            nullptr,                         /* tp_descr_set */
            0,                               /* tp_dictoffset */
            _init,  /* tp_init */
            nullptr,                         /* tp_alloc */
            _new,     /* tp_new */
            nullptr,                         /*tp_free*/ //TODO: Implement a free??
            nullptr,                         /*tp_is_gc*/
            nullptr,                         /*tp_bass*/
            nullptr,                         /*tp_mro*/
            nullptr,                         /*tp_cache*/
            nullptr,                         /*tp_subclasses*/
            nullptr,                          /*tp_weaklist*/
            nullptr,                          /*tp_del*/
            0,                          /*tp_version_tag*/
    };

    template<bool with_ellipsis, typename... Args>
    template< typename ...Throws >
    std::string PythonFunctionWrapper<true, with_ellipsis, void, Args...>::Wrapper<Throws...>::type_name;


    template<bool with_ellipsis, typename... Args>
    template< typename ...Throws >
    int PythonFunctionWrapper<true, with_ellipsis, void, Args...>::Wrapper<Throws...>::
    _init(PyObject *self, PyObject *args,
                                                                         PyObject *kwds) {
        //avoid compiler warnings (including reinterpret cast to avoid type-punned warning)
        (void) self;
        (void) args;
        (void) kwds;
        if (PyType_Ready(self->ob_type) < 0) {
            PyErr_SetString(PyExc_RuntimeError, "Unable to initialize Python type");
            return -1;
        }
        Py_INCREF(self->ob_type);
        return 0;
    }

    template<bool with_ellipsis, typename... Args>
    template< typename ...Throws >
    PyObject *PythonFunctionWrapper<true, with_ellipsis, void, Args...>::Wrapper<Throws...>::
    _call(PyObject *callable_object,
                                                                               PyObject *args,
                                                                               PyObject *kw) {
        try {
            Wrapper &wrapper = *reinterpret_cast<Wrapper *const>(callable_object);
            wrapper.callFunc(args, kw, typename argGenerator<sizeof...(Args)>::type());
            return Py_None;
        } catch (const char *const msg) {
            PyErr_SetString(PyExc_RuntimeError, msg);
            PyErr_Print();
            return nullptr;
        }
    }

    template<typename func_type>
    struct PythonFunctionWrapper2;

    template<typename function_type>
    struct PythonFunctionWrapper2<function_type *> : public PythonFunctionWrapper2<function_type> {

    };

    template<typename ReturnType, typename ...Args>
    struct PythonFunctionWrapper2<ReturnType(Args...)> :
            public PythonFunctionWrapper<is_complete<ReturnType>::value, false, ReturnType, Args...>::template Wrapper<> {

    };

    template<typename ...Args>
    struct PythonFunctionWrapper2<void(Args...)> :
            public PythonFunctionWrapper<true, false, void, Args...>::template Wrapper<> {

    };

    template<typename ReturnType, typename ...Args>
    struct PythonFunctionWrapper2<ReturnType(Args... ...)> :
            public PythonFunctionWrapper<is_complete<ReturnType>::value, true, ReturnType, Args...>::template Wrapper<> {

    };

    template<typename ...Args>
    struct PythonFunctionWrapper2<void(Args... ...)> :
            public PythonFunctionWrapper<true, true, void, Args...>::template Wrapper<> {

    };

    template<typename T>
    struct PythonClassWrapper<T, typename std::enable_if<
            std::is_function<typename std::remove_pointer<T>::type>::value>::type> :
            public PythonFunctionWrapper2<typename std::remove_pointer<T>::type> {
        static PythonClassWrapper *createPy(const ssize_t arraySize,
                                               ObjContainer <T> *const cobj, const bool isAllocated,
                                               const bool inPlace,
                                               PyObject *referencing = nullptr, const size_t depth = 0) {
            static PyObject *kwds = PyDict_New();
            static PyObject *emptyargs = PyTuple_New(0);
            PyDict_SetItemString(kwds, "__internal_allow_null", Py_True);
            PyTypeObject *type_ = &PythonFunctionWrapper2<typename std::remove_pointer<T>::type>::Type;

            if (!type_->tp_name) {
                PyErr_SetString(PyExc_RuntimeError, "Uninitialized type when creating object");
                return nullptr;
            }
            PythonClassWrapper *pyobj = (PythonClassWrapper *) PyObject_Call((PyObject *) type_, emptyargs, kwds);
            pyobj->_cfunc._cfunc = *cobj->ptr();//new ObjContainerPtrProxy<T_NoRef>(cobj, isAllocated);

            //if (referencing) pyobj->_referenced = referencing;
            return pyobj;
        }
    };


}

#endif
