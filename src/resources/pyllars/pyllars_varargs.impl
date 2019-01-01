#include "pyllars_varargs.hpp"
#include "pyllars_defns.hpp"

namespace __pyllars_internal{

        int getType(PyObject *obj, ffi_type *&type) {
            int subtype = 0;
            if (PyInt_Check(obj)) {
                type = &ffi_type_sint32;
            } else if (PyLong_Check(obj)) {
                type = &ffi_type_sint64;
            } else if (PyFloat_Check(obj)) {
                type = &ffi_type_double;
            } else if (PyBool_Check(obj)) {
                type = &ffi_type_uint8;
            } else if (PyString_Check(obj)) {
                type = &ffi_type_pointer;
                subtype = STRING_TYPE;
            #if PY_MAJOR_VERSION == 3
            } else if (PyBytes_Check(obj)) {
                type = &ffi_type_pointer;
                subtype = STRING_TYPE;
            #endif
            } else if (CommonBaseWrapper::IsClassType(obj)) {
                type = &ffi_type_pointer;
                subtype = COBJ_TYPE;
            } else if (CommonBaseWrapper::IsCFunctionType(obj)) {
                subtype = FUNC_TYPE;
                type = &ffi_type_pointer;
            } else {
                throw "Cannot conver Python object to C Object";
            }/*else if (PyList_Check(obj)){
            const C_TYPE subtype = PyTuple_Size(obj)>0?getType(PyList_GetItem(obj,0)):C_TYPE::INT;
            for(int i = 1; i < PyTuple_Size(obj); ++i){
                if(getType(PyList_GetItem(obj, i)) != subtype){
                    throw "Cannot convert mixed type list to C array";
                }
            }
            type = C_TYPE::ARRAY;
        }*/
            return subtype;
        }

}