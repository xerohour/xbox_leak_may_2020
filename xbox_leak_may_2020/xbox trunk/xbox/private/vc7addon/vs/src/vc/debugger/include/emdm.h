/**** EMDM.HMD - Common structures for Win32/NT EM and DM               ****
 *                                                                         *
 *                                                                         *
 *  Copyright <C> 1990, Microsoft Corp                                     *
 *                                                                         *
 *  Created: November 17, 1990 by David W. Gray                            *
 *                                                                         *
 *  Purpose:                                                               *
 *                                                                         *
 *      This file defines the types, enums, and constants that are common  *
 *      for all execution models, both debugger and debuggee end.          *
 *                                                                         *
 ***************************************************************************/

#ifndef _EMDM
#define _EMDM

//
// This is included to define a NONVOLATILE_CONTEXT_POINTERS structure
// of the appropriate size.  The goal is to keep any machine-specific
// reference out of emdp.c here, but we need to know how much data to
// transfer to the thread context on the different architectures.
//

#include "ctxptrs.h"

#define INITIAL_EMWBBUFFERSIZE   4096*2 
#define INITIAL_DMSENDBUFFERSIZE 1024 

#ifndef try
#define try __try
#endif
#ifndef except
#define except __except
#endif
#ifndef finally
#define finally __finally
#endif

typedef DWORD MTE;

#ifdef TARGET32
#define MAXBIGSEGS      3
#endif


typedef DWORD PID;
typedef DWORD TID;


typedef enum {
    dmfRemoteDied = -1,             /* debugger quit */
    dmfCommError  = -2,             /* transport layer error */

    dmfNull = 0,

    dmfInit,
    dmfUnInit,
    dmfSelect,
    dmfConnect,

    dmfCreatePid,
    dmfDestroyPid,

    dmfSetPath,
    dmfSpawnOrphan,
    dmfProgLoad,
	dmfLoadCrashDump,
    dmfProgFree,
    dmfDebugActive,
    dmfTerm,
    dmfDetach,

    dmfGo,
    dmfStop,
	dmfNextReadableMemory,
    dmfThreadManipulate,
	dmfLockUnLock,

    dmfSingleStep,
    dmfReturnStep,
    dmfNonLocalGoto,
    dmfRangeStep,
    dmfGoToReturn,

    dmfBreakpoint,
    dmfGetExceptionState,
    dmfSetExceptionState,

    dmfReadMem,
    dmfReadReg,
    dmfReadFrameReg,
    dmfWriteMem,
    dmfWriteReg,
    dmfWriteFrameReg,
    dmfVirtualAllocEx,
    dmfGetFP,
    dmfSetFP,
    dmfThreadStatus,
    dmfProcessStatus,
    dmfQueryTlsBase,
    dmfQuerySelector,
    dmfVirtualQuery,
    dmfReadRegEx,
    dmfWriteRegEx,
    dmfGetSections,

    dmfInit32SegValues,
    dmfSelLim,

    dmfSetMulti,
    dmfClearMulti,
    dmfDebugger,
    dmfSync,
    dmfSystemService,
    dmfGetPrompt,
    dmfSendChar,
    dmfGetDmInfo,

    dmfSetupExecute,
    dmfStartExecute,
    dmfCleanUpExecute,

    dmfRemoteUtility,       // for the mfile utility

	dmfGetSpecialReg,
	dmfSetSpecialReg,

	//
	// File I/O
	//

	dmfGetImageInformation,
	dmfFindFirstFile,
	dmfFindNextFile,
	dmfCloseFindFile,
	dmfOpenFile,
	dmfReadFile,
	dmfSeekFile,
	dmfCloseFile,
	dmfWriteCrashDump,

	dmfGetDebuggableTaskList,
    
    // Load Library
    dmfLoadLibrary,

	// Engine is ready for interop
	dmfEngineReady,


    dmfLast

} _DMF;

typedef LONG DMF;


typedef struct _DM_MSG {
    union {
        XOSD  xosdRet;
        DWORDLONG Alignment[2];
    };
    BYTE  rgb[1];
} DM_MSG;
typedef DM_MSG FAR *LPDM_MSG;

#define iflgMax 12

typedef struct _RTRNSTP {
   ADDR addrRA;         // Address to return to
   ADDR addrStack;       // Address of current SP.
   EXOP exop;
} RTRNSTP; // ReTuRN STeP packet
typedef RTRNSTP FAR *LPRTRNSTP;

#pragma pack(4)

typedef struct _RST {
#if defined(TARGET32) || defined(TARGET64)
    UOFFSET     offStart;
    UOFFSET     offEnd;
    UOFFSET     offPC;
#else
    ADDR addrStart;
    ADDR addrEnd;
    ADDR addrCSIP;
#endif
	EXOP	exop;
} RST; // Range STep Packet

typedef RST FAR *LPRST;

typedef struct _TCR {
    TID             tid;
    BOOL            bFiber;
    UOFFSET uoffTEB;
} TCR;  // Thread Create Return
typedef TCR FAR *LPTCR;

typedef struct _SETPTH {
    BOOL Set;
    TCHAR Path[1];
} SETPTH;

#pragma pack()

//
// DM Misc info structure.
//
// Some of these correspond to the debug metrics exposed by OSDebug.
// These cover the differences between user and kernel mode, Win32,
// Win32s and Win32c, maybe Cairo, whatever other DMs might be handled
// by the Win32 EM.
//
typedef struct _PROCESSOR {
    MPT   Type;
    DWORD Level;
    END   Endian;
	DWORD processorFlags;
	DWORD pageSize;
} PROCESSOR, FAR * LPPROCESSOR;

// Flags values, low word is general, high word is processor-specific
#define	PROCESSOR_FLAGS_EXTENDED	0x00000001
#define	PROCESSOR_FLAGS_I386_MMX	0x00010000
#define	PROCESSOR_FLAGS_I386_MMX2	0x00020000
#define	PROCESSOR_FLAGS_I386_MMX3	0x00040000
#define	PROCESSOR_FLAGS_I386_3DNOW	0x00080000

typedef struct _DMINFO {
    PROCESSOR Processor;
} DMINFO;
typedef DMINFO FAR * LPDMINFO;

typedef struct _EHP {
    DWORD iException;
    BOOL  fHandle;
} EHP; // Exception Handled Packet
typedef EHP FAR *LPEHP;

typedef struct _OBJD {
    UOFFSET     offset;
    DWORD	    rvaOffset;
    DWORD       cb;
    WORD        wSel;
    WORD        wPad;
} OBJD, FAR * LPOBJD;


#define	MAX_VERSION	20
typedef struct _MODULELOAD {
    SEGMENT              CSSel;
    SEGMENT              DSSel;
    LONG                 cobj;
    UOFFSET              uoffDataBase;

	MOD_INFO             modInfo;
	OSDIMAGEINFO         imageInfo;

	
	ULONG			     SectionAlignment;
    UOFFSET              uoffiTls;			// linear address in process of this module's tlsindex
    DWORD                isecTLS;			// index of .tls section
    DWORD                iTls;				// tls index, retrieved at initial breakpoint time
	TCHAR			     szVersion[MAX_VERSION];
	MODULE_DEBUG_INFO    debugInfo;         // Module Debug Info
    OBJD                 rgobjd[];
} MODULELOAD;

typedef MODULELOAD FAR *LPMODULELOAD;



//
// Whether the fourth parameter is a set of args or a command line depends
// on the dmf used: dmfSpawnOrphan = szCmdLine, dmfProgLoad = szArgs.
//

typedef struct _PROGLOAD {
	DWORD	   dwChildFlags;
	DWORD      fStartDmThread;
	LAUNCHMODE mode;
	TCHAR	   szExe [_MAX_PATH];
	TCHAR	   szDir [_MAX_PATH];

	union {
		TCHAR	szCmdLine [];		// variable length, NULL terminated
		TCHAR	szArgs [];			// variable length, NULL terminated
	};
} PROGLOAD;

typedef PROGLOAD* LPPROGLOAD;

// this struct is used by OSDOpenFile
typedef struct _FILELOAD {
	TCHAR      szExe[_MAX_PATH];    // name of the executable 
} FILELOAD, *LPFILELOAD;

// Used by OSDLoadLibrary
typedef struct _DLLLOAD {
    ADDR    addr;
    TCHAR   szDllName[_MAX_PATH];
} DLLLOAD, *LPDLLLOAD;

// this struct is used by OSDReadFile
typedef struct _FILEREAD {
	DWORD64 hFile;
	DWORD64 dwBytesToRead;	
} FILEREAD, *LPFILEREAD;

// this struct is used by OSDSeekFile
typedef struct _FILESEEK {
	DWORD64 hFile;
	DWORD64	dwLocation;	
	DWORD64	dwOrigin;	
} FILESEEK, *LPFILESEEK;

typedef struct _RWP {
    DWORD cb;
    ADDR addr;
    union {
        BYTE rgb[1];
        DWORDLONG Align;
    };
} RWP; // Read Write Packet

typedef RWP *PRWP;
typedef RWP FAR *LPRWP;

typedef struct _NPP {
    PID     pid;
    BOOL    fReallyNew;    
	DWORD   dwRegisterSet; // the register flag set;
	BOOL	fOnWin9x;      // running on win'9x?
	BOOL 	f64Bit;        // Is 64 bit?
} NPP;  // New Process Packet, used with dbcNewProc.
        // See od.h for description of fReallyNew.
typedef NPP FAR * LPNPP;

typedef struct _WPP {
    ADDR addr;
    WORD cb;
} WPP; // Watch Point Packet
typedef WPP FAR *LPWPP;

#if defined( TARGMAC68K )
typedef struct _SLI {
    DWORD dwBaseAddr;
    DWORD fExecute;
    short sRezID;
    unsigned char szName[];
} SLI;  // Segment Load Info
#else
typedef struct _SLI {
    WORD        wSelector;
    WORD        wSegNo;
    WORD        mte;
} SLI, FAR * LPSLI;
#endif

typedef SLI FAR *LPSLI;

// Exception command packet
typedef struct _EXCMD {
   EXCEPTION_CONTROL exc;
   EXCEPTION_DESCRIPTION exd;
} EXCMD;
typedef EXCMD FAR * LPEXCMD;

typedef struct _EXHDLR {
    DWORD count;
    ADDR addr[];
} EXHDLR;
typedef EXHDLR FAR * LPEXHDLR;

// 
// dbce's are packet ids meant for communication been dm and the osd
// layer, they never get propaged to the AD7 layer
//
// They may or maynot have return packets which convery information back
// to the DM
//
//
//
// The DBCEs always come back in an RTP structure, which has additional
// info.  The comments on the DBCEs below refer to the other fields of
// the RTP structure.
//
typedef enum {
    dbceAssignPID = dbcMax,     // Tell the EM what PID is associated with
                                // a given HPID.  At offset 0 of rtp.rgbVar[]
                                // is the PID.
    dbceCheckBpt,               // Find out if EM wants us to single-step
                                // over a specified breakpoint.  Upon return,
                                // rgbVar[0] is fStop to stop at this
                                // breakpoint; if fStop is FALSE, then
                                // rgbVar[1] is the byte with which to
                                // overwrite the INT 3.

    dbceExceptionDuringStep,    // Ask count prefix array of exception handlers
	dbceGetFrameWithSrc,		// Get closest frame with src
	dbceCreateThread,			// Create a thread struct and return the htid.
    dbceMax
} _DBCE;
typedef LONG DBCE;

typedef struct _DBB {
    DMF  dmf;
    union {
        HPID hpid;
        DWORDLONG Align1;
    };
    union {
        HTID htid;
        DWORDLONG Align2;
    };
    union {
        DWORDLONG Alignment;
        BYTE rgbVar[1];
    };
} DBB;

typedef DBB FAR *LPDBB;

typedef struct _RTP {
    ULONG dbc;                   // a DBC or a DBCE
    DWORD cb;                    // the length of rgbVar
    union {
        HPID hpid;
        DWORDLONG Align1;
    };
    union {
        HTID htid;
        DWORDLONG Align2;
    };
    union {
        DWORDLONG Alignment;
        BYTE rgbVar[1];         // additional information - see the
                                // definitions of the DBCE and DBC codes
    };
} RTP;

typedef RTP *PRTP;
typedef RTP FAR *LPRTP;

#define lpregDbb(dbb) ( (LPREG) &dbb )
#define lpfprDbb(dbb) ( (LPFPR) &dbb )
#define lszDbb(dbb)   ( (LSZ)   &dbb )

#define addrDbb(dbb)  (*( (LPADDR) &dbb ))
#define stpDbb(dbb)   (*( (LPSTP)  &dbb ))
#define rstDbb(dbb)   (*( (LPRST)  &dbb ))
#define gopDbb(dbb)   (*( (LPGOP)  &dbb ))
#define tstDbb(dbb)   (*( (LPTST)  &dbb ))
#define pstDbb(dbb)   (*( (LPF)    &dbb ))
#define rwpDbb(dbb)   (*( (LPRWP)  &dbb ))
#define fDbb(dbb)     (*( (LPF)    &dbb ))



/****************************************************************************
 *                                                                          *
 * Packets returned from the debuggee execution model to the debugger       *
 *  execution model.                                                        *
 *                                                                          *
 ****************************************************************************/


typedef struct _FRAME_INFO {
    CONTEXT frameRegs;
    KNONVOLATILE_CONTEXT_POINTERS frameRegPtrs;
} FRAME_INFO, * PFRAME_INFO;

typedef struct _DBGERROR {
	XOSD xosd;  // an xosd describing the Debugger error

} DBGERROR, *LPDBGERROR;

typedef struct _NLG {
    ADDR    addrNLGDispatch;
    ADDR    addrNLGDestination;
    ADDR    addrNLGReturn;
    ADDR    addrNLGReturn2;
    BOOL    fEnable;
	BOOL    fGotNLGReturn;
	BOOL    fGotNLGReturn2;
	union
	{
		HEMI    hemi;
		DWORDLONG	align;
	};
} NLG;
typedef NLG * PNLG;
typedef NLG FAR * LPNLG;


typedef struct _EMBUFFERINFO {
	LPBYTE lpEmWBBuf;
	LPBYTE lpDmSendBuf;
	DWORD  cbEmWBBuf;
	DWORD  cbDmSendBuf;
} EMBUFFERINFO;


typedef EMBUFFERINFO* LPEMBUFFERINFO;

typedef LPEMBUFFERINFO (__stdcall *PFNEMGETWBBUFFER)();

typedef struct _TLEMBUFFERMSG{
	DWORD  cbBuffer;
	union {
		PFNEMGETWBBUFFER pEmFunc;
		DWORDLONG        Align;
	}; 
		
} TLEMBUFFERMSG, *LPTLEMBUFFERMSG;

#endif  // _EMDM
