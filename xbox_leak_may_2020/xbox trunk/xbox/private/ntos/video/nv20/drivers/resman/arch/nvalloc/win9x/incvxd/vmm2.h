//
// VMM2.H
// VMM Call Macros and function prototypes for functions not defined in VMM.H
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.


//
//	Function Prototypes
//
VOID vmmOutDebugString(char *);

VOID vmmOutDebugStringValue(char *, U032);	

VOID	dbgBreakPoint();

U032 vmmAllocateGDTSelector(U032, U032, U032);

U032 vmmAllocateLDTSelector(U032, U032, U032, U032, U032);

VOID vmmBuildDescriptorDWORDs(U032, U032, U032, U032, U032, QUADWORD*);

U032 vmmFreeLDTSelector(U032, U032, U032);

U032 vmmGetSystemTime();

U032 vmmCallPriorityVMEvent(U032, U032, U032, U032, U032, U032);

VOID vmmSimulatePush(U032);

VOID vmmSimulateFarCall(U032, U032);

VOID vmmGetDescriptor(U032, U032, U032, QUADWORD *);

U032 vmmPageCheckLinRange(U032, U032, U032);

U032 vmmLinPageLock(U032, U032, U032);    

U032 vmmLinPageUnLock(U032, U032, U032);

VOID vmmPageAllocate(U032, U032, U032, U032, U032, U032, U032, U032, QUADWORD *);

U032 vmmPageReserve(U032, U032, U032);

U032 vmmPageCommitPhys(U032, U032, U032, U032);

U032 vmmPageDecommit(U032, U032, U032);

VOID vmmPageGetSizeAddr(U032, U032, QUADWORD *);

U032 vmmPageLock(U032, U032, U032, U032);

U032 vmmPageUnLock(U032, U032, U032, U032);

U032 vmmPageFree(U032, U032);

U032 vmmSetDescriptor(U032, U032, U032, U032, U032);

U032 vmmSelectorMapFlat(U032, U032, U032);

U032 vmmGetSysVMHandle(VOID);

U032 vmmGetCritSectionStatus(VOID);

U032 vmmCopyPageTable(U032, U032, U032, U032);

U032 vmmMapPhysToLinear(U032, U032, U032);

U032 vmmGetCurrentContext(VOID);

U032 vmmContextSwitch(U032);

U032 vmmGetProfileHexInt(U032, char *, char *);

U032 vmmGetProfileDecimalInt(U032, char *, char *);

U032 vmmGetSysVMHandle(VOID);

VOID vmmFatalErrorHandler(U032, U032);

VOID vmmCallBackExit(U032);

VOID vmmCallBackEntry(VOID);

VOID vmmRestoreClientState(CRS *);

VOID vmmSaveClientState(CRS *);	

U032 vmmCreateSemaphore(U032);

U032 vmmWaitSemaphore(U032, U032);	

U032 vmmSignalSemaphore(U032);

U032 picGetIRQVirtualizedStatus(U032, U032);

U032 mtrrGetVersion(VOID);

U032 mtrrSetPhysicalCacheTypeRange(U032, U032, U032, U032);

U016 vddGetVersion(VOID);

U032 vddGetDisplayInfo(U032, PDISPLAYINFO, U032);


// This is just like the macro VMMCall in VMM.H, except it doesn't use the GetVxDServiceOrdinal macro, which gets the 
// service number from the service table, which is generated on the fly. We would have to alter the service table macro
// to add more services for that to work.
// the "service" upper word is the device, the lower word is the service
#define CALL_VMM(service) \
    _asm _emit 0xcd \
    _asm _emit 0x20 \
    _asm _emit (service & 0xff) \
    _asm _emit (service >> 8) & 0xff \
    _asm _emit (service >> 16) & 0xff \
    _asm _emit (service >> 24) & 0xff 

#define VDD_Device_ID          0x0000A
//	Macro for VDD services. Just like CALL_VMM above, except substitue the VDD device number
#define CALL_VDD(service) \
    _asm _emit 0xcd \
    _asm _emit 0x20 \
    _asm _emit (service & 0xff) \
    _asm _emit (service >> 8) & 0xff \
    _asm _emit (VDD_Device_ID) & 0xff \
    _asm _emit (VDD_Device_ID >> 8) & 0xff 


//	Defines for service numbers. These are services that Microsoft hasn't bothered to define a C-callable service table for.
#define Get_Sys_VM_Handle                   0x00010003
#define Call_Priority_VM_Event              0x00010014
#define Create_Semaphore                    0x00010025
#define Wait_Semaphore                      0x00010027
#define Signal_Semaphore                    0x00010028
#define Get_Crit_Section_Status             0x00010029
#define Get_System_Time                     0x0001003f
#define Simulate_Far_Call                   0x00010048
#define Simulate_Push                       0x0001004d
#define Simulate_Pop                        0x0001004e
#define _PageFree                           0x00010055
#define _PageLock                           0x00010056
#define _PageUnLock                         0x00010057
#define _PageGetSizeAddr                    0x00010058
#define _PageAllocate                       0x00010053
#define _CopyPageTable                      0x00010061
#define _LinPageLock                        0x00010063
#define _LinPageUnLock                      0x00010064
#define _PageCheckLinRange                  0x00010067
#define _MapPhysToLinear                    0x0001006c
#define _SelectorMapFlat                    0x0001006e
#define Allocate_GDT_Selector              0x00010076
#define Allocate_LDT_Selector              0x00010078
#define Free_LDT_Selector                  0x00010079
#define BuildDescriptorDWORDs              0x0001007a
#define _GetDescriptor                      0x0001007b
#define _SetDescriptor                      0x0001007c
#define End_Nest_Exec                       0x00010086
#define Save_Client_State                   0x0001008d
#define Restore_Client_State                0x0001008e
#define Fatal_Error_Handler                 0x000100be
#define Begin_Nest_Exec                     0x00010083
#define Resume_Exec                         0x00010085
#define Get_Profile_Decimal_Int             0x000100ab
#define Get_Profile_Hex_Int                 0x000100af
#define Test_Debug_Installed                0x000100c1
#define Out_Debug_String                    0x000100c2
#define _CreateMutex                        0x00010101
#define _DestroyMutex                       0x00010102
#define _PageReserve                        0x0001011d
#define _PageDecommit                       0x0001011f
#define _PageCommitPhys                     0x00010128
#define _EnterMutex                         0x0001012e
#define _LeaveMutex                         0x0001012f
#define _ContextSwitch                      0x00010132
#define _RegOpenKey                         0x00010148
#define _RegCloseKey                        0x00010149
#define _RegQueryValue                      0x0001014d
#define _GetCurrentContext                  0x00010157
#define _MTRR_Get_Version                   0x004A0000
#define _MTRRSetPhysicalCacheTypeRange      0x004A0001
#define Time_Slice_Sleep                    0x00010161
#define _GARTReserve                        0x000101AE
#define _GARTCommit                         0x000101AF
#define _GARTUncommit                       0x000101B0
#define _GARTFree                           0x000101B1
#define _GARTMemAttributes                  0x000101B2

// VDD services. (doesn't include device number)
#define VDD_Get_Version                     0x0000  // Get version number and ID string ptr
#define VDD_Get_DISPLAYINFO                 0x0014  //get DISPLAYINFO data structure


#define	PG_UNCACHED		    0x00000001		// Uncached memory
#define	PG_WRITECOMBINED	0x00000002		// Write combined memory
            
U032 vmmGARTReserve(U032, U032, U032, U032, U032);
U032 vmmGARTCommit(U032, U032, U032, U032, U032);
U032 vmmGARTUnCommit(U032, U032, U032);
U032 vmmGARTFree(U032);
U032 vmmGARTMemAttributes(U032, U032);

//	VMM Call Macros (inline functions)

#define vmmSimulatePop() CALL_VMM(Simulate_Pop)   

#define vmmBeginNestExecution() CALL_VMM(Begin_Nest_Exec);

#define vmmResumeExec() CALL_VMM(Resume_Exec);

#define vmmEndNestExecution() CALL_VMM(End_Nest_Exec);

#define osSwapStack(newStack, oldStack)	\
	_asm	mov		eax, newStack		\
	_asm	xchg	eax, esp			\
    _asm	mov		oldStack, eax


