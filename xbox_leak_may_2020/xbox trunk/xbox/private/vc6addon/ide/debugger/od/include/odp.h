/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    odp.h

Abstract:

    This is part of OSDebug version 4.

    These are types and data which are private to OSDebug and the
    components below it: TL, EM and DM.

Author:

    Kent D. Forschmiedt (kentf)

Environment:

    Win32, User Mode

--*/

#ifndef _ODP_
#define _ODP_

#ifdef  __cplusplus
#pragma warning ( disable: 4200 )
extern "C" {
#endif

#define DECL_EMF(emfName)       emf##emfName,

typedef enum _EMF {
#include "emf.h"
} EMF;

#undef DECL_EMF

typedef struct _DBCPK {
   DBC     dbc;
   HPID    hpid;
   HTID    htid;
   LPARAM  wValue;
   LPARAM  lValue;
} DBCPK; // a dbc package
typedef DBCPK FAR *LPDBCPK;


typedef enum {
    dbcoCreateThread = dbcMax,
    dbcoNewProc,

    dbcoMax
} DBCO;  // Debug CallBacks Osdebug specific

// the set of transport layer commands process by TLFunc and DMTLFunc

typedef enum {
    tlfRegisterDBF,     // register the debugger helper functions
    tlfInit,            // initialize/create a (specific) transport layer
    tlfDestroy,         // vaporize any tl structs created
    tlfConnect,         // connect to the companion transport layer
    tlfDisconnect,      // disconnected from the companion transport layer
    tlfSendVersion,     // Send the version packet to the remote side
    tlfGetVersion,      // Request the version packet from the remote side
    tlfSetBuffer,       // set the data buffer to be used for incoming packets
    tlfDebugPacket,     // send the debug packet to the debug monitor
    tlfRequest,         // request data from the companion transport layer
    tlfReply,           // reply to a data request message
    tlfGetInfo,         // return an id string and other data
    tlfSetup,           // set up the transport layer
    tlfGetProc,         // return the true TLFUNC proc for the htl
    tlfLoadDM,          // load the DM module
    tlfSetErrorCB,      // Set the address of the error callback function
    tlfListen,          // Remote: wait for somebody to make a connection to us
    tlfRemoteQuit,      // signal loss of connection
    tlfPassiveConnect,      // the remote monitor connects to transport with this
    tlfGetLastError,    // get the string associated with the last error
    tlfMax
} _TLF;
typedef DWORD TLF;

//
// callbacks the TL uses to communicate with shell -- stub or client.
//
typedef enum {
    tlcbDisconnect,     // Transport layer was disconnected normally
    tlcbMax
} _TLCB;
typedef DWORD TLCB;


typedef XOSD (*TLFUNC_ODP) ( TLF, HPID, LPARAM, LPARAM );
typedef XOSD (*EMFUNC_ODP) ( EMF, HPID, HTID, LPARAM, LPARAM );
typedef XOSD (*TLFUNCTYPE) ( TLF, HPID, LPARAM, LPARAM );
typedef XOSD (*DMTLFUNCTYPE) ( TLF, HPID, LPARAM, LPARAM );
typedef XOSD (*TLCALLBACKTYPE) (HPID, LPARAM, LPARAM );
//typedef VOID (*LPDMINIT) ( DMTLFUNCTYPE, LPVOID ); // I think this is wrong
typedef XOSD (*LPDMINIT) ( DMTLFUNCTYPE, LPVOID );
typedef VOID (*LPDMFUNC) ( DWORD, LPBYTE );
typedef DWORD (*LPDMDLLINIT) ( LPDBF );
typedef XOSD (*TLSERVERCALLBACK) (TLCB, HPID, HTID, LPARAM, LPARAM );



DECLARE_HANDLE32(HEMP);

typedef struct _THREADINFO {
    HPID hpid;
    HLLI llemp;
} THREADINFO;
typedef THREADINFO *LPTHREADINFO;   // Thread information

typedef struct _PROCESSINFO {
    HTL     htl;
    HEMP    hempNative;
    HLLI    llemp;
    DWORD   fNative;
    DWORD   lastmodel;
    LPFNSVC lpfnsvcCC;
    HLLI    lltid;
} PROCESSINFO;
typedef PROCESSINFO *LPPROCESSINFO;   // Process information

typedef struct _EMS {
    EMFUNC_ODP  emfunc;
    EMTYPE      emtype;
    HLLI        llhpid;
    DWORD       model;
} EMS; // Execution Model Structure - per EM
typedef EMS *LPEMS;

typedef struct _EMP {
    HEM         hem;
    EMFUNC_ODP  emfunc;
    EMTYPE      emtype;
    DWORD       model;
} EMP; // Execution Model Structure - per process
typedef EMP *LPEMP;

typedef struct _TLS {
    TLFUNC_ODP  tlfunc;
} TLS; // Transport Layer Structure
typedef TLS *LPTL;

typedef struct _OSDFILE {
    HPID  hpid;
    DWORD dwPrivateData;    // EM's representation of the file
} OSDFILE;
typedef OSDFILE * LPOSDFILE;

//
// Compare Address Struct
//
typedef struct _CAS {
    LPADDR lpaddr1;
    LPADDR lpaddr2;
    LPDWORD lpResult;
} CAS;
typedef CAS * LPCAS;

//
// Range Step Struct
//
typedef struct _RSS {
    LPADDR lpaddrMin;
    LPADDR lpaddrMax;
    LPEXOP lpExop;
} RSS;
typedef RSS * LPRSS;

//
// read memory struct
//
typedef struct _RWMS {
    LPADDR lpaddr;
    LPVOID lpbBuffer;
    DWORD cbBuffer;
    LPDWORD lpcb;
} RWMS;
typedef RWMS * LPRWMS;

//
// Get Object Length struct
//
typedef struct _GOL {
    LPADDR lpaddr;
    LPUOFF32 lplBase;
    LPUOFF32 lplLen;
} GOL;
typedef GOL * LPGOL;

//
// Get Function Information Structure
//
typedef struct _GFI {
    LPADDR lpaddr;
    LPFUNCTION_INFORMATION lpFunctionInformation;
} GFI;
typedef GFI * LPGFI;

//
// Get Previous Instruction Structure
//
typedef struct _GPIS {
    LPADDR lpaddr;
    LPUOFF32 lpuoffset;
} GPIS;
typedef GPIS * LPGPIS;

//
// Set Debug Mode Structure
//
typedef struct _SDMS {
    DBM dbmService;
    LPVOID lpvData;
    DWORD cbData;
} SDMS;
typedef SDMS * LPSDMS;

typedef struct _SSS {
    SSVC ssvc;
    DWORD cbSend;
    DWORD cbReturned;
    BYTE rgbData[];
} SSS;
typedef SSS * LPSSS;

//
// The following structure is used by the emfSetupExecute message
//
typedef struct _EXECUTE_STRUCT {
    ADDR        addr;           /* Starting address for function        */
    HIND        hindDm;         /* This is the DMs handle               */
    HDEP        lphdep;         /* Handle of save area                  */
    DWORD       fIgnoreEvents:1; /* Ignore events coming back?          */
    DWORD       fFar:1;         /* Is the function a _far routine       */
} EXECUTE_STRUCT;
typedef EXECUTE_STRUCT * LPEXECUTE_STRUCT;

//
// Load DM packet, used by TL
//
typedef struct _LOADDMSTRUCT {
    LPTSTR lpDmName;
    LPTSTR lpDmParams;
} LOADDMSTRUCT, * LPLOADDMSTRUCT;


void ODPDKill  ( LPVOID );

void EMKill    ( LPVOID );
int  EMHpidCmp ( LPVOID, LPVOID, LONG );
void EMPKill   ( LPVOID );

void TLKill    ( LPVOID );

void NullKill  ( LPVOID );
int  NullComp  ( LPVOID, LPVOID, LONG );

typedef struct _EMCB {
    XOSD (*lpfnCallBackDB) ( DBC, HPID, HTID, DWORD, DWORD, VOID * );
    XOSD (*lpfnCallBackTL) ( TLF, HPID, DWORD, VOID * );
    XOSD (*lpfnCallBackNT) ( EMF, HPID, HTID, DWORD, VOID * );
    XOSD (*lpfnCallBackEM) ( EMF, HPID, HTID, DWORD, DWORD, VOID * );
} EMCB; // Execution Model CallBacks
typedef EMCB *LPEMCB;

typedef struct _REMI {
    HEMI    hemi;
    LPTSTR  lsz;
} REMI;     // Register EMI structure
typedef REMI * LPREMI;

// Packet used by OSDSpawnOrphan
typedef struct _SOS {
    DWORD   dwChildFlags;
    LPTSTR  lszRemoteExe;    // name of remote exe
    LPTSTR  lszCmdLine;      // command line
    LPTSTR  lszRemoteDir;    // initial dir of debuggee
    LPSPAWNORPHAN    lpso;   // info to return about the spawn.
} SOS, *LPSOS;        // Spawn Orphan Structure

// packet used by OSDProgramLoad
// Doesn't use SOS.lpso
typedef SOS PRL;
typedef PRL *   LPPRL;


//
//    Structures used by GetTimeStamp ()
//

typedef struct _TCS {
    LPTSTR    ImageName;
    ULONG    TimeStamp;
    ULONG    CheckSum;
} TCS;

typedef struct TCSR {
    ULONG    TimeStamp;
    ULONG    CheckSum;
} TCSR;

typedef TCS* LPTCS;
typedef TCSR* LPTCSR;

#define MHAlloc(x)   ((*lpdbf->lpfnMHAlloc)(x))
#define MHRealloc(a,b) ((*lpdbf->lpfnMHRealloc)(a,b))
#define MHFree(y)    ((*lpdbf->lpfnMHFree)(y))

#define LLInit    (*lpdbf->lpfnLLInit)
#define LLCreate  (*lpdbf->lpfnLLCreate)
#define LLAdd     (*lpdbf->lpfnLLAdd)
#define LLInsert  (*lpdbf->lpfnLLInsert)
#define LLDelete  (*lpdbf->lpfnLLDelete)
#define LLNext    (*lpdbf->lpfnLLNext)
#define LLDestroy (*lpdbf->lpfnLLDestroy)
#define LLFind    (*lpdbf->lpfnLLFind)
#define LLSize    (*lpdbf->lpfnLLSize)
#define LLLock    (*lpdbf->lpfnLLLock)
#define LLUnlock  (*lpdbf->lpfnLLUnlock)
#define LLLast    (*lpdbf->lpfnLLLast)
#define LLAddHead (*lpdbf->lpfnLLAddHead)
#define LLRemove  (*lpdbf->lpfnLLRemove)

#define LBAssert  (*lpdbf->lpfnLBAssert)
#define DHGetNumber (*lpdbf->lpfnDHGetNumber)

#define SHLocateSymbolFile (*lpdbf->lpfnSHLocateSymbolFile)
#define SHGetSymbol        (*lpdbf->lpfnSHGetSymbol)
#define SHLpGSNGetTable    (*lpdbf->lpfnSHLpGSNGetTable)
#define SHFindSymbol       (*lpdbf->lpfnSHFindSymbol)

#define SHGetDebugData     (*lpdbf->lpfnSHGetDebugData)
#define SHGetPublicAddr    (*lpdbf->lpfnSHGetPublicAddr)
#define SHAddrToPublicName (*lpdbf->lpfnSHAddrToPublicName)
#define GetTargetProcessor (*lpdbf->lpfnGetTargetProcessor)
#ifdef NT_BUILD_ONLY
#define SHWantSymbols(H)      (*lpdbf->lpfnSHWantSymbols)(H)
#else
#define SHWantSymbols(H)      (0)
#endif

#ifdef  __cplusplus
}   // extern "C"
#endif

#endif // _ODP_
