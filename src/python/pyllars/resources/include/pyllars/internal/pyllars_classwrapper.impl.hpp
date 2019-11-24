#ifndef PYLLARS__CLASSWRAPPER_IMPL
#define PYLLARS__CLASSWRAPPER_IMPL

namespace __pyllars_internal {

//Implementations are in subfile pyllars_classwrapper-*.impl.hpp

    template<typename T>
    template<typename Parent>
    status_t
    PythonClassWrapper<T, typename std::enable_if<is_rich_class<T>::value>::type>::ready(){
        int status = 0;
        for (const auto& ready_fnctn: _childrenReadyFunctions()){
            status |= (ready_fnctn() == nullptr);
        }

        return status;
    }

}
#endif
