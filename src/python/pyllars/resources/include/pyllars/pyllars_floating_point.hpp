#ifndef __PYLLARS__FLOATING_POINT_H__
#define __PYLLARS__FLOATING_POINT_H__

#include "pyllars_classwrapper.hpp"

namespace __pyllars_internal{

    template<typename number_type>
    struct FloatingPointType;

    struct PyFloatingPtCustomBase: public CommonBaseWrapper {

        std::function<double()> asDouble;

        static PyTypeObject Type;
    };

    template<typename number_type>
    struct PyFloatingPtCustomObject: public PyFloatingPtCustomBase{
    public:
        PyObject_HEAD
        typedef typename std::remove_reference<number_type>::type number_type_basic;

        static PythonClassWrapper<number_type_basic *> *alloc(PyObject *cls, PyObject *args, PyObject *kwds);
        static PyObject* to_float(PyObject *cls, PyObject *args, PyObject *kwds);

        static PyTypeObject *getPyType(){
            if(initialize() != 0){
                return nullptr;
            }
            return &Type;
        }

        static int initialize();

        static PyObject *richcompare(PyObject *a, PyObject *b, int op);

        static __pyllars_internal::PythonClassWrapper<number_type&> *fromCObject
                ( number_type & cobj, PyObject *referencing = nullptr);

        inline static bool checkType(PyObject *const obj) {
            return PyObject_TypeCheck(obj, &Type);
        }

        static constexpr PyObject *const parent_module = nullptr;

        static PyObject *repr(PyObject *o);

        static int create(PyObject *subtype, PyObject *args, PyObject *kwds);

        explicit PyFloatingPtCustomObject() : _referenced(nullptr), _depth(0), value(0){
        }

        inline number_type_basic *get_CObject() {
            return value;
        }

        std::function<double()> asDouble;

        PyObject *_referenced;
        size_t _depth;
        number_type_basic *value;

        class Initializer : public pyllars::Initializer {
        public:
            Initializer();

            status_t set_up() override;

            static Initializer *initializer;
        };

        friend class FloatingPointType<number_type>;

    private:
        static PyMethodDef _methods[];

        static PyTypeObject Type;

    };


    template<>
    class PythonClassWrapper<float> : public PyFloatingPtCustomObject<float> {
    };

    template<>
    class PythonClassWrapper<double> : public PyFloatingPtCustomObject<double> {
    };


    template<>
    class PythonClassWrapper<float&> : public PyFloatingPtCustomObject<float&> {
    };

    template<>
    class PythonClassWrapper<double&> : public PyFloatingPtCustomObject<double&> {
    };


    template<>
    class PythonClassWrapper<const float> : public PyFloatingPtCustomObject<const float> {
    };

    template<>
    class PythonClassWrapper<const double> : public PyFloatingPtCustomObject<const double> {
    };

    template<>
    class PythonClassWrapper<const float&> : public PyFloatingPtCustomObject<const float&> {
    };

    template<>
    class PythonClassWrapper<const double&> : public PyFloatingPtCustomObject<const double&> {
    };

}
#endif