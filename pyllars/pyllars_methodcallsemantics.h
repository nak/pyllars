#ifndef __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H
#define __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H
#include "pyllars_utils.h"
#include "pyllars_defns.h"

/**
* This unit defines template classes needed to contain method pointers and
* define Python-to-C call semantics for invoking class instance methods
**/

namespace __pyllars_internal{



    /**
     * class to hold referecne to a class method and define
     * method call semantics
     **/
    template<typename CClass, typename T, typename ... Args>
    class MethodCallSemantics{
    public:
        typedef typename extent_as_pointer<T>::type(CClass::*method_t)(Args...);
        static const char* const *kwlist;



        /**
         * Used for regular methods:
         */
        static PyObject* call( method_t method, CClass & self, PyObject* args, PyObject* kwds){
            try{
              return toPyObject<T, true>( call_methodBase(method, self, args, kwds, typename argGenerator<sizeof...(Args)>::type()), false);
            } catch( const char* const msg){
                PyErr_SetString( PyExc_RuntimeError, msg);
                return  nullptr;
            }
        }

    private:

        /**
         * call that invokes method a la C:
         **/
        template< typename ...PyO>
        static typename extent_as_pointer<T>::type call_methodC( typename extent_as_pointer<T>::type  (CClass::*method)(Args...),
                                        typename std::remove_reference<CClass>::type &self,
                                        PyObject *args, PyObject *kwds, PyO* ...pyargs){
            static char format[sizeof...(Args)+1] = {0};
            if (sizeof...(Args)>0)
                memset( format, 'O', sizeof...(Args));

            if(!PyArg_ParseTupleAndKeywords(args, kwds, format, (char**)kwlist, &pyargs...)){
                PyErr_Print();
                throw "Invalid arguments to method call";
            }
            T retval =  (self.*method)(*toCObject<Args, false, true, PythonClassWrapper<Args,true> >(*pyargs)...);
            return retval;
        }

        /**
         * call that converts python given arguments to make C call:
         **/
        template<int ...S>
        static typename extent_as_pointer<T>::type  call_methodBase( typename extent_as_pointer<T>::type  (CClass::*method)(Args...),
                                  typename std::remove_reference<CClass>::type &self,
                                  PyObject* args, PyObject* kwds, container<S...> s) {
            (void)s;
            PyObject pyobjs[sizeof...(Args)+1];
            return call_methodC(method, self, args, kwds, &pyobjs[S]...);
            (void)pyobjs;
        }

    };

    template< class CClass, typename ReturnType, typename ...Args>
    const char* const *
    MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;


    /**
     * specialize for void returns:
     **/
    template<typename CClass, typename ...Args>
    class MethodCallSemantics<CClass,void, Args...>{
    public:
        typedef void(CClass::*method_t)(Args...);
        static const char* const * kwlist;

        static PyObject* toPyObj(CClass & self){
            (void)self;
            return Py_None;
        }
        static PyObject* call( method_t method, CClass & self, PyObject* args, PyObject* kwds){
	  call_methodBase(method, self, args, kwds, typename argGenerator< sizeof...(Args) >::type());
            return Py_None;
        }

    private:

        template< typename ...PyO>
        static void call_methodC( void (CClass::*method)(Args...),
                                        typename std::remove_reference<CClass>::type &self,
                                        PyObject* args, PyObject* kwds,
                                        PyO* ...pyargs){

            char format[sizeof...(Args)+1]={0};
            if (sizeof...(Args) > 0)
                memset(format,'O',sizeof...(Args));
            if(!PyArg_ParseTupleAndKeywords(args, kwds, format, (char**)kwlist, &pyargs...)){
                PyErr_SetString( PyExc_RuntimeError, "Failed to parse argument on method call");
            } else {
                (self.*method)(*toCObject<Args, false, true, PythonClassWrapper<Args,true> >(*pyargs)...);
            }
        }

        template<int ...S>
        static void call_methodBase( void (CClass::*method)(Args...),
                                     typename std::remove_reference<CClass>::type &self,
                                     PyObject *args, PyObject *kwds,
				     container<S...> unused) {
	     (void)unused;
             PyObject pyobjs[sizeof...(Args)+1];
             call_methodC(method, self, args, kwds, &pyobjs[S]...);
             (void)pyobjs;
        }

     };


    template< class CClass, typename ...Args>
    const char* const *
    MethodCallSemantics<CClass, void, Args...>::kwlist;


    /**
     * This class is needed to prevent ambiguities and compiler issues in add_method
     * It holds the method call and allows specialization based on
     * underlying CClass type
     **/
    template<  class CClass,  typename E = void>
    class MethodContainer{
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type * , PyObject*);

        template<const char* const name, typename ReturnType, typename ...Args>
        class Container{
            typedef typename extent_as_pointer<ReturnType>::type TrueReturnType;
            typedef TrueReturnType(CClass::*method_t)(Args...);

            typedef const char* const * kwlist_t;
            static constexpr  kwlist_t &kwlist = MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;

            static PyObject* call(PyObject* self, PyObject* args, PyObject* kwds);

        };
    };




    /**
     * Specialization for non-const class types
     **/
    template<class CClass>
    class MethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && !std::is_const<CClass>::value >::type>{
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type * , PyObject*);

        template<const char* const name, typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef typename extent_as_pointer<ReturnType>::type(CClass::*method_t)(Args...);

            typedef const char* const * kwlist_t;
            static constexpr kwlist_t &kwlist = MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;
            static method_t method;

            static PyObject* call(PyObject* self, PyObject* args, PyObject* kwds){
                if(!self) return nullptr;
                PythonClassWrapper<CClass, true>* _this = (PythonClassWrapper<CClass, true>*)self;
                if(_this->template get_CObject<CClass>()){
                    try{
                        return MethodCallSemantics<CClass, ReturnType, Args...>::call(method, *_this->template get_CObject<CClass>(),args, kwds);
                    } catch(...){
                        return nullptr;
                    }
                }
                return nullptr;
            }

        };

    };
    template <typename CClass>
    template <const char* const name, typename ReturnType, typename ...Args>
      typename MethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && !std::is_const<CClass>::value >::type>::template Container<name, ReturnType, Args...>::method_t
      MethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && !std::is_const<CClass>::value >::type>:: Container<name, ReturnType, Args...>::method;


    /**
     * Specialization for const class types
     **/
    template<class CClass>
    class MethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && std::is_const<CClass>::value >::type>{
    public:
        typedef void (*setter_t)(typename std::remove_reference<CClass>::type * , PyObject*);

        template<const char* const name, typename ReturnType, typename ...Args>
        class Container{
        public:
            typedef typename extent_as_pointer<ReturnType>::type(CClass::*method_t)(Args...);

            typedef const char* const * kwlist_t;
            static constexpr kwlist_t &kwlist = MethodCallSemantics<CClass, ReturnType, Args...>::kwlist;
            static method_t method;

            static PyObject* call(PyObject* self, PyObject* args, PyObject* kwds){
                if(!self) return nullptr;
                PythonClassWrapper<CClass, true>* _this = (PythonClassWrapper<CClass, true>*)self;
                if(_this->template get_CObject<CClass>()){
                    try{
                        return MethodCallSemantics<CClass, ReturnType, Args...>::call(method, *_this->template get_CObject<CClass>(),args, kwds);
                    } catch(...){
                        return nullptr;
                    }
                }
                return nullptr;
            }

        };
    };

    template< class CClass>
    template< const char* const name, typename ReturnType, typename ...Args>
    typename MethodContainer<CClass, typename std::enable_if< std::is_class<CClass>::value && std::is_const<CClass>::value >::type>::template Container<name, ReturnType, Args...>::method_t
     MethodContainer< CClass, typename std::enable_if< std::is_class<CClass>::value && std::is_const<CClass>::value >::type>::Container<name, ReturnType, Args...>::method;




    /**
     * Class member container
     * TODO!!!!!!!! ADD A "IS_COMPLETE" TEMPL PARAM!!!!!!!!!!!!!!!!1
     **/
    template<class CClass>
    class MemberContainer{
    public:

        template<const char* const name, typename T>
        class Container{
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T CClass_NoRef::* member_t;

            static member_t member;

            static PyObject* call(PyObject* self, PyObject* args, PyObject* kwds){
                if(!self) return nullptr;
                PythonClassWrapper<CClass, true>* _this = (PythonClassWrapper<CClass, true>*)self;

                if( (!args || PyTuple_Size(args)==0) && (!kwds || PyDict_Size(kwds)==0)){
                    if(_this->template get_CObject<CClass>()){
                        return toPyObject<T, true>(_this->template get_CObject<CClass>()->*member, false);
                    }
                    PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
                    return nullptr;
                } else if (kwds &&  PyDict_Size(kwds)==1 && PyDict_GetItemString(kwds, "set_to")){
                    PyObject* pyVal = PyDict_GetItemString(kwds, "set_to");
                    if (pyVal == Py_None){
                        PyErr_SetString(PyExc_SyntaxError, "Unexpcted None value in member setter");
                        return nullptr;
                    }
                     (_this->template get_CObject<CClass_NoRef>()->*member) = *toCObject<T, false, true, PythonClassWrapper<T, true> >(*pyVal);
                } else{
                    PyErr_SetString(PyExc_SyntaxError, "Invalid argsuments to set class instance member variable in C");
                    return nullptr;
                }
		return Py_None;
            }

            static void setFromPyObject( typename std::remove_reference<CClass>::type * self, PyObject* pyobj){
                self->*member = *toCObject<T, false, true, PythonClassWrapper<T, true> >(*pyobj);
            }
        };


        template<const char* const name, typename T>
        class Container<name, const T>{
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef const T CClass_NoRef::* member_t;

            static member_t member;

            static PyObject* call(PyObject* self, PyObject* args, PyObject* kwds){
                if(!self) return nullptr;
                PythonClassWrapper<CClass, true>* _this = (PythonClassWrapper<CClass, true>*)self;
               if( (!args || PyTuple_Size(args)==0) && (!kwds || PyDict_Size(kwds)==0)){
                    if(_this->template get_CObject<CClass_NoRef>()){
                        return toPyObject<T, true>(_this->template get_CObject<CClass_NoRef>()->*member, false);
                    }
                    PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
                    return nullptr;
                } else if (kwds &&  PyDict_Size(kwds)==1 && PyDict_GetItemString(kwds, "set_to")){
                    PyErr_SetString(PyExc_RuntimeError, "Attempt to set constant attribute!");
                    return nullptr;
                } else{
                    PyErr_SetString(PyExc_SyntaxError, "Invalid argsuments to set class instance member variable in C");
                    return nullptr;
                }
            }

            static void setFromPyObject( typename std::remove_reference<CClass>::type * self, PyObject* pyobj){
                PyErr_SetString(PyExc_RuntimeError, "Attempt to set constant field");
            }
        };

        template<const char* const name, const size_t size, typename T>
        class Container<name, T[size]>{
        public:
            typedef typename std::remove_reference<CClass>::type CClass_NoRef;
            typedef T T_array[size];
            typedef T_array CClass_NoRef::* member_t;

            static member_t member;

            static PyObject* call(PyObject* self, PyObject* args, PyObject* kwds){
                if(!self) return nullptr;
                PythonClassWrapper<CClass, true>* _this = (PythonClassWrapper<CClass, true>*)self;

                if( (!args || PyTuple_Size(args)==0) && (!kwds || PyDict_Size(kwds)==0)){
                    if(_this->template get_CObject<CClass>()){
                        return toPyObject<T, true>(*(_this->template get_CObject<CClass>()->*member), false);
                    }
                    PyErr_SetString(PyExc_RuntimeError, "No C Object found to get member attribute value!");
                    return nullptr;
                } else if (kwds &&  PyDict_Size(kwds)==1 && PyDict_GetItemString(kwds, "set_to")){
                    PyObject* pyVal = PyDict_GetItemString(kwds, "set_to");
                    if (pyVal == Py_None){
                        PyErr_SetString(PyExc_SyntaxError, "Unexpcted None value in member setter");
                        return nullptr;
                    }
                    if( PyTuple_Check(pyVal)){
                        if(PyTuple_Size(pyVal) == size){
                           for(size_t i = 0; i < size; ++i)
                            (_this->template get_CObject<CClass_NoRef>()->*member)[i] = *toCObject<T, false, true, PythonClassWrapper<T, true> >(*PyTuple_GetItem(pyVal, i));
                        } else{
                            PyErr_SetString(PyExc_IndexError, "Mismatched array sizes");
                            return nullptr;
                        }
                    } else if ( PyObject_TypeCheck( pyVal, (&PythonClassWrapper< T_array, true>::Type)) ){
                         T_array *val = ((PythonClassWrapper<T_array, true>*) pyVal)->template get_CObject<T_array>();
                         for(size_t i = 0; i < size; ++i)
                            (_this->template get_CObject<T_array>()->*member)[i] = (*val)[i];

                    } else {
                        PyErr_SetString(PyExc_TypeError, "Invalid argument type when setting array attribute");
                        return nullptr;
                    }
                } else{
                    PyErr_SetString(PyExc_SyntaxError, "Invalid argsuments to set class instance member variable in C");
                    return nullptr;
                }
                return Py_None;
            }

            static void setFromPyObject( typename std::remove_reference<CClass>::type * self, PyObject* pyobj){
                smart_ptr<T[size], false> val = toCObject<T[size], false, true, PythonClassWrapper<T[size], true, size-1> >(*pyobj);
                for(size_t i = 0; i < size; ++i){
                  (self->*member)[i] = (*val)[i];
                }
            }
        };

    };

    template< class CClass>
    template< const char* const name, typename T>
    typename MemberContainer<CClass>::template Container< name, T>::member_t
    MemberContainer<CClass>::Container< name, T>::member;

    template< class CClass>
    template< const char* const name, const size_t size, typename T>
    typename MemberContainer<CClass>::template Container< name, T[size]>::member_t
    MemberContainer<CClass>::Container< name, T[size]>::member;

    template< class CClass>
    template< const char* const name, typename T>
    typename MemberContainer<CClass>::template Container< name, const T>::member_t
    MemberContainer<CClass>::Container< name, const T>::member;


}

#endif // __PYLLARS_INTERNAL__METHODCALLSEMANTICS_H2

