#ifndef _OSWIN_H_
#define _OSWIN_H_
//
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
//
/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: OSWIN.H                                                           *
*       Operating system wrapper functions used to abstract the OS.         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       David Schmenk (dschmenk)    11/22/93 - wrote it.                    *
*       David Schmenk (dschmenk)    06/22/94 - rewrote it.                  *
*       Jeff Westerinen (jsw)    	05/05/97 - Modified to build under MS   *
*                                                                           *
\***************************************************************************/

//
// necessary definitions due to (missing in) new headers
//
#define CARRY       void
//
// necessary abstract types due to new headers (jsw)
// typedef struct VxD_Desc_Block DDB;
//
typedef CLIENT_STRUCT *PCS;
//
// IOCTL functions.
//
typedef VOID (*IOCTL_DISPATCH)(U032, DWORD, PVOID);

//---------------------------------------------------------------------------
//
//  OS specific defines.
//
//---------------------------------------------------------------------------

//
// Number of outstanding callbacks.
//
#define NUM_CALLBACKS                   64
#define MAX_PARAMS                      10
//
// Size of service stack.
//
#define STACK_SIZE                      8192

//---------------------------------------------------------------------------
//
//  OS specific structures.
//
//---------------------------------------------------------------------------

//
// Array of callback parameters.
//
typedef struct _def_callback
{
    U016 Selector;
    U016 Offset;
    U032 Count;
    U032 Params[MAX_PARAMS];
    struct _def_callback *Next;
} CALLBACK, *PCALLBACK;

//---------------------------------------------------------------------------
//
//  External variable definitions.
//
//---------------------------------------------------------------------------

// JJV-MM
//extern PCHANNELINFO channelInfo;
extern PCLIENTINFO    clientInfo;
// JJV-MM
//extern U032         hpicIRQ;
//extern IOCTL_DISPATCH rmArchDispatchTable[];
extern U032           rmInService;
extern U032           osInService;
extern U032           rmInSafeService;
extern U032           rmInCallback;
extern U032           rmSemaphore;
extern U032           osSemaphore;
extern U032           mmSemaphore;
extern U032         * rmStackBase;
extern U032         * rmStack;
extern U032           vmmStack;

// Registry string globals.
extern char strDevNodeRM[];
extern char strRegistryGlobalParameters[];
extern char strRmCoreName[];
extern char strRmCoreVxd[];

extern char strMaxRefreshRate[];
extern char strMaxOptimalRefreshRate[];
extern char strOptimalNotFromEDID[];
extern char strDMTOverride[];
extern char strMonitorTiming[];
extern char strCursorCacheOverride[];
extern char strDisplayType[];
extern char strDisplay2Type[];
extern char strTVtype[];
extern char strStartUp[];
extern char strTVOutType[];
extern char strNewModeSet[];
extern char strFpMode[];


//
// Callback functions into the 16 bit RM DLL.
//
// JJV-MM
//extern U032      dllNotifyOffset;
//extern U032      dllNotifySelector;
//extern U032      dllIpcOffset;
//extern U032      dllIpcSelector;
extern PCALLBACK osCallbackTable;
extern PCALLBACK osCallbackList;
extern PCALLBACK osCallbackFreeList;

//---------------------------------------------------------------------------
//
// Function prototypes.
//
//---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//
//  PCI BIOS interface.
//
VOID CDECL PCI(PCS);  // (jsw)

//
// VBE BIOS interface.
//
VOID CDECL VBE(PCS);  // (jsw)
VOID CDECL readToshibaEDID(PCS);  // (lpl)
VOID CDECL VBE30(PCS);  // (jsw)

//
//  Timer BIOS interface.
//
VOID CDECL TIMER(CLIENT_STRUCT *);


#ifdef __cplusplus
};
#endif

#define CLI	_asm cli

#define STI _asm sti

//
// Function prototypes for Resource Manager entrypoints.
//
RM_STATUS CDECL rmControl(U032, U032);
U032      CDECL rmGetVersion(VOID);
U032      CDECL rmSysRegister(U032, NVRMSYSAPI *);
U032      CDECL rmEnableVga(PHWINFO, BOOL);
U032      CDECL rmFixUpVga(VOID);
U032	  CDECL rmGetAddress(U032);
U032      CDECL rmEnableHiRes(PHWINFO, BOOL);
U032      CDECL rmSetDpmLevel(U032);
U032      CDECL rmDDC(U032 *);
RM_STATUS CDECL rmSysAPI(U032, U032, V032, V032);
RM_STATUS  __fastcall     rmUsrAPI(U032, PCS);
//RM_STATUS CDECL rmUsrAPI(U032, PCS);  // (jsw)
//VOID            rmUsrPrivAPI(U032, PCRS); (jsw)
S032            rmGetDevInfo(U032);
VOID            rmInterrupt(U032, U032);
VOID            osInterrupt(U032, U032);
U032            osInterruptProcess(PHWINFO *,U032, U032);
BOOL      CDECL osService(PHWINFO);
VOID      CDECL osBoostService(VOID);
VOID      CDECL osSafeService(VOID);
VOID            osVMIntEOI(U032, U032);
VOID            osNopInt(U032, U032);
RM_STATUS CDECL osScheduleCallback(PHWINFO,U016, U016, U032, U032, ...);
VOID            osCallback(U032);
//
// Initialization routines.
//
RM_STATUS initStack(VOID);
RM_STATUS initSemaphore(VOID);
RM_STATUS initCallbackFreeList(VOID);
RM_STATUS initClientInfo(VOID);
RM_STATUS initChannelInfo(VOID);
RM_STATUS initDevInfo(VOID);
RM_STATUS initNv(VOID);
//
// Stack switch routine.
//
U032 osSwapStack(U032);
/*** Watcom syntax...  (jsw)
#pragma aux osSwapStack   =                         \
            "xchg   EAX, ESP"                       \
            parm [EAX]                              \
            value [EAX];
*/

//
// Register push/pop routines.
//
/* (jsw)
VOID osPushRegs(VOID);
VOID osPopRegs(VOID);
#pragma aux osPushRegs    =                         \
            "push   EBP"                            \
            "push   EAX"                            \
            "push   EBX"                            \
            "push   ECX"                            \
            "push   EDX"                            \
            "push   ESI"                            \
            "push   EDI"                            \
            modify [];
#pragma aux osPopRegs    =                          \
            "pop    EDI"                            \
            "pop    ESI"                            \
            "pop    EDX"                            \
            "pop    ECX"                            \
            "pop    EBX"                            \
            "pop    EAX"                            \
            "pop    EBP"                            \
            modify [];
*/

//---------------------------------------------------------------------------
//
//  Compiler specific options are set here to interface to the VMM and other
//  VxDs in the system.  Different calling conventions are used for almost
//  every function.  Inlining of functions and assembly interface operations
//  are used extensively for performance and size considerations.
//
//---------------------------------------------------------------------------

//
// Handy functions used in this module.
//
/* (jsw)
DWORD asmGetCS(VOID);
#pragma aux asmGetCS =                              \
            "xor    EAX, EAX"                       \
            "mov    AX, CS"                         \
            value [EAX];
DWORD asmGetDS(VOID);
#pragma aux asmGetDS =                              \
            "xor    EAX, EAX"                       \
            "mov    AX, DS"                         \
            value [EAX];
*/

/*  Watcom syntax  (jsw)
VOID CLI(VOID);
#pragma aux CLI = "cli";
VOID STI(VOID);
#pragma aux STI = "sti";
*/
//VOID CLI(VOID);
//VOID STI(VOID);

//
// Parameter passing/return value mapping to entrypoints.
//
/*  no such specific parameter passing tweaking in MSC -- will this kill performance???
#pragma aux (VMM_CTL)   rmControl;
#pragma aux (VMM_API)   rmUsrAPI;
#pragma aux (VPICD_INT) rmInterrupt;
#pragma aux (VMM_CB)    osCallBack;
#pragma aux (VPICD_INT) osInterrupt;
#pragma aux (VPICD_INT) osVMIntEOI;
#pragma aux (VPICD_INT) osNopInt;
*/

#endif // _OSWIN_H_
