#ifndef __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H
#define __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H

#include <Python.h>

#include "pyllars/internal/pyllars_conversions.hpp"
#include "pyllars/internal/pyllars_funttraits.hpp"
#include "pyllars/internal/pyllars_defns.hpp"

/**
* This unit defines template classes needed to contain method pointers and
* define Python-to-C call semantics for invoking class instance methods
**/

namespace __pyllars_internal {


    template<typename CClass, typename RType, typename ...Args>
    struct func_traits<RType(CClass::*)(Args...)>{
        constexpr static bool has_ellipsis = false;
        constexpr static ssize_t argsize = sizeof...(Args);
        constexpr static bool is_const_method = false;

        typedef RType(CClass::*type)(Args...);
        typedef RType ReturnType;
        typedef CClass class_type;

        template<typename Arg>
        struct PyObjectPack{
            typedef PyObject* type;
        };

        inline static ReturnType invoke(type &method, CClass & self, PyObject* extra_args_tuple, typename PyObjectPack<Args>::type... pyargs){
            return (self.*method)(std::forward<Args>(__pyllars_internal::toCArgument<Args>(*pyargs).value())...);
        }

        const static std::string type_name(){

            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(*")  +
                    + __pyllars_internal::type_name<CClass>() + std::string(")(");

                std::vector<std::string> arg_names{Types<Args>::type_name() + std::string(",")...};
                for (auto &name: arg_names){
                    n += name;
                }
                n += std::string(")");
            }
            return n.c_str();
        }

    };

    template<typename CClass, typename RType, typename ...Args>
    struct func_traits<RType(CClass::*)(Args..., ...)>{
        constexpr static bool has_ellipsis = true;
        constexpr static bool is_const_method = false;
        constexpr static ssize_t argsize = sizeof...(Args);

        typedef RType(CClass::*type)(Args..., ...);
        typedef RType ReturnType;
        typedef CClass class_type;

        const static std::string type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(*")  +
                    + __pyllars_internal::type_name<CClass>() + std::string(")(");

                std::vector<std::string> arg_names{Types<Args>::type_name() + std::string(",")...};
                for (auto &name: arg_names){
                    n += name;
                }
                n += std::string(" ...)");
            }
            return n.c_str();
        }

        template<typename Arg>
        struct PyObjectPack{
            typedef PyObject* type;
        };

        static ReturnType invoke(type &method, CClass & self, PyObject* extra_args_tuple,
                typename PyObjectPack<Args>::type... pyargs);
    };


    template<typename CClass, typename RType, typename ...Args>
    struct func_traits<RType(CClass::*)(Args...) const>{
        constexpr static bool has_ellipsis = false;
        constexpr static ssize_t argsize = sizeof...(Args);
        constexpr static bool is_const_method = true;

        typedef RType(CClass::*type)(Args...) const;
        typedef RType ReturnType;
        typedef CClass class_type;

        template<typename Arg>
        struct PyObjectPack{
            typedef PyObject* type;
        };

        class A;

        inline static ReturnType invoke(type &method, const CClass & self, PyObject* extra_args_tuple, typename PyObjectPack<Args>::type... pyargs){
            return (self.* method)(toCArgument<Args>(*pyargs).value()...);
        }
        const static std::string type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(*")  +
                        + __pyllars_internal::type_name<CClass>() + std::string(")(");

                std::vector<std::string> arg_names{Types<Args>::type_name() + std::string(",")...};
                for (auto &name: arg_names){
                    n += name;
                }
                n += std::string(") const");
            }
            return n.c_str();
        }

    };

    template<typename CClass, typename RType, typename ...Args>
    struct func_traits<RType(CClass::*)(Args..., ...) const>{
        constexpr static bool has_ellipsis = true;
        constexpr static bool is_const_method = true;
        constexpr static ssize_t argsize = sizeof...(Args);
        typedef CClass class_type;

        typedef RType(CClass::*type)(Args..., ...) const;
        typedef RType ReturnType;

        template<typename Arg>
        struct PyObjectPack{
            typedef PyObject* type;
        };

        static ReturnType invoke(type &method, const CClass & self, PyObject* extra_args_tuple,
                typename PyObjectPack<Args>::type... pyargs);

        const static std::string type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(*")  +
                    + __pyllars_internal::type_name<CClass>() + std::string(")(");

                std::vector<std::string> arg_names{Types<Args>::type_name() + std::string(",")...};
                for (auto &name : arg_names){
                    n += name;
                }
                n += std::string(" ...) const");
            }
            return n.c_str();
        }

    };


    /**
     * class to hold reference to a class method and define
     * method call semantics
     *
     * @param kwlist: null-terminated list of parameter names of method arguments
     * @param method_t: type for method to be invokde of form ReturnType(Class::*)(Args...)
     **/
    template< const char* const kwlist[],  typename method_t>
    class MethodCallSemantics{
    public:
        typedef typename func_traits<method_t>::ReturnType ReturnType;
        typedef typename func_traits<method_t>::class_type CClass;

        static PyObject *call(CClass &self, method_t method, PyObject *args, PyObject *kwds) ;

    protected:

        /**
         * call that invokes method a la C:
         **/
        template<typename ...PyO>
        static ReturnType call_methodC(CClass &self, method_t method, PyObject *args, PyObject *kwds, PyO *...pyargs);

        /**
         * call that converts python given arguments to make C call:
         **/
        template<int ...S>
        static ReturnType call_methodBase(CClass &self, method_t method, PyObject *args, PyObject *kwds, container<S...> s);

    };


    /**
     * Specialization for non-const class types
     **/
    template<const char *const kwlist[], typename method_t, method_t method>
    class MethodContainer{
    public:
        typedef typename func_traits<method_t>::class_type CClass;

        static constexpr cstring type_name = func_traits<method_t>::type_name();

        static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds){
            PyTypeObject * baseTyp = PythonClassWrapper<CClass>::getPyType();
            PyTypeObject * derivedTyp = self->ob_type;
            auto key = std::pair{baseTyp, derivedTyp};
            if (CommonBaseWrapper::castMap().count(key) == 0) {
                CClass *this_ = reinterpret_cast<PythonClassWrapper<CClass> *>(self)->get_CObject();
                return MethodCallSemantics<kwlist, method_t>::call(*this_, method, args, kwds);
            } else {
                call(CommonBaseWrapper::castMap()[key](self), args, kwds);
            }
        }

        static PyObject *callAsUnaryFunc(PyObject *self){
            static auto emptyargs = PyTuple_New(0);
            return call(self, emptyargs, nullptr);
        }

        static PyObject *callAsBinaryFunc(PyObject *self, PyObject *arg){
            auto args = PyTuple_New(1);
            PyTuple_SetItem(args, 0, arg);
            auto retval = call(self, /*Py_BuildValue("(O)",*/ args, nullptr);
            Py_INCREF(arg); // because tuple setitem steals one
            Py_DECREF(args);
            return retval;
        }


    };



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


}

#endif // __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2

