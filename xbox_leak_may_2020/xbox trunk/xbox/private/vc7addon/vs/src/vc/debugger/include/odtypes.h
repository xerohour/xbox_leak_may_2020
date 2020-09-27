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

#pragma once

#include "types.h"
#include "cvtypes.h"
#include "toolhrt.h"

#ifdef  __cplusplus
#pragma warning ( disable: 4200 )
#endif


//
// If the following definitions do not produce a 32 bit type on your
// platform, you must fix it.
//

typedef VOID FAR * HANDLE32;
typedef HANDLE32 FAR * LPHANDLE32;

#if !defined(DECLARE_HANDLE32)
#ifdef STRICT
#define DECLARE_HANDLE32(name) struct name##__32 { int unused; }; typedef struct name##__32 FAR *name
#else
#define DECLARE_HANDLE32(name) typedef HANDLE32 name
#endif
#endif

//
// Handle to a transport
//

DECLARE_HANDLE32(HTL);
typedef HTL  FAR *LPHTL;

//
// Handle to an EM
//

DECLARE_HANDLE32(HEM);
typedef HEM  FAR *LPHEM;

//
// Handle to an OSDebug file
//

DECLARE_HANDLE32(HOSDFILE);

//
// Handle to an OSD Find File object
//

DECLARE_HANDLE32(HOSDFF);

//
// Handle to a frame
//

DECLARE_HANDLE32(HFRAME);

typedef HEMI FAR *LPHEMI;


typedef char FAR *  LSZ;

#define OSDAPI WINAPI
#define OSDAPIV WINAPIV

#include "tltypes.h"


//
// Error status codes
//

#define DECL_XOSD(n,v,s) n = v,

typedef enum {
	#include "xosd.h"
} XOSD ;

#undef DECL_XOSD

typedef LONG  _XOSD;
typedef XOSD* LPXOSD;

#define IsValidXOSD(_xosd) ((_xosd) >= xosdNone && (_xosd) <= xosdInvalidMTE)

//
// Debugger callback types
//

typedef enum {			// debugger callback types
    dbctStop,           // debuggee has stopped -- no more dbc's will be sent
    dbctContinue		// debuggee is continuing to run
} DBCT;

//
// Debugger callbacks
//

#define DECL_DBC(name, fRequest, dbct)  dbc##name,


typedef enum {
        #include "dbc.h"
} DBC;

#undef DECL_DBC

#define IsValidDBC(_dbc) ((_dbc) > dbcNil && (_dbc) < dbcMax)


typedef DWORD PROCESS_ID;

//
// Debugger services export table
//

typedef struct {
    PVOID      (OSDAPI *  lpfnMHAlloc)        ( size_t );
    PVOID      (OSDAPI *  lpfnMHRealloc)      ( LPVOID, size_t );
    VOID       (OSDAPI *  lpfnMHFree)         ( LPVOID );


    LPSTR      (OSDAPI *  lpfnSHGetSymbol)    ( LPADDR  addr1,
                                                LPADDR  addr2,
                                                SHORT   sop, // should be SOP
                                                LPVOID  lpodr // should be LPODR
                                              );
    BOOL      (OSDAPI * lpfnSHGetPublicAddr) ( LPADDR, LSZ );

    LPSTR      (OSDAPI * lpfnSHAddrToPublicName)(LPADDR);
    LPVOID     (OSDAPI * lpfnSHGetDebugData)  ( HIND );

    PVOID      (OSDAPI *  lpfnSHLpGSNGetTable)( HIND );

    DWORD      (OSDAPI *  lpfnDHGetNumber)    ( LPSTR, LPLONG );
    MPT        (OSDAPI *  lpfnGetTargetProcessor)( HPID );
    LONG       (OSDAPI *  lpfnGetSet)         ( LPTSTR, LPTSTR, LPDWORD,
                                                BYTE*, DWORD, BOOL );
} DBF;  // Debugger callback Functions

typedef DBF FAR *LPDBF;


	


////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//	OSDebug callback function types
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//
// Shell callback
//

typedef XOSD (OSDAPI *
LPFNSVC)(
	DBC		dbc,
	HPID	hpid,
	HTID	htid,
	WPARAM  wParam,
	LPARAM   lParam
	);

//
// Callback to maintain Liveness of Engine
//

typedef enum {
	AddRefReq  = 0,
} REFCOUNTREQ;

	

typedef VOID (OSDAPI *
LPFNREFCOUNTENGINE)(
	LPVOID      lpRefParam,
	REFCOUNTREQ type,
	HANDLE      hThread
	);


//
// Transport callback.
//

typedef XOSD (OSDAPI *TLFUNC)();

//
// EM callback.
//

typedef XOSD (OSDAPI *EMFUNC)();


interface ISymbolHandler;


//
// EM Management
//


typedef enum _EMTYPE {
    emNative,
    emNonNative
} EMTYPE;

#define EMISINFOSIZE 80
typedef struct _EMIS {
    DWORD fCanSetup;
    DWORD dwMaxPacket;
    DWORD dwOptPacket;
    DWORD dwInfoSize;
    TCHAR rgchInfo[EMISINFOSIZE];
} EMIS;
typedef EMIS * LPEMIS;

typedef struct _EMSS {
    //
    // emfSetup is called to tell the EM to fetch workspace
    // values from the shell.
    //

    // The EM may support three phases of setup;
    // Load, interact and save.
    // Load means call GetSet for each workspace parameter
    //  with fSet == FALSE.  The value will be returned, and
    //  the EM must remember it.
    // Interact means allow the user to perform configuration
    //  operations, like clicking things in a dialog.  The new
    //  values are remembered by the EM.
    // Save means the EM will call GetSet with fSet == TRUE for
    //  each configuration value; the shell-provided GetSet
    //  will store these as workspace values.
    //
    DWORD fLoad;
    DWORD fInteractive;
    DWORD fSave;
    LPVOID lpvPrivate;

    //
    // lParam is an instance or "this" value supplied by the shell,
    // and is only used when calling lpfnGetSet().
    //
    LPARAM lParam;

    LPGETSETPROFILEPROC lpfnGetSet;
} EMSS;
typedef EMSS * LPEMSS;



// Thread State bits
typedef enum _TSTATE {
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

   tstFiber      = 0x2000,   // Thread is really a Fiber (with a fake tid)

   tstOtherMask  = 0xf000
} TSTATE;



#define THREADNAMESIZE	100

//
// Thread state structure.
//

typedef struct _WORKER_HINT{
	ADDR	WorkerFn;
	ADDR	CreateThreadFn;
	ADDR	AfxBeginThreadFn;
} WORKER_HINT;

typedef struct _TST {
    DWORD dwThreadID;
    DWORD dwSuspendCount;
    DWORD dwPriority;
    DWORD dwState;
    UOFFSET dwTeb;
    TCHAR rgchThreadName [THREADNAMESIZE];
    WORKER_HINT WorkerHint;
	BOOL  fUpdateContext; // in param to signal update of context
} TST;
typedef TST * LPTST;


// Process state bits
typedef enum _PSTATE {
    pstRunning = 0,
    pstStopped = 1,
    pstDead    = 2 
} PSTATE;

//
// Process State structure.
//

typedef struct _PST {
    DWORD dwProcessID;
    DWORD dwProcessState;
	BOOL  fCanDetach;
	XOSD  xosdDetachFailure;
} PST;
typedef PST * LPPST;

//
// Debug metrics.
//

typedef enum _MTRC {
    mtrcProcessorType ,
    mtrcProcessorLevel,
	mtrcPageSize,
    mtrcEndian,
    mtrcAddressMode,

} MTRC;

//  Return values for mtrcEndian -- big or little endian -- which
//  byte is [0] most or least significat byte
typedef enum _END {
    endBig,
    endLittle
} END;

//  Return values for mtrcAddressMode
typedef enum _ADDRMODE {
    addressModeUnknown = 0,
    addressMode32Bit  = 1,
    addressMode64Bit  = 2
} ADDRMODE;

typedef enum _BPTS {
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
} BPTS;



//
// Address manipulation
//

typedef enum _ADR {
    adrCurrent,
    adrPC,
    adrBase,
    adrStack,
    adrData,
    adrTlsBase,
    adrBaseProlog,
    adrBackStore
} ADR;



////
//// Module lists
////
//
//typedef struct _MODULE_LIST {
//    DWORD           Count;
//} MODULE_LIST;
//typedef struct _MODULE_LIST * LPMODULE_LIST;

//
// Type of Debug_Info that the Dm conveys back to Sapi
// and engine
//
typedef enum {
    DBG_TYPE_NONE = 0,
	DBG_TYPE_PDB,
	DBG_TYPE_DBG
} DBG_TYPE;

//
// Module Debug Structure
//
// This sturcture is used by the DM to return
// debug information that it finds in a module
// back to the EM.
//

#pragma pack(push,4)

typedef struct _MODULE_DEBUG_INFO
{
	DBG_TYPE dbgType;
	DWORD dbgVersion;				// DEBUG_INFO_PDB6/7

	union {
		DWORDLONG All;
	    struct
	    {
			ULONG fmt;
			ULONG filepos;
            ULONG sig;
	        ULONG age;
	        TCHAR szMBCS[_MAX_PATH*3];
	    } pdbInfo6;
		struct
		{
			ULONG fmt;
			GUID guid;
			DWORD age;
			TCHAR szUTF8[_MAX_PATH*3];
		} pdbInfo7;
	};
} MODULE_DEBUG_INFO;

#pragma pack(pop)

#define	DEBUG_INFO_PDB6	'01BN'		// NB10
#define	DEBUG_INFO_PDB7	'SDSR'		// RSDS

typedef MODULE_DEBUG_INFO* LPMODULE_DEBUG_INFO;

//
// Module Information - Kept in the EM accessed by OSD
// layer
//
typedef struct _MOD_INFO {
	UOFFSET         BaseAddress;
	DWORD 			TimeDateStamp;
	DWORD 			CheckSum;
	DWORD           Size;
	UOFFSET         PrefferedBase;
	DWORD           LoadOrder;
	bool            f64Bit;              // is this a 64bit dll 

} MOD_INFO, *LPMOD_INFO;


typedef struct _MODULE_ENTRY {

//	MOD_INFO      modInfo;

//    UOFFSET            Base;
//	DWORD	           TimeDateStamp;
//	DWORD	           CheckSum;
//    UOFFSET            PreferredBase;
//    DWORD              LoadOrder;
//    UOFFSET            Limit;
//    DWORD              Segment;
//    DWORD              Selector;
//    DWORD              Type;

//	DWORD	           SectionAlignment;
//    DWORD              SectionCount;

//    DWORD              Flat;
//    DWORD              Real;

//    HEMI               hemi;

	DWORD              modId;
	MOD_INFO           modInfo;
    TCHAR              Name[ MAX_PATH ];
    TCHAR              Version[ MAX_PATH ];
	MODULE_DEBUG_INFO  debugInfo;
} MODULE_ENTRY;
typedef struct _MODULE_ENTRY * LPMODULE_ENTRY;






//
// Bit flags for dwFlags in OSDProgramLoad
//

#define ulfMultiProcess             0x0001L     // OS2, NT, and ?MAC?
#define ulfMinimizeApp              0x0002L     // Win32
#define ulfNoActivate               0x0004L     // Win32
#define ulfInheritHandles           0x0008L     // Win32  (DM only?)
//#define ulfWowVdm                   0x0010L     // Win32
//#define ulfSqlDebug                 0x0040L     // SQL debugging wanted
#define	ulfSuspended				0x0080L		// Win32


//
// DM specific launch codes - these determine which
// state the DM should launch the process in - currently there
// are three supported states
//
// 1. Native only
// 2. InterOp mode
// 3. Detach Mode

typedef enum  { 
	modeNative  = 0,
	modeComPlus,
	modeNub
}LAUNCHMODE;


//
// Target execution control
//

typedef struct _EXOP {
    BYTE fStepOver;
    BYTE fInitialBP;
    BYTE fPassException;
    BYTE fSetFocus;
	BYTE fInterOpStepInto;
	BYTE fInterOpConsumeAllEvents; // consume all events (interop only)
} EXOP;
typedef EXOP * LPEXOP;




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

typedef struct _CANSTEPREQ {
    ADDR      addr;
	BOOL      fGetThunkInfo;
	UOFFSET   validInstrAddr; // only needed if fGetThunkInfo is set
} CANSTEPREQ, *LPCANSTEPREQ;

typedef struct _CANSTEP {
    DWORD   Flags;

	//
	// These are returned if fGetThunkInfo is set
	//
	DWORD   ThunkInstrSize;           
	UOFFSET ThunkInstrOffset;             

	union {
		UOFFSET PrologOffset;		// when flags == CANSTEP_YES
		DWORD   ThunkSize;			// when flags == CANSTEP_THUNK_OVER
                                    //v-vadimp - ever more than 4G?
	};
} CANSTEP, *LPCANSTEP;

typedef CANSTEP FAR *LPCANSTEP;

enum {
	CANSTEP_NO			= 0x0000,
	CANSTEP_YES			= 0x0001,
	CANSTEP_THUNK		= 0x0002,
	CANSTEP_THUNK_OVER	= 0x0003
};

//
// This structure is used in communicating a stop event to the EM.  It
// contains the most basic of information about the stopped thread.  A
// "frame" pointer, a program counter and bits describing the type of
// segment stopped in.
//

typedef struct _BPR {
    DWORD_PTR   dwNotify;       // Tag to identify BP #
    UOFFSET     offEIP;			// Program Counter offset
    UOFFSET     offEBP;         // Frame pointer offset
    UOFFSET     offESP;         // Stack pointer offset
    SEGMENT     segCS;          // Program counter seletor
    SEGMENT     segSS;          // Frame & Stack pointer offset
    DWORD       fFlat : 1;
    DWORD       fOff32 : 1;
    DWORD       fReal : 1;
} BPR; // BreakPoint Return

typedef BPR FAR *LPBPR;

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



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//
// Exception Handling Data structures
//
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

typedef enum {
	ExceptionTypeCxx32,
	ExceptionTypeCxx64,
	ExceptionTypeNative32,
	ExceptionTypeNative64
} EXCEPTION_TYPE;

//
// BUGBUG: allow arbitrary length type names
//

typedef struct _TYPE_NAME {
	CHAR	szName [256];
} TYPE_NAME;

typedef struct _EPR {
    BPR   bpr;
    DWORD dwFirstChance;
    DWORD ExceptionCode;
    DWORD ExceptionFlags;
	EXCEPTION_TYPE ExceptionType;

	union {

		struct {
			DWORDLONG Align;
		} JustForKicks;

		struct {
			DWORD NumberParameters;
			DWORD ExceptionInformation[];
		} Native32;

		struct {
			DWORDLONG NumberParameters;
			DWORDLONG ExceptionInformation[];
		} Native64;

		struct {
			DWORDLONG ThisPtr;
			DWORD NumberOfCatchableTypes;
			TYPE_NAME TypeNameArray [];
		} Cxx;

	} u;
	
} EPR; // Exception Return



//
// These are the actions which the debugger may take
// in response to an exception raised in the debuggee.
//

typedef enum _EXCEPTION_FILTER_DEFAULT {
    efdIgnore,
    efdNotify,
    efdCommand,
    efdStop
} EXCEPTION_FILTER_DEFAULT;
typedef EXCEPTION_FILTER_DEFAULT * LPEXCEPTION_FILTER_DEFAULT;

//
// commands understood by OSDGetExceptionState
//

typedef enum _EXCEPTION_CONTROL {
    exfFirst,
    exfNext,
    exfSpecified
} EXCEPTION_CONTROL;
typedef EXCEPTION_CONTROL * LPEXCEPTION_CONTROL;

//
// Exception information packet
//

#define EXCEPTION_STRING_SIZE 60
typedef struct _EXCEPTION_DESCRIPTION {
    DWORD                    dwExceptionCode;
    EXCEPTION_FILTER_DEFAULT efd;
    TCHAR                    rgchDescription[EXCEPTION_STRING_SIZE];
} EXCEPTION_DESCRIPTION;
typedef EXCEPTION_DESCRIPTION * LPEXCEPTION_DESCRIPTION;


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


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//
//     Message information
//
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

enum {
    msgMaskNone  = 0x0,
    msgMaskWin   = 0x1,
    msgMaskInit  = 0x2,
    msgMaskInput = 0x4,
    msgMaskMouse = 0x8,
    msgMaskSys   = 0x10,
    msgMaskClip  = 0x20,
    msgMaskNC    = 0x40,
    msgMaskDDE   = 0x80,
    msgMaskOther = 0x100,
    msgMaskAll   = 0x0FFF,
};


typedef struct _MESSAGEINFO {
    DWORD   dwMsg;         //  Message number
    LPTSTR  lszMsgText;    //  Message Text
    DWORD   dwMsgMask;     //  Message mask
} MESSAGEINFO;
typedef struct _MESSAGEINFO *LPMESSAGEINFO;

//
//  MSG Map structure
//

typedef struct _MESSAGEMAP {
    DWORD          dwCount;      //  Number of elements
    LPMESSAGEINFO  lpMsgInfo;    //  Pointer to array
} MESSAGEMAP;
typedef struct _MESSAGEMAP *LPMESSAGEMAP;


typedef struct _MASKINFO {
    DWORD dwMask;
    LPTSTR lszMaskText;
} MASKINFO;
typedef MASKINFO * LPMASKINFO;

typedef struct _MASKMAP {
    DWORD dwCount;
    LPMASKINFO lpMaskInfo;
} MASKMAP;
typedef MASKMAP * LPMASKMAP;



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


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//
//	Register types --- flags describing recommendations on register display.
//
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

enum {
    rtProcessMask   = 0x0000000f,     // Mask for processor type bits
                                	  // these are enumerates, not bitfields.
    rtCPU           = 0x00000000,     // Central Processing Unit
	rtCPUSeg		= 0x00000001,	  // CPU Segment registers
    rtFPU           = 0x00000002,     // Floating Point Unit
	rtMMX			= 0x00000003,	  // Multimedia regs
	rtKatmai		= 0x00000004,	  // KatMai regs
    rtWillamette    = 0x00000005,
    rt3DNow         = 0x00000006,
	rtStacked		= 0x00000007,     // Merced stacked registers
    rtApplication   = 0x00000008,     // Merced application regs
	rtBranch		= 0x00000009,	  // Merced branch regs
	
    rtProcessMax    = 0x0000000a,	  // How many groups? 

    rtProcessorMask = 0x0000fff0,	  // Reg Groups - bitfields
	rtMMXProc		= 0x00000010,	  // Multimedia regs
	rtKatmaiProc	= 0x00000020,	  // KatMai regs
    rtWillametteProc= 0x00000040,
    rt3DNowProc     = 0x00000080,
                                      // This is the wrong place for this - JOC

    rtGroupMask     = 0x000f0000,     // Which group(s) register falls into
                                	  // Bitfields
    rtInvisible     = 0x00010000,     // Recommend no display
    rtRegular       = 0x00020000,     // Recommend regular display
    rtExtended      = 0x00040000,     // Recommend extended display
    rtSpecial       = 0x00080000,     // Special and hidden regs, e.g. kernel mode

    rtFmtTypeMask   = 0x00f00000,     // Mask of display formats
                                	  // these are enumerates, not bitfields.
    rtInteger       = 0x00100000,     // Unsigned integer format
    rtFloat         = 0x00200000,     // Floating point format
    rtAddress       = 0x00300000,     // Address format
    rtBit 			= 0x00400000,     // IA64 registers displayed as bits

    rtMiscMask      = 0x0f000000,     // misc info
	                               	  // Bitfields
    rtPC            = 0x01000000,     // this is the PC
    rtFrame         = 0x02000000,     // this reg affects the stack frame
    rtNewLine       = 0x04000000,     // print a newline when listing
    rtFlags         = 0x08000000,     // Flags register (cast is to avoid warning)

    rtIA64Mask      = 0xf0000000,     // Mask for IA64 special values
    rtStack         = 0x10000000,     // stacked IA64 register - check the stack frame before displaying - may not be used if stack registers are moved to a separate group
    rtNat           = 0x20000000      // has a NAT bit - check the NAT register and format accordingly
};
typedef DWORD RT;   // Register Types

#define rtFmtTypeShift  8

enum {
    ftProcessMask   = 0x0f,     // Mask for processor type bits
                                // these are enumerates, not bitfields.
    ftCPU           = 0x00,     // Central Processing Unit
    ftFPU           = 0x01,     // Floating Point Unit
    ftMMU           = 0x02,     // Memory Manager Unit
	ftProcessMax	= 0x03,		// How many groups?

    ftGroupMask     = 0xf0,     // Which group(s) register falls into
                                // Bitfields
    ftInvisible     = 0x10,     // Recommend no display
    ftRegular       = 0x20,     // Recommend regular display
    ftExtended      = 0x40,     // Recommend extended display
    ftSpecial       = 0x80,     // Special and hidden regs, e.g. kernel mode

    ftFmtTypeMask   = 0xf00,    // Mask of display formats
                                // these are enumerates, not bitfields.
    ftInteger       = 0x100,    // Unsigned integer format
    ftFloat         = 0x200,    // Floating point format
    ftAddress       = 0x300,    // Address format

    ftMiscMask      = 0xf000,   // misc info
                                // Bitfields
    ftPC            = 0x1000,   // this is the PC
    ftFrame         = 0x2000,   // this reg affects the stack frame
    ftNewLine       = 0x4000    // print a newline when listing
};
typedef DWORD FT;   // Flag Types

#define ftFmtTypeShift  8

//
//  Register description:  This structure contains the description for
//		a register on the machine.  Note that dwId must be used to get
//		the value for this register but a different index is used to get
//		this description structure.
//

typedef struct {
    LPTSTR      lszName;        /* Pointer into EM for registers name   */
    RT          rt;             /* Register Type flags                  */
    DWORD       dwcbits;        /* Number of bits in the register       */
    DWORD       dwGrp;
    DWORD       dwId;           /* Value to use with Read/Write Register*/
} RD;               // Register Description
typedef RD * LPRD;

enum _RegAttrib {
	REG_ISNAT		= 0x00000001,
	REG_ISINVALID	= 0x00000002
};

typedef struct _RWReg {
	LPDWORD	attribs;
	LPVOID	lpv;
} RWReg;

//
//  Flag Data description: This structure contains the description for
//      a flag on the machine. Note that the dwId field contains the
//      value to be used with Read/Write register to get the register which
//      contains this flag.
//

typedef struct _FD {
    LPTSTR         lszName;
    FT          ft;
    DWORD       dwcbits;
    DWORD       dwGrp;
    DWORD       dwId;
} FD;
typedef FD * LPFD;


////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
// Breakpoint Data Structures
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

typedef enum {
    bptpExec,			// Standard code BP
    bptpDataC,			// data: ??
//    bptpDataW,			// data: Write
//    bptpDataR,			// data: Read
//    bptpDataExec,		// data: Execute
    bptpRange,			// Range
} BPTP;


typedef enum {
    bpnsStop,
    bpnsContinue,
    bpnsCheck,
    bpnsMax
} BPNS;

typedef struct _BPIS {
    BPTP   bptp;
    BPNS   bpns;
    DWORD  fOneThd;
    union {
        HTID   htid;
        DWORDLONG Align;
    };

	union {
        struct {
            ADDR	addr;
        } exec;
		
        struct {
            ADDR	addr;
            DWORD	cb;
            BOOL	fEmulate;
        } data;
		
        struct {
            DWORD	dwId;
        } reg;
		
        struct {
            ADDR	addr;
            DWORD	imsg;
            DWORD	cmsg;
        } msg;
		
        struct {
            ADDR	addr;
            DWORD	dwmask;
        } mcls;
		
        struct {
            DWORD	ipt;
        } ipt;
		
        struct {
            ADDR	addr;
            DWORD	cb;
        } rng;

		struct {
			ADDR	BaseOfImage;
			UINT	iSection;
		} module;
    };
} BPIS;
typedef BPIS * LPBPIS;


typedef struct _BPRS {
	BPIS	bpis;
    union {
        DWORDLONG Align;
        DWORD_PTR Notify;
    };
	XOSD	xosd;
} BPRS;
typedef BPRS* LPBPRS;


typedef struct _BPS {
    DWORD cbpis;
    DWORD fSet;
	BPRS  rgBprs [0];
} BPS;
typedef BPS * LPBPS;

#define SizeofBPS(B) ( sizeof (BPS) + ((LPBPS) (B))->cbpis * sizeof (BPRS) )


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//
//	Assembly and Disassembly Data structures
//
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

enum {
    dopNone     = 0x00000000,
    dopAddr     = 0x00000001,   // put address (w/ seg) in front of disassm
    dopFlatAddr = 0x00000002,   // put flat address (no seg)
    dopOpcode   = 0x00000004,   // dump the Opcode
    dopOperands = 0x00000008,   // dump the Operands
    dopRaw      = 0x00000010,   // dump the raw code bytes
    dopEA       = 0x00000020,   // calculate the effective address
    dopSym      = 0x00000040,   // output symbols
    dopUpper    = 0x00000080,   // force upper case for all chars except syms
    dopHexUpper = 0x00000100,   // force upper case for all hex constants
                                // (implied true if dopUpper is set)
    dopIP       = 0x00000200,   // addrIP is set
};
typedef DWORD DOP;              // Disassembly OPtions


typedef struct _SDI {
    DOP    dop;              // Disassembly OPtions (see above)
    ADDR   addr;             // The address to disassemble
	ADDR   addrIP;           // The address of the IP to align with (if dopIP is set)
    BOOL   fAssocNext;       // This instruction is associated w/ the next one
    BOOL   fIsBranch;
    BOOL   fIsCall;
    BOOL   fJumpTable;
    ADDR   addrEA0;          // First effective address
    ADDR   addrEA1;          // Second effective address
    ADDR   addrEA2;          // Third effective address
    DWORD  cbEA0;            // First effective address size
    DWORD  cbEA1;            // Second effective address size
    DWORD  cbEA2;            // Third effective address size
    LONG   ichAddr;
    LONG   ichBytes;
    LONG   ichOpcode;
    LONG   ichOperands;
    LONG   ichComment;
    LONG   ichEA0;
    LONG   ichEA1;
    LONG   ichEA2;
    LPTSTR lpch;
} SDI;  // Structured DiSsassembly
typedef SDI *LPSDI;


typedef struct _CALL_INFO {
	DWORD dwCount;			// IN: max ADDRs to return, OUT: how may we got
	ADDR addrStart;			// IN: where to start
	ADDR addrEnd;			// OUT:where to stop
	LPADDR addrList;		// OUT:list of ADDRs (size in dwCount)
} CALL_INFO;


////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//	File Management Data Structures
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

enum {
	OSD_IMAGE_UNKNOWN,
	OSD_IMAGE_TYPE_PE,
	OSD_IMAGE_TYPE_CRASHDUMP
};

typedef DWORD OSD_IMAGE_TYPE;

typedef struct _OSDIMAGEINFO {
    IMAGE_INFO ImageInfo;  // from toolhrt.h
} OSDIMAGEINFO;
	
typedef struct _OSDFINDFILEINFO {
	TCHAR	szFileName [MAX_PATH];
	DWORD	nFileSizeHigh;
	DWORD	nFileSizeLow;
	DWORD	nFileTimeLow;
	DWORD	nFileTimeHigh;
} OSDFINDFILEINFO;


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
//     OS Specific info and control
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


typedef struct _OSDTASKENTRY {
	// from toolhrt.h
    TASK_ENTRY taskEntry;	
} OSDTASKENTRY;

typedef OSDTASKENTRY * LPOSDTASKENTRY;

typedef struct _OSDTASKLIST {
    DWORD			dwCount;
    OSDTASKENTRY	rgTasks [];
} OSDTASKLIST;
typedef OSDTASKLIST * LPOSDTASKLIST;

//
//
// Hard / Soft mode stuff which can probably be removed.
//

enum {
    dbmSoftMode,
    dbmHardMode
};

typedef DWORD DBM;

//
// Used for OSDSetDebugMode -- is this useful anymore??
//

typedef struct _DBMI {
    HWND hWndFrame;
    HWND hWndMDIClient;
    HANDLE hAccelTable;
} DBMI;

//
// Packet for DebugActiveProcess
//
//
typedef struct _DAP {

    DWORD      dwProcessId;
	LAUNCHMODE mode;
	BOOL       fInterOpLaunch; // an interop mode launch?
	BOOL       fStartDmThread;
	union {
	HPID       hpid;
	DWORDLONG  AlignHpid;
	};
	TCHAR      szFullPathHint[_MAX_PATH];
} DAP;

typedef DAP FAR * LPDAP;

//
// Length of a TL error.
//

#define TL_ERROR_BUFFER_LENGTH  1024

//
// System Services
//

#include "ssvc.h"
#define FIRST_PRIVATE_SSVC 0x8000

//
// Stack Range Walking stuff
//
//

typedef struct _STACK_RANGE {

	DWORDLONG eIp;
	DWORDLONG segCs;
	DWORDLONG segSs;
	DWORDLONG eSp;
	DWORDLONG startEbp;
	DWORDLONG endEbp;
	HFRAME  currFrame;

} STACK_RANGE, *LPSTACK_RANGE;


//
// Used by tlfConnect
//
//
typedef struct _TLFCONNECTMSG {
	LPFNREFCOUNTENGINE lpRefFunc;
	LPVOID             lpRefFuncParam;     
 	LPCTSTR            lszOptions;
	TCHAR              szRegRoot[1024];
} TLFCONNECTMSG, *LPTLFCONNECTMSG;


