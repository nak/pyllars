//
// Created by jrusnak on 11/10/19.
//

#ifndef PYLLARS_PYLLARS_CLASSWRAPPER_XTORS_IMPL_HPP
#define PYLLARS_PYLLARS_CLASSWRAPPER_XTORS_IMPL_HPP
#include "pyllars_conversions.impl.hpp"
#include "pyllars_classwrapper.hpp"

namespace __pyllars_internal {


    template<typename T>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addConstructorBase(const char *const kwlist[], constructor_t c) {
        _constructors().push_back(ConstructorContainer(kwlist, c));
    }

    template<typename T>
    template< const char* const kwlist[], typename ...Args>
    void PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    addConstructor(){
        addConstructorBase(kwlist, &create<Args...>);
    }

    template<typename T>
    template<typename ...PyO>
    bool PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _parsePyArgs(const char *const kwlist[], PyObject *args, PyObject *kwds, PyO *&...pyargs) {
        char format[sizeof...(PyO) + 1] = {0};
        if (sizeof...(PyO) > 0)
            memset(format, 'O', sizeof...(PyO));
        return (args && !kwds && sizeof...(PyO) == 0) || PyArg_ParseTupleAndKeywords(args, kwds, format, (char **) kwlist, &pyargs...);
    }

    template<typename T>
    template<typename ...Args>
    typename std::remove_reference<T>::type *
    PythonClassWrapper<T,
            typename std::enable_if<is_rich_class<T>::value>::type>::
    _createBaseBase(argument_capture<Args> ... args) {
        return new T_NoRef(std::forward<typename extent_as_pointer<Args>::type>(args.value())...);
    }


    template<typename T>
    template<typename ...Args, int ...S>
    typename std::remove_reference<T>::type*
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    _createBase(PyObject *args, PyObject *kwds,
                const char *const kwlist[], container<S...>, _____fake<Args> *...) {
        if (args && PyTuple_Size(args) != sizeof...(Args)) {
            return nullptr;
        }
        PyObject *pyobjs[sizeof...(Args) + 1];
        (void) pyobjs;
        if (!_parsePyArgs(kwlist, args, kwds, pyobjs[S]...)) {
            PyErr_SetString(PyExc_TypeError, "Invalid constructor argument(s)");
            return nullptr;
        }

        return _createBaseBase<Args...>(__pyllars_internal::toCArgument<Args>(*pyobjs[S])...);
    }

    template<typename T>
    template<typename ...Args>
    typename std::remove_reference<T>::type*
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::
    create(const char *const kwlist[], PyObject *args, PyObject *kwds,
           unsigned char *location) {
        return _createBase<Args...>(args, kwds, kwlist, typename argGenerator<sizeof...(Args)>::type(),
                                    (_____fake<Args> *) nullptr...);

    }

}


#endif //PYLLARS_PYLLARS_CLASSWRAPPER_XTORS_IMPL_HPP
