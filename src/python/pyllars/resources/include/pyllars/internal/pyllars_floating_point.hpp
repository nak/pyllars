#ifndef __PYLLARS__FLOATING_POINT_H__
#define __PYLLARS__FLOATING_POINT_H__

#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/pyllars_namespacewrapper.hpp"

namespace __pyllars_internal{

    template<typename number_type>
    struct FloatingPointType;

    struct PyFloatingPtCustomBase: CommonBaseWrapper{
        PyObject_HEAD
        static PyTypeObject _Type;
        std::function<double()> asDouble;
    };

    template<typename number_type>
    struct PyFloatingPtCustomObject: public PyFloatingPtCustomBase{
    public:
        typedef typename std::remove_reference<number_type>::type number_type_basic;

        static PythonClassWrapper<number_type_basic *> *alloc(PyObject *cls, PyObject *args, PyObject *kwds);
        static PyObject* to_float(PyObject *cls, PyObject *args, PyObject *kwds);

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &_Type;
        }

        inline number_type_basic *get_CObject() const {
            return _CObject;
        }

        inline static bool checkType(PyObject *const obj) {
            return PyObject_TypeCheck(obj, &_Type);
        }

        static __pyllars_internal::PythonClassWrapper<number_type> *fromCObject( number_type & cobj,
                                                                                 PyObject *referencing = nullptr);

        static constexpr auto _new = PyType_GenericNew;

        typename std::remove_const<number_type>::type& toCArgument();

        const number_type& toCArgument() const;

        void make_reference(PyObject *obj) {
            if (_referenced) { Py_DECREF(_referenced); }
            if (obj) { Py_INCREF(obj); }
            _referenced = obj;
        }

        static int initialize(){
            return _initialize(_Type);
        }

        template<typename Parent, bool enabled = std::is_base_of<CommonBaseWrapper, Parent>::value>
        static status_t ready() {
            return _initialize(_Type);
        }

        static status_t preinit(){
            static int rc = -1;
            static bool inited = false;
            if (inited) {
                return rc;
            }
            static PyObject *module = pyllars::GlobalNS::module();
            rc = PyType_Ready(CommonBaseWrapper::getPyType()) |
                 PyType_Ready(&PyFloatingPtCustomBase::_Type) |
                 PyType_Ready(&PyFloatingPtCustomObject::_Type);
            Py_INCREF(&PyFloatingPtCustomBase::_Type);
            Py_INCREF(&PyFloatingPtCustomObject::_Type);
            if (module && rc == 0) {
                PyModule_AddObject(module, __pyllars_internal::type_name<number_type>(),
                                   (PyObject *) &PyFloatingPtCustomObject::_Type);
            }
            rc |= PyType_Ready(&_Type);
            return rc;
        }


    protected:

        static PyObject *richcompare(PyObject *a, PyObject *b, int op);

        static constexpr PyObject *const parent_module = nullptr;

        static PyObject *repr(PyObject *o);

        static int _init(PyFloatingPtCustomObject *subtype, PyObject *args, PyObject *kwds);

        // all instances will be allocated a'la Python so constructor should never be invoked (no linkage should be present)
        explicit PyFloatingPtCustomObject();

        std::function<double()> asDouble;

        PyObject *_referenced;
        size_t _depth;
        number_type_basic *_CObject;

        static void _dealloc(PyObject* self){}

        static void _free(void* self){}

        static int _initialize(PyTypeObject &);

        static PyTypeObject _Type;

        friend class FloatingPointType<const number_type>;
        friend class FloatingPointType<typename std::remove_const<number_type>::type>;

    private:
        static PyMethodDef _methods[];


    };


    template<>
    class PythonClassWrapper<float> : public PyFloatingPtCustomObject<float> {
    };

    template<>
    class PythonClassWrapper<double> : public PyFloatingPtCustomObject<double> {
    };

    template<>
    class PythonClassWrapper<const float> : public PyFloatingPtCustomObject<const float> {
    };

    template<>
    class PythonClassWrapper<const double> : public PyFloatingPtCustomObject<const double> {
    };


    template<>
    class PythonClassWrapper<volatile float> : public PyFloatingPtCustomObject<volatile float> {
    };

    template<>
    class PythonClassWrapper<volatile double> : public PyFloatingPtCustomObject<volatile double> {
    };

    template<>
    class PythonClassWrapper<const volatile float> : public PyFloatingPtCustomObject<const volatile float> {
    };

    template<>
    class PythonClassWrapper<const volatile double> : public PyFloatingPtCustomObject<const volatile double> {
    };

}
#endif
