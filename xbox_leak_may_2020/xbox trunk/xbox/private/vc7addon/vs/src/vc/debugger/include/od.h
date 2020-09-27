/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    Od.h

Abstract:

    This file contains types and prototypes which are exposed
    to all OSDebug components and clients.

Author:

    Kent Forschmiedt (kentf) 10-Sep-1993

Environment:

    Win32, User Mode

--*/

#pragma once

#include "odtypes.h"

#ifdef __cplusplus
extern "C" {
#endif


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//
//     OSDebug API set
//

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//
//	OSDebug Initialization/Termination
//


XOSD
OSDAPI
OSDInit(
    LPDBF lpdbf
    );

XOSD
OSDAPI
OSDTerm(
    VOID
    );

//
//	Execution Model Management
//

XOSD
OSDAPI
OSDAddEM(
    EMFUNC emfunc,
    LPDBF lpdbf,
    LPHEM lphem,
    EMTYPE emtype
    );

XOSD
OSDAPI
OSDDeleteEM(
    HEM hem
    );

XOSD
OSDAPI
OSDSetTLEMBuffer(
    HEM hem,
	HTL htl
    );

XOSD
OSDAPI
OSDGetCurrentEM(
    HPID hpid,
    HTID htid,
    LPHEM lphem
    );

XOSD
OSDAPI
OSDNativeOnly(
    HPID hpid,
    HTID htid,
    DWORD fNativeOnly
    );

XOSD
OSDAPI
OSDUseEM(
    HPID hpid,
    HEM hem
    );

XOSD
OSDAPI
OSDDiscardEM(
    HPID hpid,
    HTID htid,
    HEM hem
    );

XOSD
OSDAPI
OSDEMGetInfo(
    HEM hem,
    LPEMIS lpemis
    );

//XOSD
//OSDAPI
//OSDSetLocale (
//	HEM m_hem, 
//	WORD wLangID
//	);

XOSD
OSDAPI
OSDEMSetup(
    HEM hem,
    LPEMSS lpemss
    );

//
//	Transport Layer Management
//

XOSD
OSDAPI
OSDAddTL(
    TLFUNC tlfunc,
    LPDBF lpdbf,
    LPHTL lphtl
    );

XOSD
OSDAPI
OSDStartTL(
    HTL htl
    );

XOSD
OSDAPI
OSDDiscardTL(
	HPID	hpid,
	HTL     htl
	);

XOSD
OSDAPI
OSDDeleteTL(
    HTL htl
    );

XOSD
OSDAPI
OSDTLGetInfo(
    HTL htl,
    LPTLIS lptlis
    );

XOSD
OSDAPI
OSDTLSetup(
    HTL htl,
    LPTLSS lptlss
    );

XOSD
OSDAPI
OSDDisconnect(
    HPID hpid,
    HTID htid
    );

//
// Process, thread management
//

XOSD
OSDAPI
OSDCreateHpid(
    LPFNSVC lpfnsvcCallBack,
    HEM hemNative,
    HTL htl,
    LPHPID lphpid,
	LPTLFCONNECTMSG  lpTlfConnectMsg
    );

XOSD
OSDAPI
OSDDestroyHpid(
    HPID hpid
    );

XOSD
OSDAPI
OSDDestroyHtid(
    HPID hpid,
    HTID htid
    );

XOSD
OSDAPI
OSDGetThreadStatus(
    HPID hpid,
    HTID htid,
    LPTST lptst
    );

XOSD
OSDAPI
OSDGetProcessStatus(
    HPID hpid,
    LPPST lppst
    );

namespace od {
	enum ThreadManipulate {
		Thaw,
		Freeze,
		SetStopped
	};

	enum LockUnLock  {
		Lock,
		UnLock,
		HackLock
	};
		
};
	

XOSD
OSDAPI
OSDManipulateThread(
    HPID  				    hpid,
    HTID  				    htid,
	od::ThreadManipulate    manReq 
    );

XOSD
OSDAPI
OSDLockUnLock(
    HPID  				    hpid,
	od::LockUnLock			lockReq
    );

XOSD
OSDAPI
OSDSetThreadPriority(
    HPID hpid,
    HTID htid,
    DWORD dwPriority
    );

XOSD
OSDAPI
OSDGetAddr(
    HPID hpid,
    HTID htid,
	HFRAME hStackFrame,
    ADR adr,
    LPADDR lpaddr
    );

XOSD
OSDAPI
OSDSetAddr(
    HPID hpid,
    HTID htid,
	HFRAME hStackFrame,
    ADR adr,
    LPADDR lpaddr
    );

XOSD
OSDAPI
OSDFixupAddr(
    HPID hpid,
    HTID htid,
    LPADDR lpaddr
    );

XOSD
OSDAPI
OSDUnFixupAddr(
    HPID hpid,
    HTID htid,
    LPADDR lpaddr
    );

XOSD
OSDAPI
OSDSetEmi(
    HPID hpid,
    HTID htid,
    LPADDR lpaddr
    );

XOSD
OSDAPI
OSDRegisterEmi(
    HPID	hpid,
	HMODULE	hModule,
    HEMI	hemi,
	ISymbolHandler*	sapi
	);

XOSD
OSDAPI
OSDUpdateEmi(
    HPID	hpid,
    HEMI	hemi,
	ISymbolHandler*	sapi
	);

XOSD
OSDAPI
OSDUnRegisterEmi(
    HPID hpid,
    HEMI hemi
    );

XOSD
OSDAPI
OSDCompareAddrs(
    HPID hpid,
    LPADDR lpaddr1,
    LPADDR lpaddr2,
    LPLONG lpdwResult
    );

//
// Program Load / Unload
//

XOSD
OSDAPI
OSDSpawnOrphan(
    HPID hpid,
    LPCTSTR lszRemoteExe,
    LPCTSTR	lszCmdLineArgs,
    LPCTSTR	lszRemoteDir,
    DWORD dwFlags,
	PROCESS_ID*	ProcessId
    );

XOSD
OSDAPI
OSDProgramLoad(
    HPID       hpid,
    LPTSTR     lszRemoteExe,
    LPTSTR     lszArgs,
    LPTSTR     lszWorkingDir,
    LPTSTR     lszDebugger,
    DWORD      dwFlags,
	LAUNCHMODE mode
    );

XOSD
OSDAPI
OSDDebugActive(
    HPID hpid,
    LPVOID lpvData,
    DWORD cbData
    );


XOSD
OSDAPI
OSDDetach(
    HPID hpid,
    DWORD dwFlags
    );

XOSD
OSDAPI
OSDLoadCrashDump(
	IN HPID hpid,
	IN LPCTSTR szDumpFile
	);

XOSD
OSDAPI
OSDProgramFree(
    HPID hpid
    );

XOSD
OSDAPI
OSDSetPath(
    HPID hpid,
    DWORD fSet,
    LPTSTR lszPath
    );

//
// Target Execution
//

XOSD
OSDAPI
OSDGo(
    HPID hpid,
    HTID htid,
    LPEXOP lpexop
    );

XOSD
OSDAPI
OSDSingleStep(
    HPID hpid,
    HTID htid,
    LPEXOP lpexop
    );

XOSD
OSDAPI
OSDRangeStep(
    HPID hpid,
    HTID htid,
    LPADDR lpaddrMin,
    LPADDR lpaddrMax,
    LPEXOP lpexop
    );

XOSD
OSDAPI
OSDReturnStep(
    HPID hpid,
    HTID htid,
	HFRAME hFrame,
    LPEXOP lpexop
    );

XOSD
OSDAPI
OSDAsyncStop(
    HPID hpid,
	BOOL fFastBreak
    );


XOSD
OSDAPI
OSDNextReadableMemory(
    HPID 	  hpid,
    HTID      htid,
	UOFFSET   addrStart,
	DWORD     dwRange,
	UOFFSET*  addrNextReadable
);

//
// Target function evaluation
//

XOSD
OSDAPI
OSDSetupExecute(
    HPID hpid,
    HTID htid,
    LPHIND lphind
    );

XOSD
OSDAPI
OSDStartExecute(
    HPID hpid,
    HIND hind,
    LPADDR lpaddr,
    DWORD fIgnoreEvents,
    DWORD fFar
    );

XOSD
OSDAPI
OSDCleanUpExecute(
    HPID hpid,
    HIND hind
    );


//
// Target information
//

XOSD
OSDAPI
OSDGetDebugMetric(
    HPID hpid,
    HTID htid,
    MTRC mtrc,
    LPVOID lpv
    );


XOSD
OSDAPI
OSDGetMemoryInformation(
    HPID hpid,
    HTID htid,
    LPMEMINFO lpMemInfo
    );


XOSD
OSDGetModuleInfo(
    HPID            hpid,
    HTID            htid,
	DWORD           modIndex,
	LPMOD_INFO      lpModuleInfo
    );


//
// Target memory and objects
//


XOSD
OSDAPI
OSDReadMemory(
    HPID hpid,
    HTID htid,
    LPCADDR lpaddr,
    LPVOID lpBuffer,
    DWORD cbBuffer,
    LPDWORD lpcbRead
    );

XOSD
OSDAPI
OSDWriteMemory(
    HPID hpid,
    HTID htid,
    LPCADDR lpaddr,
    LPVOID lpBuffer,
    DWORD cbBuffer,
    LPDWORD lpcbWritten
    );

XOSD
OSDAPI
OSDVirtualAllocEx(
    HPID hpid,
    LPADDR lpaddr,
    DWORD allocSize,
	LPUOFFSET allocOffset
    );

XOSD
OSDAPI
OSDLoadLibrary(
    HPID hpid,
    PADDR paddr,
    LPCTSTR szDllName
    );

XOSD
OSDAPI
OSDGetObjectLength(
    HPID hpid,
    HTID htid,
    LPADDR lpaddr,
    LPUOFFSET lpuoffStart,
    LPUOFFSET lpuoffLength
    );

XOSD
OSDGetFunctionInformation(
    HPID hpid,
    HTID htid,
    LPADDR lpaddr,
    LPFUNCTION_INFORMATION lpFunctionInformation
    );
	
//
// Register manipulation
//

XOSD
OSDAPI
OSDGetRegDesc(
    HPID hpid,
    HTID htid,
    DWORD ird,
    LPRD lprd
    );

XOSD
OSDAPI
OSDGetFlagDesc(
    HPID hpid,
    HTID htid,
    DWORD ifd,
    LPFD lpfd
    );

XOSD
OSDAPI
OSDReadRegister(
    HPID hpid,
    HTID htid,
	HFRAME hFrame,
    DWORD dwid,
    RWReg* lpValue
    );

XOSD
OSDAPI
OSDWriteRegister(
    HPID hpid,
    HTID htid,
	HFRAME hFrame,
    DWORD dwId,
    RWReg* lpValue
    );

XOSD
OSDAPI
OSDReadFlag(
    HPID hpid,
    HTID htid,
	HFRAME hFrame,
    DWORD dwId,
    LPVOID lpValue
    );

XOSD
OSDAPI
OSDWriteFlag(
    HPID hpid,
    HTID htid,
	HFRAME hFrame,
    DWORD dwId,
    LPVOID lpValue
    );

//
// Breakpoints
//

XOSD
OSDAPI
OSDBreakpoint(
    HPID hpid,
    LPBPS lpbps
    );

//
// Assembly, Unassembly
//

XOSD
OSDAPI
OSDUnassemble(
    HPID hpid,
    HTID htid,
    LPSDI lpsdi
    );

XOSD
OSDAPI
OSDGetPrevAddr(
    HPID hpid,
    HTID htid,
    LPADDR lpaddr,
	int iCount
    );

XOSD
OSDAPI
OSDAssemble(
    HPID hpid,
    HTID htid,
    LPADDR lpaddr,
    LPTSTR lsz
    );

XOSD
OSDAPI
OSDGetCallInfo(
	HPID hpid,
	HTID htid,
	LPADDR addrStart,
	LPADDR addrEnd,
	LPDWORD pdwCount,
	LPADDR addrResult
	);

//
// Stack tracing
//

XOSD
OSDAPI
OSDGetFrame(
    HPID hpid,
    HTID htid,
	IN HFRAME hPrevStackFrame,
	OUT HFRAME*	lphStackFrame
    );

XOSD
OSDGetFrameWithinRange (
	IN HPID          hpid,
	IN HTID          htid,
	IN LPSTACK_RANGE  hStackRange,
	OUT HFRAME*      lphFrame
	);

XOSD
OSDAPI
OSDDestroyFrame(
	IN HPID	hpid,
	IN HFRAME	hStackFrame,
	IN DWORD  dwHack
	);



XOSD
OSDAPI
OSDGetImageInformation(
    IN	HPID hpid,
	IN LPCSTR szFileName,
	OUT	OSDIMAGEINFO* lpImgeInfo
    );

//
// Target File I/O
//
XOSD
OSDAPI
OSDFindFirstFile(
	IN HPID hpid,
	IN LPCSTR szFileParam,	OPTIONAL
	OUT HOSDFF* lpFindFileHandle,
	OUT OSDFINDFILEINFO* lpFileInfo
	);

XOSD
OSDAPI
OSDFindNextFile(
	IN HPID hpid,
	IN HOSDFF hFindFileHandle,
	OUT OSDFINDFILEINFO* lpFileInfo
	);

XOSD
OSDAPI
OSDCloseFindHandle(
	IN HPID hpid,
	IN HOSDFF hFindFileHandle
	);

XOSD
OSDAPI
OSDOpenFile(
	IN HPID hpid,
	IN LPCSTR szFileName,
	OUT HOSDFILE* lpFileHandle
	);

XOSD
OSDAPI
OSDReadFile(
	IN	HPID hpid,
	IN	HOSDFILE hFileHandle,
    OUT LPVOID lpBuffer,
	IN	DWORD64 nBytesToRead,
	OUT DWORD64* lpBytesRead
	);

XOSD
OSDAPI
OSDSeekFile(
	IN HPID hpid,
	IN HOSDFILE hFileHandle,
	IN DWORD64 dwLocation,
	IN DWORD64 dwOrigin
	);

XOSD
OSDAPI
OSDCloseFile(
	IN HPID hpid,
	IN HOSDFILE hFileHandle
	);

XOSD
OSDAPI
OSDWriteCrashDump(
	IN HPID hpid,
	IN DWORD dwType,
	IN LPSTR szDumpFile
	);

#if 0
XOSD
OSDAPI
OSDMakeFileHandle(
    HPID hpid,
    LPARAM lPrivateHandle,
    HOSDFILE * lphosdFile
    );

XOSD
OSDAPI
OSDDupFileHandle(
    HOSDFILE hosdFile,
    HOSDFILE * lphosdDup
    );

XOSD
OSDAPI
OSDCloseFile(
    HOSDFILE hosdFile
    );

XOSD
OSDAPI
OSDSeekFile(
    HOSDFILE hosdFile,
    DWORD dwLocationLo,
    DWORD dwLocationHi,
    DWORD dwOrigin
    );

XOSD
OSDAPI
OSDReadFile(
    HOSDFILE hosdFile,
    LPBYTE lpbBuffer,
    DWORD cbData,
    LPDWORD lpcbBytesRead
    );

XOSD
OSDAPI
OSDWriteFile(
    HOSDFILE hosdFile,
    LPBYTE lpbBuffer,
    DWORD cbData,
    LPDWORD lpdwBytesWritten
    );
#endif


//
// Exceptions
//

XOSD
OSDAPI
OSDGetExceptionState(
    HPID hpid,
    HTID htid,
    LPEXCEPTION_DESCRIPTION lpExd,
    EXCEPTION_CONTROL exf
    );

XOSD
OSDAPI
OSDSetExceptionState (
    HPID hpid,
    HTID htid,
    LPEXCEPTION_DESCRIPTION lpExd
    );


//
// Messages amd Message Maps
//

XOSD
OSDAPI
OSDGetMessageMap(
    HPID hpid,
    HTID htid,
    LPMESSAGEMAP * lplpMessageMap
    );

XOSD
OSDAPI
OSDGetMessageMaskMap(
    HPID hpid,
    HTID htid,
    LPMASKMAP * lplpMaskMap
    );

//
//	Miscellaneous control functions
//

XOSD
OSDAPI
OSDShowDebuggee(
    HPID hpid,
    DWORD fShow
    );

//
// Communication and synchronization with DM
//

XOSD
OSDAPI
OSDInfoReply(
    HPID hpid,
    HTID htid,
    LPVOID lpvData,
    DWORD cbData
    );


XOSD
OSDAPI
OSDContinue(
    HPID hpid,
    HTID htid
    );

// values for bReply above
#define	RUNTIME_ERROR_CRT		0
#define	RUNTIME_ERROR_IGNORE	1
#define	RUNTIME_ERROR_STOP		2

#define	RUNTIME_ERROR_STRLEN	512

typedef struct _tagRUNTIME_ERRORINFO
{
	DWORD dwErrorNo;
	BOOL bRealBug;
	UOFFSET pvReturnAddr;
	BYTE bRTReply;
	WCHAR wzUserMsg[RUNTIME_ERROR_STRLEN];
} RUNTIME_ERRORINFO;


//
// OS Specific info and control
//

XOSD
OSDAPI
OSDGetDebuggableTaskList(
    HPID hpid,
    LPOSDTASKLIST * lplpTaskList
    );

XOSD
OSDAPI
OSDSystemService(
    HPID hpid,
    HTID htid,
    SSVC ssvc,
    LPVOID lpvData,
    DWORD cbData,
    LPDWORD lpcbReturned
    );

XOSD
OSDAPI
OSDSetDebugMode(
    HPID hpid,
    DBM dbmService,
    LPVOID lpvData,
    DWORD cbData
    );

XOSD
OSDAPI
OSDEngineReady(
	HPID	hpid
	);


#define CRASHDUMP_EXTENSION _T("dmp")
#define MINIDUMP_EXTENSION  _T("mdmp")

#define DUMP_PATH_KEY _T("MODPATH")
#define DUMP_PATH_KEYEX _T("MODPATH=")

#define DUMP_EXT_KEY _T("EXTS")

#if 0
//XOSD
//OSDAPI
//OSDGetLastTLError(
//    HTL     hTL,
//    HPID    hpid,
//    LPSTR   Buffer,
//    ULONG   Length
//    );
#endif


#ifdef __cplusplus
} // extern "C"
#endif

