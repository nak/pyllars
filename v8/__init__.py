from ctypes import *
void = None

uint16_t = c_uint16
uint32_t = c_uint32
int32_t = c_int32
size_t = c_size_t
int64_t = c_int64
uint64_t = c_uint64
uintptr_t = POINTER(c_uint)
intptr_t = POINTER(c_int)

try:
    global native
    native = CDLL("")
except:
    native = None
    
def init( libname ):
    global native
    native = CDLL(libname)

class _FwdDecl_AccessorSignature(Structure):
    _fields_=[]
class _FwdDecl_Array(Structure):
    _fields_=[]
class _FwdDecl_Boolean(Structure):
    _fields_=[]
class _FwdDecl_BooleanObject(Structure):
    _fields_=[]
class _FwdDecl_Context(Structure):
    _fields_=[]
class _FwdDecl_CpuProfiler(Structure):
    _fields_=[]
class _FwdDecl_Data(Structure):
    _fields_=[]
class _FwdDecl_Date(Structure):
    _fields_=[]
class _FwdDecl_External(Structure):
    _fields_=[]
class _FwdDecl_Function(Structure):
    _fields_=[]
class _FwdDecl_FunctionTemplate(Structure):
    _fields_=[]
class _FwdDecl_HeapProfiler(Structure):
    _fields_=[]
class _FwdDecl_ImplementationUtilities(Structure):
    _fields_=[]
class _FwdDecl_Int32(Structure):
    _fields_=[]
class _FwdDecl_Integer(Structure):
    _fields_=[]
class _FwdDecl_Isolate(Structure):
    _fields_=[]
class _FwdDecl_Maybe_int_(Structure):
    _fields_=[]
class _FwdDecl_Maybe_bool_(Structure):
    _fields_=[]
class _FwdDecl_Maybe_double_(Structure):
    _fields_=[]
class _FwdDecl_Maybe_long_(Structure):
    _fields_=[]
class _FwdDecl_Maybe_unsigned_int_(Structure):
    _fields_=[]
class _FwdDecl_Maybe_v8__PropertyAttribute_(Structure):
    _fields_=[]
class _FwdDecl_Name(Structure):
    _fields_=[]
class _FwdDecl_Number(Structure):
    _fields_=[]
class _FwdDecl_NumberObject(Structure):
    _fields_=[]
class _FwdDecl_Object(Structure):
    _fields_=[]
class _FwdDecl_ObjectOperationDescriptor(Structure):
    _fields_=[]
class _FwdDecl_ObjectTemplate(Structure):
    _fields_=[]
class _FwdDecl_Platform(Structure):
    _fields_=[]
class _FwdDecl_Primitive(Structure):
    _fields_=[]
class _FwdDecl_Promise(Structure):
    _fields_=[]
class _FwdDecl_RawOperationDescriptor(Structure):
    _fields_=[]
class _FwdDecl_Script(Structure):
    _fields_=[]
class _FwdDecl_SharedArrayBuffer(Structure):
    _fields_=[]
class _FwdDecl_Signature(Structure):
    _fields_=[]
class _FwdDecl_StartupData(Structure):
    _fields_=[]
class _FwdDecl_StackFrame(Structure):
    _fields_=[]
class _FwdDecl_StackTrace(Structure):
    _fields_=[]
class _FwdDecl_String(Structure):
    _fields_=[]
class _FwdDecl_StringObject(Structure):
    _fields_=[]
class _FwdDecl_Symbol(Structure):
    _fields_=[]
class _FwdDecl_SymbolObject(Structure):
    _fields_=[]
class _FwdDecl_Uint32(Structure):
    _fields_=[]
class _FwdDecl_Utils(Structure):
    _fields_=[]
class _FwdDecl_Value(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Primitive_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Boolean_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Value_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Integer_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Script_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__String_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Context_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__UnboundScript_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Function_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Object_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__StackTrace_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__StackFrame_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Array_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__NativeWeakMap_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Number_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Uint32_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Int32_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Symbol_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Name_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__FunctionTemplate_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Map_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Set_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Promise__Resolver_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Promise_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__ArrayBuffer_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Uint8Array_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__SharedArrayBuffer_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Uint8ClampedArray_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Int8Array_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Uint16Array_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Int16Array_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Uint32Array_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Int32Array_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Float32Array_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Float64Array_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__DataView_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__RegExp_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__External_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Data_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__AccessorSignature_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Signature_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__ObjectTemplate_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__TypeSwitch_(Structure):
    _fields_=[]
class _FwdDecl_Local_v8__Message_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__Script_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__Value_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__UnboundScript_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__Function_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__String_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__Boolean_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__Number_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__Object_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__Integer_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__Uint32_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__Int32_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__Array_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__Map_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__Set_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__Promise__Resolver_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__Promise_(Structure):
    _fields_=[]
class _FwdDecl_MaybeLocal_v8__RegExp_(Structure):
    _fields_=[]
class _FwdDecl_NonCopyablePersistentTraits_v8__Value_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__Value_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__Integer_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__Boolean_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__UnboundScript_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__String_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__Object_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__Function_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__Context_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__FunctionTemplate_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__AccessorSignature_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__Signature_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__Promise_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__StackTrace_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__ObjectTemplate_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__Data_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__Name_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__Number_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__Uint32_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__Int32_(Structure):
    _fields_=[]
class _FwdDecl_PersistentBase_v8__Primitive_(Structure):
    _fields_=[]
class _FwdDecl_Persistent_v8__Value__v8__NonCopyablePersistentTraits_v8__Value___(Structure):
    _fields_=[]
class _FwdDecl_FunctionCallbackInfo_v8__Value_(Structure):
    _fields_=[]
class _FwdDecl_PropertyCallbackInfo_v8__Value_(Structure):
    _fields_=[]
class _FwdDecl_PropertyCallbackInfo_void_(Structure):
    _fields_=[]
class _FwdDecl_PropertyCallbackInfo_v8__Integer_(Structure):
    _fields_=[]
class _FwdDecl_PropertyCallbackInfo_v8__Boolean_(Structure):
    _fields_=[]
class _FwdDecl_PropertyCallbackInfo_v8__Array_(Structure):
    _fields_=[]
class _FwdDecl_CallHandlerHelper(Structure):
    _fields_=[]
class _FwdDecl_EscapableHandleScope(Structure):
    _fields_=[]
class _FwdDecl_UniqueId(Structure):
    _fields_=[]
class _FwdDecl_WeakCallbackInfo_void_(Structure):
    _fields_=[]
class _FwdDecl_WeakCallbackData_v8__Value__void_(Structure):
    _fields_=[]
class _FwdDecl_HandleScope(Structure):
    _fields_=[]
class _FwdDecl_SealHandleScope(Structure):
    _fields_=[]
class _FwdDecl_ScriptOriginOptions(Structure):
    _fields_=[]
class _FwdDecl_ScriptOrigin(Structure):
    _fields_=[]
class _FwdDecl_UnboundScript(Structure):
    _fields_=[]
class _FwdDecl_ScriptCompiler(Structure):
    _fields_=[]
class _FwdDecl_Message(Structure):
    _fields_=[]
class _FwdDecl_RegisterState(Structure):
    _fields_=[]
class _FwdDecl_SampleInfo(Structure):
    _fields_=[]
class _FwdDecl_JSON(Structure):
    _fields_=[]
class _FwdDecl_NativeWeakMap(Structure):
    _fields_=[]
class _FwdDecl_Map(Structure):
    _fields_=[]
class _FwdDecl_Set(Structure):
    _fields_=[]
class _FwdDecl_ArrayBuffer(Structure):
    _fields_=[]
class _FwdDecl_ArrayBufferView(Structure):
    _fields_=[]
class _FwdDecl_TypedArray(Structure):
    _fields_=[]
class _FwdDecl_Uint8Array(Structure):
    _fields_=[]
class _FwdDecl_Uint8ClampedArray(Structure):
    _fields_=[]
class _FwdDecl_Int8Array(Structure):
    _fields_=[]
class _FwdDecl_Uint16Array(Structure):
    _fields_=[]
class _FwdDecl_Int16Array(Structure):
    _fields_=[]
class _FwdDecl_Uint32Array(Structure):
    _fields_=[]
class _FwdDecl_Int32Array(Structure):
    _fields_=[]
class _FwdDecl_Float32Array(Structure):
    _fields_=[]
class _FwdDecl_Float64Array(Structure):
    _fields_=[]
class _FwdDecl_DataView(Structure):
    _fields_=[]
class _FwdDecl_RegExp(Structure):
    _fields_=[]
class _FwdDecl_Template(Structure):
    _fields_=[]
class _FwdDecl_NamedPropertyHandlerConfiguration(Structure):
    _fields_=[]
class _FwdDecl_IndexedPropertyHandlerConfiguration(Structure):
    _fields_=[]
class _FwdDecl_TypeSwitch(Structure):
    _fields_=[]
class _FwdDecl_ExternalOneByteStringResourceImpl(Structure):
    _fields_=[]
class _FwdDecl_Extension(Structure):
    _fields_=[]
class _FwdDecl_ResourceConstraints(Structure):
    _fields_=[]
class _FwdDecl_Exception(Structure):
    _fields_=[]
class _FwdDecl_PromiseRejectMessage(Structure):
    _fields_=[]
class _FwdDecl_HeapStatistics(Structure):
    _fields_=[]
class _FwdDecl_HeapSpaceStatistics(Structure):
    _fields_=[]
class _FwdDecl_HeapObjectStatistics(Structure):
    _fields_=[]
class _FwdDecl_RetainedObjectInfo(Structure):
    _fields_=[]
class _FwdDecl_JitCodeEvent(Structure):
    _fields_=[]
class _FwdDecl_ExternalResourceVisitor(Structure):
    _fields_=[]
class _FwdDecl_PersistentHandleVisitor(Structure):
    _fields_=[]
class _FwdDecl_V8(Structure):
    _fields_=[]
class _FwdDecl_TryCatch(Structure):
    _fields_=[]
class _FwdDecl_ExtensionConfiguration(Structure):
    _fields_=[]
class _FwdDecl_Unlocker(Structure):
    _fields_=[]
class _FwdDecl_Locker(Structure):
    _fields_=[]
from globals import uint32_t, int32_t, size_t, int64_t, uintptr_t, uint16_t, uint64_t

#TYPEDEFS
__uint16_t = c_ushort
__uint64_t = c_ulong
__int16_t = c_short
__ssize_t = c_long
__rlim64_t = c_ulong
__blksize_t = c_long
int32_t = c_int
uint_least64_t = c_ulong
__u_char = c_byte
uintptr_t = c_ulong
uintmax_t = c_ulong
__time_t = c_long
size_t = c_ulong
int_fast32_t = c_long
__syscall_ulong_t = c_ulong
int16_t = c_short
int64_t = c_long
int_fast16_t = c_long
__int128_t = (c_longlong*2)
__intptr_t = c_long
__id_t = c_uint
int_fast64_t = c_long
__gid_t = c_uint
__clockid_t = c_int
__uint32_t = c_uint
__rlim_t = c_ulong
uint8_t = c_byte
__syscall_slong_t = c_long
__off64_t = c_long
int_least8_t = c_char
__off_t = c_long
__quad_t = c_long
uint_least16_t = c_ushort
__fsfilcnt64_t = c_ulong
__daddr_t = c_int
__u_quad_t = c_ulong
__u_short = c_ushort
__ino64_t = c_ulong
uint_least32_t = c_uint
__uint128_t = (c_ulong*2)
int_least16_t = c_short
int_fast8_t = c_char
uint_least8_t = c_byte
intptr_t = c_long
__pid_t = c_int
__uint8_t = c_byte
__fsword_t = c_long
int_least32_t = c_int
__fsblkcnt64_t = c_ulong
__socklen_t = c_uint
int8_t = c_char
__clock_t = c_long
__useconds_t = c_uint
__ino_t = c_ulong
__dev_t = c_ulong
_IO_lock_t = void
__mode_t = c_uint
__u_long = c_ulong
__suseconds_t = c_long
__fsblkcnt_t = c_ulong
int_least64_t = c_long
uint_fast32_t = c_ulong
uint_fast64_t = c_ulong
__fsfilcnt_t = c_ulong
intmax_t = c_long
__u_int = c_uint
__blkcnt_t = c_long
uint_fast16_t = c_ulong
__int32_t = c_int
__int64_t = c_long
__key_t = c_int
uint32_t = c_uint
ptrdiff_t = c_long
__nlink_t = c_ulong
__uid_t = c_uint
uint64_t = c_ulong
uint16_t = c_ushort
__int8_t = c_char
uint_fast8_t = c_byte
__blkcnt64_t = c_long

func_344 = CFUNCTYPE( void, )
func_344 = CFUNCTYPE( __ssize_t,     c_void_p,     c_char_p,     size_t, )
func_345 = CFUNCTYPE( void, )
func_345 = CFUNCTYPE( __ssize_t,     c_void_p,     c_char_p,     size_t, )
func_346 = CFUNCTYPE( void, )
func_346 = CFUNCTYPE( c_int,     c_void_p,     POINTER(_FwdClassDecl___off64_t),     c_int, )
func_347 = CFUNCTYPE( void, )
func_347 = CFUNCTYPE( c_int,     c_void_p, )
func_2959 = CFUNCTYPE( void,     POINTER(_FwdClassDecl_Isolate),     c_uint, )
func_2960 = CFUNCTYPE( void,     POINTER(_FwdClassDecl_Isolate),     c_uint,     c_uint, )
func_2960 = CFUNCTYPE( void,     POINTER(_FwdClassDecl_Isolate),     c_uint,     c_uint, )
func_2980 = CFUNCTYPE( void,     POINTER(_FwdClassDecl_WeakCallbackInfo_void_), )
func_2981 = CFUNCTYPE( void,     POINTER(_FwdClassDecl_WeakCallbackData_v8__Value__void_), )
func_2779 = CFUNCTYPE( void,     Local_v8__String_,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Value_), )
func_2780 = CFUNCTYPE( void,     Local_v8__Name_,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Value_), )
func_2781 = CFUNCTYPE( void,     Local_v8__String_,     Local_v8__Value_,     POINTER(_FwdClassDecl_PropertyCallbackInfo_void_), )
func_2782 = CFUNCTYPE( void,     Local_v8__Name_,     Local_v8__Value_,     POINTER(_FwdClassDecl_PropertyCallbackInfo_void_), )
func_2789 = CFUNCTYPE( void,     POINTER(_FwdClassDecl_FunctionCallbackInfo_v8__Value_), )
func_2849 = CFUNCTYPE( void,     Local_v8__String_,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Value_), )
func_2850 = CFUNCTYPE( void,     Local_v8__String_,     Local_v8__Value_,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Value_), )
func_2851 = CFUNCTYPE( void,     Local_v8__String_,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Integer_), )
func_2852 = CFUNCTYPE( void,     Local_v8__String_,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Boolean_), )
func_2853 = CFUNCTYPE( void,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Array_), )
func_2854 = CFUNCTYPE( void,     Local_v8__Name_,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Value_), )
func_2855 = CFUNCTYPE( void,     Local_v8__Name_,     Local_v8__Value_,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Value_), )
func_2856 = CFUNCTYPE( void,     Local_v8__Name_,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Integer_), )
func_2857 = CFUNCTYPE( void,     Local_v8__Name_,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Boolean_), )
func_2858 = CFUNCTYPE( void,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Array_), )
func_2859 = CFUNCTYPE( void,     uint32_t,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Value_), )
func_2860 = CFUNCTYPE( void,     uint32_t,     Local_v8__Value_,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Value_), )
func_2861 = CFUNCTYPE( void,     uint32_t,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Integer_), )
func_2862 = CFUNCTYPE( void,     uint32_t,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Boolean_), )
func_2863 = CFUNCTYPE( void,     POINTER(_FwdClassDecl_PropertyCallbackInfo_v8__Array_), )
func_2864 = CFUNCTYPE( void,     Local_v8__Object_,     Local_v8__Value_,     c_uint,     Local_v8__Value_, )
func_2865 = CFUNCTYPE( void,     Local_v8__Object_,     uint32_t,     c_uint,     Local_v8__Value_, )
func_2877 = CFUNCTYPE( void,     c_char_p,     c_char_p, )
func_2878 = CFUNCTYPE( void,     Local_v8__Message_,     Local_v8__Value_, )
func_2879 = CFUNCTYPE( void,     c_char_p,     c_int, )
func_2882 = CFUNCTYPE( void,     c_char_p, )
func_2883 = CFUNCTYPE( void,     c_char_p,     c_int,     c_int,     size_t, )
func_2884 = CFUNCTYPE( void,     c_void_p,     c_int, )
func_2885 = CFUNCTYPE( void,     c_uint,     c_uint,     c_int, )
func_2886 = CFUNCTYPE( void, )
func_2889 = CFUNCTYPE( void,     PromiseRejectMessage, )
func_2890 = CFUNCTYPE( void,     c_void_p, )
func_2891 = CFUNCTYPE( void,     Local_v8__Object_,     c_uint,     Local_v8__Value_, )
func_2892 = CFUNCTYPE( void,     _FwdDecl_Local_v8__Context_, )
func_2893 = CFUNCTYPE( void,     c_uint,     c_uint, )
func_2893 = CFUNCTYPE( void,     c_uint,     c_uint, )
func_2894 = CFUNCTYPE( void,     POINTER(_FwdClassDecl_Isolate),     c_void_p, )
func_2901 = CFUNCTYPE( void,     uintptr_t,     uintptr_t, )
func_2924 = CFUNCTYPE( void,     POINTER(_FwdClassDecl_JitCodeEvent), )
func_2930 = CFUNCTYPE( void,     POINTER(_FwdClassDecl_c_byte),     size_t, )
func_2931 = CFUNCTYPE( void,     uintptr_t, )
__io_read_fn = func_344
class GPEpilogueCallback(object):
    
    def __init__(self, cb):
        self._callback  = cb
        
    def invoke(self, arg1, arg2):
        self._callbback( arg1, arg2)
       

GCEpilogueCallback = func_2893
IndexedPropertyDeleterCallback = func_2862
__io_close_fn = func_347
ReturnAddressLocationResolver = func_2931
GCPrologueCallback = func_2893
NamedPropertyGetterCallback = func_2849
IndexedPropertyEnumeratorCallback = func_2863
NamedPropertySetterCallback = func_2850
PromiseRejectCallback = func_2889
NamedPropertyQueryCallback = func_2851
FunctionEntryHook = func_2901
AccessorGetterCallback = func_2779
AccessorSetterCallback = func_2781
UseCounterCallback = func_2959
InterruptCallback = func_2894
EntropySource = func_2930
NamedPropertyEnumeratorCallback = func_2853
AddHistogramSampleCallback = func_2884
GenericNamedPropertyDeleterCallback = func_2857
GenericNamedPropertyGetterCallback = func_2854
NamedPropertyDeleterCallback = func_2852
LogEventCallback = func_2879
MicrotaskCallback = func_2890
CounterLookupCallback = func_2882
CallCompletedCallback = func_2886
IndexedPropertySetterCallback = func_2860
__io_seek_fn = func_346
__io_write_fn = func_345
MessageCallback = func_2878
Callback = func_2981
FailedAccessCheckCallback = func_2891
GenericNamedPropertyEnumeratorCallback = func_2858
AllowCodeGenerationFromStringsCallback = func_2892
NamedSecurityCallback = func_2864
MemoryAllocationCallback = func_2885
GenericNamedPropertyQueryCallback = func_2856
GenericNamedPropertySetterCallback = func_2855
FunctionCallback = func_2789
IndexedSecurityCallback = func_2865
IndexedPropertyGetterCallback = func_2859
IndexedPropertyQueryCallback = func_2861
AccessorNameSetterCallback = func_2782
AccessorNameGetterCallback = func_2780
FatalErrorCallback = func_2877
CreateHistogramCallback = func_2883
JitCodeEventHandler = func_2924
        

class AccessorSignature( _FwdDecl_AccessorSignature ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.AccessorSignature)            
        """
        assert(isinstance(arg1,AccessorSignature))
        obj =  AccessorSignature\
            ( native.AccessorSignature_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.AccessorSignature)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.AccessorSignature_copy(args[0])
        else:
            
            try:
                self._cobject = native.AccessorSignature_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return AccessorSignature(native.AccessorSignature_copy( self._cobject ))

    def __del__(self):
        native.AccessorSignature_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(isolate,receiver,):
        '''
        @returns: v8.Local_v8__AccessorSignature_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(receiver,Local_v8__FunctionTemplate_))
        return Local_v8__AccessorSignature_(native.v8___AccessorSignature__New(isolate,receiver._cobject,))

    def assign(self, arg1,):
        '''
        @returns: v8.AccessorSignature
        '''
        assert(isinstance(arg1,POINTER(AccessorSignature)))
        return (native.v8___AccessorSignature__assign(self._cobject, arg1._cobject,))        

class Array( _FwdDecl_Array ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Array)            
        """
        assert(isinstance(arg1,Array))
        obj =  Array\
            ( native.Array_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Array)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Array_copy(args[0])
        else:
            
            try:
                self._cobject = native.Array_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Array(native.Array_copy( self._cobject ))

    def __del__(self):
        native.Array_delete(self._cobject)
        self._cobjet = None
            
    

    def Length(self, ):
        '''
        @returns: uint32_t
        '''
        return native.v8___Array__Length(self._cobject, )

    def CloneElementAt(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.CloneElementAt_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.CloneElementAt_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def CloneElementAt_2(self, index,):
        '''
        @returns: v8.Local_v8__Object_
        '''
        assert(isinstance(index,uint32_t))
        return Local_v8__Object_(native.v8___Array__CloneElementAt(self._cobject, index,))


    def CloneElementAt_3(self, context,index,):
        '''
        @returns: v8.MaybeLocal_v8__Object_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(index,uint32_t))
        return MaybeLocal_v8__Object_(native.v8___Array__CloneElementAt(self._cobject, context._cobject,index,))


    @staticmethod
    def New(isolate,length,):
        '''
        @returns: v8.Local_v8__Array_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(length,c_int))
        return Local_v8__Array_(native.v8___Array__New(isolate,length,))

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Array
        '''
        assert(isinstance(obj,Value))
        return native.v8___Array__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Array__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Array
        '''
        assert(isinstance(arg1,POINTER(Array)))
        return (native.v8___Array__assign(self._cobject, arg1._cobject,))        

class Boolean( _FwdDecl_Boolean ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Boolean)            
        """
        assert(isinstance(arg1,Boolean))
        obj =  Boolean\
            ( native.Boolean_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Boolean)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Boolean_copy(args[0])
        else:
            
            try:
                self._cobject = native.Boolean_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Boolean(native.Boolean_copy( self._cobject ))

    def __del__(self):
        native.Boolean_delete(self._cobject)
        self._cobjet = None
            
    

    def Value(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Boolean__Value(self._cobject, )

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Boolean
        '''
        assert(isinstance(obj,Value))
        return native.v8___Boolean__Cast(obj,)

    @staticmethod
    def New(isolate,value,):
        '''
        @returns: v8.Local_v8__Boolean_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(value,c_char))
        return Local_v8__Boolean_(native.v8___Boolean__New(isolate,value,))

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Boolean__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Boolean
        '''
        assert(isinstance(arg1,POINTER(Boolean)))
        return (native.v8___Boolean__assign(self._cobject, arg1._cobject,))        

class BooleanObject( _FwdDecl_BooleanObject ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.BooleanObject)            
        """
        assert(isinstance(arg1,BooleanObject))
        obj =  BooleanObject\
            ( native.BooleanObject_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.BooleanObject)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.BooleanObject_copy(args[0])
        else:
            
            try:
                self._cobject = native.BooleanObject_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return BooleanObject(native.BooleanObject_copy( self._cobject ))

    def __del__(self):
        native.BooleanObject_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(value,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(value,c_char))
        return Local_v8__Value_(native.v8___BooleanObject__New(value,))

    def ValueOf(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___BooleanObject__ValueOf(self._cobject, )

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.BooleanObject
        '''
        assert(isinstance(obj,Value))
        return native.v8___BooleanObject__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___BooleanObject__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.BooleanObject
        '''
        assert(isinstance(arg1,POINTER(BooleanObject)))
        return (native.v8___BooleanObject__assign(self._cobject, arg1._cobject,))        

class Scope( _FwdDecl_Scope ):

    @staticmethod
    def new1(self, context,):
        """
        Signature: 
           context:v8.Local_v8__Context_            
        """
        assert(isinstance(context,Scope))
        obj =  Scope\
            ( native.Scope_new(context._cobject, ))
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Scope)            
        """
        assert(isinstance(arg1,Scope))
        obj =  Scope\
            ( native.Scope_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Local_v8__Context_            
        
        Signature[2]: 
           args[0]:POINTER(v8.Scope)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Scope_copy(args[0])
        else:
            
            try:
                self._cobject = native.Scope_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Scope_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Scope(native.Scope_copy( self._cobject ))

    def __del__(self):
        native.Scope_delete(self._cobject)
        self._cobjet = None
            
    

    def assign(self, arg1,):
        '''
        @returns: v8.Scope
        '''
        assert(isinstance(arg1,POINTER(Scope)))
        return (native.v8___Scope__assign(self._cobject, arg1._cobject,))        

class Context( _FwdDecl_Context ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Context\
            ( native.Context_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Context)            
        """
        assert(isinstance(arg1,Context))
        obj =  Context\
            ( native.Context_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.Context)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Context_copy(args[0])
        else:
            
            try:
                self._cobject = native.Context_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Context_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Context(native.Context_copy( self._cobject ))

    def __del__(self):
        native.Context_delete(self._cobject)
        self._cobjet = None
            
    

    def Global(self, ):
        '''
        @returns: v8.Local_v8__Object_
        '''
        return Local_v8__Object_(native.v8___Context__Global(self._cobject, ))

    def DetachGlobal(self, ):
        '''
        @returns: void
        '''
        return native.v8___Context__DetachGlobal(self._cobject, )

    @staticmethod
    def New(isolate,extensions,global_template,global_object,):
        '''
        @returns: v8.Local_v8__Context_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(extensions,ExtensionConfiguration))
        assert(isinstance(global_template,Local_v8__ObjectTemplate_))
        assert(isinstance(global_object,Local_v8__Value_))
        return Local_v8__Context_(native.v8___Context__New(isolate,extensions,global_template._cobject,global_object._cobject,))

    def SetSecurityToken(self, token,):
        '''
        @returns: void
        '''
        assert(isinstance(token,Local_v8__Value_))
        return native.v8___Context__SetSecurityToken(self._cobject, token._cobject,)

    def UseDefaultSecurityToken(self, ):
        '''
        @returns: void
        '''
        return native.v8___Context__UseDefaultSecurityToken(self._cobject, )

    def GetSecurityToken(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___Context__GetSecurityToken(self._cobject, ))

    def Enter(self, ):
        '''
        @returns: void
        '''
        return native.v8___Context__Enter(self._cobject, )

    def Exit(self, ):
        '''
        @returns: void
        '''
        return native.v8___Context__Exit(self._cobject, )

    def GetIsolate(self, ):
        '''
        @returns: v8.Isolate
        '''
        return native.v8___Context__GetIsolate(self._cobject, )

    def GetEmbedderData(self, index,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(index,c_int))
        return Local_v8__Value_(native.v8___Context__GetEmbedderData(self._cobject, index,))

    def GetExtrasExportsObject(self, ):
        '''
        @returns: v8.Local_v8__Object_
        '''
        return Local_v8__Object_(native.v8___Context__GetExtrasExportsObject(self._cobject, ))

    def SetEmbedderData(self, index,value,):
        '''
        @returns: void
        '''
        assert(isinstance(index,c_int))
        assert(isinstance(value,Local_v8__Value_))
        return native.v8___Context__SetEmbedderData(self._cobject, index,value._cobject,)

    def GetAlignedPointerFromEmbedderData(self, index,):
        '''
        @returns: c_void_p
        '''
        assert(isinstance(index,c_int))
        return native.v8___Context__GetAlignedPointerFromEmbedderData(self._cobject, index,)

    def SetAlignedPointerInEmbedderData(self, index,value,):
        '''
        @returns: void
        '''
        assert(isinstance(index,c_int))
        assert(isinstance(value,c_void_p))
        return native.v8___Context__SetAlignedPointerInEmbedderData(self._cobject, index,value,)

    def AllowCodeGenerationFromStrings(self, allow,):
        '''
        @returns: void
        '''
        assert(isinstance(allow,c_char))
        return native.v8___Context__AllowCodeGenerationFromStrings(self._cobject, allow,)

    def IsCodeGenerationFromStringsAllowed(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Context__IsCodeGenerationFromStringsAllowed(self._cobject, )

    def SetErrorMessageForCodeGenerationFromStrings(self, message,):
        '''
        @returns: void
        '''
        assert(isinstance(message,Local_v8__String_))
        return native.v8___Context__SetErrorMessageForCodeGenerationFromStrings(self._cobject, message._cobject,)

    def SlowGetEmbedderData(self, index,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(index,c_int))
        return Local_v8__Value_(native.v8___Context__SlowGetEmbedderData(self._cobject, index,))

    def SlowGetAlignedPointerFromEmbedderData(self, index,):
        '''
        @returns: c_void_p
        '''
        assert(isinstance(index,c_int))
        return native.v8___Context__SlowGetAlignedPointerFromEmbedderData(self._cobject, index,)

    def assign(self, arg1,):
        '''
        @returns: v8.Context
        '''
        assert(isinstance(arg1,POINTER(Context)))
        return (native.v8___Context__assign(self._cobject, arg1._cobject,))        

class CpuProfiler( _FwdDecl_CpuProfiler ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.CpuProfiler_copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return CpuProfiler(native.CpuProfiler_copy( self._cobject ))

    def __del__(self):
        native.CpuProfiler_delete(self._cobject)
        self._cobjet = None
            
            

class Data( _FwdDecl_Data ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Data)            
        """
        assert(isinstance(arg1,Data))
        obj =  Data\
            ( native.Data_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Data)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Data_copy(args[0])
        else:
            
            try:
                self._cobject = native.Data_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Data(native.Data_copy( self._cobject ))

    def __del__(self):
        native.Data_delete(self._cobject)
        self._cobjet = None
            
    

    def assign(self, arg1,):
        '''
        @returns: v8.Data
        '''
        assert(isinstance(arg1,POINTER(Data)))
        return (native.v8___Data__assign(self._cobject, arg1._cobject,))        

class Date( _FwdDecl_Date ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Date)            
        """
        assert(isinstance(arg1,Date))
        obj =  Date\
            ( native.Date_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Date)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Date_copy(args[0])
        else:
            
            try:
                self._cobject = native.Date_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Date(native.Date_copy( self._cobject ))

    def __del__(self):
        native.Date_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Date.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Date.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(isolate,time,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(time,c_double))
        return Local_v8__Value_(native.v8___Date__New(isolate,time,))

    @staticmethod
    def New_2(context,time,):
        '''
        @returns: v8.MaybeLocal_v8__Value_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(time,c_double))
        return MaybeLocal_v8__Value_(native.v8___Date__New(context._cobject,time,))

    def ValueOf(self, ):
        '''
        @returns: c_double
        '''
        return native.v8___Date__ValueOf(self._cobject, )

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Date
        '''
        assert(isinstance(obj,Value))
        return native.v8___Date__Cast(obj,)

    @staticmethod
    def DateTimeConfigurationChangeNotification(isolate,):
        '''
        @returns: void
        '''
        assert(isinstance(isolate,Isolate))
        return native.v8___Date__DateTimeConfigurationChangeNotification(isolate,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Date__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Date
        '''
        assert(isinstance(arg1,POINTER(Date)))
        return (native.v8___Date__assign(self._cobject, arg1._cobject,))        

class External( _FwdDecl_External ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.External)            
        """
        assert(isinstance(arg1,External))
        obj =  External\
            ( native.External_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.External)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.External_copy(args[0])
        else:
            
            try:
                self._cobject = native.External_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return External(native.External_copy( self._cobject ))

    def __del__(self):
        native.External_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(isolate,value,):
        '''
        @returns: v8.Local_v8__External_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(value,c_void_p))
        return Local_v8__External_(native.v8___External__New(isolate,value,))

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.External
        '''
        assert(isinstance(obj,Value))
        return native.v8___External__Cast(obj,)

    def Value(self, ):
        '''
        @returns: c_void_p
        '''
        return native.v8___External__Value(self._cobject, )

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___External__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.External
        '''
        assert(isinstance(arg1,POINTER(External)))
        return (native.v8___External__assign(self._cobject, arg1._cobject,))        

class Function( _FwdDecl_Function ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Function)            
        """
        assert(isinstance(arg1,Function))
        obj =  Function\
            ( native.Function_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Function)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Function_copy(args[0])
        else:
            
            try:
                self._cobject = native.Function_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Function(native.Function_copy( self._cobject ))

    def __del__(self):
        native.Function_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 4:
            try:
                return Function.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 4:
            try:
                return Function.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(context,callback,data,length,):
        '''
        @returns: v8.MaybeLocal_v8__Function_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(callback,FunctionCallback))
        assert(isinstance(data,Local_v8__Value_))
        assert(isinstance(length,c_int))
        return MaybeLocal_v8__Function_(native.v8___Function__New(context._cobject,callback,data._cobject,length,))

    @staticmethod
    def New_2(isolate,callback,data,length,):
        '''
        @returns: v8.Local_v8__Function_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(callback,FunctionCallback))
        assert(isinstance(data,Local_v8__Value_))
        assert(isinstance(length,c_int))
        return Local_v8__Function_(native.v8___Function__New(isolate,callback,data._cobject,length,))

    def NewInstance(self, *args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return self.NewInstance_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return self.NewInstance_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.NewInstance_3(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.NewInstance_4(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def NewInstance_3(self, argc,argv,):
        '''
        @returns: v8.Local_v8__Object_
        '''
        assert(isinstance(argc,c_int))
        assert(isinstance(argv,Local_v8__Value_))
        return Local_v8__Object_(native.v8___Function__NewInstance(self._cobject, argc,argv,))

    def NewInstance_4(self, context,argc,argv,):
        '''
        @returns: v8.MaybeLocal_v8__Object_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(argc,c_int))
        assert(isinstance(argv,Local_v8__Value_))
        return MaybeLocal_v8__Object_(native.v8___Function__NewInstance(self._cobject, context._cobject,argc,argv,))

    def NewInstance_5(self, ):
        '''
        @returns: v8.Local_v8__Object_
        '''
        return Local_v8__Object_(native.v8___Function__NewInstance(self._cobject, ))

    def NewInstance_6(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__Object_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__Object_(native.v8___Function__NewInstance(self._cobject, context._cobject,))

    def Call(self, *args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return self.Call_5(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 4:
            try:
                return self.Call_6(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def Call_7(self, recv,argc,argv,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(recv,Local_v8__Value_))
        assert(isinstance(argc,c_int))
        assert(isinstance(argv,Local_v8__Value_))
        return Local_v8__Value_(native.v8___Function__Call(self._cobject, recv._cobject,argc,argv,))

    def Call_8(self, context,recv,argc,argv,):
        '''
        @returns: v8.MaybeLocal_v8__Value_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(recv,Local_v8__Value_))
        assert(isinstance(argc,c_int))
        assert(isinstance(argv,Local_v8__Value_))
        return MaybeLocal_v8__Value_(native.v8___Function__Call(self._cobject, context._cobject,recv._cobject,argc,argv,))

    def SetName(self, name,):
        '''
        @returns: void
        '''
        assert(isinstance(name,Local_v8__String_))
        return native.v8___Function__SetName(self._cobject, name._cobject,)

    def GetName(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___Function__GetName(self._cobject, ))

    def GetInferredName(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___Function__GetInferredName(self._cobject, ))

    def GetDisplayName(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___Function__GetDisplayName(self._cobject, ))

    def GetScriptLineNumber(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___Function__GetScriptLineNumber(self._cobject, )

    def GetScriptColumnNumber(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___Function__GetScriptColumnNumber(self._cobject, )

    def IsBuiltin(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Function__IsBuiltin(self._cobject, )

    def ScriptId(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___Function__ScriptId(self._cobject, )

    def GetBoundFunction(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___Function__GetBoundFunction(self._cobject, ))

    def GetScriptOrigin(self, ):
        '''
        @returns: v8.ScriptOrigin
        '''
        return ScriptOrigin(native.v8___Function__GetScriptOrigin(self._cobject, ))

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Function
        '''
        assert(isinstance(obj,Value))
        return native.v8___Function__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Function__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Function
        '''
        assert(isinstance(arg1,POINTER(Function)))
        return (native.v8___Function__assign(self._cobject, arg1._cobject,))        

class FunctionTemplate( _FwdDecl_FunctionTemplate ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.FunctionTemplate)            
        """
        assert(isinstance(arg1,FunctionTemplate))
        obj =  FunctionTemplate\
            ( native.FunctionTemplate_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.FunctionTemplate)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.FunctionTemplate_copy(args[0])
        else:
            
            try:
                self._cobject = native.FunctionTemplate_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return FunctionTemplate(native.FunctionTemplate_copy( self._cobject ))

    def __del__(self):
        native.FunctionTemplate_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(isolate,callback,data,signature,length,):
        '''
        @returns: v8.Local_v8__FunctionTemplate_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(callback,FunctionCallback))
        assert(isinstance(data,Local_v8__Value_))
        assert(isinstance(signature,Local_v8__Signature_))
        assert(isinstance(length,c_int))
        return Local_v8__FunctionTemplate_(native.v8___FunctionTemplate__New(isolate,callback,data._cobject,signature._cobject,length,))

    def GetFunction(self, *args):
        count = 0
        if count==0 and len(*args) == 0:
            try:
                return self.GetFunction_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.GetFunction_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def GetFunction_2(self, ):
        '''
        @returns: v8.Local_v8__Function_
        '''
        return Local_v8__Function_(native.v8___FunctionTemplate__GetFunction(self._cobject, ))

    def GetFunction_3(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__Function_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__Function_(native.v8___FunctionTemplate__GetFunction(self._cobject, context._cobject,))

    def SetCallHandler(self, callback,data,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,FunctionCallback))
        assert(isinstance(data,Local_v8__Value_))
        return native.v8___FunctionTemplate__SetCallHandler(self._cobject, callback,data._cobject,)

    def SetLength(self, length,):
        '''
        @returns: void
        '''
        assert(isinstance(length,c_int))
        return native.v8___FunctionTemplate__SetLength(self._cobject, length,)

    def InstanceTemplate(self, ):
        '''
        @returns: v8.Local_v8__ObjectTemplate_
        '''
        return Local_v8__ObjectTemplate_(native.v8___FunctionTemplate__InstanceTemplate(self._cobject, ))

    def Inherit(self, parent,):
        '''
        @returns: void
        '''
        assert(isinstance(parent,Local_v8__FunctionTemplate_))
        return native.v8___FunctionTemplate__Inherit(self._cobject, parent._cobject,)

    def PrototypeTemplate(self, ):
        '''
        @returns: v8.Local_v8__ObjectTemplate_
        '''
        return Local_v8__ObjectTemplate_(native.v8___FunctionTemplate__PrototypeTemplate(self._cobject, ))

    def SetClassName(self, name,):
        '''
        @returns: void
        '''
        assert(isinstance(name,Local_v8__String_))
        return native.v8___FunctionTemplate__SetClassName(self._cobject, name._cobject,)

    def SetAcceptAnyReceiver(self, value,):
        '''
        @returns: void
        '''
        assert(isinstance(value,c_char))
        return native.v8___FunctionTemplate__SetAcceptAnyReceiver(self._cobject, value,)

    def SetHiddenPrototype(self, value,):
        '''
        @returns: void
        '''
        assert(isinstance(value,c_char))
        return native.v8___FunctionTemplate__SetHiddenPrototype(self._cobject, value,)

    def ReadOnlyPrototype(self, ):
        '''
        @returns: void
        '''
        return native.v8___FunctionTemplate__ReadOnlyPrototype(self._cobject, )

    def RemovePrototype(self, ):
        '''
        @returns: void
        '''
        return native.v8___FunctionTemplate__RemovePrototype(self._cobject, )

    def HasInstance(self, object,):
        '''
        @returns: c_char
        '''
        assert(isinstance(object,Local_v8__Value_))
        return native.v8___FunctionTemplate__HasInstance(self._cobject, object._cobject,)

    def assign(self, arg1,):
        '''
        @returns: v8.FunctionTemplate
        '''
        assert(isinstance(arg1,POINTER(FunctionTemplate)))
        return (native.v8___FunctionTemplate__assign(self._cobject, arg1._cobject,))        

class HeapProfiler( _FwdDecl_HeapProfiler ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.HeapProfiler_copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return HeapProfiler(native.HeapProfiler_copy( self._cobject ))

    def __del__(self):
        native.HeapProfiler_delete(self._cobject)
        self._cobjet = None
            
            

class ImplementationUtilities( _FwdDecl_ImplementationUtilities ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ImplementationUtilities_copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ImplementationUtilities(native.ImplementationUtilities_copy( self._cobject ))

    def __del__(self):
        native.ImplementationUtilities_delete(self._cobject)
        self._cobjet = None
            
            

class Int32( _FwdDecl_Int32 ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Int32)            
        """
        assert(isinstance(arg1,Int32))
        obj =  Int32\
            ( native.Int32_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Int32)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Int32_copy(args[0])
        else:
            
            try:
                self._cobject = native.Int32_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Int32(native.Int32_copy( self._cobject ))

    def __del__(self):
        native.Int32_delete(self._cobject)
        self._cobjet = None
            
    

    def Value(self, ):
        '''
        @returns: int32_t
        '''
        return native.v8___Int32__Value(self._cobject, )

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Int32
        '''
        assert(isinstance(obj,Value))
        return native.v8___Int32__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Int32__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Int32
        '''
        assert(isinstance(arg1,POINTER(Int32)))
        return (native.v8___Int32__assign(self._cobject, arg1._cobject,))        

class Integer( _FwdDecl_Integer ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Integer)            
        """
        assert(isinstance(arg1,Integer))
        obj =  Integer\
            ( native.Integer_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Integer)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Integer_copy(args[0])
        else:
            
            try:
                self._cobject = native.Integer_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Integer(native.Integer_copy( self._cobject ))

    def __del__(self):
        native.Integer_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(isolate,value,):
        '''
        @returns: v8.Local_v8__Integer_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(value,int32_t))
        return Local_v8__Integer_(native.v8___Integer__New(isolate,value,))


    @staticmethod
    def NewFromUnsigned(isolate,value,):
        '''
        @returns: v8.Local_v8__Integer_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(value,uint32_t))
        return Local_v8__Integer_(native.v8___Integer__NewFromUnsigned(isolate,value,))


    def Value(self, ):
        '''
        @returns: int64_t
        '''
        return native.v8___Integer__Value(self._cobject, )

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Integer
        '''
        assert(isinstance(obj,Value))
        return native.v8___Integer__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Integer__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Integer
        '''
        assert(isinstance(arg1,POINTER(Integer)))
        return (native.v8___Integer__assign(self._cobject, arg1._cobject,))


class CreateParams( _FwdDecl_CreateParams):
    _fields_ = [ ("entry_hook",FunctionEntryHook),
    ("code_event_handler",JitCodeEventHandler),
    ("constraints",v8.ResourceConstraints),
    ("snapshot_blob",v8.StartupData),
    ("counter_lookup_callback",CounterLookupCallback),
    ("create_histogram_callback",CreateHistogramCallback),
    ("add_histogram_sample_callback",AddHistogramSampleCallback),
    ("array_buffer_allocator",v8.Allocator),
    ]




        

class Scope( _FwdDecl_Scope ):

    @staticmethod
    def new1(self, isolate,):
        """
        Signature: 
           isolate:v8.Isolate            
        """
        assert(isinstance(isolate,Scope))
        obj =  Scope\
            ( native.Scope_new(isolate, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Isolate            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Scope_copy(args[0])
        else:
            
            try:
                self._cobject = native.Scope_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Scope(native.Scope_copy( self._cobject ))

    def __del__(self):
        native.Scope_delete(self._cobject)
        self._cobjet = None
            
    

    def assign(self, arg1,):
        '''
        @returns: v8.Scope
        '''
        assert(isinstance(arg1,POINTER(Scope)))
        return (native.v8___Scope__assign(self._cobject, arg1._cobject,))        

class DisallowJavascriptExecutionScope( _FwdDecl_DisallowJavascriptExecutionScope ):

    @staticmethod
    def new1(self, isolate,on_failure,):
        """
        Signature: 
           isolate:v8.Isolate            
        
           on_failure:c_uint            
        """
        assert(isinstance(isolate,DisallowJavascriptExecutionScope))
        assert(isinstance(on_failure,DisallowJavascriptExecutionScope))
        obj =  DisallowJavascriptExecutionScope\
            ( native.DisallowJavascriptExecutionScope_new(isolate, on_failure._cobject
    
    
  , ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Isolate            
        
           args[1]:c_uint            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.DisallowJavascriptExecutionScope_copy(args[0])
        else:
            
            try:
                self._cobject = native.DisallowJavascriptExecutionScope_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return DisallowJavascriptExecutionScope(native.DisallowJavascriptExecutionScope_copy( self._cobject ))

    def __del__(self):
        native.DisallowJavascriptExecutionScope_delete(self._cobject)
        self._cobjet = None
            
    

    def assign(self, arg1,):
        '''
        @returns: v8.DisallowJavascriptExecutionScope
        '''
        assert(isinstance(arg1,POINTER(DisallowJavascriptExecutionScope)))
        return (native.v8___DisallowJavascriptExecutionScope__assign(self._cobject, arg1._cobject,))        

class AllowJavascriptExecutionScope( _FwdDecl_AllowJavascriptExecutionScope ):

    @staticmethod
    def new1(self, isolate,):
        """
        Signature: 
           isolate:v8.Isolate            
        """
        assert(isinstance(isolate,AllowJavascriptExecutionScope))
        obj =  AllowJavascriptExecutionScope\
            ( native.AllowJavascriptExecutionScope_new(isolate, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Isolate            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.AllowJavascriptExecutionScope_copy(args[0])
        else:
            
            try:
                self._cobject = native.AllowJavascriptExecutionScope_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return AllowJavascriptExecutionScope(native.AllowJavascriptExecutionScope_copy( self._cobject ))

    def __del__(self):
        native.AllowJavascriptExecutionScope_delete(self._cobject)
        self._cobjet = None
            
    

    def assign(self, arg1,):
        '''
        @returns: v8.AllowJavascriptExecutionScope
        '''
        assert(isinstance(arg1,POINTER(AllowJavascriptExecutionScope)))
        return (native.v8___AllowJavascriptExecutionScope__assign(self._cobject, arg1._cobject,))        

class SuppressMicrotaskExecutionScope( _FwdDecl_SuppressMicrotaskExecutionScope ):

    @staticmethod
    def new1(self, isolate,):
        """
        Signature: 
           isolate:v8.Isolate            
        """
        assert(isinstance(isolate,SuppressMicrotaskExecutionScope))
        obj =  SuppressMicrotaskExecutionScope\
            ( native.SuppressMicrotaskExecutionScope_new(isolate, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Isolate            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.SuppressMicrotaskExecutionScope_copy(args[0])
        else:
            
            try:
                self._cobject = native.SuppressMicrotaskExecutionScope_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return SuppressMicrotaskExecutionScope(native.SuppressMicrotaskExecutionScope_copy( self._cobject ))

    def __del__(self):
        native.SuppressMicrotaskExecutionScope_delete(self._cobject)
        self._cobjet = None
            
    

    def assign(self, arg1,):
        '''
        @returns: v8.SuppressMicrotaskExecutionScope
        '''
        assert(isinstance(arg1,POINTER(SuppressMicrotaskExecutionScope)))
        return (native.v8___SuppressMicrotaskExecutionScope__assign(self._cobject, arg1._cobject,))


        

class Isolate( _FwdDecl_Isolate ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Isolate_copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Isolate(native.Isolate_copy( self._cobject ))

    def __del__(self):
        native.Isolate_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return Isolate.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return Isolate.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(params,):
        '''
        @returns: v8.Isolate
        '''
        assert(isinstance(params,POINTER(CreateParams)))
        return native.v8___Isolate__New(params._cobject,)

    @staticmethod
    def New_2():
        '''
        @returns: v8.Isolate
        '''
        return native.v8___Isolate__New()

    @staticmethod
    def GetCurrent():
        '''
        @returns: v8.Isolate
        '''
        return native.v8___Isolate__GetCurrent()

    def Enter(self, ):
        '''
        @returns: void
        '''
        return native.v8___Isolate__Enter(self._cobject, )

    def Exit(self, ):
        '''
        @returns: void
        '''
        return native.v8___Isolate__Exit(self._cobject, )

    def Dispose(self, ):
        '''
        @returns: void
        '''
        return native.v8___Isolate__Dispose(self._cobject, )

    def SetData(self, slot,data,):
        '''
        @returns: void
        '''
        assert(isinstance(slot,uint32_t))
        assert(isinstance(data,c_void_p))
        return native.v8___Isolate__SetData(self._cobject, slot,data,)


    def GetData(self, slot,):
        '''
        @returns: c_void_p
        '''
        assert(isinstance(slot,uint32_t))
        return native.v8___Isolate__GetData(self._cobject, slot,)


    @staticmethod
    def GetNumberOfDataSlots():
        '''
        @returns: uint32_t
        '''
        return native.v8___Isolate__GetNumberOfDataSlots()

    def GetHeapStatistics(self, heap_statistics,):
        '''
        @returns: void
        '''
        assert(isinstance(heap_statistics,HeapStatistics))
        return native.v8___Isolate__GetHeapStatistics(self._cobject, heap_statistics,)

    def NumberOfHeapSpaces(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___Isolate__NumberOfHeapSpaces(self._cobject, )

    def GetHeapSpaceStatistics(self, space_statistics,index,):
        '''
        @returns: c_char
        '''
        assert(isinstance(space_statistics,HeapSpaceStatistics))
        assert(isinstance(index,size_t))
        return native.v8___Isolate__GetHeapSpaceStatistics(self._cobject, space_statistics,index,)


    def NumberOfTrackedHeapObjectTypes(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___Isolate__NumberOfTrackedHeapObjectTypes(self._cobject, )

    def GetHeapObjectStatisticsAtLastGC(self, object_statistics,type_index,):
        '''
        @returns: c_char
        '''
        assert(isinstance(object_statistics,HeapObjectStatistics))
        assert(isinstance(type_index,size_t))
        return native.v8___Isolate__GetHeapObjectStatisticsAtLastGC(self._cobject, object_statistics,type_index,)


    def GetStackSample(self, state,frames,frames_limit,sample_info,):
        '''
        @returns: void
        '''
        assert(isinstance(state,POINTER(RegisterState)))
        assert(isinstance(frames,POINTER(c_void_p)))
        assert(isinstance(frames_limit,size_t))
        assert(isinstance(sample_info,SampleInfo))
        return native.v8___Isolate__GetStackSample(self._cobject, state._cobject,frames,frames_limit,sample_info,)


    def AdjustAmountOfExternalAllocatedMemory(self, change_in_bytes,):
        '''
        @returns: int64_t
        '''
        assert(isinstance(change_in_bytes,int64_t))
        return native.v8___Isolate__AdjustAmountOfExternalAllocatedMemory(self._cobject, change_in_bytes,)


    def GetHeapProfiler(self, ):
        '''
        @returns: v8.HeapProfiler
        '''
        return native.v8___Isolate__GetHeapProfiler(self._cobject, )

    def GetCpuProfiler(self, ):
        '''
        @returns: v8.CpuProfiler
        '''
        return native.v8___Isolate__GetCpuProfiler(self._cobject, )

    def InContext(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Isolate__InContext(self._cobject, )

    def GetCurrentContext(self, ):
        '''
        @returns: v8.Local_v8__Context_
        '''
        return Local_v8__Context_(native.v8___Isolate__GetCurrentContext(self._cobject, ))

    def GetCallingContext(self, ):
        '''
        @returns: v8.Local_v8__Context_
        '''
        return Local_v8__Context_(native.v8___Isolate__GetCallingContext(self._cobject, ))

    def GetEnteredContext(self, ):
        '''
        @returns: v8.Local_v8__Context_
        '''
        return Local_v8__Context_(native.v8___Isolate__GetEnteredContext(self._cobject, ))

    def ThrowException(self, exception,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(exception,Local_v8__Value_))
        return Local_v8__Value_(native.v8___Isolate__ThrowException(self._cobject, exception._cobject,))

    def AddGCPrologueCallback(self, callback,gc_type_filter,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,GCPrologueCallback))
        assert(isinstance(gc_type_filter,c_uint))
        return native.v8___Isolate__AddGCPrologueCallback(self._cobject, callback,gc_type_filter._cobject
    
    
    
  ,)
    
    
    
  

    def RemoveGCPrologueCallback(self, callback,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,GCPrologueCallback))
        return native.v8___Isolate__RemoveGCPrologueCallback(self._cobject, callback,)

    def AddGCEpilogueCallback(self, callback,gc_type_filter,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,GCEpilogueCallback))
        assert(isinstance(gc_type_filter,c_uint))
        return native.v8___Isolate__AddGCEpilogueCallback(self._cobject, callback,gc_type_filter._cobject
    
    
    
  ,)
    
    
    
  

    def RemoveGCEpilogueCallback(self, callback,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,GCEpilogueCallback))
        return native.v8___Isolate__RemoveGCEpilogueCallback(self._cobject, callback,)

    def TerminateExecution(self, ):
        '''
        @returns: void
        '''
        return native.v8___Isolate__TerminateExecution(self._cobject, )

    def IsExecutionTerminating(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Isolate__IsExecutionTerminating(self._cobject, )

    def CancelTerminateExecution(self, ):
        '''
        @returns: void
        '''
        return native.v8___Isolate__CancelTerminateExecution(self._cobject, )

    def RequestInterrupt(self, callback,data,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,InterruptCallback))
        assert(isinstance(data,c_void_p))
        return native.v8___Isolate__RequestInterrupt(self._cobject, callback,data,)

    def RequestGarbageCollectionForTesting(self, type,):
        '''
        @returns: void
        '''
        assert(isinstance(type,c_uint))
        return native.v8___Isolate__RequestGarbageCollectionForTesting(self._cobject, type._cobject
    
    
  ,)
    
    
  

    def SetEventLogger(self, that,):
        '''
        @returns: void
        '''
        assert(isinstance(that,LogEventCallback))
        return native.v8___Isolate__SetEventLogger(self._cobject, that,)

    def AddCallCompletedCallback(self, callback,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,CallCompletedCallback))
        return native.v8___Isolate__AddCallCompletedCallback(self._cobject, callback,)

    def RemoveCallCompletedCallback(self, callback,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,CallCompletedCallback))
        return native.v8___Isolate__RemoveCallCompletedCallback(self._cobject, callback,)

    def SetPromiseRejectCallback(self, callback,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,PromiseRejectCallback))
        return native.v8___Isolate__SetPromiseRejectCallback(self._cobject, callback,)

    def RunMicrotasks(self, ):
        '''
        @returns: void
        '''
        return native.v8___Isolate__RunMicrotasks(self._cobject, )

    def EnqueueMicrotask(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.EnqueueMicrotask_34(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.EnqueueMicrotask_35(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def EnqueueMicrotask_38(self, microtask,):
        '''
        @returns: void
        '''
        assert(isinstance(microtask,Local_v8__Function_))
        return native.v8___Isolate__EnqueueMicrotask(self._cobject, microtask._cobject,)

    def EnqueueMicrotask_39(self, microtask,data,):
        '''
        @returns: void
        '''
        assert(isinstance(microtask,MicrotaskCallback))
        assert(isinstance(data,c_void_p))
        return native.v8___Isolate__EnqueueMicrotask(self._cobject, microtask,data,)

    def SetAutorunMicrotasks(self, autorun,):
        '''
        @returns: void
        '''
        assert(isinstance(autorun,c_char))
        return native.v8___Isolate__SetAutorunMicrotasks(self._cobject, autorun,)

    def WillAutorunMicrotasks(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Isolate__WillAutorunMicrotasks(self._cobject, )

    def SetUseCounterCallback(self, callback,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,UseCounterCallback))
        return native.v8___Isolate__SetUseCounterCallback(self._cobject, callback,)

    def SetCounterFunction(self, arg1,):
        '''
        @returns: void
        '''
        assert(isinstance(arg1,CounterLookupCallback))
        return native.v8___Isolate__SetCounterFunction(self._cobject, arg1,)

    def SetCreateHistogramFunction(self, arg1,):
        '''
        @returns: void
        '''
        assert(isinstance(arg1,CreateHistogramCallback))
        return native.v8___Isolate__SetCreateHistogramFunction(self._cobject, arg1,)

    def SetAddHistogramSampleFunction(self, arg1,):
        '''
        @returns: void
        '''
        assert(isinstance(arg1,AddHistogramSampleCallback))
        return native.v8___Isolate__SetAddHistogramSampleFunction(self._cobject, arg1,)

    def IdleNotificationDeadline(self, deadline_in_seconds,):
        '''
        @returns: c_char
        '''
        assert(isinstance(deadline_in_seconds,c_double))
        return native.v8___Isolate__IdleNotificationDeadline(self._cobject, deadline_in_seconds,)

    def IdleNotification(self, idle_time_in_ms,):
        '''
        @returns: c_char
        '''
        assert(isinstance(idle_time_in_ms,c_int))
        return native.v8___Isolate__IdleNotification(self._cobject, idle_time_in_ms,)

    def LowMemoryNotification(self, ):
        '''
        @returns: void
        '''
        return native.v8___Isolate__LowMemoryNotification(self._cobject, )

    def ContextDisposedNotification(self, dependant_context,):
        '''
        @returns: c_int
        '''
        assert(isinstance(dependant_context,c_char))
        return native.v8___Isolate__ContextDisposedNotification(self._cobject, dependant_context,)

    def SetJitCodeEventHandler(self, options,event_handler,):
        '''
        @returns: void
        '''
        assert(isinstance(options,c_uint))
        assert(isinstance(event_handler,JitCodeEventHandler))
        return native.v8___Isolate__SetJitCodeEventHandler(self._cobject, options._cobject
    
    
  ,event_handler,)
    
    
  

    def SetStackLimit(self, stack_limit,):
        '''
        @returns: void
        '''
        assert(isinstance(stack_limit,uintptr_t))
        return native.v8___Isolate__SetStackLimit(self._cobject, stack_limit,)


    def GetCodeRange(self, start,length_in_bytes,):
        '''
        @returns: void
        '''
        assert(isinstance(start,POINTER(c_void_p)))
        assert(isinstance(length_in_bytes,POINTER(size_t)))
        return native.v8___Isolate__GetCodeRange(self._cobject, start,length_in_bytes,)


    def SetFatalErrorHandler(self, that,):
        '''
        @returns: void
        '''
        assert(isinstance(that,FatalErrorCallback))
        return native.v8___Isolate__SetFatalErrorHandler(self._cobject, that,)

    def SetAllowCodeGenerationFromStringsCallback(self, callback,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,AllowCodeGenerationFromStringsCallback))
        return native.v8___Isolate__SetAllowCodeGenerationFromStringsCallback(self._cobject, callback,)

    def IsDead(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Isolate__IsDead(self._cobject, )

    def AddMessageListener(self, that,data,):
        '''
        @returns: c_char
        '''
        assert(isinstance(that,MessageCallback))
        assert(isinstance(data,Local_v8__Value_))
        return native.v8___Isolate__AddMessageListener(self._cobject, that,data._cobject,)

    def RemoveMessageListeners(self, that,):
        '''
        @returns: void
        '''
        assert(isinstance(that,MessageCallback))
        return native.v8___Isolate__RemoveMessageListeners(self._cobject, that,)

    def SetFailedAccessCheckCallbackFunction(self, arg1,):
        '''
        @returns: void
        '''
        assert(isinstance(arg1,FailedAccessCheckCallback))
        return native.v8___Isolate__SetFailedAccessCheckCallbackFunction(self._cobject, arg1,)

    def SetCaptureStackTraceForUncaughtExceptions(self, capture,frame_limit,options,):
        '''
        @returns: void
        '''
        assert(isinstance(capture,c_char))
        assert(isinstance(frame_limit,c_int))
        assert(isinstance(options,c_uint))
        return native.v8___Isolate__SetCaptureStackTraceForUncaughtExceptions(self._cobject, capture,frame_limit,options._cobject
    
    
    
    
    
    
    
    
    
    
    
  ,)
    
    
    
    
    
    
    
    
    
    
    
  

    def AddMemoryAllocationCallback(self, callback,space,action,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,MemoryAllocationCallback))
        assert(isinstance(space,c_uint))
        assert(isinstance(action,c_uint))
        return native.v8___Isolate__AddMemoryAllocationCallback(self._cobject, callback,space._cobject
    
    
    
    
    
    
  ,action._cobject
    
    
    
  ,)
    
    
    
    
    
    
  
    
    
    
  

    def RemoveMemoryAllocationCallback(self, callback,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,MemoryAllocationCallback))
        return native.v8___Isolate__RemoveMemoryAllocationCallback(self._cobject, callback,)

    def VisitExternalResources(self, visitor,):
        '''
        @returns: void
        '''
        assert(isinstance(visitor,ExternalResourceVisitor))
        return native.v8___Isolate__VisitExternalResources(self._cobject, visitor,)

    def VisitHandlesWithClassIds(self, visitor,):
        '''
        @returns: void
        '''
        assert(isinstance(visitor,PersistentHandleVisitor))
        return native.v8___Isolate__VisitHandlesWithClassIds(self._cobject, visitor,)

    def VisitHandlesForPartialDependence(self, visitor,):
        '''
        @returns: void
        '''
        assert(isinstance(visitor,PersistentHandleVisitor))
        return native.v8___Isolate__VisitHandlesForPartialDependence(self._cobject, visitor,)

    def assign(self, arg1,):
        '''
        @returns: v8.Isolate
        '''
        assert(isinstance(arg1,POINTER(Isolate)))
        return (native.v8___Isolate__assign(self._cobject, arg1._cobject,))
    
  
    
    
  

    def SetObjectGroupId(self, object,id,):
        '''
        @returns: void
        '''
        assert(isinstance(object,POINTER(v8.internal.Object)))
        assert(isinstance(id,UniqueId))
        return native.v8___Isolate__SetObjectGroupId(self._cobject, object,id._cobject,)


    def SetReferenceFromGroup(self, id,object,):
        '''
        @returns: void
        '''
        assert(isinstance(id,UniqueId))
        assert(isinstance(object,POINTER(v8.internal.Object)))
        return native.v8___Isolate__SetReferenceFromGroup(self._cobject, id._cobject,object,)


    def SetReference(self, parent,child,):
        '''
        @returns: void
        '''
        assert(isinstance(parent,POINTER(v8.internal.Object)))
        assert(isinstance(child,POINTER(v8.internal.Object)))
        return native.v8___Isolate__SetReference(self._cobject, parent,child,)



    def CollectAllGarbage(self, gc_reason,):
        '''
        @returns: void
        '''
        assert(isinstance(gc_reason,c_char_p))
        return native.v8___Isolate__CollectAllGarbage(self._cobject, gc_reason,)        

class Maybe_int_( _FwdDecl_Maybe_int_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Maybe_int__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Maybe_int_(native.Maybe_int__copy( self._cobject ))

    def __del__(self):
        native.Maybe_int__delete(self._cobject)
        self._cobjet = None
            
            

class Maybe_bool_( _FwdDecl_Maybe_bool_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Maybe_bool__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Maybe_bool_(native.Maybe_bool__copy( self._cobject ))

    def __del__(self):
        native.Maybe_bool__delete(self._cobject)
        self._cobjet = None
            
            

class Maybe_double_( _FwdDecl_Maybe_double_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Maybe_double__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Maybe_double_(native.Maybe_double__copy( self._cobject ))

    def __del__(self):
        native.Maybe_double__delete(self._cobject)
        self._cobjet = None
            
            

class Maybe_long_( _FwdDecl_Maybe_long_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Maybe_long__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Maybe_long_(native.Maybe_long__copy( self._cobject ))

    def __del__(self):
        native.Maybe_long__delete(self._cobject)
        self._cobjet = None
            
            

class Maybe_unsigned_int_( _FwdDecl_Maybe_unsigned_int_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Maybe_unsigned_int__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Maybe_unsigned_int_(native.Maybe_unsigned_int__copy( self._cobject ))

    def __del__(self):
        native.Maybe_unsigned_int__delete(self._cobject)
        self._cobjet = None
            
            

class Maybe_v8__PropertyAttribute_( _FwdDecl_Maybe_v8__PropertyAttribute_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Maybe_v8__PropertyAttribute__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Maybe_v8__PropertyAttribute_(native.Maybe_v8__PropertyAttribute__copy( self._cobject ))

    def __del__(self):
        native.Maybe_v8__PropertyAttribute__delete(self._cobject)
        self._cobjet = None
            
            

class Name( _FwdDecl_Name ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Name)            
        """
        assert(isinstance(arg1,Name))
        obj =  Name\
            ( native.Name_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Name)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Name_copy(args[0])
        else:
            
            try:
                self._cobject = native.Name_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Name(native.Name_copy( self._cobject ))

    def __del__(self):
        native.Name_delete(self._cobject)
        self._cobjet = None
            
    

    def GetIdentityHash(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___Name__GetIdentityHash(self._cobject, )

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Name
        '''
        assert(isinstance(obj,Value))
        return native.v8___Name__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Name__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Name
        '''
        assert(isinstance(arg1,POINTER(Name)))
        return (native.v8___Name__assign(self._cobject, arg1._cobject,))        

class Number( _FwdDecl_Number ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Number)            
        """
        assert(isinstance(arg1,Number))
        obj =  Number\
            ( native.Number_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Number)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Number_copy(args[0])
        else:
            
            try:
                self._cobject = native.Number_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Number(native.Number_copy( self._cobject ))

    def __del__(self):
        native.Number_delete(self._cobject)
        self._cobjet = None
            
    

    def Value(self, ):
        '''
        @returns: c_double
        '''
        return native.v8___Number__Value(self._cobject, )

    @staticmethod
    def New(isolate,value,):
        '''
        @returns: v8.Local_v8__Number_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(value,c_double))
        return Local_v8__Number_(native.v8___Number__New(isolate,value,))

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Number
        '''
        assert(isinstance(obj,Value))
        return native.v8___Number__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Number__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Number
        '''
        assert(isinstance(arg1,POINTER(Number)))
        return (native.v8___Number__assign(self._cobject, arg1._cobject,))        

class NumberObject( _FwdDecl_NumberObject ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.NumberObject)            
        """
        assert(isinstance(arg1,NumberObject))
        obj =  NumberObject\
            ( native.NumberObject_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.NumberObject)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.NumberObject_copy(args[0])
        else:
            
            try:
                self._cobject = native.NumberObject_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return NumberObject(native.NumberObject_copy( self._cobject ))

    def __del__(self):
        native.NumberObject_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(isolate,value,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(value,c_double))
        return Local_v8__Value_(native.v8___NumberObject__New(isolate,value,))

    def ValueOf(self, ):
        '''
        @returns: c_double
        '''
        return native.v8___NumberObject__ValueOf(self._cobject, )

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.NumberObject
        '''
        assert(isinstance(obj,Value))
        return native.v8___NumberObject__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___NumberObject__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.NumberObject
        '''
        assert(isinstance(arg1,POINTER(NumberObject)))
        return (native.v8___NumberObject__assign(self._cobject, arg1._cobject,))        

class Object( _FwdDecl_Object ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Object)            
        """
        assert(isinstance(arg1,Object))
        obj =  Object\
            ( native.Object_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Object)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Object_copy(args[0])
        else:
            
            try:
                self._cobject = native.Object_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Object(native.Object_copy( self._cobject ))

    def __del__(self):
        native.Object_delete(self._cobject)
        self._cobjet = None
            
    

    def Set(self, *args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return self.Set_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return self.Set_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.Set_3(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return self.Set_4(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def Set_1(self, key,value,):
        '''
        @returns: c_char
        '''
        assert(isinstance(key,Local_v8__Value_))
        assert(isinstance(value,Local_v8__Value_))
        return native.v8___Object__Set(self._cobject, key._cobject,value._cobject,)

    def Set_2(self, context,key,value,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__Value_))
        assert(isinstance(value,Local_v8__Value_))
        return Maybe_bool_(native.v8___Object__Set(self._cobject, context._cobject,key._cobject,value._cobject,))

    def Set_3(self, index,value,):
        '''
        @returns: c_char
        '''
        assert(isinstance(index,uint32_t))
        assert(isinstance(value,Local_v8__Value_))
        return native.v8___Object__Set(self._cobject, index,value._cobject,)


    def Set_4(self, context,index,value,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(index,uint32_t))
        assert(isinstance(value,Local_v8__Value_))
        return Maybe_bool_(native.v8___Object__Set(self._cobject, context._cobject,index,value._cobject,))


    def CreateDataProperty(self, *args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return self.CreateDataProperty_5(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return self.CreateDataProperty_6(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def CreateDataProperty_5(self, context,key,value,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__Name_))
        assert(isinstance(value,Local_v8__Value_))
        return Maybe_bool_(native.v8___Object__CreateDataProperty(self._cobject, context._cobject,key._cobject,value._cobject,))

    def CreateDataProperty_6(self, context,index,value,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(index,uint32_t))
        assert(isinstance(value,Local_v8__Value_))
        return Maybe_bool_(native.v8___Object__CreateDataProperty(self._cobject, context._cobject,index,value._cobject,))


    def DefineOwnProperty(self, context,key,value,attributes,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__Name_))
        assert(isinstance(value,Local_v8__Value_))
        assert(isinstance(attributes,c_uint))
        return Maybe_bool_(native.v8___Object__DefineOwnProperty(self._cobject, context._cobject,key._cobject,value._cobject,attributes._cobject
    
    
    
    
  ,))
    
    
    
    
  

    def ForceSet(self, *args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return self.ForceSet_8(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 4:
            try:
                return self.ForceSet_9(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def ForceSet_8(self, key,value,attribs,):
        '''
        @returns: c_char
        '''
        assert(isinstance(key,Local_v8__Value_))
        assert(isinstance(value,Local_v8__Value_))
        assert(isinstance(attribs,c_uint))
        return native.v8___Object__ForceSet(self._cobject, key._cobject,value._cobject,attribs._cobject
    
    
    
    
  ,)
    
    
    
    
  

    def ForceSet_9(self, context,key,value,attribs,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__Value_))
        assert(isinstance(value,Local_v8__Value_))
        assert(isinstance(attribs,c_uint))
        return Maybe_bool_(native.v8___Object__ForceSet(self._cobject, context._cobject,key._cobject,value._cobject,attribs._cobject
    
    
    
    
  ,))
    
    
    
    
  

    def Get(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.Get_10(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.Get_11(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.Get_12(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.Get_13(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def Get_10(self, key,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(key,Local_v8__Value_))
        return Local_v8__Value_(native.v8___Object__Get(self._cobject, key._cobject,))

    def Get_11(self, context,key,):
        '''
        @returns: v8.MaybeLocal_v8__Value_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__Value_))
        return MaybeLocal_v8__Value_(native.v8___Object__Get(self._cobject, context._cobject,key._cobject,))

    def Get_12(self, index,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(index,uint32_t))
        return Local_v8__Value_(native.v8___Object__Get(self._cobject, index,))


    def Get_13(self, context,index,):
        '''
        @returns: v8.MaybeLocal_v8__Value_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(index,uint32_t))
        return MaybeLocal_v8__Value_(native.v8___Object__Get(self._cobject, context._cobject,index,))


    def GetPropertyAttributes(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.GetPropertyAttributes_14(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.GetPropertyAttributes_15(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def GetPropertyAttributes_14(self, key,):
        '''
        @returns: c_uint
        '''
        assert(isinstance(key,Local_v8__Value_))
        return native.v8___Object__GetPropertyAttributes(self._cobject, key._cobject,)

    def GetPropertyAttributes_15(self, context,key,):
        '''
        @returns: v8.Maybe_v8__PropertyAttribute_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__Value_))
        return Maybe_v8__PropertyAttribute_(native.v8___Object__GetPropertyAttributes(self._cobject, context._cobject,key._cobject,))

    def GetOwnPropertyDescriptor(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.GetOwnPropertyDescriptor_16(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.GetOwnPropertyDescriptor_17(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def GetOwnPropertyDescriptor_16(self, key,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(key,Local_v8__String_))
        return Local_v8__Value_(native.v8___Object__GetOwnPropertyDescriptor(self._cobject, key._cobject,))

    def GetOwnPropertyDescriptor_17(self, context,key,):
        '''
        @returns: v8.MaybeLocal_v8__Value_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__String_))
        return MaybeLocal_v8__Value_(native.v8___Object__GetOwnPropertyDescriptor(self._cobject, context._cobject,key._cobject,))

    def Has(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.Has_18(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.Has_19(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.Has_22(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.Has_23(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def Has_18(self, key,):
        '''
        @returns: c_char
        '''
        assert(isinstance(key,Local_v8__Value_))
        return native.v8___Object__Has(self._cobject, key._cobject,)

    def Has_19(self, context,key,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__Value_))
        return Maybe_bool_(native.v8___Object__Has(self._cobject, context._cobject,key._cobject,))

    def Delete(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.Delete_20(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.Delete_21(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.Delete_24(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.Delete_25(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def Delete_20(self, key,):
        '''
        @returns: c_char
        '''
        assert(isinstance(key,Local_v8__Value_))
        return native.v8___Object__Delete(self._cobject, key._cobject,)

    def Delete_21(self, context,key,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__Value_))
        return Maybe_bool_(native.v8___Object__Delete(self._cobject, context._cobject,key._cobject,))

    def Has_22(self, index,):
        '''
        @returns: c_char
        '''
        assert(isinstance(index,uint32_t))
        return native.v8___Object__Has(self._cobject, index,)


    def Has_23(self, context,index,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(index,uint32_t))
        return Maybe_bool_(native.v8___Object__Has(self._cobject, context._cobject,index,))


    def Delete_24(self, index,):
        '''
        @returns: c_char
        '''
        assert(isinstance(index,uint32_t))
        return native.v8___Object__Delete(self._cobject, index,)


    def Delete_25(self, context,index,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(index,uint32_t))
        return Maybe_bool_(native.v8___Object__Delete(self._cobject, context._cobject,index,))


    def SetAccessor(self, *args):
        count = 0
        if count==0 and len(*args) == 6:
            try:
                return self.SetAccessor_26(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 6:
            try:
                return self.SetAccessor_27(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 7:
            try:
                return self.SetAccessor_28(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def SetAccessor_26(self, name,getter,setter,data,settings,attribute,):
        '''
        @returns: c_char
        '''
        assert(isinstance(name,Local_v8__String_))
        assert(isinstance(getter,AccessorGetterCallback))
        assert(isinstance(setter,AccessorSetterCallback))
        assert(isinstance(data,Local_v8__Value_))
        assert(isinstance(settings,c_uint))
        assert(isinstance(attribute,c_uint))
        return native.v8___Object__SetAccessor(self._cobject, name._cobject,getter,setter,data._cobject,settings._cobject
    
    
    
    
  ,attribute._cobject
    
    
    
    
  ,)
    
    
    
    
  
    
    
    
    
  

    def SetAccessor_27(self, name,getter,setter,data,settings,attribute,):
        '''
        @returns: c_char
        '''
        assert(isinstance(name,Local_v8__Name_))
        assert(isinstance(getter,AccessorNameGetterCallback))
        assert(isinstance(setter,AccessorNameSetterCallback))
        assert(isinstance(data,Local_v8__Value_))
        assert(isinstance(settings,c_uint))
        assert(isinstance(attribute,c_uint))
        return native.v8___Object__SetAccessor(self._cobject, name._cobject,getter,setter,data._cobject,settings._cobject
    
    
    
    
  ,attribute._cobject
    
    
    
    
  ,)
    
    
    
    
  
    
    
    
    
  

    def SetAccessor_28(self, context,name,getter,setter,data,settings,attribute,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(name,Local_v8__Name_))
        assert(isinstance(getter,AccessorNameGetterCallback))
        assert(isinstance(setter,AccessorNameSetterCallback))
        assert(isinstance(data,MaybeLocal_v8__Value_))
        assert(isinstance(settings,c_uint))
        assert(isinstance(attribute,c_uint))
        return Maybe_bool_(native.v8___Object__SetAccessor(self._cobject, context._cobject,name._cobject,getter,setter,data._cobject,settings._cobject
    
    
    
    
  ,attribute._cobject
    
    
    
    
  ,))
    
    
    
    
  
    
    
    
    
  

    def SetAccessorProperty(self, name,getter,setter,attribute,settings,):
        '''
        @returns: void
        '''
        assert(isinstance(name,Local_v8__Name_))
        assert(isinstance(getter,Local_v8__Function_))
        assert(isinstance(setter,Local_v8__Function_))
        assert(isinstance(attribute,c_uint))
        assert(isinstance(settings,c_uint))
        return native.v8___Object__SetAccessorProperty(self._cobject, name._cobject,getter._cobject,setter._cobject,attribute._cobject
    
    
    
    
  ,settings._cobject
    
    
    
    
  ,)
    
    
    
    
  
    
    
    
    
  

    def GetPropertyNames(self, *args):
        count = 0
        if count==0 and len(*args) == 0:
            try:
                return self.GetPropertyNames_30(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.GetPropertyNames_31(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def GetPropertyNames_30(self, ):
        '''
        @returns: v8.Local_v8__Array_
        '''
        return Local_v8__Array_(native.v8___Object__GetPropertyNames(self._cobject, ))

    def GetPropertyNames_31(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__Array_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__Array_(native.v8___Object__GetPropertyNames(self._cobject, context._cobject,))

    def GetOwnPropertyNames(self, *args):
        count = 0
        if count==0 and len(*args) == 0:
            try:
                return self.GetOwnPropertyNames_32(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.GetOwnPropertyNames_33(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def GetOwnPropertyNames_32(self, ):
        '''
        @returns: v8.Local_v8__Array_
        '''
        return Local_v8__Array_(native.v8___Object__GetOwnPropertyNames(self._cobject, ))

    def GetOwnPropertyNames_33(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__Array_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__Array_(native.v8___Object__GetOwnPropertyNames(self._cobject, context._cobject,))

    def GetPrototype(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___Object__GetPrototype(self._cobject, ))

    def SetPrototype(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.SetPrototype_35(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.SetPrototype_36(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def SetPrototype_35(self, prototype,):
        '''
        @returns: c_char
        '''
        assert(isinstance(prototype,Local_v8__Value_))
        return native.v8___Object__SetPrototype(self._cobject, prototype._cobject,)

    def SetPrototype_36(self, context,prototype,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(prototype,Local_v8__Value_))
        return Maybe_bool_(native.v8___Object__SetPrototype(self._cobject, context._cobject,prototype._cobject,))

    def FindInstanceInPrototypeChain(self, tmpl,):
        '''
        @returns: v8.Local_v8__Object_
        '''
        assert(isinstance(tmpl,Local_v8__FunctionTemplate_))
        return Local_v8__Object_(native.v8___Object__FindInstanceInPrototypeChain(self._cobject, tmpl._cobject,))

    def ObjectProtoToString(self, *args):
        count = 0
        if count==0 and len(*args) == 0:
            try:
                return self.ObjectProtoToString_38(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.ObjectProtoToString_39(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def ObjectProtoToString_38(self, ):
        '''
        @returns: v8.Local_v8__String_
        '''
        return Local_v8__String_(native.v8___Object__ObjectProtoToString(self._cobject, ))

    def ObjectProtoToString_39(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__String_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__String_(native.v8___Object__ObjectProtoToString(self._cobject, context._cobject,))

    def GetConstructorName(self, ):
        '''
        @returns: v8.Local_v8__String_
        '''
        return Local_v8__String_(native.v8___Object__GetConstructorName(self._cobject, ))

    def InternalFieldCount(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___Object__InternalFieldCount(self._cobject, )

    @staticmethod
    def InternalFieldCount(object,):
        '''
        @returns: c_int
        '''
        assert(isinstance(object,POINTER(PersistentBase_v8__Object_)))
        return native.v8___Object__InternalFieldCount(object._cobject,)

    def GetInternalField(self, index,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(index,c_int))
        return Local_v8__Value_(native.v8___Object__GetInternalField(self._cobject, index,))

    def SetInternalField(self, index,value,):
        '''
        @returns: void
        '''
        assert(isinstance(index,c_int))
        assert(isinstance(value,Local_v8__Value_))
        return native.v8___Object__SetInternalField(self._cobject, index,value._cobject,)

    def GetAlignedPointerFromInternalField(self, index,):
        '''
        @returns: c_void_p
        '''
        assert(isinstance(index,c_int))
        return native.v8___Object__GetAlignedPointerFromInternalField(self._cobject, index,)

    @staticmethod
    def GetAlignedPointerFromInternalField(object,index,):
        '''
        @returns: c_void_p
        '''
        assert(isinstance(object,POINTER(PersistentBase_v8__Object_)))
        assert(isinstance(index,c_int))
        return native.v8___Object__GetAlignedPointerFromInternalField(object._cobject,index,)

    def SetAlignedPointerInInternalField(self, index,value,):
        '''
        @returns: void
        '''
        assert(isinstance(index,c_int))
        assert(isinstance(value,c_void_p))
        return native.v8___Object__SetAlignedPointerInInternalField(self._cobject, index,value,)

    def HasOwnProperty(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.HasOwnProperty_46(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.HasOwnProperty_47(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def HasOwnProperty_48(self, key,):
        '''
        @returns: c_char
        '''
        assert(isinstance(key,Local_v8__String_))
        return native.v8___Object__HasOwnProperty(self._cobject, key._cobject,)

    def HasOwnProperty_49(self, context,key,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__Name_))
        return Maybe_bool_(native.v8___Object__HasOwnProperty(self._cobject, context._cobject,key._cobject,))

    def HasRealNamedProperty(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.HasRealNamedProperty_48(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.HasRealNamedProperty_49(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def HasRealNamedProperty_50(self, key,):
        '''
        @returns: c_char
        '''
        assert(isinstance(key,Local_v8__String_))
        return native.v8___Object__HasRealNamedProperty(self._cobject, key._cobject,)

    def HasRealNamedProperty_51(self, context,key,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__Name_))
        return Maybe_bool_(native.v8___Object__HasRealNamedProperty(self._cobject, context._cobject,key._cobject,))

    def HasRealIndexedProperty(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.HasRealIndexedProperty_50(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.HasRealIndexedProperty_51(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def HasRealIndexedProperty_52(self, index,):
        '''
        @returns: c_char
        '''
        assert(isinstance(index,uint32_t))
        return native.v8___Object__HasRealIndexedProperty(self._cobject, index,)


    def HasRealIndexedProperty_53(self, context,index,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(index,uint32_t))
        return Maybe_bool_(native.v8___Object__HasRealIndexedProperty(self._cobject, context._cobject,index,))


    def HasRealNamedCallbackProperty(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.HasRealNamedCallbackProperty_52(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.HasRealNamedCallbackProperty_53(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def HasRealNamedCallbackProperty_54(self, key,):
        '''
        @returns: c_char
        '''
        assert(isinstance(key,Local_v8__String_))
        return native.v8___Object__HasRealNamedCallbackProperty(self._cobject, key._cobject,)

    def HasRealNamedCallbackProperty_55(self, context,key,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__Name_))
        return Maybe_bool_(native.v8___Object__HasRealNamedCallbackProperty(self._cobject, context._cobject,key._cobject,))

    def GetRealNamedPropertyInPrototypeChain(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.GetRealNamedPropertyInPrototypeChain_54(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.GetRealNamedPropertyInPrototypeChain_55(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def GetRealNamedPropertyInPrototypeChain_56(self, key,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(key,Local_v8__String_))
        return Local_v8__Value_(native.v8___Object__GetRealNamedPropertyInPrototypeChain(self._cobject, key._cobject,))

    def GetRealNamedPropertyInPrototypeChain_57(self, context,key,):
        '''
        @returns: v8.MaybeLocal_v8__Value_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__Name_))
        return MaybeLocal_v8__Value_(native.v8___Object__GetRealNamedPropertyInPrototypeChain(self._cobject, context._cobject,key._cobject,))

    def GetRealNamedPropertyAttributesInPrototypeChain(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.GetRealNamedPropertyAttributesInPrototypeChain_56(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.GetRealNamedPropertyAttributesInPrototypeChain_57(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def GetRealNamedPropertyAttributesInPrototypeChain_58(self, key,):
        '''
        @returns: v8.Maybe_v8__PropertyAttribute_
        '''
        assert(isinstance(key,Local_v8__String_))
        return Maybe_v8__PropertyAttribute_(native.v8___Object__GetRealNamedPropertyAttributesInPrototypeChain(self._cobject, key._cobject,))

    def GetRealNamedPropertyAttributesInPrototypeChain_59(self, context,key,):
        '''
        @returns: v8.Maybe_v8__PropertyAttribute_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__Name_))
        return Maybe_v8__PropertyAttribute_(native.v8___Object__GetRealNamedPropertyAttributesInPrototypeChain(self._cobject, context._cobject,key._cobject,))

    def GetRealNamedProperty(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.GetRealNamedProperty_58(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.GetRealNamedProperty_59(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def GetRealNamedProperty_60(self, key,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(key,Local_v8__String_))
        return Local_v8__Value_(native.v8___Object__GetRealNamedProperty(self._cobject, key._cobject,))

    def GetRealNamedProperty_61(self, context,key,):
        '''
        @returns: v8.MaybeLocal_v8__Value_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__Name_))
        return MaybeLocal_v8__Value_(native.v8___Object__GetRealNamedProperty(self._cobject, context._cobject,key._cobject,))

    def GetRealNamedPropertyAttributes(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.GetRealNamedPropertyAttributes_60(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.GetRealNamedPropertyAttributes_61(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def GetRealNamedPropertyAttributes_62(self, key,):
        '''
        @returns: v8.Maybe_v8__PropertyAttribute_
        '''
        assert(isinstance(key,Local_v8__String_))
        return Maybe_v8__PropertyAttribute_(native.v8___Object__GetRealNamedPropertyAttributes(self._cobject, key._cobject,))

    def GetRealNamedPropertyAttributes_63(self, context,key,):
        '''
        @returns: v8.Maybe_v8__PropertyAttribute_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(key,Local_v8__Name_))
        return Maybe_v8__PropertyAttribute_(native.v8___Object__GetRealNamedPropertyAttributes(self._cobject, context._cobject,key._cobject,))

    def HasNamedLookupInterceptor(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Object__HasNamedLookupInterceptor(self._cobject, )

    def HasIndexedLookupInterceptor(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Object__HasIndexedLookupInterceptor(self._cobject, )

    def TurnOnAccessCheck(self, ):
        '''
        @returns: void
        '''
        return native.v8___Object__TurnOnAccessCheck(self._cobject, )

    def GetIdentityHash(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___Object__GetIdentityHash(self._cobject, )

    def SetHiddenValue(self, key,value,):
        '''
        @returns: c_char
        '''
        assert(isinstance(key,Local_v8__String_))
        assert(isinstance(value,Local_v8__Value_))
        return native.v8___Object__SetHiddenValue(self._cobject, key._cobject,value._cobject,)

    def GetHiddenValue(self, key,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(key,Local_v8__String_))
        return Local_v8__Value_(native.v8___Object__GetHiddenValue(self._cobject, key._cobject,))

    def DeleteHiddenValue(self, key,):
        '''
        @returns: c_char
        '''
        assert(isinstance(key,Local_v8__String_))
        return native.v8___Object__DeleteHiddenValue(self._cobject, key._cobject,)

    def Clone(self, ):
        '''
        @returns: v8.Local_v8__Object_
        '''
        return Local_v8__Object_(native.v8___Object__Clone(self._cobject, ))

    def CreationContext(self, ):
        '''
        @returns: v8.Local_v8__Context_
        '''
        return Local_v8__Context_(native.v8___Object__CreationContext(self._cobject, ))

    def IsCallable(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Object__IsCallable(self._cobject, )

    def CallAsFunction(self, *args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return self.CallAsFunction_72(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 4:
            try:
                return self.CallAsFunction_73(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def CallAsFunction_74(self, recv,argc,argv,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(recv,Local_v8__Value_))
        assert(isinstance(argc,c_int))
        assert(isinstance(argv,Local_v8__Value_))
        return Local_v8__Value_(native.v8___Object__CallAsFunction(self._cobject, recv._cobject,argc,argv,))

    def CallAsFunction_75(self, context,recv,argc,argv,):
        '''
        @returns: v8.MaybeLocal_v8__Value_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(recv,Local_v8__Value_))
        assert(isinstance(argc,c_int))
        assert(isinstance(argv,Local_v8__Value_))
        return MaybeLocal_v8__Value_(native.v8___Object__CallAsFunction(self._cobject, context._cobject,recv._cobject,argc,argv,))

    def CallAsConstructor(self, *args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return self.CallAsConstructor_74(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return self.CallAsConstructor_75(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def CallAsConstructor_76(self, argc,argv,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(argc,c_int))
        assert(isinstance(argv,Local_v8__Value_))
        return Local_v8__Value_(native.v8___Object__CallAsConstructor(self._cobject, argc,argv,))

    def CallAsConstructor_77(self, context,argc,argv,):
        '''
        @returns: v8.MaybeLocal_v8__Value_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(argc,c_int))
        assert(isinstance(argv,Local_v8__Value_))
        return MaybeLocal_v8__Value_(native.v8___Object__CallAsConstructor(self._cobject, context._cobject,argc,argv,))

    def GetIsolate(self, ):
        '''
        @returns: v8.Isolate
        '''
        return native.v8___Object__GetIsolate(self._cobject, )

    @staticmethod
    def New(isolate,):
        '''
        @returns: v8.Local_v8__Object_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__Object_(native.v8___Object__New(isolate,))

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Object
        '''
        assert(isinstance(obj,Value))
        return native.v8___Object__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Object__CheckCast(obj,)

    def SlowGetInternalField(self, index,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(index,c_int))
        return Local_v8__Value_(native.v8___Object__SlowGetInternalField(self._cobject, index,))

    def SlowGetAlignedPointerFromInternalField(self, index,):
        '''
        @returns: c_void_p
        '''
        assert(isinstance(index,c_int))
        return native.v8___Object__SlowGetAlignedPointerFromInternalField(self._cobject, index,)

    def assign(self, arg1,):
        '''
        @returns: v8.Object
        '''
        assert(isinstance(arg1,POINTER(Object)))
        return (native.v8___Object__assign(self._cobject, arg1._cobject,))        

class ObjectOperationDescriptor( _FwdDecl_ObjectOperationDescriptor ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ObjectOperationDescriptor_copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ObjectOperationDescriptor(native.ObjectOperationDescriptor_copy( self._cobject ))

    def __del__(self):
        native.ObjectOperationDescriptor_delete(self._cobject)
        self._cobjet = None
            
            

class ObjectTemplate( _FwdDecl_ObjectTemplate ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.ObjectTemplate)            
        """
        assert(isinstance(arg1,ObjectTemplate))
        obj =  ObjectTemplate\
            ( native.ObjectTemplate_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.ObjectTemplate)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ObjectTemplate_copy(args[0])
        else:
            
            try:
                self._cobject = native.ObjectTemplate_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ObjectTemplate(native.ObjectTemplate_copy( self._cobject ))

    def __del__(self):
        native.ObjectTemplate_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return ObjectTemplate.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return ObjectTemplate.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return ObjectTemplate.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(isolate,constructor,):
        '''
        @returns: v8.Local_v8__ObjectTemplate_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(constructor,Local_v8__FunctionTemplate_))
        return Local_v8__ObjectTemplate_(native.v8___ObjectTemplate__New(isolate,constructor._cobject,))

    @staticmethod
    def New_2():
        '''
        @returns: v8.Local_v8__ObjectTemplate_
        '''
        return Local_v8__ObjectTemplate_(native.v8___ObjectTemplate__New())

    def NewInstance(self, *args):
        count = 0
        if count==0 and len(*args) == 0:
            try:
                return self.NewInstance_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.NewInstance_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def NewInstance_3(self, ):
        '''
        @returns: v8.Local_v8__Object_
        '''
        return Local_v8__Object_(native.v8___ObjectTemplate__NewInstance(self._cobject, ))

    def NewInstance_4(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__Object_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__Object_(native.v8___ObjectTemplate__NewInstance(self._cobject, context._cobject,))

    def SetAccessor(self, *args):
        count = 0
        if count==0 and len(*args) == 7:
            try:
                return self.SetAccessor_3(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 7:
            try:
                return self.SetAccessor_4(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def SetAccessor_5(self, name,getter,setter,data,settings,attribute,signature,):
        '''
        @returns: void
        '''
        assert(isinstance(name,Local_v8__String_))
        assert(isinstance(getter,AccessorGetterCallback))
        assert(isinstance(setter,AccessorSetterCallback))
        assert(isinstance(data,Local_v8__Value_))
        assert(isinstance(settings,c_uint))
        assert(isinstance(attribute,c_uint))
        assert(isinstance(signature,Local_v8__AccessorSignature_))
        return native.v8___ObjectTemplate__SetAccessor(self._cobject, name._cobject,getter,setter,data._cobject,settings._cobject
    
    
    
    
  ,attribute._cobject
    
    
    
    
  ,signature._cobject,)
    
    
    
    
  
    
    
    
    
  

    def SetAccessor_6(self, name,getter,setter,data,settings,attribute,signature,):
        '''
        @returns: void
        '''
        assert(isinstance(name,Local_v8__Name_))
        assert(isinstance(getter,AccessorNameGetterCallback))
        assert(isinstance(setter,AccessorNameSetterCallback))
        assert(isinstance(data,Local_v8__Value_))
        assert(isinstance(settings,c_uint))
        assert(isinstance(attribute,c_uint))
        assert(isinstance(signature,Local_v8__AccessorSignature_))
        return native.v8___ObjectTemplate__SetAccessor(self._cobject, name._cobject,getter,setter,data._cobject,settings._cobject
    
    
    
    
  ,attribute._cobject
    
    
    
    
  ,signature._cobject,)
    
    
    
    
  
    
    
    
    
  

    def SetNamedPropertyHandler(self, getter,setter,query,deleter,enumerator,data,):
        '''
        @returns: void
        '''
        assert(isinstance(getter,NamedPropertyGetterCallback))
        assert(isinstance(setter,NamedPropertySetterCallback))
        assert(isinstance(query,NamedPropertyQueryCallback))
        assert(isinstance(deleter,NamedPropertyDeleterCallback))
        assert(isinstance(enumerator,NamedPropertyEnumeratorCallback))
        assert(isinstance(data,Local_v8__Value_))
        return native.v8___ObjectTemplate__SetNamedPropertyHandler(self._cobject, getter,setter,query,deleter,enumerator,data._cobject,)

    def SetHandler(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.SetHandler_6(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.SetHandler_7(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def SetHandler_8(self, configuration,):
        '''
        @returns: void
        '''
        assert(isinstance(configuration,POINTER(NamedPropertyHandlerConfiguration)))
        return native.v8___ObjectTemplate__SetHandler(self._cobject, configuration._cobject,)

    def SetHandler_9(self, configuration,):
        '''
        @returns: void
        '''
        assert(isinstance(configuration,POINTER(IndexedPropertyHandlerConfiguration)))
        return native.v8___ObjectTemplate__SetHandler(self._cobject, configuration._cobject,)

    def SetIndexedPropertyHandler(self, getter,setter,query,deleter,enumerator,data,):
        '''
        @returns: void
        '''
        assert(isinstance(getter,IndexedPropertyGetterCallback))
        assert(isinstance(setter,IndexedPropertySetterCallback))
        assert(isinstance(query,IndexedPropertyQueryCallback))
        assert(isinstance(deleter,IndexedPropertyDeleterCallback))
        assert(isinstance(enumerator,IndexedPropertyEnumeratorCallback))
        assert(isinstance(data,Local_v8__Value_))
        return native.v8___ObjectTemplate__SetIndexedPropertyHandler(self._cobject, getter,setter,query,deleter,enumerator,data._cobject,)

    def SetCallAsFunctionHandler(self, callback,data,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,FunctionCallback))
        assert(isinstance(data,Local_v8__Value_))
        return native.v8___ObjectTemplate__SetCallAsFunctionHandler(self._cobject, callback,data._cobject,)

    def MarkAsUndetectable(self, ):
        '''
        @returns: void
        '''
        return native.v8___ObjectTemplate__MarkAsUndetectable(self._cobject, )

    def SetAccessCheckCallbacks(self, named_handler,indexed_handler,data,turned_on_by_default,):
        '''
        @returns: void
        '''
        assert(isinstance(named_handler,NamedSecurityCallback))
        assert(isinstance(indexed_handler,IndexedSecurityCallback))
        assert(isinstance(data,Local_v8__Value_))
        assert(isinstance(turned_on_by_default,c_char))
        return native.v8___ObjectTemplate__SetAccessCheckCallbacks(self._cobject, named_handler,indexed_handler,data._cobject,turned_on_by_default,)

    def InternalFieldCount(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___ObjectTemplate__InternalFieldCount(self._cobject, )

    def SetInternalFieldCount(self, value,):
        '''
        @returns: void
        '''
        assert(isinstance(value,c_int))
        return native.v8___ObjectTemplate__SetInternalFieldCount(self._cobject, value,)

    @staticmethod
    def New_16(isolate,constructor,):
        '''
        @returns: v8.Local_v8__ObjectTemplate_
        '''
        assert(isinstance(isolate,v8.internal.Isolate))
        assert(isinstance(constructor,Local_v8__FunctionTemplate_))
        return Local_v8__ObjectTemplate_(native.v8___ObjectTemplate__New(isolate,constructor._cobject,))


    def assign(self, arg1,):
        '''
        @returns: v8.ObjectTemplate
        '''
        assert(isinstance(arg1,POINTER(ObjectTemplate)))
        return (native.v8___ObjectTemplate__assign(self._cobject, arg1._cobject,))        

class Platform( _FwdDecl_Platform ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Platform_copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Platform(native.Platform_copy( self._cobject ))

    def __del__(self):
        native.Platform_delete(self._cobject)
        self._cobjet = None
            
            

class Primitive( _FwdDecl_Primitive ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Primitive)            
        """
        assert(isinstance(arg1,Primitive))
        obj =  Primitive\
            ( native.Primitive_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Primitive)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Primitive_copy(args[0])
        else:
            
            try:
                self._cobject = native.Primitive_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Primitive(native.Primitive_copy( self._cobject ))

    def __del__(self):
        native.Primitive_delete(self._cobject)
        self._cobjet = None
            
    

    def assign(self, arg1,):
        '''
        @returns: v8.Primitive
        '''
        assert(isinstance(arg1,POINTER(Primitive)))
        return (native.v8___Primitive__assign(self._cobject, arg1._cobject,))        

class Resolver( _FwdDecl_Resolver ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Resolver)            
        """
        assert(isinstance(arg1,Resolver))
        obj =  Resolver\
            ( native.Resolver_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Resolver)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Resolver_copy(args[0])
        else:
            
            try:
                self._cobject = native.Resolver_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Resolver(native.Resolver_copy( self._cobject ))

    def __del__(self):
        native.Resolver_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return Resolver.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return Resolver.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(isolate,):
        '''
        @returns: v8.Local_v8__Promise__Resolver_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__Promise__Resolver_(native.v8___Resolver__New(isolate,))

    @staticmethod
    def New_2(context,):
        '''
        @returns: v8.MaybeLocal_v8__Promise__Resolver_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__Promise__Resolver_(native.v8___Resolver__New(context._cobject,))

    def GetPromise(self, ):
        '''
        @returns: v8.Local_v8__Promise_
        '''
        return Local_v8__Promise_(native.v8___Resolver__GetPromise(self._cobject, ))

    def Resolve(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.Resolve_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.Resolve_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def Resolve_4(self, value,):
        '''
        @returns: void
        '''
        assert(isinstance(value,Local_v8__Value_))
        return native.v8___Resolver__Resolve(self._cobject, value._cobject,)

    def Resolve_5(self, context,value,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(value,Local_v8__Value_))
        return Maybe_bool_(native.v8___Resolver__Resolve(self._cobject, context._cobject,value._cobject,))

    def Reject(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.Reject_4(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.Reject_5(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def Reject_6(self, value,):
        '''
        @returns: void
        '''
        assert(isinstance(value,Local_v8__Value_))
        return native.v8___Resolver__Reject(self._cobject, value._cobject,)

    def Reject_7(self, context,value,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(value,Local_v8__Value_))
        return Maybe_bool_(native.v8___Resolver__Reject(self._cobject, context._cobject,value._cobject,))

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Resolver
        '''
        assert(isinstance(obj,Value))
        return native.v8___Resolver__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Resolver__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Resolver
        '''
        assert(isinstance(arg1,POINTER(Resolver)))
        return (native.v8___Resolver__assign(self._cobject, arg1._cobject,))
        

class Promise( _FwdDecl_Promise ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Promise)            
        """
        assert(isinstance(arg1,Promise))
        obj =  Promise\
            ( native.Promise_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Promise)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Promise_copy(args[0])
        else:
            
            try:
                self._cobject = native.Promise_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Promise(native.Promise_copy( self._cobject ))

    def __del__(self):
        native.Promise_delete(self._cobject)
        self._cobjet = None
            
    

    def Chain(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.Chain_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.Chain_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def Chain_1(self, handler,):
        '''
        @returns: v8.Local_v8__Promise_
        '''
        assert(isinstance(handler,Local_v8__Function_))
        return Local_v8__Promise_(native.v8___Promise__Chain(self._cobject, handler._cobject,))

    def Chain_2(self, context,handler,):
        '''
        @returns: v8.MaybeLocal_v8__Promise_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(handler,Local_v8__Function_))
        return MaybeLocal_v8__Promise_(native.v8___Promise__Chain(self._cobject, context._cobject,handler._cobject,))

    def Catch(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.Catch_3(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.Catch_4(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def Catch_3(self, handler,):
        '''
        @returns: v8.Local_v8__Promise_
        '''
        assert(isinstance(handler,Local_v8__Function_))
        return Local_v8__Promise_(native.v8___Promise__Catch(self._cobject, handler._cobject,))

    def Catch_4(self, context,handler,):
        '''
        @returns: v8.MaybeLocal_v8__Promise_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(handler,Local_v8__Function_))
        return MaybeLocal_v8__Promise_(native.v8___Promise__Catch(self._cobject, context._cobject,handler._cobject,))

    def Then(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.Then_5(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.Then_6(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def Then_5(self, handler,):
        '''
        @returns: v8.Local_v8__Promise_
        '''
        assert(isinstance(handler,Local_v8__Function_))
        return Local_v8__Promise_(native.v8___Promise__Then(self._cobject, handler._cobject,))

    def Then_6(self, context,handler,):
        '''
        @returns: v8.MaybeLocal_v8__Promise_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(handler,Local_v8__Function_))
        return MaybeLocal_v8__Promise_(native.v8___Promise__Then(self._cobject, context._cobject,handler._cobject,))

    def HasHandler(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Promise__HasHandler(self._cobject, )

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Promise
        '''
        assert(isinstance(obj,Value))
        return native.v8___Promise__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Promise__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Promise
        '''
        assert(isinstance(arg1,POINTER(Promise)))
        return (native.v8___Promise__assign(self._cobject, arg1._cobject,))        

class RawOperationDescriptor( _FwdDecl_RawOperationDescriptor ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.RawOperationDescriptor_copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return RawOperationDescriptor(native.RawOperationDescriptor_copy( self._cobject ))

    def __del__(self):
        native.RawOperationDescriptor_delete(self._cobject)
        self._cobjet = None
            
            

class Script( _FwdDecl_Script ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Script\
            ( native.Script_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Script)            
        """
        assert(isinstance(arg1,Script))
        obj =  Script\
            ( native.Script_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.Script)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Script_copy(args[0])
        else:
            
            try:
                self._cobject = native.Script_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Script_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Script(native.Script_copy( self._cobject ))

    def __del__(self):
        native.Script_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def Compile(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Script.Compile_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return Script.Compile_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Script.Compile_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def Compile_1(source,origin,):
        '''
        @returns: v8.Local_v8__Script_
        '''
        assert(isinstance(source,Local_v8__String_))
        assert(isinstance(origin,ScriptOrigin))
        return Local_v8__Script_(native.v8___Script__Compile(source._cobject,origin,))

    @staticmethod
    def Compile_2(context,source,origin,):
        '''
        @returns: v8.MaybeLocal_v8__Script_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(source,Local_v8__String_))
        assert(isinstance(origin,ScriptOrigin))
        return MaybeLocal_v8__Script_(native.v8___Script__Compile(context._cobject,source._cobject,origin,))

    @staticmethod
    def Compile_3(source,file_name,):
        '''
        @returns: v8.Local_v8__Script_
        '''
        assert(isinstance(source,Local_v8__String_))
        assert(isinstance(file_name,Local_v8__String_))
        return Local_v8__Script_(native.v8___Script__Compile(source._cobject,file_name._cobject,))

    def Run(self, *args):
        count = 0
        if count==0 and len(*args) == 0:
            try:
                return self.Run_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.Run_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def Run_4(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___Script__Run(self._cobject, ))

    def Run_5(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__Value_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__Value_(native.v8___Script__Run(self._cobject, context._cobject,))

    def GetUnboundScript(self, ):
        '''
        @returns: v8.Local_v8__UnboundScript_
        '''
        return Local_v8__UnboundScript_(native.v8___Script__GetUnboundScript(self._cobject, ))

    def GetId(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___Script__GetId(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: v8.Script
        '''
        assert(isinstance(arg1,POINTER(Script)))
        return (native.v8___Script__assign(self._cobject, arg1._cobject,))        

class Contents( _FwdDecl_Contents ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Contents\
            ( native.Contents_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Contents)            
        """
        assert(isinstance(arg1,Contents))
        obj =  Contents\
            ( native.Contents_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.Contents)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Contents_copy(args[0])
        else:
            
            try:
                self._cobject = native.Contents_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Contents_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Contents(native.Contents_copy( self._cobject ))

    def __del__(self):
        native.Contents_delete(self._cobject)
        self._cobjet = None
            
    

    def Data(self, ):
        '''
        @returns: c_void_p
        '''
        return native.v8___Contents__Data(self._cobject, )

    def ByteLength(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___Contents__ByteLength(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: v8.Contents
        '''
        assert(isinstance(arg1,POINTER(Contents)))
        return (native.v8___Contents__assign(self._cobject, arg1._cobject,))
        

class SharedArrayBuffer( _FwdDecl_SharedArrayBuffer ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.SharedArrayBuffer)            
        """
        assert(isinstance(arg1,SharedArrayBuffer))
        obj =  SharedArrayBuffer\
            ( native.SharedArrayBuffer_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.SharedArrayBuffer)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.SharedArrayBuffer_copy(args[0])
        else:
            
            try:
                self._cobject = native.SharedArrayBuffer_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return SharedArrayBuffer(native.SharedArrayBuffer_copy( self._cobject ))

    def __del__(self):
        native.SharedArrayBuffer_delete(self._cobject)
        self._cobjet = None
            
    

    def ByteLength(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___SharedArrayBuffer__ByteLength(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return SharedArrayBuffer.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 4:
            try:
                return SharedArrayBuffer.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_2(isolate,byte_length,):
        '''
        @returns: v8.Local_v8__SharedArrayBuffer_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(byte_length,size_t))
        return Local_v8__SharedArrayBuffer_(native.v8___SharedArrayBuffer__New(isolate,byte_length,))


    @staticmethod
    def New_3(isolate,data,byte_length,mode,):
        '''
        @returns: v8.Local_v8__SharedArrayBuffer_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(data,c_void_p))
        assert(isinstance(byte_length,size_t))
        assert(isinstance(mode,c_uint))
        return Local_v8__SharedArrayBuffer_(native.v8___SharedArrayBuffer__New(isolate,data,byte_length,mode._cobject
    
    
  ,))
    
    
  


    def IsExternal(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___SharedArrayBuffer__IsExternal(self._cobject, )

    def Externalize(self, ):
        '''
        @returns: v8.Contents
        '''
        return Contents(native.v8___SharedArrayBuffer__Externalize(self._cobject, ))

    def GetContents(self, ):
        '''
        @returns: v8.Contents
        '''
        return Contents(native.v8___SharedArrayBuffer__GetContents(self._cobject, ))

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.SharedArrayBuffer
        '''
        assert(isinstance(obj,Value))
        return native.v8___SharedArrayBuffer__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___SharedArrayBuffer__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.SharedArrayBuffer
        '''
        assert(isinstance(arg1,POINTER(SharedArrayBuffer)))
        return (native.v8___SharedArrayBuffer__assign(self._cobject, arg1._cobject,))        

class Signature( _FwdDecl_Signature ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Signature)            
        """
        assert(isinstance(arg1,Signature))
        obj =  Signature\
            ( native.Signature_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Signature)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Signature_copy(args[0])
        else:
            
            try:
                self._cobject = native.Signature_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Signature(native.Signature_copy( self._cobject ))

    def __del__(self):
        native.Signature_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(isolate,receiver,):
        '''
        @returns: v8.Local_v8__Signature_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(receiver,Local_v8__FunctionTemplate_))
        return Local_v8__Signature_(native.v8___Signature__New(isolate,receiver._cobject,))

    def assign(self, arg1,):
        '''
        @returns: v8.Signature
        '''
        assert(isinstance(arg1,POINTER(Signature)))
        return (native.v8___Signature__assign(self._cobject, arg1._cobject,))        

class StartupData( _FwdDecl_StartupData ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  StartupData\
            ( native.StartupData_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.StartupData)            
        """
        assert(isinstance(arg1,StartupData))
        obj =  StartupData\
            ( native.StartupData_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.StartupData)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.StartupData_copy(args[0])
        else:
            
            try:
                self._cobject = native.StartupData_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.StartupData_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return StartupData(native.StartupData_copy( self._cobject ))

    def __del__(self):
        native.StartupData_delete(self._cobject)
        self._cobjet = None
            
    

    def assign(self, arg1,):
        '''
        @returns: v8.StartupData
        '''
        assert(isinstance(arg1,POINTER(StartupData)))
        return (native.v8___StartupData__assign(self._cobject, arg1._cobject,))        

class StackFrame( _FwdDecl_StackFrame ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  StackFrame\
            ( native.StackFrame_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.StackFrame)            
        """
        assert(isinstance(arg1,StackFrame))
        obj =  StackFrame\
            ( native.StackFrame_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.StackFrame)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.StackFrame_copy(args[0])
        else:
            
            try:
                self._cobject = native.StackFrame_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.StackFrame_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return StackFrame(native.StackFrame_copy( self._cobject ))

    def __del__(self):
        native.StackFrame_delete(self._cobject)
        self._cobjet = None
            
    

    def GetLineNumber(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___StackFrame__GetLineNumber(self._cobject, )

    def GetColumn(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___StackFrame__GetColumn(self._cobject, )

    def GetScriptId(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___StackFrame__GetScriptId(self._cobject, )

    def GetScriptName(self, ):
        '''
        @returns: v8.Local_v8__String_
        '''
        return Local_v8__String_(native.v8___StackFrame__GetScriptName(self._cobject, ))

    def GetScriptNameOrSourceURL(self, ):
        '''
        @returns: v8.Local_v8__String_
        '''
        return Local_v8__String_(native.v8___StackFrame__GetScriptNameOrSourceURL(self._cobject, ))

    def GetFunctionName(self, ):
        '''
        @returns: v8.Local_v8__String_
        '''
        return Local_v8__String_(native.v8___StackFrame__GetFunctionName(self._cobject, ))

    def IsEval(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___StackFrame__IsEval(self._cobject, )

    def IsConstructor(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___StackFrame__IsConstructor(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: v8.StackFrame
        '''
        assert(isinstance(arg1,POINTER(StackFrame)))
        return (native.v8___StackFrame__assign(self._cobject, arg1._cobject,))        

class StackTrace( _FwdDecl_StackTrace ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  StackTrace\
            ( native.StackTrace_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.StackTrace)            
        """
        assert(isinstance(arg1,StackTrace))
        obj =  StackTrace\
            ( native.StackTrace_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.StackTrace)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.StackTrace_copy(args[0])
        else:
            
            try:
                self._cobject = native.StackTrace_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.StackTrace_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return StackTrace(native.StackTrace_copy( self._cobject ))

    def __del__(self):
        native.StackTrace_delete(self._cobject)
        self._cobjet = None
            
    

    def GetFrame(self, index,):
        '''
        @returns: v8.Local_v8__StackFrame_
        '''
        assert(isinstance(index,uint32_t))
        return Local_v8__StackFrame_(native.v8___StackTrace__GetFrame(self._cobject, index,))


    def GetFrameCount(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___StackTrace__GetFrameCount(self._cobject, )

    def AsArray(self, ):
        '''
        @returns: v8.Local_v8__Array_
        '''
        return Local_v8__Array_(native.v8___StackTrace__AsArray(self._cobject, ))

    @staticmethod
    def CurrentStackTrace(isolate,frame_limit,options,):
        '''
        @returns: v8.Local_v8__StackTrace_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(frame_limit,c_int))
        assert(isinstance(options,c_uint))
        return Local_v8__StackTrace_(native.v8___StackTrace__CurrentStackTrace(isolate,frame_limit,options._cobject
    
    
    
    
    
    
    
    
    
    
    
  ,))
    
    
    
    
    
    
    
    
    
    
    
  

    def assign(self, arg1,):
        '''
        @returns: v8.StackTrace
        '''
        assert(isinstance(arg1,POINTER(StackTrace)))
        return (native.v8___StackTrace__assign(self._cobject, arg1._cobject,))        

class ExternalStringResourceBase( _FwdDecl_ExternalStringResourceBase ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ExternalStringResourceBase_copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ExternalStringResourceBase(native.ExternalStringResourceBase_copy( self._cobject ))

    def __del__(self):
        native.ExternalStringResourceBase_delete(self._cobject)
        self._cobjet = None
            
    

    def Dispose(self, ):
        '''
        @returns: void
        '''
        return native.v8___ExternalStringResourceBase__Dispose(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: void
        '''
        assert(isinstance(arg1,POINTER(ExternalStringResourceBase)))
        return native.v8___ExternalStringResourceBase__assign(self._cobject, arg1._cobject,)
        

class ExternalStringResource( _FwdDecl_ExternalStringResource ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ExternalStringResource_copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ExternalStringResource(native.ExternalStringResource_copy( self._cobject ))

    def __del__(self):
        native.ExternalStringResource_delete(self._cobject)
        self._cobjet = None
            
    

    def data(self, ):
        '''
        @returns: POINTER(uint16_t)
        '''
        return native.v8___ExternalStringResource__data(self._cobject, )

    def length(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___ExternalStringResource__length(self._cobject, )        

class ExternalOneByteStringResource( _FwdDecl_ExternalOneByteStringResource ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ExternalOneByteStringResource_copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ExternalOneByteStringResource(native.ExternalOneByteStringResource_copy( self._cobject ))

    def __del__(self):
        native.ExternalOneByteStringResource_delete(self._cobject)
        self._cobjet = None
            
    

    def data(self, ):
        '''
        @returns: c_char_p
        '''
        return native.v8___ExternalOneByteStringResource__data(self._cobject, )

    def length(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___ExternalOneByteStringResource__length(self._cobject, )        

class Utf8Value( _FwdDecl_Utf8Value ):

    @staticmethod
    def new1(self, obj,):
        """
        Signature: 
           obj:v8.Local_v8__Value_            
        """
        assert(isinstance(obj,Utf8Value))
        obj =  Utf8Value\
            ( native.Utf8Value_new(obj._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Local_v8__Value_            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Utf8Value_copy(args[0])
        else:
            
            try:
                self._cobject = native.Utf8Value_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Utf8Value(native.Utf8Value_copy( self._cobject ))

    def __del__(self):
        native.Utf8Value_delete(self._cobject)
        self._cobjet = None
            
    

    def __str__(self, *args):
        count = 0
        if count==0 and len(*args) == 0:
            try:
                return self.__str___1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.__str___2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def __str___1(self, ):
        '''
        @returns: c_char_p
        '''
        return native.v8___Utf8Value____str__(self._cobject, )

    def __str___2(self, ):
        '''
        @returns: c_char_p
        '''
        return native.v8___Utf8Value____str__(self._cobject, )

    def length(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___Utf8Value__length(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: void
        '''
        assert(isinstance(arg1,POINTER(Utf8Value)))
        return native.v8___Utf8Value__assign(self._cobject, arg1._cobject,)
        

class Value( _FwdDecl_Value ):

    @staticmethod
    def new1(self, obj,):
        """
        Signature: 
           obj:v8.Local_v8__Value_            
        """
        assert(isinstance(obj,Value))
        obj =  Value\
            ( native.Value_new(obj._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Local_v8__Value_            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Value_copy(args[0])
        else:
            
            try:
                self._cobject = native.Value_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Value(native.Value_copy( self._cobject ))

    def __del__(self):
        native.Value_delete(self._cobject)
        self._cobjet = None
            
    

    def deref(self, *args):
        count = 0
        if count==0 and len(*args) == 0:
            try:
                return self.deref_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.deref_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def deref_1(self, ):
        '''
        @returns: POINTER(uint16_t)
        '''
        return native.v8___Value__deref(self._cobject, )

    def deref_2(self, ):
        '''
        @returns: POINTER(uint16_t)
        '''
        return native.v8___Value__deref(self._cobject, )

    def length(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___Value__length(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: void
        '''
        assert(isinstance(arg1,POINTER(Value)))
        return native.v8___Value__assign(self._cobject, arg1._cobject,)
        

class String( _FwdDecl_String ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.String)            
        """
        assert(isinstance(arg1,String))
        obj =  String\
            ( native.String_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.String)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.String_copy(args[0])
        else:
            
            try:
                self._cobject = native.String_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return String(native.String_copy( self._cobject ))

    def __del__(self):
        native.String_delete(self._cobject)
        self._cobjet = None
            
    

    def Length(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___String__Length(self._cobject, )

    def Utf8Length(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___String__Utf8Length(self._cobject, )

    def IsOneByte(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___String__IsOneByte(self._cobject, )

    def ContainsOnlyOneByte(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___String__ContainsOnlyOneByte(self._cobject, )

    def Write(self, buffer,start,length,options,):
        '''
        @returns: c_int
        '''
        assert(isinstance(buffer,POINTER(uint16_t)))
        assert(isinstance(start,c_int))
        assert(isinstance(length,c_int))
        assert(isinstance(options,c_int))
        return native.v8___String__Write(self._cobject, buffer,start,length,options,)


    def WriteOneByte(self, buffer,start,length,options,):
        '''
        @returns: c_int
        '''
        assert(isinstance(buffer,POINTER(uint8_t)))
        assert(isinstance(start,c_int))
        assert(isinstance(length,c_int))
        assert(isinstance(options,c_int))
        return native.v8___String__WriteOneByte(self._cobject, buffer,start,length,options,)


    def WriteUtf8(self, buffer,length,nchars_ref,options,):
        '''
        @returns: c_int
        '''
        assert(isinstance(buffer,c_char_p))
        assert(isinstance(length,c_int))
        assert(isinstance(nchars_ref,POINTER(c_int)))
        assert(isinstance(options,c_int))
        return native.v8___String__WriteUtf8(self._cobject, buffer,length,nchars_ref,options,)

    @staticmethod
    def Empty(isolate,):
        '''
        @returns: v8.Local_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__String_(native.v8___String__Empty(isolate,))

    def IsExternal(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___String__IsExternal(self._cobject, )

    def IsExternalOneByte(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___String__IsExternalOneByte(self._cobject, )

    def GetExternalStringResourceBase(self, encoding_out,):
        '''
        @returns: v8.ExternalStringResourceBase
        '''
        assert(isinstance(encoding_out,POINTER(c_uint)))
        return native.v8___String__GetExternalStringResourceBase(self._cobject, encoding_out,)

    
    
    
  

    def GetExternalStringResource(self, ):
        '''
        @returns: v8.ExternalStringResource
        '''
        return native.v8___String__GetExternalStringResource(self._cobject, )

    def GetExternalOneByteStringResource(self, ):
        '''
        @returns: POINTER(v8.ExternalOneByteStringResource)
        '''
        return native.v8___String__GetExternalOneByteStringResource(self._cobject, )

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.String
        '''
        assert(isinstance(obj,Value))
        return native.v8___String__Cast(obj,)

    @staticmethod
    def NewFromUtf8(*args):
        count = 0
        if count==0 and len(*args) == 4:
            try:
                return String.NewFromUtf8_3(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 4:
            try:
                return String.NewFromUtf8_4(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def NewFromUtf8_15(isolate,data,type,length,):
        '''
        @returns: v8.Local_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(data,c_char_p))
        assert(isinstance(type,c_uint))
        assert(isinstance(length,c_int))
        return Local_v8__String_(native.v8___String__NewFromUtf8(isolate,data,type._cobject
    
    
  ,length,))
    
    
  


    @staticmethod
    def NewFromUtf8_16(isolate,data,type,length,):
        '''
        @returns: v8.MaybeLocal_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(data,c_char_p))
        assert(isinstance(type,c_uint))
        assert(isinstance(length,c_int))
        return MaybeLocal_v8__String_(native.v8___String__NewFromUtf8(isolate,data,type._cobject
    
    
  ,length,))
    
    
  

    @staticmethod
    def NewFromOneByte(*args):
        count = 0
        if count==0 and len(*args) == 4:
            try:
                return String.NewFromOneByte_5(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 4:
            try:
                return String.NewFromOneByte_6(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def NewFromOneByte_17(isolate,data,type,length,):
        '''
        @returns: v8.Local_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(data,POINTER(uint8_t)))
        assert(isinstance(type,c_uint))
        assert(isinstance(length,c_int))
        return Local_v8__String_(native.v8___String__NewFromOneByte(isolate,data,type._cobject
    
    
  ,length,))
    
    
  



    @staticmethod
    def NewFromOneByte_18(isolate,data,type,length,):
        '''
        @returns: v8.MaybeLocal_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(data,POINTER(uint8_t)))
        assert(isinstance(type,c_uint))
        assert(isinstance(length,c_int))
        return MaybeLocal_v8__String_(native.v8___String__NewFromOneByte(isolate,data,type._cobject
    
    
  ,length,))
    
    
  


    @staticmethod
    def NewFromTwoByte(*args):
        count = 0
        if count==0 and len(*args) == 4:
            try:
                return String.NewFromTwoByte_7(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 4:
            try:
                return String.NewFromTwoByte_8(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def NewFromTwoByte_19(isolate,data,type,length,):
        '''
        @returns: v8.Local_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(data,POINTER(uint16_t)))
        assert(isinstance(type,c_uint))
        assert(isinstance(length,c_int))
        return Local_v8__String_(native.v8___String__NewFromTwoByte(isolate,data,type._cobject
    
    
  ,length,))
    
    
  



    @staticmethod
    def NewFromTwoByte_20(isolate,data,type,length,):
        '''
        @returns: v8.MaybeLocal_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(data,POINTER(uint16_t)))
        assert(isinstance(type,c_uint))
        assert(isinstance(length,c_int))
        return MaybeLocal_v8__String_(native.v8___String__NewFromTwoByte(isolate,data,type._cobject
    
    
  ,length,))
    
    
  


    @staticmethod
    def Concat(left,right,):
        '''
        @returns: v8.Local_v8__String_
        '''
        assert(isinstance(left,Local_v8__String_))
        assert(isinstance(right,Local_v8__String_))
        return Local_v8__String_(native.v8___String__Concat(left._cobject,right._cobject,))

    @staticmethod
    def NewExternal(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return String.NewExternal_10(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return String.NewExternal_12(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def NewExternal_22(isolate,resource,):
        '''
        @returns: v8.Local_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(resource,ExternalStringResource))
        return Local_v8__String_(native.v8___String__NewExternal(isolate,resource,))

    
  

    @staticmethod
    def NewExternalTwoByte(isolate,resource,):
        '''
        @returns: v8.MaybeLocal_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(resource,ExternalStringResource))
        return MaybeLocal_v8__String_(native.v8___String__NewExternalTwoByte(isolate,resource,))

    
  

    def MakeExternal(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.MakeExternal_13(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.MakeExternal_14(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def MakeExternal_24(self, resource,):
        '''
        @returns: c_char
        '''
        assert(isinstance(resource,ExternalStringResource))
        return native.v8___String__MakeExternal(self._cobject, resource,)

    
  

    @staticmethod
    def NewExternal_25(isolate,resource,):
        '''
        @returns: v8.Local_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(resource,ExternalOneByteStringResource))
        return Local_v8__String_(native.v8___String__NewExternal(isolate,resource,))

    
  

    @staticmethod
    def NewExternalOneByte(isolate,resource,):
        '''
        @returns: v8.MaybeLocal_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(resource,ExternalOneByteStringResource))
        return MaybeLocal_v8__String_(native.v8___String__NewExternalOneByte(isolate,resource,))

    
  

    def MakeExternal_27(self, resource,):
        '''
        @returns: c_char
        '''
        assert(isinstance(resource,ExternalOneByteStringResource))
        return native.v8___String__MakeExternal(self._cobject, resource,)

    
  

    def CanMakeExternal(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___String__CanMakeExternal(self._cobject, )

    def VerifyExternalStringResourceBase(self, v,encoding,):
        '''
        @returns: void
        '''
        assert(isinstance(v,ExternalStringResourceBase))
        assert(isinstance(encoding,c_uint))
        return native.v8___String__VerifyExternalStringResourceBase(self._cobject, v,encoding._cobject
    
    
    
  ,)
    
    
    
  



    def VerifyExternalStringResource(self, val,):
        '''
        @returns: void
        '''
        assert(isinstance(val,ExternalStringResource))
        return native.v8___String__VerifyExternalStringResource(self._cobject, val,)

    
  

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___String__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.String
        '''
        assert(isinstance(arg1,POINTER(String)))
        return (native.v8___String__assign(self._cobject, arg1._cobject,))        

class StringObject( _FwdDecl_StringObject ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.StringObject)            
        """
        assert(isinstance(arg1,StringObject))
        obj =  StringObject\
            ( native.StringObject_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.StringObject)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.StringObject_copy(args[0])
        else:
            
            try:
                self._cobject = native.StringObject_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return StringObject(native.StringObject_copy( self._cobject ))

    def __del__(self):
        native.StringObject_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(value,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(value,Local_v8__String_))
        return Local_v8__Value_(native.v8___StringObject__New(value._cobject,))

    def ValueOf(self, ):
        '''
        @returns: v8.Local_v8__String_
        '''
        return Local_v8__String_(native.v8___StringObject__ValueOf(self._cobject, ))

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.StringObject
        '''
        assert(isinstance(obj,Value))
        return native.v8___StringObject__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___StringObject__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.StringObject
        '''
        assert(isinstance(arg1,POINTER(StringObject)))
        return (native.v8___StringObject__assign(self._cobject, arg1._cobject,))        

class Symbol( _FwdDecl_Symbol ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Symbol)            
        """
        assert(isinstance(arg1,Symbol))
        obj =  Symbol\
            ( native.Symbol_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Symbol)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Symbol_copy(args[0])
        else:
            
            try:
                self._cobject = native.Symbol_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Symbol(native.Symbol_copy( self._cobject ))

    def __del__(self):
        native.Symbol_delete(self._cobject)
        self._cobjet = None
            
    

    def Name(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___Symbol__Name(self._cobject, ))

    @staticmethod
    def New(isolate,name,):
        '''
        @returns: v8.Local_v8__Symbol_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(name,Local_v8__String_))
        return Local_v8__Symbol_(native.v8___Symbol__New(isolate,name._cobject,))

    @staticmethod
    def For(isolate,name,):
        '''
        @returns: v8.Local_v8__Symbol_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(name,Local_v8__String_))
        return Local_v8__Symbol_(native.v8___Symbol__For(isolate,name._cobject,))

    @staticmethod
    def ForApi(isolate,name,):
        '''
        @returns: v8.Local_v8__Symbol_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(name,Local_v8__String_))
        return Local_v8__Symbol_(native.v8___Symbol__ForApi(isolate,name._cobject,))

    @staticmethod
    def GetIterator(isolate,):
        '''
        @returns: v8.Local_v8__Symbol_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__Symbol_(native.v8___Symbol__GetIterator(isolate,))

    @staticmethod
    def GetUnscopables(isolate,):
        '''
        @returns: v8.Local_v8__Symbol_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__Symbol_(native.v8___Symbol__GetUnscopables(isolate,))

    @staticmethod
    def GetToStringTag(isolate,):
        '''
        @returns: v8.Local_v8__Symbol_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__Symbol_(native.v8___Symbol__GetToStringTag(isolate,))

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Symbol
        '''
        assert(isinstance(obj,Value))
        return native.v8___Symbol__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Symbol__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Symbol
        '''
        assert(isinstance(arg1,POINTER(Symbol)))
        return (native.v8___Symbol__assign(self._cobject, arg1._cobject,))        

class SymbolObject( _FwdDecl_SymbolObject ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.SymbolObject)            
        """
        assert(isinstance(arg1,SymbolObject))
        obj =  SymbolObject\
            ( native.SymbolObject_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.SymbolObject)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.SymbolObject_copy(args[0])
        else:
            
            try:
                self._cobject = native.SymbolObject_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return SymbolObject(native.SymbolObject_copy( self._cobject ))

    def __del__(self):
        native.SymbolObject_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(isolate,value,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(value,Local_v8__Symbol_))
        return Local_v8__Value_(native.v8___SymbolObject__New(isolate,value._cobject,))

    def ValueOf(self, ):
        '''
        @returns: v8.Local_v8__Symbol_
        '''
        return Local_v8__Symbol_(native.v8___SymbolObject__ValueOf(self._cobject, ))

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.SymbolObject
        '''
        assert(isinstance(obj,Value))
        return native.v8___SymbolObject__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___SymbolObject__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.SymbolObject
        '''
        assert(isinstance(arg1,POINTER(SymbolObject)))
        return (native.v8___SymbolObject__assign(self._cobject, arg1._cobject,))        

class Uint32( _FwdDecl_Uint32 ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Uint32)            
        """
        assert(isinstance(arg1,Uint32))
        obj =  Uint32\
            ( native.Uint32_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Uint32)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Uint32_copy(args[0])
        else:
            
            try:
                self._cobject = native.Uint32_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Uint32(native.Uint32_copy( self._cobject ))

    def __del__(self):
        native.Uint32_delete(self._cobject)
        self._cobjet = None
            
    

    def Value(self, ):
        '''
        @returns: uint32_t
        '''
        return native.v8___Uint32__Value(self._cobject, )

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Uint32
        '''
        assert(isinstance(obj,Value))
        return native.v8___Uint32__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Uint32__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Uint32
        '''
        assert(isinstance(arg1,POINTER(Uint32)))
        return (native.v8___Uint32__assign(self._cobject, arg1._cobject,))        

class Utils( _FwdDecl_Utils ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Utils_copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Utils(native.Utils_copy( self._cobject ))

    def __del__(self):
        native.Utils_delete(self._cobject)
        self._cobjet = None
            
            

class Value( _FwdDecl_Value ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Value)            
        """
        assert(isinstance(arg1,Value))
        obj =  Value\
            ( native.Value_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Value)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Value_copy(args[0])
        else:
            
            try:
                self._cobject = native.Value_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Value(native.Value_copy( self._cobject ))

    def __del__(self):
        native.Value_delete(self._cobject)
        self._cobjet = None
            
    

    def IsUndefined(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsUndefined(self._cobject, )

    def IsNull(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsNull(self._cobject, )

    def IsTrue(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsTrue(self._cobject, )

    def IsFalse(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsFalse(self._cobject, )

    def IsName(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsName(self._cobject, )

    def IsString(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsString(self._cobject, )

    def IsSymbol(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsSymbol(self._cobject, )

    def IsFunction(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsFunction(self._cobject, )

    def IsArray(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsArray(self._cobject, )

    def IsObject(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsObject(self._cobject, )

    def IsBoolean(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsBoolean(self._cobject, )

    def IsNumber(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsNumber(self._cobject, )

    def IsExternal(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsExternal(self._cobject, )

    def IsInt32(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsInt32(self._cobject, )

    def IsUint32(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsUint32(self._cobject, )

    def IsDate(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsDate(self._cobject, )

    def IsArgumentsObject(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsArgumentsObject(self._cobject, )

    def IsBooleanObject(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsBooleanObject(self._cobject, )

    def IsNumberObject(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsNumberObject(self._cobject, )

    def IsStringObject(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsStringObject(self._cobject, )

    def IsSymbolObject(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsSymbolObject(self._cobject, )

    def IsNativeError(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsNativeError(self._cobject, )

    def IsRegExp(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsRegExp(self._cobject, )

    def IsGeneratorFunction(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsGeneratorFunction(self._cobject, )

    def IsGeneratorObject(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsGeneratorObject(self._cobject, )

    def IsPromise(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsPromise(self._cobject, )

    def IsMap(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsMap(self._cobject, )

    def IsSet(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsSet(self._cobject, )

    def IsMapIterator(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsMapIterator(self._cobject, )

    def IsSetIterator(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsSetIterator(self._cobject, )

    def IsWeakMap(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsWeakMap(self._cobject, )

    def IsWeakSet(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsWeakSet(self._cobject, )

    def IsArrayBuffer(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsArrayBuffer(self._cobject, )

    def IsArrayBufferView(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsArrayBufferView(self._cobject, )

    def IsTypedArray(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsTypedArray(self._cobject, )

    def IsUint8Array(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsUint8Array(self._cobject, )

    def IsUint8ClampedArray(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsUint8ClampedArray(self._cobject, )

    def IsInt8Array(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsInt8Array(self._cobject, )

    def IsUint16Array(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsUint16Array(self._cobject, )

    def IsInt16Array(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsInt16Array(self._cobject, )

    def IsUint32Array(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsUint32Array(self._cobject, )

    def IsInt32Array(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsInt32Array(self._cobject, )

    def IsFloat32Array(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsFloat32Array(self._cobject, )

    def IsFloat64Array(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsFloat64Array(self._cobject, )

    def IsFloat32x4(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsFloat32x4(self._cobject, )

    def IsDataView(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsDataView(self._cobject, )

    def IsSharedArrayBuffer(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__IsSharedArrayBuffer(self._cobject, )

    def ToBoolean(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.ToBoolean_48(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.ToBoolean_56(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.ToBoolean_64(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def ToBoolean_48(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__Boolean_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__Boolean_(native.v8___Value__ToBoolean(self._cobject, context._cobject,))

    def ToNumber(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.ToNumber_49(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.ToNumber_57(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.ToNumber_65(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def ToNumber_49(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__Number_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__Number_(native.v8___Value__ToNumber(self._cobject, context._cobject,))

    def ToString(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.ToString_50(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.ToString_58(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.ToString_66(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def ToString_50(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__String_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__String_(native.v8___Value__ToString(self._cobject, context._cobject,))

    def ToDetailString(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.ToDetailString_51(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.ToDetailString_59(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.ToDetailString_67(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def ToDetailString_51(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__String_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__String_(native.v8___Value__ToDetailString(self._cobject, context._cobject,))

    def ToObject(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.ToObject_52(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.ToObject_60(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.ToObject_68(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def ToObject_52(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__Object_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__Object_(native.v8___Value__ToObject(self._cobject, context._cobject,))

    def ToInteger(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.ToInteger_53(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.ToInteger_61(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.ToInteger_69(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def ToInteger_53(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__Integer_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__Integer_(native.v8___Value__ToInteger(self._cobject, context._cobject,))

    def ToUint32(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.ToUint32_54(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.ToUint32_62(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.ToUint32_70(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def ToUint32_54(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__Uint32_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__Uint32_(native.v8___Value__ToUint32(self._cobject, context._cobject,))

    def ToInt32(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.ToInt32_55(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.ToInt32_63(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.ToInt32_71(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def ToInt32_55(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__Int32_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__Int32_(native.v8___Value__ToInt32(self._cobject, context._cobject,))

    def ToBoolean_56(self, isolate,):
        '''
        @returns: v8.Local_v8__Boolean_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__Boolean_(native.v8___Value__ToBoolean(self._cobject, isolate,))

    def ToNumber_57(self, isolate,):
        '''
        @returns: v8.Local_v8__Number_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__Number_(native.v8___Value__ToNumber(self._cobject, isolate,))

    def ToString_58(self, isolate,):
        '''
        @returns: v8.Local_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__String_(native.v8___Value__ToString(self._cobject, isolate,))

    def ToDetailString_59(self, isolate,):
        '''
        @returns: v8.Local_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__String_(native.v8___Value__ToDetailString(self._cobject, isolate,))

    def ToObject_60(self, isolate,):
        '''
        @returns: v8.Local_v8__Object_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__Object_(native.v8___Value__ToObject(self._cobject, isolate,))

    def ToInteger_61(self, isolate,):
        '''
        @returns: v8.Local_v8__Integer_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__Integer_(native.v8___Value__ToInteger(self._cobject, isolate,))

    def ToUint32_62(self, isolate,):
        '''
        @returns: v8.Local_v8__Uint32_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__Uint32_(native.v8___Value__ToUint32(self._cobject, isolate,))

    def ToInt32_63(self, isolate,):
        '''
        @returns: v8.Local_v8__Int32_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__Int32_(native.v8___Value__ToInt32(self._cobject, isolate,))

    def ToBoolean_64(self, ):
        '''
        @returns: v8.Local_v8__Boolean_
        '''
        return Local_v8__Boolean_(native.v8___Value__ToBoolean(self._cobject, ))

    def ToNumber_65(self, ):
        '''
        @returns: v8.Local_v8__Number_
        '''
        return Local_v8__Number_(native.v8___Value__ToNumber(self._cobject, ))

    def ToString_66(self, ):
        '''
        @returns: v8.Local_v8__String_
        '''
        return Local_v8__String_(native.v8___Value__ToString(self._cobject, ))

    def ToDetailString_67(self, ):
        '''
        @returns: v8.Local_v8__String_
        '''
        return Local_v8__String_(native.v8___Value__ToDetailString(self._cobject, ))

    def ToObject_68(self, ):
        '''
        @returns: v8.Local_v8__Object_
        '''
        return Local_v8__Object_(native.v8___Value__ToObject(self._cobject, ))

    def ToInteger_69(self, ):
        '''
        @returns: v8.Local_v8__Integer_
        '''
        return Local_v8__Integer_(native.v8___Value__ToInteger(self._cobject, ))

    def ToUint32_70(self, ):
        '''
        @returns: v8.Local_v8__Uint32_
        '''
        return Local_v8__Uint32_(native.v8___Value__ToUint32(self._cobject, ))

    def ToInt32_71(self, ):
        '''
        @returns: v8.Local_v8__Int32_
        '''
        return Local_v8__Int32_(native.v8___Value__ToInt32(self._cobject, ))

    def ToArrayIndex(self, *args):
        count = 0
        if count==0 and len(*args) == 0:
            try:
                return self.ToArrayIndex_72(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.ToArrayIndex_73(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def ToArrayIndex_72(self, ):
        '''
        @returns: v8.Local_v8__Uint32_
        '''
        return Local_v8__Uint32_(native.v8___Value__ToArrayIndex(self._cobject, ))

    def ToArrayIndex_73(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__Uint32_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__Uint32_(native.v8___Value__ToArrayIndex(self._cobject, context._cobject,))

    def BooleanValue(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.BooleanValue_74(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.BooleanValue_79(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def BooleanValue_74(self, context,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return Maybe_bool_(native.v8___Value__BooleanValue(self._cobject, context._cobject,))

    def NumberValue(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.NumberValue_75(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.NumberValue_80(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def NumberValue_75(self, context,):
        '''
        @returns: v8.Maybe_double_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return Maybe_double_(native.v8___Value__NumberValue(self._cobject, context._cobject,))

    def IntegerValue(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.IntegerValue_76(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.IntegerValue_81(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def IntegerValue_76(self, context,):
        '''
        @returns: v8.Maybe_long_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return Maybe_long_(native.v8___Value__IntegerValue(self._cobject, context._cobject,))

    def Uint32Value(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.Uint32Value_77(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.Uint32Value_82(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def Uint32Value_77(self, context,):
        '''
        @returns: v8.Maybe_unsigned_int_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return Maybe_unsigned_int_(native.v8___Value__Uint32Value(self._cobject, context._cobject,))

    def Int32Value(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.Int32Value_78(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 0:
            try:
                return self.Int32Value_83(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def Int32Value_78(self, context,):
        '''
        @returns: v8.Maybe_int_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return Maybe_int_(native.v8___Value__Int32Value(self._cobject, context._cobject,))

    def BooleanValue_79(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__BooleanValue(self._cobject, )

    def NumberValue_80(self, ):
        '''
        @returns: c_double
        '''
        return native.v8___Value__NumberValue(self._cobject, )

    def IntegerValue_81(self, ):
        '''
        @returns: int64_t
        '''
        return native.v8___Value__IntegerValue(self._cobject, )

    def Uint32Value_82(self, ):
        '''
        @returns: uint32_t
        '''
        return native.v8___Value__Uint32Value(self._cobject, )

    def Int32Value_83(self, ):
        '''
        @returns: int32_t
        '''
        return native.v8___Value__Int32Value(self._cobject, )

    def Equals(self, *args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return self.Equals_84(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return self.Equals_85(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def Equals_84(self, that,):
        '''
        @returns: c_char
        '''
        assert(isinstance(that,Local_v8__Value_))
        return native.v8___Value__Equals(self._cobject, that._cobject,)

    def Equals_85(self, context,that,):
        '''
        @returns: v8.Maybe_bool_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(that,Local_v8__Value_))
        return Maybe_bool_(native.v8___Value__Equals(self._cobject, context._cobject,that._cobject,))

    def StrictEquals(self, that,):
        '''
        @returns: c_char
        '''
        assert(isinstance(that,Local_v8__Value_))
        return native.v8___Value__StrictEquals(self._cobject, that._cobject,)

    def SameValue(self, that,):
        '''
        @returns: c_char
        '''
        assert(isinstance(that,Local_v8__Value_))
        return native.v8___Value__SameValue(self._cobject, that._cobject,)

    def QuickIsUndefined(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__QuickIsUndefined(self._cobject, )

    def QuickIsNull(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__QuickIsNull(self._cobject, )

    def QuickIsString(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__QuickIsString(self._cobject, )

    def FullIsUndefined(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__FullIsUndefined(self._cobject, )

    def FullIsNull(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__FullIsNull(self._cobject, )

    def FullIsString(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Value__FullIsString(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: v8.Value
        '''
        assert(isinstance(arg1,POINTER(Value)))
        return (native.v8___Value__assign(self._cobject, arg1._cobject,))        

class Local_v8__Primitive_( _FwdDecl_Local_v8__Primitive_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__Primitive_\
            ( native.Local_v8__Primitive__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__Primitive_            
        """
        assert(isinstance(that,Local_v8__Primitive_))
        obj =  Local_v8__Primitive_\
            ( native.Local_v8__Primitive__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__Primitive_)            
        """
        assert(isinstance(arg1,Local_v8__Primitive_))
        obj =  Local_v8__Primitive_\
            ( native.Local_v8__Primitive__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__Primitive_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__Primitive_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Primitive__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Primitive_(native.Local_v8__Primitive__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Primitive__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__Primitive___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__Primitive___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.Primitive
        '''
        return native.v8___Local_v8__Primitive___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.Primitive
        '''
        return native.v8___Local_v8__Primitive___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Primitive_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Primitive_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Primitive_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__Primitive_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__Primitive_))
        return Local_v8__Primitive_(native.v8___Local_v8__Primitive___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__Primitive_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__Primitive_)))
        return Local_v8__Primitive_(native.v8___Local_v8__Primitive___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__Primitive_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Primitive))
        return Local_v8__Primitive_(native.v8___Local_v8__Primitive___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__Primitive_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__Primitive_)))
        return (native.v8___Local_v8__Primitive___assign(self._cobject, arg1._cobject,))        

class Local_v8__Boolean_( _FwdDecl_Local_v8__Boolean_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__Boolean_\
            ( native.Local_v8__Boolean__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__Boolean_            
        """
        assert(isinstance(that,Local_v8__Boolean_))
        obj =  Local_v8__Boolean_\
            ( native.Local_v8__Boolean__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__Boolean_)            
        """
        assert(isinstance(arg1,Local_v8__Boolean_))
        obj =  Local_v8__Boolean_\
            ( native.Local_v8__Boolean__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__Boolean_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__Boolean_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Boolean__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Boolean_(native.Local_v8__Boolean__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Boolean__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__Boolean___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__Boolean___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.Boolean
        '''
        return native.v8___Local_v8__Boolean___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.Boolean
        '''
        return native.v8___Local_v8__Boolean___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Boolean_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Boolean_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Boolean_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__Boolean_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__Boolean_))
        return Local_v8__Boolean_(native.v8___Local_v8__Boolean___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__Boolean_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__Boolean_)))
        return Local_v8__Boolean_(native.v8___Local_v8__Boolean___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__Boolean_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Boolean))
        return Local_v8__Boolean_(native.v8___Local_v8__Boolean___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__Boolean_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__Boolean_)))
        return (native.v8___Local_v8__Boolean___assign(self._cobject, arg1._cobject,))        

class Local_v8__Value_( _FwdDecl_Local_v8__Value_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__Value_\
            ( native.Local_v8__Value__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__Value_            
        """
        assert(isinstance(that,Local_v8__Value_))
        obj =  Local_v8__Value_\
            ( native.Local_v8__Value__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__Value_)            
        """
        assert(isinstance(arg1,Local_v8__Value_))
        obj =  Local_v8__Value_\
            ( native.Local_v8__Value__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__Value_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__Value_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Value__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Value_(native.Local_v8__Value__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Value__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__Value___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__Value___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.Value
        '''
        return native.v8___Local_v8__Value___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.Value
        '''
        return native.v8___Local_v8__Value___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Value_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Value_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Value_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__Value_))
        return Local_v8__Value_(native.v8___Local_v8__Value___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__Value_)))
        return Local_v8__Value_(native.v8___Local_v8__Value___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Value))
        return Local_v8__Value_(native.v8___Local_v8__Value___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__Value_)))
        return (native.v8___Local_v8__Value___assign(self._cobject, arg1._cobject,))        

class Local_v8__Integer_( _FwdDecl_Local_v8__Integer_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__Integer_\
            ( native.Local_v8__Integer__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__Integer_            
        """
        assert(isinstance(that,Local_v8__Integer_))
        obj =  Local_v8__Integer_\
            ( native.Local_v8__Integer__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__Integer_)            
        """
        assert(isinstance(arg1,Local_v8__Integer_))
        obj =  Local_v8__Integer_\
            ( native.Local_v8__Integer__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__Integer_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__Integer_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Integer__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Integer_(native.Local_v8__Integer__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Integer__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__Integer___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__Integer___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.Integer
        '''
        return native.v8___Local_v8__Integer___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.Integer
        '''
        return native.v8___Local_v8__Integer___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Integer_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Integer_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Integer_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__Integer_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__Integer_))
        return Local_v8__Integer_(native.v8___Local_v8__Integer___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__Integer_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__Integer_)))
        return Local_v8__Integer_(native.v8___Local_v8__Integer___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__Integer_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Integer))
        return Local_v8__Integer_(native.v8___Local_v8__Integer___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__Integer_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__Integer_)))
        return (native.v8___Local_v8__Integer___assign(self._cobject, arg1._cobject,))        

class Local_v8__Script_( _FwdDecl_Local_v8__Script_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Script__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Script_(native.Local_v8__Script__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Script__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__String_( _FwdDecl_Local_v8__String_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__String_\
            ( native.Local_v8__String__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__String_            
        """
        assert(isinstance(that,Local_v8__String_))
        obj =  Local_v8__String_\
            ( native.Local_v8__String__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__String_)            
        """
        assert(isinstance(arg1,Local_v8__String_))
        obj =  Local_v8__String_\
            ( native.Local_v8__String__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__String_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__String_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__String__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__String_(native.Local_v8__String__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__String__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__String___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__String___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.String
        '''
        return native.v8___Local_v8__String___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.String
        '''
        return native.v8___Local_v8__String___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__String_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__String_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__String_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__String_))
        return Local_v8__String_(native.v8___Local_v8__String___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__String_)))
        return Local_v8__String_(native.v8___Local_v8__String___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__String_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,String))
        return Local_v8__String_(native.v8___Local_v8__String___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__String_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__String_)))
        return (native.v8___Local_v8__String___assign(self._cobject, arg1._cobject,))        

class Local_v8__Context_( _FwdDecl_Local_v8__Context_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__Context_\
            ( native.Local_v8__Context__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__Context_            
        """
        assert(isinstance(that,Local_v8__Context_))
        obj =  Local_v8__Context_\
            ( native.Local_v8__Context__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__Context_)            
        """
        assert(isinstance(arg1,Local_v8__Context_))
        obj =  Local_v8__Context_\
            ( native.Local_v8__Context__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__Context_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__Context_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Context__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Context_(native.Local_v8__Context__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Context__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__Context___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__Context___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.Context
        '''
        return native.v8___Local_v8__Context___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.Context
        '''
        return native.v8___Local_v8__Context___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Context_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Context_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Context_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__Context_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__Context_))
        return Local_v8__Context_(native.v8___Local_v8__Context___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__Context_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__Context_)))
        return Local_v8__Context_(native.v8___Local_v8__Context___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__Context_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Context))
        return Local_v8__Context_(native.v8___Local_v8__Context___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__Context_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__Context_)))
        return (native.v8___Local_v8__Context___assign(self._cobject, arg1._cobject,))        

class Local_v8__UnboundScript_( _FwdDecl_Local_v8__UnboundScript_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__UnboundScript_\
            ( native.Local_v8__UnboundScript__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__UnboundScript_            
        """
        assert(isinstance(that,Local_v8__UnboundScript_))
        obj =  Local_v8__UnboundScript_\
            ( native.Local_v8__UnboundScript__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__UnboundScript_)            
        """
        assert(isinstance(arg1,Local_v8__UnboundScript_))
        obj =  Local_v8__UnboundScript_\
            ( native.Local_v8__UnboundScript__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__UnboundScript_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__UnboundScript_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__UnboundScript__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__UnboundScript_(native.Local_v8__UnboundScript__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__UnboundScript__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__UnboundScript___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__UnboundScript___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.UnboundScript
        '''
        return native.v8___Local_v8__UnboundScript___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.UnboundScript
        '''
        return native.v8___Local_v8__UnboundScript___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__UnboundScript_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__UnboundScript_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__UnboundScript_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__UnboundScript_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__UnboundScript_))
        return Local_v8__UnboundScript_(native.v8___Local_v8__UnboundScript___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__UnboundScript_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__UnboundScript_)))
        return Local_v8__UnboundScript_(native.v8___Local_v8__UnboundScript___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__UnboundScript_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,UnboundScript))
        return Local_v8__UnboundScript_(native.v8___Local_v8__UnboundScript___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__UnboundScript_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__UnboundScript_)))
        return (native.v8___Local_v8__UnboundScript___assign(self._cobject, arg1._cobject,))        

class Local_v8__Function_( _FwdDecl_Local_v8__Function_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__Function_\
            ( native.Local_v8__Function__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__Function_            
        """
        assert(isinstance(that,Local_v8__Function_))
        obj =  Local_v8__Function_\
            ( native.Local_v8__Function__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__Function_)            
        """
        assert(isinstance(arg1,Local_v8__Function_))
        obj =  Local_v8__Function_\
            ( native.Local_v8__Function__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__Function_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__Function_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Function__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Function_(native.Local_v8__Function__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Function__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__Function___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__Function___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.Function
        '''
        return native.v8___Local_v8__Function___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.Function
        '''
        return native.v8___Local_v8__Function___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Function_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Function_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Function_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__Function_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__Function_))
        return Local_v8__Function_(native.v8___Local_v8__Function___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__Function_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__Function_)))
        return Local_v8__Function_(native.v8___Local_v8__Function___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__Function_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Function))
        return Local_v8__Function_(native.v8___Local_v8__Function___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__Function_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__Function_)))
        return (native.v8___Local_v8__Function___assign(self._cobject, arg1._cobject,))        

class Local_v8__Object_( _FwdDecl_Local_v8__Object_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__Object_\
            ( native.Local_v8__Object__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__Object_            
        """
        assert(isinstance(that,Local_v8__Object_))
        obj =  Local_v8__Object_\
            ( native.Local_v8__Object__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__Object_)            
        """
        assert(isinstance(arg1,Local_v8__Object_))
        obj =  Local_v8__Object_\
            ( native.Local_v8__Object__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__Object_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__Object_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Object__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Object_(native.Local_v8__Object__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Object__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__Object___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__Object___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.Object
        '''
        return native.v8___Local_v8__Object___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.Object
        '''
        return native.v8___Local_v8__Object___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Object_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Object_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Object_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__Object_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__Object_))
        return Local_v8__Object_(native.v8___Local_v8__Object___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__Object_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__Object_)))
        return Local_v8__Object_(native.v8___Local_v8__Object___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__Object_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Object))
        return Local_v8__Object_(native.v8___Local_v8__Object___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__Object_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__Object_)))
        return (native.v8___Local_v8__Object___assign(self._cobject, arg1._cobject,))        

class Local_v8__StackTrace_( _FwdDecl_Local_v8__StackTrace_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__StackTrace_\
            ( native.Local_v8__StackTrace__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__StackTrace_            
        """
        assert(isinstance(that,Local_v8__StackTrace_))
        obj =  Local_v8__StackTrace_\
            ( native.Local_v8__StackTrace__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__StackTrace_)            
        """
        assert(isinstance(arg1,Local_v8__StackTrace_))
        obj =  Local_v8__StackTrace_\
            ( native.Local_v8__StackTrace__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__StackTrace_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__StackTrace_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__StackTrace__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__StackTrace_(native.Local_v8__StackTrace__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__StackTrace__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__StackTrace___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__StackTrace___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.StackTrace
        '''
        return native.v8___Local_v8__StackTrace___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.StackTrace
        '''
        return native.v8___Local_v8__StackTrace___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__StackTrace_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__StackTrace_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__StackTrace_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__StackTrace_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__StackTrace_))
        return Local_v8__StackTrace_(native.v8___Local_v8__StackTrace___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__StackTrace_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__StackTrace_)))
        return Local_v8__StackTrace_(native.v8___Local_v8__StackTrace___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__StackTrace_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,StackTrace))
        return Local_v8__StackTrace_(native.v8___Local_v8__StackTrace___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__StackTrace_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__StackTrace_)))
        return (native.v8___Local_v8__StackTrace___assign(self._cobject, arg1._cobject,))        

class Local_v8__StackFrame_( _FwdDecl_Local_v8__StackFrame_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__StackFrame__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__StackFrame_(native.Local_v8__StackFrame__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__StackFrame__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Array_( _FwdDecl_Local_v8__Array_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Array__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Array_(native.Local_v8__Array__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Array__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__NativeWeakMap_( _FwdDecl_Local_v8__NativeWeakMap_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__NativeWeakMap__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__NativeWeakMap_(native.Local_v8__NativeWeakMap__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__NativeWeakMap__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Number_( _FwdDecl_Local_v8__Number_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__Number_\
            ( native.Local_v8__Number__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__Number_            
        """
        assert(isinstance(that,Local_v8__Number_))
        obj =  Local_v8__Number_\
            ( native.Local_v8__Number__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__Number_)            
        """
        assert(isinstance(arg1,Local_v8__Number_))
        obj =  Local_v8__Number_\
            ( native.Local_v8__Number__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__Number_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__Number_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Number__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Number_(native.Local_v8__Number__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Number__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__Number___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__Number___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.Number
        '''
        return native.v8___Local_v8__Number___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.Number
        '''
        return native.v8___Local_v8__Number___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Number_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Number_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Number_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__Number_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__Number_))
        return Local_v8__Number_(native.v8___Local_v8__Number___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__Number_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__Number_)))
        return Local_v8__Number_(native.v8___Local_v8__Number___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__Number_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Number))
        return Local_v8__Number_(native.v8___Local_v8__Number___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__Number_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__Number_)))
        return (native.v8___Local_v8__Number___assign(self._cobject, arg1._cobject,))        

class Local_v8__Uint32_( _FwdDecl_Local_v8__Uint32_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__Uint32_\
            ( native.Local_v8__Uint32__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__Uint32_            
        """
        assert(isinstance(that,Local_v8__Uint32_))
        obj =  Local_v8__Uint32_\
            ( native.Local_v8__Uint32__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__Uint32_)            
        """
        assert(isinstance(arg1,Local_v8__Uint32_))
        obj =  Local_v8__Uint32_\
            ( native.Local_v8__Uint32__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__Uint32_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__Uint32_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Uint32__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Uint32_(native.Local_v8__Uint32__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Uint32__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__Uint32___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__Uint32___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.Uint32
        '''
        return native.v8___Local_v8__Uint32___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.Uint32
        '''
        return native.v8___Local_v8__Uint32___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Uint32_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Uint32_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Uint32_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__Uint32_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__Uint32_))
        return Local_v8__Uint32_(native.v8___Local_v8__Uint32___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__Uint32_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__Uint32_)))
        return Local_v8__Uint32_(native.v8___Local_v8__Uint32___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__Uint32_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Uint32))
        return Local_v8__Uint32_(native.v8___Local_v8__Uint32___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__Uint32_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__Uint32_)))
        return (native.v8___Local_v8__Uint32___assign(self._cobject, arg1._cobject,))        

class Local_v8__Int32_( _FwdDecl_Local_v8__Int32_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__Int32_\
            ( native.Local_v8__Int32__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__Int32_            
        """
        assert(isinstance(that,Local_v8__Int32_))
        obj =  Local_v8__Int32_\
            ( native.Local_v8__Int32__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__Int32_)            
        """
        assert(isinstance(arg1,Local_v8__Int32_))
        obj =  Local_v8__Int32_\
            ( native.Local_v8__Int32__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__Int32_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__Int32_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Int32__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Int32_(native.Local_v8__Int32__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Int32__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__Int32___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__Int32___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.Int32
        '''
        return native.v8___Local_v8__Int32___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.Int32
        '''
        return native.v8___Local_v8__Int32___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Int32_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Int32_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Int32_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__Int32_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__Int32_))
        return Local_v8__Int32_(native.v8___Local_v8__Int32___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__Int32_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__Int32_)))
        return Local_v8__Int32_(native.v8___Local_v8__Int32___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__Int32_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Int32))
        return Local_v8__Int32_(native.v8___Local_v8__Int32___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__Int32_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__Int32_)))
        return (native.v8___Local_v8__Int32___assign(self._cobject, arg1._cobject,))        

class Local_v8__Symbol_( _FwdDecl_Local_v8__Symbol_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Symbol__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Symbol_(native.Local_v8__Symbol__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Symbol__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Name_( _FwdDecl_Local_v8__Name_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__Name_\
            ( native.Local_v8__Name__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__String_            
        """
        assert(isinstance(that,Local_v8__Name_))
        obj =  Local_v8__Name_\
            ( native.Local_v8__Name__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, that,):
        """
        Signature: 
           that:v8.Local_v8__Name_            
        """
        assert(isinstance(that,Local_v8__Name_))
        obj =  Local_v8__Name_\
            ( native.Local_v8__Name__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new4(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__Name_)            
        """
        assert(isinstance(arg1,Local_v8__Name_))
        obj =  Local_v8__Name_\
            ( native.Local_v8__Name__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__String_            
        
        Signature[3]: 
           args[0]:v8.Local_v8__Name_            
        
        Signature[4]: 
           args[0]:POINTER(v8.Local_v8__Name_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Name__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new4(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Name_(native.Local_v8__Name__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Name__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__Name___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__Name___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.Name
        '''
        return native.v8___Local_v8__Name___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.Name
        '''
        return native.v8___Local_v8__Name___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Name_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Name_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Name_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__Name_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__Name_))
        return Local_v8__Name_(native.v8___Local_v8__Name___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__Name_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__Name_)))
        return Local_v8__Name_(native.v8___Local_v8__Name___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__Name_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Name))
        return Local_v8__Name_(native.v8___Local_v8__Name___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__Name_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__Name_)))
        return (native.v8___Local_v8__Name___assign(self._cobject, arg1._cobject,))        

class Local_v8__FunctionTemplate_( _FwdDecl_Local_v8__FunctionTemplate_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__FunctionTemplate_\
            ( native.Local_v8__FunctionTemplate__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__FunctionTemplate_            
        """
        assert(isinstance(that,Local_v8__FunctionTemplate_))
        obj =  Local_v8__FunctionTemplate_\
            ( native.Local_v8__FunctionTemplate__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__FunctionTemplate_)            
        """
        assert(isinstance(arg1,Local_v8__FunctionTemplate_))
        obj =  Local_v8__FunctionTemplate_\
            ( native.Local_v8__FunctionTemplate__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__FunctionTemplate_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__FunctionTemplate_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__FunctionTemplate__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__FunctionTemplate_(native.Local_v8__FunctionTemplate__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__FunctionTemplate__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__FunctionTemplate___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__FunctionTemplate___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.FunctionTemplate
        '''
        return native.v8___Local_v8__FunctionTemplate___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.FunctionTemplate
        '''
        return native.v8___Local_v8__FunctionTemplate___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__FunctionTemplate_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__FunctionTemplate_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__FunctionTemplate_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__FunctionTemplate_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__FunctionTemplate_))
        return Local_v8__FunctionTemplate_(native.v8___Local_v8__FunctionTemplate___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__FunctionTemplate_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__FunctionTemplate_)))
        return Local_v8__FunctionTemplate_(native.v8___Local_v8__FunctionTemplate___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__FunctionTemplate_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,FunctionTemplate))
        return Local_v8__FunctionTemplate_(native.v8___Local_v8__FunctionTemplate___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__FunctionTemplate_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__FunctionTemplate_)))
        return (native.v8___Local_v8__FunctionTemplate___assign(self._cobject, arg1._cobject,))        

class Local_v8__Map_( _FwdDecl_Local_v8__Map_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Map__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Map_(native.Local_v8__Map__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Map__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Set_( _FwdDecl_Local_v8__Set_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Set__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Set_(native.Local_v8__Set__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Set__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Promise__Resolver_( _FwdDecl_Local_v8__Promise__Resolver_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Promise__Resolver__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Promise__Resolver_(native.Local_v8__Promise__Resolver__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Promise__Resolver__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Promise_( _FwdDecl_Local_v8__Promise_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__Promise_\
            ( native.Local_v8__Promise__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__Promise_            
        """
        assert(isinstance(that,Local_v8__Promise_))
        obj =  Local_v8__Promise_\
            ( native.Local_v8__Promise__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__Promise_)            
        """
        assert(isinstance(arg1,Local_v8__Promise_))
        obj =  Local_v8__Promise_\
            ( native.Local_v8__Promise__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__Promise_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__Promise_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Promise__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Promise_(native.Local_v8__Promise__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Promise__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__Promise___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__Promise___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.Promise
        '''
        return native.v8___Local_v8__Promise___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.Promise
        '''
        return native.v8___Local_v8__Promise___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Promise_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Promise_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Promise_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__Promise_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__Promise_))
        return Local_v8__Promise_(native.v8___Local_v8__Promise___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__Promise_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__Promise_)))
        return Local_v8__Promise_(native.v8___Local_v8__Promise___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__Promise_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Promise))
        return Local_v8__Promise_(native.v8___Local_v8__Promise___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__Promise_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__Promise_)))
        return (native.v8___Local_v8__Promise___assign(self._cobject, arg1._cobject,))        

class Local_v8__ArrayBuffer_( _FwdDecl_Local_v8__ArrayBuffer_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__ArrayBuffer__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__ArrayBuffer_(native.Local_v8__ArrayBuffer__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__ArrayBuffer__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Uint8Array_( _FwdDecl_Local_v8__Uint8Array_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Uint8Array__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Uint8Array_(native.Local_v8__Uint8Array__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Uint8Array__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__SharedArrayBuffer_( _FwdDecl_Local_v8__SharedArrayBuffer_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__SharedArrayBuffer__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__SharedArrayBuffer_(native.Local_v8__SharedArrayBuffer__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__SharedArrayBuffer__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Uint8ClampedArray_( _FwdDecl_Local_v8__Uint8ClampedArray_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Uint8ClampedArray__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Uint8ClampedArray_(native.Local_v8__Uint8ClampedArray__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Uint8ClampedArray__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Int8Array_( _FwdDecl_Local_v8__Int8Array_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Int8Array__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Int8Array_(native.Local_v8__Int8Array__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Int8Array__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Uint16Array_( _FwdDecl_Local_v8__Uint16Array_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Uint16Array__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Uint16Array_(native.Local_v8__Uint16Array__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Uint16Array__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Int16Array_( _FwdDecl_Local_v8__Int16Array_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Int16Array__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Int16Array_(native.Local_v8__Int16Array__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Int16Array__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Uint32Array_( _FwdDecl_Local_v8__Uint32Array_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Uint32Array__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Uint32Array_(native.Local_v8__Uint32Array__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Uint32Array__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Int32Array_( _FwdDecl_Local_v8__Int32Array_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Int32Array__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Int32Array_(native.Local_v8__Int32Array__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Int32Array__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Float32Array_( _FwdDecl_Local_v8__Float32Array_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Float32Array__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Float32Array_(native.Local_v8__Float32Array__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Float32Array__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Float64Array_( _FwdDecl_Local_v8__Float64Array_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Float64Array__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Float64Array_(native.Local_v8__Float64Array__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Float64Array__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__DataView_( _FwdDecl_Local_v8__DataView_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__DataView__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__DataView_(native.Local_v8__DataView__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__DataView__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__RegExp_( _FwdDecl_Local_v8__RegExp_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__RegExp__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__RegExp_(native.Local_v8__RegExp__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__RegExp__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__External_( _FwdDecl_Local_v8__External_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__External__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__External_(native.Local_v8__External__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__External__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Data_( _FwdDecl_Local_v8__Data_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__Data_\
            ( native.Local_v8__Data__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__Data_            
        """
        assert(isinstance(that,Local_v8__Data_))
        obj =  Local_v8__Data_\
            ( native.Local_v8__Data__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__Data_)            
        """
        assert(isinstance(arg1,Local_v8__Data_))
        obj =  Local_v8__Data_\
            ( native.Local_v8__Data__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__Data_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__Data_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Data__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Data_(native.Local_v8__Data__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Data__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__Data___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__Data___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.Data
        '''
        return native.v8___Local_v8__Data___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.Data
        '''
        return native.v8___Local_v8__Data___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Data_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Data_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Data_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__Data_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__Data_))
        return Local_v8__Data_(native.v8___Local_v8__Data___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__Data_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__Data_)))
        return Local_v8__Data_(native.v8___Local_v8__Data___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__Data_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Data))
        return Local_v8__Data_(native.v8___Local_v8__Data___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__Data_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__Data_)))
        return (native.v8___Local_v8__Data___assign(self._cobject, arg1._cobject,))        

class Local_v8__AccessorSignature_( _FwdDecl_Local_v8__AccessorSignature_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__AccessorSignature_\
            ( native.Local_v8__AccessorSignature__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__AccessorSignature_            
        """
        assert(isinstance(that,Local_v8__AccessorSignature_))
        obj =  Local_v8__AccessorSignature_\
            ( native.Local_v8__AccessorSignature__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__AccessorSignature_)            
        """
        assert(isinstance(arg1,Local_v8__AccessorSignature_))
        obj =  Local_v8__AccessorSignature_\
            ( native.Local_v8__AccessorSignature__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__AccessorSignature_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__AccessorSignature_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__AccessorSignature__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__AccessorSignature_(native.Local_v8__AccessorSignature__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__AccessorSignature__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__AccessorSignature___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__AccessorSignature___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.AccessorSignature
        '''
        return native.v8___Local_v8__AccessorSignature___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.AccessorSignature
        '''
        return native.v8___Local_v8__AccessorSignature___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__AccessorSignature_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__AccessorSignature_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__AccessorSignature_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__AccessorSignature_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__AccessorSignature_))
        return Local_v8__AccessorSignature_(native.v8___Local_v8__AccessorSignature___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__AccessorSignature_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__AccessorSignature_)))
        return Local_v8__AccessorSignature_(native.v8___Local_v8__AccessorSignature___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__AccessorSignature_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,AccessorSignature))
        return Local_v8__AccessorSignature_(native.v8___Local_v8__AccessorSignature___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__AccessorSignature_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__AccessorSignature_)))
        return (native.v8___Local_v8__AccessorSignature___assign(self._cobject, arg1._cobject,))        

class Local_v8__Signature_( _FwdDecl_Local_v8__Signature_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__Signature_\
            ( native.Local_v8__Signature__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__Signature_            
        """
        assert(isinstance(that,Local_v8__Signature_))
        obj =  Local_v8__Signature_\
            ( native.Local_v8__Signature__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__Signature_)            
        """
        assert(isinstance(arg1,Local_v8__Signature_))
        obj =  Local_v8__Signature_\
            ( native.Local_v8__Signature__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__Signature_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__Signature_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Signature__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Signature_(native.Local_v8__Signature__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Signature__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__Signature___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__Signature___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.Signature
        '''
        return native.v8___Local_v8__Signature___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.Signature
        '''
        return native.v8___Local_v8__Signature___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Signature_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Signature_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__Signature_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__Signature_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__Signature_))
        return Local_v8__Signature_(native.v8___Local_v8__Signature___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__Signature_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__Signature_)))
        return Local_v8__Signature_(native.v8___Local_v8__Signature___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__Signature_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Signature))
        return Local_v8__Signature_(native.v8___Local_v8__Signature___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__Signature_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__Signature_)))
        return (native.v8___Local_v8__Signature___assign(self._cobject, arg1._cobject,))        

class Local_v8__ObjectTemplate_( _FwdDecl_Local_v8__ObjectTemplate_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Local_v8__ObjectTemplate_\
            ( native.Local_v8__ObjectTemplate__new())
        return obj
        

    @staticmethod
    def new2(self, that,):
        """
        Signature: 
           that:v8.Local_v8__ObjectTemplate_            
        """
        assert(isinstance(that,Local_v8__ObjectTemplate_))
        obj =  Local_v8__ObjectTemplate_\
            ( native.Local_v8__ObjectTemplate__new(that._cobject, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Local_v8__ObjectTemplate_)            
        """
        assert(isinstance(arg1,Local_v8__ObjectTemplate_))
        obj =  Local_v8__ObjectTemplate_\
            ( native.Local_v8__ObjectTemplate__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Local_v8__ObjectTemplate_            
        
        Signature[3]: 
           args[0]:POINTER(v8.Local_v8__ObjectTemplate_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__ObjectTemplate__copy(args[0])
        else:
            
            try:
                self._cobject = native.Local_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.Local_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__ObjectTemplate_(native.Local_v8__ObjectTemplate__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__ObjectTemplate__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Local_v8__ObjectTemplate___IsEmpty(self._cobject, )

    def Clear(self, ):
        '''
        @returns: void
        '''
        return native.v8___Local_v8__ObjectTemplate___Clear(self._cobject, )

    def deref2(self, ):
        '''
        @returns: v8.ObjectTemplate
        '''
        return native.v8___Local_v8__ObjectTemplate___deref2(self._cobject, )

    def deref(self, ):
        '''
        @returns: v8.ObjectTemplate
        '''
        return native.v8___Local_v8__ObjectTemplate___deref(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__ObjectTemplate_.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__ObjectTemplate_.New_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return Local_v8__ObjectTemplate_.New_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_5(isolate,that,):
        '''
        @returns: v8.Local_v8__ObjectTemplate_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Local_v8__ObjectTemplate_))
        return Local_v8__ObjectTemplate_(native.v8___Local_v8__ObjectTemplate___New(isolate,that._cobject,))

    @staticmethod
    def New_6(isolate,that,):
        '''
        @returns: v8.Local_v8__ObjectTemplate_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,POINTER(PersistentBase_v8__ObjectTemplate_)))
        return Local_v8__ObjectTemplate_(native.v8___Local_v8__ObjectTemplate___New(isolate,that._cobject,))

    @staticmethod
    def New_7(isolate,that,):
        '''
        @returns: v8.Local_v8__ObjectTemplate_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,ObjectTemplate))
        return Local_v8__ObjectTemplate_(native.v8___Local_v8__ObjectTemplate___New(isolate,that,))

    def assign(self, arg1,):
        '''
        @returns: v8.Local_v8__ObjectTemplate_
        '''
        assert(isinstance(arg1,POINTER(Local_v8__ObjectTemplate_)))
        return (native.v8___Local_v8__ObjectTemplate___assign(self._cobject, arg1._cobject,))        

class Local_v8__TypeSwitch_( _FwdDecl_Local_v8__TypeSwitch_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__TypeSwitch__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__TypeSwitch_(native.Local_v8__TypeSwitch__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__TypeSwitch__delete(self._cobject)
        self._cobjet = None
            
            

class Local_v8__Message_( _FwdDecl_Local_v8__Message_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Local_v8__Message__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Local_v8__Message_(native.Local_v8__Message__copy( self._cobject ))

    def __del__(self):
        native.Local_v8__Message__delete(self._cobject)
        self._cobjet = None
            
            

class MaybeLocal_v8__Script_( _FwdDecl_MaybeLocal_v8__Script_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__Script__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__Script_(native.MaybeLocal_v8__Script__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__Script__delete(self._cobject)
        self._cobjet = None
            
            

class MaybeLocal_v8__Value_( _FwdDecl_MaybeLocal_v8__Value_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  MaybeLocal_v8__Value_\
            ( native.MaybeLocal_v8__Value__new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.MaybeLocal_v8__Value_)            
        """
        assert(isinstance(arg1,MaybeLocal_v8__Value_))
        obj =  MaybeLocal_v8__Value_\
            ( native.MaybeLocal_v8__Value__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.MaybeLocal_v8__Value_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__Value__copy(args[0])
        else:
            
            try:
                self._cobject = native.MaybeLocal_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.MaybeLocal_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__Value_(native.MaybeLocal_v8__Value__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__Value__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___MaybeLocal_v8__Value___IsEmpty(self._cobject, )

    def ToLocalChecked(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___MaybeLocal_v8__Value___ToLocalChecked(self._cobject, ))

    def assign(self, arg1,):
        '''
        @returns: v8.MaybeLocal_v8__Value_
        '''
        assert(isinstance(arg1,POINTER(MaybeLocal_v8__Value_)))
        return (native.v8___MaybeLocal_v8__Value___assign(self._cobject, arg1._cobject,))        

class MaybeLocal_v8__UnboundScript_( _FwdDecl_MaybeLocal_v8__UnboundScript_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__UnboundScript__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__UnboundScript_(native.MaybeLocal_v8__UnboundScript__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__UnboundScript__delete(self._cobject)
        self._cobjet = None
            
            

class MaybeLocal_v8__Function_( _FwdDecl_MaybeLocal_v8__Function_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__Function__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__Function_(native.MaybeLocal_v8__Function__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__Function__delete(self._cobject)
        self._cobjet = None
            
            

class MaybeLocal_v8__String_( _FwdDecl_MaybeLocal_v8__String_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  MaybeLocal_v8__String_\
            ( native.MaybeLocal_v8__String__new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.MaybeLocal_v8__String_)            
        """
        assert(isinstance(arg1,MaybeLocal_v8__String_))
        obj =  MaybeLocal_v8__String_\
            ( native.MaybeLocal_v8__String__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.MaybeLocal_v8__String_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__String__copy(args[0])
        else:
            
            try:
                self._cobject = native.MaybeLocal_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.MaybeLocal_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__String_(native.MaybeLocal_v8__String__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__String__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___MaybeLocal_v8__String___IsEmpty(self._cobject, )

    def ToLocalChecked(self, ):
        '''
        @returns: v8.Local_v8__String_
        '''
        return Local_v8__String_(native.v8___MaybeLocal_v8__String___ToLocalChecked(self._cobject, ))

    def FromMaybe(self, default_value,):
        '''
        @returns: v8.Local_v8__String_
        '''
        assert(isinstance(default_value,Local_v8__String_))
        return Local_v8__String_(native.v8___MaybeLocal_v8__String___FromMaybe(self._cobject, default_value._cobject,))

    def assign(self, arg1,):
        '''
        @returns: v8.MaybeLocal_v8__String_
        '''
        assert(isinstance(arg1,POINTER(MaybeLocal_v8__String_)))
        return (native.v8___MaybeLocal_v8__String___assign(self._cobject, arg1._cobject,))        

class MaybeLocal_v8__Boolean_( _FwdDecl_MaybeLocal_v8__Boolean_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  MaybeLocal_v8__Boolean_\
            ( native.MaybeLocal_v8__Boolean__new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.MaybeLocal_v8__Boolean_)            
        """
        assert(isinstance(arg1,MaybeLocal_v8__Boolean_))
        obj =  MaybeLocal_v8__Boolean_\
            ( native.MaybeLocal_v8__Boolean__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.MaybeLocal_v8__Boolean_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__Boolean__copy(args[0])
        else:
            
            try:
                self._cobject = native.MaybeLocal_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.MaybeLocal_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__Boolean_(native.MaybeLocal_v8__Boolean__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__Boolean__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___MaybeLocal_v8__Boolean___IsEmpty(self._cobject, )

    def ToLocalChecked(self, ):
        '''
        @returns: v8.Local_v8__Boolean_
        '''
        return Local_v8__Boolean_(native.v8___MaybeLocal_v8__Boolean___ToLocalChecked(self._cobject, ))

    def FromMaybe(self, default_value,):
        '''
        @returns: v8.Local_v8__Boolean_
        '''
        assert(isinstance(default_value,Local_v8__Boolean_))
        return Local_v8__Boolean_(native.v8___MaybeLocal_v8__Boolean___FromMaybe(self._cobject, default_value._cobject,))

    def assign(self, arg1,):
        '''
        @returns: v8.MaybeLocal_v8__Boolean_
        '''
        assert(isinstance(arg1,POINTER(MaybeLocal_v8__Boolean_)))
        return (native.v8___MaybeLocal_v8__Boolean___assign(self._cobject, arg1._cobject,))        

class MaybeLocal_v8__Number_( _FwdDecl_MaybeLocal_v8__Number_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  MaybeLocal_v8__Number_\
            ( native.MaybeLocal_v8__Number__new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.MaybeLocal_v8__Number_)            
        """
        assert(isinstance(arg1,MaybeLocal_v8__Number_))
        obj =  MaybeLocal_v8__Number_\
            ( native.MaybeLocal_v8__Number__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.MaybeLocal_v8__Number_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__Number__copy(args[0])
        else:
            
            try:
                self._cobject = native.MaybeLocal_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.MaybeLocal_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__Number_(native.MaybeLocal_v8__Number__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__Number__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___MaybeLocal_v8__Number___IsEmpty(self._cobject, )

    def ToLocalChecked(self, ):
        '''
        @returns: v8.Local_v8__Number_
        '''
        return Local_v8__Number_(native.v8___MaybeLocal_v8__Number___ToLocalChecked(self._cobject, ))

    def FromMaybe(self, default_value,):
        '''
        @returns: v8.Local_v8__Number_
        '''
        assert(isinstance(default_value,Local_v8__Number_))
        return Local_v8__Number_(native.v8___MaybeLocal_v8__Number___FromMaybe(self._cobject, default_value._cobject,))

    def assign(self, arg1,):
        '''
        @returns: v8.MaybeLocal_v8__Number_
        '''
        assert(isinstance(arg1,POINTER(MaybeLocal_v8__Number_)))
        return (native.v8___MaybeLocal_v8__Number___assign(self._cobject, arg1._cobject,))        

class MaybeLocal_v8__Object_( _FwdDecl_MaybeLocal_v8__Object_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  MaybeLocal_v8__Object_\
            ( native.MaybeLocal_v8__Object__new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.MaybeLocal_v8__Object_)            
        """
        assert(isinstance(arg1,MaybeLocal_v8__Object_))
        obj =  MaybeLocal_v8__Object_\
            ( native.MaybeLocal_v8__Object__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.MaybeLocal_v8__Object_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__Object__copy(args[0])
        else:
            
            try:
                self._cobject = native.MaybeLocal_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.MaybeLocal_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__Object_(native.MaybeLocal_v8__Object__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__Object__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___MaybeLocal_v8__Object___IsEmpty(self._cobject, )

    def ToLocalChecked(self, ):
        '''
        @returns: v8.Local_v8__Object_
        '''
        return Local_v8__Object_(native.v8___MaybeLocal_v8__Object___ToLocalChecked(self._cobject, ))

    def FromMaybe(self, default_value,):
        '''
        @returns: v8.Local_v8__Object_
        '''
        assert(isinstance(default_value,Local_v8__Object_))
        return Local_v8__Object_(native.v8___MaybeLocal_v8__Object___FromMaybe(self._cobject, default_value._cobject,))

    def assign(self, arg1,):
        '''
        @returns: v8.MaybeLocal_v8__Object_
        '''
        assert(isinstance(arg1,POINTER(MaybeLocal_v8__Object_)))
        return (native.v8___MaybeLocal_v8__Object___assign(self._cobject, arg1._cobject,))        

class MaybeLocal_v8__Integer_( _FwdDecl_MaybeLocal_v8__Integer_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  MaybeLocal_v8__Integer_\
            ( native.MaybeLocal_v8__Integer__new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.MaybeLocal_v8__Integer_)            
        """
        assert(isinstance(arg1,MaybeLocal_v8__Integer_))
        obj =  MaybeLocal_v8__Integer_\
            ( native.MaybeLocal_v8__Integer__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.MaybeLocal_v8__Integer_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__Integer__copy(args[0])
        else:
            
            try:
                self._cobject = native.MaybeLocal_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.MaybeLocal_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__Integer_(native.MaybeLocal_v8__Integer__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__Integer__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___MaybeLocal_v8__Integer___IsEmpty(self._cobject, )

    def ToLocalChecked(self, ):
        '''
        @returns: v8.Local_v8__Integer_
        '''
        return Local_v8__Integer_(native.v8___MaybeLocal_v8__Integer___ToLocalChecked(self._cobject, ))

    def FromMaybe(self, default_value,):
        '''
        @returns: v8.Local_v8__Integer_
        '''
        assert(isinstance(default_value,Local_v8__Integer_))
        return Local_v8__Integer_(native.v8___MaybeLocal_v8__Integer___FromMaybe(self._cobject, default_value._cobject,))

    def assign(self, arg1,):
        '''
        @returns: v8.MaybeLocal_v8__Integer_
        '''
        assert(isinstance(arg1,POINTER(MaybeLocal_v8__Integer_)))
        return (native.v8___MaybeLocal_v8__Integer___assign(self._cobject, arg1._cobject,))        

class MaybeLocal_v8__Uint32_( _FwdDecl_MaybeLocal_v8__Uint32_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  MaybeLocal_v8__Uint32_\
            ( native.MaybeLocal_v8__Uint32__new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.MaybeLocal_v8__Uint32_)            
        """
        assert(isinstance(arg1,MaybeLocal_v8__Uint32_))
        obj =  MaybeLocal_v8__Uint32_\
            ( native.MaybeLocal_v8__Uint32__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.MaybeLocal_v8__Uint32_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__Uint32__copy(args[0])
        else:
            
            try:
                self._cobject = native.MaybeLocal_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.MaybeLocal_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__Uint32_(native.MaybeLocal_v8__Uint32__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__Uint32__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___MaybeLocal_v8__Uint32___IsEmpty(self._cobject, )

    def ToLocalChecked(self, ):
        '''
        @returns: v8.Local_v8__Uint32_
        '''
        return Local_v8__Uint32_(native.v8___MaybeLocal_v8__Uint32___ToLocalChecked(self._cobject, ))

    def FromMaybe(self, default_value,):
        '''
        @returns: v8.Local_v8__Uint32_
        '''
        assert(isinstance(default_value,Local_v8__Uint32_))
        return Local_v8__Uint32_(native.v8___MaybeLocal_v8__Uint32___FromMaybe(self._cobject, default_value._cobject,))

    def assign(self, arg1,):
        '''
        @returns: v8.MaybeLocal_v8__Uint32_
        '''
        assert(isinstance(arg1,POINTER(MaybeLocal_v8__Uint32_)))
        return (native.v8___MaybeLocal_v8__Uint32___assign(self._cobject, arg1._cobject,))        

class MaybeLocal_v8__Int32_( _FwdDecl_MaybeLocal_v8__Int32_ ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  MaybeLocal_v8__Int32_\
            ( native.MaybeLocal_v8__Int32__new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.MaybeLocal_v8__Int32_)            
        """
        assert(isinstance(arg1,MaybeLocal_v8__Int32_))
        obj =  MaybeLocal_v8__Int32_\
            ( native.MaybeLocal_v8__Int32__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.MaybeLocal_v8__Int32_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__Int32__copy(args[0])
        else:
            
            try:
                self._cobject = native.MaybeLocal_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.MaybeLocal_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__Int32_(native.MaybeLocal_v8__Int32__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__Int32__delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___MaybeLocal_v8__Int32___IsEmpty(self._cobject, )

    def ToLocalChecked(self, ):
        '''
        @returns: v8.Local_v8__Int32_
        '''
        return Local_v8__Int32_(native.v8___MaybeLocal_v8__Int32___ToLocalChecked(self._cobject, ))

    def FromMaybe(self, default_value,):
        '''
        @returns: v8.Local_v8__Int32_
        '''
        assert(isinstance(default_value,Local_v8__Int32_))
        return Local_v8__Int32_(native.v8___MaybeLocal_v8__Int32___FromMaybe(self._cobject, default_value._cobject,))

    def assign(self, arg1,):
        '''
        @returns: v8.MaybeLocal_v8__Int32_
        '''
        assert(isinstance(arg1,POINTER(MaybeLocal_v8__Int32_)))
        return (native.v8___MaybeLocal_v8__Int32___assign(self._cobject, arg1._cobject,))        

class MaybeLocal_v8__Array_( _FwdDecl_MaybeLocal_v8__Array_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__Array__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__Array_(native.MaybeLocal_v8__Array__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__Array__delete(self._cobject)
        self._cobjet = None
            
            

class MaybeLocal_v8__Map_( _FwdDecl_MaybeLocal_v8__Map_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__Map__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__Map_(native.MaybeLocal_v8__Map__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__Map__delete(self._cobject)
        self._cobjet = None
            
            

class MaybeLocal_v8__Set_( _FwdDecl_MaybeLocal_v8__Set_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__Set__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__Set_(native.MaybeLocal_v8__Set__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__Set__delete(self._cobject)
        self._cobjet = None
            
            

class MaybeLocal_v8__Promise__Resolver_( _FwdDecl_MaybeLocal_v8__Promise__Resolver_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__Promise__Resolver__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__Promise__Resolver_(native.MaybeLocal_v8__Promise__Resolver__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__Promise__Resolver__delete(self._cobject)
        self._cobjet = None
            
            

class MaybeLocal_v8__Promise_( _FwdDecl_MaybeLocal_v8__Promise_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__Promise__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__Promise_(native.MaybeLocal_v8__Promise__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__Promise__delete(self._cobject)
        self._cobjet = None
            
            

class MaybeLocal_v8__RegExp_( _FwdDecl_MaybeLocal_v8__RegExp_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.MaybeLocal_v8__RegExp__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return MaybeLocal_v8__RegExp_(native.MaybeLocal_v8__RegExp__copy( self._cobject ))

    def __del__(self):
        native.MaybeLocal_v8__RegExp__delete(self._cobject)
        self._cobjet = None
            
            

class NonCopyablePersistentTraits_v8__Value_( _FwdDecl_NonCopyablePersistentTraits_v8__Value_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.NonCopyablePersistentTraits_v8__Value__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return NonCopyablePersistentTraits_v8__Value_(native.NonCopyablePersistentTraits_v8__Value__copy( self._cobject ))

    def __del__(self):
        native.NonCopyablePersistentTraits_v8__Value__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__Value_( _FwdDecl_PersistentBase_v8__Value_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__Value__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__Value_(native.PersistentBase_v8__Value__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__Value__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__Integer_( _FwdDecl_PersistentBase_v8__Integer_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__Integer__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__Integer_(native.PersistentBase_v8__Integer__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__Integer__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__Boolean_( _FwdDecl_PersistentBase_v8__Boolean_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__Boolean__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__Boolean_(native.PersistentBase_v8__Boolean__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__Boolean__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__UnboundScript_( _FwdDecl_PersistentBase_v8__UnboundScript_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__UnboundScript__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__UnboundScript_(native.PersistentBase_v8__UnboundScript__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__UnboundScript__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__String_( _FwdDecl_PersistentBase_v8__String_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__String__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__String_(native.PersistentBase_v8__String__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__String__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__Object_( _FwdDecl_PersistentBase_v8__Object_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__Object__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__Object_(native.PersistentBase_v8__Object__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__Object__delete(self._cobject)
        self._cobjet = None
            
    

    def Reset(self, ):
        '''
        @returns: void
        '''
        return native.v8___PersistentBase_v8__Object___Reset(self._cobject, )

    def IsEmpty(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___PersistentBase_v8__Object___IsEmpty(self._cobject, )

    def Empty(self, ):
        '''
        @returns: void
        '''
        return native.v8___PersistentBase_v8__Object___Empty(self._cobject, )

    def ClearWeak(self, ):
        '''
        @returns: void
        '''
        return native.v8___PersistentBase_v8__Object___ClearWeak(self._cobject, )

    def MarkIndependent(self, ):
        '''
        @returns: void
        '''
        return native.v8___PersistentBase_v8__Object___MarkIndependent(self._cobject, )

    def MarkPartiallyDependent(self, ):
        '''
        @returns: void
        '''
        return native.v8___PersistentBase_v8__Object___MarkPartiallyDependent(self._cobject, )

    def IsIndependent(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___PersistentBase_v8__Object___IsIndependent(self._cobject, )

    def IsNearDeath(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___PersistentBase_v8__Object___IsNearDeath(self._cobject, )

    def IsWeak(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___PersistentBase_v8__Object___IsWeak(self._cobject, )

    def SetWrapperClassId(self, class_id,):
        '''
        @returns: void
        '''
        assert(isinstance(class_id,uint16_t))
        return native.v8___PersistentBase_v8__Object___SetWrapperClassId(self._cobject, class_id,)


    def WrapperClassId(self, ):
        '''
        @returns: uint16_t
        '''
        return native.v8___PersistentBase_v8__Object___WrapperClassId(self._cobject, )

    @staticmethod
    def New(isolate,that,):
        '''
        @returns: v8.Object
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(that,Object))
        return native.v8___PersistentBase_v8__Object___New(isolate,that,)        

class PersistentBase_v8__Function_( _FwdDecl_PersistentBase_v8__Function_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__Function__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__Function_(native.PersistentBase_v8__Function__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__Function__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__Context_( _FwdDecl_PersistentBase_v8__Context_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__Context__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__Context_(native.PersistentBase_v8__Context__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__Context__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__FunctionTemplate_( _FwdDecl_PersistentBase_v8__FunctionTemplate_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__FunctionTemplate__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__FunctionTemplate_(native.PersistentBase_v8__FunctionTemplate__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__FunctionTemplate__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__AccessorSignature_( _FwdDecl_PersistentBase_v8__AccessorSignature_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__AccessorSignature__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__AccessorSignature_(native.PersistentBase_v8__AccessorSignature__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__AccessorSignature__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__Signature_( _FwdDecl_PersistentBase_v8__Signature_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__Signature__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__Signature_(native.PersistentBase_v8__Signature__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__Signature__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__Promise_( _FwdDecl_PersistentBase_v8__Promise_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__Promise__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__Promise_(native.PersistentBase_v8__Promise__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__Promise__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__StackTrace_( _FwdDecl_PersistentBase_v8__StackTrace_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__StackTrace__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__StackTrace_(native.PersistentBase_v8__StackTrace__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__StackTrace__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__ObjectTemplate_( _FwdDecl_PersistentBase_v8__ObjectTemplate_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__ObjectTemplate__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__ObjectTemplate_(native.PersistentBase_v8__ObjectTemplate__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__ObjectTemplate__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__Data_( _FwdDecl_PersistentBase_v8__Data_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__Data__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__Data_(native.PersistentBase_v8__Data__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__Data__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__Name_( _FwdDecl_PersistentBase_v8__Name_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__Name__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__Name_(native.PersistentBase_v8__Name__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__Name__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__Number_( _FwdDecl_PersistentBase_v8__Number_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__Number__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__Number_(native.PersistentBase_v8__Number__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__Number__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__Uint32_( _FwdDecl_PersistentBase_v8__Uint32_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__Uint32__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__Uint32_(native.PersistentBase_v8__Uint32__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__Uint32__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__Int32_( _FwdDecl_PersistentBase_v8__Int32_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__Int32__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__Int32_(native.PersistentBase_v8__Int32__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__Int32__delete(self._cobject)
        self._cobjet = None
            
            

class PersistentBase_v8__Primitive_( _FwdDecl_PersistentBase_v8__Primitive_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentBase_v8__Primitive__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentBase_v8__Primitive_(native.PersistentBase_v8__Primitive__copy( self._cobject ))

    def __del__(self):
        native.PersistentBase_v8__Primitive__delete(self._cobject)
        self._cobjet = None
            
            

class Persistent_v8__Value__v8__NonCopyablePersistentTraits_v8__Value___( _FwdDecl_Persistent_v8__Value__v8__NonCopyablePersistentTraits_v8__Value___ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Persistent_v8__Value__v8__NonCopyablePersistentTraits_v8__Value____copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Persistent_v8__Value__v8__NonCopyablePersistentTraits_v8__Value___(native.Persistent_v8__Value__v8__NonCopyablePersistentTraits_v8__Value____copy( self._cobject ))

    def __del__(self):
        native.Persistent_v8__Value__v8__NonCopyablePersistentTraits_v8__Value____delete(self._cobject)
        self._cobjet = None
            
            

class FunctionCallbackInfo_v8__Value_( _FwdDecl_FunctionCallbackInfo_v8__Value_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.FunctionCallbackInfo_v8__Value__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return FunctionCallbackInfo_v8__Value_(native.FunctionCallbackInfo_v8__Value__copy( self._cobject ))

    def __del__(self):
        native.FunctionCallbackInfo_v8__Value__delete(self._cobject)
        self._cobjet = None
            
            

class PropertyCallbackInfo_v8__Value_( _FwdDecl_PropertyCallbackInfo_v8__Value_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PropertyCallbackInfo_v8__Value__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PropertyCallbackInfo_v8__Value_(native.PropertyCallbackInfo_v8__Value__copy( self._cobject ))

    def __del__(self):
        native.PropertyCallbackInfo_v8__Value__delete(self._cobject)
        self._cobjet = None
            
            

class PropertyCallbackInfo_void_( _FwdDecl_PropertyCallbackInfo_void_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PropertyCallbackInfo_void__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PropertyCallbackInfo_void_(native.PropertyCallbackInfo_void__copy( self._cobject ))

    def __del__(self):
        native.PropertyCallbackInfo_void__delete(self._cobject)
        self._cobjet = None
            
            

class PropertyCallbackInfo_v8__Integer_( _FwdDecl_PropertyCallbackInfo_v8__Integer_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PropertyCallbackInfo_v8__Integer__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PropertyCallbackInfo_v8__Integer_(native.PropertyCallbackInfo_v8__Integer__copy( self._cobject ))

    def __del__(self):
        native.PropertyCallbackInfo_v8__Integer__delete(self._cobject)
        self._cobjet = None
            
            

class PropertyCallbackInfo_v8__Boolean_( _FwdDecl_PropertyCallbackInfo_v8__Boolean_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PropertyCallbackInfo_v8__Boolean__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PropertyCallbackInfo_v8__Boolean_(native.PropertyCallbackInfo_v8__Boolean__copy( self._cobject ))

    def __del__(self):
        native.PropertyCallbackInfo_v8__Boolean__delete(self._cobject)
        self._cobjet = None
            
            

class PropertyCallbackInfo_v8__Array_( _FwdDecl_PropertyCallbackInfo_v8__Array_ ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PropertyCallbackInfo_v8__Array__copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PropertyCallbackInfo_v8__Array_(native.PropertyCallbackInfo_v8__Array__copy( self._cobject ))

    def __del__(self):
        native.PropertyCallbackInfo_v8__Array__delete(self._cobject)
        self._cobjet = None
            
            

class CallHandlerHelper( _FwdDecl_CallHandlerHelper ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.CallHandlerHelper_copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return CallHandlerHelper(native.CallHandlerHelper_copy( self._cobject ))

    def __del__(self):
        native.CallHandlerHelper_delete(self._cobject)
        self._cobjet = None
            
            

class EscapableHandleScope( _FwdDecl_EscapableHandleScope ):

    @staticmethod
    def new1(self, isolate,):
        """
        Signature: 
           isolate:v8.Isolate            
        """
        assert(isinstance(isolate,EscapableHandleScope))
        obj =  EscapableHandleScope\
            ( native.EscapableHandleScope_new(isolate, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Isolate            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.EscapableHandleScope_copy(args[0])
        else:
            
            try:
                self._cobject = native.EscapableHandleScope_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return EscapableHandleScope(native.EscapableHandleScope_copy( self._cobject ))

    def __del__(self):
        native.EscapableHandleScope_delete(self._cobject)
        self._cobjet = None
            
    

    def Escape(self, escape_value,):
        '''
        @returns: POINTER(v8.internal.Object)
        '''
        assert(isinstance(escape_value,POINTER(v8.internal.Object)))
        return native.v8___EscapableHandleScope__Escape(self._cobject, escape_value,)


    def assign(self, arg1,):
        '''
        @returns: void
        '''
        assert(isinstance(arg1,POINTER(EscapableHandleScope)))
        return native.v8___EscapableHandleScope__assign(self._cobject, arg1._cobject,)
    
  
    
    
          

class UniqueId( _FwdDecl_UniqueId ):

    @staticmethod
    def new1(self, data,):
        """
        Signature: 
           data:intptr_t            
        """
        assert(isinstance(data,UniqueId))
        obj =  UniqueId\
            ( native.UniqueId_new(data, ))
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.UniqueId)            
        """
        assert(isinstance(arg1,UniqueId))
        obj =  UniqueId\
            ( native.UniqueId_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:intptr_t            
        
        Signature[2]: 
           args[0]:POINTER(v8.UniqueId)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.UniqueId_copy(args[0])
        else:
            
            try:
                self._cobject = native.UniqueId_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.UniqueId_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return UniqueId(native.UniqueId_copy( self._cobject ))

    def __del__(self):
        native.UniqueId_delete(self._cobject)
        self._cobjet = None
            
    

    def __cmp__(self, other,):
        '''
        @returns: c_char
        '''
        assert(isinstance(other,POINTER(UniqueId)))
        return native.v8___UniqueId____cmp__(self._cobject, other._cobject,)

    def __notcmp__(self, other,):
        '''
        @returns: c_char
        '''
        assert(isinstance(other,POINTER(UniqueId)))
        return native.v8___UniqueId____notcmp__(self._cobject, other._cobject,)

    def __lt__(self, other,):
        '''
        @returns: c_char
        '''
        assert(isinstance(other,POINTER(UniqueId)))
        return native.v8___UniqueId____lt__(self._cobject, other._cobject,)

    def assign(self, arg1,):
        '''
        @returns: v8.UniqueId
        '''
        assert(isinstance(arg1,POINTER(UniqueId)))
        return (native.v8___UniqueId__assign(self._cobject, arg1._cobject,))
        

class WeakCallbackInfo_void_( _FwdDecl_WeakCallbackInfo_void_ ):

    @staticmethod
    def new1(self, isolate,parameter,internal_fields,callback,):
        """
        Signature: 
           isolate:v8.Isolate            
        
           parameter:c_void_p            
        
           internal_fields:POINTER(c_void_p)            
        
           callback:POINTER(Callback)            
        """
        assert(isinstance(isolate,WeakCallbackInfo_void_))
        assert(isinstance(parameter,WeakCallbackInfo_void_))
        assert(isinstance(internal_fields,WeakCallbackInfo_void_))
        assert(isinstance(callback,WeakCallbackInfo_void_))
        obj =  WeakCallbackInfo_void_\
            ( native.WeakCallbackInfo_void__new(isolate, parameter, internal_fields, callback, ))
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.WeakCallbackInfo_void_)            
        """
        assert(isinstance(arg1,WeakCallbackInfo_void_))
        obj =  WeakCallbackInfo_void_\
            ( native.WeakCallbackInfo_void__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Isolate            
        
           args[1]:c_void_p            
        
           args[2]:POINTER(c_void_p)            
        
           args[3]:POINTER(Callback)            
        
        Signature[2]: 
           args[0]:POINTER(v8.WeakCallbackInfo_void_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.WeakCallbackInfo_void__copy(args[0])
        else:
            
            try:
                self._cobject = native.WeakCallbackInfo_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.WeakCallbackInfo_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return WeakCallbackInfo_void_(native.WeakCallbackInfo_void__copy( self._cobject ))

    def __del__(self):
        native.WeakCallbackInfo_void__delete(self._cobject)
        self._cobjet = None
            
    

    def GetIsolate(self, ):
        '''
        @returns: v8.Isolate
        '''
        return native.v8___WeakCallbackInfo_void___GetIsolate(self._cobject, )

    def GetParameter(self, ):
        '''
        @returns: c_void_p
        '''
        return native.v8___WeakCallbackInfo_void___GetParameter(self._cobject, )

    def GetInternalField(self, index,):
        '''
        @returns: c_void_p
        '''
        assert(isinstance(index,c_int))
        return native.v8___WeakCallbackInfo_void___GetInternalField(self._cobject, index,)

    def GetInternalField1(self, ):
        '''
        @returns: c_void_p
        '''
        return native.v8___WeakCallbackInfo_void___GetInternalField1(self._cobject, )

    def GetInternalField2(self, ):
        '''
        @returns: c_void_p
        '''
        return native.v8___WeakCallbackInfo_void___GetInternalField2(self._cobject, )

    def IsFirstPass(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___WeakCallbackInfo_void___IsFirstPass(self._cobject, )

    def SetSecondPassCallback(self, callback,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,Callback))
        return native.v8___WeakCallbackInfo_void___SetSecondPassCallback(self._cobject, callback,)

    def assign(self, arg1,):
        '''
        @returns: v8.WeakCallbackInfo_void_
        '''
        assert(isinstance(arg1,POINTER(WeakCallbackInfo_void_)))
        return (native.v8___WeakCallbackInfo_void___assign(self._cobject, arg1._cobject,))
        

class WeakCallbackData_v8__Value__void_( _FwdDecl_WeakCallbackData_v8__Value__void_ ):

    @staticmethod
    def new1(self, isolate,parameter,handle,):
        """
        Signature: 
           isolate:v8.Isolate            
        
           parameter:c_void_p            
        
           handle:v8.Local_v8__Value_            
        """
        assert(isinstance(isolate,WeakCallbackData_v8__Value__void_))
        assert(isinstance(parameter,WeakCallbackData_v8__Value__void_))
        assert(isinstance(handle,WeakCallbackData_v8__Value__void_))
        obj =  WeakCallbackData_v8__Value__void_\
            ( native.WeakCallbackData_v8__Value__void__new(isolate, parameter, handle._cobject, ))
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.WeakCallbackData_v8__Value__void_)            
        """
        assert(isinstance(arg1,WeakCallbackData_v8__Value__void_))
        obj =  WeakCallbackData_v8__Value__void_\
            ( native.WeakCallbackData_v8__Value__void__new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Isolate            
        
           args[1]:c_void_p            
        
           args[2]:v8.Local_v8__Value_            
        
        Signature[2]: 
           args[0]:POINTER(v8.WeakCallbackData_v8__Value__void_)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.WeakCallbackData_v8__Value__void__copy(args[0])
        else:
            
            try:
                self._cobject = native.WeakCallbackData_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.WeakCallbackData_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return WeakCallbackData_v8__Value__void_(native.WeakCallbackData_v8__Value__void__copy( self._cobject ))

    def __del__(self):
        native.WeakCallbackData_v8__Value__void__delete(self._cobject)
        self._cobjet = None
            
    

    def GetIsolate(self, ):
        '''
        @returns: v8.Isolate
        '''
        return native.v8___WeakCallbackData_v8__Value__void___GetIsolate(self._cobject, )

    def GetParameter(self, ):
        '''
        @returns: c_void_p
        '''
        return native.v8___WeakCallbackData_v8__Value__void___GetParameter(self._cobject, )

    def GetValue(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___WeakCallbackData_v8__Value__void___GetValue(self._cobject, ))

    def assign(self, arg1,):
        '''
        @returns: v8.WeakCallbackData_v8__Value__void_
        '''
        assert(isinstance(arg1,POINTER(WeakCallbackData_v8__Value__void_)))
        return (native.v8___WeakCallbackData_v8__Value__void___assign(self._cobject, arg1._cobject,))        

class HandleScope( _FwdDecl_HandleScope ):

    @staticmethod
    def new1(self, isolate,):
        """
        Signature: 
           isolate:v8.Isolate            
        """
        assert(isinstance(isolate,HandleScope))
        obj =  HandleScope\
            ( native.HandleScope_new(isolate, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Isolate            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.HandleScope_copy(args[0])
        else:
            
            try:
                self._cobject = native.HandleScope_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return HandleScope(native.HandleScope_copy( self._cobject ))

    def __del__(self):
        native.HandleScope_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def NumberOfHandles(isolate,):
        '''
        @returns: c_int
        '''
        assert(isinstance(isolate,Isolate))
        return native.v8___HandleScope__NumberOfHandles(isolate,)

    def GetIsolate(self, ):
        '''
        @returns: v8.Isolate
        '''
        return native.v8___HandleScope__GetIsolate(self._cobject, )

    def Initialize(self, isolate,):
        '''
        @returns: void
        '''
        assert(isinstance(isolate,Isolate))
        return native.v8___HandleScope__Initialize(self._cobject, isolate,)

    @staticmethod
    def CreateHandle(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return HandleScope.CreateHandle_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return HandleScope.CreateHandle_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def CreateHandle_4(isolate,value,):
        '''
        @returns: POINTER(v8.internal.Object)
        '''
        assert(isinstance(isolate,v8.internal.Isolate))
        assert(isinstance(value,v8.internal.Object))
        return native.v8___HandleScope__CreateHandle(isolate,value,)



    @staticmethod
    def CreateHandle_5(heap_object,value,):
        '''
        @returns: POINTER(v8.internal.Object)
        '''
        assert(isinstance(heap_object,v8.internal.HeapObject))
        assert(isinstance(value,v8.internal.Object))
        return native.v8___HandleScope__CreateHandle(heap_object,value,)



    def assign(self, arg1,):
        '''
        @returns: void
        '''
        assert(isinstance(arg1,POINTER(HandleScope)))
        return native.v8___HandleScope__assign(self._cobject, arg1._cobject,)
    
  
    
    
          

class SealHandleScope( _FwdDecl_SealHandleScope ):

    @staticmethod
    def new1(self, isolate,):
        """
        Signature: 
           isolate:v8.Isolate            
        """
        assert(isinstance(isolate,SealHandleScope))
        obj =  SealHandleScope\
            ( native.SealHandleScope_new(isolate, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Isolate            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.SealHandleScope_copy(args[0])
        else:
            
            try:
                self._cobject = native.SealHandleScope_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return SealHandleScope(native.SealHandleScope_copy( self._cobject ))

    def __del__(self):
        native.SealHandleScope_delete(self._cobject)
        self._cobjet = None
            
    

    def assign(self, arg1,):
        '''
        @returns: void
        '''
        assert(isinstance(arg1,POINTER(SealHandleScope)))
        return native.v8___SealHandleScope__assign(self._cobject, arg1._cobject,)
    
  
    
    
          

class ScriptOriginOptions( _FwdDecl_ScriptOriginOptions ):

    @staticmethod
    def new1(self, is_embedder_debug_script,is_shared_cross_origin,is_opaque,):
        """
        Signature: 
           is_embedder_debug_script:c_char            
        
           is_shared_cross_origin:c_char            
        
           is_opaque:c_char            
        """
        assert(isinstance(is_embedder_debug_script,ScriptOriginOptions))
        assert(isinstance(is_shared_cross_origin,ScriptOriginOptions))
        assert(isinstance(is_opaque,ScriptOriginOptions))
        obj =  ScriptOriginOptions\
            ( native.ScriptOriginOptions_new(is_embedder_debug_script, is_shared_cross_origin, is_opaque, ))
        return obj
        

    @staticmethod
    def new2(self, flags,):
        """
        Signature: 
           flags:c_int            
        """
        assert(isinstance(flags,ScriptOriginOptions))
        obj =  ScriptOriginOptions\
            ( native.ScriptOriginOptions_new(flags, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.ScriptOriginOptions)            
        """
        assert(isinstance(arg1,ScriptOriginOptions))
        obj =  ScriptOriginOptions\
            ( native.ScriptOriginOptions_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:c_char            
        
           args[1]:c_char            
        
           args[2]:c_char            
        
        Signature[2]: 
           args[0]:c_int            
        
        Signature[3]: 
           args[0]:POINTER(v8.ScriptOriginOptions)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ScriptOriginOptions_copy(args[0])
        else:
            
            try:
                self._cobject = native.ScriptOriginOptions_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.ScriptOriginOptions_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.ScriptOriginOptions_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ScriptOriginOptions(native.ScriptOriginOptions_copy( self._cobject ))

    def __del__(self):
        native.ScriptOriginOptions_delete(self._cobject)
        self._cobjet = None
            
    

    def IsEmbedderDebugScript(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___ScriptOriginOptions__IsEmbedderDebugScript(self._cobject, )

    def IsSharedCrossOrigin(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___ScriptOriginOptions__IsSharedCrossOrigin(self._cobject, )

    def IsOpaque(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___ScriptOriginOptions__IsOpaque(self._cobject, )

    def Flags(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___ScriptOriginOptions__Flags(self._cobject, )        

class ScriptOrigin( _FwdDecl_ScriptOrigin ):

    @staticmethod
    def new1(self, resource_name,resource_line_offset,resource_column_offset,resource_is_shared_cross_origin,script_id,resource_is_embedder_debug_script,source_map_url,resource_is_opaque,):
        """
        Signature: 
           resource_name:v8.Local_v8__Value_            
        
           resource_line_offset:v8.Local_v8__Integer_            
        
           resource_column_offset:v8.Local_v8__Integer_            
        
           resource_is_shared_cross_origin:v8.Local_v8__Boolean_            
        
           script_id:v8.Local_v8__Integer_            
        
           resource_is_embedder_debug_script:v8.Local_v8__Boolean_            
        
           source_map_url:v8.Local_v8__Value_            
        
           resource_is_opaque:v8.Local_v8__Boolean_            
        """
        assert(isinstance(resource_name,ScriptOrigin))
        assert(isinstance(resource_line_offset,ScriptOrigin))
        assert(isinstance(resource_column_offset,ScriptOrigin))
        assert(isinstance(resource_is_shared_cross_origin,ScriptOrigin))
        assert(isinstance(script_id,ScriptOrigin))
        assert(isinstance(resource_is_embedder_debug_script,ScriptOrigin))
        assert(isinstance(source_map_url,ScriptOrigin))
        assert(isinstance(resource_is_opaque,ScriptOrigin))
        obj =  ScriptOrigin\
            ( native.ScriptOrigin_new(resource_name._cobject, resource_line_offset._cobject, resource_column_offset._cobject, resource_is_shared_cross_origin._cobject, script_id._cobject, resource_is_embedder_debug_script._cobject, source_map_url._cobject, resource_is_opaque._cobject, ))
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.ScriptOrigin)            
        """
        assert(isinstance(arg1,ScriptOrigin))
        obj =  ScriptOrigin\
            ( native.ScriptOrigin_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Local_v8__Value_            
        
           args[1]:v8.Local_v8__Integer_            
        
           args[2]:v8.Local_v8__Integer_            
        
           args[3]:v8.Local_v8__Boolean_            
        
           args[4]:v8.Local_v8__Integer_            
        
           args[5]:v8.Local_v8__Boolean_            
        
           args[6]:v8.Local_v8__Value_            
        
           args[7]:v8.Local_v8__Boolean_            
        
        Signature[2]: 
           args[0]:POINTER(v8.ScriptOrigin)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ScriptOrigin_copy(args[0])
        else:
            
            try:
                self._cobject = native.ScriptOrigin_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.ScriptOrigin_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ScriptOrigin(native.ScriptOrigin_copy( self._cobject ))

    def __del__(self):
        native.ScriptOrigin_delete(self._cobject)
        self._cobjet = None
            
    

    def ResourceName(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___ScriptOrigin__ResourceName(self._cobject, ))

    def ResourceLineOffset(self, ):
        '''
        @returns: v8.Local_v8__Integer_
        '''
        return Local_v8__Integer_(native.v8___ScriptOrigin__ResourceLineOffset(self._cobject, ))

    def ResourceColumnOffset(self, ):
        '''
        @returns: v8.Local_v8__Integer_
        '''
        return Local_v8__Integer_(native.v8___ScriptOrigin__ResourceColumnOffset(self._cobject, ))

    def ScriptID(self, ):
        '''
        @returns: v8.Local_v8__Integer_
        '''
        return Local_v8__Integer_(native.v8___ScriptOrigin__ScriptID(self._cobject, ))

    def SourceMapUrl(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___ScriptOrigin__SourceMapUrl(self._cobject, ))

    def Options(self, ):
        '''
        @returns: v8.ScriptOriginOptions
        '''
        return ScriptOriginOptions(native.v8___ScriptOrigin__Options(self._cobject, ))        

class UnboundScript( _FwdDecl_UnboundScript ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  UnboundScript\
            ( native.UnboundScript_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.UnboundScript)            
        """
        assert(isinstance(arg1,UnboundScript))
        obj =  UnboundScript\
            ( native.UnboundScript_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.UnboundScript)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.UnboundScript_copy(args[0])
        else:
            
            try:
                self._cobject = native.UnboundScript_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.UnboundScript_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return UnboundScript(native.UnboundScript_copy( self._cobject ))

    def __del__(self):
        native.UnboundScript_delete(self._cobject)
        self._cobjet = None
            
    

    def BindToCurrentContext(self, ):
        '''
        @returns: v8.Local_v8__Script_
        '''
        return Local_v8__Script_(native.v8___UnboundScript__BindToCurrentContext(self._cobject, ))

    def GetId(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___UnboundScript__GetId(self._cobject, )

    def GetScriptName(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___UnboundScript__GetScriptName(self._cobject, ))

    def GetSourceURL(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___UnboundScript__GetSourceURL(self._cobject, ))

    def GetSourceMappingURL(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___UnboundScript__GetSourceMappingURL(self._cobject, ))

    def GetLineNumber(self, code_pos,):
        '''
        @returns: c_int
        '''
        assert(isinstance(code_pos,c_int))
        return native.v8___UnboundScript__GetLineNumber(self._cobject, code_pos,)

    def assign(self, arg1,):
        '''
        @returns: v8.UnboundScript
        '''
        assert(isinstance(arg1,POINTER(UnboundScript)))
        return (native.v8___UnboundScript__assign(self._cobject, arg1._cobject,))


class CachedData( _FwdDecl_CachedData):
    _fields_ = [ ("data",POINTER(uint8_t)),
    ("length",c_int),
    ("rejected",c_char),
    ("buffer_policy",c_uint),
    ]




        

class Source( _FwdDecl_Source ):

    @staticmethod
    def new1(self, source_string,origin,cached_data,):
        """
        Signature: 
           source_string:v8.Local_v8__String_            
        
           origin:POINTER(v8.ScriptOrigin)            
        
           cached_data:v8.CachedData            
        """
        assert(isinstance(source_string,Source))
        assert(isinstance(origin,Source))
        assert(isinstance(cached_data,Source))
        obj =  Source\
            ( native.Source_new(source_string._cobject, origin._cobject, cached_data, ))
        return obj
        

    @staticmethod
    def new2(self, source_string,cached_data,):
        """
        Signature: 
           source_string:v8.Local_v8__String_            
        
           cached_data:v8.CachedData            
        """
        assert(isinstance(source_string,Source))
        assert(isinstance(cached_data,Source))
        obj =  Source\
            ( native.Source_new(source_string._cobject, cached_data, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Local_v8__String_            
        
           args[1]:POINTER(v8.ScriptOrigin)            
        
           args[2]:v8.CachedData            
        
        Signature[2]: 
           args[0]:v8.Local_v8__String_            
        
           args[1]:v8.CachedData            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Source_copy(args[0])
        else:
            
            try:
                self._cobject = native.Source_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Source_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Source(native.Source_copy( self._cobject ))

    def __del__(self):
        native.Source_delete(self._cobject)
        self._cobjet = None
            
    

    def GetCachedData(self, ):
        '''
        @returns: POINTER(v8.CachedData)
        '''
        return native.v8___Source__GetCachedData(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: v8.Source
        '''
        assert(isinstance(arg1,POINTER(Source)))
        return (native.v8___Source__assign(self._cobject, arg1._cobject,))        

class ExternalSourceStream( _FwdDecl_ExternalSourceStream ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  ExternalSourceStream\
            ( native.ExternalSourceStream_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.ExternalSourceStream)            
        """
        assert(isinstance(arg1,ExternalSourceStream))
        obj =  ExternalSourceStream\
            ( native.ExternalSourceStream_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.ExternalSourceStream)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ExternalSourceStream_copy(args[0])
        else:
            
            try:
                self._cobject = native.ExternalSourceStream_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.ExternalSourceStream_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ExternalSourceStream(native.ExternalSourceStream_copy( self._cobject ))

    def __del__(self):
        native.ExternalSourceStream_delete(self._cobject)
        self._cobjet = None
            
    

    def GetMoreData(self, src,):
        '''
        @returns: size_t
        '''
        assert(isinstance(src,POINTER(POINTER(uint8_t))))
        return native.v8___ExternalSourceStream__GetMoreData(self._cobject, src,)


    def SetBookmark(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___ExternalSourceStream__SetBookmark(self._cobject, )

    def ResetToBookmark(self, ):
        '''
        @returns: void
        '''
        return native.v8___ExternalSourceStream__ResetToBookmark(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: v8.ExternalSourceStream
        '''
        assert(isinstance(arg1,POINTER(ExternalSourceStream)))
        return (native.v8___ExternalSourceStream__assign(self._cobject, arg1._cobject,))        

class StreamedSource( _FwdDecl_StreamedSource ):

    @staticmethod
    def new1(self, source_stream,encoding,):
        """
        Signature: 
           source_stream:v8.ExternalSourceStream            
        
           encoding:c_uint            
        """
        assert(isinstance(source_stream,StreamedSource))
        assert(isinstance(encoding,StreamedSource))
        obj =  StreamedSource\
            ( native.StreamedSource_new(source_stream, encoding._cobject
    
    
    
  , ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.ExternalSourceStream            
        
           args[1]:c_uint            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.StreamedSource_copy(args[0])
        else:
            
            try:
                self._cobject = native.StreamedSource_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return StreamedSource(native.StreamedSource_copy( self._cobject ))

    def __del__(self):
        native.StreamedSource_delete(self._cobject)
        self._cobjet = None
            
    

    def GetCachedData(self, ):
        '''
        @returns: POINTER(v8.CachedData)
        '''
        return native.v8___StreamedSource__GetCachedData(self._cobject, )

    def impl(self, ):
        '''
        @returns: v8.internal.StreamedSource
        '''
        return native.v8___StreamedSource__impl(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: v8.StreamedSource
        '''
        assert(isinstance(arg1,POINTER(StreamedSource)))
        return (native.v8___StreamedSource__assign(self._cobject, arg1._cobject,))        

class ScriptStreamingTask( _FwdDecl_ScriptStreamingTask ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  ScriptStreamingTask\
            ( native.ScriptStreamingTask_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.ScriptStreamingTask)            
        """
        assert(isinstance(arg1,ScriptStreamingTask))
        obj =  ScriptStreamingTask\
            ( native.ScriptStreamingTask_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.ScriptStreamingTask)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ScriptStreamingTask_copy(args[0])
        else:
            
            try:
                self._cobject = native.ScriptStreamingTask_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.ScriptStreamingTask_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ScriptStreamingTask(native.ScriptStreamingTask_copy( self._cobject ))

    def __del__(self):
        native.ScriptStreamingTask_delete(self._cobject)
        self._cobjet = None
            
    

    def Run(self, ):
        '''
        @returns: void
        '''
        return native.v8___ScriptStreamingTask__Run(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: v8.ScriptStreamingTask
        '''
        assert(isinstance(arg1,POINTER(ScriptStreamingTask)))
        return (native.v8___ScriptStreamingTask__assign(self._cobject, arg1._cobject,))        

class ScriptCompiler( _FwdDecl_ScriptCompiler ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  ScriptCompiler\
            ( native.ScriptCompiler_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.ScriptCompiler)            
        """
        assert(isinstance(arg1,ScriptCompiler))
        obj =  ScriptCompiler\
            ( native.ScriptCompiler_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.ScriptCompiler)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ScriptCompiler_copy(args[0])
        else:
            
            try:
                self._cobject = native.ScriptCompiler_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.ScriptCompiler_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ScriptCompiler(native.ScriptCompiler_copy( self._cobject ))

    def __del__(self):
        native.ScriptCompiler_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def CompileUnbound(isolate,source,options,):
        '''
        @returns: v8.Local_v8__UnboundScript_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(source,Source))
        assert(isinstance(options,c_uint))
        return Local_v8__UnboundScript_(native.v8___ScriptCompiler__CompileUnbound(isolate,source,options._cobject
    
    
    
    
    
    
  ,))
    
    
    
    
    
    
  

    @staticmethod
    def CompileUnboundScript(isolate,source,options,):
        '''
        @returns: v8.MaybeLocal_v8__UnboundScript_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(source,Source))
        assert(isinstance(options,c_uint))
        return MaybeLocal_v8__UnboundScript_(native.v8___ScriptCompiler__CompileUnboundScript(isolate,source,options._cobject
    
    
    
    
    
    
  ,))
    
    
    
    
    
    
  

    @staticmethod
    def Compile(*args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return ScriptCompiler.Compile_3(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return ScriptCompiler.Compile_4(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 4:
            try:
                return ScriptCompiler.Compile_6(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 4:
            try:
                return ScriptCompiler.Compile_7(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def Compile_3(isolate,source,options,):
        '''
        @returns: v8.Local_v8__Script_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(source,Source))
        assert(isinstance(options,c_uint))
        return Local_v8__Script_(native.v8___ScriptCompiler__Compile(isolate,source,options._cobject
    
    
    
    
    
    
  ,))
    
    
    
    
    
    
  

    @staticmethod
    def Compile_4(context,source,options,):
        '''
        @returns: v8.MaybeLocal_v8__Script_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(source,Source))
        assert(isinstance(options,c_uint))
        return MaybeLocal_v8__Script_(native.v8___ScriptCompiler__Compile(context._cobject,source,options._cobject
    
    
    
    
    
    
  ,))
    
    
    
    
    
    
  

    @staticmethod
    def StartStreamingScript(isolate,source,options,):
        '''
        @returns: v8.ScriptStreamingTask
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(source,StreamedSource))
        assert(isinstance(options,c_uint))
        return native.v8___ScriptCompiler__StartStreamingScript(isolate,source,options._cobject
    
    
    
    
    
    
  ,)
    
    
    
    
    
    
  

    @staticmethod
    def Compile_6(isolate,source,full_source_string,origin,):
        '''
        @returns: v8.Local_v8__Script_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(source,StreamedSource))
        assert(isinstance(full_source_string,Local_v8__String_))
        assert(isinstance(origin,POINTER(ScriptOrigin)))
        return Local_v8__Script_(native.v8___ScriptCompiler__Compile(isolate,source,full_source_string._cobject,origin._cobject,))

    @staticmethod
    def Compile_7(context,source,full_source_string,origin,):
        '''
        @returns: v8.MaybeLocal_v8__Script_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(source,StreamedSource))
        assert(isinstance(full_source_string,Local_v8__String_))
        assert(isinstance(origin,POINTER(ScriptOrigin)))
        return MaybeLocal_v8__Script_(native.v8___ScriptCompiler__Compile(context._cobject,source,full_source_string._cobject,origin._cobject,))

    @staticmethod
    def CachedDataVersionTag():
        '''
        @returns: uint32_t
        '''
        return native.v8___ScriptCompiler__CachedDataVersionTag()

    @staticmethod
    def CompileModule(*args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return ScriptCompiler.CompileModule_9(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return ScriptCompiler.CompileModule_10(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def CompileModule_9(isolate,source,options,):
        '''
        @returns: v8.Local_v8__Script_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(source,Source))
        assert(isinstance(options,c_uint))
        return Local_v8__Script_(native.v8___ScriptCompiler__CompileModule(isolate,source,options._cobject
    
    
    
    
    
    
  ,))
    
    
    
    
    
    
  

    @staticmethod
    def CompileModule_10(context,source,options,):
        '''
        @returns: v8.MaybeLocal_v8__Script_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(source,Source))
        assert(isinstance(options,c_uint))
        return MaybeLocal_v8__Script_(native.v8___ScriptCompiler__CompileModule(context._cobject,source,options._cobject
    
    
    
    
    
    
  ,))
    
    
    
    
    
    
  

    @staticmethod
    def CompileFunctionInContext(*args):
        count = 0
        if count==0 and len(*args) == 7:
            try:
                return ScriptCompiler.CompileFunctionInContext_11(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 6:
            try:
                return ScriptCompiler.CompileFunctionInContext_12(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def CompileFunctionInContext_11(isolate,source,context,arguments_count,arguments,context_extension_count,context_extensions,):
        '''
        @returns: v8.Local_v8__Function_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(source,Source))
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(arguments_count,size_t))
        assert(isinstance(arguments,Local_v8__String_))
        assert(isinstance(context_extension_count,size_t))
        assert(isinstance(context_extensions,Local_v8__Object_))
        return Local_v8__Function_(native.v8___ScriptCompiler__CompileFunctionInContext(isolate,source,context._cobject,arguments_count,arguments,context_extension_count,context_extensions,))



    @staticmethod
    def CompileFunctionInContext_12(context,source,arguments_count,arguments,context_extension_count,context_extensions,):
        '''
        @returns: v8.MaybeLocal_v8__Function_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(source,Source))
        assert(isinstance(arguments_count,size_t))
        assert(isinstance(arguments,Local_v8__String_))
        assert(isinstance(context_extension_count,size_t))
        assert(isinstance(context_extensions,Local_v8__Object_))
        return MaybeLocal_v8__Function_(native.v8___ScriptCompiler__CompileFunctionInContext(context._cobject,source,arguments_count,arguments,context_extension_count,context_extensions,))



    @staticmethod
    def CompileUnboundInternal(isolate,source,options,is_module,):
        '''
        @returns: v8.MaybeLocal_v8__UnboundScript_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(source,Source))
        assert(isinstance(options,c_uint))
        assert(isinstance(is_module,c_char))
        return MaybeLocal_v8__UnboundScript_(native.v8___ScriptCompiler__CompileUnboundInternal(isolate,source,options._cobject
    
    
    
    
    
    
  ,is_module,))
    
    
    
    
    
    
  

    def assign(self, arg1,):
        '''
        @returns: v8.ScriptCompiler
        '''
        assert(isinstance(arg1,POINTER(ScriptCompiler)))
        return (native.v8___ScriptCompiler__assign(self._cobject, arg1._cobject,))        

class Message( _FwdDecl_Message ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Message\
            ( native.Message_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Message)            
        """
        assert(isinstance(arg1,Message))
        obj =  Message\
            ( native.Message_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.Message)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Message_copy(args[0])
        else:
            
            try:
                self._cobject = native.Message_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Message_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Message(native.Message_copy( self._cobject ))

    def __del__(self):
        native.Message_delete(self._cobject)
        self._cobjet = None
            
    

    def Get(self, ):
        '''
        @returns: v8.Local_v8__String_
        '''
        return Local_v8__String_(native.v8___Message__Get(self._cobject, ))

    def GetSourceLine(self, *args):
        count = 0
        if count==0 and len(*args) == 0:
            try:
                return self.GetSourceLine_2(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.GetSourceLine_3(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def GetSourceLine_2(self, ):
        '''
        @returns: v8.Local_v8__String_
        '''
        return Local_v8__String_(native.v8___Message__GetSourceLine(self._cobject, ))

    def GetSourceLine_3(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__String_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__String_(native.v8___Message__GetSourceLine(self._cobject, context._cobject,))

    def GetScriptOrigin(self, ):
        '''
        @returns: v8.ScriptOrigin
        '''
        return ScriptOrigin(native.v8___Message__GetScriptOrigin(self._cobject, ))

    def GetScriptResourceName(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___Message__GetScriptResourceName(self._cobject, ))

    def GetStackTrace(self, ):
        '''
        @returns: v8.Local_v8__StackTrace_
        '''
        return Local_v8__StackTrace_(native.v8___Message__GetStackTrace(self._cobject, ))

    def GetLineNumber(self, *args):
        count = 0
        if count==0 and len(*args) == 0:
            try:
                return self.GetLineNumber_7(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.GetLineNumber_8(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def GetLineNumber_7(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___Message__GetLineNumber(self._cobject, )

    def GetLineNumber_8(self, context,):
        '''
        @returns: v8.Maybe_int_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return Maybe_int_(native.v8___Message__GetLineNumber(self._cobject, context._cobject,))

    def GetStartPosition(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___Message__GetStartPosition(self._cobject, )

    def GetEndPosition(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___Message__GetEndPosition(self._cobject, )

    def GetStartColumn(self, *args):
        count = 0
        if count==0 and len(*args) == 0:
            try:
                return self.GetStartColumn_11(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.GetStartColumn_12(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def GetStartColumn_11(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___Message__GetStartColumn(self._cobject, )

    def GetStartColumn_12(self, context,):
        '''
        @returns: v8.Maybe_int_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return Maybe_int_(native.v8___Message__GetStartColumn(self._cobject, context._cobject,))

    def GetEndColumn(self, *args):
        count = 0
        if count==0 and len(*args) == 0:
            try:
                return self.GetEndColumn_13(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.GetEndColumn_14(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def GetEndColumn_13(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___Message__GetEndColumn(self._cobject, )

    def GetEndColumn_14(self, context,):
        '''
        @returns: v8.Maybe_int_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return Maybe_int_(native.v8___Message__GetEndColumn(self._cobject, context._cobject,))

    def IsSharedCrossOrigin(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Message__IsSharedCrossOrigin(self._cobject, )

    def IsOpaque(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Message__IsOpaque(self._cobject, )

    @staticmethod
    def PrintCurrentStackTrace(isolate,out,):
        '''
        @returns: void
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(out,POINTER(FILE)))
        return native.v8___Message__PrintCurrentStackTrace(isolate,out,)


    def assign(self, arg1,):
        '''
        @returns: v8.Message
        '''
        assert(isinstance(arg1,POINTER(Message)))
        return (native.v8___Message__assign(self._cobject, arg1._cobject,))

class RegisterState( _FwdDecl_RegisterState):
    _fields_ = [ ("pc",c_void_p),
    ("sp",c_void_p),
    ("fp",c_void_p),
    ]






class SampleInfo( _FwdDecl_SampleInfo):
    _fields_ = [ ("frames_count",size_t),
    ("vm_state",c_uint),
    ]




        

class JSON( _FwdDecl_JSON ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  JSON\
            ( native.JSON_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.JSON)            
        """
        assert(isinstance(arg1,JSON))
        obj =  JSON\
            ( native.JSON_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.JSON)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.JSON_copy(args[0])
        else:
            
            try:
                self._cobject = native.JSON_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.JSON_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return JSON(native.JSON_copy( self._cobject ))

    def __del__(self):
        native.JSON_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def Parse(*args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return JSON.Parse_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return JSON.Parse_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def Parse_1(json_string,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(json_string,Local_v8__String_))
        return Local_v8__Value_(native.v8___JSON__Parse(json_string._cobject,))

    @staticmethod
    def Parse_2(isolate,json_string,):
        '''
        @returns: v8.MaybeLocal_v8__Value_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(json_string,Local_v8__String_))
        return MaybeLocal_v8__Value_(native.v8___JSON__Parse(isolate,json_string._cobject,))

    def assign(self, arg1,):
        '''
        @returns: v8.JSON
        '''
        assert(isinstance(arg1,POINTER(JSON)))
        return (native.v8___JSON__assign(self._cobject, arg1._cobject,))        

class NativeWeakMap( _FwdDecl_NativeWeakMap ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.NativeWeakMap)            
        """
        assert(isinstance(arg1,NativeWeakMap))
        obj =  NativeWeakMap\
            ( native.NativeWeakMap_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.NativeWeakMap)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.NativeWeakMap_copy(args[0])
        else:
            
            try:
                self._cobject = native.NativeWeakMap_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return NativeWeakMap(native.NativeWeakMap_copy( self._cobject ))

    def __del__(self):
        native.NativeWeakMap_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(isolate,):
        '''
        @returns: v8.Local_v8__NativeWeakMap_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__NativeWeakMap_(native.v8___NativeWeakMap__New(isolate,))

    def Set(self, key,value,):
        '''
        @returns: void
        '''
        assert(isinstance(key,Local_v8__Value_))
        assert(isinstance(value,Local_v8__Value_))
        return native.v8___NativeWeakMap__Set(self._cobject, key._cobject,value._cobject,)

    def Get(self, key,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(key,Local_v8__Value_))
        return Local_v8__Value_(native.v8___NativeWeakMap__Get(self._cobject, key._cobject,))

    def Has(self, key,):
        '''
        @returns: c_char
        '''
        assert(isinstance(key,Local_v8__Value_))
        return native.v8___NativeWeakMap__Has(self._cobject, key._cobject,)

    def Delete(self, key,):
        '''
        @returns: c_char
        '''
        assert(isinstance(key,Local_v8__Value_))
        return native.v8___NativeWeakMap__Delete(self._cobject, key._cobject,)

    def assign(self, arg1,):
        '''
        @returns: v8.NativeWeakMap
        '''
        assert(isinstance(arg1,POINTER(NativeWeakMap)))
        return (native.v8___NativeWeakMap__assign(self._cobject, arg1._cobject,))



        

class Map( _FwdDecl_Map ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Map)            
        """
        assert(isinstance(arg1,Map))
        obj =  Map\
            ( native.Map_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Map)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Map_copy(args[0])
        else:
            
            try:
                self._cobject = native.Map_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Map(native.Map_copy( self._cobject ))

    def __del__(self):
        native.Map_delete(self._cobject)
        self._cobjet = None
            
    

    def Size(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___Map__Size(self._cobject, )

    def AsArray(self, ):
        '''
        @returns: v8.Local_v8__Array_
        '''
        return Local_v8__Array_(native.v8___Map__AsArray(self._cobject, ))

    @staticmethod
    def New(isolate,):
        '''
        @returns: v8.Local_v8__Map_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__Map_(native.v8___Map__New(isolate,))

    @staticmethod
    def FromArray(context,array,):
        '''
        @returns: v8.MaybeLocal_v8__Map_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(array,Local_v8__Array_))
        return MaybeLocal_v8__Map_(native.v8___Map__FromArray(context._cobject,array._cobject,))

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Map
        '''
        assert(isinstance(obj,Value))
        return native.v8___Map__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Map__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Map
        '''
        assert(isinstance(arg1,POINTER(Map)))
        return (native.v8___Map__assign(self._cobject, arg1._cobject,))        

class Set( _FwdDecl_Set ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Set)            
        """
        assert(isinstance(arg1,Set))
        obj =  Set\
            ( native.Set_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Set)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Set_copy(args[0])
        else:
            
            try:
                self._cobject = native.Set_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Set(native.Set_copy( self._cobject ))

    def __del__(self):
        native.Set_delete(self._cobject)
        self._cobjet = None
            
    

    def Size(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___Set__Size(self._cobject, )

    def AsArray(self, ):
        '''
        @returns: v8.Local_v8__Array_
        '''
        return Local_v8__Array_(native.v8___Set__AsArray(self._cobject, ))

    @staticmethod
    def New(isolate,):
        '''
        @returns: v8.Local_v8__Set_
        '''
        assert(isinstance(isolate,Isolate))
        return Local_v8__Set_(native.v8___Set__New(isolate,))

    @staticmethod
    def FromArray(context,array,):
        '''
        @returns: v8.MaybeLocal_v8__Set_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(array,Local_v8__Array_))
        return MaybeLocal_v8__Set_(native.v8___Set__FromArray(context._cobject,array._cobject,))

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Set
        '''
        assert(isinstance(obj,Value))
        return native.v8___Set__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Set__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Set
        '''
        assert(isinstance(arg1,POINTER(Set)))
        return (native.v8___Set__assign(self._cobject, arg1._cobject,))
        

class Allocator( _FwdDecl_Allocator ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Allocator\
            ( native.Allocator_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Allocator)            
        """
        assert(isinstance(arg1,Allocator))
        obj =  Allocator\
            ( native.Allocator_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.Allocator)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Allocator_copy(args[0])
        else:
            
            try:
                self._cobject = native.Allocator_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Allocator_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Allocator(native.Allocator_copy( self._cobject ))

    def __del__(self):
        native.Allocator_delete(self._cobject)
        self._cobjet = None
            
    

    def Allocate(self, length,):
        '''
        @returns: c_void_p
        '''
        assert(isinstance(length,size_t))
        return native.v8___Allocator__Allocate(self._cobject, length,)


    def AllocateUninitialized(self, length,):
        '''
        @returns: c_void_p
        '''
        assert(isinstance(length,size_t))
        return native.v8___Allocator__AllocateUninitialized(self._cobject, length,)


    def Free(self, data,length,):
        '''
        @returns: void
        '''
        assert(isinstance(data,c_void_p))
        assert(isinstance(length,size_t))
        return native.v8___Allocator__Free(self._cobject, data,length,)


    def assign(self, arg1,):
        '''
        @returns: v8.Allocator
        '''
        assert(isinstance(arg1,POINTER(Allocator)))
        return (native.v8___Allocator__assign(self._cobject, arg1._cobject,))
        

class Contents( _FwdDecl_Contents ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Contents\
            ( native.Contents_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Contents)            
        """
        assert(isinstance(arg1,Contents))
        obj =  Contents\
            ( native.Contents_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.Contents)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Contents_copy(args[0])
        else:
            
            try:
                self._cobject = native.Contents_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Contents_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Contents(native.Contents_copy( self._cobject ))

    def __del__(self):
        native.Contents_delete(self._cobject)
        self._cobjet = None
            
    

    def Data(self, ):
        '''
        @returns: c_void_p
        '''
        return native.v8___Contents__Data(self._cobject, )

    def ByteLength(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___Contents__ByteLength(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: v8.Contents
        '''
        assert(isinstance(arg1,POINTER(Contents)))
        return (native.v8___Contents__assign(self._cobject, arg1._cobject,))
        

class ArrayBuffer( _FwdDecl_ArrayBuffer ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.ArrayBuffer)            
        """
        assert(isinstance(arg1,ArrayBuffer))
        obj =  ArrayBuffer\
            ( native.ArrayBuffer_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.ArrayBuffer)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ArrayBuffer_copy(args[0])
        else:
            
            try:
                self._cobject = native.ArrayBuffer_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ArrayBuffer(native.ArrayBuffer_copy( self._cobject ))

    def __del__(self):
        native.ArrayBuffer_delete(self._cobject)
        self._cobjet = None
            
    

    def ByteLength(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___ArrayBuffer__ByteLength(self._cobject, )

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return ArrayBuffer.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 4:
            try:
                return ArrayBuffer.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_2(isolate,byte_length,):
        '''
        @returns: v8.Local_v8__ArrayBuffer_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(byte_length,size_t))
        return Local_v8__ArrayBuffer_(native.v8___ArrayBuffer__New(isolate,byte_length,))


    @staticmethod
    def New_3(isolate,data,byte_length,mode,):
        '''
        @returns: v8.Local_v8__ArrayBuffer_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(data,c_void_p))
        assert(isinstance(byte_length,size_t))
        assert(isinstance(mode,c_uint))
        return Local_v8__ArrayBuffer_(native.v8___ArrayBuffer__New(isolate,data,byte_length,mode._cobject
    
    
  ,))
    
    
  


    def IsExternal(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___ArrayBuffer__IsExternal(self._cobject, )

    def IsNeuterable(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___ArrayBuffer__IsNeuterable(self._cobject, )

    def Neuter(self, ):
        '''
        @returns: void
        '''
        return native.v8___ArrayBuffer__Neuter(self._cobject, )

    def Externalize(self, ):
        '''
        @returns: v8.Contents
        '''
        return Contents(native.v8___ArrayBuffer__Externalize(self._cobject, ))

    def GetContents(self, ):
        '''
        @returns: v8.Contents
        '''
        return Contents(native.v8___ArrayBuffer__GetContents(self._cobject, ))

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.ArrayBuffer
        '''
        assert(isinstance(obj,Value))
        return native.v8___ArrayBuffer__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___ArrayBuffer__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.ArrayBuffer
        '''
        assert(isinstance(arg1,POINTER(ArrayBuffer)))
        return (native.v8___ArrayBuffer__assign(self._cobject, arg1._cobject,))        

class ArrayBufferView( _FwdDecl_ArrayBufferView ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.ArrayBufferView)            
        """
        assert(isinstance(arg1,ArrayBufferView))
        obj =  ArrayBufferView\
            ( native.ArrayBufferView_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.ArrayBufferView)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ArrayBufferView_copy(args[0])
        else:
            
            try:
                self._cobject = native.ArrayBufferView_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ArrayBufferView(native.ArrayBufferView_copy( self._cobject ))

    def __del__(self):
        native.ArrayBufferView_delete(self._cobject)
        self._cobjet = None
            
    

    def Buffer(self, ):
        '''
        @returns: v8.Local_v8__ArrayBuffer_
        '''
        return Local_v8__ArrayBuffer_(native.v8___ArrayBufferView__Buffer(self._cobject, ))

    def ByteOffset(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___ArrayBufferView__ByteOffset(self._cobject, )

    def ByteLength(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___ArrayBufferView__ByteLength(self._cobject, )

    def CopyContents(self, dest,byte_length,):
        '''
        @returns: size_t
        '''
        assert(isinstance(dest,c_void_p))
        assert(isinstance(byte_length,size_t))
        return native.v8___ArrayBufferView__CopyContents(self._cobject, dest,byte_length,)


    def HasBuffer(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___ArrayBufferView__HasBuffer(self._cobject, )

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.ArrayBufferView
        '''
        assert(isinstance(obj,Value))
        return native.v8___ArrayBufferView__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___ArrayBufferView__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.ArrayBufferView
        '''
        assert(isinstance(arg1,POINTER(ArrayBufferView)))
        return (native.v8___ArrayBufferView__assign(self._cobject, arg1._cobject,))        

class TypedArray( _FwdDecl_TypedArray ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.TypedArray)            
        """
        assert(isinstance(arg1,TypedArray))
        obj =  TypedArray\
            ( native.TypedArray_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.TypedArray)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.TypedArray_copy(args[0])
        else:
            
            try:
                self._cobject = native.TypedArray_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return TypedArray(native.TypedArray_copy( self._cobject ))

    def __del__(self):
        native.TypedArray_delete(self._cobject)
        self._cobjet = None
            
    

    def Length(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___TypedArray__Length(self._cobject, )

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.TypedArray
        '''
        assert(isinstance(obj,Value))
        return native.v8___TypedArray__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___TypedArray__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.TypedArray
        '''
        assert(isinstance(arg1,POINTER(TypedArray)))
        return (native.v8___TypedArray__assign(self._cobject, arg1._cobject,))        

class Uint8Array( _FwdDecl_Uint8Array ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Uint8Array)            
        """
        assert(isinstance(arg1,Uint8Array))
        obj =  Uint8Array\
            ( native.Uint8Array_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Uint8Array)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Uint8Array_copy(args[0])
        else:
            
            try:
                self._cobject = native.Uint8Array_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Uint8Array(native.Uint8Array_copy( self._cobject ))

    def __del__(self):
        native.Uint8Array_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return Uint8Array.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return Uint8Array.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Uint8Array_
        '''
        assert(isinstance(array_buffer,Local_v8__ArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Uint8Array_(native.v8___Uint8Array__New(array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def New_2(shared_array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Uint8Array_
        '''
        assert(isinstance(shared_array_buffer,Local_v8__SharedArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Uint8Array_(native.v8___Uint8Array__New(shared_array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Uint8Array
        '''
        assert(isinstance(obj,Value))
        return native.v8___Uint8Array__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Uint8Array__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Uint8Array
        '''
        assert(isinstance(arg1,POINTER(Uint8Array)))
        return (native.v8___Uint8Array__assign(self._cobject, arg1._cobject,))        

class Uint8ClampedArray( _FwdDecl_Uint8ClampedArray ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Uint8ClampedArray)            
        """
        assert(isinstance(arg1,Uint8ClampedArray))
        obj =  Uint8ClampedArray\
            ( native.Uint8ClampedArray_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Uint8ClampedArray)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Uint8ClampedArray_copy(args[0])
        else:
            
            try:
                self._cobject = native.Uint8ClampedArray_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Uint8ClampedArray(native.Uint8ClampedArray_copy( self._cobject ))

    def __del__(self):
        native.Uint8ClampedArray_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return Uint8ClampedArray.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return Uint8ClampedArray.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Uint8ClampedArray_
        '''
        assert(isinstance(array_buffer,Local_v8__ArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Uint8ClampedArray_(native.v8___Uint8ClampedArray__New(array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def New_2(shared_array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Uint8ClampedArray_
        '''
        assert(isinstance(shared_array_buffer,Local_v8__SharedArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Uint8ClampedArray_(native.v8___Uint8ClampedArray__New(shared_array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Uint8ClampedArray
        '''
        assert(isinstance(obj,Value))
        return native.v8___Uint8ClampedArray__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Uint8ClampedArray__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Uint8ClampedArray
        '''
        assert(isinstance(arg1,POINTER(Uint8ClampedArray)))
        return (native.v8___Uint8ClampedArray__assign(self._cobject, arg1._cobject,))        

class Int8Array( _FwdDecl_Int8Array ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Int8Array)            
        """
        assert(isinstance(arg1,Int8Array))
        obj =  Int8Array\
            ( native.Int8Array_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Int8Array)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Int8Array_copy(args[0])
        else:
            
            try:
                self._cobject = native.Int8Array_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Int8Array(native.Int8Array_copy( self._cobject ))

    def __del__(self):
        native.Int8Array_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return Int8Array.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return Int8Array.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Int8Array_
        '''
        assert(isinstance(array_buffer,Local_v8__ArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Int8Array_(native.v8___Int8Array__New(array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def New_2(shared_array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Int8Array_
        '''
        assert(isinstance(shared_array_buffer,Local_v8__SharedArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Int8Array_(native.v8___Int8Array__New(shared_array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Int8Array
        '''
        assert(isinstance(obj,Value))
        return native.v8___Int8Array__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Int8Array__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Int8Array
        '''
        assert(isinstance(arg1,POINTER(Int8Array)))
        return (native.v8___Int8Array__assign(self._cobject, arg1._cobject,))        

class Uint16Array( _FwdDecl_Uint16Array ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Uint16Array)            
        """
        assert(isinstance(arg1,Uint16Array))
        obj =  Uint16Array\
            ( native.Uint16Array_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Uint16Array)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Uint16Array_copy(args[0])
        else:
            
            try:
                self._cobject = native.Uint16Array_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Uint16Array(native.Uint16Array_copy( self._cobject ))

    def __del__(self):
        native.Uint16Array_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return Uint16Array.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return Uint16Array.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Uint16Array_
        '''
        assert(isinstance(array_buffer,Local_v8__ArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Uint16Array_(native.v8___Uint16Array__New(array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def New_2(shared_array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Uint16Array_
        '''
        assert(isinstance(shared_array_buffer,Local_v8__SharedArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Uint16Array_(native.v8___Uint16Array__New(shared_array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Uint16Array
        '''
        assert(isinstance(obj,Value))
        return native.v8___Uint16Array__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Uint16Array__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Uint16Array
        '''
        assert(isinstance(arg1,POINTER(Uint16Array)))
        return (native.v8___Uint16Array__assign(self._cobject, arg1._cobject,))        

class Int16Array( _FwdDecl_Int16Array ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Int16Array)            
        """
        assert(isinstance(arg1,Int16Array))
        obj =  Int16Array\
            ( native.Int16Array_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Int16Array)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Int16Array_copy(args[0])
        else:
            
            try:
                self._cobject = native.Int16Array_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Int16Array(native.Int16Array_copy( self._cobject ))

    def __del__(self):
        native.Int16Array_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return Int16Array.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return Int16Array.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Int16Array_
        '''
        assert(isinstance(array_buffer,Local_v8__ArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Int16Array_(native.v8___Int16Array__New(array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def New_2(shared_array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Int16Array_
        '''
        assert(isinstance(shared_array_buffer,Local_v8__SharedArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Int16Array_(native.v8___Int16Array__New(shared_array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Int16Array
        '''
        assert(isinstance(obj,Value))
        return native.v8___Int16Array__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Int16Array__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Int16Array
        '''
        assert(isinstance(arg1,POINTER(Int16Array)))
        return (native.v8___Int16Array__assign(self._cobject, arg1._cobject,))        

class Uint32Array( _FwdDecl_Uint32Array ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Uint32Array)            
        """
        assert(isinstance(arg1,Uint32Array))
        obj =  Uint32Array\
            ( native.Uint32Array_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Uint32Array)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Uint32Array_copy(args[0])
        else:
            
            try:
                self._cobject = native.Uint32Array_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Uint32Array(native.Uint32Array_copy( self._cobject ))

    def __del__(self):
        native.Uint32Array_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return Uint32Array.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return Uint32Array.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Uint32Array_
        '''
        assert(isinstance(array_buffer,Local_v8__ArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Uint32Array_(native.v8___Uint32Array__New(array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def New_2(shared_array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Uint32Array_
        '''
        assert(isinstance(shared_array_buffer,Local_v8__SharedArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Uint32Array_(native.v8___Uint32Array__New(shared_array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Uint32Array
        '''
        assert(isinstance(obj,Value))
        return native.v8___Uint32Array__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Uint32Array__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Uint32Array
        '''
        assert(isinstance(arg1,POINTER(Uint32Array)))
        return (native.v8___Uint32Array__assign(self._cobject, arg1._cobject,))        

class Int32Array( _FwdDecl_Int32Array ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Int32Array)            
        """
        assert(isinstance(arg1,Int32Array))
        obj =  Int32Array\
            ( native.Int32Array_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Int32Array)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Int32Array_copy(args[0])
        else:
            
            try:
                self._cobject = native.Int32Array_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Int32Array(native.Int32Array_copy( self._cobject ))

    def __del__(self):
        native.Int32Array_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return Int32Array.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return Int32Array.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Int32Array_
        '''
        assert(isinstance(array_buffer,Local_v8__ArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Int32Array_(native.v8___Int32Array__New(array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def New_2(shared_array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Int32Array_
        '''
        assert(isinstance(shared_array_buffer,Local_v8__SharedArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Int32Array_(native.v8___Int32Array__New(shared_array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Int32Array
        '''
        assert(isinstance(obj,Value))
        return native.v8___Int32Array__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Int32Array__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Int32Array
        '''
        assert(isinstance(arg1,POINTER(Int32Array)))
        return (native.v8___Int32Array__assign(self._cobject, arg1._cobject,))        

class Float32Array( _FwdDecl_Float32Array ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Float32Array)            
        """
        assert(isinstance(arg1,Float32Array))
        obj =  Float32Array\
            ( native.Float32Array_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Float32Array)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Float32Array_copy(args[0])
        else:
            
            try:
                self._cobject = native.Float32Array_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Float32Array(native.Float32Array_copy( self._cobject ))

    def __del__(self):
        native.Float32Array_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return Float32Array.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return Float32Array.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Float32Array_
        '''
        assert(isinstance(array_buffer,Local_v8__ArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Float32Array_(native.v8___Float32Array__New(array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def New_2(shared_array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Float32Array_
        '''
        assert(isinstance(shared_array_buffer,Local_v8__SharedArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Float32Array_(native.v8___Float32Array__New(shared_array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Float32Array
        '''
        assert(isinstance(obj,Value))
        return native.v8___Float32Array__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Float32Array__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Float32Array
        '''
        assert(isinstance(arg1,POINTER(Float32Array)))
        return (native.v8___Float32Array__assign(self._cobject, arg1._cobject,))        

class Float64Array( _FwdDecl_Float64Array ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Float64Array)            
        """
        assert(isinstance(arg1,Float64Array))
        obj =  Float64Array\
            ( native.Float64Array_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Float64Array)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Float64Array_copy(args[0])
        else:
            
            try:
                self._cobject = native.Float64Array_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Float64Array(native.Float64Array_copy( self._cobject ))

    def __del__(self):
        native.Float64Array_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return Float64Array.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return Float64Array.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Float64Array_
        '''
        assert(isinstance(array_buffer,Local_v8__ArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Float64Array_(native.v8___Float64Array__New(array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def New_2(shared_array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__Float64Array_
        '''
        assert(isinstance(shared_array_buffer,Local_v8__SharedArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__Float64Array_(native.v8___Float64Array__New(shared_array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.Float64Array
        '''
        assert(isinstance(obj,Value))
        return native.v8___Float64Array__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___Float64Array__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.Float64Array
        '''
        assert(isinstance(arg1,POINTER(Float64Array)))
        return (native.v8___Float64Array__assign(self._cobject, arg1._cobject,))        

class DataView( _FwdDecl_DataView ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.DataView)            
        """
        assert(isinstance(arg1,DataView))
        obj =  DataView\
            ( native.DataView_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.DataView)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.DataView_copy(args[0])
        else:
            
            try:
                self._cobject = native.DataView_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return DataView(native.DataView_copy( self._cobject ))

    def __del__(self):
        native.DataView_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return DataView.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return DataView.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__DataView_
        '''
        assert(isinstance(array_buffer,Local_v8__ArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__DataView_(native.v8___DataView__New(array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def New_2(shared_array_buffer,byte_offset,length,):
        '''
        @returns: v8.Local_v8__DataView_
        '''
        assert(isinstance(shared_array_buffer,Local_v8__SharedArrayBuffer_))
        assert(isinstance(byte_offset,size_t))
        assert(isinstance(length,size_t))
        return Local_v8__DataView_(native.v8___DataView__New(shared_array_buffer._cobject,byte_offset,length,))



    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.DataView
        '''
        assert(isinstance(obj,Value))
        return native.v8___DataView__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___DataView__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.DataView
        '''
        assert(isinstance(arg1,POINTER(DataView)))
        return (native.v8___DataView__assign(self._cobject, arg1._cobject,))        

class RegExp( _FwdDecl_RegExp ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.RegExp)            
        """
        assert(isinstance(arg1,RegExp))
        obj =  RegExp\
            ( native.RegExp_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.RegExp)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.RegExp_copy(args[0])
        else:
            
            try:
                self._cobject = native.RegExp_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return RegExp(native.RegExp_copy( self._cobject ))

    def __del__(self):
        native.RegExp_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return RegExp.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return RegExp.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(pattern,flags,):
        '''
        @returns: v8.Local_v8__RegExp_
        '''
        assert(isinstance(pattern,Local_v8__String_))
        assert(isinstance(flags,c_uint))
        return Local_v8__RegExp_(native.v8___RegExp__New(pattern._cobject,flags._cobject
    
    
    
    
  ,))
    
    
    
    
  


    @staticmethod
    def New_2(context,pattern,flags,):
        '''
        @returns: v8.MaybeLocal_v8__RegExp_
        '''
        assert(isinstance(context,Local_v8__Context_))
        assert(isinstance(pattern,Local_v8__String_))
        assert(isinstance(flags,c_uint))
        return MaybeLocal_v8__RegExp_(native.v8___RegExp__New(context._cobject,pattern._cobject,flags._cobject
    
    
    
    
  ,))
    
    
    
    
  


    def GetSource(self, ):
        '''
        @returns: v8.Local_v8__String_
        '''
        return Local_v8__String_(native.v8___RegExp__GetSource(self._cobject, ))

    def GetFlags(self, ):
        '''
        @returns: c_uint
        '''
        return native.v8___RegExp__GetFlags(self._cobject, )

    @staticmethod
    def Cast(obj,):
        '''
        @returns: v8.RegExp
        '''
        assert(isinstance(obj,Value))
        return native.v8___RegExp__Cast(obj,)

    @staticmethod
    def CheckCast(obj,):
        '''
        @returns: void
        '''
        assert(isinstance(obj,Value))
        return native.v8___RegExp__CheckCast(obj,)

    def assign(self, arg1,):
        '''
        @returns: v8.RegExp
        '''
        assert(isinstance(arg1,POINTER(RegExp)))
        return (native.v8___RegExp__assign(self._cobject, arg1._cobject,))        

class Template( _FwdDecl_Template ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Template)            
        """
        assert(isinstance(arg1,Template))
        obj =  Template\
            ( native.Template_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.Template)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Template_copy(args[0])
        else:
            
            try:
                self._cobject = native.Template_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Template(native.Template_copy( self._cobject ))

    def __del__(self):
        native.Template_delete(self._cobject)
        self._cobjet = None
            
    

    def Set(self, *args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return self.Set_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return self.Set_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def Set_1(self, name,value,attributes,):
        '''
        @returns: void
        '''
        assert(isinstance(name,Local_v8__Name_))
        assert(isinstance(value,Local_v8__Data_))
        assert(isinstance(attributes,c_uint))
        return native.v8___Template__Set(self._cobject, name._cobject,value._cobject,attributes._cobject
    
    
    
    
  ,)
    
    
    
    
  

    def Set_2(self, isolate,name,value,):
        '''
        @returns: void
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(name,c_char_p))
        assert(isinstance(value,Local_v8__Data_))
        return native.v8___Template__Set(self._cobject, isolate,name,value._cobject,)

    def SetAccessorProperty(self, name,getter,setter,attribute,settings,):
        '''
        @returns: void
        '''
        assert(isinstance(name,Local_v8__Name_))
        assert(isinstance(getter,Local_v8__FunctionTemplate_))
        assert(isinstance(setter,Local_v8__FunctionTemplate_))
        assert(isinstance(attribute,c_uint))
        assert(isinstance(settings,c_uint))
        return native.v8___Template__SetAccessorProperty(self._cobject, name._cobject,getter._cobject,setter._cobject,attribute._cobject
    
    
    
    
  ,settings._cobject
    
    
    
    
  ,)
    
    
    
    
  
    
    
    
    
  

    def SetNativeDataProperty(self, *args):
        count = 0
        if count==0 and len(*args) == 7:
            try:
                return self.SetNativeDataProperty_4(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 7:
            try:
                return self.SetNativeDataProperty_5(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def SetNativeDataProperty_4(self, name,getter,setter,data,attribute,signature,settings,):
        '''
        @returns: void
        '''
        assert(isinstance(name,Local_v8__String_))
        assert(isinstance(getter,AccessorGetterCallback))
        assert(isinstance(setter,AccessorSetterCallback))
        assert(isinstance(data,Local_v8__Value_))
        assert(isinstance(attribute,c_uint))
        assert(isinstance(signature,Local_v8__AccessorSignature_))
        assert(isinstance(settings,c_uint))
        return native.v8___Template__SetNativeDataProperty(self._cobject, name._cobject,getter,setter,data._cobject,attribute._cobject
    
    
    
    
  ,signature._cobject,settings._cobject
    
    
    
    
  ,)
    
    
    
    
  
    
    
    
    
  

    def SetNativeDataProperty_5(self, name,getter,setter,data,attribute,signature,settings,):
        '''
        @returns: void
        '''
        assert(isinstance(name,Local_v8__Name_))
        assert(isinstance(getter,AccessorNameGetterCallback))
        assert(isinstance(setter,AccessorNameSetterCallback))
        assert(isinstance(data,Local_v8__Value_))
        assert(isinstance(attribute,c_uint))
        assert(isinstance(signature,Local_v8__AccessorSignature_))
        assert(isinstance(settings,c_uint))
        return native.v8___Template__SetNativeDataProperty(self._cobject, name._cobject,getter,setter,data._cobject,attribute._cobject
    
    
    
    
  ,signature._cobject,settings._cobject
    
    
    
    
  ,)
    
    
    
    
  
    
    
    
    
  

    def assign(self, arg1,):
        '''
        @returns: v8.Template
        '''
        assert(isinstance(arg1,POINTER(Template)))
        return (native.v8___Template__assign(self._cobject, arg1._cobject,))


















class NamedPropertyHandlerConfiguration( _FwdDecl_NamedPropertyHandlerConfiguration):
    _fields_ = [ ("getter",GenericNamedPropertyGetterCallback),
    ("setter",GenericNamedPropertySetterCallback),
    ("query",GenericNamedPropertyQueryCallback),
    ("deleter",GenericNamedPropertyDeleterCallback),
    ("enumerator",GenericNamedPropertyEnumeratorCallback),
    ("data",v8.Local_v8__Value_),
    ("flags",c_uint),
    ]





class IndexedPropertyHandlerConfiguration( _FwdDecl_IndexedPropertyHandlerConfiguration):
    _fields_ = [ ("getter",IndexedPropertyGetterCallback),
    ("setter",IndexedPropertySetterCallback),
    ("query",IndexedPropertyQueryCallback),
    ("deleter",IndexedPropertyDeleterCallback),
    ("enumerator",IndexedPropertyEnumeratorCallback),
    ("data",v8.Local_v8__Value_),
    ("flags",c_uint),
    ]



        

class TypeSwitch( _FwdDecl_TypeSwitch ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.TypeSwitch)            
        """
        assert(isinstance(arg1,TypeSwitch))
        obj =  TypeSwitch\
            ( native.TypeSwitch_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.TypeSwitch)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.TypeSwitch_copy(args[0])
        else:
            
            try:
                self._cobject = native.TypeSwitch_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return TypeSwitch(native.TypeSwitch_copy( self._cobject ))

    def __del__(self):
        native.TypeSwitch_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def New(*args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return TypeSwitch.New_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return TypeSwitch.New_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def New_1(type,):
        '''
        @returns: v8.Local_v8__TypeSwitch_
        '''
        assert(isinstance(type,Local_v8__FunctionTemplate_))
        return Local_v8__TypeSwitch_(native.v8___TypeSwitch__New(type._cobject,))

    @staticmethod
    def New_2(argc,types,):
        '''
        @returns: v8.Local_v8__TypeSwitch_
        '''
        assert(isinstance(argc,c_int))
        assert(isinstance(types,Local_v8__FunctionTemplate_))
        return Local_v8__TypeSwitch_(native.v8___TypeSwitch__New(argc,types,))

    def match(self, value,):
        '''
        @returns: c_int
        '''
        assert(isinstance(value,Local_v8__Value_))
        return native.v8___TypeSwitch__match(self._cobject, value._cobject,)

    def assign(self, arg1,):
        '''
        @returns: v8.TypeSwitch
        '''
        assert(isinstance(arg1,POINTER(TypeSwitch)))
        return (native.v8___TypeSwitch__assign(self._cobject, arg1._cobject,))        

class ExternalOneByteStringResourceImpl( _FwdDecl_ExternalOneByteStringResourceImpl ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  ExternalOneByteStringResourceImpl\
            ( native.ExternalOneByteStringResourceImpl_new())
        return obj
        

    @staticmethod
    def new2(self, data,length,):
        """
        Signature: 
           data:c_char_p            
        
           length:size_t            
        """
        assert(isinstance(data,ExternalOneByteStringResourceImpl))
        assert(isinstance(length,ExternalOneByteStringResourceImpl))
        obj =  ExternalOneByteStringResourceImpl\
            ( native.ExternalOneByteStringResourceImpl_new(data, length, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:c_char_p            
        
           args[1]:size_t            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ExternalOneByteStringResourceImpl_copy(args[0])
        else:
            
            try:
                self._cobject = native.ExternalOneByteStringResourceImpl_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.ExternalOneByteStringResourceImpl_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ExternalOneByteStringResourceImpl(native.ExternalOneByteStringResourceImpl_copy( self._cobject ))

    def __del__(self):
        native.ExternalOneByteStringResourceImpl_delete(self._cobject)
        self._cobjet = None
            
    

    def data(self, ):
        '''
        @returns: c_char_p
        '''
        return native.v8___ExternalOneByteStringResourceImpl__data(self._cobject, )

    def length(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___ExternalOneByteStringResourceImpl__length(self._cobject, )        

class Extension( _FwdDecl_Extension ):

    @staticmethod
    def new1(self, name,source,dep_count,deps,source_length,):
        """
        Signature: 
           name:c_char_p            
        
           source:c_char_p            
        
           dep_count:c_int            
        
           deps:POINTER(c_char_p)            
        
           source_length:c_int            
        """
        assert(isinstance(name,Extension))
        assert(isinstance(source,Extension))
        assert(isinstance(dep_count,Extension))
        assert(isinstance(deps,Extension))
        assert(isinstance(source_length,Extension))
        obj =  Extension\
            ( native.Extension_new(name, source, dep_count, deps, source_length, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:c_char_p            
        
           args[1]:c_char_p            
        
           args[2]:c_int            
        
           args[3]:POINTER(c_char_p)            
        
           args[4]:c_int            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Extension_copy(args[0])
        else:
            
            try:
                self._cobject = native.Extension_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Extension(native.Extension_copy( self._cobject ))

    def __del__(self):
        native.Extension_delete(self._cobject)
        self._cobjet = None
            
    

    def GetNativeFunctionTemplate(self, isolate,name,):
        '''
        @returns: v8.Local_v8__FunctionTemplate_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(name,Local_v8__String_))
        return Local_v8__FunctionTemplate_(native.v8___Extension__GetNativeFunctionTemplate(self._cobject, isolate,name._cobject,))

    def name(self, ):
        '''
        @returns: c_char_p
        '''
        return native.v8___Extension__name(self._cobject, )

    def source_length(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___Extension__source_length(self._cobject, )

    def source(self, ):
        '''
        @returns: POINTER(v8.ExternalOneByteStringResource)
        '''
        return native.v8___Extension__source(self._cobject, )

    def dependency_count(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___Extension__dependency_count(self._cobject, )

    def dependencies(self, ):
        '''
        @returns: POINTER(c_char_p)
        '''
        return native.v8___Extension__dependencies(self._cobject, )

    def set_auto_enable(self, value,):
        '''
        @returns: void
        '''
        assert(isinstance(value,c_char))
        return native.v8___Extension__set_auto_enable(self._cobject, value,)

    def auto_enable(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___Extension__auto_enable(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: void
        '''
        assert(isinstance(arg1,POINTER(Extension)))
        return native.v8___Extension__assign(self._cobject, arg1._cobject,)        

class ResourceConstraints( _FwdDecl_ResourceConstraints ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  ResourceConstraints\
            ( native.ResourceConstraints_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.ResourceConstraints)            
        """
        assert(isinstance(arg1,ResourceConstraints))
        obj =  ResourceConstraints\
            ( native.ResourceConstraints_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.ResourceConstraints)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ResourceConstraints_copy(args[0])
        else:
            
            try:
                self._cobject = native.ResourceConstraints_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.ResourceConstraints_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ResourceConstraints(native.ResourceConstraints_copy( self._cobject ))

    def __del__(self):
        native.ResourceConstraints_delete(self._cobject)
        self._cobjet = None
            
    

    def ConfigureDefaults(self, *args):
        count = 0
        if count==0 and len(*args) == 2:
            try:
                return self.ConfigureDefaults_1(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 3:
            try:
                return self.ConfigureDefaults_2(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def ConfigureDefaults_1(self, physical_memory,virtual_memory_limit,):
        '''
        @returns: void
        '''
        assert(isinstance(physical_memory,uint64_t))
        assert(isinstance(virtual_memory_limit,uint64_t))
        return native.v8___ResourceConstraints__ConfigureDefaults(self._cobject, physical_memory,virtual_memory_limit,)



    def ConfigureDefaults_2(self, physical_memory,virtual_memory_limit,number_of_processors,):
        '''
        @returns: void
        '''
        assert(isinstance(physical_memory,uint64_t))
        assert(isinstance(virtual_memory_limit,uint64_t))
        assert(isinstance(number_of_processors,uint32_t))
        return native.v8___ResourceConstraints__ConfigureDefaults(self._cobject, physical_memory,virtual_memory_limit,number_of_processors,)




    def max_semi_space_size(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___ResourceConstraints__max_semi_space_size(self._cobject, )

    def set_max_semi_space_size(self, value,):
        '''
        @returns: void
        '''
        assert(isinstance(value,c_int))
        return native.v8___ResourceConstraints__set_max_semi_space_size(self._cobject, value,)

    def max_old_space_size(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___ResourceConstraints__max_old_space_size(self._cobject, )

    def set_max_old_space_size(self, value,):
        '''
        @returns: void
        '''
        assert(isinstance(value,c_int))
        return native.v8___ResourceConstraints__set_max_old_space_size(self._cobject, value,)

    def max_executable_size(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___ResourceConstraints__max_executable_size(self._cobject, )

    def set_max_executable_size(self, value,):
        '''
        @returns: void
        '''
        assert(isinstance(value,c_int))
        return native.v8___ResourceConstraints__set_max_executable_size(self._cobject, value,)

    def stack_limit(self, ):
        '''
        @returns: POINTER(uint32_t)
        '''
        return native.v8___ResourceConstraints__stack_limit(self._cobject, )

    def set_stack_limit(self, value,):
        '''
        @returns: void
        '''
        assert(isinstance(value,POINTER(uint32_t)))
        return native.v8___ResourceConstraints__set_stack_limit(self._cobject, value,)


    def max_available_threads(self, ):
        '''
        @returns: c_int
        '''
        return native.v8___ResourceConstraints__max_available_threads(self._cobject, )

    def set_max_available_threads(self, value,):
        '''
        @returns: void
        '''
        assert(isinstance(value,c_int))
        return native.v8___ResourceConstraints__set_max_available_threads(self._cobject, value,)

    def code_range_size(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___ResourceConstraints__code_range_size(self._cobject, )

    def set_code_range_size(self, value,):
        '''
        @returns: void
        '''
        assert(isinstance(value,size_t))
        return native.v8___ResourceConstraints__set_code_range_size(self._cobject, value,)


    def assign(self, arg1,):
        '''
        @returns: v8.ResourceConstraints
        '''
        assert(isinstance(arg1,POINTER(ResourceConstraints)))
        return (native.v8___ResourceConstraints__assign(self._cobject, arg1._cobject,))


        

class Exception( _FwdDecl_Exception ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Exception\
            ( native.Exception_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Exception)            
        """
        assert(isinstance(arg1,Exception))
        obj =  Exception\
            ( native.Exception_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.Exception)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Exception_copy(args[0])
        else:
            
            try:
                self._cobject = native.Exception_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Exception_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Exception(native.Exception_copy( self._cobject ))

    def __del__(self):
        native.Exception_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def RangeError(message,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(message,Local_v8__String_))
        return Local_v8__Value_(native.v8___Exception__RangeError(message._cobject,))

    @staticmethod
    def ReferenceError(message,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(message,Local_v8__String_))
        return Local_v8__Value_(native.v8___Exception__ReferenceError(message._cobject,))

    @staticmethod
    def SyntaxError(message,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(message,Local_v8__String_))
        return Local_v8__Value_(native.v8___Exception__SyntaxError(message._cobject,))

    @staticmethod
    def TypeError(message,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(message,Local_v8__String_))
        return Local_v8__Value_(native.v8___Exception__TypeError(message._cobject,))

    @staticmethod
    def Error(message,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(message,Local_v8__String_))
        return Local_v8__Value_(native.v8___Exception__Error(message._cobject,))

    @staticmethod
    def CreateMessage(exception,):
        '''
        @returns: v8.Local_v8__Message_
        '''
        assert(isinstance(exception,Local_v8__Value_))
        return Local_v8__Message_(native.v8___Exception__CreateMessage(exception._cobject,))

    @staticmethod
    def GetStackTrace(exception,):
        '''
        @returns: v8.Local_v8__StackTrace_
        '''
        assert(isinstance(exception,Local_v8__Value_))
        return Local_v8__StackTrace_(native.v8___Exception__GetStackTrace(exception._cobject,))

    def assign(self, arg1,):
        '''
        @returns: v8.Exception
        '''
        assert(isinstance(arg1,POINTER(Exception)))
        return (native.v8___Exception__assign(self._cobject, arg1._cobject,))




        

class PromiseRejectMessage( _FwdDecl_PromiseRejectMessage ):

    @staticmethod
    def new1(self, promise,event,value,stack_trace,):
        """
        Signature: 
           promise:v8.Local_v8__Promise_            
        
           event:c_uint            
        
           value:v8.Local_v8__Value_            
        
           stack_trace:v8.Local_v8__StackTrace_            
        """
        assert(isinstance(promise,PromiseRejectMessage))
        assert(isinstance(event,PromiseRejectMessage))
        assert(isinstance(value,PromiseRejectMessage))
        assert(isinstance(stack_trace,PromiseRejectMessage))
        obj =  PromiseRejectMessage\
            ( native.PromiseRejectMessage_new(promise._cobject, event._cobject
    
    
  , value._cobject, stack_trace._cobject, ))
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.PromiseRejectMessage)            
        """
        assert(isinstance(arg1,PromiseRejectMessage))
        obj =  PromiseRejectMessage\
            ( native.PromiseRejectMessage_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Local_v8__Promise_            
        
           args[1]:c_uint            
        
           args[2]:v8.Local_v8__Value_            
        
           args[3]:v8.Local_v8__StackTrace_            
        
        Signature[2]: 
           args[0]:POINTER(v8.PromiseRejectMessage)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PromiseRejectMessage_copy(args[0])
        else:
            
            try:
                self._cobject = native.PromiseRejectMessage_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.PromiseRejectMessage_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PromiseRejectMessage(native.PromiseRejectMessage_copy( self._cobject ))

    def __del__(self):
        native.PromiseRejectMessage_delete(self._cobject)
        self._cobjet = None
            
    

    def GetPromise(self, ):
        '''
        @returns: v8.Local_v8__Promise_
        '''
        return Local_v8__Promise_(native.v8___PromiseRejectMessage__GetPromise(self._cobject, ))

    def GetEvent(self, ):
        '''
        @returns: c_uint
        '''
        return native.v8___PromiseRejectMessage__GetEvent(self._cobject, )

    def GetValue(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___PromiseRejectMessage__GetValue(self._cobject, ))

    def GetStackTrace(self, ):
        '''
        @returns: v8.Local_v8__StackTrace_
        '''
        return Local_v8__StackTrace_(native.v8___PromiseRejectMessage__GetStackTrace(self._cobject, ))

    def assign(self, arg1,):
        '''
        @returns: v8.PromiseRejectMessage
        '''
        assert(isinstance(arg1,POINTER(PromiseRejectMessage)))
        return (native.v8___PromiseRejectMessage__assign(self._cobject, arg1._cobject,))






        

class HeapStatistics( _FwdDecl_HeapStatistics ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  HeapStatistics\
            ( native.HeapStatistics_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.HeapStatistics)            
        """
        assert(isinstance(arg1,HeapStatistics))
        obj =  HeapStatistics\
            ( native.HeapStatistics_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.HeapStatistics)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.HeapStatistics_copy(args[0])
        else:
            
            try:
                self._cobject = native.HeapStatistics_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.HeapStatistics_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return HeapStatistics(native.HeapStatistics_copy( self._cobject ))

    def __del__(self):
        native.HeapStatistics_delete(self._cobject)
        self._cobjet = None
            
    

    def total_heap_size(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___HeapStatistics__total_heap_size(self._cobject, )

    def total_heap_size_executable(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___HeapStatistics__total_heap_size_executable(self._cobject, )

    def total_physical_size(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___HeapStatistics__total_physical_size(self._cobject, )

    def total_available_size(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___HeapStatistics__total_available_size(self._cobject, )

    def used_heap_size(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___HeapStatistics__used_heap_size(self._cobject, )

    def heap_size_limit(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___HeapStatistics__heap_size_limit(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: v8.HeapStatistics
        '''
        assert(isinstance(arg1,POINTER(HeapStatistics)))
        return (native.v8___HeapStatistics__assign(self._cobject, arg1._cobject,))        

class HeapSpaceStatistics( _FwdDecl_HeapSpaceStatistics ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  HeapSpaceStatistics\
            ( native.HeapSpaceStatistics_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.HeapSpaceStatistics)            
        """
        assert(isinstance(arg1,HeapSpaceStatistics))
        obj =  HeapSpaceStatistics\
            ( native.HeapSpaceStatistics_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.HeapSpaceStatistics)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.HeapSpaceStatistics_copy(args[0])
        else:
            
            try:
                self._cobject = native.HeapSpaceStatistics_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.HeapSpaceStatistics_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return HeapSpaceStatistics(native.HeapSpaceStatistics_copy( self._cobject ))

    def __del__(self):
        native.HeapSpaceStatistics_delete(self._cobject)
        self._cobjet = None
            
    

    def space_name(self, ):
        '''
        @returns: c_char_p
        '''
        return native.v8___HeapSpaceStatistics__space_name(self._cobject, )

    def space_size(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___HeapSpaceStatistics__space_size(self._cobject, )

    def space_used_size(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___HeapSpaceStatistics__space_used_size(self._cobject, )

    def space_available_size(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___HeapSpaceStatistics__space_available_size(self._cobject, )

    def physical_space_size(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___HeapSpaceStatistics__physical_space_size(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: v8.HeapSpaceStatistics
        '''
        assert(isinstance(arg1,POINTER(HeapSpaceStatistics)))
        return (native.v8___HeapSpaceStatistics__assign(self._cobject, arg1._cobject,))        

class HeapObjectStatistics( _FwdDecl_HeapObjectStatistics ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  HeapObjectStatistics\
            ( native.HeapObjectStatistics_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.HeapObjectStatistics)            
        """
        assert(isinstance(arg1,HeapObjectStatistics))
        obj =  HeapObjectStatistics\
            ( native.HeapObjectStatistics_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.HeapObjectStatistics)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.HeapObjectStatistics_copy(args[0])
        else:
            
            try:
                self._cobject = native.HeapObjectStatistics_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.HeapObjectStatistics_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return HeapObjectStatistics(native.HeapObjectStatistics_copy( self._cobject ))

    def __del__(self):
        native.HeapObjectStatistics_delete(self._cobject)
        self._cobjet = None
            
    

    def object_type(self, ):
        '''
        @returns: c_char_p
        '''
        return native.v8___HeapObjectStatistics__object_type(self._cobject, )

    def object_sub_type(self, ):
        '''
        @returns: c_char_p
        '''
        return native.v8___HeapObjectStatistics__object_sub_type(self._cobject, )

    def object_count(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___HeapObjectStatistics__object_count(self._cobject, )

    def object_size(self, ):
        '''
        @returns: size_t
        '''
        return native.v8___HeapObjectStatistics__object_size(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: v8.HeapObjectStatistics
        '''
        assert(isinstance(arg1,POINTER(HeapObjectStatistics)))
        return (native.v8___HeapObjectStatistics__assign(self._cobject, arg1._cobject,))        

class RetainedObjectInfo( _FwdDecl_RetainedObjectInfo ):
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.RetainedObjectInfo_copy(args[0])
        else:
            self._cobject = None#No public constructor
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return RetainedObjectInfo(native.RetainedObjectInfo_copy( self._cobject ))

    def __del__(self):
        native.RetainedObjectInfo_delete(self._cobject)
        self._cobjet = None
            
    



class JitCodeEvent( _FwdDecl_JitCodeEvent):
    _fields_ = [ ("type",c_uint),
    ("code_start",c_void_p),
    ("code_len",size_t),
    ("script",v8.Local_v8__UnboundScript_),
    ("user_data",c_void_p),
    ("member_2151",
CUnion_Anon__2150),
    ]





        

class ExternalResourceVisitor( _FwdDecl_ExternalResourceVisitor ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  ExternalResourceVisitor\
            ( native.ExternalResourceVisitor_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.ExternalResourceVisitor)            
        """
        assert(isinstance(arg1,ExternalResourceVisitor))
        obj =  ExternalResourceVisitor\
            ( native.ExternalResourceVisitor_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.ExternalResourceVisitor)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ExternalResourceVisitor_copy(args[0])
        else:
            
            try:
                self._cobject = native.ExternalResourceVisitor_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.ExternalResourceVisitor_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ExternalResourceVisitor(native.ExternalResourceVisitor_copy( self._cobject ))

    def __del__(self):
        native.ExternalResourceVisitor_delete(self._cobject)
        self._cobjet = None
            
    

    def VisitExternalString(self, string,):
        '''
        @returns: void
        '''
        assert(isinstance(string,Local_v8__String_))
        return native.v8___ExternalResourceVisitor__VisitExternalString(self._cobject, string._cobject,)

    def assign(self, arg1,):
        '''
        @returns: v8.ExternalResourceVisitor
        '''
        assert(isinstance(arg1,POINTER(ExternalResourceVisitor)))
        return (native.v8___ExternalResourceVisitor__assign(self._cobject, arg1._cobject,))        

class PersistentHandleVisitor( _FwdDecl_PersistentHandleVisitor ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  PersistentHandleVisitor\
            ( native.PersistentHandleVisitor_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.PersistentHandleVisitor)            
        """
        assert(isinstance(arg1,PersistentHandleVisitor))
        obj =  PersistentHandleVisitor\
            ( native.PersistentHandleVisitor_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.PersistentHandleVisitor)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.PersistentHandleVisitor_copy(args[0])
        else:
            
            try:
                self._cobject = native.PersistentHandleVisitor_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.PersistentHandleVisitor_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PersistentHandleVisitor(native.PersistentHandleVisitor_copy( self._cobject ))

    def __del__(self):
        native.PersistentHandleVisitor_delete(self._cobject)
        self._cobjet = None
            
    

    def VisitPersistentHandle(self, value,class_id,):
        '''
        @returns: void
        '''
        assert(isinstance(value,Persistent_v8__Value__v8__NonCopyablePersistentTraits_v8__Value___))
        assert(isinstance(class_id,uint16_t))
        return native.v8___PersistentHandleVisitor__VisitPersistentHandle(self._cobject, value,class_id,)


    def assign(self, arg1,):
        '''
        @returns: v8.PersistentHandleVisitor
        '''
        assert(isinstance(arg1,POINTER(PersistentHandleVisitor)))
        return (native.v8___PersistentHandleVisitor__assign(self._cobject, arg1._cobject,))


        

class V8( _FwdDecl_V8 ):

    @staticmethod
    def new1(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.V8)            
        """
        assert(isinstance(arg1,V8))
        obj =  V8\
            ( native.V8_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:POINTER(v8.V8)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.V8_copy(args[0])
        else:
            
            try:
                self._cobject = native.V8_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return V8(native.V8_copy( self._cobject ))

    def __del__(self):
        native.V8_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def SetFatalErrorHandler(that,):
        '''
        @returns: void
        '''
        assert(isinstance(that,FatalErrorCallback))
        return native.v8___V8__SetFatalErrorHandler(that,)

    @staticmethod
    def SetAllowCodeGenerationFromStringsCallback(that,):
        '''
        @returns: void
        '''
        assert(isinstance(that,AllowCodeGenerationFromStringsCallback))
        return native.v8___V8__SetAllowCodeGenerationFromStringsCallback(that,)

    @staticmethod
    def SetArrayBufferAllocator(allocator,):
        '''
        @returns: void
        '''
        assert(isinstance(allocator,Allocator))
        return native.v8___V8__SetArrayBufferAllocator(allocator,)


    @staticmethod
    def IsDead():
        '''
        @returns: c_char
        '''
        return native.v8___V8__IsDead()

    @staticmethod
    def SetNativesDataBlob(startup_blob,):
        '''
        @returns: void
        '''
        assert(isinstance(startup_blob,StartupData))
        return native.v8___V8__SetNativesDataBlob(startup_blob,)

    @staticmethod
    def SetSnapshotDataBlob(startup_blob,):
        '''
        @returns: void
        '''
        assert(isinstance(startup_blob,StartupData))
        return native.v8___V8__SetSnapshotDataBlob(startup_blob,)

    @staticmethod
    def CreateSnapshotDataBlob(custom_source,):
        '''
        @returns: v8.StartupData
        '''
        assert(isinstance(custom_source,c_char_p))
        return StartupData(native.v8___V8__CreateSnapshotDataBlob(custom_source,))

    @staticmethod
    def AddMessageListener(that,data,):
        '''
        @returns: c_char
        '''
        assert(isinstance(that,MessageCallback))
        assert(isinstance(data,Local_v8__Value_))
        return native.v8___V8__AddMessageListener(that,data._cobject,)

    @staticmethod
    def RemoveMessageListeners(that,):
        '''
        @returns: void
        '''
        assert(isinstance(that,MessageCallback))
        return native.v8___V8__RemoveMessageListeners(that,)

    @staticmethod
    def SetCaptureStackTraceForUncaughtExceptions(capture,frame_limit,options,):
        '''
        @returns: void
        '''
        assert(isinstance(capture,c_char))
        assert(isinstance(frame_limit,c_int))
        assert(isinstance(options,c_uint))
        return native.v8___V8__SetCaptureStackTraceForUncaughtExceptions(capture,frame_limit,options._cobject
    
    
    
    
    
    
    
    
    
    
    
  ,)
    
    
    
    
    
    
    
    
    
    
    
  

    @staticmethod
    def SetFlagsFromString(str,length,):
        '''
        @returns: void
        '''
        assert(isinstance(str,c_char_p))
        assert(isinstance(length,c_int))
        return native.v8___V8__SetFlagsFromString(str,length,)

    @staticmethod
    def SetFlagsFromCommandLine(argc,argv,remove_flags,):
        '''
        @returns: void
        '''
        assert(isinstance(argc,POINTER(c_int)))
        assert(isinstance(argv,POINTER(c_char_p)))
        assert(isinstance(remove_flags,c_char))
        return native.v8___V8__SetFlagsFromCommandLine(argc,argv,remove_flags,)

    @staticmethod
    def GetVersion():
        '''
        @returns: c_char_p
        '''
        return native.v8___V8__GetVersion()

    @staticmethod
    def SetFailedAccessCheckCallbackFunction(arg1,):
        '''
        @returns: void
        '''
        assert(isinstance(arg1,FailedAccessCheckCallback))
        return native.v8___V8__SetFailedAccessCheckCallbackFunction(arg1,)

    @staticmethod
    def AddGCPrologueCallback(callback,gc_type_filter,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,GCPrologueCallback))
        assert(isinstance(gc_type_filter,c_uint))
        return native.v8___V8__AddGCPrologueCallback(callback,gc_type_filter._cobject
    
    
    
  ,)
    
    
    
  

    @staticmethod
    def RemoveGCPrologueCallback(callback,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,GCPrologueCallback))
        return native.v8___V8__RemoveGCPrologueCallback(callback,)

    @staticmethod
    def AddGCEpilogueCallback(callback,gc_type_filter,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,GCEpilogueCallback))
        assert(isinstance(gc_type_filter,c_uint))
        return native.v8___V8__AddGCEpilogueCallback(callback,gc_type_filter._cobject
    
    
    
  ,)
    
    
    
  

    @staticmethod
    def RemoveGCEpilogueCallback(callback,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,GCEpilogueCallback))
        return native.v8___V8__RemoveGCEpilogueCallback(callback,)

    @staticmethod
    def AddMemoryAllocationCallback(callback,space,action,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,MemoryAllocationCallback))
        assert(isinstance(space,c_uint))
        assert(isinstance(action,c_uint))
        return native.v8___V8__AddMemoryAllocationCallback(callback,space._cobject
    
    
    
    
    
    
  ,action._cobject
    
    
    
  ,)
    
    
    
    
    
    
  
    
    
    
  

    @staticmethod
    def RemoveMemoryAllocationCallback(callback,):
        '''
        @returns: void
        '''
        assert(isinstance(callback,MemoryAllocationCallback))
        return native.v8___V8__RemoveMemoryAllocationCallback(callback,)

    @staticmethod
    def Initialize():
        '''
        @returns: c_char
        '''
        return native.v8___V8__Initialize()

    @staticmethod
    def SetEntropySource(source,):
        '''
        @returns: void
        '''
        assert(isinstance(source,EntropySource))
        return native.v8___V8__SetEntropySource(source,)

    @staticmethod
    def SetReturnAddressLocationResolver(return_address_resolver,):
        '''
        @returns: void
        '''
        assert(isinstance(return_address_resolver,ReturnAddressLocationResolver))
        return native.v8___V8__SetReturnAddressLocationResolver(return_address_resolver,)

    @staticmethod
    def TerminateExecution(isolate,):
        '''
        @returns: void
        '''
        assert(isinstance(isolate,Isolate))
        return native.v8___V8__TerminateExecution(isolate,)

    @staticmethod
    def IsExecutionTerminating(isolate,):
        '''
        @returns: c_char
        '''
        assert(isinstance(isolate,Isolate))
        return native.v8___V8__IsExecutionTerminating(isolate,)

    @staticmethod
    def CancelTerminateExecution(isolate,):
        '''
        @returns: void
        '''
        assert(isinstance(isolate,Isolate))
        return native.v8___V8__CancelTerminateExecution(isolate,)

    @staticmethod
    def Dispose():
        '''
        @returns: c_char
        '''
        return native.v8___V8__Dispose()

    @staticmethod
    def VisitExternalResources(visitor,):
        '''
        @returns: void
        '''
        assert(isinstance(visitor,ExternalResourceVisitor))
        return native.v8___V8__VisitExternalResources(visitor,)

    @staticmethod
    def VisitHandlesWithClassIds(*args):
        count = 0
        if count==0 and len(*args) == 1:
            try:
                return V8.VisitHandlesWithClassIds_29(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 2:
            try:
                return V8.VisitHandlesWithClassIds_30(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def VisitHandlesWithClassIds_29(visitor,):
        '''
        @returns: void
        '''
        assert(isinstance(visitor,PersistentHandleVisitor))
        return native.v8___V8__VisitHandlesWithClassIds(visitor,)

    @staticmethod
    def VisitHandlesWithClassIds_30(isolate,visitor,):
        '''
        @returns: void
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(visitor,PersistentHandleVisitor))
        return native.v8___V8__VisitHandlesWithClassIds(isolate,visitor,)

    @staticmethod
    def VisitHandlesForPartialDependence(isolate,visitor,):
        '''
        @returns: void
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(visitor,PersistentHandleVisitor))
        return native.v8___V8__VisitHandlesForPartialDependence(isolate,visitor,)

    @staticmethod
    def InitializeICU(icu_data_file,):
        '''
        @returns: c_char
        '''
        assert(isinstance(icu_data_file,c_char_p))
        return native.v8___V8__InitializeICU(icu_data_file,)

    @staticmethod
    def InitializePlatform(platform,):
        '''
        @returns: void
        '''
        assert(isinstance(platform,Platform))
        return native.v8___V8__InitializePlatform(platform,)

    @staticmethod
    def ShutdownPlatform():
        '''
        @returns: void
        '''
        return native.v8___V8__ShutdownPlatform()

    @staticmethod
    def GlobalizeReference(isolate,handle,):
        '''
        @returns: POINTER(v8.internal.Object)
        '''
        assert(isinstance(isolate,v8.internal.Isolate))
        assert(isinstance(handle,POINTER(v8.internal.Object)))
        return native.v8___V8__GlobalizeReference(isolate,handle,)



    @staticmethod
    def CopyPersistent(handle,):
        '''
        @returns: POINTER(v8.internal.Object)
        '''
        assert(isinstance(handle,POINTER(v8.internal.Object)))
        return native.v8___V8__CopyPersistent(handle,)


    @staticmethod
    def DisposeGlobal(global_handle,):
        '''
        @returns: void
        '''
        assert(isinstance(global_handle,POINTER(v8.internal.Object)))
        return native.v8___V8__DisposeGlobal(global_handle,)


    @staticmethod
    def MakeWeak(*args):
        count = 0
        if count==0 and len(*args) == 3:
            try:
                return V8.MakeWeak_38(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 4:
            try:
                return V8.MakeWeak_39(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 5:
            try:
                return V8.MakeWeak_40(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    @staticmethod
    def MakeWeak_38(global_handle,data,weak_callback,):
        '''
        @returns: void
        '''
        assert(isinstance(global_handle,POINTER(v8.internal.Object)))
        assert(isinstance(data,c_void_p))
        assert(isinstance(weak_callback,WeakCallback))
        return native.v8___V8__MakeWeak(global_handle,data,weak_callback,)


    @staticmethod
    def MakeWeak_39(global_handle,data,weak_callback,type,):
        '''
        @returns: void
        '''
        assert(isinstance(global_handle,POINTER(v8.internal.Object)))
        assert(isinstance(data,c_void_p))
        assert(isinstance(weak_callback,Callback))
        assert(isinstance(type,c_uint))
        return native.v8___V8__MakeWeak(global_handle,data,weak_callback,type._cobject
    
    
  ,)
    
    
  


    @staticmethod
    def MakeWeak_40(global_handle,data,internal_field_index1,internal_field_index2,weak_callback,):
        '''
        @returns: void
        '''
        assert(isinstance(global_handle,POINTER(v8.internal.Object)))
        assert(isinstance(data,c_void_p))
        assert(isinstance(internal_field_index1,c_int))
        assert(isinstance(internal_field_index2,c_int))
        assert(isinstance(weak_callback,Callback))
        return native.v8___V8__MakeWeak(global_handle,data,internal_field_index1,internal_field_index2,weak_callback,)


    @staticmethod
    def ClearWeak(global_handle,):
        '''
        @returns: c_void_p
        '''
        assert(isinstance(global_handle,POINTER(v8.internal.Object)))
        return native.v8___V8__ClearWeak(global_handle,)


    @staticmethod
    def Eternalize(isolate,handle,index,):
        '''
        @returns: void
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(handle,Value))
        assert(isinstance(index,POINTER(c_int)))
        return native.v8___V8__Eternalize(isolate,handle,index,)

    @staticmethod
    def GetEternal(isolate,index,):
        '''
        @returns: v8.Local_v8__Value_
        '''
        assert(isinstance(isolate,Isolate))
        assert(isinstance(index,c_int))
        return Local_v8__Value_(native.v8___V8__GetEternal(isolate,index,))

    @staticmethod
    def FromJustIsNothing():
        '''
        @returns: void
        '''
        return native.v8___V8__FromJustIsNothing()

    @staticmethod
    def ToLocalEmpty():
        '''
        @returns: void
        '''
        return native.v8___V8__ToLocalEmpty()

    @staticmethod
    def InternalFieldOutOfBounds(index,):
        '''
        @returns: void
        '''
        assert(isinstance(index,c_int))
        return native.v8___V8__InternalFieldOutOfBounds(index,)

    def assign(self, arg1,):
        '''
        @returns: v8.V8
        '''
        assert(isinstance(arg1,POINTER(V8)))
        return (native.v8___V8__assign(self._cobject, arg1._cobject,))        

class TryCatch( _FwdDecl_TryCatch ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  TryCatch\
            ( native.TryCatch_new())
        return obj
        

    @staticmethod
    def new2(self, isolate,):
        """
        Signature: 
           isolate:v8.Isolate            
        """
        assert(isinstance(isolate,TryCatch))
        obj =  TryCatch\
            ( native.TryCatch_new(isolate, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:v8.Isolate            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.TryCatch_copy(args[0])
        else:
            
            try:
                self._cobject = native.TryCatch_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.TryCatch_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return TryCatch(native.TryCatch_copy( self._cobject ))

    def __del__(self):
        native.TryCatch_delete(self._cobject)
        self._cobjet = None
            
    

    def HasCaught(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___TryCatch__HasCaught(self._cobject, )

    def CanContinue(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___TryCatch__CanContinue(self._cobject, )

    def HasTerminated(self, ):
        '''
        @returns: c_char
        '''
        return native.v8___TryCatch__HasTerminated(self._cobject, )

    def ReThrow(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___TryCatch__ReThrow(self._cobject, ))

    def Exception(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___TryCatch__Exception(self._cobject, ))

    def StackTrace(self, *args):
        count = 0
        if count==0 and len(*args) == 0:
            try:
                return self.StackTrace_6(*args)
                count += 1
            except:
                pass
        if count==0 and len(*args) == 1:
            try:
                return self.StackTrace_7(*args)
                count += 1
            except:
                pass
        if not count:
            raise Exception("No signature matching arguments")


    def StackTrace_6(self, ):
        '''
        @returns: v8.Local_v8__Value_
        '''
        return Local_v8__Value_(native.v8___TryCatch__StackTrace(self._cobject, ))

    def StackTrace_7(self, context,):
        '''
        @returns: v8.MaybeLocal_v8__Value_
        '''
        assert(isinstance(context,Local_v8__Context_))
        return MaybeLocal_v8__Value_(native.v8___TryCatch__StackTrace(self._cobject, context._cobject,))

    def Message(self, ):
        '''
        @returns: v8.Local_v8__Message_
        '''
        return Local_v8__Message_(native.v8___TryCatch__Message(self._cobject, ))

    def Reset(self, ):
        '''
        @returns: void
        '''
        return native.v8___TryCatch__Reset(self._cobject, )

    def SetVerbose(self, value,):
        '''
        @returns: void
        '''
        assert(isinstance(value,c_char))
        return native.v8___TryCatch__SetVerbose(self._cobject, value,)

    def SetCaptureMessage(self, value,):
        '''
        @returns: void
        '''
        assert(isinstance(value,c_char))
        return native.v8___TryCatch__SetCaptureMessage(self._cobject, value,)

    @staticmethod
    def JSStackComparableAddress(handler,):
        '''
        @returns: c_void_p
        '''
        assert(isinstance(handler,TryCatch))
        return native.v8___TryCatch__JSStackComparableAddress(handler,)

    def ResetInternal(self, ):
        '''
        @returns: void
        '''
        return native.v8___TryCatch__ResetInternal(self._cobject, )

    def assign(self, arg1,):
        '''
        @returns: void
        '''
        assert(isinstance(arg1,POINTER(TryCatch)))
        return native.v8___TryCatch__assign(self._cobject, arg1._cobject,)
    
  
    
    
          

class ExtensionConfiguration( _FwdDecl_ExtensionConfiguration ):

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  ExtensionConfiguration\
            ( native.ExtensionConfiguration_new())
        return obj
        

    @staticmethod
    def new2(self, name_count,names,):
        """
        Signature: 
           name_count:c_int            
        
           names:POINTER(c_char_p)            
        """
        assert(isinstance(name_count,ExtensionConfiguration))
        assert(isinstance(names,ExtensionConfiguration))
        obj =  ExtensionConfiguration\
            ( native.ExtensionConfiguration_new(name_count, names, ))
        return obj
        

    @staticmethod
    def new3(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.ExtensionConfiguration)            
        """
        assert(isinstance(arg1,ExtensionConfiguration))
        obj =  ExtensionConfiguration\
            ( native.ExtensionConfiguration_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:c_int            
        
           args[1]:POINTER(c_char_p)            
        
        Signature[3]: 
           args[0]:POINTER(v8.ExtensionConfiguration)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.ExtensionConfiguration_copy(args[0])
        else:
            
            try:
                self._cobject = native.ExtensionConfiguration_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.ExtensionConfiguration_new2(*args)
            except:
                pass
        
            try:
                self._cobject = native.ExtensionConfiguration_new3(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return ExtensionConfiguration(native.ExtensionConfiguration_copy( self._cobject ))

    def __del__(self):
        native.ExtensionConfiguration_delete(self._cobject)
        self._cobjet = None
            
    

    def begin(self, ):
        '''
        @returns: POINTER(c_char_p)
        '''
        return native.v8___ExtensionConfiguration__begin(self._cobject, )

    def end(self, ):
        '''
        @returns: POINTER(c_char_p)
        '''
        return native.v8___ExtensionConfiguration__end(self._cobject, )        

class Unlocker( _FwdDecl_Unlocker ):

    @staticmethod
    def new1(self, isolate,):
        """
        Signature: 
           isolate:v8.Isolate            
        """
        assert(isinstance(isolate,Unlocker))
        obj =  Unlocker\
            ( native.Unlocker_new(isolate, ))
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.Unlocker)            
        """
        assert(isinstance(arg1,Unlocker))
        obj =  Unlocker\
            ( native.Unlocker_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Isolate            
        
        Signature[2]: 
           args[0]:POINTER(v8.Unlocker)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Unlocker_copy(args[0])
        else:
            
            try:
                self._cobject = native.Unlocker_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Unlocker_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Unlocker(native.Unlocker_copy( self._cobject ))

    def __del__(self):
        native.Unlocker_delete(self._cobject)
        self._cobjet = None
            
    

    def Initialize(self, isolate,):
        '''
        @returns: void
        '''
        assert(isinstance(isolate,Isolate))
        return native.v8___Unlocker__Initialize(self._cobject, isolate,)

    def assign(self, arg1,):
        '''
        @returns: v8.Unlocker
        '''
        assert(isinstance(arg1,POINTER(Unlocker)))
        return (native.v8___Unlocker__assign(self._cobject, arg1._cobject,))        

class Locker( _FwdDecl_Locker ):

    @staticmethod
    def new1(self, isolate,):
        """
        Signature: 
           isolate:v8.Isolate            
        """
        assert(isinstance(isolate,Locker))
        obj =  Locker\
            ( native.Locker_new(isolate, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           args[0]:v8.Isolate            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], c_void_p):
            self._cobject = native.Locker_copy(args[0])
        else:
            
            try:
                self._cobject = native.Locker_new1(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Locker(native.Locker_copy( self._cobject ))

    def __del__(self):
        native.Locker_delete(self._cobject)
        self._cobjet = None
            
    

    @staticmethod
    def IsLocked(isolate,):
        '''
        @returns: c_char
        '''
        assert(isinstance(isolate,Isolate))
        return native.v8___Locker__IsLocked(isolate,)

    @staticmethod
    def IsActive():
        '''
        @returns: c_char
        '''
        return native.v8___Locker__IsActive()

    def Initialize(self, isolate,):
        '''
        @returns: void
        '''
        assert(isinstance(isolate,Isolate))
        return native.v8___Locker__Initialize(self._cobject, isolate,)

    def assign(self, arg1,):
        '''
        @returns: void
        '''
        assert(isinstance(arg1,POINTER(Locker)))
        return native.v8___Locker__assign(self._cobject, arg1._cobject,)
