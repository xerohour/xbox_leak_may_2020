//
//
// Xbox debug interface functions
// Copyright Microsoft Corporation 2000 - 2001. All Rights Reserved.
//
//
#ifndef _XBDM_H          ;xbdm
#define _XBDM_H          ;xbdm
#ifndef _XBOXDBG_H       ;xboxdbg
#define _XBOXDBG_H       ;xboxdbg

;begin_internal
//
//
// Xbox debug interface functions -- internal version
// Copyright Microsoft Corporation 2000 - 2001. All Rights Reserved.
//
//
#ifndef _XBDMP_H
#define _XBDMP_H

;end_internal

#ifdef __cplusplus
extern "C" {
#endif

#ifdef XBDBGS
#define DMAPI
#endif

#ifndef DMAPI
#ifdef _XBDM_
#define DMAPI
#else
#define DMAPI __declspec(dllimport)
#endif
#endif

#define DMHRAPI DMAPI HRESULT __stdcall

// Notification types
#define DM_NONE 0
#define DM_BREAK 1
#define DM_DEBUGSTR 2
#define DM_EXEC 3
#define DM_SINGLESTEP 4
#define DM_MODLOAD 5
#define DM_MODUNLOAD 6
#define DM_CREATETHREAD 7
#define DM_DESTROYTHREAD 8
#define DM_EXCEPTION 9
;begin_xbdm
#define DM_CLOCKINT 10
;end_xbdm
#define DM_ASSERT 12
#define DM_DATABREAK 13
#define DM_RIP 14
;begin_xbdm
#define DM_THREADSWITCH 15
;end_xbdm
#define DM_SECTIONLOAD 16
#define DM_SECTIONUNLOAD 17
#define DM_FIBER 18
#define DM_NOTIFYMAX 18

#define DM_NOTIFICATIONMASK 0xffffff
#define DM_STOPTHREAD 0x80000000

typedef DWORD (__stdcall *PDM_NOTIFY_FUNCTION)(ULONG dwNotification, DWORD dwParam);

// Break notification structure
typedef struct _DMN_BREAK {
    PVOID Address;
    DWORD ThreadId;
} DMN_BREAK, *PDMN_BREAK;

// Data breakpoint notification structure {
typedef struct _DMN_DATABREAK {
    PVOID Address;
    DWORD ThreadId;
    DWORD BreakType;
    PVOID DataAddress;
} DMN_DATABREAK, *PDMN_DATABREAK;

// Debugstr notification structure
typedef struct _DMN_DEBUGSTR {
    DWORD ThreadId;
    DWORD Length;
    LPCSTR String;
} DMN_DEBUGSTR, *PDMN_DEBUGSTR;

// execution notification values
#define DMN_EXEC_STOP 0
#define DMN_EXEC_START 1
#define DMN_EXEC_REBOOT 2
#define DMN_EXEC_PENDING 3

// Module load notification structure
typedef struct _DMN_MODLOAD {
    char Name[MAX_PATH];
    PVOID BaseAddress;
    ULONG Size;
    ULONG TimeStamp;
    ULONG CheckSum;
    ULONG Flags;
} DMN_MODLOAD, *PDMN_MODLOAD;

#define DMN_MODFLAG_XBE     0x0001
#define DMN_MODFLAG_TLS     0x0002
;begin_internal
#define DMN_MODFLAG_PEHEADER 0x8000
;end_internal

// Section load/unload notification structure
typedef struct _DMN_SECTIONLOAD {
    char Name[MAX_PATH];
    PVOID BaseAddress;
    ULONG Size;
    USHORT Index;
    USHORT Flags;
} DMN_SECTIONLOAD, *PDMN_SECTIONLOAD;

#define DMN_SECFLAG_LOADED  0x0001

// thread create notification structure
typedef struct _DMN_CREATETHREAD {
    DWORD ThreadId;
    PVOID StartAddress;
} DMN_CREATETHREAD, *PDMN_CREATETHREAD;

// fiber create/delete notification structure
typedef struct _DMN_FIBER {
    DWORD FiberId;
    BOOL Create;
    PVOID StartAddress;
} DMN_FIBER, *PDMN_FIBER;

;begin_xbdm
// thread switch notification structure
typedef struct _DMN_THREADSWITCH {
    DWORD OldThreadId;
    DWORD NewThreadId;
} DMN_THREADSWITCH, *PDMN_THREADSWITCH;
;end_xbdm

// exception notification
typedef struct _DMN_EXCEPTION {
    DWORD ThreadId;
    DWORD Code;
    PVOID Address;
    DWORD Flags;
    DWORD Information[2];
} DMN_EXCEPTION, *PDMN_EXCEPTION;

#define DM_EXCEPT_NONCONTINUABLE 1
#define DM_EXCEPT_FIRSTCHANCE 2

;begin_xbdm
// profiling
typedef struct _DMN_PROFINT {
    DWORD Eip;
    DWORD EFlags;
    DWORD SegCs;
} DMN_PROFINT, *PDMN_PROFINT;
;end_xbdm

// Notification
#define DM_PERSISTENT 1
#define DM_DEBUGSESSION 2
#define DM_ASYNCSESSION 4
typedef struct _DMN_SESSION *PDMN_SESSION;
DMHRAPI DmOpenNotificationSession(DWORD dwFlags, PDMN_SESSION *pSession);
DMHRAPI DmCloseNotificationSession(PDMN_SESSION Session);
DMHRAPI DmNotify(PDMN_SESSION Session, DWORD dwNotification,
    PDM_NOTIFY_FUNCTION pfnHandler);

// notification extensions
typedef DWORD (__stdcall *PDM_EXT_NOTIFY_FUNCTION)(LPCSTR szNotification);
DMHRAPI DmRegisterNotificationProcessor(PDMN_SESSION Session, LPCSTR szType,
    PDM_EXT_NOTIFY_FUNCTION pfn);

// protocol
#define DEBUGGER_PORT 0x2db

// breakpoint types
#define DMBREAK_NONE 0
#define DMBREAK_WRITE 1
#define DMBREAK_READWRITE 2
#define DMBREAK_EXECUTE 3
#define DMBREAK_FIXED 4

// breakpoint routines
DMHRAPI DmSetBreakpoint(PVOID addr);
DMHRAPI DmRemoveBreakpoint(PVOID addr);
DMHRAPI DmSetInitialBreakpoint(void);
DMHRAPI DmSetDataBreakpoint(PVOID addr, DWORD dwType, DWORD dwSize);
DMHRAPI DmIsBreakpoint(PVOID addr, LPDWORD dwType);

// execution start and stop
DMHRAPI DmStop(void);
DMHRAPI DmGo(void);
DMHRAPI DmHaltThread(DWORD dwThreadId);
DMHRAPI DmContinueThread(DWORD dwThreadId, BOOL fException);
DMHRAPI DmSetupFunctionCall(DWORD dwThreadId);

// debugger
;begin_xbdm
DMAPI BOOL __stdcall DmIsDebuggerPresent(void);
;end_xbdm
;begin_xboxdbg
DMHRAPI DmConnectDebugger(BOOL fConnect);
;end_xboxdbg

// event stop control
#define DMSTOP_CREATETHREAD 1
#define DMSTOP_FCE 2
#define DMSTOP_DEBUGSTR 4
DMHRAPI DmStopOn(DWORD dwStopFlags, BOOL fStop);

// reboot
#define DMBOOT_WAIT 1
#define DMBOOT_WARM 2
#define DMBOOT_NODEBUG 4
#define DMBOOT_STOP 8
DMHRAPI DmReboot(DWORD dwFlags);

// memory
DMHRAPI DmGetMemory(LPCVOID lpbAddr, DWORD cb, LPVOID lpbBuf,
    LPDWORD pcbRet);
DMHRAPI DmSetMemory(LPVOID lpbAddr, DWORD cb, LPCVOID lpbBuf,
    LPDWORD pcbRet);

;begin_xbdm
// pool memory
DMAPI PVOID __stdcall DmAllocatePool(ULONG cb);
DMAPI PVOID __stdcall DmAllocatePoolWithTag(ULONG cb, ULONG tag);
DMAPI VOID __stdcall DmFreePool(PVOID p);

// profile interrupts
typedef void (__stdcall *PDMPROFILE_HANDLER)(PDMN_PROFINT);
DMHRAPI DmStartProfile(PHANDLE, ULONG, PDMPROFILE_HANDLER);
DMHRAPI DmStopProfile(HANDLE);
;end_xbdm

// thread stopped info
typedef struct _DM_THREADSTOP {
    DWORD NotifiedReason;
    union {
        DMN_BREAK Break;
        DMN_DATABREAK DataBreak;
        DMN_EXCEPTION Exception;
        DMN_DEBUGSTR DebugStr;
    } u;
} DM_THREADSTOP, *PDM_THREADSTOP;

// thread general info
typedef struct _DM_THREADINFO {
    DWORD SuspendCount;
    DWORD Priority;
    LPVOID TlsBase;
} DM_THREADINFO, *PDM_THREADINFO;

// thread routines
DMHRAPI DmGetThreadList(LPDWORD rgdwThreads, LPDWORD pcThreads);
DMHRAPI DmGetThreadContext(DWORD dwThreadId, PCONTEXT pdmcr);
DMHRAPI DmSetThreadContext(DWORD dwThreadId, PCONTEXT pdmcr);
DMHRAPI DmIsThreadStopped(DWORD dwThreadId, PDM_THREADSTOP pdmts);
DMHRAPI DmGetThreadInfo(DWORD dwThreadId, PDM_THREADINFO pdmti);
DMHRAPI DmSuspendThread(DWORD dwThreadId);
DMHRAPI DmResumeThread(DWORD dwThreadId);

// XTL data
typedef struct _DM_XTLDATA {
    DWORD LastErrorOffset;
} DM_XTLDATA, *PDM_XTLDATA;

DMHRAPI DmGetXtlData(PDM_XTLDATA);

// loaded modules and sections
typedef struct _DM_WALK_MODULES *PDM_WALK_MODULES;
DMHRAPI DmWalkLoadedModules(PDM_WALK_MODULES *, PDMN_MODLOAD);
DMHRAPI DmCloseLoadedModules(PDM_WALK_MODULES);
typedef struct _DM_WALK_MODSECT *PDM_WALK_MODSECT;
DMHRAPI DmWalkModuleSections(PDM_WALK_MODSECT *, LPCSTR, PDMN_SECTIONLOAD);
DMHRAPI DmCloseModuleSections(PDM_WALK_MODSECT);
DMHRAPI DmGetModuleLongName(LPCSTR szShortName, LPSTR szLongName, LPDWORD pcch);

// XBE info
typedef struct _DM_XBE {
    char LaunchPath[MAX_PATH+1];
    DWORD TimeStamp;
    DWORD CheckSum;
    DWORD StackSize;
} DM_XBE, *PDM_XBE;
DMHRAPI DmGetXbeInfo(LPCSTR szName, PDM_XBE);

;begin_xbdm
// command extension
typedef ULONG (__stdcall *PDM_ENTRYPROC)(ULONG, ULONG, ULONG);

typedef struct _DM_CMDCONT *PDM_CMDCONT;
typedef HRESULT (__stdcall *PDM_CMDCONTPROC)(PDM_CMDCONT pdmcc, LPSTR
    szResponse, DWORD cchResponse);
typedef struct _DM_CMDCONT {
    PDM_CMDCONTPROC HandlingFunction;
    DWORD DataSize;
    PVOID Buffer;
    DWORD BufferSize;
    PVOID CustomData;
    DWORD BytesRemaining;
} DM_CMDCONT;

typedef HRESULT (__stdcall *PDM_CMDPROC)(LPCSTR szCommand, LPSTR szResponse,
    DWORD cchResponse, PDM_CMDCONT pdmcc);
DMHRAPI DmRegisterCommandProcessor(LPCSTR szProcessor, PDM_CMDPROC pfn);
DMHRAPI DmRegisterCommandProcessorEx(LPCSTR szProcessor, PDM_CMDPROC pfn,
    PVOID pfnCreateThread);

#define DmRegisterThreadedCommandProcessor(sz, pfn) \
    DmRegisterCommandProcessorEx(sz, pfn, CreateThread)

DMHRAPI DmSendNotificationString(LPCSTR sz);

// per-thread data
DMHRAPI DmThreadUserData(DWORD tid, LPDWORD *ppdwData);
#define DM_CURRENT_THREAD -1

// Dynamic loading of debugger extensions
DMHRAPI DmLoadExtension(LPCSTR szName, PHANDLE phModule, PVOID *pvBase);
DMHRAPI DmUnloadExtension(HANDLE hModule);
DMHRAPI DmGetProcAddress(HANDLE hModule, LPCSTR szProcName, PVOID *ppvRet);
;end_xbdm
;begin_xboxdbg
// socket-level commands
typedef struct _DM_CONNECTION *PDM_CONNECTION;
DMHRAPI DmSetConnectionTimeout(DWORD dwConnectTimeout, DWORD dwConversationTimeout);
DMHRAPI DmOpenConnection(PDM_CONNECTION *);
DMHRAPI DmOpenSecureConnection(PDM_CONNECTION *, LPCSTR szPassword);
DMHRAPI DmCloseConnection(PDM_CONNECTION);
DMHRAPI DmSendCommand(PDM_CONNECTION, LPCSTR szCommand, LPSTR szResponse,
    LPDWORD lpdwResponseSize);
DMHRAPI DmReceiveStatusResponse(PDM_CONNECTION, LPSTR szResponse,
    LPDWORD lpdwResponseSize);
DMHRAPI DmReceiveSocketLine(PDM_CONNECTION, LPSTR szResponse,
    LPDWORD lpdwResponseSize);
DMHRAPI DmDedicateConnection(PDM_CONNECTION, LPCSTR szHandler);
DMHRAPI DmSendBinary(PDM_CONNECTION, LPCVOID pv, DWORD cb);
DMHRAPI DmReceiveBinary(PDM_CONNECTION, LPCVOID pv, DWORD cb, LPDWORD pcbRet);
DMHRAPI DmResolveXboxName(LPDWORD lpdwAddress);
DMHRAPI DmGetNameOfXbox(LPSTR szName, LPDWORD lpdwSize, BOOL fResolvable);
DMHRAPI DmGetAltAddress(LPDWORD lpdwAddress);

// protocol commands
DMHRAPI DmUseSharedConnection(BOOL);

// file attributes
typedef struct _DM_FILE_ATTRIBUTES {
    char Name[256];
    FILETIME CreationTime;
    FILETIME ChangeTime;
    DWORD SizeHigh;
    DWORD SizeLow;
    DWORD Attributes;
} DM_FILE_ATTRIBUTES, *PDM_FILE_ATTRIBUTES;

// filesystem
DMHRAPI DmSendFile%(LPCTSTR% szLocalName, LPCSTR szRemoteName);

DMHRAPI DmReceiveFile%(LPCTSTR% szLocalName, LPCSTR szRemoteName);

DMHRAPI DmGetFileAttributes(LPCSTR szFileName, PDM_FILE_ATTRIBUTES);
DMHRAPI DmSetFileAttributes(LPCSTR szFileName, PDM_FILE_ATTRIBUTES);
DMHRAPI DmMkdir(LPCSTR szDirectoryName);
DMHRAPI DmRenameFile(LPCSTR szOldName, LPCSTR szNewName);
DMHRAPI DmDeleteFile(LPCSTR szFileName, BOOL fIsDirectory);

// directory walking
typedef struct _DM_WALK_DIR *PDM_WALK_DIR;
DMHRAPI DmWalkDir(PDM_WALK_DIR *, LPCSTR szDir, PDM_FILE_ATTRIBUTES);
DMHRAPI DmCloseDir(PDM_WALK_DIR);

// time
DMHRAPI DmGetSystemTime(LPSYSTEMTIME lpSysTime);

// config functions
DMHRAPI DmSetConfigValue(ULONG ulValueIndex, ULONG ulType, PVOID pValue, ULONG cbValueLength);
;end_xboxdbg

// name functions
DMHRAPI DmGetXboxName(LPSTR, LPDWORD);
DMHRAPI DmSetXboxName(LPCSTR);
;begin_xboxdbg
DMHRAPI DmSetXboxNameNoRegister(LPCSTR);
;end_xboxdbg

// management functions
typedef struct _DM_USER {
    char UserName[256];
    DWORD AccessPrivileges;
} DM_USER, *PDM_USER;

#define DMPL_PRIV_READ           0x0001
#define DMPL_PRIV_WRITE          0x0002
#define DMPL_PRIV_CONTROL        0x0004
#define DMPL_PRIV_CONFIGURE      0x0008
#define DMPL_PRIV_MANAGE         0x0010

DMHRAPI DmAddUser(LPCSTR szUserName, DWORD dwAccess);
DMHRAPI DmRemoveUser(LPCSTR szUserName);
DMHRAPI DmSetUserAccess(LPCSTR szUserName, DWORD dwAccess);
DMHRAPI DmGetUserAccess(LPCSTR szUserName, LPDWORD lpdwAccess);
typedef struct _DM_WALK_USERS *PDM_WALK_USERS;
DMHRAPI DmWalkUserList(PDM_WALK_USERS *, PDM_USER);
DMHRAPI DmCloseUserList(PDM_WALK_USERS);
DMHRAPI DmEnableSecurity(BOOL fEnable);
DMHRAPI DmIsSecurityEnabled(LPBOOL pfEnabled);
;begin_xboxdbg
DMHRAPI DmSetAdminPassword(LPCSTR szPassword);
;end_xboxdbg

// title to launch
DMHRAPI DmSetTitle(LPCSTR szDir, LPCSTR szTitle, LPCSTR szCmdLine);

;begin_xboxdbg
// Screenshot functionality
DMHRAPI DmScreenShot(LPCSTR filename);
// Pixel shader snapshot
DMHRAPI DmPixelShaderSnapshot(DWORD dwX, DWORD dwY, DWORD dwFlags, DWORD dwMarker, BYTE *pBuf);
// Vertex shader snapshot
DMHRAPI DmVertexShaderSnapshot(DWORD dwFirst, DWORD dwLast, DWORD dwFlags, DWORD dwMarker, BYTE *pBuf);
;end_xboxdbg

// start / stop profile data collection
DMHRAPI DmCAPControl(LPCSTR action);

// performance counters
#define DMCOUNT_FREQUENCY   0x000F
#define DMCOUNT_FREQ100MS   0x0001
#define DMCOUNT_FREQ1SEC    0x000A

#define DMCOUNT_COUNTTYPE   0x0030
#define DMCOUNT_EVENT       0x0010
#define DMCOUNT_VALUE       0x0000
#define DMCOUNT_PRATIO      0x0020
#define DMCOUNT_COUNTSUBTYPE 0x0FC0

// event rates
#define DMCOUNT_PERSEC      0x0040
#define DMCOUNT_PERMSEC     0x0080
#define DMCOUNT_PERFRAME    0x0100
#define DMCOUNT_PERTICK     0x0200

// value types
#define DMCOUNT_AVERAGE     0x0040

typedef struct _DM_COUNTDATA {
    LARGE_INTEGER CountValue;
    LARGE_INTEGER RateValue;
    DWORD CountType;
} DM_COUNTDATA, *PDM_COUNTDATA;

typedef struct _DM_COUNTINFO {
    char Name[256];
    DWORD Type;
} DM_COUNTINFO, *PDM_COUNTINFO;

;begin_xbdm
DMHRAPI DmOpenPerformanceCounter(LPCSTR szName, HANDLE *phCounter);
DMHRAPI DmQueryPerformanceCounterHandle(HANDLE hCounter, DWORD dwType, PDM_COUNTDATA);
DMHRAPI DmClosePerformanceCounter(HANDLE hCounter);
;end_xbdm
;begin_xboxdbg
DMHRAPI DmQueryPerformanceCounter(LPCSTR szName, DWORD dwType, PDM_COUNTDATA);
;end_xboxdbg
typedef struct _DM_WALK_COUNTERS *PDM_WALK_COUNTERS;
DMHRAPI DmWalkPerformanceCounters(PDM_WALK_COUNTERS *, PDM_COUNTINFO);
DMHRAPI DmCloseCounters(PDM_WALK_COUNTERS);
DMHRAPI DmEnableGPUCounter(BOOL);

;begin_xboxdbg
DMHRAPI DmGetDriveList(LPSTR rgchDrives, DWORD *pcDrives);
DMHRAPI DmGetDiskFreeSpace(char *szDrive,
                           PULARGE_INTEGER pnFreeBytesAvailableToCaller,
                           PULARGE_INTEGER pnTotalNumberOfBytes,
                           PULARGE_INTEGER pnTotalNumberOfFreeBytes);
;end_xboxdbg

;begin_xbdm
typedef HRESULT (__stdcall *PDM_COUNTPROC)(PLARGE_INTEGER, PLARGE_INTEGER);
#define DMCOUNT_SYNC      0x00010000
#define DMCOUNT_ASYNC32   0x00020000
#define DMCOUNT_ASYNC64   0x00040000
#define DMCOUNT_ASYNC     0x00080000
;begin_internal
#define DMCOUNT_SYNCCD    0x00100000
;end_internal
DMHRAPI DmRegisterPerformanceCounter(LPCSTR szName, DWORD dwType, PVOID);
DMHRAPI DmUnregisterPerformanceCounter(LPCSTR szName);
;end_xbdm

// error codes
#define FACILITY_XBDM 0x2db
#ifndef MAKE_HRESULT
#define MAKE_HRESULT(sev,fac,code) \
    ((HRESULT)(((unsigned long)sev<<31)|((unsigned long)fac<<16)|((unsigned long)code)))
#endif
#define XBDM_HRESERR(code) MAKE_HRESULT(1, FACILITY_XBDM, code)
#define XBDM_HRESSUCC(code) MAKE_HRESULT(0, FACILITY_XBDM, code)

#define XBDM_NOERR XBDM_HRESSUCC(0)

#define XBDM_UNDEFINED XBDM_HRESERR(0)
#define XBDM_MAXCONNECT XBDM_HRESERR(1)
#define XBDM_NOSUCHFILE XBDM_HRESERR(2)
#define XBDM_NOMODULE XBDM_HRESERR(3)
#define XBDM_MEMUNMAPPED XBDM_HRESERR(4)
#define XBDM_NOTHREAD XBDM_HRESERR(5)
#define XBDM_CLOCKNOTSET XBDM_HRESERR(6)
#define XBDM_INVALIDCMD XBDM_HRESERR(7)
#define XBDM_NOTSTOPPED XBDM_HRESERR(8)
#define XBDM_MUSTCOPY XBDM_HRESERR(9)
#define XBDM_ALREADYEXISTS XBDM_HRESERR(10)
#define XBDM_DIRNOTEMPTY XBDM_HRESERR(11)
#define XBDM_BADFILENAME XBDM_HRESERR(12)
#define XBDM_CANNOTCREATE XBDM_HRESERR(13)
#define XBDM_CANNOTACCESS XBDM_HRESERR(14)
#define XBDM_DEVICEFULL XBDM_HRESERR(15)
#define XBDM_NOTDEBUGGABLE XBDM_HRESERR(16)
#define XBDM_BADCOUNTTYPE XBDM_HRESERR(17)
#define XBDM_COUNTUNAVAILABLE XBDM_HRESERR(18)
#define XBDM_NOTLOCKED XBDM_HRESERR(20)
#define XBDM_KEYXCHG XBDM_HRESERR(21)
#define XBDM_MUSTBEDEDICATED XBDM_HRESERR(22)
#define XBDM_CANNOTCONNECT XBDM_HRESERR(0x100)
#define XBDM_CONNECTIONLOST XBDM_HRESERR(0x101)
#define XBDM_FILEERROR XBDM_HRESERR(0x103)
#define XBDM_ENDOFLIST XBDM_HRESERR(0x104)
#define XBDM_BUFFER_TOO_SMALL XBDM_HRESERR(0x105)
#define XBDM_NOTXBEFILE XBDM_HRESERR(0x106)
#define XBDM_MEMSETINCOMPLETE XBDM_HRESERR(0x107)
#define XBDM_NOXBOXNAME XBDM_HRESERR(0x108)
#define XBDM_NOERRORSTRING XBDM_HRESERR(0x109)

#define XBDM_CONNECTED XBDM_HRESSUCC(1)
#define XBDM_MULTIRESPONSE XBDM_HRESSUCC(2)
#define XBDM_BINRESPONSE XBDM_HRESSUCC(3)
#define XBDM_READYFORBIN XBDM_HRESSUCC(4)
#define XBDM_DEDICATED XBDM_HRESSUCC(5)

;begin_xbdm
// Call Attributes Profiler Support Function
#define DM_PROFILE_START    1
#define DM_PROFILE_STOP     2

DWORD WINAPI DmProfileControl(DWORD Action, DWORD Parameter);
;end_xbdm

;begin_xboxdbg
#ifndef XBDBGS
DMHRAPI DmTranslateError%(HRESULT hr, LPTSTR% lpBuffer, int nBufferMax);
#endif
;end_xboxdbg

;begin_xboxdbg
// Image file routines
DMHRAPI XbeGetLocalModulePath%(LPCTSTR% szXbe, LPCSTR szModule, LPSTR szPath,
    DWORD cchPath);
DMHRAPI XbeGetXbeInfo%(LPCTSTR% szXbe, PDM_XBE pXbe);
DMHRAPI XbeGetBuilderArgs%(LPCTSTR% szXbe, LPSTR szArgs, LPDWORD pcch);
;end_xboxdbg

#ifdef __cplusplus
}
#endif
;begin_both

#endif
;end_both
