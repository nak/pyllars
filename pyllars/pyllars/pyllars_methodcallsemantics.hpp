#ifndef __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H
#define __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H

#include <Python.h>

#include "pyllars_utils.hpp"
#include "pyllars_defns.hpp"

/**
* This unit defines template classes needed to contain method pointers and
* define Python-to-C call semantics for invoking class instance methods
**/

namespace __pyllars_internal {

    /**
     * class to hold referecne to a class method and define
     * method call semantics
     **/
    template<typename CClass, typename T, typename ... Args>
    class MethodCallSemantics {
    public:
        typedef typename extent_as_pointer<T>::type(CClass::*method_t)(Args...);

        static const char *const *kwlist;

        /**
         * Used for regular methods:
         */
        static PyObject *call(method_t method, CClass &self, PyObject *args, PyObject *kwds) {
            try {
                typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
                T result = call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type());
                // const ssize_t type_size = Sizeof<T_base>::value;
                const ssize_t array_size = ArraySize<T>::size;//type_size > 0 ? sizeof(result) / type_size : 1;
                return toPyObject<T>(result, false, array_size);
            } catch (const char *const msg) {
                PyErr_SetString(PyExc_RuntimeError, msg);
                return nullptr;
            }
        }

    private:

        /**
         * call that invokes method a la C:
         **/
        template<typename ...PyO>
        static typename extent_as_pointer<T>::type call_methodC(
                typename extent_as_pointer<T>::type  (CClass::*method)(Args...),
                typename std::remove_reference<CClass>::type &self,
                PyObject *args, PyObject *kwds, PyO *...pyargs) {
            static char format[sizeof...(Args) + 1] = {0};
            if (sizeof...(Args) > 0)
                memset(format, 'O', sizeof...(Args));

            if (!PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...)) {
                PyErr_Print();
                throw "Invalid arguments to method call";
            }
            T retval = (self.*method)(*toCObject<Args, false, PythonClassWrapper<Args> >(*pyargs)...);
            return retval;
        }

        /**
         * call that converts python given arguments to make C call:
         **/
        template<int ...S>
        static typename extent_as_pointer<T>::type call_methodBase(
                typename extent_as_pointer<T>::type  (CClass::*method)(Args...),
                typename std::remove_reference<CClass>::type &self,
                PyObject *args, PyObject *kwds, container<S...> s) {
            (void) s;
            PyObject pyobjs[sizeof...(Args) + 1];
            (void) pyobjs;
            return call_methodC(method, self, args, kwds, &pyobjs[S]...);

        }

    };

    template<class CClass, typename ReturnType, typename ...Args>
    const char *const *
            MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;


    /**
     * specialize for void returns:
     **/
    template<typename CClass, typename ...Args>
    class MethodCallSemantics<CClass, void, Args...> {
    public:
        typedef void(CClass::*method_t)(Args...);

        static const char *const *kwlist;

        static PyObject *toPyObj(CClass &self) {
            (void) self;
            return Py_None;
        }

        static PyObject *call(method_t method, CClass &self, PyObject *args, PyObject *kwds) {
            call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type());
            return Py_None;
        }

    private:

        template<typename ...PyO>
        static void call_methodC(void (CClass::*method)(Args...),
                                 typename std::remove_reference<CClass>::type &self,
                                 PyObject *args, PyObject *kwds,
                                 PyO *...pyargs) {

            char format[sizeof...(Args) + 1] = {0};
            if (sizeof...(Args) > 0)
                memset(format, 'O', sizeof...(Args));
            if (!PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...)) {
                PyErr_SetString(PyExc_RuntimeError, "Failed to parse argument on method call");
            } else {
                (self.*method)(*toCObject<Args, false, PythonClassWrapper<Args> >(*pyargs)...);
            }
        }

        template<int ...S>
        static void call_methodBase(void (CClass::*method)(Args...),
                                    typename std::remove_reference<CClass>::type &self,
                                    PyObject *args, PyObject *kwds,
                                    container<S...> unused) {
            (void) unused;
            PyObject pyobjs[sizeof...(Args) + 1];
            call_methodC(method, self, args, kwds, &pyobjs[S]...);
            (void) pyobjs;
        }

    };


    template<class CClass, typename ...Args>
    const char *const *
            MethodCallSemantics<CClass, void, Args...>::kwlist;


    /**
     * This class is needed to prevent ambiguities and compiler issues in add_method
     * It holds the method call and allows specialization based on
     * underlying CClass type
     **/
    template<class CClass, typename E = void>
    class MethodContainer {
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type *, PyObject *);

        template<const char *const name, typename ReturnType, typename ...Args>
        class Container {
            typedef typename extent_as_pointer<ReturnType>::type TrueReturnType;

            typedef TrueReturnType(CClass::*method_t)(Args...);

            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds);

        };
    };


    /**
     * Specialization for non-const class types
     **/
    template<class CClass>
    class MethodContainer<CClass, typename std::enable_if<
            std::is_class<CClass>::value && !std::is_const<CClass>::value>::type> {
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type *, PyObject *);

        template<const char *const name, typename ReturnType, typename ...Args>
        class Container {
        public:
            typedef typename extent_as_pointer<ReturnType>::type(CClass::*method_t)(Args...);

            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;
            static method_t method;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds) {
                if (!self) return nullptr;
                typedef PythonClassWrapper<CClass> Wrapper;
                Wrapper *_this = (Wrapper *) self;
                if (_this->template get_CObject<CClass>()) {
                    try {
                        return MethodCallSemantics<CClass, ReturnType, Args...>::call(method,
                                                                                      *_this->template get_CObject<CClass>(),
                                                                                      args, kwds);
                    } catch (...) {
                        return nullptr;
                    }
                }
                return nullptr;
            }

        };

    };

    template<typename CClass>
    template<const char *const name, typename ReturnType, typename ...Args>
    typename MethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                                         !std::is_const<CClass>::value>::type>::template Container<name, ReturnType, Args...>::method_t
            MethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                                        !std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::method;


    /**
     * Specialization for const class types
     **/
    template<class CClass>
    class MethodContainer<CClass, typename std::enable_if<
            std::is_class<CClass>::value && std::is_const<CClass>::value>::type> {
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type *, PyObject *);

        template<const char *const name, typename ReturnType, typename ...Args>
        class Container {
        public:
            typedef typename extent_as_pointer<ReturnType>::type(CClass::*method_t)(Args...);

            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;
            static method_t method;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds) {
                if (!self) return nullptr;
                PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;
                if (_this->template get_CObject<CClass>()) {
                    try {
                        return MethodCallSemantics<CClass, ReturnType, Args...>::call(method,
                                                                                      *_this->template get_CObject<CClass>(),
                                                                                      args, kwds);
                    } catch (...) {
                        return nullptr;
                    }
                }
                return nullptr;
            }

        };
    };

    template<class CClass>
    template<const char *const name, typename ReturnType, typename ...Args>
    typename MethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                                         std::is_const<CClass>::value>::type>::template Container<name, ReturnType, Args...>::method_t
            MethodContainer<CClass, typename std::enable_if<std::is_class<CClass>::value &&
                                                                        std::is_const<CClass>::value>::type>::Container<name, ReturnType, Args...>::method;


    /**
     * Class member container
     * TODO!!!!!!!! ADD A "IS_COMPLETE" TEMPL PARAM!!!!!!!!!!!!!!!!1
     **/

    template<typename T, typename E=void>
    struct ArrayHelper;

    template<typename T>
    struct ArrayHelper<T, typename std::enable_if<std::is_array<T>::value &&
                                                  is_complete<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type> {
        static ssize_t base_sizeof() {
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;
            return sizeof(T_base);
        }
    };

    template<typename T>
    struct ArrayHelper<T, typename std::enable_if<!std::is_array<T>::value &&
                                                  is_complete<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type> {
        static ssize_t base_sizeof() {
            return sizeof(T);
        }
    };

    template<typename T>
    struct ArrayHelper<T, typename std::enable_if<!is_complete<typename std::remove_pointer<typename extent_as_pointer<T>::type>::type>::value>::type> {
        static ssize_t base_sizeof() {
            return -1;
        }
    };


    template<class CClass>
    class MemberContainer {
    public:

        template<const char *const name, typename T, typename E = void>
        class Container;


        template<const char *const name, typename T>
        class Container<name, T, typename std::enable_if<
                !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T CClass_NoRef::* member_t;

            static member_t member;
            static size_t array_size;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds) {
                if (!self) return nullptr;
                typedef PythonClassWrapper<CClass> Wrapper;
                Wrapper *_this = (Wrapper *) self;

                if ((!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
                    const ssize_t base_size = ArrayHelper<T>::base_sizeof();
                    const ssize_t array_size =
                            base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                                          : UNKNOWN_SIZE;
                    if (_this->template get_CObject<CClass>()) {
                        return toPyObject<T>(_this->template get_CObject<CClass>()->*member, false, array_size);
                    }
                    PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
                    return nullptr;
                } else if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "set_to")) {
                    PyObject *pyVal = PyDict_GetItemString(kwds, "set_to");
                    if (pyVal == Py_None) {
                        PyErr_SetString(PyExc_SyntaxError, "Unexpcted None value in member setter");
                        return nullptr;
                    }
                    (_this->template get_CObject<CClass_NoRef>()->*member) =
                            *toCObject<T, false, PythonClassWrapper<T> >(*pyVal);
                } else {
                    PyErr_SetString(PyExc_SyntaxError, "Invalid argsuments to set class instance member variable in C");
                    return nullptr;
                }
                return Py_None;
            }

            static void setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
                self->*member = *toCObject<T, false, PythonClassWrapper<T> >(*pyobj);
            }
        };

        //C++ forces this since T[0] is not an array type
         template<const char *const name, typename T>
        class Container<name, T, typename std::enable_if<
                !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0>::type> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type CClass_NoRef::* member_t;

            static member_t member;
            static size_t array_size;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds) {
                if (!self) return nullptr;
                typedef PythonClassWrapper<CClass> Wrapper;
                Wrapper *_this = (Wrapper *) self;

                if ((!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
                    const ssize_t base_size = ArrayHelper<T>::base_sizeof();
                    const ssize_t array_size =
                            base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                                          : UNKNOWN_SIZE;
                    if (_this->template get_CObject<CClass>()) {
                        return toPyObject<T>(_this->template get_CObject<CClass>()->*member, false, array_size);
                    }
                    PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
                    return nullptr;
                } else if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "set_to")) {
                    PyObject *pyVal = PyDict_GetItemString(kwds, "set_to");
                    if (pyVal == Py_None) {
                        PyErr_SetString(PyExc_SyntaxError, "Unexpcted None value in member setter");
                        return nullptr;
                    }

                } else {
                    PyErr_SetString(PyExc_SyntaxError, "Invalid argsuments to set class instance member variable in C");
                    return nullptr;
                }
                return Py_None;
            }

            static void setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
                (void) self;
                (void) pyobj;
            }
        };


        template<const char *const name, typename T>
        class Container<name, T,  typename std::enable_if<
                std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef const T CClass_NoRef::* member_t;

            static member_t member;
            static size_t array_size;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds) {
                if (!self) return nullptr;
                typedef PythonClassWrapper<CClass> Wrapper;
                Wrapper *_this = (Wrapper *) self;
                if ((!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
                    if (_this->template get_CObject<CClass_NoRef>()) {
                        const ssize_t base_size = ArrayHelper<T>::base_sizeof();
                        const ssize_t array_size =
                                base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                                              : UNKNOWN_SIZE;
                        return toPyObject<T>(_this->template get_CObject<CClass_NoRef>()->*member, false, array_size);
                    }
                    PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
                    return nullptr;
                } else if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "set_to")) {
                    PyErr_SetString(PyExc_RuntimeError, "Attempt to set constant attribute!");
                    return nullptr;
                } else {
                    PyErr_SetString(PyExc_SyntaxError, "Invalid argsuments to set class instance member variable in C");
                    return nullptr;
                }
            }

            static void setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
                PyErr_SetString(PyExc_RuntimeError, "Attempt to set constant field");
            }
        };

        template<const char *const name, ssize_t size, typename T>
        class Container<name, T[size], void> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T T_array[size];
            typedef T_array CClass_NoRef::* member_t;

            static member_t member;
            static size_t array_size;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds) {
	      try{
                if (!self) return nullptr;
                PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;

                if ((!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
                    if (array_size != size) {
                        static char msg[250];
                        snprintf(msg, 250, "Mismatched array sizes (static)%lld!=%lld", (long long)array_size, (long long)size);
                        PyErr_SetString(PyExc_TypeError, msg);
                        return nullptr;
                    }
                    if (_this->template get_CObject<CClass>()) {
                        const ssize_t base_size = ArrayHelper<T_array>::base_sizeof();
                        const ssize_t array_size =
                                base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                                              : UNKNOWN_SIZE;
                        return toPyObject<T_array, size>(_this->template get_CObject<CClass>()->*member, false,
                                                         array_size);
                    }
                    PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
                    return nullptr;
                } else if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "set_to")) {
                    PyObject *pyVal = PyDict_GetItemString(kwds, "set_to");
                    if (pyVal == Py_None) {
                        PyErr_SetString(PyExc_SyntaxError, "Unexpcted None value in member setter");
                        return nullptr;
                    }
                    if (PyTuple_Check(pyVal)) {
                        if (PyTuple_Size(pyVal) == size) {
                           for (size_t i = 0; i < size; ++i)
                                (_this->template get_CObject<CClass_NoRef>()->*
                                 member)[i] = *toCObject<T, false, PythonClassWrapper<T> >(
                                        *PyTuple_GetItem(pyVal, i));
                        } else {
                            static char msg[250];
                            snprintf(msg, 250, "Mismatched array sizes (tuple)%lld!=%lld", (long long)PyTuple_Size(pyVal),
                                     (long long)size);
                            PyErr_SetString(PyExc_IndexError, msg);
                            return nullptr;
                        }
                    } else if (PythonClassWrapper<T_array>::checkType(pyVal)) {
                        T_array *val = ((PythonClassWrapper<T_array> *) pyVal)->template get_CObject<T_array>();
                        for (size_t i = 0; i < size; ++i)
                            (_this->template get_CObject<T_array>()->*member)[i] = (*val)[i];

                    } else {
                        PyErr_SetString(PyExc_TypeError, "Invalid argument type when setting array attribute");
                        return nullptr;
                    }
                } else {
                    PyErr_SetString(PyExc_SyntaxError, "Invalid arguments to set class instance member variable in C");
                    return nullptr;
                }
                return Py_None;
	      } catch(const char* const msg){
		PyErr_SetString(PyExc_SystemError, msg);
		return nullptr;
	      }
            }

            static void setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
                smart_ptr<T[size], false> val = toCObject<T[size], false, PythonClassWrapper<T[size],  size - 1> >(
                        *pyobj);
                for (size_t i = 0; i < size; ++i) {
                    (self->*member)[i] = (*val)[i];
                }
            }
        };

        template<const char *const name, ssize_t size, typename T>
        class Container<name, const T[size], void> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef const T T_array[size];
            typedef T_array CClass_NoRef::* member_t;

            static member_t member;
            static size_t array_size;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds) {
                if (!self) return nullptr;
                PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;

                if ((!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
                    const ssize_t base_size = ArrayHelper<T_array>::base_sizeof();
                    const ssize_t array_size =
                            base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                                          : UNKNOWN_SIZE;
                    if (array_size != size) {
                        PyErr_SetString(PyExc_TypeError, "Mismatched array sizes");
                        return nullptr;
                    }
                    if (_this->template get_CObject<CClass>()) {
                        return toPyObject<T_array, size>(*(_this->template get_CObject<CClass>()->*member), false,
                                                         array_size);
                    }
                    PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
                    return nullptr;
                } else {
                    PyErr_SetString(PyExc_SyntaxError, "Cannot set const array elements!");
                    return nullptr;
                }
                return Py_None;
            }

            static void setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
                smart_ptr<T[size], false> val = toCObject<T[size], false, PythonClassWrapper<T[size], size - 1> >(
                        *pyobj);
                for (size_t i = 0; i < size; ++i) {
                    (self->*member)[i] = (*val)[i];
                }
            }
        };


        template<const char *const name, typename T>
        class Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T T_array[];
            typedef T_array CClass_NoRef::* member_t;

            static member_t member;
            static size_t array_size;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds) {
                if (!self) return nullptr;
                PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass>  *) self;

                if ((!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
                    if (_this->template get_CObject<CClass>()) {
                        const ssize_t base_size = ArrayHelper<T[]>::base_sizeof();
                        const ssize_t array_size =
                                base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                                              : UNKNOWN_SIZE;
                        return toPyObject<T[]>(*(_this->template get_CObject<CClass>()->*member), false, array_size);
                    }
                    PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
                    return nullptr;
                } else if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "set_to")) {
                    PyObject *pyVal = PyDict_GetItemString(kwds, "set_to");
                    if (pyVal == Py_None) {
                        PyErr_SetString(PyExc_RuntimeError, "Unexpcted None value in member setter");
                        return nullptr;
                    }
                    if (array_size == 0) {
                        PyErr_SetString(PyExc_RuntimeError, "Attempt to set array elements on array of unknown size.");
                        return nullptr;
                    }
                    if (PyTuple_Check(pyVal)) {
                        if (PyTuple_Size(pyVal) == array_size) {
                            for (size_t i = 0; i < array_size; ++i)
                                (_this->template get_CObject<CClass_NoRef>()->*
                                 member)[i] = *toCObject<T, false, PythonClassWrapper<T> >(
                                        *PyTuple_GetItem(pyVal, i));
                        } else {
                            static char msg[250];
                            snprintf(msg, 250, "Mismatched array sizes (tuple)%lld!=%lld", (long long)PyTuple_Size(pyVal),
                                     (long long)array_size);
                            PyErr_SetString(PyExc_IndexError, msg);
                            return nullptr;
                        }
                    } else if (PythonClassWrapper<T_array>::checkType(pyVal)) {
                        T_array *val = ((PythonClassWrapper<T_array> *) pyVal)->template get_CObject<T_array>();
                        //TODO: check size????
                        for (size_t i = 0; i < array_size; ++i)
                            (_this->template get_CObject<T_array>()->*member)[i] = (*val)[i];

                    } else {
                        PyErr_SetString(PyExc_TypeError, "Invalid argument type when setting array attribute");
                        return nullptr;
                    }
                } else {
                    PyErr_SetString(PyExc_SyntaxError, "Invalid argsuments to set class instance member variable in C");
                    return nullptr;
                }
                return Py_None;
            }

            static void setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
                smart_ptr<T[], false> val = toCObject<T[], false, PythonClassWrapper<T[], -1> >(*pyobj);
                for (size_t i = 0; i < array_size; ++i) {
                    (self->*member)[i] = (*val)[i];
                }
            }
        };

        template<const char *const name, typename T>
        class Container<name, T[], typename std::enable_if<std::is_const<T>::value>::type> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef const T T_array[];
            typedef T_array CClass_NoRef::* member_t;

            static member_t member;
            static size_t array_size;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds) {
                if (!self) return nullptr;
                PythonClassWrapper<CClass> *_this = (PythonClassWrapper<CClass> *) self;

                if ((!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
                    if (_this->template get_CObject<CClass>()) {
                        const ssize_t base_size = ArrayHelper<const T[]>::base_sizeof();
                        const ssize_t array_size =
                                base_size > 0 ? sizeof(_this->template get_CObject<CClass>()->*member) / base_size
                                              : UNKNOWN_SIZE;
                        return toPyObject<const T[]>(*(_this->template get_CObject<CClass>()->*member), false,
                                                     array_size);
                    }
                    PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
                    return nullptr;
                } else {
                    PyErr_SetString(PyExc_RuntimeError, "Cannot set const array elements!");
                    return nullptr;
                }
                return Py_None;
            }

            static void setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj) {
                PyErr_SetString(PyExc_RuntimeError, "Cannot set const array elements!");
            }
        };


    };


    template<class CClass>
    template<const char *const name, typename T>
    typename MemberContainer<CClass>::template Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0>::type>::member_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value == 0>::type>::member;

    template<class CClass>
    template<const char *const name, typename T>
    typename MemberContainer<CClass>::template Container<name, T, typename std::enable_if<
            !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::member_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    !std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::member;

    template<class CClass >
    template<const char *const name, ssize_t size, typename T>
    typename MemberContainer<CClass>::template Container<name, T[size], void>::member_t
            MemberContainer<CClass>::Container<name, T[size], void>::member;

    template<class CClass>
    template<const char *const name, typename T>
    typename MemberContainer<CClass>::template Container<name, T, typename std::enable_if<
            std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::member_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type>::member;

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    typename MemberContainer<CClass>::template Container<name, const T[size], void>::member_t
            MemberContainer<CClass>::Container<name, const T[size], void>::member;

    template<class CClass>
    template<const char *const name, typename T>
    typename MemberContainer<CClass>::template Container<name, T[],  typename std::enable_if<!std::is_const<T>::value>::type>::member_t
            MemberContainer<CClass>::Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type>::member;

    template<class CClass>
    template<const char *const name, typename T>
    typename MemberContainer<CClass>::template Container<name, T[], typename std::enable_if<std::is_const<T>::value>::type>::member_t
            MemberContainer<CClass>::Container<name, T[], typename std::enable_if<std::is_const<T>::value>::type>::member;


    template<class CClass>
    template<const char *const name, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    !std::is_const<T>::value && !std::is_array<T>::value &&
                    Sizeof<T>::value == 0>::type>::array_size = 0;

    template<class CClass>
    template<const char *const name, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    !std::is_const<T>::value && !std::is_array<T>::value &&
                    Sizeof<T>::value != 0>::type>::array_size = 0;

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T[size],  void>::array_size = 0;

    template<class CClass>
    template<const char *const name, ssize_t size, typename T>
    size_t
            MemberContainer<CClass>::Container<name, const T[size], void>::array_size = 0;

    template<class CClass>
    template<const char *const name, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T, typename std::enable_if<
                    std::is_const<T>::value && !std::is_array<T>::value &&
                    Sizeof<T>::value != 0>::type>::array_size = 0;

    template<class CClass>
    template<const char *const name, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type>::array_size = 0;

    template<class CClass>
    template<const char *const name, typename T>
    size_t
            MemberContainer<CClass>::Container<name, T[], typename std::enable_if<std::is_const<T>::value>::type>::array_size = 0;


    template<typename CClass>
    class BitFieldContainer {
    public:

        template<const char *const name, typename T, const size_t bits>
        class Container {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;

            typedef std::function<T(const CClass_NoRef &)> getter_t;
            typedef std::function<T(CClass_NoRef &, const T &)> setter_t;

            static getter_t _getter;
            static setter_t _setter;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds) {
                if (!self) return nullptr;
                typedef PythonClassWrapper<CClass> Wrapper;
                Wrapper *_this = (Wrapper *) self;

                if ((!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
                    if (_this->template get_CObject<CClass>()) {
                        return toPyObject<T>(_getter(*(_this->template get_CObject<CClass>())), false, 1);
                    }
                    PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
                    return nullptr;
                } else if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "set_to")) {
                    PyObject *pyVal = PyDict_GetItemString(kwds, "set_to");
                    if (pyVal == Py_None) {
                        PyErr_SetString(PyExc_SyntaxError, "Unexpected None value in member setter");
                        return nullptr;
                    }
                    T &value = *toCObject<T, false, PythonClassWrapper<T> >(*pyVal);
                    if (!BitFieldLimits<T, bits>::is_in_bounds(value)) {
                        PyErr_SetString(PyExc_TypeError, "Value out of bounds");
                        return nullptr;
                    }
                    _setter(*(_this->template get_CObject<CClass_NoRef>()), value);
                } else {
                    PyErr_SetString(PyExc_SyntaxError, "Invalid arguments to set class instance member variable in C");
                    return nullptr;
                }
                return Py_None;
            }

            static void setFromPyObject(CClass_NoRef *self, PyObject *pyobj) {
                T &value = *toCObject<T, false, PythonClassWrapper<T> >(*pyobj);
                if (!BitFieldLimits<T, bits>::is_in_bounds(value)) {
                    throw "Value out of bounds";
                }
                _setter(*self, value);
            }

        };


        template<const char *const name, typename T, const size_t bits>
        class ConstContainer {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;

            typedef std::function<T(const CClass_NoRef &)> getter_t;

            static getter_t _getter;


            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds) {
                if (!self) return nullptr;
                typedef PythonClassWrapper<CClass> Wrapper;
                Wrapper *_this = (Wrapper *) self;

                if ((!args || PyTuple_Size(args) == 0) && (!kwds || PyDict_Size(kwds) == 0)) {
                    if (_this->template get_CObject<CClass>()) {
                        return toPyObject<T>(_getter(*(_this->template get_CObject<CClass>())), false, 1);
                    }
                    PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
                    return nullptr;
                } else if (kwds && PyDict_Size(kwds) == 1 && PyDict_GetItemString(kwds, "set_to")) {
                    PyErr_SetString(PyExc_SyntaxError, "Cannot set const bit field");
                    return nullptr;
                }
                return Py_None;
            }

            static void setFromPyObject(CClass_NoRef *self, PyObject *pyobj) {
                (void) self;
                (void) pyobj;
                throw "Cannot set const bit field";
            }

        };

    };

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    typename BitFieldContainer<CClass>::template Container<name, T, bits>::getter_t BitFieldContainer<CClass>::Container<name, T, bits>::_getter;

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    typename BitFieldContainer<CClass>::template Container<name, T, bits>::setter_t BitFieldContainer<CClass>::Container<name, T, bits>::_setter;

    template<typename CClass>
    template<const char *const name, typename T, const size_t bits>
    typename BitFieldContainer<CClass>::template ConstContainer<name, T, bits>::getter_t BitFieldContainer<CClass>::ConstContainer<name, T, bits>::_getter;

}

#endif // __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2

