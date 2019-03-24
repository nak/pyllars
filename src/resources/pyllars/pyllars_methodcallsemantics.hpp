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

    template<bool is_const, bool with_ellipsis, typename CClass, typename T, typename ... Args>
    class MethodTypeDef;

    template<typename CClass, typename T, typename ... Args>
    class MethodTypeDef<false, false, CClass, T, Args...>{
    public:
        typedef typename extent_as_pointer<T>::type ReturnType;
        typedef ReturnType(CClass::*method_t)(Args...);
    };

    template<typename CClass, typename T, typename ... Args>
    class MethodTypeDef<false, true, CClass, T, Args...>{
    public:
        typedef typename extent_as_pointer<T>::type ReturnType;
        typedef ReturnType(CClass::*method_t)(Args... ...);
    };

   template<typename CClass, typename T, typename ... Args>
    class MethodTypeDef<true, false, CClass, T, Args...>{
    public:
        typedef typename extent_as_pointer<T>::type ReturnType;
        typedef ReturnType(CClass::*method_t)(Args...) const;
    };

   template<typename CClass, typename T, typename ... Args>
   class MethodTypeDef<true, true, CClass, T, Args...>{
    public:
        typedef typename extent_as_pointer<T>::type ReturnType;
        typedef ReturnType(CClass::*method_t)(Args... ...) const;
    };

   /**
     * class to hold reference to a class method and define
     * method call semantics
     **/
    template< bool is_const, bool with_ellipsis, const char* const kwlist[], typename CClass, typename T, typename ... Args>
    class MethodCallSemantics ;


    /**
     * class to hold reference to a class method and define
     * method call semantics
     **/
    template<bool is_const, const char* const kwlist[], typename CClass, typename T, typename ... Args>
    class MethodCallSemantics<is_const, false, kwlist, CClass, T, Args...> {
    public:
        typedef typename MethodTypeDef<is_const, false, CClass, T, Args...>::ReturnType ReturnType;
        typedef typename MethodTypeDef<is_const, false, CClass, T, Args...>::method_t method_t;

        /**
         * Used for regular methods:
         */
        static PyObject *call(method_t method, typename std::remove_reference<CClass>::type &self, PyObject *args, PyObject *kwds) ;

    private:

        static
        ReturnType make_call(typename std::remove_reference<CClass>::type &self,  method_t method,
                argument_capture<Args>... args){
            return (self.*method)(args.value()...);
        }

        /**
         * call that invokes method a la C:
         **/
        template<typename ...PyO>
        static typename extent_as_pointer<T>::type call_methodC(
                method_t method,
                typename std::remove_reference<CClass>::type &self,
                PyObject *args, PyObject *kwds, PyO *...pyargs);

        /**
         * call that converts python given arguments to make C call:
         **/
        template<int ...S>
        static typename extent_as_pointer<T>::type call_methodBase(
                method_t method,
                typename std::remove_reference<CClass>::type &self,
                PyObject *args, PyObject *kwds, container<S...> s);

    };


   /**
     * class to hold referecne to a class method and define
     * method call semantics
     **/
    template<bool is_const, const char* const kwlist[], typename CClass, typename T, typename ... Args>
    class MethodCallSemantics<is_const, true, kwlist, CClass, T, Args...> {
    public:
        typedef typename MethodTypeDef<is_const, true, CClass, T, Args...>::ReturnType ReturnType;
        typedef typename MethodTypeDef<is_const, true, CClass, T, Args...>::method_t method_t;

        /**
         * Used for regular methods:
         */
        static ReturnType call(method_t method, typename std::remove_reference<CClass>::type &self, Args..., PyObject *extra_args );
        static PyObject *call(method_t method, typename std::remove_reference<CClass>::type &self, PyObject *args, PyObject *kwds) ;

    private:

        /**
         * call that invokes method a la C:
         **/
        template<typename ...PyO>
        static typename extent_as_pointer<T>::type call_methodC(
                method_t method,
                typename std::remove_reference<CClass>::type &self,
                PyObject *args, PyObject *kwds, PyO *...pyargs);

        /**
         * call that converts python given arguments to make C call:
         **/
        template<int ...S>
        static typename extent_as_pointer<T>::type call_methodBase(
                method_t method,
                typename std::remove_reference<CClass>::type &self,
                PyObject *args, PyObject *kwds, container<S...> s);

    };

    /**
     * specialize for void returns:
     **/
    template<bool is_const, const char* const kwlist[], typename CClass, typename ...Args>
    class MethodCallSemantics<is_const, false, kwlist, CClass, void, Args...> {
    public:
        typedef typename MethodTypeDef<is_const, false, CClass, void, Args...>::method_t method_t;

        static PyObject *call(method_t method, typename std::remove_reference<CClass>::type &self, PyObject *args, PyObject *kwds);

    private:

        template<typename ...PyO>
        static void call_methodC(method_t method,
                                 typename std::remove_reference<CClass>::type &self,
                                 PyObject *args, PyObject *kwds,
                                 PyO *...pyargs);

        template<int ...S>
        static void call_methodBase(method_t method,
                                    typename std::remove_reference<CClass>::type &self,
                                    PyObject *args, PyObject *kwds,
                                    container<S...> unused) ;

    };


    template<bool is_const, const char* const kwlist[], typename CClass, typename ...Args>
    class MethodCallSemantics<is_const, true, kwlist,  CClass, void, Args...> {
    public:
        typedef typename MethodTypeDef<is_const, true, CClass, void, Args...>::method_t method_t;

        static void call(method_t method, typename std::remove_reference<CClass>::type &self,Args... args, PyObject *extra_args);
        static PyObject *call(method_t method, typename std::remove_reference<CClass>::type &self, PyObject *args, PyObject *kwds) ;

    private:

        template<typename ...PyO>
        static void call_methodC(method_t method,
                                 typename std::remove_reference<CClass>::type &self,
                                 PyObject *args, PyObject *kwds,
                                 PyO *...pyargs);

        template<int ...S>
        static void call_methodBase(method_t method,
                                    typename std::remove_reference<CClass>::type &self,
                                    PyObject *args, PyObject *kwds,
                                    container<S...> unused) ;

    };



    /**
     * This class is needed to prevent ambiguities and compiler issues in add_method
     * It holds the method call and allows specialization based on
     * underlying CClass type
     **/
    template<class CClass, const char* const, typename E = void>
    class MethodContainer ;

    /**
     * Specialization for non-const class types
     **/
    template<class CClass, const char *const name>
    class MethodContainer<CClass, name, typename std::enable_if<
            !std::is_const<CClass>::value>::type> {
    public:
        static constexpr cstring name_ = name;

        static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds);

        static PyObject *callAsUnaryFunc(PyObject *self, PyObject *arg){
            return call(self, arg, nullptr);
        }

        static PyObject *callAsBinaryFunc(PyObject *self, PyObject *arg){
            auto args = PyTuple_New(1);
            PyTuple_SetItem(args, 0, arg);
            return call(self, /*Py_BuildValue("(O)",*/ args, nullptr);
        }

        class BaseContainer{
        public:
            virtual PyObject* _call(PyObject* self, PyObject* args, PyObject* kwds) = 0;
        };

        template<bool is_const, const char* const kwlist[], typename ReturnType, typename ...Args>
        class Container: public BaseContainer{
        public:
            typedef typename MethodCallSemantics<is_const, false, kwlist, CClass, ReturnType, Args...>::method_t method_t;

            static void setMethod(method_t method){
                if (_method){
                    // we only use virtual call method to handle different signature in
                    //static manner, but should only ever have one method of a given signature
                    throw "Duplicate method specification";
                }
                _method = method;
                std::pair<BaseContainer*, const char* const*> pair(new Container(), kwlist);
                MethodContainer::_methods.push_back(pair);
            }

            PyObject* _call(PyObject* self, PyObject* args, PyObject* kwds) override;

        private:
            static method_t _method;
        };

        template <bool is_const, const char* const kwlist[], typename ReturnType, typename ...Args>
        friend class Container;

    protected:
        typedef std::vector<std::pair<BaseContainer*, const char* const*>> method_collection_t;
        static method_collection_t _methods;

    };


    /**
     * Specialization for const class types
     **/
    template<class CClass,const char* const name>
    class MethodContainer<CClass, name, typename std::enable_if<
            std::is_const<CClass>::value>::type> {
    public:
        static constexpr cstring name_ = name;

        static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds);

        static PyObject *callAsUnaryFunc(PyObject *self, PyObject *arg){
            return call(self, arg, nullptr);
        }

        static PyObject *callAsBinaryFunc(PyObject *self, PyObject *arg){
            auto args = PyTuple_New(1);
            PyTuple_SetItem(args, 0, arg);
            return call(self, /*Py_BuildValue("(O)",*/ args, nullptr);
        }

        class BaseContainer{
        public:
            virtual PyObject* _call(PyObject* self, PyObject* args, PyObject* kwds) = 0;
        };

        template<bool is_const, const char* const kwlist[], typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef typename MethodCallSemantics<true, false, kwlist, CClass, ReturnType, Args...>::method_t method_t;

            static void setMethod(method_t method){
                if (_method){
                    // we only use virtual call method to handle different signature in
                    //static manner, but should only ever have one method of a given signature
                    throw "Duplicate method specification";
                }
                _method = method;
                std::pair<BaseContainer*, const char* const*> pair(new Container(), kwlist);
                MethodContainer::_methods.push_back(pair);
            }

            PyObject* _call(PyObject* self, PyObject* args, PyObject* kwds) override;

        private:
            static method_t _method;
        };
        template <bool is_const, const char* const kwlist[], typename ReturnType, typename ...Args>
        friend class Container;

    protected:
        typedef std::vector<std::pair<BaseContainer*, const char* const*>> method_collection_t;
        static method_collection_t _methods;

    };


    ////////////// VARARGS VERSIONS //////////////////////




    /**
     * This class is needed to prevent ambiguities and compiler issues in add_method
     * It holds the method call and allows specialization based on
     * underlying CClass type
     **/
    template<class CClass,const char *const name, typename E = void>
    class MethodContainerVarargs;

    template<class CClass,const char *const name>
    class MethodContainerVarargs<CClass, name,
            typename std::enable_if< !std::is_const<CClass>::value>::type
            >{
    public:
        static constexpr cstring name_ = name;
        static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds);

        static PyObject *callAsUnaryFunc(PyObject *self, PyObject *arg){
            return call(self, arg, nullptr);
        }

        static PyObject *callAsBinaryFunc(PyObject *self, PyObject *arg){
            auto args = PyTuple_New(1);
            PyTuple_SetItem(args, 0, arg);
            return call(self, /*Py_BuildValue("(O)",*/ args, nullptr);
        }

        class BaseContainer{
        public:

            virtual PyObject* _call(PyObject* self, PyObject* args, PyObject* kwds) = 0;

        };

        template<bool is_const, const char* const kwlist[], typename ReturnType, typename ...Args>
        class Container: public BaseContainer{
        public:
            typedef typename MethodCallSemantics<is_const, true, kwlist, CClass, ReturnType, Args...>::method_t method_t;

            static void setMethod(method_t method){
                if (_method){
                    // we only use virtual call method to handle different signature in
                    //static manner, but should only ever have one method of a given signature
                    throw "Duplicate method specification";
                }
                _method = method;
                std::pair<BaseContainer*, const char* const*> pair(new Container(), kwlist);
                MethodContainerVarargs::_methods.push_back(pair);
            }

            PyObject* _call(PyObject* self, PyObject* args, PyObject* kwds) override;

        private:
            static method_t _method;
        };

        template<bool is_const, typename ReturnType, typename ...Args>
        friend class Container;

    protected:
        typedef std::vector<std::pair<BaseContainer*, const char* const*>> method_collection_t;
        static method_collection_t _methods;

    };


    template<class CClass,const char *const name >
    class MethodContainerVarargs<CClass, name,
        typename std::enable_if< std::is_const<CClass>::value>::type>{
    public:
        static constexpr cstring name_ = name;
        static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds);

        static PyObject *callAsUnaryFunc(PyObject *self, PyObject *arg){
            return call(self, arg, nullptr);
        }

        static PyObject *callAsBinaryFunc(PyObject *self, PyObject *arg){
            auto args = PyTuple_New(1);
            PyTuple_SetItem(args, 0, arg);
            return call(self, /*Py_BuildValue("(O)",*/ args, nullptr);
        }

        class BaseContainer{
        public:
            virtual PyObject* _call(PyObject* self, PyObject* args, PyObject* kwds) = 0;
        };

        template<bool is_const, const char* const kwlist[], typename ReturnType, typename ...Args> //is_const to keep same sig, but unused as const CClass means only const methods allowed
        class Container: public BaseContainer{
        public:
            typedef typename MethodCallSemantics<true, true, kwlist, CClass, ReturnType, Args...>::method_t method_t;


            static void setMethod(method_t method){
                if (_method){
                    // we only use virtual call method to handle different signature in
                    //static manner, but should only ever have one method of a given signature
                    throw "Duplicate method specification";
                }
                _method = method;
                std::pair<BaseContainer*, const char* const*> pair(new Container(), kwlist);
                MethodContainerVarargs::_methods.push_back(pair);
            }

            PyObject* _call(PyObject* self, PyObject* args, PyObject* kwds) override;

        private:
            static method_t _method;
        };

        template<bool is_const, typename ReturnType, typename ...Args>
        friend class Container;

    protected:
        typedef std::vector<std::pair<BaseContainer*, const char* const*>> method_collection_t;
        static method_collection_t _methods;

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

