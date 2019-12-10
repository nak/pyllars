//
// Created by jrusnak on 12/10/19.
//

#ifndef PYLLARS_PYLLARS_BASE_HPP
#define PYLLARS_PYLLARS_BASE_HPP
#include "pyllars_defns.hpp"
#include "pyllars_utils.hpp"

namespace pyllars_internal{

    /**
     * Class common to all C++ wrapper classes
     **/
    struct DLLEXPORT CommonBaseWrapper {
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


            static PyTypeObject* getPyType();
            static PyTypeObject* getRawType();

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
                return (bool) PyObject_TypeCheck(obj, CommonBaseWrapper::getRawType()) && // is truly wrapping a C object
                       (self->_coreTypePtr == PythonClassWrapper<core_t>::getPyType()) && //core type match
                       (to_is_const || !to_is_reference || !self->_is_const); // logic for conversion-is-allowed
            }

            static std::map< std::pair<PyTypeObject*, PyTypeObject*>, PyObject* (*)(PyObject*)> & castMap();


            template<typename Other>
            static PyObject* reinterpret(PyObject* self){
                PyTypeObject * typ = PythonClassWrapper<Other>::getPyType();
                if (typ == self->ob_type){
                    return self;
                } else if(_castAsCArgument().count(std::pair{self->ob_type, typ}) > 0){
                    return _castAsCArgument()[std::pair{self->ob_type, typ}](self);
                }
                return nullptr;
            }

            template <typename Class, typename Other>
            static PyObject* interpret_cast(PyObject* self){
                auto self_ = (PythonClassWrapper<Class>*)self;
                auto castWrapper = reinterpret_cast<PythonClassWrapper<Other>*>(
                        PyObject_Call((PyObject*)PythonClassWrapper<Other>::getPyType(),
                                      NULL_ARGS(), nullptr));
                typedef typename std::remove_reference_t <Other> Other_NoRef;
                castWrapper->set_CObject(const_cast<Other_NoRef *>(self_->get_CObject()));
                return (PyObject*) castWrapper;
            }


            template<typename From, typename To>
            static void addCast(PyTypeObject* o, PyTypeObject* o2, PyObject*(*)(PyObject*));

            protected:
            static std::map<std::pair<PyTypeObject*, PyTypeObject*>, PyObject*(*)(PyObject*)>& _castAsCArgument();

            struct TypedProxy{

                TypedProxy(PyTypeObject* Type):_Type(Type){}

                PyTypeObject & type(){
                    return *_Type;
                }

                operator PyTypeObject&(){
                    return *_Type;
                }

                bool checkType(PyObject * obj);

                /**
                 * add a static method with a ReturnType to be available in this classes' corresponding  Python type object
                 *
                 * @templateparameter name: name of the method (as it will appear in Python, but should be same as C name)
                 * @templateparam func_type: function signature in form ReturnType(Args...)
                 * @templateparam method: pointer to method to be added
                 *
                 * @param method: the pointer to the metho to be added
                 * @param kwlist: list of keyword names of araguments
                **/
                template<const char *const name, const char *const kwlist[], typename func_type, func_type method>
                void addStaticMethod();


                /**
                 * add a method with given compile-time-known name to the contained collection
                 * @param name : name of method
                 * @param kwlist : nullptr-terminated list of parameter names
                 * @param method_t: signature of the method of form ReturnType (Class::*)(Args...)
                 * @param method: The address of method_t to be added
                 **/
                template<typename Class, const char *const name, const char* const kwlist[], typename method_t, method_t method>
                void addMethod();

                /**
                 * Add a direct python method
                 */
                void addPyMethod(PyMethodDef& methodDef, const bool is_const);

                /**
                 *  Adda a named type (a class-within-this-class), where typeobj may not have yet been
                 *  readied, but will be so during call to this's ready.  This allows us to handle a type
                 *  of chicken-and-egg situation
                 * @param name   name of type to add
                 * @param typeobj the type to add
                 */
                void addStaticType( const char* const name, PyTypeObject* (*typeobj)()){
                    _classTypes[name] = typeobj;
                }

                /**
                 * Add a bare Python Object (of C-classification) to this Type (proxy)
                 * @param name
                 * @param obj
                 */
                void addClassObject(const char* const name, PyObject* const obj){
                    _classObjects[name] = obj;
                }


                PyObject* mapGet(PyObject* self, PyObject* key );
                int mapSet(PyObject* self, PyObject* key, PyObject* value,const bool is_const);


                std::map<std::string, std::pair<std::function<PyObject*(PyObject*, PyObject*)>,
                        std::function<int(bool, PyObject*, PyObject*, PyObject*)> > >_mapMethodCollection;

                std::map<std::string, _getattrfunc > _member_getters;
                std::map<std::string, _setattrfunc > _member_setters;
                std::vector<_setattrfunc > _assigners;

                std::map<std::string, PyMethodDef> _methodCollection;
                std::map<std::string, PyMethodDef> _methodCollectionConst;

                std::vector<PyTypeObject *> _baseClasses;
                std::vector<PyTypeObject*(*)()> _childrenReadyFunctions;
                std::map<std::string, PyObject*> _classObjects;
                std::map<std::string, PyTypeObject*(*)()> _classTypes;

                std::map<OpUnaryEnum , unaryfunc> _unaryOperators;
                std::map<OpUnaryEnum , unaryfunc> _unaryOperatorsConst;
                std::map<OpBinaryEnum, binaryfunc> _binaryOperators;
                std::map<OpBinaryEnum, binaryfunc> _binaryOperatorsConst;

                private:

                PyTypeObject* _Type;
            };


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

            CommonBaseWrapper(){} // never invoked as Python allocates memory directly

            private:
            DLLIMPORT static PyTypeObject _BaseType;
    };

    template<typename T>
    struct DLLEXPORT PythonBaseWrapper: public CommonBaseWrapper{

        typedef std::remove_reference_t <T> T_bare;

        /**
         * return the C-like object associated with this Python wrapper
         */
        inline T_bare *get_CObject() const{return _CObject;}
        inline T_bare * & get_CObject(){return _CObject;}

        inline void set_CObject(T_bare * value ){_CObject = value;}

        inline static constexpr TypedProxy & getTypeProxy(){return _Type;}

        inline static bool checkType(PyObject* o){return _Type.checkType(o);}

        /**
         * Return underlying PyTypeObject, possibly uninitialized (no call to PyType_Ready is guaranteed)
         * Use sparingly
         *
         * @return  PyTypeObject associated with this class
         */
        static PyTypeObject * getRawType(){return &_Type.type();}

        static size_t offset_of_CObj(){
            return  offset_of<T*, PythonBaseWrapper >(&PythonBaseWrapper::_CObject);
        }

    private:

        static TypedProxy _Type;

        T_bare * _CObject;

    };

}

#endif //PYLLARS_PYLLARS_BASE_HPP
