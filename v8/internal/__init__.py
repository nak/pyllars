from ctypes import *

try:
    native = CDLL("")
except:
    native = None
    
def init( libname ):
    global native
    native = CDLL(libname)
from :: import uint32_t, int32_t, size_t, int64_t, uintptr_t, uint16_t, uint64_t, intptr_t, uint8_t

        

class Arguments( object ):
    
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(*args, c_void_p):
            self._cobject = (*args)[0]
        else:
            
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Arguments(native.Arguments_copy( self._cobject ))

    def __del__(self)
        native.Arguments_delete(self._cobject)
        self._cobjet = None
            
            

class Heap( object ):
    
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(*args, c_void_p):
            self._cobject = (*args)[0]
        else:
            
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Heap(native.Heap_copy( self._cobject ))

    def __del__(self)
        native.Heap_delete(self._cobject)
        self._cobjet = None
            
            

class HeapObject( object ):
    
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(*args, c_void_p):
            self._cobject = (*args)[0]
        else:
            
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return HeapObject(native.HeapObject_copy( self._cobject ))

    def __del__(self)
        native.HeapObject_delete(self._cobject)
        self._cobjet = None
            
            

class Isolate( object ):
    
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(*args, c_void_p):
            self._cobject = (*args)[0]
        else:
            
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Isolate(native.Isolate_copy( self._cobject ))

    def __del__(self)
        native.Isolate_delete(self._cobject)
        self._cobjet = None
            
            

class Object( object ):
    
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(*args, c_void_p):
            self._cobject = (*args)[0]
        else:
            
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Object(native.Object_copy( self._cobject ))

    def __del__(self)
        native.Object_delete(self._cobject)
        self._cobjet = None
            
    

class StreamedSource( Structure):
    _fields_ = [ ]



        

class PropertyCallbackArguments( object ):
    
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(*args, c_void_p):
            self._cobject = (*args)[0]
        else:
            
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return PropertyCallbackArguments(native.PropertyCallbackArguments_copy( self._cobject ))

    def __del__(self)
        native.PropertyCallbackArguments_delete(self._cobject)
        self._cobjet = None
            
            

class FunctionCallbackArguments( object ):
    
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(*args, c_void_p):
            self._cobject = (*args)[0]
        else:
            
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return FunctionCallbackArguments(native.FunctionCallbackArguments_copy( self._cobject ))

    def __del__(self)
        native.FunctionCallbackArguments_delete(self._cobject)
        self._cobjet = None
            
            

class GlobalHandles( object ):
    
    def __init__(self,*args):
        """"""
        self._cobject = None
        if len(*args) == 1 and isinstance(*args, c_void_p):
            self._cobject = (*args)[0]
        else:
            
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return GlobalHandles(native.GlobalHandles_copy( self._cobject ))

    def __del__(self)
        native.GlobalHandles_delete(self._cobject)
        self._cobjet = None
            
    

class SmiTagging<4>( Structure):
    _fields_ = [ ]




    def SmiShiftSize(self, ):
        return native.v8internal___SmiTagging<4>__SmiShiftSize(self._cobject, )
        
    def SmiValueSize(self, ):
        return native.v8internal___SmiTagging<4>__SmiValueSize(self._cobject, )
        
    def SmiToInt(self, value,):
        assert(isinstance(value,POINTER(v8.internal.Object))
        return native.v8internal___SmiTagging<4>__SmiToInt(self._cobject, value,)
        
    def IntToSmi(self, value,):
        assert(isinstance(value,c_int)
        return native.v8internal___SmiTagging<4>__IntToSmi(self._cobject, value,)
        
    def IsValidSmi(self, value,):
        assert(isinstance(value,intptr_t)
        return native.v8internal___SmiTagging<4>__IsValidSmi(self._cobject, value,)
        


class SmiTagging<8>( Structure):
    _fields_ = [ ]




    def SmiShiftSize(self, ):
        return native.v8internal___SmiTagging<8>__SmiShiftSize(self._cobject, )
        
    def SmiValueSize(self, ):
        return native.v8internal___SmiTagging<8>__SmiValueSize(self._cobject, )
        
    def SmiToInt(self, value,):
        assert(isinstance(value,POINTER(v8.internal.Object))
        return native.v8internal___SmiTagging<8>__SmiToInt(self._cobject, value,)
        
    def IntToSmi(self, value,):
        assert(isinstance(value,c_int)
        return native.v8internal___SmiTagging<8>__IntToSmi(self._cobject, value,)
        
    def IsValidSmi(self, value,):
        assert(isinstance(value,intptr_t)
        return native.v8internal___SmiTagging<8>__IsValidSmi(self._cobject, value,)
        

#typedef
PlatformSmiTagging = SmiTagging_8_        

class Internals( object ):
    

    @staticmethod
    def new1(self, ):
        """
        Signature: """
        obj =  Internals\
            ( native.Internals_new())
        return obj
        

    @staticmethod
    def new2(self, arg1,):
        """
        Signature: 
           arg1:POINTER(v8.internal.Internals)            
        """
        assert(isinstance(arg1,Internals))
        obj =  Internals\
            ( native.Internals_new(arg1._cobject, ))
        return obj
        
    def __init__(self,*args):
        """
        Signature[1]: 
           No arguments
        Signature[2]: 
           args[0]:POINTER(v8.internal.Internals)            
        """
        self._cobject = None
        if len(*args) == 1 and isinstance(*args, c_void_p):
            self._cobject = (*args)[0]
        else:
            
            try:
                self._cobject = native.Internals_new1(*args)
            except:
                pass
        
            try:
                self._cobject = native.Internals_new2(*args)
            except:
                pass
        
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return Internals(native.Internals_copy( self._cobject ))

    def __del__(self)
        native.Internals_delete(self._cobject)
        self._cobjet = None
            
    
    def CheckInitializedImpl(self, isolate,):
        assert(isinstance(isolate,POINTER(v8.Isolate))
        return native.v8internal___Internals__CheckInitializedImpl(self._cobject, isolate,)
        

    def CheckInitialized(self, isolate,):
        assert(isinstance(isolate,POINTER(v8.Isolate))
        return native.v8internal___Internals__CheckInitialized(self._cobject, isolate,)
        

    def HasHeapObjectTag(self, value,):
        assert(isinstance(value,POINTER(v8.internal.Object))
        return native.v8internal___Internals__HasHeapObjectTag(self._cobject, value,)
        
    def SmiValue(self, value,):
        assert(isinstance(value,POINTER(v8.internal.Object))
        return native.v8internal___Internals__SmiValue(self._cobject, value,)
        
    def IntToSmi(self, value,):
        assert(isinstance(value,c_int)
        return native.v8internal___Internals__IntToSmi(self._cobject, value,)
        
    def IsValidSmi(self, value,):
        assert(isinstance(value,intptr_t)
        return native.v8internal___Internals__IsValidSmi(self._cobject, value,)
        

    def GetInstanceType(self, obj,):
        assert(isinstance(obj,POINTER(v8.internal.Object))
        return native.v8internal___Internals__GetInstanceType(self._cobject, obj,)
        
    def GetOddballKind(self, obj,):
        assert(isinstance(obj,POINTER(v8.internal.Object))
        return native.v8internal___Internals__GetOddballKind(self._cobject, obj,)
        
    def IsExternalTwoByteString(self, instance_type,):
        assert(isinstance(instance_type,c_int)
        return native.v8internal___Internals__IsExternalTwoByteString(self._cobject, instance_type,)
        
    def GetNodeFlag(self, obj,shift,):
        assert(isinstance(obj,POINTER(POINTER(v8.internal.Object)))
        assert(isinstance(shift,c_int)
        return native.v8internal___Internals__GetNodeFlag(self._cobject, obj,shift,)
        
    def UpdateNodeFlag(self, obj,value,shift,):
        assert(isinstance(obj,POINTER(POINTER(v8.internal.Object)))
        assert(isinstance(value,c_char)
        assert(isinstance(shift,c_int)
        return native.v8internal___Internals__UpdateNodeFlag(self._cobject, obj,value,shift,)
        
    def GetNodeState(self, obj,):
        assert(isinstance(obj,POINTER(POINTER(v8.internal.Object)))
        return native.v8internal___Internals__GetNodeState(self._cobject, obj,)
        
    def UpdateNodeState(self, obj,value,):
        assert(isinstance(obj,POINTER(POINTER(v8.internal.Object)))
        assert(isinstance(value,uint8_t)
        return native.v8internal___Internals__UpdateNodeState(self._cobject, obj,value,)
        

    def SetEmbedderData(self, isolate,slot,data,):
        assert(isinstance(isolate,POINTER(v8.Isolate))
        assert(isinstance(slot,uint32_t)
        assert(isinstance(data,c_void_p)
        return native.v8internal___Internals__SetEmbedderData(self._cobject, isolate,slot,data,)
        


    def GetEmbedderData(self, isolate,slot,):
        assert(isinstance(isolate,POINTER(v8.Isolate))
        assert(isinstance(slot,uint32_t)
        return native.v8internal___Internals__GetEmbedderData(self._cobject, isolate,slot,)
        


    def GetRoot(self, isolate,index,):
        assert(isinstance(isolate,POINTER(v8.Isolate))
        assert(isinstance(index,c_int)
        return native.v8internal___Internals__GetRoot(self._cobject, isolate,index,)
        

    def ReadField_17(self, ptr,offset,):
        assert(isinstance(ptr,POINTER(v8.internal.Object))
        assert(isinstance(offset,c_int)
        return native.v8internal___Internals__ReadField(self._cobject, ptr,offset,)
        
    def ReadField_18(self, ptr,offset,):
        assert(isinstance(ptr,POINTER(v8.internal.Object))
        assert(isinstance(offset,c_int)
        return native.v8internal___Internals__ReadField(self._cobject, ptr,offset,)
        
    def ReadField_19(self, ptr,offset,):
        assert(isinstance(ptr,POINTER(v8.internal.Object))
        assert(isinstance(offset,c_int)
        return native.v8internal___Internals__ReadField(self._cobject, ptr,offset,)
        
    def ReadEmbedderData_20(self, context,index,):
        assert(isinstance(context,POINTER(v8.Context))
        assert(isinstance(index,c_int)
        return native.v8internal___Internals__ReadEmbedderData(self._cobject, context,index,)
        

    def ReadEmbedderData_21(self, context,index,):
        assert(isinstance(context,POINTER(v8.Context))
        assert(isinstance(index,c_int)
        return native.v8internal___Internals__ReadEmbedderData(self._cobject, context,index,)
        

    def assign(self, arg1,):
        assert(isinstance(arg1,POINTER(v8.internal.Internals))
        return native.v8internal___Internals__assign(self._cobject, arg1._cobject,)
        
