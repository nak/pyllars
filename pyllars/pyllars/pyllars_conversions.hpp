#ifndef __PYLLARS__CONVERSIONS
#define __PYLLARS__CONVERSIONS

#include <memory>
#include <Python.h>

#include "pyllars_classwrapper.hpp"
#include "pyllars_pointer.hpp"
#include "pyllars_callbacks.hpp"
#include "pyllars_utils.hpp"

/**
 * This header file defines conversions from Python to C objects
 **/
namespace __pyllars_internal {
    namespace {
        const bool PTR_IS_ALLOCATED = true;
        const bool PTR_IS_NOT_ALLOCATED = false;
    }
    /**
     * template function to convert python to C object
     **/
    template<typename T, bool is_array, typename ClassWrapper, typename E = void>
    class CObjectConversionHelper;


    /**
     * Specialization for non-fundamental, non-array, non-function types
     **/
    template<typename T, bool is_array, typename ClassWrapper>
    class CObjectConversionHelper<T, is_array, ClassWrapper,
            typename std::enable_if< !std::is_array<T>::value &&
                                     !std::is_function<typename std::remove_pointer<T>::type>::value &&
                                     !std::is_enum<typename std::remove_reference<T>::type >::value &&
                                     !std::is_integral<typename std::remove_reference<T>::type>::value &&
                                     !std::is_floating_point<typename std::remove_reference<T>::type>::value >::type> {
    public:

        typedef typename ClassWrapper::NoRefWrapper NoRefClassWrapper;
        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper   ConstClassWrapper;

        typedef typename std::remove_reference<T>::type T_bare;

      static smart_ptr<T_bare, is_array> toCObject(PyObject &pyobj) {
            if (&pyobj == nullptr) {
                throw "Invalid argument for conversion";
            }
            // Check different pointer type matches
            if (ClassWrapper::checkType(&pyobj)) {
                return smart_ptr<T_bare, is_array>((T_bare *) reinterpret_cast<ClassWrapper *>
				    (&pyobj)->template get_CObject<T_bare>(), PTR_IS_NOT_ALLOCATED);
            } else if (NoRefClassWrapper::checkType(&pyobj)) {
                return smart_ptr<T_bare, is_array>(reinterpret_cast<NoRefClassWrapper *>(&pyobj)->
                        template get_CObject<typename std::remove_const<T_bare>::type>(), PTR_IS_NOT_ALLOCATED);
            } else if (ConstClassWrapper::checkType(&pyobj)) {
                //Must make copy of this item, since cast from const T to T requested
                //and cannot cast away const-ness
                ConstClassWrapper* self =  reinterpret_cast<ConstClassWrapper *>(&pyobj);
                T_bare* copy = (T_bare*) ObjectLifecycleHelpers::Copy<const T>::new_copy( *self->template get_CObject<const T>());
                return smart_ptr<T_bare, is_array>(copy, PTR_IS_ALLOCATED);
            } else if (NonConstClassWrapper::checkType(&pyobj)) {
	      return smart_ptr<T_bare, is_array>(reinterpret_cast<NonConstClassWrapper *>(&pyobj)->
                    template get_CObject<typename std::remove_const<T_bare>::type>(), PTR_IS_NOT_ALLOCATED);
            } else {
                fprintf(stderr, "\n");
                PyObject_Print(&pyobj, stderr, 0);
                fprintf(stderr, "\n");
                PyObject_Print(PyObject_Type(&pyobj), stderr, 0);
                fprintf(stderr, "\n");
                throw "Invalid type conversion when converting to C object";
            }
        }
    };

    /**
     * Specialization for integer types
     **/
    template<typename T, bool is_array, typename ClassWrapper>
    class CObjectConversionHelper<T, is_array, ClassWrapper, typename std::enable_if<
           std::is_integral<typename std::remove_reference<T>::type>::value
            || std::is_enum< typename std::remove_reference<T>::type>::value >::type> {
    public:

        typedef typename ClassWrapper::NoRefWrapper NoRefClassWrapper;
        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper   ConstClassWrapper;

      static smart_ptr<T, is_array> toCObject(PyObject &pyobj) {
            typedef typename std::remove_reference<typename std::remove_const<T>::type>::type T_bare;
            if (PyInt_Check(&pyobj)) {
                T_bare *value = new T_bare((T_bare) PyInt_AsLong(&pyobj));
                return smart_ptr<T, is_array>(value, PTR_IS_ALLOCATED);
            } else if (PyLong_Check(&pyobj)) {
                T_bare *value = new T_bare((T_bare) PyLong_AsLongLong(&pyobj));
                return smart_ptr<T, is_array>(value, PTR_IS_NOT_ALLOCATED);
            } else if (ClassWrapper::checkType(&pyobj)) {
                return smart_ptr<T, is_array>(reinterpret_cast<ClassWrapper * >(&pyobj)->template get_CObject<T_bare>(), PTR_IS_NOT_ALLOCATED);
            } else if (NoRefClassWrapper::checkType(&pyobj)){
                return smart_ptr<T, is_array>(reinterpret_cast<NoRefClassWrapper * >(&pyobj)->template get_CObject<T_bare>(), PTR_IS_NOT_ALLOCATED);
            }else if (ConstClassWrapper::checkType(&pyobj)){
                //Attempting to cast const T to T  which requires a copy
                ConstClassWrapper* self =  reinterpret_cast<ConstClassWrapper *>(&pyobj);
                T_bare* copy = (T_bare*) ObjectLifecycleHelpers::Copy<const T>::new_copy( *self->template get_CObject<const T>());
                return smart_ptr<T, is_array>(copy, PTR_IS_ALLOCATED);
            } else if (NonConstClassWrapper::checkType(&pyobj)){
                //casting T to const T:
                return smart_ptr<T, is_array>(reinterpret_cast<NonConstClassWrapper * >(&pyobj)->template get_CObject<std::remove_const<T_bare> >(), PTR_IS_NOT_ALLOCATED);
            }
            throw "Invalid type converting to C object";
        }
    };

    /**
     * Specialization for floating point types
     **/
    template<typename T, bool is_array, typename ClassWrapper>
    class CObjectConversionHelper<T, is_array, ClassWrapper,
            typename std::enable_if< std::is_floating_point< typename std::remove_reference<T>::type> ::value >::type > {
    public:
        typedef typename ClassWrapper::NoRefWrapper NoRefClassWrapper;
        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper   ConstClassWrapper;

        static smart_ptr<T> toCObject(PyObject &pyobj) {
            typedef typename std::remove_reference<T>::type T_bare;

            if (PyFloat_Check(&pyobj)) {
                return smart_ptr<T>(new T_bare(PyFloat_AsDouble(&pyobj)), PTR_IS_ALLOCATED);
            } else if (ClassWrapper::checkType(&pyobj)) {
                T_bare *retval = (reinterpret_cast<ClassWrapper * >(&pyobj)->template get_CObject<T_bare>());
                return smart_ptr<T>(retval, PTR_IS_NOT_ALLOCATED);
            } else if (NoRefClassWrapper::checkType(&pyobj)) {
                T_bare *retval = (reinterpret_cast<NoRefClassWrapper * >(&pyobj)->template get_CObject<T_bare>());
                return smart_ptr<T>(retval, PTR_IS_NOT_ALLOCATED);
            } else if (NonConstClassWrapper::checkType(&pyobj)) {
                T_bare *retval = (reinterpret_cast<NonConstClassWrapper * >(&pyobj)->template get_CObject<T_bare>());
                return smart_ptr<T>(retval, PTR_IS_NOT_ALLOCATED);
            }else if (ConstClassWrapper::checkType(&pyobj)) {
                //Attempting to cast const T to T  which requires a copy
                ConstClassWrapper* self =  reinterpret_cast<ConstClassWrapper *>(&pyobj);
                T_bare* copy = (T_bare*) ObjectLifecycleHelpers::Copy<const T>::new_copy( *self->template get_CObject<const T>());
                return smart_ptr<T>(copy, PTR_IS_ALLOCATED);
            }
            PyObject_Print(&pyobj, stderr, 0);
            throw "Invalid type converting to C object";
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
            return smart_ptr<callback_t, false>(retval, PTR_IS_ALLOCATED);
        }
    };

    /**
     * Specialization for callbacks
     **/
    template<typename ClassWrapper, typename ReturnType,
            typename ...Args>
    class CObjectConversionHelper<ReturnType(*)(Args..., ...), false, ClassWrapper, void> {
    public:
      typedef ReturnType(*callback_t)(Args...,...);

        static smart_ptr<callback_t, false> toCObject(PyObject &pyobj) {
            if (!PyCallable_Check(&pyobj)) {
                throw "Python callback is not callable!";
            }
            callback_t *retval = new callback_t(PyCallbackWrapperVar<ReturnType, Args...>(&pyobj).get_C_callback());
            return smart_ptr<callback_t, false>(retval, PTR_IS_ALLOCATED);
        }
    };

    /**
    * Specialization for cosnt char*
    **/
    template<typename ClassWrapper>
    class CObjectConversionHelper<const char *, false, ClassWrapper> {
    public:

        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper   ConstClassWrapper;

        static smart_ptr<const char *> toCObject(PyObject &pyobj) {
            const char *name = nullptr;
            if (ConstClassWrapper::checkType(&pyobj)) {
                ConstClassWrapper *const self_ = (ConstClassWrapper *const) &pyobj;
                name = (const char *) self_->template get_CObject<const char *const>();
            } else if (ClassWrapper::checkType(&pyobj) ) {
                ClassWrapper *const self_ = (ClassWrapper *const) &pyobj;
                name = (const char *) self_->template get_CObject<const char *>();
            }else if (NonConstClassWrapper::checkType(&pyobj)) {
                 NonConstClassWrapper *const self_ = (NonConstClassWrapper *const) &pyobj;
                name = (const char *) self_->template get_CObject<const char *>();
            } else if (PyString_Check(&pyobj)) {
                name = (const char *) PyString_AS_STRING(&pyobj);
            } else {
                PyObject_Print(&pyobj, stderr, 0);
                throw "Conversion to C string from non-string Python object!";
            }
            if (!name) { throw "Error converting string: null pointer encountered"; }

            return smart_ptr<const char *, false>( new (const char*)(name), PTR_IS_ALLOCATED);
        }
    };

    /**
     * Specialization for char* &
     **/
    template<typename ClassWrapper>
    class CObjectConversionHelper<const char *&, false, ClassWrapper> {
    public:

        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper   ConstClassWrapper;

        static smart_ptr<const char *> toCObject(PyObject &pyobj) {
            const char *name = nullptr;
            if (ConstClassWrapper::checkType(&pyobj)) {
                ConstClassWrapper *const self_ = (ConstClassWrapper *const) &pyobj;
                name = (const char *) self_->template get_CObject<const char *const>();
            } else if (ClassWrapper::checkType(&pyobj)) {
                ClassWrapper *const self_ = (ClassWrapper *const) &pyobj;
                name = (const char *) self_->template get_CObject<const char *>();
            }else if (NonConstClassWrapper::checkType(&pyobj)) {
                NonConstClassWrapper *const self_ = (NonConstClassWrapper *const) &pyobj;
                name = (const char *) self_->template get_CObject<const char *>();
            } else if (PyString_Check(&pyobj)) {
                name = (const char *) PyString_AS_STRING(&pyobj);
            } else {
                PyObject_Print(&pyobj, stderr, 0);
                throw "Conversion to C string from non-string or incompatible Python object!";
            }
            if (!name) { throw "Error converting string: null pointer encountered"; }

            return smart_ptr<const char *, false>(new (const char*)(name), PTR_IS_ALLOCATED);
        }
    };

    /**
     * Specialization for char*
     **/
    template<typename ClassWrapper>
    class CObjectConversionHelper<const char *const, false, ClassWrapper> {
    public:

        typedef typename ClassWrapper::NonConstWrapper NonConstClassWrapper;
        typedef typename ClassWrapper::ConstWrapper   ConstClassWrapper;

        static smart_ptr<const char *const, false> toCObject(PyObject &pyobj) {
            const char *name = nullptr;
            if (NonConstClassWrapper::checkType(&pyobj)) {
                NonConstClassWrapper *const self_ = ((NonConstClassWrapper *const) &pyobj);
                name = (const char *) self_->template get_CObject<const char *>();
            } else if (ClassWrapper::checkType(&pyobj)) {
                ClassWrapper *const self_ = ((ClassWrapper *const) &pyobj);
                name = (const char *) self_->template get_CObject<const char *const>();
            }else if (ConstClassWrapper::checkType(&pyobj)) {
               throw "Attempt to cast away const-ness (c-string)";
            } else if (PyString_Check(&pyobj)) {
                name = (const char *) PyString_AS_STRING(&pyobj);
            } else {
                PyObject_Print(&pyobj, stderr, 0);
                throw "Conversion to C-string from non-string Python object!";
            }
            if (!name) { throw "Error converting string: null pointer encountered"; }

            return smart_ptr<const char *const, false>(new (const char*)(name), PTR_IS_ALLOCATED);
        }
    };


    /**
     * Specialization for char*
     **/
    template<  typename ClassWrapper>
    class CObjectConversionHelper<char *const,  true, ClassWrapper> {
    public:
        static smart_ptr<char *const, true> toCObject(PyObject &pyobj) {
            const char *name = nullptr;
            if (PyString_Check(&pyobj)) {
                name = (const char *) PyString_AS_STRING(&pyobj);
            } else {
                throw "Conversiont o C stgring from non-string Python object!";
            }
            if (!name) { throw "Error converting string: null pointer encountered"; }
            //since char * contents are mutable, must make copy
            //on return
            char *new_name = new char[strlen(name) + 1];
            strcpy(new_name, name);
            return smart_ptr<char * const, true>(&new_name, PTR_IS_ALLOCATED);
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
            //since char * contents are mutable, must make copy
            //on return
            char *new_name = new char[strlen(name) + 1];
            strcpy(new_name, name);
            return smart_ptr<char *, true>(&new_name, PTR_IS_ALLOCATED);
        }
    };


    /**
     * Specialization for fixed-size array
     **/
    template<typename T, const size_t size, const bool is_array, typename ClassWrapper>
    class CObjectConversionHelper<T[size], is_array, ClassWrapper> {
    public:

        typedef T T_array[size];
        typedef const T Const_T_array[size];
        typedef typename std::remove_const<T>::type NonConst_T_array[size];

        typedef typename ClassWrapper::AsPtrWrapper PtrWrapper;
       // typedef typename ClassWrapper::DereferencedWrapper NonPtrWrapper;
        typedef typename ClassWrapper::NonConstWrapper NonConstArrayWrapper;
        typedef typename ClassWrapper::ConstWrapper ConstArrayWrapper;

        static smart_ptr<T[size], is_array> toCObject(PyObject &pyobj) {

            if (PtrWrapper::checkType(&pyobj)) {
                PtrWrapper* self = reinterpret_cast<PtrWrapper * >(&pyobj);
                if (self->_arraySize != size){
                    throw "Attempt to convert between arrays of differing sizes";
                }
                T *val_ = *self->template get_CObject<T *>();
                T_array *val = (T_array *) &val_;
                return smart_ptr<T[size], is_array>(val, PTR_IS_ALLOCATED);
            } else if (ClassWrapper::checkType(&pyobj)) {
                return smart_ptr<T_array, is_array>(
                        reinterpret_cast<ClassWrapper * >(&pyobj)->template get_CObject<T_array>(), PTR_IS_NOT_ALLOCATED);
            }  else if (NonConstArrayWrapper::checkType(&pyobj)) {
                ConstArrayWrapper * self = reinterpret_cast<ConstArrayWrapper * >(&pyobj);
                T_array * val = (T_array*) self->template get_CObject<Const_T_array>();
                return smart_ptr<T_array, is_array>(val , PTR_IS_NOT_ALLOCATED);
            } else if (ConstArrayWrapper::checkType(&pyobj)) {
                //cannot cast away const-ness so make copy on return:
                ConstArrayWrapper * self =  reinterpret_cast<ConstArrayWrapper * >(&pyobj);
                NonConst_T_array *val = new NonConst_T_array[1];
                T_array *self_value = (T_array*) self->template get_CObject<T_array>();
                for (size_t i = 0; i < size; ++i) {
                    (*val)[i] = (*self_value)[i];
                }
                return smart_ptr<T_array, is_array>( val, PTR_IS_ALLOCATED);
            } else if (PyList_Check(&pyobj)) {
                if (PyList_Size(&pyobj) != size) {
                    throw "Inconsistent sizes in array assignment";
                }
                //if (!is_array) {
                //    throw "Invalid attempt on non-array allocation of fixed-size array";
                //}
                //return a copy generated from the elements of the Python list:
                NonConst_T_array *val = new NonConst_T_array[1];
                for (size_t i = 0; i < size; ++i) {
                    PyObject *listitem = PyList_GetItem(&pyobj, i);
                    if (ClassWrapper::checkTypeDereferenced(listitem)) {
                        (*val)[i] = *(reinterpret_cast< ClassWrapper * >(listitem)->template get_CObject<T>());
                    } else {
                        delete[] val;
                        throw "Invalid type in array element assignment";
                    }
                }

                return smart_ptr<T_array, is_array>(val, PTR_IS_ALLOCATED);
            }
            throw "Conversion to C-string from non-string Python object!";

        }

    };

    /**
     * function to convert python object to underlying C type using a class helper
     **/
    template<typename T, bool is_array, typename ClassWrapper>
    smart_ptr<typename std::remove_reference<T>::type, is_array> toCObject(PyObject &pyobj) {
        return CObjectConversionHelper<typename std::remove_reference<T>::type, is_array, ClassWrapper>::toCObject(pyobj);
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
                typename std::enable_if< !std::is_integral<T>::value &&
                                         !std::is_enum<T>::value &&
                                         !std::is_floating_point<T>::value  >::type> {
        public:
            typedef typename std::remove_reference<T>::type T_NoRef;

	  static PyObject *toPyObject(T_NoRef &var, const bool asReference, const ssize_t array_size = -1, const size_t depth = ptr_depth<T>::value) {
	    ObjContainer<T_NoRef> * const ref = (asReference?new ObjContainerPtrProxy<T_NoRef, true>(&var, false):ObjectLifecycleHelpers::Copy<T>::new_copy(var));
	    PyObject* pyobj = (PyObject *) ClassWrapper::createPy(array_size, ref, !asReference, nullptr, depth);
                if (!pyobj || !ClassWrapper::checkType(pyobj)) {
                    PyErr_Format(PyExc_TypeError, "Unable to convert C type object to Python object %s: %s", pyobj->ob_type->tp_name, ClassWrapper::get_name().c_str());
                    pyobj = nullptr;
                }
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
            static PyObject *toPyObject(const T &var, const bool asReference, const ssize_t array_size = -1, const size_t depth = 1) {
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
            static PyObject *toPyObject(const T &var, const bool asReference, const ssize_t array_size = -1, const size_t depth = 1) {
                (void) asReference;
                return PyFloat_FromDouble(var);
            }
        };

        /**
         * Specialized for char*:
         **/
        template<const ssize_t max, typename ClassWrapper>
        class PyObjectConversionHelper<const char *, ClassWrapper, max, void> {
        public:
            static PyObject *toPyObject(const char *const &var, const bool asReference, const ssize_t array_size = -1, const size_t depth = 1) {
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
            static PyObject *toPyObject(char *const &var, const bool asReference, const ssize_t array_size = -1, const size_t depth = 1) {
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
            static PyObject *toPyObject(const char *const &var, const bool asReference, const ssize_t array_size = -1, const size_t depth = 1) {
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
    template<typename T,  const ssize_t max, typename E>
    PyObject *toPyObject(T &var, const bool asArgument, const ssize_t array_size, const size_t depth ) {
        return ConversionHelpers::PyObjectConversionHelper<T, PythonClassWrapper<T,   E>, max>::toPyObject(
													      var, asArgument, array_size, depth);
    }

    template<typename T, const ssize_t max, typename E>
    PyObject *toPyObject(const T &var, const bool asArgument, const ssize_t array_size, const size_t depth) {
        return ConversionHelpers::PyObjectConversionHelper<const T, PythonClassWrapper<const T, E>, max>::toPyObject(
                var, asArgument, array_size);
    }

}

#endif