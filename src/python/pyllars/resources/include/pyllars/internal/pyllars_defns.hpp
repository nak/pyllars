#ifndef __PYLLARS_INTERNAL_DEFNS_H_
#define __PYLLARS_INTERNAL_DEFNS_H_

#include <type_traits>
#include <map>

#include <Python.h>

#include "pyllars_type_traits.hpp"


namespace __pyllars_internal {
    extern PyObject* NULL_ARGS();

    constexpr size_t ct_strlen( const char* s ) noexcept{
        return *s ? 1 + ct_strlen(s + 1) : 0;
    }

    typedef const char *const cstring;

    namespace {
        constexpr int ERROR_TYPE_MISMATCH = -2;
        constexpr ssize_t UNKNOWN_SIZE = -1;
        //basic constants (names and such)
        extern const char address_name[] = "this";
        extern const char alloc_name_[] = "new";
    }


    template<typename T>
    struct TypeInfo {
        static const char* const type_name;
    };

    template<>
    struct TypeInfo<double> {
        static constexpr cstring type_name = "c_double";
    };

    template<>
    struct TypeInfo<float> {
        static constexpr cstring type_name = "c_float";
    };

    template<>
    struct TypeInfo<signed char> {
        static constexpr cstring type_name = "c_signed_char";
    };

    template<>
    struct TypeInfo<char> {
        static constexpr cstring type_name = "c_char";
    };

    template<>
    struct TypeInfo<short> {
        static constexpr cstring type_name = "c_short";
    };

    template<>
    struct TypeInfo<int> {
        static constexpr cstring type_name = "c_int";
    };

    template<>
    struct TypeInfo<long> {
        static constexpr cstring type_name = "c_long";
    };

    template<>
    struct TypeInfo<long long> {
        static constexpr cstring type_name = "c_long_long";
    };

    template<>
    struct TypeInfo<unsigned char> {
        static constexpr cstring type_name = "c_unsigned_char";
    };

    template<>
    struct TypeInfo<unsigned short> {
        static constexpr cstring type_name = "c_unsigned_short";
    };

    template<>
    struct TypeInfo<unsigned int> {
        static constexpr cstring type_name = "c_unsigned_int";
    };

    template<>
    struct TypeInfo<unsigned long> {
        static constexpr cstring type_name = "c_unsigned_long";
    };

    template<>
    struct TypeInfo<unsigned long long> {
        static constexpr cstring type_name = "c_unsigned_long_long";
    };

    template<typename T>
    struct Types {
        static const char *const type_name() {
            if constexpr(std::is_function<T>::value){
                static const char* const name = "<function>";
                return name;
            } else {
                return TypeInfo<T>::type_name;
            }
        }
    };

    template<typename T>
    struct Types<const T> {
        static const char *const type_name() {
            static std::string name("const_");
            static bool init = false;
            if (!init) {
                name += Types<typename std::remove_const<T>::type>::type_name();
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<volatile T> {
        static const char *const type_name() {

            static std::string name("volatile_");
            static bool init = false;
            if (!init) {
                name += Types<typename std::remove_const<T>::type>::type_name();
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<const volatile T> {
        static const char *const type_name() {

            static std::string name("const_volatile_");
            static bool init = false;
            if (!init) {
                name += Types<typename std::remove_const<T>::type>::type_name();
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<T *> {
        static const char *const type_name() {

            static std::string name(Types<typename std::remove_const<T>::type>::type_name());
            static bool init = false;
            if (!init) {
                name += "*";
            }
            init = true;
            return name.c_str();

        }
    };

    template<typename T>
    struct Types<const T *> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_") + Types<typename std::remove_const<T>::type>::type_name() + "*";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<volatile T *> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("volatile_") + Types<typename std::remove_const<T>::type>::type_name() + "*";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<const volatile T *> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_volatile_") + Types<typename std::remove_const<T>::type>::type_name() + "*";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<T &> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string(Types<typename std::remove_const<T>::type>::type_name()) + "&";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<const T &> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_") + Types<typename std::remove_const<T>::type>::type_name() + "&";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<volatile T &> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("volatile") + Types<typename std::remove_const<T>::type>::type_name() + "&";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<const volatile T &> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_volatile_") + Types<typename std::remove_const<T>::type>::type_name() + "&";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<T[]> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string(Types<typename std::remove_const<T>::type>::type_name()) + "[]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<const T[]> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_") + Types<typename std::remove_const<T>::type>::type_name() + "[]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<volatile T[]> {
        static const char *const type_name() {


            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("volatile_") + Types<typename std::remove_const<T>::type>::type_name() + "[]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<const volatile T[]> {
        static const char *const type_name() {


            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_volatile") + Types<typename std::remove_const<T>::type>::type_name() + "[]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T, size_t size>
    struct Types<T[size]> {
        static const char *const type_name() {
            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string(Types<typename std::remove_const<T>::type>::type_name()) + "[" +
                  std::to_string(size) + "]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T, size_t size>
    struct Types<const T[size]> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_") + Types<typename std::remove_const<T>::type>::type_name() + "[" +
                    std::to_string(size) + "]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T, size_t size>
    struct Types<volatile T[size]> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("volatile_") + Types<typename std::remove_const<T>::type>::type_name() + "[" +
                        std::to_string(size) + "]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T, size_t size>
    struct Types<const volatile T[size]> {
        static const char *const type_name() {


            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_volatile") + Types<typename std::remove_const<T>::type>::type_name() + "[" +
                        std::to_string(size) + "]";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<T &&> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string(Types<typename std::remove_const<T>::type>::type_name()) + "&&";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<const T &&> {
        static const char *const type_name(){
            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_") + Types<typename std::remove_const<T>::type>::type_name() + "&&";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<volatile T &&> {
        static const char *const type_name() {
            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("volatile_") + Types<typename std::remove_const<T>::type>::type_name() + "&&";
            }
            init = true;
            return name.c_str();
        }
    };

    template<typename T>
    struct Types<const volatile T &&> {
        static const char *const type_name() {

            static std::string name;
            static bool init = false;
            if(!init){
                name = std::string("const_volatile_") + Types<typename std::remove_const<T>::type>::type_name() + "&&";
            }
            init = true;
            return name.c_str();
        }
    };


    template<>
    struct Types<void> {
        static const char *const type_name() {
            static constexpr cstring name = "void";
            return name;
        }
    };

    template<>
    struct Types<bool> {
        static const char *const type_name() {
            static constexpr cstring name = "c_bool";
            return name;
        }
    };

    template<typename T>
    const char *const type_name() {
        return Types<T>::type_name();
    }

    template<typename ReturnType, typename ...Args>
    struct Types<ReturnType(Args...)>{
        static const char* const type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(*)(");

                std::string arg_names[] = {Types<Args>::type_name()...};
                for (unsigned long i = 0; i < sizeof...(Args); ++i) {
                    n += arg_names[i] + std::string(",");
                }
                n += std::string(")");
            }
            return n.c_str();
        }
    };

    template<typename ReturnType, typename ...Args>
    struct Types<ReturnType(Args..., ...)>{
        static const char* type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(*)(");

                std::string arg_names[] = {Types<Args>::type_name()...};
                for (unsigned int i = 0; i < sizeof...(Args); ++i) {
                    n += arg_names[i] + std::string(",");
                }
                n += std::string(" ...)");
            }
            return n.c_str();
        }
    };


    template<typename CClass, typename ReturnType, typename ...Args>
    struct Types<ReturnType(CClass::*)(Args...)>{
        static const char* type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(") +
                        __pyllars_internal::type_name<CClass>() + std::string("*)(");

                std::string arg_names[] = {Types<Args>::type_name()...};
                for (unsigned int i = 0; i < sizeof...(Args); ++i) {
                    n += arg_names[i] + std::string(",");
                }
                n += std::string(")");
            }
            return n.c_str();
        }
    };

    template<typename CClass, typename ReturnType, typename ...Args>
    struct Types<ReturnType(CClass::*)(Args..., ...)>{
        static const char* type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(") +
                        __pyllars_internal::type_name<CClass>() + std::string("*)(");

                std::string arg_names[] = {Types<Args>::type_name()...};
                for (unsigned int i = 0; i < sizeof...(Args); ++i) {
                    n += arg_names[i] + std::string(",");
                }
                n += std::string(" ...)");
            }
            return n.c_str();
        }
    };

    template<typename CClass, typename ReturnType, typename ...Args>
    struct Types<ReturnType(CClass::*)(Args...) const>{
        static const char* type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(") +
                    __pyllars_internal::type_name<CClass>() + std::string("*)(");

                std::string arg_names[] = {Types<Args>::type_name()...};
                for (unsigned int i = 0; i < sizeof...(Args); ++i) {
                    n += arg_names[i] + std::string(",");
                }
                n += std::string(") const");
            }
            return n.c_str();
        }
    };

    template<typename CClass, typename ReturnType, typename ...Args>
    struct Types<ReturnType(CClass::*)(Args..., ...) const>{
        static const char* type_name(){
            static std::string n;
            if (n.size()==0) {
                n = std::string(__pyllars_internal::type_name<ReturnType>()) + std::string("(") +
                    __pyllars_internal::type_name<CClass>() + std::string("*)(");

                std::string arg_names[] = {Types<Args>::type_name()...};
                for (unsigned int i = 0; i < sizeof...(Args); ++i) {
                    n += arg_names[i] + std::string(",");
                }
                n += std::string(" ...) const");
            }
            return n.c_str();
        }
    };

    ///////////
    // Helper conversion functions
    //////////


    template<typename T>
    PyObject *toPyObject(T var, const ssize_t array_size) ;

    template<typename T, ssize_t size>
    PyObject *toPyObject(T var[size], const ssize_t array_size) ;


    template<typename T>
    struct argument_capture;

    template<typename T, typename E = void>
    struct PythonClassWrapper;

    template<>
    class PythonClassWrapper<const float>;


    template<>
    class PythonClassWrapper<const double>;


    /**
     * Class common to all C++ wrapper classes
     **/
    struct CommonBaseWrapper{
        PyObject_HEAD

        typedef bool (*comparison_func_t)(CommonBaseWrapper*, CommonBaseWrapper*);
        typedef size_t (*hash_t)(CommonBaseWrapper*);
        typedef const char *const cstring;

        static constexpr cstring tp_name_prefix = "[*pyllars*] ";
        static constexpr size_t tp_name_prefix_len = ct_strlen(tp_name_prefix);
        static constexpr cstring ptrtp_name_prefix = "[*pyllars:ptr*] ";
        static constexpr size_t ptrtp_name_prefix_len = ct_strlen(ptrtp_name_prefix);


        comparison_func_t compare;
        hash_t hash;


        static PyTypeObject* getPyType(){return &_BaseType;}
/*
        struct Base{
            PyObject_HEAD
            //Per Python API docs

            typedef PyTypeObject *TypePtr_t;

            static auto constexpr TypePtr = &PyBaseObject_Type;

        } baseClass;
*/

        static bool IsClassType(PyObject *obj);

        static bool IsCFunctionType(PyObject *obj);

        void make_reference(PyObject *obj);

        inline PyObject *getReferenced() {
            return _referenced;
        }

        /**
             * Check for valid conversion to type T from given Python object in use as passing as argument to C function
             * (after conversion)
             * @tparam T type to convert to
             * @param obj Python with underlying C object to convert from
             * @return true if such a conversion allowed, false otherwise
             */
        template<typename T>
        static bool checkImplicitArgumentConversion(PyObject *obj){
            static constexpr bool to_is_const = std::is_const<T>::value;
            static constexpr bool to_is_reference = std::is_reference<T>::value;
            typedef typename core_type<T>::type core_t;
            auto const self = reinterpret_cast<CommonBaseWrapper*>(obj);
            return (bool) PyObject_TypeCheck(obj, &CommonBaseWrapper::_BaseType) && // is truly wrapping a C object
                   (self->_coreTypePtr == PythonClassWrapper<core_t>::getPyType()) && //core type match
                   (to_is_const || !to_is_reference || !self->_is_const); // logic for conversion-is-allowed
        }

        static std::map< std::pair<PyTypeObject*, PyTypeObject*>, PyObject* (*)(PyObject*)> & castMap();


        template<typename Other>
        static PyObject* castToCArgument(PyObject* self){
            PyTypeObject * typ = PythonClassWrapper<Other>::getPyType();
            if (typ == self->ob_type){
                return self;
            }
            if(_castAsCArgument().count(std::pair{self->ob_type, typ}) > 0){
                return _castAsCArgument()[std::pair{self->ob_type, typ}](self);
            }
            return nullptr;
        }

        template <class Class, class Other>
        static PyObject* interpret_cast(PyObject* self){
            auto self_ = (PythonClassWrapper<Class>*)self;
            auto castWrapper = (PythonClassWrapper<Other>*) PyObject_Call((PyObject*)PythonClassWrapper<Other>::getPyType(),
                                                                            NULL_ARGS(), nullptr);
            typedef typename std::remove_reference_t <Other> Other_NoRef;
            castWrapper->set_CObject(const_cast<Other_NoRef *>(self_->get_CObject()));
            return (PyObject*) castWrapper;
        }

        static std::map<std::pair<PyTypeObject*, PyTypeObject*>, PyObject*(*)(PyObject*)>& _castAsCArgument();

    protected:



        static PyTypeObject _BaseType;

        static int
        inline __init(PyObject *self, PyObject *args, PyObject *kwds){
            return 0;
        }

        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds);


        template<typename T>
        void populate_type_info(bool(*checkType)(PyObject* const),
                PyTypeObject* const coreTypePtr){
            _is_const = std::is_const<T>::value;
            _is_volatile = std::is_volatile<T>::value;
            _is_reference = std::is_reference<T>::value;
            _is_pointer = std::is_pointer<T>::value;
            __checkType = checkType;
            _coreTypePtr = coreTypePtr;
        }

        bool _is_const;
        bool _is_reference;
        bool _is_volatile;
        bool _is_pointer;
        PyObject *_referenced;
        bool (*__checkType)(PyObject * typ);
        PyTypeObject* _coreTypePtr;
        CommonBaseWrapper(); // never invoked as Python allocates memory directly
    };

    template<typename T, bool is_array, const ssize_t array_size, typename E = void>
    PyObject *set_array_values(T values, ssize_t size, PyObject *fromTuple, PyObject *referenced);

    template<typename T, bool is_array, const ssize_t array_size, typename std::enable_if<std::is_assignable<T, T>::value>::type>
    PyObject *set_array_values(T *values, ssize_t size, PyObject *from, PyObject *referenced);

    template<>
    PyObject *set_array_values<const char **, false, -1, void>(const char **values, ssize_t size, PyObject *from,
                                                               PyObject *referenced);

    template<bool varargs, typename ReturnType, typename ...Args>
    struct FuncContainer;

    template<typename ReturnType, typename ...Args>
    struct FuncContainer<false, ReturnType, Args...> {
    public:
        template<int unused, typename ...Throws>
        struct Type {
        public:
            ReturnType (*_cfunc)(Args...);
            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct Type<unused, void> {
            ReturnType (*_cfunc)(Args...);

            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct Type<unused> {
            ReturnType (*_cfunc)(Args...);

            typedef decltype(_cfunc) func_type;
        };
    };

    template<typename ReturnType, typename ...Args>
    struct FuncContainer<true, ReturnType, Args...> {

        template<int unused, typename ...Throws>
        struct Type {
            ReturnType (*_cfunc)(Args... ...);

            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct Type<unused, void> {
            ReturnType (*_cfunc)(Args... ...);

            typedef decltype(_cfunc) func_type;
        };

        template<int unused>
        struct Type<unused> {
            ReturnType (*_cfunc)(Args... ...);

            typedef decltype(_cfunc) func_type;
        };
    };



    template<typename const_or_nonconst_char>
    const_or_nonconst_char* fromPyStringLike(PyObject* obj);


    class PyllarsException{
    public:
        explicit PyllarsException(PyObject* excType, const char* const msg):_msg(msg), _excType(excType){
        }

        inline const char* const msg() const{
            return _msg.c_str();
        }

        inline PyObject * type() const{
            return _excType;
        }

        void raise(){
            if(PyErr_Occurred()){
                PyErr_Print();
            }
            PyErr_SetString(_excType, _msg.c_str());
        }

        static void raise_internal_cpp(const char* const msg = nullptr){
            if(!msg){
                PyllarsException(PyExc_RuntimeError, "internal c++ exception thrown:  ").raise();
            } else {
                PyllarsException(PyExc_RuntimeError, msg).raise();
            }
        }

    private:
        const std::string _msg;
        PyObject* const _excType;
    };


    constexpr int ERR_PYLLARS_ON_CREATE = -1;
}
#endif
