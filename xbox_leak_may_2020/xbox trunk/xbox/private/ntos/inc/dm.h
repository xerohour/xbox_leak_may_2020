/*
 *
 * dm.h
 *
 * Information for the debug monitor
 *
 */

#ifndef _DM_
#define _DM_

// Per thread debug monitor data
typedef struct _DMTD {
    ULONG LastError;
    PVOID WinSockData;
    PEXCEPTION_RECORD ExceptionRecord;
    PCONTEXT ExceptionContext;
    ULONG ExceptionDr6;
    ULONG DebugFlags;
    union {
        PKSYSTEM_ROUTINE SystemStartupRoutine;
        PVOID FuncCallData;
    };
    ULONG UserData;
    PKEVENT DebugEvent;
// don't change anything above this line
    PVOID ExceptionStack;
    KEVENT DebugEventData;
} DMTD, *PDMTD;

// values for dmtd.DebugFlags
#define DMFLAG_HALTTHREAD 1 // halt this thread
#define DMFLAG_FCE 2 // stopped in first chance exception
#define DMFLAG_EXCEPTION 4 // continue exception processing
#define DMFLAG_STOPPED 8 // this thread is stopped
#define DMFLAG_DEBUGTHREAD 0x10 // this thread belongs to the debugger
#define DMFLAG_XAPITHREAD 0x20 // this is an XAPI thread
#define DMFLAG_KDEXCEPT 0x40 // send the current exception to KD
#define DMFLAG_STARTTHREAD 0x80 // this thread is starting up
#define DMFLAG_FUNCCALL 0x100 // going to execute a function call
#define DMFLAG_INFUNCCALL 0x200 // inside a function call

// Data shared between the debug monitor and XAPI
typedef struct _DMXAPI {
    BOOLEAN XapiStarted;
    ULONG LastErrorTlsOff;
    ULONG CurrentFiberTlsOff;
// don't change anything above this line
} DMXAPI, *PDMXAPI;

// Data shared between the debug monitor and the D3D driver
typedef struct D3DSurface D3DSurface;
typedef struct _DMGD {
    volatile ULONG *FrameCounter;
    volatile PULONG FlipCounter;
    D3DSurface *Surface;
    PVOID *RegisterBase;
    PVOID PerfCounters;
    volatile PULONG pdwOpcode;
    unsigned char **ppSnapshotBuffer;
// don't change anything above this line
} DMGD, *PDMGD;

// Opcode constants for handshake between DM and D3D
// Pixel shader snapshot handshake with DM goes like this:
//     DM                 D3D
//  --------------     ------------
//                     PSSNAP_IDLE
//  PSSNAP_REQUEST
//                     PSSNAP_BUSY
//                     PSSNAP_ACK
//  PSSNAP_DONE
//                     PSSNAP_IDLE

#define PSSNAP_IDLE     0x00000000
#define PSSNAP_REQUEST  0x01000000
#define PSSNAP_BUSY     0x02000000
#define PSSNAP_ACK      0x03000000
#define PSSNAP_ERROR    0x04000000
#define PSSNAP_DONE     0x05000000

#define VSSNAP_IDLE     0x00000000
#define VSSNAP_REQUEST  0x81000000
#define VSSNAP_BUSY     0x82000000
#define VSSNAP_ACK      0x83000000
#define VSSNAP_ERROR    0x84000000
#define VSSNAP_DONE     0x85000000

#define XRAY_IDLE       0x00000000
#define XRAY_BEGINREQ   0x11000000
#define XRAY_BEGINACK   0x12000000
#define XRAY_ENDREQ     0x13000000
#define XRAY_ENDACK     0x14000000
#define XRAY_ERROR      0x15000000


// System-wide global debug monitor state
typedef struct _DMINIT {
    PKDEBUG_ROUTINE *DebugRoutine;
    PLIST_ENTRY LoadedModuleList;
    ULONG Flags;
    PVOID *ClockIntRoutine;
    PVOID *ProfIntRoutine;
    ULONG (*TellRoutine)(ULONG code, PVOID data);
    PVOID *CtxSwapNotifyRoutine;
    PVOID XProfpDataPtr;
    PDMGD D3DDriverData;
    PDMXAPI XapiData;
    PBOOLEAN DisallowXbdm;
    void (*HalStartProfileRoutine)(ULONG);
    void (*HalStopProfileRoutine)(ULONG);
    void (*HalProfileIntervalRoutine)(ULONG);
    PVOID *DpcDispatchNotifyRoutine;
// don't change anything above this line
} DMINIT, *PDMINIT;

// values for dminit.Flags
#define DMIFLAG_BREAK 2 // break prior to calling xapi
#define DMIFLAG_BUGCHECK 4 // we're in a bugcheck
#define DMIFLAG_RUNSHELL 8 // we've launched the shell
#define DMIFLAG_CDBOOT  16 // we've booted from the CD

// No code outside XBDM should be accessing the DebugData field
#ifdef DEVKIT
#define DmGetCurrentDmtd() ((PDMTD)PsGetCurrentThread()->DebugData)
#endif

#define DmGetCurrentDmi() ((PDMINIT)KeGetCurrentPrcb()->DebugMonitorData)

FORCEINLINE PVOID DmGetCurrentValue(ULONG ulOff)
{
    PDMINIT pdmi = DmGetCurrentDmi();
    return pdmi ? *(PVOID*)((ULONG_PTR)pdmi + ulOff) : NULL;
}

#define DmiOffset(field) ((ULONG_PTR)&((PDMINIT)NULL)->field)
#define DmGetCurrentDmgd() ((PDMGD)DmGetCurrentValue(DmiOffset(D3DDriverData)))
#define DmGetCurrentXapi() ((PDMXAPI)DmGetCurrentValue(DmiOffset(XapiData)))

FORCEINLINE ULONG DmTell(ULONG ulCode, PVOID pvData)
{
    PDMINIT pdmi = DmGetCurrentDmi();
    if(pdmi)
        return pdmi->TellRoutine(ulCode, pvData);
    else
        return 0;
}

#define DMTELL_ENTERDEBUGGER    1
#define DMTELL_REBOOT           2
#define DMTELL_MAPDRIVE         3 // pointer to DMDRIVE
#define DMTELL_FRAMECOUNT       4 // pointer to D3D frame counter
#define DMTELL_GPUPERF          5 // pointer to d3d's global D3DPERF
#define DMTELL_REGISTERPERFCOUNTER 6 // pointer to a DMREGISTERPERFCOUNTERPARAMBLOCK
#define DMTELL_UNREGISTERPERFCOUNTER 7 // LPCCH szName name of counter to deregister
#define DMTELL_SETDBGIP         8
#define DMTELL_GETDBGIP         9

// Use DmTell_RegisterPerformanceCounter to register performance counters
// without having to link to xbdm.dll

typedef struct _DMREGISTERPERFCOUNTERPARAMBLOCK
{
    LPCCH szName;
    ULONG dwType;
    PVOID pvArg;
} DMREGISTERPERFCOUNTERPARAMBLOCK,*PDMREGISTERPERFCOUNTERPARAMBLOCK;


FORCEINLINE ULONG DmTell_RegisterPerformanceCounter(LPCCH szName, ULONG dwType, PVOID pvArg)
{
    DMREGISTERPERFCOUNTERPARAMBLOCK block;
    block.szName = szName;
    block.dwType = dwType;
    block.pvArg = pvArg;
    return DmTell(DMTELL_REGISTERPERFCOUNTER, &block);
}

FORCEINLINE ULONG DmTell_UnregisterPerformanceCounter(LPCCH szName)
{
    return DmTell(DMTELL_UNREGISTERPERFCOUNTER, (PVOID) szName);
}

typedef struct _DMDRIVE {
    OCHAR Drive;
    POSTR Path;
} DMDRIVE, *PDMDRIVE;

#endif // _DM_
