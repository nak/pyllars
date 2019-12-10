#ifndef __PYLLARS__FLOATING_POINT_H__
#define __PYLLARS__FLOATING_POINT_H__

#include "pyllars/internal/pyllars_classwrapper.hpp"
#include "pyllars/pyllars_namespacewrapper.hpp"
#include "pyllars_base.hpp"

namespace pyllars_internal{

    template<typename number_type>
    struct DLLEXPORT FloatingPointType;

    struct DLLEXPORT PyFloatingPtCustomBase: public CommonBaseWrapper{
        PyObject_HEAD
        DLLIMPORT static PyTypeObject _Type;

        double (*toDouble)(PyFloatingPtCustomBase*);

        template<typename to>
        to reinterpret(){
            return static_cast<float>((*toDouble)(this));
        }

        static PyTypeObject* getRawType();

    };

    template<typename number_type>
    struct DLLEXPORT PyFloatingPtCustomObject: public PyFloatingPtCustomBase{
    public:
        typedef typename std::remove_reference<number_type>::type number_type_basic;
        typedef std::remove_volatile_t <number_type> nonv_number_t;

        static PythonClassWrapper<number_type_basic *> *alloc(PyObject *cls, PyObject *args, PyObject *kwds);
        static PyObject* to_float(PyObject *cls, PyObject *args, PyObject *kwds);

        static PyTypeObject *getPyType();

        static PyTypeObject *getRawType();

        inline number_type_basic *get_CObject() const {
            return _CObject;
        }

        inline void set_CObject(number_type_basic * v){
            _CObject = v;
        }

        inline static bool checkType(PyObject *const obj) {
            return PyObject_TypeCheck(obj, getPyType());
        }

        static pyllars_internal::PythonClassWrapper<number_type> *fromCObject(number_type & cobj,
                                                                              PyObject *referencing = nullptr);

        static constexpr auto _new = PyType_GenericNew;

        typename std::remove_const<number_type>::type& toCArgument();

        const number_type& toCArgument() const;

        void make_reference(PyObject *obj) {
            if (_referenced) { Py_DECREF(_referenced); }
            if (obj) { Py_INCREF(obj); }
            _referenced = obj;
        }

        static status_t initialize();

        template<typename Parent, bool enabled = std::is_base_of<CommonBaseWrapper, Parent>::value>
        static status_t ready() {
            return initialize();
        }

        static status_t preinit();

        static double convertToDouble(PyFloatingPtCustomBase* o){
            return static_cast<double>(*((PyFloatingPtCustomObject*) o)->get_CObject());
        }

        template <typename Other>
        static bool is_convertible(){
            return std::is_convertible<number_type, Other>::value;
        }

    protected:

        static PyObject *richcompare(PyObject *a, PyObject *b, int op);

        static constexpr PyObject *const parent_module = nullptr;

        static PyObject *repr(PyObject *o);

        static int _init(PyFloatingPtCustomObject *subtype, PyObject *args, PyObject *kwds);

        std::function<double()> asDouble;

        PyObject *_referenced;
        size_t _depth;
        number_type_basic *_CObject;

        static void _dealloc(PyObject* self){}

        static void _free(void* self){}

        static int _initialize(PyTypeObject &);


        friend struct FloatingPointType<const number_type>;
        friend struct FloatingPointType<typename std::remove_const<number_type>::type>;

    private:
        DLLIMPORT static PyTypeObject _Type;
        static PyMethodDef _methods[];
    };


    template<>
    struct DLLEXPORT PythonClassWrapper<float> : public PyFloatingPtCustomObject<float> {
    };

    template<>
    struct DLLEXPORT PythonClassWrapper<double> : public PyFloatingPtCustomObject<double> {
    };

    template<>
    struct DLLEXPORT PythonClassWrapper<const float> : public PyFloatingPtCustomObject<const float> {
    };

    template<>
    struct DLLEXPORT PythonClassWrapper<const double> : public PyFloatingPtCustomObject<const double> {
    };


    template<>
    struct DLLEXPORT PythonClassWrapper<volatile float> : public PyFloatingPtCustomObject<volatile float> {
    };

    template<>
    struct DLLEXPORT PythonClassWrapper<volatile double> : public PyFloatingPtCustomObject<volatile double> {
    };

    template<>
    struct DLLEXPORT PythonClassWrapper<const volatile float> : public PyFloatingPtCustomObject<const volatile float> {
    };

    template<>
    struct DLLEXPORT PythonClassWrapper<const volatile double> : public PyFloatingPtCustomObject<const volatile double> {
    };

}
#endif

