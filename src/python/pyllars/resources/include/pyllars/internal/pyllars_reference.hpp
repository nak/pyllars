#ifndef __PYLLARS__INTERNAL__REFERENCE_H
#define __PYLLARS__INTERNAL__REFERENCE_H
#include <type_traits>

#include <limits>
#include <vector>
#include <functional>

#include <Python.h>
#include <structmember.h>
#include <climits>
#include <map>

#include "pyllars/internal/pyllars_integer.hpp"
#include "pyllars/internal/pyllars_floating_point.hpp"
#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/internal/pyllars_containment.hpp"
#include "pyllars_pointer.hpp"


namespace pyllars_internal {

/*
    template<typename T>
    struct DLLEXPORT PythonClassWrapper<T&, void> : public PythonClassWrapper<T& > {

     //   typedef PythonClassWrapper_Base<T&> Base;
     //   typedef typename std::remove_pointer<typename extent_as_pointer<T>::type>::type T_base;


        static int initialize();

    //    static PyTypeObject* getPyType(){
    //        return initialize() == 0?Base::getRawType():nullptr;
    //    }

      //  static PythonClassWrapper *fromCObject(T& cobj, PyObject *referencing = nullptr);

    //    static PythonClassWrapper *fromCPointer(T& cobj, const ssize_t size, PyObject *referencing = nullptr);


      //  template<typename Base>
      //  static void addBaseClass() {
      //      PythonClassWrapper<std::remove_reference_t <T> >::template addBaseClass<Base>();
      //  }

    protected:
      //  static int _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds);

    private:
        typedef int (*set_t)(PyObject *self, char *attrname, PyObject * value);
        typedef PyObject* (*get_t)(PyObject* self,  char* attrname);


     //   PythonClassWrapper():PythonClassWrapper_Base<T&>(){
     //     //  Base::_depth = 1;
     //   }

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
            ((PythonClassWrapper*)self)->set_CObject(nullptr);
            Base::_free(self);
        }

        static void _dealloc(PyObject* self){
            ((Base*)self)->set_CObject(nullptr);
            Base::_dealloc(self);
        }

        struct Initializer{
            Initializer(){
                Init::registerInit(&PythonClassWrapper::initialize);
            }
        };

        static Initializer initializer;
    };



    template<typename T>
    struct DLLEXPORT PythonClassWrapper<T&&, void> : public PythonClassWrapper_Base<T&&> {

      //  typedef PythonClassWrapper_Base<T&&> Base;



        static int initialize(){
            static bool inited = false;
            static int status = 0;
            if (inited) return status;
            PyTypeObject* Type = Base::getRawType();
            Type->tp_basicsize = sizeof(PythonClassWrapper);
            Type->tp_itemsize = 0;
            Type->tp_dealloc = (destructor) _dealloc;
            Type->tp_getattr = (getattrfunc) getter();
            Type->tp_setattr = (setattrfunc) setter();
            Type->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES;
            Type->tp_init = (initproc) _init;
            Type->tp_free = _free;
            Type->tp_base = PythonClassWrapper_Base<T>::getRawType();

            if (Base::_initialize(*Type) == 0) {
                inited = true;
                Base::getRawType()->tp_base = PythonClassWrapper<T>::getRawType();
                return PyType_Ready(Base::getRawType());
            } else {
                PyErr_SetString(PyExc_SystemError, "Failed to initializer Python type wrapper to C object reference");
                status = -1;
                return status;
            }
        }

       // static PyTypeObject* getPyType(){
       //     return initialize() == 0?Base::getRawType():nullptr;
       // }

       // static PythonClassWrapper *fromCObject(T&& cobj, PyObject *referencing = nullptr);

      //  static PythonClassWrapper *fromCPointer(T& cobj, const ssize_t size, PyObject *referencing = nullptr);


    protected:

        static int _init(PythonClassWrapper *self, PyObject *args, PyObject *kwds);

    private:
        PythonClassWrapper():PythonClassWrapper_Base<T&&>(){
            // Base::_depth = 1;
        }

        static void _free(void* self){
            //this is a reference, so prevent base from deleting it by setting it to nullptr
            ((PythonClassWrapper*)self)->set_CObject(nullptr);
            Base::_free(self);
        }

        static void _dealloc(PythonClassWrapper* self){
            self->set_CObject(nullptr);
            Base::_dealloc((PyObject*)self);
        }

        struct Initializer{
            Initializer(){
                Init::registerInit(&PythonClassWrapper::initialize);
            }
        };

        static Initializer initializer;
    };
    */
}
#endif
