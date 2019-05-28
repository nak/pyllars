#ifndef __PYLLARS__FLOATING_POINT_H__
#define __PYLLARS__FLOATING_POINT_H__

#include "pyllars_classwrapper.hpp"

namespace __pyllars_internal{

    template<typename number_type>
    struct FloatingPointType;

    struct PyFloatingPtCustomBase: public CommonBaseWrapper {

        std::function<double()> asDouble;

        static PyTypeObject _Type;
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
            return &_Type;
        }

        inline number_type_basic *get_CObject() {
            return _CObject;
        }


        inline static bool checkType(PyObject *const obj) {
            return PyObject_TypeCheck(obj, &_Type);
        }

        static __pyllars_internal::PythonClassWrapper<number_type> *fromCObject( number_type & cobj,
                                                                                 PyObject *referencing = nullptr);

    protected:
        static int initialize(){
            return _initialize(_Type);
        }

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

        class Initializer : public pyllars::Initializer {
        public:
            Initializer();

            status_t set_up() override;

            static Initializer *initializer;
        };

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


}
#endif