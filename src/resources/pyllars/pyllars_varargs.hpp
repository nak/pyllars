#ifndef __PYLLARS__VARARGS__
#define __PYLLARS__VARARGS__

#include <ffi.h>
#include <Python.h>

namespace __pyllars_internal {

    static constexpr int COBJ_TYPE = 1;
    static constexpr int FUNC_TYPE = 2;
    static constexpr int STRING_TYPE = 3;

    int getType(PyObject *obj, ffi_type *&type);

    template<typename T, typename Z = void>
    struct FFIType {
        static ffi_type *type() {
            throw "Unsupport return type in var arg function";
        }
    };

    template<>
    struct FFIType<float, void> {
        static ffi_type *type() {
            return &ffi_type_float;
        }
    };

    template<>
    struct FFIType<double, void> {
        static ffi_type *type() {
            return &ffi_type_double;
        }
    };

    template<>
    struct FFIType<void, void> {
        static ffi_type *type() {
            return nullptr;
        }
    };

    template<typename T>
    struct FFIType<T, typename std::enable_if<std::is_pointer<T>::value>::type> {
        static ffi_type *type() {
            return &ffi_type_pointer;
        }
    };

    template<typename T>
    struct FFIType<T, typename std::enable_if<std::is_integral<T>::value>::type> {
        static ffi_type *type() {
            if (std::is_signed<T>::value) {
                switch (sizeof(T)) {
                    case 1:
                        return &ffi_type_sint8;
                    case 2:
                        return &ffi_type_sint16;
                    case 3:
                        return &ffi_type_sint32;
                    case 4:
                        return &ffi_type_sint64;
                    default:
                        throw "Unsupported return type in var arg function";
                }
            } else {
                switch (sizeof(T)) {
                    case 1:
                        return &ffi_type_uint8;
                    case 2:
                        return &ffi_type_uint16;
                    case 3:
                        return &ffi_type_uint32;
                    case 4:
                        return &ffi_type_uint64;
                    default:
                        throw "Unsupported return type in var arg function";
                }
            }
        }
    };

}
#endif
