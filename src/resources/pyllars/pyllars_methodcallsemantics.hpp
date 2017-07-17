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
     * class to hold referecne to a class method and define
     * method call semantics
     **/
    template< bool is_const, bool with_ellipsis, typename CClass, typename T, typename ... Args>
    class MethodCallSemantics ;


    /**
     * class to hold referecne to a class method and define
     * method call semantics
     **/
    template<bool is_const, typename CClass, typename T, typename ... Args>
    class MethodCallSemantics<is_const, false, CClass, T, Args...> {
    public:
        typedef typename MethodTypeDef<is_const, false, CClass, T, Args...>::ReturnType ReturnType;
        typedef typename MethodTypeDef<is_const, false, CClass, T, Args...>::method_t method_t;

        static const char *const *kwlist;

        /**
         * Used for regular methods:
         */
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
     * class to hold referecne to a class method and define
     * method call semantics
     **/
    template<bool is_const, typename CClass, typename T, typename ... Args>
    class MethodCallSemantics<is_const, true, CClass, T, Args...> {
    public:
        typedef typename MethodTypeDef<is_const, true, CClass, T, Args...>::ReturnType ReturnType;
        typedef typename MethodTypeDef<is_const, true, CClass, T, Args...>::method_t method_t;

        static const char *const *kwlist;

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
    template<bool is_const, typename CClass, typename ...Args>
    class MethodCallSemantics<is_const, false, CClass, void, Args...> {
    public:
        typedef typename MethodTypeDef<is_const, false, CClass, void, Args...>::method_t method_t;

        static const char *const *kwlist;

        static PyObject *toPyObj(CClass &self) ;

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


    template<bool is_const, typename CClass, typename ...Args>
    class MethodCallSemantics<is_const, true, CClass, void, Args...> {
    public:
        typedef typename MethodTypeDef<is_const, true, CClass, void, Args...>::method_t method_t;

        static const char *const *kwlist;

        static PyObject *toPyObj(CClass &self) ;

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
    template<class CClass, bool is_const = false, typename E = void>
    class MethodContainer {
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type *, PyObject *);

        template<const char *const name, typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef typename MethodCallSemantics<is_const, false, CClass, ReturnType, Args...>::ReturnType TrueReturnType;
            typedef typename MethodCallSemantics<is_const, false, CClass, ReturnType, Args...>::method_t method_t;
            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = MethodCallSemantics<is_const, false, CClass, ReturnType, Args...>::kwlist;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds);

            static PyObject *callAsBinaryFunc(PyObject *self, PyObject *args){
                return call(self, Py_BuildValue("(O)", args), nullptr);
            }
        };
    };


    /**
     * Specialization for non-const class types
     **/
    template<class CClass, bool is_const>
    class MethodContainer<CClass, is_const, typename std::enable_if<
            std::is_class<CClass>::value && !std::is_const<CClass>::value>::type> {
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type *, PyObject *);

        template<const char *const name, typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef typename MethodCallSemantics<is_const, false, CClass, ReturnType, Args...>::method_t method_t;

            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = MethodCallSemantics<is_const, false, CClass, ReturnType, Args...>::kwlist;
            static method_t method;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds);

            static PyObject *callAsBinaryFunc(PyObject *self, PyObject *args){
                return call(self, Py_BuildValue("(O)", args), nullptr);
            }
        };

    };


    /**
     * Specialization for const class types
     **/
    template<class CClass, bool is_const>
    class MethodContainer<CClass, is_const, typename std::enable_if<
            std::is_class<CClass>::value && std::is_const<CClass>::value>::type> {
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type *, PyObject *);

        template<const char *const name, typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef typename MethodCallSemantics<is_const, false, CClass, ReturnType, Args...>::ReturnType TrueReturnType;
            typedef typename MethodCallSemantics<is_const, false, CClass, ReturnType, Args...>::method_t method_t;

            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = MethodCallSemantics<is_const, false, CClass, ReturnType, Args...>::kwlist;
            static method_t method;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds);

            static PyObject *callAsBinaryFunc(PyObject *self, PyObject *args){
                return call(self, Py_BuildValue("(O)", args), nullptr);
            }
        };
    };


    ////////////// VARARGS VERSIONS //////////////////////




    /**
     * This class is needed to prevent ambiguities and compiler issues in add_method
     * It holds the method call and allows specialization based on
     * underlying CClass type
     **/
    template<class CClass, bool is_const=false, typename E = void>
    class MethodContainerVarargs {
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type *, PyObject *);

        template<const char *const name, typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef typename MethodCallSemantics<is_const, true, CClass, ReturnType, Args...>::ReturnType TrueReturnType;
            typedef typename MethodCallSemantics<is_const, true, CClass, ReturnType, Args...>::method_t method_t;

            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = MethodCallSemantics<is_const, true, CClass, ReturnType, Args...>::kwlist;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds);

            static PyObject *callAsBinaryFunc(PyObject *self, PyObject *args){
                return call(self, Py_BuildValue("(O)", args), nullptr);
            }
        };
    };


    /**
     * Specialization for non-const class types
     **/
    template<class CClass, bool is_const>
    class MethodContainerVarargs<CClass, is_const, typename std::enable_if<
            std::is_class<CClass>::value && !std::is_const<CClass>::value>::type> {
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type *, PyObject *);

        template<const char *const name, typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef typename MethodCallSemantics<is_const, true, CClass, ReturnType, Args...>::ReturnType TrueReturnType;
            typedef typename MethodCallSemantics<is_const, true, CClass, ReturnType, Args...>::method_t method_t;

            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = MethodCallSemantics<is_const, true, CClass, ReturnType, Args...>::kwlist;
            static method_t method;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds);

            static PyObject *callAsBinaryFunc(PyObject *self, PyObject *args){
                return call(self, Py_BuildValue("(O)", args), nullptr);
            }
        };

    };


    /**
     * Specialization for const class types
     **/
    template<class CClass, bool is_const>
    class MethodContainerVarargs<CClass, is_const, typename std::enable_if<
            std::is_class<CClass>::value && std::is_const<CClass>::value>::type> {
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type *, PyObject *);

        template<const char *const name, typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef typename MethodCallSemantics<is_const, true, CClass, ReturnType, Args...>::ReturnType TrueReturnType;
            typedef typename MethodCallSemantics<is_const, true, CClass, ReturnType, Args...>::method_t method_t;


            typedef const char *const *kwlist_t;
            static constexpr kwlist_t &kwlist = MethodCallSemantics<is_const, true, CClass, ReturnType, Args...>::kwlist;
            static method_t method;

            static PyObject *call(PyObject *self, PyObject *args, PyObject *kwds);

            static PyObject *callAsBinaryFunc(PyObject *self, PyObject *args){
                return call(self, Py_BuildValue("(O)", args), nullptr);
            }
        };
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

            static int set(PyObject* o, PyObject* value);
            static PyObject* get(PyObject* o);

            static void setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj);

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

            static int set(PyObject* o,  PyObject* value);
            static PyObject* get(PyObject* o);


            static void setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj);
        };


        template<const char *const name, typename T>
        class Container<name, T, typename std::enable_if<
                std::is_const<T>::value && !std::is_array<T>::value && Sizeof<T>::value != 0>::type> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef const T CClass_NoRef::* member_t;

            static member_t member;
            static size_t array_size;

            static int set(PyObject* o, PyObject* value);
            static PyObject* get(PyObject* o);


            static void setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj);
        };

        template<const char *const name, ssize_t size, typename T>
        class Container<name, T[size], void> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T T_array[size];
            typedef T_array CClass_NoRef::* member_t;

            static member_t member;
            static size_t array_size;

            static int set(PyObject* o, PyObject* value);
            static PyObject* get(PyObject* o);

            static void setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj);

        };

        template<const char *const name, ssize_t size, typename T>
        class Container<name, const T[size], void> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef const T T_array[size];
            typedef T_array CClass_NoRef::* member_t;

            static member_t member;
            static size_t array_size;

            static int set(PyObject* o, PyObject* value);
            static PyObject* get(PyObject* o);

            static void setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj);

        };


        template<const char *const name, typename T>
        class Container<name, T[], typename std::enable_if<!std::is_const<T>::value>::type> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T T_array[];
            typedef T_array CClass_NoRef::* member_t;

            static member_t member;
            static size_t array_size;

            static int set(PyObject* o, PyObject* value);
            static PyObject* get(PyObject* o);

            static void setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj);

        };

        template<const char *const name, typename T>
        class Container<name, T[], typename std::enable_if<std::is_const<T>::value>::type> {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef const T T_array[];
            typedef T_array CClass_NoRef::* member_t;

            static member_t member;
            static size_t array_size;

            static int set(PyObject* o, PyObject* value);
            static PyObject* get(PyObject* o);


            static void setFromPyObject(typename std::remove_reference<CClass>::type *self, PyObject *pyobj);
        };


    };


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

            static int set(PyObject* o, PyObject* value);
            static PyObject* get(PyObject* o);


            static void setFromPyObject(CClass_NoRef *self, PyObject *pyobj);

        };


        template<const char *const name, typename T, const size_t bits>
        class ConstContainer {
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;

            typedef std::function<T(const CClass_NoRef &)> getter_t;

            static getter_t _getter;

            static int set(PyObject* o, PyObject* value);
            static PyObject* get(PyObject* o);

            static void setFromPyObject(CClass_NoRef *self, PyObject *pyobj);

        };

    };

}

#endif // __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2
