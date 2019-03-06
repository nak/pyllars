#ifndef __PYLLARS__CONTAINMENT_H
#define __PYLLARS__CONTAINMENT_H
namespace __pyllars_test{


    template<typename T>
    struct ObjectContainer{
        typedef typename std::remove_reference<T> T_NoRef;

        ObjectContainer(T_NoRef& obj):_contained(obj), _containedP(&_contained){
        }

        operator T_NoRef&(){
            return _contained;
        }

        operator const T_NoRef&() const{
            return _contained;
        }


    protected:
        T_NoRef &_contained;
        T_NoRef *_containedP;
    };


    template<typename T, typename ...Args>
    struct ObjectContainerConstructed: public ObjectContainer<T>{
        ObjectContainerConstructed(Args ...args):_constructed(args...),
        ObjectContainer<T>(_constructed){

        }
    private:
        T _constructed;
    };
}
#endif