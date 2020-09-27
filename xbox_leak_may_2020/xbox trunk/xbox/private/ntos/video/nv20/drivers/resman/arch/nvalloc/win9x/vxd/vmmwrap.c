//
// (C) Copyright NVIDIA Corporation Inc., 1997. All rights reserved.
//
//	VMM Call functions
//	These are wrappers for VMM services (a few other VxD services too)
//  Some other services are defined as macros in VMM2.H

#include "basedef.h"
#include "vmmtypes.h"
#include "vmm.h"
#include "vpicd.h"
#include "vdd.h"
// NVAUDIO does not want to include all
// kernel header files that nvrm.h includes
#if !defined NVAUDIO
#include <nvrm.h>
#include "nvrmwin.h"
#include "vnvrmd.h"
#include "oswin.h"
#else
#include <nvtypes.h>
#endif
#include "vmm2.h"		// more vmm services
//
// pragmas
//
#pragma warning(disable : 4035)

//	return 2 DWORDS of descriptor in address passed as argument	
VOID vmmBuildDescriptorDWORDs(U032 DESCBase, U032 DESCLimit, U032 DESCType, U032 DESCSize, U032 Flags, QUADWORD *DescQWORD)
{
			__asm {
				mov		eax, Flags
				push	eax
				mov		eax, DESCSize
				push	eax
				mov		eax, DESCType
				push	eax
				mov		eax, DESCLimit
				push	eax
				mov		eax, DESCBase
				push	eax
			}
	        CALL_VMM(BuildDescriptorDWORDs);
			// return descriptor via pointer DescQWORD
			__asm {
				push	esi
                mov		esi,DescQWORD
				mov		[esi], eax
				mov		[esi+4], edx
                pop		esi
				add		esp,5*4
			}
}

U032 vmmFreeLDTSelector(U032 vmh, U032 Selector, U032 Flags)
{
			__asm {
            	mov		eax, Flags		
				push	eax				
				mov		eax, Selector	
				push	eax				
				mov		eax, vmh		
				push	eax				
			}
	        CALL_VMM(Free_LDT_Selector)     
			_asm add	esp,3*4
}

U032 vmmAllocateGDTSelector(U032 DescDWORD1, U032 DescDWORD2, U032 Flags)
{
			__asm {
            	push	edx		
				mov		eax, Flags	
				push	eax		
			 	mov		eax, DescDWORD2	
			 	push 	eax		
			 	mov 	eax, DescDWORD1	
			 	push 	eax		
			}
	        CALL_VMM(Allocate_GDT_Selector) 
            __asm {
				add		esp,3*4
				pop		edx
            }
// return:  EAX = new selector 
//			EDX low =  GDT selector
//			EDX high = number of selectors in GDT
//			we don't use edx, so we push and pop to preserve it for the invoking code
}

U032 vmmAllocateLDTSelector(U032 vmh, U032 DescDWORD1, U032 DescDWORD2, U032 Count, U032 Flags)
{
			__asm {
            	push	edx				
				mov		eax, Flags		
				push	eax				
				mov		eax, Count		
				push	eax				
				mov		eax, DescDWORD2	
				push	eax				
				mov		eax, DescDWORD1	
				push	eax				
				mov		eax, vmh
                push	eax
			}
            CALL_VMM(Allocate_LDT_Selector) 
			__asm {
				add		esp, 5*4
				pop		edx		
			}
// return:  EAX = new selector 
//			EDX low =  GDT selector
//			EDX high = number of selectors in GDT
//			we don't use edx (or eax) so we push and pop to preserve it for the invoking code
}           


//	the only reason this needs to be a function is so it can be used as an expression
U032 vmmGetSystemTime()
{
            CALL_VMM(Get_System_Time)
}

U032 vmmCallPriorityVMEvent(U032 PriorityBoost, U032 vmh, U032 flags, U032 RefData, U032 Callback, U032 Timeout)
{
			_asm   mov eax, PriorityBoost 
			_asm   mov ebx, vmh
			_asm   mov ecx, flags		   
			_asm   mov edx, RefData
			_asm   mov esi, Callback
			_asm   mov edi, Timeout
			CALL_VMM(Call_Priority_VM_Event)	
			_asm   mov eax, esi
}


VOID vmmSimulatePush(U032 Value)
{
			_asm	mov eax, Value
            CALL_VMM(Simulate_Push)  
}

VOID vmmSimulateFarCall(U032 Segment, U032 Off)
{
			_asm	mov ecx, Segment
			_asm	mov edx, Off
			CALL_VMM(Simulate_Far_Call)
}

U032 vmmPageCheckLinRange(U032 PageBase, U032 PageCount, U032 flags) 
{
			_asm	mov	eax, flags		
			_asm 	push	eax			
			_asm	mov	eax, PageCount	
			_asm	push	eax			
			_asm	mov	eax, PageBase	
			_asm	push	eax			
            CALL_VMM(_PageCheckLinRange)           
			_asm 	add	esp,3*4
}

U032 vmmLinPageLock(U032 HLinPgNum, U032 nPages, U032 flags)
{
			_asm	mov	eax, flags		
			_asm 	push	eax			
			_asm	mov	eax, nPages		
			_asm	push	eax			
			_asm	mov	eax, HLinPgNum	
			_asm	push	eax			
            CALL_VMM(_LinPageLock)                  
			_asm 	add	esp,3*4
}

U032 vmmLinPageUnLock(U032 HLinPgNum, U032 nPages, U032 flags)
{
			_asm	mov	eax, flags		
			_asm 	push	eax			
			_asm	mov	eax, nPages		
			_asm	push	eax			
			_asm	mov	eax, HLinPgNum	
			_asm	push	eax			
            CALL_VMM(_LinPageUnLock)               
			_asm 	add	esp,3*4
}

U032 vmmPageReserve(U032 uPage, U032 nPages, U032 flags)
{
			_asm	mov	eax, flags		
			_asm 	push	eax			
			_asm	mov	eax, nPages		
			_asm	push	eax			
			_asm	mov	eax, uPage	
			_asm	push	eax			
            CALL_VMM(_PageReserve)               
			_asm 	add	esp,3*4
}

U032 vmmPageCommitPhys(U032 uLinPage, U032 nPages, U032 uPhysPage, U032 flags)
{
			_asm	mov	eax, flags		
			_asm 	push	eax			
			_asm	mov	eax, uPhysPage		
			_asm 	push	eax			
			_asm	mov	eax, nPages		
			_asm	push	eax			
			_asm	mov	eax, uLinPage	
			_asm	push	eax			
            CALL_VMM(_PageCommitPhys)               
			_asm 	add	esp,4*4
}

U032 vmmPageDecommit(U032 uLinPage, U032 nPages, U032 flags)
{
			_asm	mov	eax, flags		
			_asm 	push	eax			
			_asm	mov	eax, nPages		
			_asm	push	eax			
			_asm	mov	eax, uLinPage	
			_asm	push	eax			
            CALL_VMM(_PageDecommit)               
			_asm 	add	esp,3*4
}


VOID vmmPageAllocate(U032 nPages, 
			U032 pType, 
            U032 VM, 
            U032 AlignMask, 
			U032 minPhys, 
            U032 maxPhys, 
            U032 PhysAddr, 
            U032 flags, 
            QUADWORD *Desc)
{
			_asm {
				mov		eax,flags
                push	eax
				mov		eax,PhysAddr
                push	eax
				mov		eax,maxPhys
                push	eax
				mov		eax,minPhys
                push	eax
				mov		eax,AlignMask
                push	eax
				mov		eax,VM
                push	eax
				mov		eax,pType
                push	eax
				mov		eax,nPages
                push	eax
            }
            CALL_VMM(_PageAllocate)                 \
			_asm {
            	push	esi
                mov		esi, Desc
            	mov		[esi], eax
            	mov		[esi+4], edx
				pop		esi
			 	add		esp,8*4
			}
}

VOID vmmPageGetSizeAddr(U032 hMem, U032 flags, QUADWORD *desc64)
{
			_asm {
				mov		eax, flags		
				push	eax				
				mov		eax, hMem		
				push	eax				
			}
            CALL_VMM(_PageGetSizeAddr)
			_asm {
				push	esi
                mov		esi, desc64
            	mov		[esi], eax
                mov		[esi+4], edx
                pop		esi
				add		esp,2*4
			}
}

U032 vmmPageLock(U032 hMem, U032 nPages, U032 PageOff, U032 flags)
{
			_asm	mov		eax, flags		
			_asm	push	eax				
			_asm	mov		eax, PageOff	
			_asm	push	eax				
			_asm	mov		eax, nPages		
			_asm	push	eax				
			_asm	mov		eax, hMem		
			_asm	push	eax				
            CALL_VMM(_PageLock)                    
			_asm 	add		esp,4*4
}

U032 vmmPageUnLock(U032 hMem, U032 nPages, U032 PageOff, U032 flags) 
{
			_asm 	mov		eax, flags		
			_asm 	push	eax				
			_asm 	mov		eax, PageOff	
			_asm 	push	eax				
			_asm 	mov		eax, nPages		
			_asm 	push	eax				
			_asm 	mov		eax, hMem		
			_asm 	push	eax				
            CALL_VMM(_PageUnLock)                   
			_asm 	add 	esp,4*4
}

U032 vmmPageFree(U032 hMem, U032 flags)
{
			_asm	 mov	eax, flags		
			_asm	 push	eax				
			_asm	 mov	eax, hMem		
			_asm	 push	eax				
            CALL_VMM(_PageFree)  	
			_asm add esp,2*4
}

VOID vmmGetDescriptor(U032 Selector, U032 VM, U032 flags, QUADWORD *Desc)
{
			_asm {
            	mov		eax, flags		
			 	push	eax				
			 	mov		eax, VM			
			 	push	eax				
			 	mov		eax, Selector	
			 	push	eax				
			}
            CALL_VMM(_GetDescriptor)
			_asm {
            	push	esi
                mov		esi, Desc
			 	mov		[esi], eax	
			 	mov		[esi+4], edx
				pop		esi
			 	add		esp,3*4
			}
}

U032 vmmSetDescriptor(U032 Selector, U032 VM, U032 DescDWORD1, U032 DescDWORD2, U032 flags)
{
			_asm {
            	mov		eax, flags		
			 	push	eax				
			 	mov		eax, DescDWORD2	
			 	push	eax				
			 	mov		eax, DescDWORD1	
			 	push	eax				
			 	mov		eax, VM			
			 	push	eax				
			 	mov		eax, Selector	
			 	push	eax				
			}
            CALL_VMM(_SetDescriptor)
			_asm add	esp,5*4
}

U032 vmmSelectorMapFlat(U032 VMh, U032 Selector, U032 flags)
{
			_asm {
            	mov		eax, flags
			 	push	eax				
			 	mov		eax, Selector	
			 	push	eax				
			 	mov		eax, VMh
			 	push	eax				
			}
            CALL_VMM(_SelectorMapFlat)              
			_asm add	esp,3*4
}

U032 vmmGetSysVMHandle()
{
			CALL_VMM(Get_Sys_VM_Handle)
			_asm	mov		eax, ebx
}

U032 vmmGetCritSectionStatus(VOID)
{
			CALL_VMM(Get_Crit_Section_Status)
			_asm	mov		eax, ecx		// return # of times crit section claimed
}

U032 vmmCopyPageTable(U032 LinPgNum, U032 nPages, U032 PageBuf, U032 flags)
{
			_asm {
            	mov		eax, flags
                push	eax
                mov		eax, PageBuf
                push	eax
                mov		eax, nPages
                push	eax
                mov		eax, LinPgNum
                push	eax
              }
            CALL_VMM(_CopyPageTable)                
			_asm add	esp,4*4
}

U032 vmmMapPhysToLinear(U032 PhysAddr, U032 nBytes, U032 flags)
{
			_asm {
            	mov		eax, flags
                push	eax
                mov		eax, nBytes
                push	eax
                mov		eax, PhysAddr
                push	eax
			}
            CALL_VMM(_MapPhysToLinear)              
			_asm add	esp,3*4
}

U032 vmmGetCurrentContext(VOID)
{
            CALL_VMM(_GetCurrentContext)           
}

U032 vmmContextSwitch(U032 Context)
{
            _asm	mov		eax, Context
            _asm 	push	eax
            CALL_VMM(_ContextSwitch)
            _asm 	add	esp,4
}

U032 vmmGetProfileHexInt(U032 Default, char *Profile, char *Keyname)
{
			_asm {
                mov		eax, Default
                mov		esi, Profile
            	mov		edi, Keyname
			}
			CALL_VMM(Get_Profile_Hex_Int)
}            

U032 vmmGetProfileDecimalInt(U032 Default, char *Profile, char *Keyname)
{
			_asm {
                mov		eax, Default
                mov		esi, Profile
            	mov		edi, Keyname
			}
            CALL_VMM(Get_Profile_Decimal_Int)       
}

VOID vmmFatalErrorHandler(U032 MsgPtr, U032 ErrFlags)
{
			_asm 	mov		eax, ErrFlags		
			_asm 	mov		esi, MsgPtr	
            CALL_VMM(Fatal_Error_Handler)           
}

VOID vmmCallBackExit(U032 flag)
{
			_asm	mov		eax, flag	
            _asm 	stc                
            _asm 	or		eax, eax   
            _asm	jnz    cbExit      
            _asm	clc                
            _asm	cbExit:            
}

VOID vmmCallBackEntry(VOID)
{
}

VOID vmmSaveClientState(CRS *Buffer)		
{
			_asm	mov		edi, Buffer	
            CALL_VMM(Save_Client_State)             
}

VOID vmmRestoreClientState(CRS *Buffer)   
{
			_asm	mov		esi, Buffer	
            CALL_VMM(Restore_Client_State)          
}

U032 vmmYield(U032 timeout)
{
            _asm mov eax, timeout
            CALL_VMM(Time_Slice_Sleep);
}

U032 vmmCreateSemaphore(U032 TokenCount)	
{
			_asm	mov	ecx, TokenCount	
            CALL_VMM(Create_Semaphore)   
}

U032 vmmWaitSemaphore(U032 Semaphore, U032 Flags)	
{
			_asm	mov eax, Semaphore	
			_asm   mov ecx, Flags	
            CALL_VMM(Wait_Semaphore) 
}			

U032 vmmSignalSemaphore(U032 Semaphore)	
{
			_asm	mov	eax, Semaphore
            CALL_VMM(Signal_Semaphore)   
}			

U032 vmmCreateMutex(U032 Boost, U032 Flags)	
{
			_asm	mov	eax, Flags		
			_asm 	push	eax			
			_asm	mov	eax, Boost		
			_asm	push	eax			
            CALL_VMM(_CreateMutex)                  
			_asm 	add	esp,2*4
}

U032 vmmEnterMutex(U032 Mutex, U032 Flags)	
{
			_asm	mov	eax, Flags		
			_asm 	push	eax			
			_asm	mov	eax, Mutex		
			_asm	push	eax			
            CALL_VMM(_EnterMutex)                  
			_asm 	add	esp,2*4
}			

U032 vmmLeaveMutex(U032 Mutex)	
{
			_asm	mov	eax, Mutex		
			_asm	push	eax			
            CALL_VMM(_LeaveMutex)                  
			_asm 	add	esp,1*4
}			

U032 picGetIRQVirtualizedStatus(U032 IRQnum, U032 VMh)
{
            //
            // We need to preserve the CY flag, and if we use
            // the VPICD_Get() function the compiler will
            // readjust the stack upon return, therefore
            // destroying the CY flag.  So, let's just
            // call VPICD directly.
            //
            //VPICD_Get_IRQ_Complete_Status(IRQnum, VMh);
            __asm xor ecx,ecx
            __asm mov eax,IRQnum
            VxDCall(VPICD_Get_IRQ_Complete_Status)
			_asm	sbb		eax, eax				// set return value to carry value        
        
}

U032 vmmGARTReserve(U032 pDevObj, U032 ulNumPages, U032 ulAlignMask, U032 pulGARTDev, U032 ulFlags)
{
			_asm {
			        push	ulFlags
			        push	pulGARTDev			
			        push	ulAlignMask			
			        push	ulNumPages			
			        push	pDevObj
            }                    			
            CALL_VMM(_GARTReserve)                  
			_asm 	add	esp,5*4
}

U032 vmmGARTCommit(U032 pGARTLin, U032 ulPageOffset, U032 ulNumPages, U032 pulGARTDev, U032 ulFlags)
{
			_asm {
			        push	ulFlags
			        push	pulGARTDev			
			        push	ulNumPages			
			        push	ulPageOffset			
			        push	pGARTLin
            }                    			
            CALL_VMM(_GARTCommit)                  
			_asm 	add	esp,5*4
}

U032 vmmGARTUnCommit(U032 pGARTLin, U032 ulPageOffset, U032 ulNumPages)
{
			_asm {
			        push	ulNumPages			
			        push	ulPageOffset			
			        push	pGARTLin
            }                    			
            CALL_VMM(_GARTUncommit)                  
			_asm 	add	esp,3*4
}

U032 vmmGARTFree(U032 pGARTLin)
{
			_asm {
			        push	pGARTLin
            }                    			
            CALL_VMM(_GARTFree)                  
			_asm 	add	esp,1*4
}

U032 vmmGARTMemAttributes(U032 pGARTLin, U032 ulFlags)
{
			_asm {
			        push	ulFlags
			        push	pGARTLin
            }                    			
            CALL_VMM(_GARTMemAttributes)                  
			_asm 	add	esp,2*4
}

U032 mtrrGetVersion(VOID)
{
            CALL_VMM(_MTRR_Get_Version) 
}


// called as        mtrrSetPhysicalCacheTypeRange(nvInfo->Mapping.PhysFbAddr, 0, 0x400000, 0);
U032 mtrrSetPhysicalCacheTypeRange(U032 arg1, U032 arg2, U032 arg3, U032 arg4)
{
			_asm {
            	push	arg4
                push	arg3
                push	arg2
                push	arg1
			}
            CALL_VMM(_MTRRSetPhysicalCacheTypeRange)           
//			_asm	add	esp,16		// the callee adjusted the stack
}

U016 vddGetVersion(VOID)
{
            CALL_VDD(VDD_Get_Version)        
}

U032 vddGetDisplayInfo(U032 OsDeviceHandle, PDISPLAYINFO Info, U032 infoSize)
{
			_asm	mov		eax, Info
            _asm    mov     ebx, OsDeviceHandle
            _asm	mov		ecx, infoSize
            CALL_VDD(VDD_Get_DISPLAYINFO)    
}

//	write null terminated string to the debugging device
VOID vmmOutDebugString(char *String)	
{
		_asm	pushfd				
		_asm	pushad				
		_asm 	mov esi, String		
		CALL_VMM(Out_Debug_String)	
		_asm	popad				
		_asm	popfd
}

//	write null terminated string to the debugging device
//	the string contains a register placeholder, so the actual register value will be printed also
VOID vmmOutDebugStringValue(char *String, U032 Value)	
{
		_asm	mov eax, Value
		_asm	pushfd				
		_asm	pushad				
		_asm 	mov esi, String		
		CALL_VMM(Out_Debug_String)	
		_asm	popad				
		_asm	popfd
}

VOID	dbgBreakPoint()
{
	_asm	int	3
}

#ifdef DEBUG
// Test if the debugger is installed.
U032 vmmTestDebugInstalled(void)	
{
  U032 return_val;
          
  CALL_VMM(Test_Debug_Installed)
  _asm  mov    return_val,0  
  _asm	jz     installedExit      
  _asm  mov    return_val,1  
  _asm	installedExit:     
  return(return_val);       
}
#endif
