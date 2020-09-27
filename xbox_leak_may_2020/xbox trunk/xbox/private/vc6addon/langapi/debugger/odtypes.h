/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    odtypes.h

Abstract:

Author:

    David J. Gilman (davegi) 05-Apr-1992

Environment:

    Win32, User Mode


--*/

#if ! defined _ODTYPES_
#define _ODTYPES_

#include "types.h"
#include "cvtypes.h"

#ifdef  __cplusplus
#pragma warning ( disable: 4200 )
#endif


//  If the following definitions do not produce a 32 bit type on your
//  platform, please fix it.
//

typedef void FAR * HANDLE32;
typedef HANDLE32 FAR * LPHANDLE32;

#if !defined(DECLARE_HANDLE32)
#ifdef STRICT
#define DECLARE_HANDLE32(name) struct name##__32 { int unused; }; typedef struct name##__32 FAR *name
#else
#define DECLARE_HANDLE32(name) typedef HANDLE32 name
#endif
#endif
//

typedef void FAR * HANDLE32;
typedef HANDLE32 FAR * LPHANDLE32;

#if !defined(DECLARE_HANDLE32)
#ifdef STRICT
#define DECLARE_HANDLE32(name) struct name##__32 { int unused; }; typedef struct name##__32 FAR *name
#else
#define DECLARE_HANDLE32(name) typedef HANDLE32 name
#endif
#endif

#ifdef STRICT

    DECLARE_HANDLE32(HTL);
    DECLARE_HANDLE32(HEM);
    DECLARE_HANDLE32(HOSDFILE);

#else

    typedef HIND HTL;               // handle to a transport layer
    typedef HIND HEM;               // handle to an execution model
    typedef HIND HOSDFILE;

#endif

typedef HTL  FAR *LPHTL;
typedef HEM  FAR *LPHEM;
typedef HEMI FAR *LPHEMI;


typedef char FAR *  LSZ;

#define OSDAPI WINAPI
#define OSDAPIV WINAPIV


//
// Error status codes
//

#define DECL_XOSD(n,v,s) n = v,

enum {
#include "xosd.h"
};
typedef LONG XOSD;
typedef XOSD FAR *LPXOSD;

#undef DECL_XOSD


//
// Debugger callback types
//

typedef enum DBCT {     // debugger callback types
    dbctStop,           // debuggee has stopped -- no more dbc's will be sent
    dbctContinue,       // debuggee is continuing to run
    dbctMaybeContinue,  // debuggee may or may not continue, depending on other
                        //  information.  Interpretation is DBC-specific.
} DBCT;

//
// Debugger callbacks
//

#define DECL_DBC(name, fRequest, dbct)  dbc##name,


typedef enum {
        #include "dbc.h"
} _DBC;

typedef DWORD DBC;

#undef DECL_DBC

//
// Debugger services export table
//

typedef struct {
    PVOID      (OSDAPI *  lpfnMHAlloc)        ( size_t );
    PVOID      (OSDAPI *  lpfnMHRealloc)      ( LPVOID, size_t );
    VOID       (OSDAPI *  lpfnMHFree)         ( LPVOID );

    HLLI       (OSDAPI *  lpfnLLInit)         ( DWORD,
                                                LLF,
                                                LPFNKILLNODE,
                                                LPFNFCMPNODE );
    HLLE       (OSDAPI *  lpfnLLCreate)       ( HLLI );
    VOID       (OSDAPI *  lpfnLLAdd)          ( HLLI, HLLE );
    VOID       (OSDAPI *  lpfnLLInsert)       ( HLLI, HLLE, DWORD );
    BOOL       (OSDAPI *  lpfnLLDelete)       ( HLLI, HLLE );
    HLLE       (OSDAPI *  lpfnLLNext)         ( HLLI, HLLE );
    DWORD      (OSDAPI *  lpfnLLDestroy)      ( HLLI );
    HLLE       (OSDAPI *  lpfnLLFind)         ( HLLI,
                                                HLLE,
                                                LPVOID,
                                                DWORD );
    DWORD      (OSDAPI *  lpfnLLSize)         ( HLLI );
    PVOID      (OSDAPI *  lpfnLLLock)         ( HLLE );
    VOID       (OSDAPI *  lpfnLLUnlock)       ( HLLE );
    HLLE       (OSDAPI *  lpfnLLLast)         ( HLLI );
    VOID       (OSDAPI *  lpfnLLAddHead)      ( HLLI, HLLE );
    BOOL       (OSDAPI *  lpfnLLRemove)       ( HLLI, HLLE );

    int        (OSDAPI *  lpfnLBAssert)       ( LPSTR, LPSTR, DWORD);
    int        (OSDAPI *  lpfnLBQuit)         ( DWORD );

    LPSTR      (OSDAPI *  lpfnSHGetSymbol)    ( LPADDR  addr1,
                                                LPADDR  addr2,
                                                SHORT   sop, // should be SOP
                                                LPVOID  lpodr // should be LPODR
                                              );
    DWORD      (OSDAPI * lpfnSHGetPublicAddr) ( LPADDR, LSZ );
#ifdef NT_BUILD_ONLY
    LPSTR      (OSDAPI * lpfnSHAddrToPublicName)(LPADDR, LPADDR);
#else
    LPSTR      (OSDAPI * lpfnSHAddrToPublicName)(LPADDR);
#endif
    LPVOID     (OSDAPI * lpfnSHGetDebugData)  ( HIND );

// BUGBUG kentf   this gets implemented!!!!! Soon!!!!!
    // DWORD      (OSDAPI *  lpfnSHLocateSymbolFile)( LPSTR, DWORD );

    PVOID      (OSDAPI *  lpfnSHLpGSNGetTable)( HIND );

#ifdef NT_BUILD_ONLY
    BOOL       (OSDAPI *  lpfnSHWantSymbols)  ( HIND );
#endif

    DWORD      (OSDAPI *  lpfnDHGetNumber)    ( LPSTR, LPLONG );
    MPT        (OSDAPI *  lpfnGetTargetProcessor)( HPID );
    LONG       (OSDAPI *  lpfnGetSet)         ( LPTSTR, LPTSTR, LPDWORD,
                                                BYTE*, DWORD, BOOL );

    PVOID      lpSHF;                  // really LPSHF

} DBF;  // DeBugger callback Functions

typedef DBF FAR *LPDBF;

// Thread State bits
typedef enum {
   tstRunnable   = 0,        // New thread, has not run yet.
   tstStopped    = 1,        // Thread is at a debug event
   tstRunning    = 2,        // Thread is currently running/runnable
   tstExiting    = 3,        // Thread is in the process of exiting
   tstDead       = 4,        // Thread is no longer schedulable
   tstRunMask    = 0xf,

   tstExcept1st  = 0x10,     // Thread is at first chance exception
   tstExcept2nd  = 0x20,     // Thread is at second change exception
   tstRip        = 0x30,     // Thread is in a RIP state
   tstExceptionMask = 0xf0,

   tstFrozen     = 0x100,    // Thread has been frozen by Debugger
   tstSuspended  = 0x200,    // Thread has been frozen by Other
   tstBlocked    = 0x300,    // Thread is blocked on something
                             // (i.e. a semaphore)
   tstSuspendMask= 0xf00,

   tstCritSec    = 0x1000,   // Thread is currently in a critical
                             // section.
   tstOtherMask  = 0xf000
} TSTATE;


// Process state bits
typedef enum {
    pstRunning = 0,
    pstStopped = 1,
    pstExited  = 2,
    pstDead    = 3
} PSTATE;


//
// Debug metrics.
//

enum _MTRC {
    mtrcProcessorType,
    mtrcProcessorLevel,
    mtrcEndian,
    mtrcThreads,
    mtrcCRegs,
    mtrcCFlags,
    mtrcExtRegs,
    mtrcExtFP,
    mtrcExtMMU,
    mtrcPidSize,
    mtrcTidSize,
    mtrcExceptionHandling,
    mtrcAssembler,
    mtrcAsync,
    mtrcAsyncStop,
    mtrcBreakPoints,
    mtrcReturnStep,
    mtrcShowDebuggee,
    mtrcHardSoftMode,
    mtrcRemote,
    mtrcOleRpc,         // Supports OLE Remote Procedure Call debugging?
    mtrcNativeDebugger, // Supports low-level debugging (eg MacsBug)
    mtrcOSVersion,
    mtrcMultInstances,
    mtrcTidValue // HACK for IDE
};
typedef DWORD MTRC;


enum _BPTS {
    bptsExec     = 0x0001,
    bptsDataC    = 0x0002,
    bptsDataW    = 0x0004,
    bptsDataR    = 0x0008,
    bptsRegC     = 0x0010,
    bptsRegW     = 0x0020,
    bptsRegR     = 0x0040,
    bptsMessage  = 0x0080,
    bptsMClass   = 0x0100,
    bptsRange    = 0x0200,
    bptsDataExec = 0x0400,
    bptsPrologOk = 0x0800   // BP in prolog is harmless
};
typedef DWORD BPTS;

enum {
    asyncRun    = 0x0001,   // Debuggee runs asynchronously from debugger
    asyncMem    = 0x0002,   // Can read/write memory asynchronously
    asyncStop   = 0x0004,   // Can stop/restart debuggee asynchronously
    asyncBP     = 0x0008,   // Can change breakpoints asynchronously
    asyncKill   = 0x0010,   // Can kill child asynchronously
    asyncWP     = 0x0020,   // Can change watchpoints asyncronously
    asyncSpawn  = 0x0040,   // Can spawn another process asynchronously
};
typedef DWORD ASYNC;

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

//
// Types which we can't seem to escape
//

typedef struct {
    BYTE b[10];
} REAL10;
typedef REAL10 FAR * LPREAL10;



//
// Packet supplied to shell by dbcCanStep
//
//
typedef struct _CANSTEP {
    DWORD   Flags;

	union {
		UOFF32  PrologOffset;		// when flags == CANSTEP_YES
		UOFF32	ThunkSize;			// when flags == CANSTEP_THUNK_OVER
	};
	
} CANSTEP;

typedef CANSTEP FAR *LPCANSTEP;

#define CANSTEP_NO			0x00000000
#define CANSTEP_YES			0x00000001
#define CANSTEP_THUNK		0x00000002
#define CANSTEP_THUNK_OVER	0x00000003	// a step-over thunk

/*
 *  This structure is used in communicating a stop event to the EM.  It
 *      contains the most basic of information about the stopped thread.
 *      A "frame" pointer, a program counter and bits describing the type
 *      of segment stopped in.
 */

typedef struct _BPR {
    DWORD       dwNotify;       /* Tag to identify BP #          */
    UOFFSET     offEIP;         /* Program Counter offset        */
    UOFFSET     offEBP;         /* Frame pointer offset          */
    UOFFSET     offESP;         /* Stack pointer offset          */
    SEGMENT     segCS;          /* Program counter seletor       */
    SEGMENT     segSS;          /* Frame & Stack pointer offset  */
    DWORD       fFlat:1;
    DWORD       fOff32:1;
    DWORD       fReal:1;
} BPR; // BreakPoint Return

typedef BPR FAR *LPBPR;


//
// Exception reporting packet
//
//
typedef struct _EPR {
    BPR   bpr;
    DWORD dwFirstChance;
    DWORD ExceptionCode;
    DWORD ExceptionFlags;
    DWORD NumberParameters;
    DWORD ExceptionInformation[];
} EPR; // Exception Return

typedef EPR FAR *LPEPR;

//
// Structure passed with dbcInfoAvail
//
typedef struct _INFOAVAIL {
    DWORD   fReply;
    DWORD   fUniCode;
    BYTE    buffer[];   // the string
} INFOAVAIL; // InfoAvail return
typedef INFOAVAIL FAR * LPINFOAVAIL;

//
// Structure returned via dbcMsg*
//
typedef struct _MSGI {
    DWORD dwMessage;
    DWORD dwMask;
    ADDR  addr;
    CHAR  rgch [ ];
} MSGI;     // MeSsaGe Info
typedef MSGI FAR *LPMSGI;

//
// function information; derived from
// FPO, PDATA or whatever else there may be.
//
// This will contain information pertaining to the block
// containing the address specified in OSDGetFunctionInformation().
// It may be a nested block; it need not be an entire function.
//
typedef struct _FUNCTION_INFORMATION {
    ADDR    AddrStart;          // fixedup addresses
    ADDR    AddrPrologEnd;
    ADDR    AddrEnd;            // end of function
    //ADDR    FilterAddress;      // Address of exception filter
} FUNCTION_INFORMATION, *LPFUNCTION_INFORMATION;

#endif // _ODTYPES_
