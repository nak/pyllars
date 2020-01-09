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

        typedef bool (*comparison_func_t)(CommonBaseWrapper *, CommonBaseWrapper *);

        typedef size_t (*hash_t)(CommonBaseWrapper *);

        typedef const char *const cstring;

        static constexpr cstring tp_name_prefix = "[*pyllars*] ";
        static constexpr size_t tp_name_prefix_len = ct_strlen(tp_name_prefix);
        static constexpr cstring ptrtp_name_prefix = "[*pyllars:ptr*] ";
        static constexpr size_t ptrtp_name_prefix_len = ct_strlen(ptrtp_name_prefix);

        /**
         * @return the base Type Object for all pyllars-related python objects
         */
        static PyTypeObject *getBaseType();

        /**
         * @return true if obj is of a PythonClassWrapper<T> type
         */
        static bool IsClassType(PyObject *obj);

        /**
         * @return true if obj is of PythonFunctionWrapper type
         */
        static bool IsCFunctionType(PyObject *obj);

        void make_reference(PyObject *obj);

        inline PyObject *getReferenced() {
            return _referenced;
        }

        static std::map<std::pair<PyTypeObject *, PyTypeObject *>, PyObject *(*)(PyObject *)> &castMap();

        template<typename Other>
        static PyObject *reinterpret(PyObject *self) {
            PyTypeObject *typ = PythonClassWrapper<Other>::getPyType();
            PyTypeObject *thisTyp = self->ob_type;
            if (typ == thisTyp) {
                return self;
            } else if (conversions().count(std::pair(thisTyp, typ)) > 0){
                return conversions()[std::pair(thisTyp, typ)](self);
            }
            return nullptr;
        }

        typedef PyObject* (*convert_t)(PyObject*);

        static std::map< std::pair<PyTypeObject*, PyTypeObject*>, convert_t> &conversions(){
            static std::map< std::pair<PyTypeObject*, PyTypeObject*>, convert_t> _conversions;
            return _conversions;
        }

        static void addConversion(PyTypeObject* t1, PyTypeObject* t2, convert_t convert){
            if (t1 != t2){
                conversions()[std::pair(t2, t2)] = convert;
            }
        }

        template <typename Class, typename Other>
        static PyObject* interpret_cast(PyObject* self);


        static void addCast(PyTypeObject* o, PyTypeObject* o2, PyObject*(*)(PyObject*));

        size_t get_ptr_depth() const{
            return _ptr_depth;
        }

        void set_ptr_depth(size_t size){
            _ptr_depth = size;
        }

        comparison_func_t compare;
        hash_t hash;

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
            std::map<std::string, PyObject*> _classObjects;
            std::map<std::string, PyTypeObject*(*)()> _classTypes;

            std::map<OpUnaryEnum , unaryfunc> _unaryOperators;
            std::map<OpUnaryEnum , unaryfunc> _unaryOperatorsConst;
            std::map<OpBinaryEnum, binaryfunc> _binaryOperators;
            std::map<OpBinaryEnum, binaryfunc> _binaryOperatorsConst;

            private:

            PyTypeObject* _Type;
        };

        static PyObject *_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
        template<typename T>
        void populate_type_info(bool(*checkType)(PyObject* const)){
            _ptr_depth = 0;
            _is_const = std::is_const<T>::value;
            _is_volatile = std::is_volatile<T>::value;
            _is_reference = std::is_reference<T>::value;
            _is_pointer = std::is_pointer<T>::value;
            __checkType = checkType;
        }
        size_t _ptr_depth;
        bool _is_const;
        bool _is_reference;
        bool _is_volatile;
        bool _is_pointer;
        PyObject *_referenced;
        bool (*__checkType)(PyObject * typ);

        CommonBaseWrapper(){} // never invoked as Python allocates memory directly

    private:
        DLLIMPORT static PyTypeObject _BaseType;
    };

    template<typename T>
    struct Classes;

    template<typename T>
    struct DLLEXPORT PythonBaseWrapper: public CommonBaseWrapper{

        friend class Classes<T>;

        //to prevent infinite recursions in supporting an "addr" method to take address of an element,
        //we represent all point-to-pointer(-to-pointer...) as simply a point-to-pointer type, and so assert
        //that we never have a base represenetation of pointer depth greater than 2
        static_assert(ptr_depth<T>::value <= 2);

        typedef std::remove_reference_t <T> T_bare;

        typedef T_bare* storage_type;

        /**
         * return the C-like object associated with this Python wrapper
         */
        inline storage_type  get_CObject() const{return _CObject;}
        inline storage_type & get_CObject(){return _CObject;}

        inline void set_CObject(const storage_type value ){_CObject = value;}

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


        /**
          * Create an instance of underlying class based on python arguments converting them to C to call
          * the constructor
          *
          * @param kwlist: list of keyword names of the Python parameters
          * @param args: list of Python args from the Python call into the C constructor
          * @params kwds: keywoards bassed into the copnstructor
          **/
        template<typename ...Args>
        static T_bare* create(const char *const kwlist[], PyObject *args, PyObject *kwds, unsigned char* location=nullptr) ;

        /**
         * add a constructor method with given compile-time-known name to the contained collection
         *
         * @templateparams Args varidic list of templat argument types
         *
         * @param kwlist: list of keyword names of parameter names for name association
         **/
        template<const char*  const kwlist[], typename ...Args>
        static void addConstructor();

        size_t get_depth() const{return _ptrDepth;}

        void set_depth(size_t depth){_ptrDepth = depth;}

        template<typename To>
        static void addConversion();

    protected:

        template <typename T_NoRef = T> //to force instantiation only for non-reference types (references cannot be allocated)
        static storage_type allocate(PyObject *args, PyObject *kwds, unsigned char* location=nullptr);

        static std::map<std::string, const std::remove_reference_t<typename std::remove_cv_t<T>>*>& _classEnumValues(){
            static std::map<std::string, const std::remove_reference_t<typename std::remove_cv_t<T>>*> container;
            return container;
        }

        static void _free(void *self_);

        static void _dealloc(PyObject *self);

        /**
         * return Python object representing the address of the contained object
         **/
        static PyObject *addr(PyObject *self, PyObject *args) ;

    private:

        typedef T_bare* (*constructor_t)(const char *const kwlist[], PyObject *args, PyObject *kwds,
                                    unsigned char* const location);

        template<typename ...PyO>
        static bool _parsePyArgs(const char *const kwlist[], PyObject *args, PyObject *kwds, PyO *&...pyargs);


        template <typename Z>
        struct _____fake{

        };

        /**
         * in order to convert args to a parameter list of args, need to go through
         * two lower level create functions
         **/
        template<typename ...Args>
        static T_bare* _createBaseBase( unsigned char *location, argument_capture<Args> ... args);

        template<typename ...Args, int ...S >
        static T_bare* _createBase( unsigned char *location, PyObject *args, PyObject *kwds,
                               const char *const kwlist[], container<S...> unused1, _____fake<Args> *... unused2);

        typedef std::pair<const char* const*, constructor_t> ConstructorContainer;
        static std::vector<ConstructorContainer>& _constructors(){
            static std::vector<ConstructorContainer> container;
            return container;
        }


        static CommonBaseWrapper::TypedProxy _Type;

        T_bare * _CObject;
        size_t _ptrDepth;

        struct Initializer{
            Initializer();

            template <typename T_element>
            static void addPointers();
        };

        static Initializer initializer;
    };


}

#endif //PYLLARS_PYLLARS_BASE_HPP
