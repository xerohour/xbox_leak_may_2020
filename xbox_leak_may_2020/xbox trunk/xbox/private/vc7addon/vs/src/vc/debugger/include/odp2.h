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
   WPARAM  wValue;
   LPARAM  lValue;
} DBCPK; // a dbc package
typedef DBCPK FAR *LPDBCPK;


// There are not more than 
#define Number_of_dbce_events 10
typedef enum {
    dbcoCreateThread = dbcMax+Number_of_dbce_events,
    dbcoNewProc,

    dbcoMax
} DBCO;  // Debug CallBacks Osdebug specific

// the set of transport layer commands process by TLFunc and DMTLFunc

typedef enum {
    tlfInit,            // initialize/create a (specific) transport layer
    tlfDestroy,         // vaporize any tl structs created
    tlfConnect,         // connect to the companion transport layer
    tlfDisconnect,      // disconnected from the companion transport layer
    tlfSetEMBuffer,     // set the data buffer to by TL to communicate with EM  
    tlfSetDMBuffer,     // set the data buffer to by TL to communicate with DM  
    tlfDebugPacket,     // send the debug packet to the debug monitor
    tlfRequest,         // request data from the companion transport layer
    tlfReply,           // reply to a data request message
    tlfLoadDM,          // load the DM module
    tlfListen,          // Remote: wait for somebody to make a connection to us
    tlfPassiveConnect,      // the remote monitor connects to transport with this
    tlfSetErrorCB,      // Set the address of the error callback function
    tlfDebugPacketWithoutAck,     // send the debug packet to the debug monitor
    tlfMax
} _TLF;
typedef _TLF TLF;

//
// callbacks the TL uses to communicate with shell -- stub or client.
//
typedef enum {
    tlcbDisconnect,     // Transport layer was disconnected normally
    tlcbMax
} _TLCB;
typedef _TLCB TLCB;


typedef XOSD (*TLFUNC_ODP) ( TLF, HPID, WPARAM , LPARAM );
typedef XOSD (*EMFUNC_ODP) ( EMF, HPID, HTID, WPARAM , LPARAM);
typedef XOSD (*TLFUNCTYPE) ( TLF, HPID, WPARAM , LPARAM );
typedef XOSD (*DMTLFUNCTYPE) ( TLF, HPID, WPARAM , LPARAM );
typedef XOSD (*TLCALLBACKTYPE) (HPID, WPARAM , LPARAM );
//typedef VOID (*LPDMINIT) ( DMTLFUNCTYPE, LPVOID ); // I think this is wrong
typedef XOSD (*LPDMINIT) ( LPARAM );
typedef VOID (*LPDMFUNC) ( WPARAM, LPBYTE );
typedef XOSD (*TLSERVERCALLBACK) (TLCB, HPID, HTID, WPARAM , LPARAM  );



DECLARE_HANDLE32(HEMP);

typedef struct _THREADINFO {
    HPID hpid;
    HLLI llemp;
} THREADINFO;
typedef THREADINFO *LPTHREADINFO;   // Thread information

typedef struct _PROCESSINFO {
    HTL        htl;
    HEMP       hempNative;
    HLLI       llemp;
    //DWORD      fNative;
    //DWORD_PTR  lastmodel;
    LPFNSVC    lpfnsvcCC;
    HLLI       lltid;
#ifdef TRACK_HPIDS
	DWORD	   dwMagic;		// for testing and debugging
#endif

} PROCESSINFO;
typedef PROCESSINFO *LPPROCESSINFO;   // Process information

typedef struct _EMS {
	HEM			__unused;	// this structure should be aligned with
    EMFUNC_ODP  emfunc;		// the LPEMP for reasons only od.c
    EMTYPE      emtype;		// understands . . . 
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
    LPCADDR lpaddr;
    LPVOID lpbBuffer;
    DWORD cbBuffer;
    LPDWORD lpcb;
} RWMS;
typedef RWMS * LPRWMS;

// 
// Load Library structure
//
typedef struct _LDLLS {
    PADDR   paddr;
    LPCTSTR szDllName;
} LDLLS;
typedef LDLLS * LPLDLLS;

//
// Get Object Length struct
//
typedef struct _GOL {
    LPADDR lpaddr;
    LPUOFFSET lplBase;
    LPUOFFSET lplLen;
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
	int iCount;						// how many instructions should we go back?
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
	DWORD		dwTimeout;		/* How long to wait						*/
    BOOL        fIgnoreEvents:1; /* Ignore events coming back?          */
} EXECUTE_STRUCT;
typedef EXECUTE_STRUCT * LPEXECUTE_STRUCT;

//
// Load DM packet, used by TL
//
// Warning; struct is used on 32 & 64 bit side needs to be properly
// aligned
//
//
typedef struct _LOADDMSTRUCT {
	union {
    	LPTSTR             lpDmName;
		DWORDLONG          pad0;
	};

	// these pointers are only valid for a local DMInit
	union {
		DMTLFUNCTYPE       lpDMTLFunc;
		DWORDLONG 		   pad2;
	};

	union {
		LPFNREFCOUNTENGINE lpRefFunc;
		DWORDLONG 		   pad3;
	};

	union {
		LPVOID             lpRefFuncParam;
		DWORDLONG		   pad4;
	};

} LOADDMSTRUCT, * LPLOADDMSTRUCT;

typedef struct _DMINITS {

	

	TCHAR 		szDMParams[512];
	TCHAR       szRegRoot[1024];


} DMINITS, *LPDMINITS;


void ODPDKill  ( LPVOID );

void EMKill    ( LPVOID );
int  EMHpidCmp ( LPVOID, LPVOID, LONG );
void EMPKill   ( LPVOID );

void TLKill    ( LPVOID );

void NullKill  ( LPVOID );
int  NullComp  ( LPVOID, LPVOID, LONG );

typedef struct _EMCB {
    XOSD (*lpfnCallBackDB) ( DBC, HPID, HTID, WPARAM, WPARAM, LPARAM );
    XOSD (*lpfnCallBackTL) ( TLF, HPID, WPARAM, LPARAM );
    XOSD (*lpfnCallBackNT) ( EMF, HPID, HTID, WPARAM, LPARAM );
    XOSD (*lpfnCallBackEM) ( EMF, HPID, HTID, WPARAM, WPARAM, LPARAM );
} EMCB; // Execution Model CallBacks
typedef EMCB *LPEMCB;

interface ISymbolHandler;

typedef struct _REMI {
    HEMI    hemi;
	HMODULE	hModule;
	ISymbolHandler *pSymHandler;
} REMI;     // Register EMI structure
typedef REMI * LPREMI;

//
// Packet used by OSDSpawnOrphan
//

typedef struct _SOS {
    DWORD   dwChildFlags;
    LPTSTR  lszRemoteExe;   // name of remote exe
    LPTSTR  lszCmdLine;     // command line
    LPTSTR  lszRemoteDir;	// initial dir of debuggee
	PROCESS_ID pid;			// OUT Parameter with the process id
} SOS;        // Spawn Orphan Structure
typedef SOS* LPSOS;


//
// packet used by NextReadableMemory
//
typedef struct _NREADMEM {
	UOFFSET offStart;
	DWORD   dwRange;
	UOFFSET offNextReadable;
} NREADMEM, *LPNREADMEM;


//
//	packet used by OSDProgramLoad
//

typedef struct _PRL {
    DWORD      dwChildFlags;
	DWORD      fStartDmThread; // Hack for stress restart failures on Tlloc 
    LPTSTR     lszRemoteExe;    // name of remote exe
    LPTSTR     lszCmdLine;      // command line
    LPTSTR     lszRemoteDir;    // initial dir of debuggee
	LAUNCHMODE mode;            // launch mode - native/complus/blah...
 } PRL; // PRogram Load structure
typedef PRL *   LPPRL;


//
//		Structures used by GetAddr and SetAddr
//

typedef struct _GETSETADDR {
	HFRAME	hFrame;
	ADR		adr;
	LPADDR	lpaddr;
} GETSETADDR;



typedef struct _GETSETREGISTERFLAG {
	HFRAME	hFrame;
	DWORD	dwIndex;
	LPDWORD attribs;					
	LPVOID	lpv;
} GETSETREGISTERFLAG;

//
//	Used by the FindFile functions
//

typedef struct FINDFILEFIRST {
	LPCSTR	szFileParam;
	HOSDFF*	lpFindFileHandle;
	OSDFINDFILEINFO* lpFileInfo;
} FINDFILEFIRST;


	
typedef struct TCSR {
    DWORD	dwTimeStamp;
    DWORD	dwCheckSum;
} TCSR;

typedef TCSR* LPTCSR;

#define LBAssert  (*lpdbf->lpfnLBAssert)

#ifdef  __cplusplus
}   // extern "C"
#endif

#ifndef VCDBG_DONOT_OVERRIDE_ALLOCACTORS

#include <debugmem.h>

#endif

#endif // _ODP_
