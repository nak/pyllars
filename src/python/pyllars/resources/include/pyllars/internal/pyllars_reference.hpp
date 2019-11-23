#ifndef __PYLLARS__INTERNAL__REFERENCE_H
#define __PYLLARS__INTERNAL__REFERENCE_H
#include <type_traits>

#include <limits>
#include <vector>
#include <functional>

#include <Python.h>
#include <structmember.h>
#include <climits>

#include "pyllars/internal/pyllars_integer.hpp"
#include "pyllars/internal/pyllars_floating_point.hpp"
#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_containment.hpp"
#include "pyllars_pointer.hpp"


namespace __pyllars_internal {


    template<typename T>
    struct PythonClassWrapper<T&, void> : protected PythonClassWrapper<T> {

        typedef PythonClassWrapper<T> Base;
        typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;

        PythonClassWrapper():PythonClassWrapper<T>(){
           Base::_depth = 1;
        }

        T * get_CObject() const;

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &_Type;
        }

        static int initialize();

        static PyTypeObject* getRawType(){
            return &_Type;
        }

        static bool checkType(PyObject *obj);

        static PythonClassWrapper *fromCObject(T& cobj, PyObject *referencing = nullptr);

        static PythonClassWrapper *fromCPointer(T& cobj, const ssize_t size, PyObject *referencing = nullptr);

        void make_reference(PyObject* ref){
            Base::make_reference(ref);
        }

        typename std::remove_const<T>::type& toCArgument(){
            return Base::toCArgument();
        }

        const T& toCArgument() const{
            return Base::toCArgument();
        }

        void set_CObject(T* value){
            PythonClassWrapper<T >::set_CObject(value);
        }
        /**
         * Add a base class to this Python definition (called before initialize)
         **/
        template<typename Base>
        static void addBaseClass() {
            PythonClassWrapper<std::remove_reference_t <T> >::template addBaseClass<Base>();
        }

    protected:
        static int _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds);
        static PyTypeObject _Type;

    private:
        typedef int (*set_t)(PyObject *self, char *attrname, PyObject * value);
        typedef PyObject* (*get_t)(PyObject* self,  char* attrname);


        static std::map<PyTypeObject*, PyObject*(*)(PyObject*)>& _castAsCArgument(){
            static std::map<PyTypeObject*, PyObject*(*)(PyObject*)> container;
            return container;
        }

        template<typename ...DerivedType>
        struct ForEach{

            template<typename T2>
            static void insert(){
                typedef typename std::remove_reference_t<T> T_NoRef;
                typedef typename extent_as_pointer<T_NoRef >::type T_bare;
                if constexpr (!std::is_pointer<T_bare>::value){
                    _castAsCArgument().insert(std::make_pair(PythonClassWrapper<T2>::getRawType(),
                                                             &PythonClassWrapper<T_NoRef >::template interpret_cast<T, T2>));
                }
            }

            static void init() {
                int unused[] = {(insert<DerivedType>(), 0)...};
            }
        };

        static void __initAddCArgCasts(){
            if constexpr (!std::is_const<T>::value) {
                ForEach<T, const T &, const T,  volatile T, const volatile T &, const volatile T>::init();
            } else {
                ForEach<const T &, T, volatile T, const volatile T &, const volatile T>::init();
            }
        }

        constexpr static set_t setter() {
            if constexpr (is_rich_class<T>::value){
                return PythonClassWrapper<T>::_pySetAttr;
            } else {
                return nullptr;
            }
        }

        constexpr static  get_t getter() {
            if constexpr (is_rich_class<T>::value){
                return PythonClassWrapper<T>::_pyGetAttr;
            } else {
                return nullptr;
            }
        }

        static void _free(void* self){
            //this is a reference, so prevent base from deleting it by setting it to nullptr
            ((PythonClassWrapper*)self)->_CObject = nullptr;
            Base::_free(self);
        }

        static void _dealloc(PyObject* self){
            ((PythonClassWrapper*)self)->_CObject = nullptr;
            Base::_dealloc(self);
        }
    };



    template<typename T>
    struct PythonClassWrapper<T&&, void> : protected PythonClassWrapper<T> {

        typedef PythonClassWrapper<T> Base;

        PythonClassWrapper():PythonClassWrapper<T>(){
            Base::_depth = 1;
        }

        T * get_CObject() const{
            return PythonClassWrapper<T>::get_CObject();
        }

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &_Type;
        }

        static int initialize(){return Base::_initialize(_Type);}

        static bool checkType(PyObject *obj);

        static PythonClassWrapper *fromCObject(T&& cobj, PyObject *referencing = nullptr);

        static PythonClassWrapper *fromCPointer(T& cobj, const ssize_t size, PyObject *referencing = nullptr);

        typename std::remove_const<T>::type& toCArgument() {
            return Base::toCArgument();
        }

        const T& toCArgument() const{
            return Base::toCArgument();
        }
    protected:

        static int _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds);
        static PyTypeObject _Type;

    private:

        static void _free(void* self){
            //this is a reference, so prevent base from deleting it by setting it to nullptr
            ((PythonClassWrapper*)self)->_CObject = nullptr;
            Base::_free(self);
        }

        static void _dealloc(PythonClassWrapper* self){
            self->_CObject = nullptr;
            Base::_dealloc((PyObject*)self);
        }
    };
}
#endif
