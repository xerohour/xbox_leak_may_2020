/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    dmx32.c

Abstract:

Author:

    Wesley Witt (wesw) 15-Aug-1992

Environment:

    NT 4.0, Win95

Revision History:

--*/
#include "precomp.h"
#pragma hdrstop

#include "vcexcept.h"
#ifdef OMNI
#include "OMNI.h"
#endif
#ifndef XBOX
#include "tllp.h"
#endif
#include "fiber.h"

//DBF *lpdbf;

#undef LOCAL

typedef enum {
    Image_Unknown,
    Image_16,
    Image_32,
    Image_Dump
} IMAGETYPE;
typedef IMAGETYPE *PIMAGETYPE;

enum {
    System_Invalid = -1,             /* The exe can not be debugged  */
    System_Console =  1,             /* The exe needs a console      */
    System_GUI     =  0              /* The exe is a Windows exe     */
};


ULONG	gTraceFlag = 0;

BOOL
InRpcrt4(
	HPRCX	hthd,
	ULONG	ReturnDestination
	);

void
ActionAsyncStop(
	DEBUG_EVENT*	de,
	HTHDX			hthd,
	DWORD			unused,
	LPVOID			lpv
	);
	
/////
// Taken from vspawn.h
/////
const TCHAR szVCSpawnCmd[] = _T("vcspawn.exe");  // No exe?
const TCHAR szVCSToolFlag[] = _T(" -t ");  
const TCHAR szVCSPauseFlag[] = _T(" -p ");


static   TCHAR cModuleDemarcator = _T('|');

int    pCharMode(LPTSTR szAppName, PIMAGETYPE Image);

TCHAR       rgchDebug[256];
BOOL        FVerbose = 0;

// std{in,out,err} for redirection of console apps.  See ProcessDebuggeeRedirection()
static HANDLE           rgh[3] = {0, 0, 0};

DMTLFUNCTYPE        DmTlFunc = NULL;

static BOOL FDMRemote = FALSE;  // set true for remote debug

BOOL FUseOutputDebugString = FALSE;

BOOL fApprovedAttach;

BOOL FXSAVESupported = FALSE;

EXPECTED_EVENT  masterEE = {0L,0L};
EXPECTED_EVENT *eeList = &masterEE;

static HTHDXSTRUCT masterTH = {0L,0L};
HTHDX       thdList = &masterTH;

static HPRCXSTRUCT masterPR = {0L,0L};
HPRCX       prcList = &masterPR;

// control access to thread and process lists:
CRITICAL_SECTION csThreadProcList;
CRITICAL_SECTION csFXOps;
CRITICAL_SECTION csEventList;

// control access to Walk list
CRITICAL_SECTION    csWalk;

extern PDMN_SESSION psessNotify;

HPID hpidRoot = (HPID)INVALID;  // this hpid is our hook to the native EM
BOOL fUseRoot;                  // next CREATE_PROCESS will use hpidRoot

DEBUG_EVENT falseSSEvent;
DEBUG_EVENT falseBPEvent;
DEBUG_EVENT FuncExitEvent;
METHOD      EMNotifyMethod;

// Don't allow debug event processing during some shell operations
CRITICAL_SECTION csProcessDebugEvent;

// Event handles for synchronizing with the shell on proc/thread creates.
HANDLE hEventCreateProcess;
HANDLE hEventContinue;

// event handle for synchronizing connnect/reconnect with the tl
HANDLE hEventRemoteQuit;

HANDLE hEventNoDebuggee;        // set when no debuggee is attached

int    nWaitingForLdrBreakpoint = 0;

BOOL    fDisconnected = FALSE;

DWORD g_ProcFlags = 0;
BOOL fxFlagSet = FALSE;
DEBUG_EVENT deBackup;

#ifndef KERNEL
//
// crash dump stuff
//
#if 0
BOOL                            CrashDump;
PCONTEXT                        CrashContext;
PEXCEPTION_RECORD               CrashException;
PUSERMODE_CRASHDUMP_HEADER      CrashDumpHeader;
#endif
ULONG                           KiPcrBaseAddress;
ULONG                           KiProcessors;

HANDLE hDmPollThread = 0;       // Handle for event loop thread.
BOOL   fDmPollQuit = FALSE;     // tell poll thread to exit NOW
HANDLE hTerminateThread = 0;	// Handle for Terminate Thread

typedef DWORD ( WINAPI *LPFNGETNAME )
(
    HANDLE hProcess,
    HMODULE hModule,
    LPSTR lpFilename,
    DWORD nSize
);

//HANDLE hModPsapi = NULL;
BOOL   fAttemptedLoadingPsapi = FALSE;
LPFNGETNAME pFnGetModuleFileNameExA = NULL;

#if 0
SYSTEM_INFO SystemInfo;
OSVERSIONINFO OsVersionInfo;
#endif

WT_STRUCT             WtStruct;             // ..  for wt

VOID
Cleanup(
    VOID
    );

DWORD WINAPI
CallDmPoll(
    LPVOID lpv
    );

VOID
CrashDumpThread(
    LPVOID lpv
    );

void Close3Handles(
        HANDLE *rgh
        );

#if 0
XOSD
ProcessDebuggeeRedirection(
        LPTSTR lszCommandLine,
    STARTUPINFO FAR * psi
    );
#endif

BOOL
SetDebugPrivilege(
    void
    );

#ifdef OMNI
VOID ProcessOmniDllLoadEvent(DEBUG_EVENT* pdeIn, HTHDX hthd);
#endif
#endif // !KERNEL

#ifdef KERNEL
extern BOOL fCrashDump;

#ifdef NTKERNEL
KDOPTIONS KdOptions[] = {
    _T("BaudRate"),        KDO_BAUDRATE,      KDT_DWORD,     9600,
    _T("Port"),            KDO_PORT,          KDT_DWORD,     2,
    _T("Cache"),           KDO_CACHE,         KDT_DWORD,     8192,
    _T("Verbose"),         KDO_VERBOSE,       KDT_DWORD,     0,
    _T("InitialBp"),       KDO_INITIALBP,     KDT_DWORD,     0,
    _T("Defer"),           KDO_DEFER,         KDT_DWORD,     0,
    _T("UseModem"),        KDO_USEMODEM,      KDT_DWORD,     0,
    _T("LogfileAppend"),   KDO_LOGFILEAPPEND, KDT_DWORD,     0,
    _T("GoExit"),          KDO_GOEXIT,        KDT_DWORD,     0,
    _T("SymbolPath"),      KDO_SYMBOLPATH,    KDT_STRING,    0,
    _T("LogfileName"),     KDO_LOGFILENAME,   KDT_STRING,    0,
    _T("CrashDump"),       KDO_CRASHDUMP,     KDT_STRING,    0
};

VOID
GetKernelSymbolAddresses(
    VOID
    );

MODULEALIAS  ModuleAlias[MAX_MODULEALIAS];
#endif

#endif  // KERNEL


TCHAR  nameBuffer[256];

// Reply buffers to and from em
char  abEMReplyBuf[1024];       // Buffer for EM to reply to us in
char  abDMReplyBuf[1024];       // Buffer for us to reply to EM requests in
LPDM_MSG LpDmMsg = (LPDM_MSG)abDMReplyBuf;

// To send a reply of the struct msMyStruct, do this:
//      LpDmMsg->xosdRet = xosdMyReturnValue
//      memcpy (LpDmMsg->rgb, &msMyStruct, sizeof (msMyStruct));
//      Reply (sizeof (msMyStruct), LpDmMsg, hpid);

DDVECTOR DebugDispatchTable[] = {
    ProcessExceptionEvent,
    ProcessCreateThreadEvent,
    ProcessCreateProcessEvent,
    ProcessExitThreadEvent,
    ProcessExitProcessEvent,
    ProcessLoadDLLEvent,
    ProcessUnloadDLLEvent,
    ProcessOutputDebugStringEvent,
    ProcessRipEvent,
    ProcessBreakpointEvent,
    NULL,                       /* CHECK_BREAKPOINT_DEBUG_EVENT */
    ProcessSegmentLoadEvent,    /* SEGMENT_LOAD_DEBUG_EVENT */
    NULL,                       /* DESTROY_PROCESS_DEBUG_EVENT */
    NULL,                       /* DESTROY_THREAD_DEBUG_EVENT */
    NULL,                       /* ATTACH_DEADLOCK_DEBUG_EVENT */
    ProcessEntryPointEvent,     /* ENTRYPOINT_DEBUG_EVENT */
    NULL,                       /* LOAD_COMPLETE_DEBUG_EVENT */
    NULL,                       /* INPUT_DEBUG_STRING_EVENT */
    NULL,                       /* MESSAGE_DEBUG_EVENT */
    NULL,                       /* MESSAGE_SEND_DEBUG_EVENT */
    NULL,                       /* FUNC_EXIT_EVENT */
#ifdef KERNEL
    NULL,
#else
    ProcessOleEvent,            /* OLE_DEBUG_EVENT */
    ProcessFiberEvent,
#endif
    NULL,                       /* GENERIC_DEBUG_EVENT */
#ifdef KERNEL
    NULL,
	NULL,
#else
    ProcessBogusSSEvent,        /* BOGUS_WIN95_SINGLESTEP_EVENT */
	ProcessSpecialEvent,		/* SPECIAL_DEBUGGER_EVENT */
#endif
#ifdef OMNI
	ProcessOmniDllLoadEvent,
#else
	NULL,
#endif

    NULL
};

/*
 *  This array contains the set of default actions to be taken for
 *      all debug events if the thread has the "In Function Evaluation"
 *      bit set.
 */

DDVECTOR RgfnFuncEventDispatch[] = {
    EvntException,
    NULL,                       /* This can never happen */
    NULL,                       /* This can never happen */
    ProcessExitThreadEvent,
    EvntExitProcess,
    ProcessLoadDLLEvent,        /* Use normal processing */
    ProcessUnloadDLLEvent,      /* Use normal processing */
    ProcessOutputDebugStringEvent, /* Use normal processing */
    NULL,
    EvntBreakpoint,             /* Breakpoint processor */
    NULL,
    ProcessSegmentLoadEvent,    /* WOW event */
    NULL,
    NULL,
    NULL,
    ProcessEntryPointEvent,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
        ProcessBogusSSEvent,
#ifdef KERNEL
		NULL,
#else
		ProcessSpecialEvent,
#endif
    NULL
};

void    UNREFERENCED_PARAMETERS(LPVOID lpv,...)
{
    lpv=NULL;
}

#if 0
SPAWN_STRUCT          SpawnStruct;          // packet for causing CreateProcess()
#endif

DEBUG_ACTIVE_STRUCT   DebugActiveStruct;    // ... for DebugActiveProcess()

PKILLSTRUCT           KillQueue;
CRITICAL_SECTION      csKillQueue;

BOOL IsExceptionIgnored(HPRCX, DWORD);

TCHAR SearchPathString[ 10000 ];
BOOL SearchPathSet;
BOOL fUseRealName = FALSE;

#ifndef XBOX
HINSTANCE hInstance; // The DM DLL's hInstance

LPTSTR
FmtMsg(
    int msgid
    )
{
    void * lpb;

    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                      FORMAT_MESSAGE_IGNORE_INSERTS |
                      FORMAT_MESSAGE_FROM_SYSTEM |
                      0, 0, msgid, 0, (LPTSTR)&lpb, 0, 0))
    {
        return (LPTSTR) lpb;
    }
    else
    {
        return NULL;
    }
}
#endif

TCHAR szErrArg[_MAX_PATH];

LPTSTR
LszFromErr(
    DWORD dwErr
    )
{
#ifndef XBOX // LOCBUG
    #define NTERR(name) ERROR_##name, IDS_##name,
    static struct {
        DWORD   dwErr;
        UINT    idsErr;
    } mpiszErr [] = {
        NTERR(FILE_NOT_FOUND)
        NTERR(PATH_NOT_FOUND)
        NTERR(INVALID_HANDLE)
        NTERR(INVALID_DRIVE)
        NTERR(INVALID_PARAMETER)
        NTERR(INVALID_NAME)
        NTERR(BAD_PATHNAME)
        NTERR(FILENAME_EXCED_RANGE)
        NTERR(OUTOFMEMORY)
        NTERR(NOT_ENOUGH_MEMORY)
        NTERR(ACCESS_DENIED)
        NTERR(SHARING_VIOLATION)
        NTERR(OPEN_FAILED)
        NTERR(BAD_FORMAT)
        NTERR(CHILD_NOT_COMPLETE)
        NTERR(INVALID_MODULETYPE)
        NTERR(INVALID_EXE_SIGNATURE)
        NTERR(EXE_MARKED_INVALID)
        NTERR(BAD_EXE_FORMAT)
        NTERR(DIRECTORY)
        0,  IDS_UNKNOWN_ERROR
    };
#endif
    static TCHAR rgchErr[256+_MAX_PATH]; // must be static!
    static TCHAR rgchErrNum[256];
    LPTSTR sz = rgchErr;
    LPTSTR lszFmtErr = NULL;
    int i;

#ifdef XBOX // LOCBUG
	strcpy(sz, "debug error: ");
#else
    if (!LoadString(hInstance, IDS_COULD_NOT_LOAD_DEBUGGEE,
            sz, rgchErr + _tsizeof(rgchErr) - sz)) {
        assert(FALSE);
    }
#endif

    sz += _tcslen(sz);

    // If there's an argument to put in the string, put it here
    if (szErrArg[0]) {
        _tcscpy(sz, szErrArg);
        _tcscat(sz, _T(": "));
        sz += _tcslen(sz);
    }

#ifndef XBOX
    for (i = 0; mpiszErr[i].dwErr; ++i) {
        if (mpiszErr[ i ].dwErr == dwErr) {
            break;
        }
    }

    // If we didn't find an error string in our list, call FormatMessage
    // to get an error message from the operating system.
    if (mpiszErr[i].dwErr == 0) {
        lszFmtErr = FmtMsg(dwErr);
    }

    // If we got an error message from the operating system, display that,
    // otherwise display our own message.
    if (lszFmtErr) {
        _tcsncpy(sz, lszFmtErr, rgchErr + _tsizeof(rgchErr) - sz);
        rgchErr[_tsizeof(rgchErr)-1] = _T('\0');
    }
    else {
        // Even if we got through the above loop without finding a match,
        // we're okay, because the mpiszErr[] table ends with "unknown error".

        if (!LoadString(hInstance, mpiszErr[i].idsErr,
                sz, rgchErr+_tsizeof(rgchErr)-sz)) {
            assert(FALSE);
        }
    }
#endif

    sz += _tcslen(sz);

#ifndef XBOX
    if (!LoadString(hInstance, IDS_NTError,
            rgchErrNum, _tsizeof(rgchErrNum))) {
        assert(FALSE);
    }
    sz += _stprintf(sz, rgchErrNum, dwErr);
#endif

    return rgchErr;
}

void
SendNTError(
    HPRCX hprc,
    DWORD dwErr,
    LPTSTR lszString
    )
{
    LPTSTR     lszError;

    if (lszString) {
        lszError = lszString;
    }
    else {
        lszError = (LPTSTR) LszFromErr(dwErr);
    }

        SendDBCError(hprc, dwErr, lszError);
}

void
SendDBCError(
    HPRCX hprc,
    DWORD dwErr,
    LPTSTR lszString
    )
{
    typedef struct _LE {
        XOSD    xosd;
        TCHAR   rgchErr[];
    } LE;       // load error: passed with dbcError

    LE FAR *    ple;
    LPRTP       lprtp;
    DWORD       cbBuf;
    LPCTSTR     lszError;

        if (lszString) {
                lszError = lszString;
        }
        else {
                lszError = "";  // Empty string.
        }

    cbBuf = FIELD_OFFSET(RTP, rgbVar) + sizeof(LE) + (_tcslen(lszError) + 1)*sizeof(TCHAR);
    lprtp = (LPRTP) MHAlloc(cbBuf);
    assert(lprtp);
    ple = (LE *) (lprtp->rgbVar);

    ple->xosd = (XOSD) dwErr;
    _tcscpy(ple->rgchErr, lszError);
    lprtp->dbc = dbcError;

    if (hprc != NULL) {
                lprtp->hpid= hprc->hpid;
        } else {
                lprtp->hpid = hpidRoot;
    }

    lprtp->htid = NULL;
    lprtp->cb = cbBuf;
    DmTlFunc(tlfDebugPacket, lprtp->hpid, lprtp->cb, (LONG)(LPVOID) lprtp);
    MHFree(lprtp);
}

#if 0
BOOL
ResolveFile(
    LPTSTR   lpName,
    LPTSTR   lpFullName,
    BOOL    fUseRealName
    )
{
    DWORD   dwAttr;
    LPTSTR  lpFilePart;
    BOOL    fOk;

    if (fUseRealName) {
        dwAttr = GetFileAttributes(lpName);
        fOk = ((dwAttr != 0xffffffff)
             && ((dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0));

        if (fOk) {
            _ftcscpy(lpFullName, lpName);
        }

    } else {

        fOk = SearchPath(SearchPathString,
                         lpName,
                         NULL,
                         MAX_PATH,
                         lpFullName,
                         &lpFilePart
                         );
        if (!fOk) {
            *lpFullName = 0;
        }
    }
    return fOk;
}

#ifndef KERNEL
XOSD
Load(
    HPRCX hprc,
    LPCTSTR szAppName,
    LPCTSTR szArg,
    LPVOID pattrib,
    LPVOID tattrib,
    DWORD creationFlags,
    BOOL fInheritHandles,
    CONST LPCTSTR* environment,
    LPCTSTR currentDirectory,
    STARTUPINFO FAR * pStartupInfo,
    LPPROCESS_INFORMATION lppi
    )
/*++

Routine Description:


Arguments:


Return Value:

    TRUE if the process was successfully created and FALSE otherwise.

--*/
{
    XOSD      xosd;
    int       type;
    TCHAR     ch;
    int       fQuotedFileName;
    int       l;
    IMAGETYPE Image;
    LPTSTR    lpch;
    // Just spawning an exec, not debugging
    BOOL      fSpawnOrphan =  !(creationFlags & DEBUG_PROCESS || creationFlags & DEBUG_ONLY_THIS_PROCESS);

    static TCHAR szFullName[MAX_PATH];
    static TCHAR szCommandLine[8192];
    static TCHAR szCurrentDir[MAX_PATH]; // Directory to spawn the debuggee in.

    Unreferenced( pattrib );
    Unreferenced( tattrib );
    Unreferenced( creationFlags );

    /* NOTE: Might have to do the same sort of copying for
     * szArg, pattrib, tattrib and
     * startupInfo. Determine if this is necessary.
     */

    //
    // global flag to help with special handling of DOS/WOW apps.
    //

    FLoading16 = FALSE;


    //
    //  Form the command line.
    //

    //
    //  First, we extract the program name and get its full path. Then
    //  we append the arguments.
    //

    if (szAppName[0] == _T('"')) {
        // If the user specified a quoted name (ie: a Long File Name, perhaps?),

    // Examination of the calling code reveals that quotes are stripped 
    // from the exe name.  I'm curious if this ever changes, so I've put
    // a benign assert here.  If you hit it, rest assured it doesn't mean
    // that anything is wrong.  I'd appreciate an email about it to
    // JayBaz.  If it annoys you, remove it.  I don't mind.
    assert (FALSE);

        fQuotedFileName=TRUE;
        szAppName++;    // Advance past the quote.
    } else {
        fQuotedFileName=FALSE;
    }

    //
    // Null terminate the command line
    //

    if (  (_ftcslen(szAppName) > 2 && szAppName[1] == _T(':'))
        || szAppName[0] == _T('\\')) {

        _ftcscpy(szCommandLine, szAppName);
        fUseRealName = TRUE;

    } else if (_ftcschr(szAppName, _T('\\')) || !SearchPathSet) {

        _ftcscpy(szCommandLine, _T(".\\") );
        _ftcscat(szCommandLine, szAppName );
        fUseRealName = TRUE;

    } else {

        if (!*SearchPathString) {
            _ftcscpy(SearchPathString, _T(".;"));
            l = 2;
            l += GetSystemDirectory(SearchPathString+l,
                                    _tsizeof(SearchPathString)-l);
            SearchPathString[l++] = _T(';');
            l += GetWindowsDirectory(SearchPathString+l,
                                     _tsizeof(SearchPathString)-l);
            SearchPathString[l++] = _T(';');
            GetEnvironmentVariable(_T("PATH"),
                                   SearchPathString+l,
                                   _tsizeof(SearchPathString)-l);
        }

        _ftcscpy(szCommandLine, szAppName);
        fUseRealName = FALSE;
    }

    if (fQuotedFileName) {
        szAppName--;
    }

    //
    // to be similar to the shell, we look for:
    //
    // .COM
    // .EXE
    // nothing
    //
    // since '.' is a valid filename character on many filesystems,
    // we don't automatically assume that anything following a '.'
    // is an "extension."  If the extension is "COM" or "EXE", leave
    // it alone; otherwise try the extensions.
    //

    lpch = _ftcschr(szCommandLine, _T('.'));
    if (lpch &&
             ( lpch[1] == 0
            || _ftcsicmp(lpch, _T(".COM")) == 0
            || _ftcsicmp(lpch, _T(".EXE")) == 0)
    ) {
        lpch = NULL;
    } else {
        lpch = szCommandLine + _ftcslen(szCommandLine);
    }

    *szFullName = 0;
    if (lpch) {
        _ftcscpy(lpch, _T(".COM"));
        ResolveFile(szCommandLine, szFullName, fUseRealName);
    }
    if (!*szFullName && lpch) {
        _ftcscpy(lpch, _T(".EXE"));
        ResolveFile(szCommandLine, szFullName, fUseRealName);
    }
    if (!*szFullName) {
        if (lpch) {
            *lpch = 0;
        }
        ResolveFile(szCommandLine, szFullName, fUseRealName);
    }

    if (!*szFullName) {

        return xosdFileNotFound;

    }


    if ((type = pCharMode(szFullName, &Image)) == INVALID) {

        return xosdFileNotFound;

    } else {

        switch ( Image ) {
            case Image_Unknown:
                // treat as a com file
                //return xosdBadFormat;

            case Image_16:
                FLoading16 = TRUE;
#if defined(TARGET_i386)
                if ( (type == System_GUI) &&
                        !(creationFlags & CREATE_SEPARATE_WOW_VDM) &&
                        IsWOWPresent() )
                {
                    // TODO need dbcError here
                    return xosdGeneral;
                }
                break;
#else
                // TODO all platforms will suppport this
                return xosdGeneral;
#endif

            default:
                break;

        }
    }

    creationFlags |= (type?CREATE_NEW_CONSOLE:0);

    {
        //
        //  Add rest of arguments and quotes just in case
        //

        static TCHAR szCommandLineT[8192];

        // assert for the non-believers
        assert (szCommandLine[0] != _T('\"'));

        _ftcscpy (szCommandLineT, _T("\""));
        _ftcscat (szCommandLineT, szCommandLine);
        _ftcscat (szCommandLineT, _T("\""));
        _ftcscat (szCommandLineT, szArg);
        _ftcscpy (szCommandLine, szCommandLineT);

    }

    if (Image == Image_Dump) {
        //
        // must be a crash dump file
        //
        if (!StartCrashPollThread()) {
            return xosdUnknown;
        }
        return xosdNone;
    }

    if (!fSpawnOrphan) {
        if (!StartDmPollThread()) {
            return xosdUnknown;
        }
    }

    // Handle the current directory
    if ( currentDirectory == NULL ) {
            TCHAR szDir[_MAX_DIR];
            // szCurrentDir is temporarily used to get the drive letter in it.
            _tsplitpath(szFullName, szCurrentDir, szDir, NULL, NULL);
            // After the _tcscat szCurrentDir will have the right value.
            _tcscat(szCurrentDir, szDir);
    } else {
            _tcscpy(szCurrentDir, currentDirectory);
    }

    ResetEvent(SpawnStruct.hEventApiDone);

    SpawnStruct.szAppName = szFullName;

    xosd = ProcessDebuggeeRedirection (szCommandLine, pStartupInfo);
    if (xosd != xosdNone) {
        return xosd;
    }

    if (pStartupInfo -> dwFlags & STARTF_USESTDHANDLES && !fInheritHandles) {
        // If we're redirecting the debuggee's STDIO, inheritHandles must be set.
        assert (FALSE);
        fInheritHandles = TRUE;
    }

    if (!_ftcslen (szCommandLine)) {
        //
        // In Win 95, if the Args are an empty string,
        // the AppName is interpreted as with arguments.  This fixes it.
        //

        SpawnStruct.szArgs = NULL;

    } else {
        //
        // parse any IO redirection
        //
        SpawnStruct.szArgs = szCommandLine;
    }

    if (!_ftcslen(szCurrentDir)) {
        SpawnStruct.pszCurrentDirectory = NULL;
    } else {
        SpawnStruct.pszCurrentDirectory = szCurrentDir;
    }

    SpawnStruct.fdwCreate = creationFlags;
    SpawnStruct.si        = *pStartupInfo;
    SpawnStruct.fInheritHandles = fInheritHandles;

    //
    // The second argument to get command line should have both the exe name
    // and the command line args. GetCommandLine & argv[] are based on the second
    // argument and we need the exe name to appear there.
    //

    if (fSpawnOrphan) {

		//
        // If we're not debugging, it's because we're doing an Execute
		// Instead of making the debug thread spawn, we spawn here.
		//

        // First, if it's a console app we run the app with 'vcspawn -p -t' so
		// the user gets a 'Press any key to continue' when it's done.
		//
		
		IMAGETYPE iType;
		
		if (pCharMode (SpawnStruct.szAppName, &iType) == System_Console) {
			LPTSTR lpFilePart;
			static TCHAR szVCSpawnCmdPath[_MAX_PATH + 1];
			// We rely on only our vcspawn.exe in path.
			if (SearchPath(
				NULL,			/* lpPath */
				szVCSpawnCmd,		/* lpFileName */
				NULL,			/* lpExtension */
				_MAX_PATH,		/* nBurrerLength */
				szVCSpawnCmdPath,	/* lpBuffer */
				&lpFilePart		/* lpFilePart */
				) != 0) {
				//
				// We found vcspawn on the path
				//
				
				static TCHAR szArgsTmp[8128];
				
				// Build a new szArgs that looks like this:
				// "vcspawn -t -p d:\Path\MyDebuggee arg1 arg2..."
				
				_ftcscpy (szArgsTmp, szVCSpawnCmd);      // "vcspawn.exe"
				_ftcscat (szArgsTmp, szVCSToolFlag);	 // " -t " 
				_ftcscat (szArgsTmp, szVCSPauseFlag);    // " -p " 
				_ftcscat (szArgsTmp, SpawnStruct.szArgs); // the rest 
				
				// Put it back in the args
				_ftcscpy (SpawnStruct.szArgs, szArgsTmp);
				
				// Make vcspawn the exe to run, as found by SearchPath
				_ftcscpy (SpawnStruct.szAppName, szVCSpawnCmdPath);
			}
		}

        SpawnStruct.fReturn = CreateProcess( SpawnStruct.szAppName,
            SpawnStruct.szArgs,
            NULL,
            NULL,
            SpawnStruct.fInheritHandles,
            SpawnStruct.fdwCreate,
            NULL,
            SpawnStruct.pszCurrentDirectory,
            &SpawnStruct.si,
            lppi);

        Close3Handles(rgh);
        if (!SpawnStruct.fReturn) {
            SpawnStruct.dwError = GetLastError();
        } else {
			 // Close the thread and process handles
            CloseHandle(lppi->hThread);
            CloseHandle(lppi->hProcess);
		}
    } else {
        
        
		//
        //  This is a semaphore!  Set it last!
        //

        SpawnStruct.fSpawn    = TRUE;

        if (WaitForSingleObject( SpawnStruct.hEventApiDone, INFINITE ) != 0) {
            SpawnStruct.fReturn = FALSE;
            SpawnStruct.dwError = GetLastError();
        }
    }


    if (SpawnStruct.fReturn) {

        xosd = xosdNone;

    } else {

        DPRINT(1, (_T("Failed.\n")));

        xosd = xosdGeneral;
        // make a dbcError with SpawnStruct.dwError
        SendNTError(hprc, SpawnStruct.dwError, NULL);

    }

    return xosd;
}

#endif  // !KERNEL
#endif // 0


HPRCX
InitProcess(
    HPID hpid
    )
/*++

Routine Description:


Arguments:


Return Value:

--*/
{
    HPRCX   hprc;

    /*
     * Create a process structure, place it
     * at the head of the master list.
     */

    hprc = (HPRCX)MHAlloc(sizeof(HPRCXSTRUCT));
    memset(hprc, 0, sizeof(*hprc));

    EnterCriticalSection(&csThreadProcList);

    hprc->next          = prcList->next;
    prcList->next       = hprc;
    hprc->hpid          = hpid;
    hprc->exceptionList = NULL;
    hprc->pid           = (PID)-1;      // Indicates prenatal process
    hprc->pstate        = 0;
    hprc->cLdrBPWait    = 0;
    hprc->fExited		= 0;
    hprc->hEventCreateThread = CreateEvent(NULL, TRUE, TRUE, NULL);
    hprc->f16bit        = FALSE;
	//hprc->fSoftBroken   = FALSE;

#ifndef KERNEL
    hprc->pFbrCntx      = NULL;  //Ignore Fibers
    hprc->FbrLst        = NULL;
    hprc->OrpcDebugging = ORPC_NOT_DEBUGGING;
    hprc->dwKernel32Base = 0;
    hprc->llnlg = LLInit(sizeof ( NLG ), llfNull, NULL, NLGComp );
#endif
	hprc->fStepInto		= FALSE;
	hprc->hExitFailed	= NULL;
	hprc->hAsyncStopRequest = NULL;

	VERIFY (CreateTimerEventQueue (&hprc->TimerQueue));
    InitExceptionList(hprc);

    LeaveCriticalSection(&csThreadProcList);

    return hprc;
}


void
ActionDebugNewReady(
    DEBUG_EVENT * pde,
    HTHDX hthd,
    DWORD unused,
    HPRCX hprc
    )
/*++

Routine Description:

    This function is called when a new child process is ready to run.
    The process is in exactly the same state as in ActionAllDllsLoaded.
    However, in this case the debugger is not waiting for a reply.

Arguments:


Return Value:

--*/
{
#if defined(INTERNAL)
    LPBYTE lpbPacket;
    WORD   cbPacket;
    PDLL_DEFER_LIST pddl;
    PDLL_DEFER_LIST pddlT;
    DEBUG_EVENT     de;
#endif

    DPRINT(5, (_T("Child finished loading\n")));

#ifdef TARGET_i386
    hthd->fContextDirty = FALSE;  // Undo the change made in ProcessDebugEvent
#endif

    hprc->pstate &= ~ps_preStart;       // Clear the loading state flag
    hprc->pstate |=  ps_preEntry;       // next stage...
    hthd->tstate |=  ts_stopped;        // Set that we have stopped on event
    --nWaitingForLdrBreakpoint;

#if 0
    SendDllLoads(hthd);
#endif

#if defined(INTERNAL)
    hprc->fNameRequired = TRUE;

    for (pddl = hprc->pDllDeferList; pddl; pddl = pddlT) {

        pddlT = pddl->next;

        de.dwDebugEventCode        = LOAD_DLL_DEBUG_EVENT;
        de.dwProcessId             = pde->dwProcessId;
        de.dwThreadId              = pde->dwThreadId;
        de.u.LoadDll               = pddl->LoadDll;

        if (LoadDll(&de, hthd, &cbPacket, &lpbPacket) && (cbPacket != 0)) {
            LeaveCriticalSection(&csProcessDebugEvent);
            NotifyEM(&de, hthd, cbPacket, lpbPacket);
            EnterCriticalSection(&csProcessDebugEvent);
        }

        MHFree(pddl);
    }
    hprc->pDllDeferList = NULL;
#endif

    /*
     * Prepare to stop on thread entry point
     */

    SetupEntryBP(hthd);

    /*
     * leave it stopped and notify the debugger.
     */
#if defined(TARGET_MIPS) || defined(TARGET_ALPHA) || defined(TARGET_PPC)
    SetBPFlag(hthd, EMBEDDED_BP);
#endif
    pde->dwDebugEventCode = LOAD_COMPLETE_DEBUG_EVENT;

    NotifyEM(pde, hthd, 0, 0L);

    return;
}                                       /* ActionDebugNewReady() */


void
ActionDebugActiveReady(
    DEBUG_EVENT * pde,
    HTHDX hthd,
    DWORD unused,
    HPRCX hprc
    )
/*++

Routine Description:

    This function is called when a newly attached process is ready to run.
    This process is not the same as the previous two.  It is either running
    or at an exception, and a thread has been created by DebugActiveProcess
    for the sole purpose of hitting a breakpoint.

    If we have an event handle, it needs to be signalled before the
    breakpoint is continued.

Arguments:


Return Value:

--*/
{

    DPRINT(5, (_T("Active process finished loading\n")));

#ifdef TARGET_i386
    hthd->fContextDirty = FALSE;  // Undo the change made in ProcessDebugEvent
#endif // i386

    hprc->pstate &= ~ps_preStart;
    hthd->tstate |=  ts_stopped;    // Set that we have stopped on event
    --nWaitingForLdrBreakpoint;

#if 0
    // BUGBUG kentf this is not wise.  As implemented, this will run code
    // in the crashed debuggee, calling LoadLibrary and other foolish things.
    SendDllLoads(hthd);
#endif

    /*
     * If this is a crashed process, tell the OS
     * to raise the exception.
     * Tell the EM that we are finished loading;
     * it will say GO, and we will catch the exception
     * soon after.
     */

    if (pde->dwProcessId == DebugActiveStruct.dwProcessId) {
        if (DebugActiveStruct.hEventGo) {
            SetEvent(DebugActiveStruct.hEventGo);
            CloseHandle(DebugActiveStruct.hEventGo);
        }
        DebugActiveStruct.dwProcessId = 0;
        DebugActiveStruct.hEventGo = 0;
        SetEvent(DebugActiveStruct.hEventReady);
    }

#if defined(TARGET_MIPS) || defined(TARGET_ALPHA) || defined(TARGET_PPC)
    SetBPFlag(hthd, EMBEDDED_BP);
#endif
    pde->dwDebugEventCode = LOAD_COMPLETE_DEBUG_EVENT;

    NotifyEM(pde, hthd, 0, 0L);

    return;
}                                       /* ActionDebugActiveReady() */


void
ActionEntryPoint16(
    DEBUG_EVENT   * pde,
    HTHDX           hthdx,
    DWORD           unused,
    LPVOID          lpv
    )
/*++

Routine Description:

    This is the registered event routine called when vdm
    sends a DBG_TASKSTART notification.

Arguments:


Return Value:

    None

--*/
{
    hthdx->hprc->pstate &= ~ps_preEntry;
    hthdx->tstate |= ts_stopped;
    NotifyEM(pde, hthdx, 0, (LPVOID)ENTRY_BP);
}


void
ActionEntryPoint(
    DEBUG_EVENT   * pde,
    HTHDX           hthd,
    DWORD           unused,
    LPVOID          lpv
    )
/*++

Routine Description:

    This is the registered event routine called when the base
    exe's entry point is executed.  The action we take here
    depends on whether we are debugging a 32 bit or 16 bit exe.

Arguments:

    pde     - Supplies debug event for breakpoint

    hthd    - Supplies descriptor for thread that hit BP

    unused  - unused

    lpv     - unused

Return Value:

    None

--*/
{
    PBREAKPOINT pbp;

    Unreferenced(lpv);

    pbp = AtBP(hthd);
    assert(pbp);
    RemoveBP(pbp);

    // the main reason we are here.
    ExprBPRestoreDebugRegs(hthd);

    if (hthd->hprc->f16bit) {

        //
        // if this is a 16 bit exe, continue and watch for
        // the task start event.
        //

        SetDebugEventThreadState (hthd->hprc, ts_running);

        AddQueue( QT_CONTINUE_DEBUG_EVENT,
                  hthd->hprc->pid,
                  hthd->tid,
                  DBG_CONTINUE,
                  0 );


    } else {
        // if this is a 32 bit exe, stay stopped and notify the EM
        hthd->hprc->pstate &= ~ps_preEntry;
        hthd->tstate |= ts_stopped;
        pde->dwDebugEventCode = ENTRYPOINT_DEBUG_EVENT;
        NotifyEM(pde, hthd, 0, (LPVOID)ENTRY_BP);
        //DMSqlStartup( hthd->hprc );
    }
}


void
HandleDebugActiveDeadlock(
    HPRCX hprc
    )
{
    DEBUG_EVENT de;
    HTHDX   hthd;

    // This timed out waiting for the loader
    // breakpoint.  Clear the prestart state,
    // and tell the EM we are screwed up.
    // The shell should then stop waiting for
    // the loader BP.

    hprc->pstate &= ~ps_preStart;
    --nWaitingForLdrBreakpoint;
    ConsumeAllProcessEvents(hprc, TRUE);

    if (hprc->pid == DebugActiveStruct.dwProcessId) {
        if (DebugActiveStruct.hEventGo) {
            SetEvent(DebugActiveStruct.hEventGo);
            CloseHandle(DebugActiveStruct.hEventGo);
        }
        DebugActiveStruct.dwProcessId = 0;
        DebugActiveStruct.hEventGo = 0;
        SetEvent(DebugActiveStruct.hEventReady);
    }

    de.dwDebugEventCode      = ATTACH_DEADLOCK_DEBUG_EVENT;
    de.dwProcessId           = hprc->pid;
    hthd = hprc->hthdChild;
    if (hthd) {
        de.dwThreadId = hthd->tid;
    } else {
        de.dwThreadId = 0;
    }
    NotifyEM(&de, hthd, 0, 0);

}                                   /* HandleDebugActiveDeadlock() */



BOOL
SetupSingleStep(
    HTHDX hthd,
    BOOL  DoContinue,
	BOOL  fOnTrace
    )
/*++

Routine Description:

    description-of-function.

Arguments:

    hthd        -   Supplies The handle to the thread which is to
                    be single stepped
    DoContinue  -   Supplies continuation flag

	fOnTrace-   for Instr of type INSTR_CANNOT_TRACE, we have already set
					a hardcoded bp, we should not set up single stepping in 
					this case - fOnTrace toggles trace bit 
					

Return Value:

    TRUE if successfly started step and FALSE otherwise

--*/
{

    PBREAKPOINT         pbp;
    ADDR                addr;

#if defined(NO_TRACE_FLAG)

    /*
     *  Set a breakpoint at the next legal offset and mark the breakpoint
     *  as being for a single step.
     */

	if (fOnTrace) {

        AddrInitEx( &addr, 0, GetNextOffset(hthd, FALSE), 
				    hthd->fAddrOff64, FALSE
				   );
        pbp = SetBP( hthd->hprc, hthd, bptpExec, 
				     bpnsStop, &addr, (HPID) INVALID
					);
        if ( pbp != NULL ) {
            pbp->isStep++;
        }
	}

    /*
     * Now issue the command to execute the child
     */

    if ( DoContinue ) {
        ThreadContinueDebugEvent (hthd);
    }


#else   // NO_TRACE_FLAG


#if defined(TARGET_i386)
#ifndef KERNEL

    /*
     *  Set the single step flag in the context and then start the
     *  thread running
     *
     *  Modify the processor flags in the child's context
     */

    //
    // If we are passing an exception on to the child we cannot set the
    // trace flag, so do the non-trace version of this code.
    //


    hthd->fContextDirty = TRUE;

	if (!fOnTrace) {
        HthdReg(hthd,EFlags) &= ~TF_BIT_MASK;
	}
	else {

        if (!IsPassingException (hthd->hprc)) {
            HthdReg(hthd,EFlags) |= TF_BIT_MASK;

        } else {
            int lpf = 0;

            HthdReg(hthd,EFlags) &= ~TF_BIT_MASK;

            //
            //  this sequence initializes addr to the addr after the current inst

            AddrFromHthdx (&addr, hthd);
            IsCall (hthd, &addr, &lpf, FALSE);


            pbp = SetBP( hthd->hprc, hthd, bptpExec, 
					     bpnsStop, &addr, (HPID) INVALID);
           if ( pbp != NULL ) {
               pbp->isStep++;
           }
       }
	}

#endif  // KERNEL

#elif defined(TARGET_IA64)

#else   // i386

#error "Need code for new CPU with trace bit"

#endif  // i386

    /*
     * Now issue the command to execute the child
     */

    if ( DoContinue ) {
        ThreadContinueDebugEvent (hthd);
    }

#endif  // NO_TRACE_FLAG

    return TRUE;
}                                       /*  SetupSingleStep() */
VOID
ActionReturnStep(
    DEBUG_EVENT   * pde,
    HTHDX           hthd,
    DWORD           unused,
    LPVOID          lpv
    )
/*++
        lpv - stack pointer at the top of the call
--*/
{
    BREAKPOINT* pbp = AtBP(hthd);

    assert(pbp);
#if defined(_M_ALPHA)
	if (STACK_POINTER(hthd) >= (DWORD)lpv) {
#else
    if (STACK_POINTER(hthd) > (DWORD)lpv) {
#endif
		RemoveBP (pbp);

#if 0
		//
		// If ORPC debuging is on and we've stopped in an ORPC section,
		// continue on.
		//
		
		if (hthd->hprc->OrpcDebugging == ORPC_DEBUGGING &&
			FAddrInOle (hthd->hprc, PC (hthd)) ||
			InRpcrt4 (hthd->hprc, PC (hthd))) {

            RegisterExpectedEvent (hthd->hprc,
                                   hthd,
                                   OLE_DEBUG_EVENT,
                                   orpcServerGetBufferSize,
                                   DONT_NOTIFY,
                                   ActionOrpcServerGetBufferSize,
                                   FALSE,
                                   (LPVOID) FALSE
                                   );

            ThreadContinueDebugEvent (hthd);

		} else
#endif

		{

			pde->dwDebugEventCode = EXCEPTION_DEBUG_EVENT;
			pde->u.Exception.ExceptionRecord.ExceptionCode = EXCEPTION_SINGLE_STEP;
			ConsumeAllThreadEvents (hthd, FALSE);
			NotifyEM(pde, hthd, 0, NULL);
		}
    } else {
        METHOD *ContinueSSMethod;
        assert(pbp != EMBEDDED_BP);
        assert(pbp != NULL);
        ClearBPFlag(hthd);
        RestoreInstrBP(hthd, pbp);
        ContinueSSMethod = (METHOD*) MHAlloc(sizeof(METHOD));
        ContinueSSMethod->notifyFunction = MethodContinueSS;
        ContinueSSMethod->lparam = ContinueSSMethod;
        ContinueSSMethod->lparam2 = pbp;
        ++pbp->cthd;
        /* Reenable */
        RegisterExpectedEvent(hthd->hprc, hthd,
                              BREAKPOINT_DEBUG_EVENT,
                              (DWORD) pbp,
                              DONT_NOTIFY,
                              ActionReturnStep,
                              FALSE,
                              lpv);
        SingleStepEx(hthd, ContinueSSMethod, FALSE, FALSE, TRUE);
    }
}

BOOL
SetupReturnStep(
    HTHDX hthd,
    BOOL  DoContinue,
    LPADDR lpaddr,
    LPADDR addrStack
    )
/*++

Routine Description:

    description-of-function.

Arguments:

    hthd        -   Supplies The handle to the thread which is to
                    be single stepped

    DoContinue  -   Supplies continuation flag

    lpaddr      -   Supplies the address to step to

    addrStack    -   Supplies the address for SP.
Return Value:

    TRUE if successfully started step and FALSE otherwise

--*/
{
    BREAKPOINT *        pbp;

    /*
     */

    pbp = SetBP( hthd->hprc, hthd, bptpExec, bpnsStop, lpaddr, (HPID)INVALID);

        if ( !pbp ) {

#if 0
#ifndef KERNEL
			if (CrashDump) {
				return FALSE;
			}
#endif
#endif
		
			//
			// On Win95 we might not be able to set the bp because
			// we might be in a callback and returning to system code.
			//
			
			//assert(IsChicago ());

			if (IsInSystemDll(GetAddrOff(*lpaddr))) {
				SendDBCErrorStep(hthd->hprc);
			} else {
					assert(FALSE); // Shouldn't happen any other time.
			}

			return FALSE;
		}


    if (addrStack) {
        RegisterExpectedEvent(hthd->hprc, hthd,
                              BREAKPOINT_DEBUG_EVENT,
                              (DWORD)pbp,
                              DONT_NOTIFY,
                              ActionReturnStep,
                              FALSE,
                              (LPVOID) GetAddrOff(*addrStack));
    }

#if 0

    SetupOrpcRangeStep (hthd);

#endif

    /*
     * Now issue the command to execute the child
     */

    if ( DoContinue ) {
        ExprBPContinue( hthd->hprc, hthd );
        ThreadContinueDebugEvent (hthd);
    }

    return TRUE;
}                                       /*  SetupReturnStep() */



void
SetupEntryBP(
    HTHDX   hthd
    )
/*++

Routine Description:

    Set a breakpoint and make a persistent expected event for the
    entry point of the first thread in a new process.

Arguments:

    hthd    - Supplies descriptor for thread to act on.

Return Value:

    None

--*/
{
    ADDR            addr;
    BREAKPOINT    * bp;
#if defined(TARGET_PPC)
    OFFSET        real_addr_entry_pt;
    DWORD         cb;
#endif

    AddrInit(&addr,
             0,
             0,
             (OFFSET)hthd->lpStartAddress,
             TRUE,
             TRUE,
             FALSE,
             FALSE);

#if defined(TARGET_PPC)

    // for PPC we have a function entry at lpStartAddress because
    // of the darn TOC, so  we need to dereference it
    // All other BP's work out fine 'cause we use the CV info. which
    // gets around the problem.


    AddrReadMemory(hthd->hprc, hthd, &addr,
                      &real_addr_entry_pt,
                      sizeof(real_addr_entry_pt),&cb);

    if (cb != sizeof(real_addr_entry_pt)) {

        DPRINT(1,(_T("Could not read the info located at 0x%lx -- cb = %ld"),addr.addr.off,cb));
        assert(FALSE);
        return;
    }

    DPRINT(1,(_T("The dereferenced address of the entry pt is 0x%lx"),
           real_addr_entry_pt));


    AddrInit(&addr,
             0,
             0,
             real_addr_entry_pt,
             TRUE,
             TRUE,
             FALSE,
             FALSE);

#endif // PPC

    bp = SetBP(hthd->hprc, hthd, bptpExec, bpnsStop, &addr, (HPID)ENTRY_BP);

    // register expected event
    RegisterExpectedEvent(hthd->hprc,
                          hthd,
                          BREAKPOINT_DEBUG_EVENT,
                          (DWORD)bp,
                          DONT_NOTIFY,
                          ActionEntryPoint,
                          TRUE,     // Persistent!
                          NULL
                         );
}                                   /* SetupEntryBP() */


#ifdef KERNEL
VOID
RestoreKernelBreakpoints (
    HTHDX   hthd,
    UOFF32  Offset
    )
/*++

Routine Description:

    Restores all breakpoints in our bp list that fall in the range of
    offset -> offset+dbgkd_maxstream.  This is necessary because the kd
    stub in the target system clears all breakpoints in this range before
    delivering an exception to the debugger.

Arguments:

    hthd    - handle to the current thread

    Offset  - beginning of the range, usually the current pc

Return Value:

    None

--*/
{

    BREAKPOINT              *pbp;
    DBGKD_WRITE_BREAKPOINT  bps[MAX_KD_BPS];
    DWORD                   i = 0;


    EnterCriticalSection(&csThreadProcList);

    ZeroMemory( bps, sizeof(bps) );

    for (pbp=bpList->next; pbp; pbp=pbp->next) {

        if (GetAddrOff(pbp->addr) >= Offset &&
            GetAddrOff(pbp->addr) <  Offset+DBGKD_MAXSTREAM) {
            if (i < MAX_KD_BPS) {
                bps[i++].BreakPointAddress = (LPVOID)GetAddrOff(pbp->addr);
            }
        }
    }

    if (i) {
        WriteBreakPointEx( hthd, i, bps, 0 );

        for (i=0,pbp=bpList->next; pbp; pbp=pbp->next) {

            if (GetAddrOff(pbp->addr) == (DWORD)bps[i].BreakPointAddress) {
                pbp->hBreakPoint = bps[i++].BreakPointHandle;

            }
        }
    }

    LeaveCriticalSection(&csThreadProcList);
}
#endif // KERNEL


#ifndef KERNEL

void
UpdateThreadContext(
    HTHDX   hthd
    )
/*++

Routine Description:

    This routine updates a thread's context, including real/32bit modes and
    context dirty flags.

--*/
{
#if defined(TARGET_i386)
    LDT_ENTRY           ldtEntry;
#endif

#ifndef REGSYNC
    hthd->ctx.ContextFlags = CONTEXT_FULL | CONTEXT_FLOATING_POINT |
		CONTEXT_EXTENDED_REGISTERS;
    DbgGetThreadContext( hthd, &hthd->ctx);
    hthd->fContextDirty = FALSE;
    hthd->fIsCallDone = FALSE;
#if 0
    if (hthd->ctx.EFlags & V86FLAGS_V86) {
        hthd->fAddrIsReal  = TRUE;
        hthd->fAddrIsFlat  = FALSE;
        hthd->fAddrOff32   = FALSE;
    } else
#endif
	{
        hthd->fAddrIsReal  = FALSE;
        hthd->fAddrIsFlat = hthd->fAddrOff32 = TRUE;
    }
#else
	/* Start with an empty context until somebody wants something */
	hthd->ctx.ContextFlags = 0;
    hthd->fIsCallDone = FALSE;
    hthd->fContextDirty = FALSE;
    hthd->fAddrIsReal  = FALSE;
	hthd->fAddrIsFlat = hthd->fAddrOff32 = TRUE;
#endif
}


void
SetDebugEventThreadState(
    HPRCX   hprc,
    TSTATEX state
    )
{
    HTHDX   hthd;

    hthd = (HTHDX) HTHDXFromPIDTID (hprc->pid, hprc->lastTidDebugEvent);

    assert (hthd);
    switch (state)
    {
        case ts_running:
            hthd->tstate &= ~(ts_stopped | ts_first | ts_second);
            hthd->tstate |= ts_running;
            break;

        default:
            assert (FALSE);
    }
}



void
ProcessDebugEvent(
    DEBUG_EVENT *  de
    )
/*++

Routine Description:

    This routine is called whenever a debug event notification comes from
    the operating system.

Arguments:

    de      - Supplies a pointer to the debug event which just occured

Return Value:

    None.

Notes:

    This is the user-mode version of this function.

--*/

{
    EXPECTED_EVENT *    ee;
    DWORD               eventCode = de->dwDebugEventCode;
    DWORD               subClass = 0L;
    HTHDX               hthd = NULL;
    HPRCX               hprc;
    BREAKPOINT *        bp;
    ADDR                addr;
    BP_UNIT             instr;
    DWORD               len;
    BOOL                fInstrIsBp;

    DPRINT(3, (_T("Event Code == %x\n"), eventCode));

    hprc = HPRCFromPID(de->dwProcessId);

    /*
     * While killing process, ignore everything
     * except for exit events.
     */

    if (hprc) {
        hprc->cLdrBPWait = 0;

        //
        // If any of the threads are stopped, we got a debug event while
        // processing another debug event.  This can happen in console apps.
        // Ignore it and continue on.
        //

#if 0
		if (!CrashDump) 
#endif
		{

			for (hthd = hprc->hthdChild; hthd; hthd = hthd->nextSibling) {

				if (hthd->tstate & ts_stopped && !(hthd->tstate & ts_dead)) {
					/* We need to continue this thread if it's hit an
					 * exception or a breakpoint */

					// REVIEW jlange -- why do we ever get here?
					assert(FALSE);
					if(eventCode == EXCEPTION_DEBUG_EVENT) {
						DmContinueThread(de->dwThreadId, FALSE);
						DmGo();
					}
#if 0
					ContinueDebugEvent (de->dwProcessId, de->dwThreadId,
										DBG_CONTINUE);
#endif
					return ;
				}
					
            }
		}
    
        
                
#ifndef KERNEL
        hprc->lastTidDebugEvent = de->dwThreadId;
#endif
    }

    if ( hprc && (hprc->pstate & ps_killed) ) {
        if (eventCode == EXCEPTION_DEBUG_EVENT) {

            AddQueue( QT_CONTINUE_DEBUG_EVENT,
                      de->dwProcessId,
                      de->dwThreadId,
                      (DWORD)DBG_EXCEPTION_NOT_HANDLED,
                      0);
            return;

        } else if (eventCode != EXIT_THREAD_DEBUG_EVENT
          && eventCode != EXIT_PROCESS_DEBUG_EVENT ) {
            AddQueue( QT_CONTINUE_DEBUG_EVENT,
                      de->dwProcessId,
                      de->dwThreadId,
                      (DWORD)DBG_EXCEPTION_NOT_HANDLED,
                      0);
            return;
        }
    }

    EnterCriticalSection(&csProcessDebugEvent);

    if (eventCode == CREATE_THREAD_DEBUG_EVENT){

        DPRINT(3, (_T("*** NEW TID = (PID,TID)(%08lx, %08lx)\n"),
                      de->dwProcessId, de->dwThreadId));

    } else {

        /*
         *  Find our structure for this event's process
         */

        DEBUG_PRINT(_T("Not Create Thread Debug Event\r\n"));
        hthd = HTHDXFromPIDTID((PID)de->dwProcessId,(TID)de->dwThreadId);

        //
        //  Update the context for all threads, not just the main one.  We
        //  may be able to do this a little quicker by using the
        //  fContextDirty flag.
        //

        //
        //  First, update the main threads ExceptionRecord if necessary

        if (hthd && eventCode == EXCEPTION_DEBUG_EVENT) {
            hthd->ExceptionRecord = de->u.Exception.ExceptionRecord;
        }

        //
        //  Loop through all threads and update contexts

        if (hthd && hprc) {
            HTHDX   hthdT;
            //
            // If for any reason this is set unset it.
            //
#ifndef KERNEL
            hprc->pFbrCntx = NULL;
#endif
            /* If this is an exit process event, we can't deal with getting
             * the thread context, so we just handle the event and get out
             * of here */
            if(eventCode == EXIT_PROCESS_DEBUG_EVENT) {
                ProcessExitProcessEvent(de, hthd);
                goto done;
            }

            //
            // v-vadimp - since the fxsave/fxrstor hack screws up exception handling we have to limit its use:
            // 1. try to get extended registers only if the event was an exception (should not need them for dll loading, thread starts, etc)
            // 2. do not get them for a first chance exception that are the user's list - exceptuion handlers will be screwed up (the user will not see extended registers)
            // 3. do get them for second-chance exceptions, when all user hanlers have excuted
            //			
            hthd->fContextExtendedDirty = FALSE;
            if (FXSAVESupported && (eventCode == EXCEPTION_DEBUG_EVENT)) {
                hthd->fContextExtendedDirty = TRUE;
                if (de->u.Exception.dwFirstChance == 1) {
                    EXCEPTION_LIST   *eList;
                    for (eList=hprc->exceptionList; eList; eList=eList->next) {
                        if (eList->excp.dwExceptionCode == (DWORD)de->u.Exception.ExceptionRecord.ExceptionCode ) {
                            break;
                        }
                    }
                    hthd->fContextExtendedDirty = (eList == NULL);
                }
            }

            for (hthdT = hprc->hthdChild; hthdT; hthdT = hthdT->nextSibling) {
                UpdateThreadContext (hthdT);
            }

        } else if (hprc && (hprc->pstate & ps_killed)) {

            /*
             * this is an event for a thread that
             * we never created:
             */
            if (eventCode == EXIT_PROCESS_DEBUG_EVENT) {
                /* Process exited on a thread we didn't pick up */
                ProcessExitProcessEvent(de, NULL);
            } else {
                /* this is an exit thread for a thread we never picked up */
                AddQueue( QT_CONTINUE_DEBUG_EVENT,
                          de->dwProcessId,
                          de->dwThreadId,
                          DBG_CONTINUE,
                          0);
            }
            goto done;

        } else if (eventCode!=CREATE_PROCESS_DEBUG_EVENT) {

            //
            // This will happen sometimes when killing a process with
            // ProcessTerminateProcessCmd and ProcessUnloadCmd.
            //

            AddQueue( QT_CONTINUE_DEBUG_EVENT,
                      de->dwProcessId,
                      de->dwThreadId,
                      DBG_CONTINUE,
                      0);
            goto done;

        }
    }

    //
    //  Mark the thread as having been stopped for some event.
    //

    if (hthd) {
        hthd->tstate &= ~ts_running;
        hthd->tstate |= ts_stopped;
    }

    /* If it is an exception event get the subclass */

    if (eventCode == EXCEPTION_DEBUG_EVENT) {

        subClass = de->u.Exception.ExceptionRecord.ExceptionCode;
        DPRINT(1, (_T("Exception Event: subclass = %x    "), subClass));

        switch (subClass) {
        case (DWORD)STATUS_SEGMENT_NOTIFICATION:
            eventCode = de->dwDebugEventCode = SEGMENT_LOAD_DEBUG_EVENT;
            break;

        case (DWORD)EXCEPTION_SINGLE_STEP:
#if !defined(TARGET_i386)
            assert(_T("!EXCEPTION_SINGLE_STEP on non-x86!"));
#endif
            AddrFromHthdx(&addr, hthd);

            //
            // This may be a single step or a hardware breakpoint.
            // If it is a single step, leave it at that.  If it is
            // a hardware breakpoint, convert it to a BREAKPOINT_DEBUG_EVENT.
            //

            DecodeSingleStepEvent( hthd, de, &eventCode, &subClass );
            break;

        case (DWORD)EXCEPTION_BREAKPOINT:

            /*
             * Check if it is a BREAKPOINT exception:
             * If it is, change the debug event to our pseudo-event,
             * BREAKPOINT_DEBUG_EVENT (this is a pseudo-event because
             * the API does not define such an event, and we are
             * synthesizing not only the class of event but the
             * subclass as well -- the subclass is set to the appropriate
             * breakpoint structure)
             */

            AddrFromHthdx(&addr, hthd);
            //
            // correct for machine overrun on a breakpoint
            //
            // On NT the offset value is overrun by 1 every time except
            // for when doing post-mortem debugging. The value in the
            // Debug Event structure is always correct though
            //

            addr.addr.off = (DWORD) de->u.Exception.ExceptionRecord.ExceptionAddress ;

            DPRINT(3, (_T("Looking for BP@%lx\n"), addr.addr.off));

            EnterCriticalSection(&csThreadProcList);
            /*
             *  Lookup the breakpoint in our (the dm) table
             *  Need to do this before the instruction check
             */
            bp = FindBP(hthd->hprc, hthd, bptpExec, (BPNS)-1, &addr, FALSE);
            //
            //  We need to be able to find BPs for message BPs as well

            if (bp == NULL) {
                bp = FindBP (hthd->hprc,
                             hthd,
                             bptpMessage,
                             (BPNS) -1,
                             &addr,
                             FALSE);
            }

            SetBPFlag(hthd, bp?bp:EMBEDDED_BP);
            LeaveCriticalSection(&csThreadProcList);

            //
            // Determine the start of the breakpoint instruction
            //

            if ((AddrReadMemory(hprc, hthd, &addr, &instr, BP_SIZE, &len) == 0)
                            || (len != BP_SIZE)) {
                DPRINT(1, (_T("Memory read failed!!!\n")));
                assert(FALSE);
                instr = 0;
            }

            /*
             *  It may have been a 0xcd 0x03 rather than a 0xcc
             *  (ie: check if it is a 1 or a 2 byte INT 3)
             */

            fInstrIsBp = FALSE;
            if (instr == BP_OPCODE) {
                fInstrIsBp = TRUE;
            } else if (instr == 0x3) { // 0xcd?
                --addr.addr.off;
                if (AddrReadMemory(hprc,
                                  hthd,
                                  &addr,
                                  &instr,
                                  1,
                                  &len)
                     && (len == 1)
                     && (instr == 0xcd)) {

                    fInstrIsBp = TRUE;
                } else {
                    ++addr.addr.off;
                }
            } else {
			    /* Because Xbox breakpoints don't actually show the int 3 as
                 * being in memory, we need to explicitly check whether this
                 * was a set breakpoint */
                DWORD dwType;
                if(SUCCEEDED(DmIsBreakpoint((PVOID)addr.addr.off, &dwType)) &&
                        dwType == DMBREAK_FIXED)
                    fInstrIsBp = TRUE;
            }

            PC(hthd) = (LONG)addr.addr.off;
            hthd->fContextDirty = TRUE;

            if (!bp && !fInstrIsBp) {
                //
                // If the instruction is not a bp, and there is no record of
                // the bp, this happened because the exception was already
                // in the queue when we cleared the bp.
                //
                // We will just continue it.
                //
                DPRINT(1, (_T("Continuing false BP.\n")));
				SetDebugEventThreadState(hthd->hprc, ts_running);
                AddQueue(QT_CONTINUE_DEBUG_EVENT,
                         hthd->hprc->pid,
                         hthd->tid,
                         DBG_CONTINUE,
                         0 );
                goto done;
            }


            //
            // Q: What does it mean if we find the bp record, but the
            // instruction is not a bp???
            //
            // A: It means that this thread hit the BP after another
            // thread hit it, but before this thread could be suspended
            // by the debug subsystem.  The debugger cleared the BP
            // temporarily in order that the other thread could run
            // past it, and will put it back as soon as that thread
            // hits the temp BP on the next instruction.
            //
            //assert(!bp || fInstrIsBp);

            /*
             *  Reassign the event code to our pseudo-event code
             */
            DPRINT(3, (_T("Reassigning event code!\n")));

            /*
             *  For some machines there is not single instruction tracing
             *  on the chip.  In this case we need to do it in software.
             *
             *  Check to see if the breakpoint we just hit was there for
             *  doing single step emulation.  If so then remap it to
             *  a single step exception.
             */

            if (bp && bp->isStep){
                de->u.Exception.ExceptionRecord.ExceptionCode
                  = subClass = (DWORD)EXCEPTION_SINGLE_STEP;
                RemoveBP(bp);
                break;
            }

            /*
             * Reassign the subclass to point to the correct
             * breakpoint structure
             *
             */

            de->dwDebugEventCode = eventCode = BREAKPOINT_DEBUG_EVENT;
            de->u.Exception.ExceptionRecord.ExceptionAddress =
                (PVOID) addr.addr.off;
            de->u.Exception.ExceptionRecord.ExceptionCode =
              subClass = (DWORD)bp;

            break;


#if 0
            case EXCEPTION_ORPC_DEBUG:
            {
                ORPC orpc;

                // Is this really an OLE notification?
                if ((orpc = OrpcFromPthd(hthd, de)) != orpcNil) {
                    if (orpc == orpcUnrecognized) {
                        // Unrecognized notification.  Resume execution.
                        AddQueue(QT_CONTINUE_DEBUG_EVENT,
                                 hthd->hprc->pid,
                                 hthd->tid,
                                 DBG_CONTINUE,
                                 0 );
                        return;
                    }

                    /*
                    ** Reassign the event code to our pseudo-event code
                    */
                    de->dwDebugEventCode = eventCode = OLE_DEBUG_EVENT;

                    /*
                    ** Reassign the exception code to the type of OLE
                    ** event that occurred.
                    */
                    de->u.Exception.ExceptionRecord.ExceptionCode =
                        subClass = (DWORD) orpc;
                }
                break;
            }
#endif
            case EXCEPTION_FIBER_DEBUG:
            {
                //If fibers are in use change to fiber event
                if(hprc->fUseFbrs) {
                    de->dwDebugEventCode = eventCode = FIBER_DEBUG_EVENT;
                }
                break;
            }
			case EXCEPTION_VISUALCPP_DEBUGGER:
				de->dwDebugEventCode = eventCode = SPECIAL_DEBUGGER_EVENT;
				break;

#ifdef OMNI
			case EXCEPTION_OMNI_DEBUGGER_PRESENT:
				// just catch the exception
                /* this is an exit thread for a thread we never picked up */
                AddQueue( QT_CONTINUE_DEBUG_EVENT,
                          de->dwProcessId,
                          de->dwThreadId,
                          DBG_CONTINUE,
                          0);
				goto done;

            case EXCEPTION_OMNI_DLL_LOAD:
            {
                // Omni Modload - simulate a LoadDll
                de->dwDebugEventCode = eventCode = OMNI_DLL_LOAD_EVENT;
                break;
            }
#endif
        }
    }

    /*
     *  Check if this debug event was expected
     */

    ee = PeeIsEventExpected(hthd, eventCode, subClass, TRUE);


    /*
     * If it wasn't, clear all consummable events
     * and then run the standard handler with
     * notifications going to the execution model
     */

    assert((0 < eventCode) && (eventCode < MAX_EVENT_CODE));

    if (!ee) {

        if ((hthd != NULL) && (hthd->tstate & ts_funceval)) {
            assert(RgfnFuncEventDispatch[eventCode-EXCEPTION_DEBUG_EVENT]);
            RgfnFuncEventDispatch[eventCode-EXCEPTION_DEBUG_EVENT](de, hthd);
        } else {
            assert(DebugDispatchTable[eventCode-EXCEPTION_DEBUG_EVENT]);
            DebugDispatchTable[eventCode-EXCEPTION_DEBUG_EVENT](de,hthd);
        }

    } else {

        /*
         *  If it was expected then call the action
         * function if one was specified
         */

        if (ee->action) {
            (ee->action)(de, hthd, 0, ee->lparam);
        }

        /*
         *  And call the notifier if one was specified
         */

        if (ee->notifier) {
            METHOD  *nm = ee->notifier;
            (nm->notifyFunction)(de, hthd, 0, nm->lparam);
        }

        MHFree(ee);
    }

done:

    LeaveCriticalSection(&csProcessDebugEvent);
    return;
}                               /* ProcessDebugEvent() */



#else // KERNEL




void
ProcessDebugEvent(
    DEBUG_EVENT              *de,
    DBGKD_WAIT_STATE_CHANGE  *sc
    )
/*++

Routine Description:

    This routine is called whenever a debug event notification comes from
    the operating system.

Arguments:

    de      - Supplies a pointer to the debug event which just occured

Return Value:

    None.

Comments:

    This is the kernel debugging version of this fucntion.

--*/

{
    EXPECTED_EVENT *    ee;
    DWORD               eventCode = de->dwDebugEventCode;
    DWORD               subClass = 0L;
    HTHDX               hthd = NULL;
    HPRCX               hprc;
    PBREAKPOINT         bp;
    ADDR                addr;
    DWORD               cb;
    BP_UNIT             instr;
    BOOL                fInstrIsBp = FALSE;


    DPRINT(3, (_T("Event Code == %x\n"), eventCode));

    hprc = HPRCFromPID(de->dwProcessId);

    /*
     * While killing process, ignore everything
     * except for exit events.
     */

    if (hprc) {
        hprc->cLdrBPWait = 0;
    }

    if ( hprc && (hprc->pstate & ps_killed) ) {
        if (eventCode == EXCEPTION_DEBUG_EVENT) {

            AddQueue( QT_CONTINUE_DEBUG_EVENT,
                      de->dwProcessId,
                      de->dwThreadId,
                      (DWORD)DBG_EXCEPTION_NOT_HANDLED,
                      0);
            return;

        } else if (eventCode != EXIT_THREAD_DEBUG_EVENT
          && eventCode != EXIT_PROCESS_DEBUG_EVENT ) {

            AddQueue( QT_CONTINUE_DEBUG_EVENT,
                      de->dwProcessId,
                      de->dwThreadId,
                      (DWORD)DBG_EXCEPTION_NOT_HANDLED,
                      0);
            return;
        }
    }

    EnterCriticalSection(&csProcessDebugEvent);

    if (eventCode == CREATE_THREAD_DEBUG_EVENT){

        DPRINT(3, (_T("*** NEW TID = (PID,TID)(%08lx, %08lx)\n"),
                      de->dwProcessId, de->dwThreadId));

    } else {

        /*
         *  Find our structure for this event's process
         */

        hthd = HTHDXFromPIDTID((PID)de->dwProcessId,(TID)de->dwThreadId);

        /*
         *  Update our context structure for this thread if we found one
         *      in our list.  If we did not find a thread and this is
         *      not a create process debug event then return without
         *      processing the event as we are in big trouble.
         */

        if (hthd) {
            if (eventCode == EXCEPTION_DEBUG_EVENT) {
                hthd->ExceptionRecord = de->u.Exception.ExceptionRecord;
            }
            hthd->context.ContextFlags = CONTEXT_FULL | CONTEXT_FLOATING_POINT;
            DbgGetThreadContext(hthd,&hthd->context);
            hthd->fContextDirty = FALSE;
            hthd->fIsCallDone   = FALSE;
            hthd->fAddrIsReal   = FALSE;
            hthd->fAddrIsFlat   = TRUE;
            hthd->fAddrOff32    = TRUE;

        } else if (hprc && (hprc->pstate & ps_killed)) {

            /*
             * this is an event for a thread that
             * we never created:
             */

            if (eventCode == EXIT_PROCESS_DEBUG_EVENT) {
                /* Process exited on a thread we didn't pick up */
                ProcessExitProcessEvent(de, NULL);
            } else {
                /* this is an exit thread for a thread we never picked up */
                AddQueue( QT_CONTINUE_DEBUG_EVENT,
                          de->dwProcessId,
                          de->dwThreadId,
                          DBG_CONTINUE,
                          0);
            }
            goto done;

        } else if (eventCode != CREATE_PROCESS_DEBUG_EVENT) {

            assert (FALSE);

            AddQueue( QT_CONTINUE_DEBUG_EVENT,
                      de->dwProcessId,
                      de->dwThreadId,
                      DBG_CONTINUE,
                      0);
            goto done;
        }
    }

    /*
     *  Mark the thread as having been stopped for some event.
     */

    if (hthd) {
        hthd->tstate &= ~ts_running;
        hthd->tstate |= ts_stopped;
    }

    /* If it is an exception event get the subclass */

    if (eventCode==EXCEPTION_DEBUG_EVENT){

        subClass = de->u.Exception.ExceptionRecord.ExceptionCode;
        DPRINT(1, (_T("Exception Event: subclass = %x    "), subClass));

        switch (subClass) {
        case (DWORD)STATUS_SEGMENT_NOTIFICATION:
            eventCode = de->dwDebugEventCode = SEGMENT_LOAD_DEBUG_EVENT;
            break;

        case (DWORD)EXCEPTION_SINGLE_STEP:
#if !defined(TARGET_i386)
            assert(_T("!EXCEPTION_SINGLE_STEP on non-x86!"));
#endif
            AddrFromHthdx(&addr, hthd);
            RestoreKernelBreakpoints( hthd, GetAddrOff(addr) );

            //
            // This may be a single step or a hardware breakpoint.
            // If it is a single step, leave it at that.  If it is
            // a hardware breakpoint, convert it to a BREAKPOINT_DEBUG_EVENT.
            //

            DecodeSingleStepEvent( hthd, de, &eventCode, &subClass );
            break;

        case (DWORD)EXCEPTION_BREAKPOINT:

            /*
             * Check if it is a BREAKPOINT exception:
             * If it is, change the debug event to our pseudo-event,
             * BREAKPOINT_DEBUG_EVENT (this is a pseudo-event because
             * the API does not define such an event, and we are
             * synthesizing not only the class of event but the
             * subclass as well -- the subclass is set to the appropriate
             * breakpoint structure)
             */

            hthd->fDontStepOff = FALSE;

            AddrFromHthdx(&addr, hthd);

            /*
             *  Lookup the breakpoint in our (the dm) table
             */

            bp = FindBP(hthd->hprc, hthd, bptpExec, (BPNS)-1, &addr, FALSE);
            SetBPFlag(hthd, bp);

            /*
             *  Reassign the event code to our pseudo-event code
             */
            DPRINT(3, (_T("Reassigning event code!\n")));

            /*
             *  For some machines there is not single instruction tracing
             *  on the chip.  In this case we need to do it in software.
             *
             *  Check to see if the breakpoint we just hit was there for
             *  doing single step emulation.  If so then remap it to
             *  a single step exception.
             */

            if (bp) {
                if (bp->isStep){
                    de->u.Exception.ExceptionRecord.ExceptionCode
                      = subClass = (DWORD)EXCEPTION_SINGLE_STEP;
                    RemoveBP(bp);
                    RestoreKernelBreakpoints( hthd, GetAddrOff(addr) );
                    break;
                } else {
                    RestoreKernelBreakpoints( hthd, GetAddrOff(addr) );
                }
            }

            //
            // Determine the start of the breakpoint instruction
            //

            if (fCrashDump) {
                cb = DmpReadMemory((LPVOID)GetAddrOff(addr),&instr,BP_SIZE);
                if (cb != BP_SIZE) {
                    DPRINT(1, (_T("Memory read failed!!!\n")));
                    instr = 0;
                }
            } else {
                if (DmKdReadVirtualMemoryNow((LPVOID)GetAddrOff(addr),&instr,BP_SIZE,&cb) || cb != BP_SIZE) {
                    DPRINT(1, (_T("Memory read failed!!!\n")));
                    instr = 0;
                }
            }

#if defined(TARGET_ALPHA)

            switch (instr) {
                case 0:
                case CALLPAL_OP | CALLKD_FUNC:
                case CALLPAL_OP |    BPT_FUNC:
                case CALLPAL_OP |   KBPT_FUNC:
                     fInstrIsBp = TRUE;
                     break;
                default:
                    addr.addr.off -= BP_SIZE;
                    if (fCrashDump) {
                        cb = DmpReadMemory((LPVOID)GetAddrOff(addr),&instr,BP_SIZE);
                        if (cb != BP_SIZE) {
                            DPRINT(1, (_T("Memory read failed!!!\n")));
                            instr = 0;
                        }
                    } else {
                        if (DmKdReadVirtualMemoryNow((LPVOID)GetAddrOff(addr),&instr,BP_SIZE,&cb) || cb != BP_SIZE) {
                            DPRINT(1, (_T("Memory read failed!!!\n")));
                            instr = 0;
                        }
                    }
                    switch (instr) {
                        case 0:
                        case CALLPAL_OP | CALLKD_FUNC:
                        case CALLPAL_OP |    BPT_FUNC:
                        case CALLPAL_OP |   KBPT_FUNC:
                             fInstrIsBp = TRUE;
                             hthd->fDontStepOff = TRUE;
                             break;
                        default:
                             fInstrIsBp = FALSE;
                    }
            }

#elif defined(TARGET_PPC)
                if ((instr == BP_OPCODE) || (instr == 0))
                   fInstrIsBp = TRUE;

                if ((!fInstrIsBp) && (LPVOID) GetAddrOff(addr)) {
                    addr.addr.off -= BP_SIZE;
                    if (fCrashDump) {
                        cb = DmpReadMemory((LPVOID)GetAddrOff(addr),&instr,BP_SIZE);
                        if (cb != BP_SIZE) {
                            DPRINT(1, (_T("Memory read failed!!!\n")));
                            instr = 0;
                        }
                    } else {
                        if (DmKdReadVirtualMemoryNow((LPVOID)GetAddrOff(addr),&instr,BP_SIZE,&cb) || cb != BP_SIZE) {
                            DPRINT(1, (_T("Memory read failed!!!\n")));
                            instr = 0;
                        }
                    }

                    if (instr == PPC_KERNEL_BREAKIN_OPCODE)
                    {
                        fInstrIsBp = TRUE;
                        hthd->fDontStepOff = TRUE;

                    }
}

#elif defined(TARGET_i386)

            /*
             *  It may have been a 0xcd 0x03 rather than a 0xcc
             *  (ie: check if it is a 1 or a 2 byte INT 3)
             */

            fInstrIsBp = FALSE;
            if (instr == BP_OPCODE || instr == 0) {
                fInstrIsBp = TRUE;
            } else
            if (instr == 0x3) { // 0xcd?
                --addr.addr.off;
                if (fCrashDump) {
                    cb = DmpReadMemory((LPVOID)GetAddrOff(addr),&instr,BP_SIZE);
                    if (cb != BP_SIZE) {
                        DPRINT(1, (_T("Memory read failed!!!\n")));
                        instr = 0;
                    }
                } else {
                    if (DmKdReadVirtualMemoryNow((LPVOID)GetAddrOff(addr),&instr,BP_SIZE,&cb) || cb != BP_SIZE) {
                        DPRINT(1, (_T("Memory read failed!!!\n")));
                        instr = 0;
                    }
                }
                if (cb == 1 && instr == 0xcd) {
                    --addr.addr.off;
                    fInstrIsBp = TRUE;
                }
            } else {
                hthd->fDontStepOff = TRUE;
            }

#elif defined(TARGET_MIPS)

            {
                PINSTR bi = (PINSTR)&instr;
                if ((bi->break_instr.Opcode == SPEC_OP &&
                     bi->break_instr.Function == BREAK_OP) || (instr == 0)) {

                    fInstrIsBp = TRUE;

                }

                if (!fInstrIsBp) {
                    addr.addr.off -= BP_SIZE;
                    if (fCrashDump) {
                        cb = DmpReadMemory((LPVOID)GetAddrOff(addr),&instr,BP_SIZE);
                        if (cb != BP_SIZE) {
                            DPRINT(1, (_T("Memory read failed!!!\n")));
                            instr = 0;
                        }
                    } else {
                        if (DmKdReadVirtualMemoryNow((LPVOID)GetAddrOff(addr),&instr,BP_SIZE,&cb) || cb != BP_SIZE) {
                            DPRINT(1, (_T("Memory read failed!!!\n")));
                            instr = 0;
                        }
                    }
                    if (bi->break_instr.Opcode == SPEC_OP &&
                        bi->break_instr.Function == BREAK_OP &&
                        bi->break_instr.Code == BREAKIN_BREAKPOINT) {

                        fInstrIsBp = TRUE;
                        hthd->fDontStepOff = TRUE;

                    }
                }
            }

#else

#pragma error( "undefined processor type" );

#endif

            if (!bp && !fInstrIsBp) {
                DMPrintShellMsg( _T("Stopped at an unexpected exception: code=%08x addr=%08x\n"),
                                 de->u.Exception.ExceptionRecord.ExceptionCode,
                                 de->u.Exception.ExceptionRecord.ExceptionAddress
                               );
            }

            /*
             * Reassign the subclass to point to the correct
             * breakpoint structure
             *
             */

            de->dwDebugEventCode = eventCode = BREAKPOINT_DEBUG_EVENT;
            de->u.Exception.ExceptionRecord.ExceptionAddress =
                (PVOID) addr.addr.off;
            de->u.Exception.ExceptionRecord.ExceptionCode =
              subClass = (DWORD)bp;

            break;
        }
    }

    /*
     *  Check if this debug event was expected
     */

    ee = PeeIsEventExpected(hthd, eventCode, subClass, TRUE);


    /*
     * If it wasn't, clear all consummable events
     * and then run the standard handler with
     * notifications going to the execution model
     */

    assert((0 < eventCode) && (eventCode < MAX_EVENT_CODE));

    if (!ee) {

        if ((hthd != NULL) && (hthd->tstate & ts_funceval)) {
            assert(RgfnFuncEventDispatch[eventCode-EXCEPTION_DEBUG_EVENT]);
            RgfnFuncEventDispatch[eventCode-EXCEPTION_DEBUG_EVENT](de, hthd);
        } else {
            assert(DebugDispatchTable[eventCode-EXCEPTION_DEBUG_EVENT]);
            DebugDispatchTable[eventCode-EXCEPTION_DEBUG_EVENT](de,hthd);
        }

    } else {

        /*
         *  If it was expected then call the action
         * function if one was specified
         */

        if (ee->action) {
            (ee->action)(de, hthd, 0, ee->lparam);
        }

        /*
         *  And call the notifier if one was specified
         */

        if (ee->notifier) {
            METHOD  *nm = ee->notifier;
            (nm->notifyFunction)(de, hthd, 0, nm->lparam);
        }

        MHFree(ee);
    }

done:
    LeaveCriticalSection(&csProcessDebugEvent);
    return;
}                               /* ProcessDebugEvent() */



#endif // KERNEL

#if 0

#ifndef KERNEL
////////////////////////////////////////////////////////////////////

//  Helper functions for LoadDll.

////////////////////////////////////////////////////////////////////

//
// iTemp1 is a magic module ID number for when we have to make
// up a name for a module.
//

static iTemp1 = 0;

BOOL
GetModnameFromImage(
    PIMAGE_NT_HEADERS       pNtHdr,
    PIMAGE_SECTION_HEADER   pSH,
    LOAD_DLL_DEBUG_INFO   * pldd,
    LPTSTR                  lpName,
    int                     cbName
    )
/*++

Routine Description:

    This routine attempts to get the name of the exe as placed
    in the debug section section by the linker.

Arguments:

    pNtHdr  - Supplies pointer to NT headers in image PE header

    pSH     - Supplies pointer to section headers

    pldd    - Supplies the info structure from the debug event

    lpName  - Returns the exe name

    cbName  - Supplies the size of the buffer at lpName

Return Value:

    TRUE if a name was found, FALSE if not.
    The exe name is returned as an ANSI string in lpName.

--*/
{
    /*
     * See if the exe name is in the image
     */
    PIMAGE_OPTIONAL_HEADER      pOptHdr = &pNtHdr->OptionalHeader;
    PIMAGE_DEBUG_DIRECTORY      pDebugDir;
    IMAGE_DEBUG_DIRECTORY       DebugDir;
    PIMAGE_DEBUG_MISC           pMisc;
    PIMAGE_DEBUG_MISC           pT;
    DWORD                       rva;
    int                         nDebugDirs;
    int                         i;
    int                         l;
    BOOL                        rVal = FALSE;

    nDebugDirs = pOptHdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size /
                 sizeof(IMAGE_DEBUG_DIRECTORY);

    if (!nDebugDirs) {
        return FALSE;
    }

    rva = pOptHdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;

    for (i = 0; i < pNtHdr->FileHeader.NumberOfSections; i++) {
        if (rva >= pSH[i].VirtualAddress
          && rva < pSH[i].VirtualAddress + pSH[i].SizeOfRawData)
        {
            break;
        }
    }

    if (i >= pNtHdr->FileHeader.NumberOfSections) {
        return FALSE;
    }

    //
    // this is a pointer in the debuggee image:
    //
    if (pldd->hFile == 0) {
        pDebugDir = (PIMAGE_DEBUG_DIRECTORY)
                    ((rva - pSH[i].VirtualAddress) + pSH[i].VirtualAddress);
    } else {
        pDebugDir = (PIMAGE_DEBUG_DIRECTORY)
                    (rva - pSH[i].VirtualAddress + pSH[i].PointerToRawData);
    }

    for (i = 0; i < nDebugDirs; i++) {

        SetReadPointer((ULONG)(&pDebugDir[i]), FILE_BEGIN);
        DoRead((LPV)&DebugDir, sizeof(DebugDir));

        if (DebugDir.Type == IMAGE_DEBUG_TYPE_MISC) {

            l = DebugDir.SizeOfData;
            pMisc = pT = MHAlloc(l);

            if (pldd->hFile == 0) {
                SetReadPointer((ULONG)DebugDir.AddressOfRawData, FILE_BEGIN);
            } else {
                SetReadPointer((ULONG)DebugDir.PointerToRawData, FILE_BEGIN);
            }

            DoRead((LPV)pMisc, l);

            while (l > 0) {
                if (pMisc->DataType != IMAGE_DEBUG_MISC_EXENAME) {
                    l -= pMisc->Length;
                    if (l > (int)DebugDir.SizeOfData) {
                        l = 0; // Avoid AV on bad exe
                        break;
                    }
                    pMisc = (PIMAGE_DEBUG_MISC)
                                (((LPSTR)pMisc) + pMisc->Length);
                } else {

                    PVOID pExeName;

                    pExeName = (PVOID)&pMisc->Data[ 0 ];

#if !defined(_UNICODE)
                    if (!pMisc->Unicode) {
                        _tcscpy(lpName, (LPSTR)pExeName);
                        rVal = TRUE;
                    } else {
                        WideCharToMultiByte(CP_ACP,
                                            0,
                                            (LPWSTR)pExeName,
                                            -1,
                                            lpName,
                                            cbName,
                                            NULL,
                                            NULL);
                        rVal = TRUE;
                    }
#else
                    if (pMisc->Unicode) {
                        wcscpy(lpName, (LPTSTR)pExeName);
                        rVal = TRUE;
                    } else {
                        MultiByteToWideChar(CP_ACP,
                                            0,
                                            (LPTSTR)pExeName,
                                            -1,
                                            lpName,
                                            cbName);
                        rVal = TRUE;
                    }
#endif
                    if (_ftcsicmp(&lpName[_ftcslen(lpName)-4], _T(".DBG")) == 0) {
                        TCHAR    rgchPath[_MAX_PATH];
                        TCHAR    rgchBase[_MAX_FNAME];

                        _tsplitpath(lpName, NULL, rgchPath, rgchBase, NULL);
                        if (_ftcslen(rgchPath)==4) {
                            rgchPath[_ftcslen(rgchPath)-1] = 0;
                            _ftcscpy(lpName, rgchBase);
                            _ftcscat(lpName, _T("."));
                            _ftcscat(lpName, rgchPath);
                        } else {
                            _ftcscpy(lpName, rgchBase);
                            _ftcscat(lpName, _T(".exe"));
                        }
                    }
                    break;
                }
            }

            MHFree(pT);

            break;

        }
    }

    return rVal;
}


BOOL
GetModnameFromExportTable(
    PIMAGE_NT_HEADERS       pNtHdr,
    PIMAGE_SECTION_HEADER   pSH,
    LOAD_DLL_DEBUG_INFO   * pldd,
    LPTSTR                  lpName,
    int                     cbName
    )
/*++

Routine Descriotion:

    This routine attempts to invent an exe name for a DLL
    from the module name found in the export table.  This
    will fail if there is no export table, so it is not
    usually useful for EXEs.

Arguments:

    pNtHdr  - Supplies pointer to NT header in image PE header

    pSH     - Supplies pointer to section header table

    pldd    - Supplies ptr to info record from debug event

    lpName  - Returns name when successful

    cbName  - Supplies size of buffer at lpName

Return Value:

    TRUE if successful and name is copied to lpName, FALSE
    if not successful.

--*/
{
    IMAGE_EXPORT_DIRECTORY      expDir;
    ULONG                       ExportVA;
    ULONG                       oExports;
    int                         iobj;
    int                         cobj;

    /*
     * Find object which has the same RVA as the
     * export table.
     */

    cobj = pNtHdr->FileHeader.NumberOfSections;

    ExportVA = pNtHdr->
                OptionalHeader.
                 DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].
                  VirtualAddress;

    if (!ExportVA) {
        return FALSE;
    }

    for (iobj=0; iobj<cobj; iobj++) {
        if (pSH[iobj].VirtualAddress == ExportVA) {
            oExports = pSH[iobj].PointerToRawData;
            break;
        }
    }

    if (iobj >= cobj) {
        return FALSE;
    }

    if (  (SetReadPointer(oExports, FILE_BEGIN) == -1L)
       || !DoRead(&expDir, sizeof(expDir)) ) {

        return FALSE;
    }

    SetReadPointer(oExports + (ULONG) expDir.Name - ExportVA,
                   FILE_BEGIN);

    _ftcscpy(lpName, _T("#:\\"));

    if (!DoRead(lpName+3, cbName - 3)) {
        // It's a DLL, but we can't get the name...
        _stprintf(lpName+3, _T("DLL%02d.DLL"), ++iTemp1);
    }

    return TRUE;
}

BOOL
GetModNameUsingPsApi(
    HTHDX hthd,
    LOAD_DLL_DEBUG_INFO *pldd,
    LPTSTR              lpName,
    int                 cbName
    )
/*++

Routine Descriotion:

    This routine attempts to get the fullpathname for a DLL
    by calling an entry point in psapi.dll.  This
    will fail on Win95

Arguments:

    hthd    - Ptr to the current thread structure.
    pldd    - Supplies ptr to info record from debug event

    lpName  - Returns name when successful

    cbName  - Supplies size of buffer at lpName

Return Value:

    TRUE if successful and name is copied to lpName, FALSE
    if not successful.

--*/
{
    BOOL fRet = FALSE;

#if 0
    if ( IsChicago( ) ) {
        return FALSE;
    }
#endif

	_asm int 3

    if (hModPsapi == NULL && !fAttemptedLoadingPsapi) {
        fAttemptedLoadingPsapi = TRUE;
        if ((hModPsapi = LoadLibrary("psapi.dll")) != NULL) {
	        pFnGetModuleFileNameExA = (LPFNGETNAME)GetProcAddress(hModPsapi, "GetModuleFileNameExA");
        }		
    }

    if (pFnGetModuleFileNameExA != NULL) {

        if ( (*pFnGetModuleFileNameExA) (  hthd->hprc->rwHand,
                            (HMODULE)pldd->lpBaseOfDll, /* Same as hModule */
                            lpName,
                            cbName )
            )
        {
            fRet = TRUE;
        }
    }

    return fRet;
}



#ifdef INTERNAL
void
DeferIt(
    HTHDX       hthd,
    DEBUG_EVENT *pde
    )
{
    PDLL_DEFER_LIST pddl;
    PDLL_DEFER_LIST *ppddl;

    pddl = MHAlloc(sizeof(DLL_DEFER_LIST));
    pddl->next = NULL;
    pddl->LoadDll = pde->u.LoadDll;
    for (ppddl = &hthd->hprc->pDllDeferList; *ppddl; ) {
         ppddl = & ((*ppddl)->next);
    }
    *ppddl = pddl;
}
#endif  // INTERNAL

/*** FFilesIdentical
 *
 * PURPOSE:
 *      Determine whether a filename and a file handle refer to the same file.
 *
 * INPUT:
 *      szFilename: a filename
 *      hFile: a file handle
 *
 * OUTPUT:
 *      Returns TRUE if these refer to the same file.
 */

BOOL
FFilesIdentical(
    LPTSTR szFileName,
    HANDLE hFile
    )
{
    HANDLE  hFile2;
    BY_HANDLE_FILE_INFORMATION bhfi1, bhfi2;
    BOOL fIdentical = FALSE;

    hFile2 = CreateFile(szFileName, GENERIC_READ,
        FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile2 != INVALID_HANDLE_VALUE)
    {
        if (GetFileInformationByHandle(hFile , &bhfi1) &&
            GetFileInformationByHandle(hFile2, &bhfi2))
        {
            if (bhfi1.dwVolumeSerialNumber == bhfi2.dwVolumeSerialNumber &&
                bhfi1.nFileIndexHigh       == bhfi2.nFileIndexHigh &&
                bhfi1.nFileIndexLow        == bhfi2.nFileIndexLow)
            {
                fIdentical = TRUE;
            }
        }

        VERIFY(CloseHandle(hFile2));
    }

    return fIdentical;
}

/*** FINDMODULE
 *
 * PURPOSE:
 *      Find a module (DLL or EXE) on the path.
 *
 * INPUT:
 *      szModName   Buffer with the name of a module, e.g. "FOO.EXE",
 *                  "BAR.DLL", etc.  May have mixed case.
 *      cchModName  Length of buffer szModName.
 *
 * OUTPUT:
 *      szModName   Replaced with a more specific (but not necessarily
 *                  canonical) path to the module, e.g.
 *                  "C:\NT\SYSTEM\FOO.EXE", "MYDIR\BAR.DLL", if the
 *                  module can be found.  If it can't be found or
 *                  there isn't enough space in the buffer, the buffer
 *                  is left unchanged.
 *
 ********************************************************************/

BOOL
FindModule(
    LPTSTR szModName,
    UINT cchModName
    )
{
    LPTSTR pSlash;
    LPTSTR szFullPath = (LPTSTR)_alloca( cchModName*sizeof(TCHAR) );

    /*
    ** We call SearchPath which is a convenient way to
    ** find a file along the path that Windows uses to load a DLL.
    ** REVIEW: BUG: In order to find the DLL correctly, the call to
    ** SearchPath must be made with the Current Directory set to the
    ** Current Directory of the process that has just loaded the DLL.
    */
    DWORD result = SearchPath( NULL, szModName, NULL, cchModName, szFullPath, &pSlash );
    if ( (result!=0) && (result!=cchModName) )
    {
        _tcscpy( szModName, szFullPath );
        return TRUE;
    }

    return(FALSE);
}

/*** GetNTDebugModuleFileNameLastChance
 *
 * PURPOSE:
 *      Do the best job we can of getting a filename for an EXE or DLL
 *      which has just been loaded.  There is no good way to do this under
 *      NT, so we kludge around a lot.  For EXEs, we saved the name of
 *      the EXE we started before starting it.  For DLLs, we look in the
 *      export table of the DLL for its name, which may or may not be
 *      correct.
 *
 * INPUT:
 *      hProcess    Handle to the process in question (either the
 *                  process which has just started, or the process
 *                  for which a DLL has just been loaded)
 *      hFile       Handle to disk file for the EXE or DLL
 *      lpBaseOfImage   Address in hProcess's address space of the
 *                  beginning of the newly loaded EXE or DLL
 *      cchModName  Size of szModName
 *
 * OUTPUT:
 *      szModName   Buffer where module name (possibly with path)
 *                  is written
 *
 ********************************************************************/

BOOL
FGetNTFileName (
    HTHDX           hthd,
    PDLLLOAD_ITEM   pdll,
    HANDLE          hFile,
    LPTSTR          szModName,
    UINT            cchModName
    )
{
    BOOL fRet = FindModule(szModName, cchModName);

    /* If this isn't the right filename, keep looking ... */
    if (FFilesIdentical(szModName, hFile)) {
        fRet = TRUE;
    } else {
        /* Try to determine the filename from the handle, by whatever
        ** means available
        */
#if 1
        fRet = FALSE;
#else
        if (!FTrojanGetDebugModuleFileName(hthd, pdll, szModName, cchModName))
        {
            /* We shouldn't get here!  We've got to do everything
            ** we possibly can to get the filename, because if we don't
            ** get the filename, we'll have major problems later on.
            ** If we ever hit the assertion, we've just got to come up
            ** with some other ways to try to determine the filename.
            */
            assert(FALSE);
        } else {
            fRet = TRUE;
        }
#endif
    }
    return(fRet);
}


/*** FIXCASE
 *
 * PURPOSE:
 *      Fix the upper/lower case of a filename so that it matches what
 *      is on disk.  If the disk in question supports mixed-case
 *      filenames, we change the name we have to match the case of the
 *      name on disk; otherwise, we set the name to lower case (because
 *      that's prettier than upper case).
 *
 * INPUT:
 *      szFilename  Name of file.
 *
 * OUTPUT:
 *      szFilename  Upper/lower case changed.
 *
 ********************************************************************/

VOID
FixCase(
    LPTSTR szFilename
    )
{

#ifdef WIN32

    TCHAR           rgchDrive[4];   /* "X:\" */
    LPTSTR          szDrive;
    DWORD           dwFlags;
    WIN32_FIND_DATA wfd;
    LPTSTR          pch;
    LPTSTR          pchStart;
    TCHAR           ch;
    HANDLE          hSearch;

    if (szFilename[0] && szFilename[1] == _T(':')) {
        _stprintf(rgchDrive, _T("%c:\\"), szFilename[0]);
        szDrive = rgchDrive;
    }
    else {
        szDrive = NULL;
    }

    if (GetVolumeInformation(szDrive, NULL, 0, NULL, NULL, &dwFlags, NULL, 0)
        &&
        (dwFlags & FS_CASE_IS_PRESERVED)
        ) {

        /*
        ** For each filename component, check what case it has on disk.
        */
        pch = szFilename;

        if (pch[0] && pch[1] == _T(':')) {  /* path has drive letter?   */
            *pch = (TCHAR)_totupper ( *pch );       /* upper case drive letter  */
            pch += 2;
        }

        if (*pch == _T('/') || *pch == _T('\\')) {
            *pch = _T('\\');
            ++pch;
        }

        while (*pch) {

            pchStart = pch;

            while (*pch && *pch != _T('\\') && *pch != _T('/')) {
                pch = _tcsinc( pch );
            }

            ch = *pch;
            *pch = _T('\0');

            /*
            ** Find this filename component
            */
            hSearch = FindFirstFile(szFilename, &wfd);

            /*
            ** If the search failed, or if it returned a name of a
            ** different length than the one we asked for (e.g. we
            ** asked for "FOO." and it gave us "FOO"), we'll give
            ** up and convert the rest of the name to lower case
            */
            if (hSearch == INVALID_HANDLE_VALUE ||
                _ftcslen(pchStart) != _ftcslen(wfd.cFileName)) {
                *pch = ch;
                _ftcslwr ( pchStart );
                return;
            }

            /*
            ** Copy the correct case into our filename
            */
            _tcscpy ( pchStart, wfd.cFileName );

            /*
            ** Close the search
            */
            assert ( !FindNextFile(hSearch, &wfd) );
            VERIFY(FindClose ( hSearch ));

            /*
            ** Restore the slash or NULL
            */
            *pch = ch;

            /*
            ** If we're on a separator, move to next filename component
            */
            if (*pch) {
                *pch = _T('\\');
                pch++;
            }
        }

        return;
    }

#endif

    /*
    ** Convert to lower case, with backslashes
    */

    _ftcslwr(szFilename);

    for (; *szFilename; szFilename = _tcsinc( szFilename) ) {
        if (*szFilename == _T('/')) {
            *szFilename = _T('\\');
        }
    }
}

void
FixFilename(
    TCHAR *szTempFilename,
    const TCHAR *szInFilename
    )
/*++

/*** FIXFILENAME
 *
 * PURPOSE:
 *      Fix the upper/lower case of a filename and so that it matches what
 *      is on disk.  Also if we have a 8.3 name for a long filename
 *      convert the whole path to its long filename equivalen.
 *      If the disk in question supports mixed-case
 *      filenames, we change the name we have to match the case of the
 *      name on disk; otherwise, we set the name to lower case (because
 *      that's prettier than upper case).
 *
 * INPUT:
 *      szInFilename  Name of file. .
 *
 * OUTPUT:
 *      szTempFilename  Upper/lower case changed, long filename variant.
 *                      Assumes size is big enough to hold long
 *                  filename
 *
 * Note: Win95 has an API to do this (called via an int 31h or somesuch),
 * but NT has no such API. Why? To make everyone go through this pain I guess.
 *
 ********************************************************************/

{
    CHAR            rgchDrive[4];   /* "X:\" */
    CHAR *          szDrive;
    DWORD           dwFlags;
    WIN32_FIND_DATA wfd;
    CHAR *          pch;
    CHAR *          pchTemp;
    CHAR *          pchStart;
    CHAR            ch;
    HANDLE          hSearch;
    CHAR            szFilename[512];

    _tcscpy( szFilename, szInFilename );                // make local copy

    if (szFilename[0] && szFilename[1] == ':') {
        sprintf(rgchDrive, "%c:\\", szFilename[0]);
        szDrive = rgchDrive;
    }
    else {
        szDrive = NULL;
    }

    if (GetVolumeInformation(szDrive, NULL, 0, NULL, NULL, &dwFlags, NULL, 0)
        &&
        (dwFlags & FS_CASE_IS_PRESERVED)
        ) {

        /*
        ** For each filename component, check what case it has on disk.
        */
        pch = szFilename;
        pchTemp = szTempFilename;

        if (pch[0] && pch[1] == ':') {  /* path has drive letter?   */
            _tcsncpy(pchTemp, pch, 2);

            /* upper case drive letter  */
            *pchTemp = (char) CharUpper ( (LPTSTR)(unsigned long)(unsigned char)(*pchTemp) );       /* upper case drive letter  */
            pch += 2;
            pchTemp += 2;
        }

        if (*pch == '/' || *pch == '\\') {
            *pch = *pchTemp = '\\';
            ++pchTemp;
            ++pch;
        }

        while (*pch) {
            size_t iLen;

            pchStart = pch;

            while (*pch && *pch != '\\' && *pch != '/')
                pch = _tcsinc( pch );

            ch = *pch;
            *pch = '\0';

            /*
            ** Find this filename component
            */
            hSearch = FindFirstFile(szFilename, &wfd);

            /*
            ** If the search failed, we'll give
            ** up and convert the rest of the name to lower case
            */
            if (hSearch == INVALID_HANDLE_VALUE )
            {
                *pch = ch;
                CharLower ( pchStart );
                // Copy over the rest of the filename to the temporary buffer.
                // this will now have the best we can do about converting
                // this filename.
                _tcscpy(pchTemp, pchStart);

                _tcscpy(szFilename, szTempFilename);

                return;
            }

            /*
            ** Copy the correct name into the temp filename,
            */
            iLen = _tcslen(wfd.cFileName);
            _tcsncpy ( pchTemp, wfd.cFileName, iLen );
            pchTemp += iLen;

            /*
            ** Close the search
            */
            assert ( !FindNextFile(hSearch, &wfd) );
            FindClose ( hSearch );

            /*
            ** Restore the slash or NULL
            */
            *pch = ch;

            /*
            ** If we're on a separator, move to next filename component
            */
            if (*pch) {
                *pchTemp = *pch = '\\';
                pch++; pchTemp++;
            }
        }

        *pchTemp = '\0';
    }
    else
        _tcscpy(szTempFilename, szFilename );           // just copy it if not case sensitive
}


void
FixupDebugImage(
	IN OUT TCHAR*	ImageBuffer
	)
/*++

Routine Description:

	This routine fixes up an image name of the form xxx\image.dbg to be
	image.xxx or -- the more common case -- dll\image.dbg to be image.dll.

	The prefix (xxx above) must be three characters.  If it is not, we will
	not match it.  The path also must be relative or we will not match.

Comments:

	This seems pretty hokey, but we really go to the ends of the earth to
	try and find the real image name.

--*/
{
	TCHAR*	pch = NULL;
	TCHAR*	pchT = NULL;
	TCHAR	bufferT [10];
	int		len;
	
	pch = _tcsrchr (ImageBuffer, '.');

	if (pch && _tcsicmp (pch, _T (".dbg")) == 00) {

		//
		// check if it is of the form "xxx\foo.dbg"
		//

		pchT = ImageBuffer;

		pchT = _tcsinc (pchT);
		pchT = _tcsinc (pchT);
		pchT = _tcsinc (pchT);
		
		if (*pchT == '\\') {
			len = pchT - ImageBuffer;
			memcpy (bufferT, ImageBuffer, len);
			bufferT [len] = '\000';

			pchT = _tcsinc (pchT);
			_tcscpy (ImageBuffer, pchT);
			pch = _tcsrchr (ImageBuffer, '.');
			pch = _tcsinc (pch);
			memcpy (pch, bufferT, len);
		}
	}

}
			
	
#ifdef OMNI
typedef struct DMOmniClass {
	DWORD	cSections;
	BOOL	fEnabled;
	DWORD	cbJITMaps;
	OmniSectionMapRecord *pSectionMap;
}  DMOmniClass;

__inline DWORD DMOmniPVOffset(DMOmniClass *this, DWORD i)
{
	assert (i < this->cSections);
	return (this->pSectionMap + i)->pvBase;
}
__inline DWORD DMOmniRVAOffset(DMOmniClass *this, DWORD i)
{
	assert (i < this->cSections);
	return (this->pSectionMap + i)->rvaBase;
}
__inline DWORD DMOmniCbSizeOfSection(DMOmniClass *this, DWORD i)
{
	assert (i < this->cSections);
	return (this->pSectionMap + i)->cb;
}
__inline LPVOID DMOmniPJITMap(DMOmniClass *this, DWORD i) 
{
	assert (i < this->cSections);
	return (this->pSectionMap + i)->pJITFixupMap;
}
__inline VOID DMOmniCopyJITMaps(DMOmniClass *this, BYTE* pb)
{
	DWORD i;
	for (i=0; i < this->cSections; i++)
	{
		if ((this->pSectionMap + i)->pJITFixupMap)
		{
			_fmemcpy(pb , (this->pSectionMap + i)->pJITFixupMap, (this->pSectionMap + i)->cb);
			pb += (this->pSectionMap + i)->cb;
		}
	}
}

void DMOmniClassInit(DMOmniClass * this, HPRCX hprc, PVOID pOmniAddrMapIn)
{
	OmniAddrMap OAM;
	if (pOmniAddrMapIn) 
	{
		DWORD cbSectionMap;	 
		DWORD i;
		PVOID pv;

		this->cbJITMaps = 0;

		if (!DbgReadMemory( hprc, (PVOID)pOmniAddrMapIn, (PVOID)&(OAM), sizeof(OmniAddrMap), NULL))
		{
			goto errExit;
		}

		this->cSections = OAM.cSection;
		cbSectionMap = this->cSections * sizeof(OmniSectionMapRecord);

		if (!(this->pSectionMap = (OmniSectionMapRecord *) MHAlloc(cbSectionMap)))
			goto errExit;

		if (!DbgReadMemory( hprc, (PVOID)(OAM.rgSectionMap), (PVOID)(this->pSectionMap), cbSectionMap, NULL))
		{
			goto errExit;
		}

		for (i = 0; i < this->cSections; i++)
		{
			if (this->pSectionMap[i].pJITFixupMap && this->pSectionMap[i].pJITBase) 
			{
				DWORD *pdw;
				PVOID *pv;
				if (!(pv = MHAlloc(this->pSectionMap[i].cb)))
				{
					goto errExit;
				}
				if (!DbgReadMemory(hprc, (PVOID)(this->pSectionMap[i].pJITFixupMap),
					pv, this->pSectionMap[i].cb, NULL)) 
				{
					goto errExit;
				}
				this->pSectionMap[i].pJITFixupMap = pv;
				// add JITBase address to jit map offsets
				for (pdw = (DWORD *) pv;
					(char *)pdw < (char *)pv + this->pSectionMap[i].cb;
					pdw++)
				{
					//preserve zero entries (except for the first entry)
					//these are in the middle of a bytecode instructions > 4 bytes
					if (*pdw || (pdw == (DWORD *)pv))
						*pdw += (DWORD)(this->pSectionMap[i].pJITBase);
				}
				this->cbJITMaps += this->pSectionMap[i].cb;
			}
		}
		this->fEnabled = TRUE;
		return;
	}

errExit:
	this->cbJITMaps = 0;
	this->cSections = 0;
	this->pSectionMap = 0;
	this->fEnabled = FALSE;
}

void DMOmniClassClose(DMOmniClass *this)
{
	DWORD i;

	// free up any jit maps
	for (i=0; i < this->cSections; i++)
	{
		if ((this->pSectionMap + i)->pJITFixupMap)
			MHFree((this->pSectionMap + i)->pJITFixupMap);
	}

	if (this->pSectionMap) MHFree(this->pSectionMap);

}


BOOL
LoadDll5Parm(
    DEBUG_EVENT *   de,
    HTHDX           hthd,
    LPWORD          lpcbPacket,
    LPBYTE *        lplpbPacket,
	LPVOID			pOmniAddrMap
    );

__inline BOOL LoadDll(
    DEBUG_EVENT *   de,
    HTHDX           hthd,
    LPWORD          lpcbPacket,
    LPBYTE *        lplpbPacket
    )
{
	return LoadDll5Parm(de, hthd, lpcbPacket, lplpbPacket, NULL);
}

BOOL
LoadDll5Parm(
    DEBUG_EVENT *   de,
    HTHDX           hthd,
    LPWORD          lpcbPacket,
    LPBYTE *        lplpbPacket,
	LPVOID			pOmniAddrMap
    )
#else
BOOL
LoadDll(
    DEBUG_EVENT *   de,
    HTHDX           hthd,
    LPWORD          lpcbPacket,
    LPBYTE *        lplpbPacket
    )
#endif
/*++

Routine Description:

    This routine is used to load the signification information about
    a PE exe file.  This information consists of the name of the exe
    just loaded (hopefully this will be provided later by the OS) and
    a description of the sections in the exe file.

Arguments:

    de         - Supplies a pointer to the current debug event

    hthd       - Supplies a pointer to the current thread structure

    lpcbPacket - Returns the count of bytes in the created packet

    lplpbPacket - Returns the pointer to the created packet

Return Value:

    True on success and FALSE on failure

--*/

{
    LOAD_DLL_DEBUG_INFO *       ldd = &de->u.LoadDll;
    LPMODULELOAD                lpmdl;
    TCHAR                       szModName[512];
    TCHAR                       szAnsiName[512];
    DWORD                       offset, cbObject;
    DWORD                       lenSz, lenTable;
    DWORD                       cobj, iobj;
    DWORD                       isecTLS;
    IMAGE_DOS_HEADER            dosHdr;
    IMAGE_NT_HEADERS            ntHdr;
    IMAGE_SECTION_HEADER *      rgSecHdr = NULL;
    HANDLE                      hFile;
    int                         iDll;
    HPRCX                       hprc = hthd->hprc;
    DWORD                       cb;
    TCHAR                       rgch[512];
    LPVOID                      lpv;
    LPTSTR                      lpsz;
    ADDR                        addr;
    BOOL                        fTlsPresent;
    OFFSET                      off;
    TCHAR                       fname[_MAX_FNAME];
    TCHAR                       ext[_MAX_EXT];
#ifdef OMNI
	DMOmniClass DMOmni;
#endif


	_asm int 3

    if ( hprc->pstate & (ps_killed | ps_dead) ) {
        //
        //  Process is dead, don't bother doing anything.
        //
        return FALSE;
    }

#ifdef OMNI
	DMOmniClassInit (&DMOmni, hprc, pOmniAddrMap);
#endif

    //
    //  Create an entry in the DLL list and set the index to it so that
    //  we can have information about all DLLs for the current system.
    //
    for (iDll=0; iDll<hprc->cDllList; iDll+=1) {
        if ((hprc->rgDllList[iDll].offBaseOfImage == (DWORD)ldd->lpBaseOfDll) ||
            (!hprc->rgDllList[iDll].fValidDll)) {
            break;
        }
    }

    if (iDll == hprc->cDllList) {
        //
        // the dll list needs to be expanded
        //
        hprc->cDllList += 10;
        if (!hprc->rgDllList) {
            hprc->rgDllList = (PDLLLOAD_ITEM) MHAlloc(sizeof(DLLLOAD_ITEM) * 10);
            memset(hprc->rgDllList, 0, sizeof(DLLLOAD_ITEM)*10);
        } else {
            hprc->rgDllList = MHRealloc(hprc->rgDllList,
                                  hprc->cDllList * sizeof(DLLLOAD_ITEM));
            memset(&hprc->rgDllList[hprc->cDllList-10], 0, 10*sizeof(DLLLOAD_ITEM));
        }
    } else if (hprc->rgDllList[iDll].offBaseOfImage != (DWORD)ldd->lpBaseOfDll) {
        memset(&hprc->rgDllList[iDll], 0, sizeof(DLLLOAD_ITEM));
    }

    //
    // stick the demarcator at the start of the string so that we
    // don't have to move the string over later.
    //
    *szModName = cModuleDemarcator;

    //
    //   Process the DOS header.  It is currently regarded as mandatory
    //
    //   This has to be read before attempting to resolve the
    //   name of a module on NT.  If the name resolution is aborted,
    //   the memory allocated for the IMAGE_SECTION_HEADER must be
    //   freed.
    //
again:
    if (ldd->hFile == 0) {
        SetPointerToMemory(hprc, ldd->lpBaseOfDll);
    } else {
        SetPointerToFile(ldd->hFile);
    }

    SetReadPointer(0, FILE_BEGIN);

    if (DoRead(&dosHdr, sizeof(dosHdr)) == FALSE) {
		if ( (ldd->hFile!=0) && (ldd->hFile!=INVALID_HANDLE_VALUE) ) {
			// bad handle from OS, fallback to reading DLL from memory
			CloseHandle(ldd->hFile);
			ldd->hFile = 0;
			goto again;
		}
        DPRINT(1, (_T("ReadFile got error %u\r\n"), GetLastError()));
        goto exitFalse;
    }

    //
    //  Read in the PE header record
    //

    if ((dosHdr.e_magic != IMAGE_DOS_SIGNATURE) ||
        (SetReadPointer(dosHdr.e_lfanew, FILE_BEGIN) == -1L)) {
        goto exitFalse;
    }

    if (!DoRead(&ntHdr, sizeof(ntHdr))) {
        goto exitFalse;
    }

    //
    //      test whether we have a TLS directory
    //
    fTlsPresent = !!ntHdr.OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_TLS ].Size;

    if (sizeof(ntHdr.OptionalHeader) != ntHdr.FileHeader.SizeOfOptionalHeader) {
        SetReadPointer(ntHdr.FileHeader.SizeOfOptionalHeader -
                       sizeof(ntHdr.OptionalHeader), FILE_CURRENT);
    }

#ifdef OMNI
	if (DMOmni.fEnabled)
	{
		cobj = DMOmni.cSections;
	}
	else
#endif
	{
		//
		//   Save off the count of objects in the dll/exe file
		//
		cobj = ntHdr.FileHeader.NumberOfSections;
        
		//
		//   Save away the offset in the file where the object table
		//   starts.  We will need this later to get information about
		//   each of the objects.
		//
		rgSecHdr = (IMAGE_SECTION_HEADER *) MHAlloc( cobj * sizeof(IMAGE_SECTION_HEADER));
		if (!DoRead( rgSecHdr, cobj * sizeof(IMAGE_SECTION_HEADER))) {
		    assert (FALSE );
			MHFree(rgSecHdr);
			goto exitFalse;
		}
	}



    if (hprc->rgDllList[iDll].offBaseOfImage == (DWORD)ldd->lpBaseOfDll) {

        //
        // in this case we are re-doing a mod load for a dll
        // that is part of a process that is being reconnected
        //
        assert( hprc->rgDllList[iDll].szDllName != NULL );
        _ftcscpy( szModName + 1, hprc->rgDllList[iDll].szDllName );

    } else {


        if (CrashDump) {
            _ftcscpy( szModName+1, ldd->lpImageName );

			FixupDebugImage (szModName + 1);

			
        } else if ((ldd->lpImageName != NULL)
            && DbgReadMemory(hprc,
                             ldd->lpImageName,
                             &lpv,
                             sizeof(lpv),
                             (int *) &cb)
            && (cb == sizeof(lpv))
            && (lpv != NULL)
            && DbgReadMemory(hprc,
                             lpv,
                             rgch,
                             sizeof(rgch),
                             (int *) &cb))
        {

            // we're happy...
#if !defined(_UNICODE)
            if (!ldd->fUnicode) {
                FixFilename(szModName+1, rgch );
            } else {
                WideCharToMultiByte(CP_ACP,
                                    0,
                                    (LPWSTR)rgch,
                                    -1,
                                    szAnsiName,
                                    _tsizeof(szAnsiName),
                                    NULL,
                                    NULL);
                FixFilename(szModName + 1, szAnsiName );
			}
            if ((hprc->rgDllList[iDll].szDllName = MHAlloc(_tcslen(szModName+1) + 1)) != NULL)
            {
                _tcscpy(hprc->rgDllList[iDll].szDllName, szModName+1);
            } else {
				goto exitFalse;
            }
#else
            if (ldd->fUnicode) {
                FixFilename(szModName+1, rgch );
            } else {
                MultiByteToWideChar(CP_ACP,
                                    0,
                                    (LPTSTR)rgch,
                                    -1,
                                    szAnsiName,
                                    _tsizeof(szAnsiName));
                FixFilename(szModName + 1, szAnsiName );
                if ((hprc->rgDllList[iDll].szDllName = MHAlloc((_tcslen(szModName+1) + 1)) * sizeof(TCHAR)) != NULL)
                {
                    _tcscpy(hprc->rgDllList[iDll].szDllName, szModName+1);
                } else {
					goto exitFalse;
                }
            }
#endif

        }

        else {
#ifdef OMNI
			assert(!DMOmni.fEnabled);
#endif
			if (*nameBuffer) {

            /*
             *      If *nameBuffer != 0 then we know we are really
             *      dealing with the root exe and we can steal the
             *      name from there.
             */

            if (FDMRemote) {
                _tsplitpath( nameBuffer, NULL, NULL, fname, ext );
                sprintf( szModName+1, _T("#:\\%s%s"), fname, ext );
            } else {
                _tcscpy(szModName + 1, nameBuffer);     // do NOT FixFilename this one
            }

            if ((hprc->rgDllList[iDll].szDllName = MHAlloc(_tcslen(szModName+1) + 1)) != NULL)
            {
                _tcscpy(hprc->rgDllList[iDll].szDllName, szModName+1);
            } else {
			     goto exitFalse;
            }

        } else if (GetModNameUsingPsApi(hthd, ldd, rgch, _tsizeof(rgch))) {
            // cool...
            FixFilename(szModName + 1, rgch);
            if ((hprc->rgDllList[iDll].szDllName = MHAlloc(_tcslen(szModName+1) + 1)) != NULL)
            {
                _tcscpy(hprc->rgDllList[iDll].szDllName, szModName+1);
            }
            else
            {
			     goto exitFalse;
            }
        } else if (GetModnameFromImage(&ntHdr, rgSecHdr, ldd, rgch, _tsizeof(rgch))) {

            // joyful...
            lpsz = _ftcsrchr(rgch, _T('\\'));
            if (!lpsz) {
                lpsz = _ftcsrchr(rgch, _T(':'));
            }
            if (lpsz) {
                lpsz = _ftcsinc(lpsz);
            } else {
                lpsz = rgch;
            }
#if defined(DOLPHIN)
            if (FGetNTFileName(hthd, &hprc->rgDllList[iDll], ldd->hFile, rgch, _tsizeof(rgch))) {
                _ftcscpy(szModName + 1, rgch);
            }
                        else
#endif
            {
                _ftcscpy(szModName + 1, _T("#:\\"));
                _ftcscpy(szModName + 4, lpsz);
            }

            if ((hprc->rgDllList[iDll].szDllName = MHAlloc(_tcslen(lpsz) + 1)) != NULL)
            {
                _tcscpy(hprc->rgDllList[iDll].szDllName, lpsz);
            }
            else
            {
				goto exitFalse;
            }

        } else if (GetModnameFromExportTable(&ntHdr, rgSecHdr, ldd, rgch, _tsizeof(rgch))) {

            // serene...
            _ftcscpy(szModName + 1, rgch);
            if ((hprc->rgDllList[iDll].szDllName = MHAlloc(_tcslen(rgch) + 1)) != NULL)
            {
                _tcscpy(hprc->rgDllList[iDll].szDllName, rgch);
            }
            else
            {
			     goto exitFalse;
            }

        } else {

            // hopeless...
#if defined(DOLPHIN)
            if (!LoadString(hInstance, IDS_UnknownExe, rgch, _tsizeof(rgch))) {
                assert(FALSE);
            }
            if (FGetNTFileName(hthd, &hprc->rgDllList[iDll], ldd->hFile, rgch, _tsizeof(rgch))) {
                _ftcscpy(szModName + 1, rgch);
            } else
#endif
            sprintf(szModName+1, _T("#:\\APP%02d.EXE"), ++iTemp1);
            if ((hprc->rgDllList[iDll].szDllName = MHAlloc(_tcslen(szModName + 1) + 1)) != NULL)
            {
                _tcscpy(hprc->rgDllList[iDll].szDllName, szModName + 1);
            }
            else
            {
		        goto exitFalse;
            }
        }
		}

        *nameBuffer = 0;
    }

    //
    // for remote case, kill the drive letter to
    // prevent finding same exe on wrong platform,
    // except when user gave path to exe.
    //
    if (fUseRealName) {
        fUseRealName = FALSE;
    }
    lenSz=_ftcslen(szModName);
    DPRINT(10, (_T("*** LoadDll %s  base=%x\n"), szModName, ldd->lpBaseOfDll));

    szModName[lenSz] = 0;

    lpsz = _ftcsrchr(szModName, _T('\\'));
    if (!lpsz) {
        lpsz = _ftcsrchr(szModName, _T(':'));
    }
    if (lpsz) {
        lpsz = _ftcsinc(lpsz);
    } else {
        lpsz = szModName;
    }

    if (_ftcsicmp(lpsz, _T("kernel32.dll")) == 0) {
        hprc->dwKernel32Base = (DWORD)ldd->lpBaseOfDll;
    }

    if (hprc->rgDllList[iDll].offBaseOfImage != (DWORD)ldd->lpBaseOfDll) {
        //
        // new dll to add to the list
        //
        hprc->rgDllList[iDll].fValidDll = TRUE;
        hprc->rgDllList[iDll].offBaseOfImage = (OFFSET) ldd->lpBaseOfDll;
        hprc->rgDllList[iDll].cbImage = ntHdr.OptionalHeader.SizeOfImage;
    }

    //
    //  Find address of OLE RPC tracing export (if any)
    //

    //
    // If hFile is NULL, we can not do any RPC debugging. This may happen in
    // certain platforms. So, check for this.
    
    if (ldd->hFile != NULL) {
    
        FGetExport (&hprc->rgDllList[iDll],
                    (HFILE)ldd->hFile,
                    _T("DllDebugObjectRPCHook"),
                    &hprc->rgDllList[iDll].lpvOleRpc
                    );
    } else {
    
        hprc->rgDllList[iDll].lpvOleRpc = NULL;
    }

    //DMSqlLoadDll( hprc, ldd, iDll );

    szModName[lenSz] = cModuleDemarcator;

    if (/* FDMRemote */ TRUE ) {
        if (ldd->hFile != 0 && ldd->hFile != (HANDLE)-1) {
            CloseHandle(ldd->hFile);  //  don't need this anymore
        }
        hFile = (HANDLE)-1; // remote: can't send file handle across wire
    } else {

        if (ldd->hFile == 0) {
            hFile = (HANDLE)-1;
        } else {
            hFile = ldd->hFile; // local: let SH use our handle
        }
    }

    /*
     *  Make up a record to send back from the name.
     *  Additionally send back:
     *          The file handle (if local)
     *          The load base of the dll
     *          The time and date stamp of the exe
     *          The checksum of the file
     */

    sprintf( szModName+lenSz+1, _T("0x%08lX%c0x%08lX%c0x%08lX%c0x%08lX%c"),
            ntHdr.FileHeader.TimeDateStamp, cModuleDemarcator,
            ntHdr.OptionalHeader.CheckSum, cModuleDemarcator,
            hFile, cModuleDemarcator,
            (long) ldd->lpBaseOfDll, cModuleDemarcator);
    lenSz = _ftcslen(szModName);
    /*
     * Allocate the packet which will be sent across to the EM.
     * The packet will consist of:
     *     The MODULELOAD structure             sizeof(MODULELOAD) +
     *     The section description array        cobj*sizeof(OBJD) +
     *     The name of the DLL                  lenSz+1
     */

	lenTable = (cobj * sizeof(OBJD));
#ifdef OMNI
    *lpcbPacket = (WORD)(sizeof(MODULELOAD) + lenTable + (lenSz+1) * sizeof(TCHAR) +
		DMOmni.cbJITMaps);
#else
    *lpcbPacket = (WORD)(sizeof(MODULELOAD) + lenTable + (lenSz+1) * sizeof(TCHAR));
#endif
    *lplpbPacket= (LPBYTE)(lpmdl=(LPMODULELOAD)MHAlloc(*lpcbPacket));
    lpmdl->lpBaseOfDll = ldd->lpBaseOfDll;
    lpmdl->cobj = cobj;
    lpmdl->mte = (WORD) -1;
#ifdef OMNI
	lpmdl->fOmniMap = DMOmni.fEnabled;
#endif
#ifdef TARGET_i386
    lpmdl->CSSel    = (unsigned short)hthd->context.SegCs;
    lpmdl->DSSel    = (unsigned short)hthd->context.SegDs;
#else
    lpmdl->CSSel = lpmdl->DSSel = 0;
#endif // i386

    //
    // Set up the descriptors for each of the section headers so that the EM
    // can map between section numbers and flat addresses.
    //

    lpmdl->uoffDataBase = 0;
    
    for (iobj=0; iobj<cobj; iobj++) {
        OLESEG oleseg;

        lpmdl->rgobjd[iobj].wPad = 1;

#ifdef OMNI
		if (DMOmni.fEnabled)
		{
			lpmdl->rgobjd[iobj].offset  = DMOmniPVOffset(&DMOmni, iobj);
			lpmdl->rgobjd[iobj].rvaOffset  = DMOmniRVAOffset(&DMOmni, iobj);
			lpmdl->rgobjd[iobj].cb = DMOmniCbSizeOfSection(&DMOmni, iobj);
			lpmdl->rgobjd[iobj].pOmniJITMap = DMOmniPJITMap(&DMOmni, iobj);
			lpmdl->rgobjd[iobj].wSel = 0;
		}
		else
#endif
		{
#ifdef OMNI
			lpmdl->rgobjd[iobj].pOmniJITMap = NULL;
#endif
			offset = rgSecHdr[iobj].VirtualAddress + (long) ldd->lpBaseOfDll;
			cbObject = rgSecHdr[iobj].Misc.VirtualSize;
			if (cbObject == 0) {
			    cbObject = rgSecHdr[iobj].SizeOfRawData;
			}

			lpmdl->rgobjd[iobj].offset = offset;
			lpmdl->rgobjd[iobj].cb = cbObject;
#if defined(TARGET_i386)

			if (IMAGE_SCN_CNT_CODE & rgSecHdr[iobj].Characteristics) {
			    lpmdl->rgobjd[iobj].wSel = (WORD) hthd->context.SegCs;
			} else {
			    lpmdl->rgobjd[iobj].wSel = (WORD) hthd->context.SegDs;
			}
#else
			lpmdl->rgobjd[iobj].wSel = 0;
#endif  // TARGET_i386

			if (!_fmemcmp( rgSecHdr[iobj].Name, ".data\0\0", IMAGE_SIZEOF_SHORT_NAME)) {
				if (lpmdl->uoffDataBase == 0) {
					lpmdl->uoffDataBase = offset;
				}
			}
        
#if 0
			//
			// If the section is one of the special OLE segments, we keep track
			// of the address ranges in the OLERG structure.
			//
         
			if ((oleseg = GetOleSegType(rgSecHdr[iobj].Name)) != olenone) {
				OLERG*      lpolerg;
				DWORD       i;

				hprc->rgDllList[iDll].fContainsOle = TRUE;

				++hprc->colerg;
				if (hprc->rgolerg) {

				 hprc->rgolerg = MHRealloc(hprc->rgolerg,
				                           hprc->colerg * sizeof(OLERG));

				} else {
            
				 hprc->rgolerg = MHAlloc(hprc->colerg * sizeof(OLERG));
				}

				/* find place to insert new OLE range */

				for (i = 0; i < hprc->colerg - 1; ++i) {
					if (offset < hprc->rgolerg[i].uoffMin) {
						break;
					}
				}

				/* insert an OLERG */
				memmove(&hprc->rgolerg[i+1],
                    &hprc->rgolerg[i],
                    sizeof(OLERG) * (hprc->colerg - i - 1));

				/* insert new OLE range */
				lpolerg = &hprc->rgolerg[i];
				lpolerg->uoffMin   = offset;
				lpolerg->uoffMax   = offset + cbObject;
				lpolerg->segType   = oleseg;

#if 0
				// undef this is you want to see the module names and ranges
				// of the orpc modules
            
				DebugPrint ("dll: %s\n", szModName);

				DebugPrint ("Ole segment: %s %#x - %#x\n",
					rgSecHdr [iobj].Name,
                    lpolerg->uoffMin,
                    lpolerg->uoffMax);
#endif
            
			}

#endif

			if ( fTlsPresent && !_fmemcmp ( rgSecHdr[iobj].Name, ".tls\0\0\0", IMAGE_SIZEOF_SHORT_NAME ) ) {
				isecTLS = iobj + 1;
			}
		}
    }

    lpmdl->fRealMode = FALSE;
    lpmdl->fFlatMode = TRUE;
    lpmdl->fOffset32 = TRUE;
    lpmdl->dwSizeOfDll = ntHdr.OptionalHeader.SizeOfImage;
    lpmdl->uoffiTls = 0;
    lpmdl->isecTLS = 0;
    lpmdl->iTls = 0;       // cache it later on demand if uoffiTls != 0

    /*
     *  Copy the name of the dll to the end of the packet.
     */

    _fmemcpy(((BYTE*)&lpmdl->rgobjd)+lenTable, szModName, lenSz+1);
#ifdef OMNI
	if (DMOmni.fEnabled) 
	{
		DMOmniCopyJITMaps(&DMOmni, ((BYTE*)&lpmdl->rgobjd)+lenTable + lenSz + 1);
	}
#endif

    /*
     *  Locate the TLS section if one exists.  If so then get the
     *      pointer to the TLS index
     *
     *  Structure at the address is:
     *
     *          VA      lpRawData
     *          ULONG   cbRawData
     *          VA      lpIndex
     *          VA      lpCallBacks
     */

     if (ntHdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress != 0) {
#ifdef OMNI
		 // cannot support tls debugging in omni as of yet
		 assert(!DMOmni.fEnabled);
#endif
         if ((DbgReadMemory(hprc,
                            ntHdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress + (char *) ldd->lpBaseOfDll + 8,
                            &off,
                            sizeof(OFFSET),
                            &cb) == 0) ||
             (cb != sizeof(OFFSET))) {
             assert(FALSE);
         }

         hprc->rgDllList[iDll].offTlsIndex = off;
         lpmdl->uoffiTls = off;
         lpmdl->isecTLS = isecTLS;
     }

    /*
     * free up the memory used for holding the section headers
     */

#ifdef OMNI
	if (!DMOmni.fEnabled)
#endif
		MHFree(rgSecHdr);


    if (fDisconnected) {

        //
        // this will prevent the dm from sending a message up to
        // the shell.  the dm's data structures are setup just fine
        // so that when the debugger re-connects we can deliver the
        // mod loads correctly.
        //

exitFalse:
#ifdef OMNI
		DMOmniClassClose(&DMOmni);
#endif
		return FALSE;

    }

#ifdef OMNI
		DMOmniClassClose(&DMOmni);
#endif
    return TRUE;
}                               /* LoadDll() */

#endif  // !KERNEL
#endif // 0

int
FindTLSSection(
    PDMN_MODLOAD pdmml
    )
{
    IMAGE_DOS_HEADER dosh;
    IMAGE_NT_HEADERS nth;
    int isec;
    DWORD dwSecHdrAddr;
    IMAGE_SECTION_HEADER sh;

    /* Start by getting the DOS header */
    if(FAILED(DmGetMemory(pdmml->BaseAddress, sizeof dosh, &dosh, NULL)))
        return 0;

    /* Verify the signature */
    if(dosh.e_magic != IMAGE_DOS_SIGNATURE)
        return 0;

    /* Get the base NT headers */
    if(FAILED(DmGetMemory((PVOID)((DWORD)pdmml->BaseAddress + dosh.e_lfanew),
            sizeof nth, &nth, NULL)))
        return 0;

    /* Walk the section header list, looking for .tls */
    dwSecHdrAddr = (DWORD)pdmml->BaseAddress + dosh.e_lfanew + sizeof nth +
        nth.FileHeader.SizeOfOptionalHeader - sizeof nth.OptionalHeader;
    for(isec = 0; isec < nth.FileHeader.NumberOfSections; ++isec) {
        if(FAILED(DmGetMemory((PVOID)dwSecHdrAddr, sizeof sh, &sh, NULL)))
            return 0;
        if(0 == _fmemcmp(sh.Name, ".tls\0\0\0", IMAGE_SIZEOF_SHORT_NAME))
            return isec + 1;
        dwSecHdrAddr += sizeof sh;
    }

    /* Didn't find it */
    return 0;
}

BOOL
LoadDll(
    DEBUG_EVENT *   de,
    HTHDX           hthd,
    LPWORD          lpcbPacket,
    LPBYTE *        lplpbPacket
    )
{
    LOAD_DLL_DEBUG_INFO *       ldd = &de->u.LoadDll;
	PDMN_MODLOAD				pdmml = ldd->hFile;
    LPMODULELOAD                lpmdl;
    TCHAR                       szModName[512];
    DWORD                       offset, cbObject;
    DWORD                       lenSz, lenTable;
    DWORD                       cobj, iobj;
    DWORD                       isecTLS;
    int                         iDll;
    HPRCX                       hprc = hthd->hprc;
    DWORD                       cb;
    LPVOID                      lpv;
    LPTSTR                      lpsz;
    ADDR                        addr;
    OFFSET                      off;
	PDM_WALK_MODSECT			pdmws;
	DMN_SECTIONLOAD				dmsl;
	PDMN_SECTIONLOAD			*rgpdmsl;

    if ( hprc->pstate & (ps_killed | ps_dead) ) {
        //
        //  Process is dead, don't bother doing anything.
        //
        return FALSE;
    }


    //
    //  Create an entry in the DLL list and set the index to it so that
    //  we can have information about all DLLs for the current system.
    //
    for (iDll=0; iDll<hprc->cDllList; iDll+=1) {
        if ((hprc->rgDllList[iDll].offBaseOfImage == (DWORD)ldd->lpBaseOfDll) ||
            (!hprc->rgDllList[iDll].fValidDll)) {
            break;
        }
    }

    if (iDll == hprc->cDllList) {
        //
        // the dll list needs to be expanded
        //
        hprc->cDllList += 10;
        if (!hprc->rgDllList) {
            hprc->rgDllList = (PDLLLOAD_ITEM) MHAlloc(sizeof(DLLLOAD_ITEM) * 10);
            memset(hprc->rgDllList, 0, sizeof(DLLLOAD_ITEM)*10);
        } else {
            hprc->rgDllList = MHRealloc(hprc->rgDllList,
                                  hprc->cDllList * sizeof(DLLLOAD_ITEM));
            memset(&hprc->rgDllList[hprc->cDllList-10], 0, 10*sizeof(DLLLOAD_ITEM));
        }
    } else if (hprc->rgDllList[iDll].offBaseOfImage != (DWORD)ldd->lpBaseOfDll) {
        memset(&hprc->rgDllList[iDll], 0, sizeof(DLLLOAD_ITEM));
    }

    //
    // stick the demarcator at the start of the string so that we
    // don't have to move the string over later.
    //
    *szModName = cModuleDemarcator;

	// Walk this module's section list
	pdmws = NULL;
	cobj = 0;
    rgpdmsl = NULL;
	while(SUCCEEDED(DmWalkModuleSections(&pdmws, pdmml->Name, &dmsl)))
	{
        /* Reallocate the module list if necessary */
        if((cobj & 31) == 0) {
        	PDMN_SECTIONLOAD *rgpdmslT;
            rgpdmslT = rgpdmsl;
            rgpdmsl = MHAlloc((cobj + 32) * sizeof(void *));
            if(rgpdmslT) {
                memcpy(rgpdmsl, rgpdmslT, cobj * sizeof(void *));
                MHFree(rgpdmslT);
            }
            if(!rgpdmsl) {
                /* We're screwed now */
                cobj = 0;
                break;
            }
        }

		rgpdmsl[cobj] = MHAlloc(sizeof(dmsl));
		if(!rgpdmsl[cobj])
			break;
		*rgpdmsl[cobj++] = dmsl;
	}
	DmCloseModuleSections(pdmws);

    if (hprc->rgDllList[iDll].offBaseOfImage == (DWORD)ldd->lpBaseOfDll) {

        //
        // in this case we are re-doing a mod load for a dll
        // that is part of a process that is being reconnected
        //
        assert( hprc->rgDllList[iDll].szDllName != NULL );
        _ftcscpy( szModName + 1, hprc->rgDllList[iDll].szDllName );

    } else {

#if 0
        if (CrashDump) {
            _ftcscpy( szModName+1, ldd->lpImageName );

			_asm int 3
			//FixupDebugImage (szModName + 1);

			
        } else
#endif
		{

            // we're happy...
            //FixFilename(szModName+1, rgch );
			strcpy(szModName+1, pdmml->Name);
            if ((hprc->rgDllList[iDll].szDllName = MHAlloc(_tcslen(szModName+1) + 1)) != NULL)
            {
                _tcscpy(hprc->rgDllList[iDll].szDllName, szModName+1);
            } else {
				goto exitFalse;
            }

		}

        *nameBuffer = 0;
    }

    //
    // for remote case, kill the drive letter to
    // prevent finding same exe on wrong platform,
    // except when user gave path to exe.
    //
    if (fUseRealName) {
        fUseRealName = FALSE;
    }
    lenSz=_ftcslen(szModName);
    DPRINT(10, (_T("*** LoadDll %s  base=%x\n"), szModName, ldd->lpBaseOfDll));

    szModName[lenSz] = 0;

    lpsz = _ftcsrchr(szModName, _T('\\'));
    if (!lpsz) {
        lpsz = _ftcsrchr(szModName, _T(':'));
    }
    if (lpsz) {
        lpsz = _ftcsinc(lpsz);
    } else {
        lpsz = szModName;
    }

	// REVIEW jlange -- change this to ntoskrnl?
    if (_ftcsicmp(lpsz, _T("kernel32.dll")) == 0) {
        hprc->dwKernel32Base = (DWORD)ldd->lpBaseOfDll;
    }

    if (hprc->rgDllList[iDll].offBaseOfImage != (DWORD)ldd->lpBaseOfDll) {
        //
        // new dll to add to the list
        //
        hprc->rgDllList[iDll].fValidDll = TRUE;
        hprc->rgDllList[iDll].offBaseOfImage = (OFFSET) ldd->lpBaseOfDll;
        hprc->rgDllList[iDll].cbImage = pdmml->Size;
    }

    szModName[lenSz] = cModuleDemarcator;

    /*
     *  Make up a record to send back from the name.
     *  Additionally send back:
     *          The file handle (if local)
     *          The load base of the dll
     *          The time and date stamp of the exe
     *          The checksum of the file
     */

    sprintf( szModName+lenSz+1, _T("0x%08lX%c0x%08lX%c0x%08lX%c0x%08lX%c"),
            pdmml->TimeStamp, cModuleDemarcator,
            pdmml->CheckSum, cModuleDemarcator,
            -1L, cModuleDemarcator,
            (long) ldd->lpBaseOfDll, cModuleDemarcator);
    lenSz = _ftcslen(szModName);
    /*
     * Allocate the packet which will be sent across to the EM.
     * The packet will consist of:
     *     The MODULELOAD structure             sizeof(MODULELOAD) +
     *     The section description array        cobj*sizeof(OBJD) +
     *     The name of the DLL                  lenSz+1
     */

	lenTable = (cobj * sizeof(OBJD));
    *lpcbPacket = (WORD)(sizeof(MODULELOAD) + lenTable + (lenSz+1) * sizeof(TCHAR));
    *lplpbPacket= (LPBYTE)(lpmdl=(LPMODULELOAD)MHAlloc(*lpcbPacket));
    lpmdl->lpBaseOfDll = ldd->lpBaseOfDll;
    lpmdl->cobj = cobj;
    lpmdl->mte = (WORD) -1;
    lpmdl->CSSel = lpmdl->DSSel = 0;

    //
    // Set up the descriptors for each of the section headers so that the EM
    // can map between section numbers and flat addresses.
    //

    lpmdl->uoffDataBase = 0;

    isecTLS = 0;
    for (iobj=0; iobj<cobj; iobj++) {

        lpmdl->rgobjd[iobj].wPad = 1;

		lpmdl->rgobjd[iobj].offset = (DWORD)rgpdmsl[iobj]->BaseAddress;;
		lpmdl->rgobjd[iobj].cb = rgpdmsl[iobj]->Size;
		lpmdl->rgobjd[iobj].wSel = 0;

		if (!memcmp( rgpdmsl[iobj]->Name, ".data\0\0", 7)) {
			if (lpmdl->uoffDataBase == 0) {
				lpmdl->uoffDataBase = lpmdl->rgobjd[iobj].offset;
			}
		}

		MHFree(rgpdmsl[iobj]);

    }

    if(rgpdmsl)
        MHFree(rgpdmsl);

    lpmdl->fRealMode = FALSE;
    lpmdl->fFlatMode = TRUE;
    lpmdl->fOffset32 = TRUE;
    lpmdl->dwSizeOfDll = pdmml->Size;
    lpmdl->uoffiTls = 0;
    lpmdl->isecTLS = 0;
    lpmdl->iTls = 0;       // cache it later on demand if uoffiTls != 0

    /*
     *  Copy the name of the dll to the end of the packet.
     */

    memcpy(((BYTE*)&lpmdl->rgobjd)+lenTable, szModName, lenSz+1);

    /*
     *  Locate the TLS section if one exists.  If so then get the
     *      pointer to the TLS index
     */

    if(pdmml->Flags & DMN_MODFLAG_TLS) {
        /* The TLS section is gone from the list at this point, but if the
         * PE headers are present, then we can figure out which section was
         * the TLS sections by examining them */
        isecTLS = FindTLSSection(pdmml);
        if(isecTLS) {
            hprc->rgDllList[iDll].offTlsIndex = 1;
            lpmdl->uoffiTls = 1;
            lpmdl->isecTLS = isecTLS;
        }
    }

    if (fDisconnected) {

        //
        // this will prevent the dm from sending a message up to
        // the shell.  the dm's data structures are setup just fine
        // so that when the debugger re-connects we can deliver the
        // mod loads correctly.
        //

exitFalse:
		return FALSE;

    }

    return TRUE;
}

#ifdef KERNEL

BOOL
GetModnameFromImage(
    HPRCX                   hprc,
    LOAD_DLL_DEBUG_INFO     *ldd,
    LPTSTR                   lpName
    )
/*++

Routine Description:

    This routine attempts to get the name of the exe as placed
    in the debug section by the linker.

Arguments:

Return Value:

    TRUE if a name was found, FALSE if not.
    The exe name is returned as an ANSI string in lpName.

--*/
{
    #define ReadMem(b,s) DbgReadMemory( hprc, (LPVOID)(address), (b), (s), NULL ); address += (s)

    IMAGE_DEBUG_DIRECTORY       DebugDir;
    PIMAGE_DEBUG_MISC           pMisc;
    PIMAGE_DEBUG_MISC           pT;
    DWORD                       rva;
    int                         nDebugDirs;
    int                         i;
    int                         j;
    int                         l;
    BOOL                        rVal = FALSE;
    PVOID                       pExeName;
    IMAGE_NT_HEADERS            nh;
    IMAGE_DOS_HEADER            dh;
    IMAGE_ROM_OPTIONAL_HEADER   rom;
    DWORD                       address;
    DWORD                       sig;
    PIMAGE_SECTION_HEADER       pSH;
    DWORD                       cb;


    lpName[0] = 0;

    address = (ULONG)ldd->lpBaseOfDll;

    ReadMem( &dh, sizeof(dh) );

    if (dh.e_magic == IMAGE_DOS_SIGNATURE) {
        address = (ULONG)ldd->lpBaseOfDll + dh.e_lfanew;
    } else {
        address = (ULONG)ldd->lpBaseOfDll;
    }

    ReadMem( &sig, sizeof(sig) );
    address -= sizeof(sig);

    if (sig == IMAGE_NT_SIGNATURE) {
        ReadMem( &nh, sizeof(nh) );
    } else {
        ReadMem( &nh.FileHeader, sizeof(IMAGE_FILE_HEADER) );
        if (nh.FileHeader.SizeOfOptionalHeader == IMAGE_SIZEOF_ROM_OPTIONAL_HEADER) {
            ReadMem( &rom, sizeof(rom) );
            ZeroMemory( &nh.OptionalHeader, sizeof(nh.OptionalHeader) );
            nh.OptionalHeader.SizeOfImage      = rom.SizeOfCode;
            nh.OptionalHeader.ImageBase        = rom.BaseOfCode;
        } else {
            return FALSE;
        }
    }

    cb = nh.FileHeader.NumberOfSections * IMAGE_SIZEOF_SECTION_HEADER;
    pSH = MHAlloc( cb );
    ReadMem( pSH, cb );

    nDebugDirs = nh.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size /
                 sizeof(IMAGE_DEBUG_DIRECTORY);

    if (!nDebugDirs) {
        return FALSE;
    }

    rva = nh.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;

    for(i = 0; i < nh.FileHeader.NumberOfSections; i++) {
        if (rva >= pSH[i].VirtualAddress &&
            rva < pSH[i].VirtualAddress + pSH[i].SizeOfRawData) {
            break;
        }
    }

    if (i >= nh.FileHeader.NumberOfSections) {
        return FALSE;
    }

    rva = ((rva - pSH[i].VirtualAddress) + pSH[i].VirtualAddress);

    for (j = 0; j < nDebugDirs; j++) {

        address = rva + (sizeof(DebugDir) * j) + (ULONG)ldd->lpBaseOfDll;
        ReadMem( &DebugDir, sizeof(DebugDir) );

        if (DebugDir.Type == IMAGE_DEBUG_TYPE_MISC) {

            l = DebugDir.SizeOfData;
            pMisc = pT = MHAlloc(l);

            if ((ULONG)DebugDir.AddressOfRawData < pSH[i].VirtualAddress ||
                  (ULONG)DebugDir.AddressOfRawData >=
                                         pSH[i].VirtualAddress + pSH[i].SizeOfRawData) {
                //
                // the misc debug data MUST be in the .rdata section
                // otherwise windbg cannot access it as it is not mapped in
                //
                continue;
            }

            address = (ULONG)DebugDir.AddressOfRawData + (ULONG)ldd->lpBaseOfDll;
            ReadMem( pMisc, l );

            while (l > 0) {
                if (pMisc->DataType != IMAGE_DEBUG_MISC_EXENAME) {
                    l -= pMisc->Length;
                    pMisc = (PIMAGE_DEBUG_MISC)
                                (((LPSTR)pMisc) + pMisc->Length);
                } else {

                    pExeName = (PVOID)&pMisc->Data[ 0 ];

                    if (!pMisc->Unicode) {
                        _tcscpy(lpName, (LPSTR)pExeName);
                        rVal = TRUE;
                    } else {
                        WideCharToMultiByte(CP_ACP,
                                            0,
                                            (LPWSTR)pExeName,
                                            -1,
                                            lpName,
                                            MAX_PATH,
                                            NULL,
                                            NULL);
                        rVal = TRUE;
                    }

                    /*
                     *  Undo stevewo's error
                     */

                    if (_ftcsicmp(&lpName[_ftcslen(lpName)-4], ".DBG") == 0) {
                        TCHAR    rgchPath[_MAX_PATH];
                        TCHAR    rgchBase[_MAX_FNAME];

                        _splitpath(lpName, NULL, rgchPath, rgchBase, NULL);
                        if (_ftcslen(rgchPath)==4) {
                            rgchPath[_ftcslen(rgchPath)-1] = 0;
                            _ftcscpy(lpName, rgchBase);
                            _ftcscat(lpName, _T("."));
                            _ftcscat(lpName, rgchPath);
                        } else {
                            _ftcscpy(lpName, rgchBase);
                            _ftcscat(lpName, _T(".exe"));
                        }
                    }
                    break;
                }
            }

            MHFree(pT);

            break;

        }
    }

    return rVal;
}

LPMODULEALIAS
FindAddAliasByModule(
    LPSTR lpImageName,
    LPSTR lpModuleName
    )
/*++

Routine Description:

    Look for an alias entry by its "common" name, for example, look
    for "NT".  If it does not exist, and a new image name has been
    provided, add it.  If it does exist and a new image name has been
    provided, replace the image name.  Return the new or found record.

Arguments:


Return Value:


--*/
{
    int i;

    for (i=0; i<MAX_MODULEALIAS; i++) {
        if (ModuleAlias[i].ModuleName[0] == 0) {
            if (!lpImageName) {
                return NULL;
            } else {
                _tcscpy( ModuleAlias[i].Alias, lpImageName );
                _tcscpy( ModuleAlias[i].ModuleName, lpModuleName );
                ModuleAlias[i].Special = 1;
                return &ModuleAlias[i];
            }
        }
        if (_tcsicmp( ModuleAlias[i].ModuleName, lpModuleName ) == 0) {
            if (lpImageName) {
                _tcscpy( ModuleAlias[i].Alias, lpImageName);
            }
            return &ModuleAlias[i];
        }
    }
}

LPMODULEALIAS
FindAliasByImageName(
    LPSTR lpImageName
    )
{
    int i;

    for (i=0; i<MAX_MODULEALIAS; i++) {
        if (ModuleAlias[i].ModuleName[0] == 0) {
            return NULL;
        }
        if (_tcsicmp( ModuleAlias[i].Alias, lpImageName ) == 0) {
            return &ModuleAlias[i];
        }
    }
}

LPMODULEALIAS
CheckForRenamedImage(
    HPRCX hprc,
    LOAD_DLL_DEBUG_INFO *ldd,
    LPTSTR lpOrigImageName,
    LPTSTR lpModuleName
    )
{
    CHAR  ImageName[MAX_PATH];
    CHAR  fname[_MAX_FNAME];
    CHAR  ext[_MAX_EXT];
    DWORD i;


    if (_ftcsicmp( ldd->lpImageName, lpOrigImageName ) != 0) {
        return NULL;
    }

    if (GetModnameFromImage( hprc, ldd, ImageName ) && ImageName[0]) {
        _splitpath( ImageName, NULL, NULL, fname, ext );
        sprintf( ImageName, _T("%s%s"), fname, ext );
        return FindAddAliasByModule(ImageName, lpModuleName);
    }

    return NULL;
}


BOOL
LoadDll(
    DEBUG_EVENT *   de,
    HTHDX           hthd,
    LPWORD          lpcbPacket,
    LPBYTE *        lplpbPacket
    )
/*++

Routine Description:

    This routine is used to load the signification information about
    a PE exe file.  This information consists of the name of the exe
    just loaded (hopefully this will be provided later by the OS) and
    a description of the sections in the exe file.

Arguments:

    de         - Supplies a pointer to the current debug event

    hthd       - Supplies a pointer to the current thread structure

    lpcbPacket - Returns the count of bytes in the created packet

    lplpbPackt - Returns the pointer to the created packet

Return Value:

    True on success and FALSE on failure

--*/

{
    LOAD_DLL_DEBUG_INFO *       ldd = &de->u.LoadDll;
    LPMODULELOAD                lpmdl;
    CHAR                        szModName[MAX_PATH];
    DWORD                       lenSz;
    INT                         iDll;
    HPRCX                       hprc = hthd->hprc;
    CHAR                        fname[_MAX_FNAME];
    CHAR                        ext[_MAX_EXT];
    CHAR                        szFoundName[_MAX_PATH];
    LPMODULEALIAS               Alias = NULL;
    DWORD                       i;
    IMAGEINFO                   ii;

    static int FakeDllNumber = 0;
    TCHAR FakeDllName[13];

    //
    // extern owned by kdapi.c:
    //
    extern DWORD CacheProcessors;


    if ( hprc->pstate & (ps_killed | ps_dead) ) {
        //
        //  Process is dead, don't bother doing anything.
        //
        return FALSE;
    }

    if ( *(LPTSTR)ldd->lpImageName == 0 ) {
        ldd->lpImageName = FakeDllName;
        sprintf(FakeDllName, _T("DLL%05x"), FakeDllNumber++);
    }

    if (_ftcsicmp( ldd->lpImageName, HAL_IMAGE_NAME ) == 0) {
        Alias = CheckForRenamedImage( hprc, ldd, HAL_IMAGE_NAME, HAL_MODULE_NAME );
    }

    if (_ftcsicmp( ldd->lpImageName, KERNEL_IMAGE_NAME ) == 0) {
        Alias = CheckForRenamedImage( hprc, ldd, KERNEL_IMAGE_NAME, KERNEL_MODULE_NAME );
    if (!Alias && CacheProcessors > 1) {
            Alias = FindAddAliasByModule( KERNEL_IMAGE_NAME_MP, KERNEL_MODULE_NAME );
        }
    }

    if (!Alias) {
        Alias = FindAliasByImageName(ldd->lpImageName);
    }

    //
    //  Create an entry in the DLL list and set the index to it so that
    //  we can have information about all DLLs for the current system.
    //
    for (iDll=0; iDll<hprc->cDllList; iDll+=1) {
        if (!hprc->rgDllList[iDll].fValidDll) {
            break;
        }
    }

    if (iDll == hprc->cDllList) {
        //
        // the dll list needs to be expanded
        //
        hprc->cDllList += 10;
        hprc->rgDllList = MHRealloc(hprc->rgDllList,
                                  hprc->cDllList * sizeof(DLLLOAD_ITEM));
        memset(&hprc->rgDllList[hprc->cDllList-10], 0, 10*sizeof(DLLLOAD_ITEM));
    } else {
        memset(&hprc->rgDllList[iDll], 0, sizeof(DLLLOAD_ITEM));
    }

    hprc->rgDllList[iDll].fValidDll = TRUE;
    hprc->rgDllList[iDll].offBaseOfImage = (OFFSET) ldd->lpBaseOfDll;
    hprc->rgDllList[iDll].cbImage = ldd->dwDebugInfoFileOffset;
    if (Alias) {
        _splitpath( Alias->ModuleName, NULL, NULL, fname, ext );
    } else {
        _splitpath( ldd->lpImageName, NULL, NULL, fname, ext );
    }
    hprc->rgDllList[iDll].szDllName = MHAlloc(_ftcslen(fname)+_ftcslen(ext)+4);
    sprintf( hprc->rgDllList[iDll].szDllName, _T("%s%s"), fname, ext );
    hprc->rgDllList[iDll].NumberOfSections = 0;
    hprc->rgDllList[iDll].Sections = NULL;
    hprc->rgDllList[iDll].sec = NULL;

    *szFoundName = 0;
    if (!((ULONG)ldd->lpBaseOfDll & 0x80000000)) {
        //
        // must be a usermode module
        //
        if (ReadImageInfo( hprc->rgDllList[iDll].szDllName,
                           szFoundName,
                           (LPSTR)KdOptions[KDO_SYMBOLPATH].value,
                           &ii )) {
            //
            // we found the debug info, so now save the sections
            // this data is used by processgetsectionscmd()
            //
            hprc->rgDllList[iDll].NumberOfSections = ii.NumberOfSections;
            hprc->rgDllList[iDll].sec = ii.Sections;
            hprc->rgDllList[iDll].offBaseOfImage = ii.BaseOfImage;
            hprc->rgDllList[iDll].cbImage = ii.SizeOfImage;
            ldd->hFile = (HANDLE)ii.CheckSum;
        }
    }

#ifdef TARGET_i386
    hprc->rgDllList[iDll].SegCs = (WORD) hthd->context.SegCs;
    hprc->rgDllList[iDll].SegDs = (WORD) hthd->context.SegDs;
#endif

    //
    //  Make up a record to send back from the name.
    //  Additionally send back:
    //          The file handle (if local)
    //          The load base of the dll
    //          The time and date stamp of the exe
    //          The checksum of the file
    //          ... and optionally the string "MP" to signal
    //              a multi-processor system to the symbol handler
    //
    *szModName = cModuleDemarcator;

    //
    // Send the name found by ReadImageInfo if it found an exe; if it
    // only found a dbg, send the default name.
    //
    _ftcscpy( szModName + 1,
            *szFoundName ? szFoundName : hprc->rgDllList[iDll].szDllName);
    lenSz=_ftcslen(szModName);
    szModName[lenSz] = cModuleDemarcator;
    sprintf( szModName+lenSz+1,_T("0x%08lX%c0x%08lX%c0x%08lX%c0x%08lX%c"),
             -1,                             cModuleDemarcator,    // timestamp
             ldd->hFile,                     cModuleDemarcator,    // checksum
             -1,                             cModuleDemarcator,
             hprc->rgDllList[iDll].offBaseOfImage,  cModuleDemarcator
           );

    if (Alias) {
        _ftcscat( szModName, Alias->Alias );
        lenSz = _ftcslen(szModName);
        szModName[lenSz] = cModuleDemarcator;
        szModName[lenSz+1] = 0;
        if (Alias->Special == 2) {
            // If it's a one-shot alias, nuke it.
            memset(Alias, 0, sizeof(MODULEALIAS));
        }
    }

    lenSz = _ftcslen(szModName);
    _ftcsupr(szModName);

    //
    // Allocate the packet which will be sent across to the EM.
    // The packet will consist of:
    //     The MDL structure                    sizeof(MDL) +
    //     The section description array        cobj*sizeof(OBJD) +
    //     The name of the DLL                  lenSz+1
    //
    *lpcbPacket = (WORD)(sizeof(MODULELOAD) + (lenSz+1));
    *lplpbPacket= (LPBYTE)(lpmdl=(LPMODULELOAD)MHAlloc(*lpcbPacket));
    ZeroMemory( lpmdl, *lpcbPacket );
    lpmdl->lpBaseOfDll = (LPVOID) hprc->rgDllList[iDll].offBaseOfImage;
    // mark the MDL packet as deferred:
    lpmdl->cobj = -1;
    lpmdl->mte = (WORD) -1;
#ifdef TARGET_i386
    lpmdl->CSSel    = (unsigned short)hthd->context.SegCs;
    lpmdl->DSSel    = (unsigned short)hthd->context.SegDs;
#else
    lpmdl->CSSel = lpmdl->DSSel = 0;
#endif

    lpmdl->fRealMode = 0;
    lpmdl->fFlatMode = 1;
    lpmdl->fOffset32 = 1;

    lpmdl->dwSizeOfDll = hprc->rgDllList[iDll].cbImage;

    //
    //  Copy the name of the dll to the end of the packet.
    //
    memcpy(((BYTE*)&lpmdl->rgobjd), szModName, lenSz+1);

    if (fDisconnected) {

        //
        // this will prevent the dm from sending a message up to
        // the shell.  the dm's data structures are setup just fine
        // so that when the debugger re-connects we can deliver the
        // mod loads correctly.
        //

        return FALSE;

    }

    return TRUE;
}                               /* LoadDll() */




#endif // KERNEL


/**     ConsumeThreadEventsAndNotifyEM
**
**              Description:
**                      Does exactly what it says it does
*/

void
ConsumeThreadEventsAndNotifyEM(
        DEBUG_EVENT* de,
        HTHDX hthd,
        DWORD wParam,
        LPVOID lparam
        )
{
    ConsumeAllThreadEvents(hthd, FALSE);
    NotifyEM(de, hthd, wParam, lparam);
}



/***    NotifyEM
**
**  Synopsis:
**
**  Entry:
**
**  Returns:
**
**  Description:
**      Given a debug event from the OS send the correct information
**      back to the debugger.
**
*/


void
NotifyEM(
    DEBUG_EVENT* de,
    HTHDX hthd,
    DWORD wparam,
    LPVOID lparam
    )
/*++

Routine Description:

    This is the interface for telling the EM about debug events.

    In general, de describes an event which the debugger needs
    to know about.  In some cases, a reply is needed.  In those
    cases this routine handles the reply and does the appropriate
    thing with the data from the reply.

Arguments:

    de      - Supplies debug event structure

    hthd    - Supplies thread that got the event

    wparam  - Supplies data specific to event

    lparam  - Supplies data specific to event

Return Value:

    None

--*/
{
    DWORD       eventCode = de->dwDebugEventCode;
    DWORD       subClass;
    RTP         rtp;
    RTP *       lprtp;
    WORD        cbPacket=0;
    LPBYTE      lpbPacket;
    LPVOID      toFree=(LPVOID)0;
    WORD        packetType = tlfDebugPacket;


    if (hthd) {
        rtp.hpid = hthd->hprc->hpid;
        rtp.htid = hthd->htid;
    } else if (hpidRoot == (HPID)INVALID) {
        return;
    } else {
        // cheat:
        rtp.hpid = hpidRoot;
        rtp.htid = NULL;
    }
    subClass = de->u.Exception.ExceptionRecord.ExceptionCode;

    switch(eventCode){

    case FUNC_EXIT_EVENT:
        if (hthd->fDisplayReturnValues) {
            cbPacket = sizeof(ADDR);
            rtp.dbc = dbcExitedFunction;
            lpbPacket = (LPBYTE) lparam;
                        assert(lpbPacket);
        } else {
            return;
        }
        break;

    case EXCEPTION_DEBUG_EVENT:
        if (subClass!=EXCEPTION_SINGLE_STEP){
            PEXCEPTION_RECORD pexr=&de->u.Exception.ExceptionRecord;
            DWORD cParam = pexr->NumberParameters;
            DWORD nBytes = sizeof(EPR)+sizeof(DWORD)*cParam;
            LPEPR lpepr  = MHAlloc(nBytes);

            toFree    = (LPVOID) lpepr;
            cbPacket  = (WORD)   nBytes;
            lpbPacket = (LPBYTE) lpepr;
#ifdef TARGET_i386
            lpepr->bpr.segCS = (SEGMENT)HthdReg(hthd,SegCs);
            lpepr->bpr.segSS = (SEGMENT)HthdReg(hthd,SegSs);
#endif
            lpepr->bpr.offEBP =  (DWORD)FRAME_POINTER(hthd);
            lpepr->bpr.offESP =  (DWORD)STACK_POINTER(hthd);
            lpepr->bpr.offEIP =  (DWORD)PC(hthd);
            lpepr->bpr.fFlat  =  hthd->fAddrIsFlat;
            lpepr->bpr.fOff32 =  hthd->fAddrOff32;
            lpepr->bpr.fReal  =  hthd->fAddrIsReal;

            lpepr->dwFirstChance  = de->u.Exception.dwFirstChance;
            lpepr->ExceptionCode    = pexr->ExceptionCode;
            lpepr->ExceptionFlags   = pexr->ExceptionFlags;
            lpepr->NumberParameters = cParam;
            for(;cParam;cParam--) {
                lpepr->ExceptionInformation[cParam-1]=
                  pexr->ExceptionInformation[cParam-1];
            }

            rtp.dbc = dbcException;
            break;
        };

                // Add 'foo returned' to Auto watch window
        if (hthd->fReturning) {
            hthd->fReturning = FALSE;
            NotifyEM(&FuncExitEvent, hthd, 0, &hthd->addrFrom );
        }



        // Fall through when subClass == EXCEPTION_SINGLE_STEP

    case BREAKPOINT_DEBUG_EVENT:
    case ENTRYPOINT_DEBUG_EVENT:
    case CHECK_BREAKPOINT_DEBUG_EVENT:
    case LOAD_COMPLETE_DEBUG_EVENT:
        {
            LPBPR lpbpr = MHAlloc ( sizeof ( BPR ) );
            toFree=lpbpr;
            cbPacket = sizeof ( BPR );
            lpbPacket = (LPBYTE) lpbpr;

#ifdef TARGET_i386
            lpbpr->segCS = (SEGMENT)HthdReg(hthd,SegCs);
            lpbpr->segSS = (SEGMENT)HthdReg(hthd,SegSs);
#endif

            lpbpr->offEBP =  (DWORD)FRAME_POINTER(hthd);
            lpbpr->offESP =  (DWORD)STACK_POINTER(hthd);
            lpbpr->offEIP =  (DWORD)PC(hthd);
            lpbpr->fFlat  =  hthd->fAddrIsFlat;
            lpbpr->fOff32 =  hthd->fAddrOff32;
            lpbpr->fReal  =  hthd->fAddrIsReal;
            lpbpr->dwNotify = (DWORD)lparam;

            if (eventCode==EXCEPTION_DEBUG_EVENT) {
                rtp.dbc = dbcStep;
                break;
            } else {              /* (the breakpoint case) */

                // REVIEW: It would be cleaner to setup something which got
                // called when the EE got consumed and turned this off. I
                // don't think the DM has this notion except for preset bps.
                // I think a general function for cleanup would be a good
                // idea here.

                if (eventCode != CHECK_BREAKPOINT_DEBUG_EVENT) {
                hthd->fReturning = FALSE;
                }

                if (eventCode == ENTRYPOINT_DEBUG_EVENT) {
                    rtp.dbc = dbcEntryPoint;
                } else if (eventCode == LOAD_COMPLETE_DEBUG_EVENT) {
                    rtp.dbc = dbcLoadComplete;
                } else if (eventCode == CHECK_BREAKPOINT_DEBUG_EVENT) {
                    rtp.dbc = dbcCheckBpt;
                    packetType = tlfRequest;
                } else {
                    rtp.dbc = dbcBpt;
                }

                /* NOTE: Ok try to follow this: If this was one
                 *   of our breakpoints then we have already
                 *   decremented the IP to point to the actual
                 *   INT3 instruction (0xCC), this is so we
                 *   can just replace the byte and continue
                 *   execution from that point on.
                 *
                 *   But if it was hard coded in by the user
                 *   then we can't do anything but execute the
                 *   NEXT instruction (because there is no
                 *   instruction "under" this INT3 instruction)
                 *   So the IP is left pointing at the NEXT
                 *   instruction. But we don't want the EM
                 *   think that we stopped at the instruction
                 *   after the INT3, so we need to decrement
                 *   offEIP so it's pointing at the hard-coded
                 *   INT3. Got it?
                 *
                 *   On an ENTRYPOINT_DEBUG_EVENT, the address is
                 *   already right, and lparam is ENTRY_BP.
                 */

                if (!lparam) {
                    lpbpr->offEIP = (UOFFSET)de->
                       u.Exception.ExceptionRecord.ExceptionAddress;
                }
            }

        }
        break;


    case CREATE_PROCESS_DEBUG_EVENT:
        /*
         *  A Create Process event has occured.  The following
         *  messages need to be sent back
         *
         *  dbceAssignPID: Associate our handle with the debugger
         *
         *  dbcModLoad: Inform the debugger of the module load for
         *  the main exe (this is done at the end of this routine)
         */
        {
            HPRCX  hprc = (HPRCX)lparam;

            /*
             * Has the debugger requested this process?
             * ie: has it already given us the HPID for this process?
             */
            if (hprc->hpid != (HPID)INVALID){

                lpbPacket = (LPBYTE)&(hprc->pid);
                cbPacket  = sizeof(hprc->pid);

                /* Want the hprc for the child NOT the DM */
                rtp.hpid  = hprc->hpid;

#if 0
#ifndef KERNEL
                //
                // hack for made-up image names:
                //
                iTemp1    = 0;
#endif
#endif
                rtp.dbc   = dbceAssignPID;
            }

            /*
             * The debugger doesn't know about this process yet,
             * request an HPID for this new process.
             */

            else {
                LPNPP lpnpp = MHAlloc(cbPacket=sizeof(NPP));

                toFree            = lpnpp;
                lpbPacket         = (LPBYTE)lpnpp;
                packetType        = tlfRequest;

                /*
                 * We must temporarily assign a valid HPID to this HPRC
                 * because OSDebug will try to de-reference it in the
                 * TL callback function
                 */
                rtp.hpid          = hpidRoot;
                lpnpp->pid        = hprc->pid;
                lpnpp->fReallyNew = TRUE;
                rtp.dbc           = dbcNewProc;
            }
        }
        break;

    case CREATE_THREAD_DEBUG_EVENT:
        {
            LPTCR lptcr = (LPTCR) MHAlloc(cbPacket=sizeof(TCR));
            toFree      = lpbPacket = (LPBYTE)lptcr;
            packetType  = tlfRequest;

            lptcr->tid     = hthd->tid;
            lptcr->uoffTEB = (UOFFSET) hthd->offTeb;

            rtp.hpid = hthd->hprc->hpid;
            rtp.htid = hthd->htid;
            rtp.dbc  = dbcCreateThread;
        }
        break;

    case EXIT_PROCESS_DEBUG_EVENT:
        cbPacket    = sizeof(DWORD);
        lpbPacket   = (LPBYTE) &(de->u.ExitProcess.dwExitCode);

        hthd->hprc->pstate |= ps_exited;
        rtp.hpid    = hthd->hprc->hpid;
        rtp.htid    = hthd->htid;
        rtp.dbc = dbcProcTerm;
        break;

    case EXIT_THREAD_DEBUG_EVENT:
        cbPacket    = sizeof(DWORD);
        lpbPacket   = (LPBYTE) &(de->u.ExitThread.dwExitCode);

        hthd->tstate        |= ts_dead; /* Mark thread as dead */
        hthd->hprc->pstate  |= ps_deadThread;
        rtp.dbc = dbcThreadTerm;
        break;

    case DESTROY_PROCESS_DEBUG_EVENT:
        DPRINT(3, (_T("DESTROY PROCESS\n")));
        hthd->hprc->pstate |= ps_destroyed;
        rtp.dbc = dbcDeleteProc;
        break;

    case DESTROY_THREAD_DEBUG_EVENT:
        /*
         *  Check if already destroyed
         */

        assert( (hthd->tstate & ts_destroyed) == 0 );

        DPRINT(3, (_T("DESTROY THREAD\n")));

        hthd->tstate |= ts_destroyed;
        cbPacket    = sizeof(DWORD);
//NOTENOTE a-kentf exit code is bogus here
        lpbPacket   = (LPBYTE) &(de->u.ExitThread.dwExitCode);
        rtp.dbc     = dbcDeleteThread;
        break;


    case LOAD_DLL_DEBUG_EVENT:

        packetType  = tlfRequest;
        rtp.dbc     = dbcModLoad;

#ifndef KERNEL
        //
        // this was changed for a deadlock problem in user mode.
        // if somebody gets ambitious, they can match the change
        // for the KD side.
        //
        lpbPacket   = lparam;
        cbPacket    = (USHORT)wparam;
#else
        if (!LoadDll(de, hthd, &cbPacket, &lpbPacket) || (cbPacket == 0)) {
            return;
        }
#endif
        //ValidateHeap();
        toFree      = (LPVOID)lpbPacket;
        break;

    case UNLOAD_DLL_DEBUG_EVENT:
        packetType  = tlfRequest;
        cbPacket  = sizeof(DWORD);
        lpbPacket = (LPBYTE) &(de->u.UnloadDll.lpBaseOfDll);

        rtp.dbc   = dbceModFree32;
        break;

    case OUTPUT_DEBUG_STRING_EVENT:
        {
            LPINFOAVAIL   lpinf;
            DWORD   cbR;

            rtp.dbc = dbcInfoAvail;

            cbPacket = (WORD)(sizeof(INFOAVAIL) +
                               de->u.DebugString.nDebugStringLength + 1);
            lpinf = (LPINFOAVAIL) lpbPacket = MHAlloc(cbPacket);
            toFree = lpbPacket;

            lpinf->fReply   = FALSE;
            lpinf->fUniCode = de->u.DebugString.fUnicode;

			memcpy(&lpinf->buffer[0], de->u.DebugString.lpDebugStringData,
				de->u.DebugString.nDebugStringLength);
            lpinf->buffer[de->u.DebugString.nDebugStringLength] = 0;
        }
        break;


    case INPUT_DEBUG_STRING_EVENT:
        {
            LPINFOAVAIL   lpinf;

            packetType = tlfRequest;
            rtp.dbc = dbcInfoReq;

            cbPacket =
                (WORD)(sizeof(INFOAVAIL) + de->u.DebugString.nDebugStringLength + 1);
            lpinf = (LPINFOAVAIL) lpbPacket = MHAlloc(cbPacket);
            toFree = lpbPacket;

            lpinf->fReply   = TRUE;
            lpinf->fUniCode = de->u.DebugString.fUnicode;

            memcpy( &lpinf->buffer[0],
                    de->u.DebugString.lpDebugStringData,
                    de->u.DebugString.nDebugStringLength);
            lpinf->buffer[ de->u.DebugString.nDebugStringLength ] = 0;
        }
        break;

    case RIP_EVENT:
#if 0
        {
            LPRIP_INFO  prip   = &de->u.RipInfo;
            DWORD       nBytes = sizeof(NT_RIP);
            LPNT_RIP    lprip  = MHAlloc(nBytes);

            toFree    = (LPVOID) lprip;
            cbPacket  = (WORD)   nBytes;
            lpbPacket = (LPBYTE) lprip;

#ifdef TARGET_i386
            lprip->bpr.segCS = (SEGMENT)hthd->context.SegCs;
            lprip->bpr.segSS = (SEGMENT)hthd->context.SegSs;
#endif
            lprip->bpr.offEBP =  (DWORD)FRAME_POINTER(hthd);
            lprip->bpr.offESP =  (DWORD)STACK_POINTER(hthd);
            lprip->bpr.offEIP =  (DWORD)PC(hthd);
            lprip->bpr.fFlat  =  hthd->fAddrIsFlat;
            lprip->bpr.fOff32 =  hthd->fAddrOff32;
            lprip->bpr.fReal  =  hthd->fAddrIsReal;

            lprip->ulErrorCode  = prip->dwError;
            lprip->ulErrorLevel = prip->dwType;

            rtp.dbc = dbcNtRip;
        }
#endif  // !OSDEBUG4
		
		/* If we hit a RIP, we need to bring up a msgbox for the assert,
		 * and then stop execution, which we do by faking an async stop */
		{
			LPBPR lpbpr;

			XboxRip((LPSTR)de->u.RipInfo.dwError);
			if(de->u.RipInfo.dwError)
				MHFree((PVOID)de->u.RipInfo.dwError);
			cbPacket = sizeof(BPR);
			lpbpr = MHAlloc(cbPacket);
			toFree = lpbpr;
			lpbPacket = (LPBYTE)lpbpr;
			
#ifdef TARGET_i386
            lpbpr->segCS = (SEGMENT)HthdReg(hthd,SegCs);
            lpbpr->segSS = (SEGMENT)HthdReg(hthd,SegSs);
#endif
            lpbpr->offEBP =  (DWORD)FRAME_POINTER(hthd);
            lpbpr->offESP =  (DWORD)STACK_POINTER(hthd);
            lpbpr->offEIP =  (DWORD)PC(hthd);
            lpbpr->fFlat  =  hthd->fAddrIsFlat;
            lpbpr->fOff32 =  hthd->fAddrOff32;
            lpbpr->fReal  =  hthd->fAddrIsReal;
			lpbpr->dwNotify = 0;

			rtp.dbc = dbcAsyncStop;
		}

        break;

    case ATTACH_DEADLOCK_DEBUG_EVENT:
        {
			// We get here if we've timed out waiting to attach to the 
			// debuggee. This applies both in the AttachToActive/JIT and 
			// normal debugging cases. In the latter case, starting a debuggee 
			// which can't find all DLLs and then waiting at the System dialog
			// while it times out.

			// A DebugPacket needs an XOSD and a string;
			struct _ATTACH_DEADLOCK_DEBUG_PACKET {
				XOSD	xosd;
				LPTSTR	lpstrMessage;
			} addp = { xosdAttachDeadlock, NULL };

			assert (!lparam && !wparam); // Should be 0; 
			assert (packetType == tlfDebugPacket);

            cbPacket    = sizeof(addp);
            lpbPacket   = (LPBYTE) &addp;
            rtp.dbc     = dbcError;
        }
        break;

    case MESSAGE_DEBUG_EVENT:
    case MESSAGE_SEND_DEBUG_EVENT:
        cbPacket = sizeof ( MSGI );
        lpbPacket = (LPBYTE) lparam;

        rtp.dbc = (eventCode == MESSAGE_DEBUG_EVENT) ? dbcMsgBpt :dbcSendMsgBpt;
        break;

    default:
        DPRINT(1, (_T("Error, unknown event\n\r")));
        SetDebugEventThreadState (hthd->hprc, ts_running);
        AddQueue( QT_CONTINUE_DEBUG_EVENT,
                  hthd->hprc->pid,
                  hthd->tid,
                  DBG_CONTINUE,
                  0);
        return;
    }


    DPRINT(3, (_T("Notify the debugger: dbc=%x, hpid=%x, htid=%x, cbpacket=%d "),
                  rtp.dbc, rtp.hpid, rtp.htid, cbPacket+FIELD_OFFSET(RTP, rgbVar)));

    if (!(rtp.cb=cbPacket)) {
        DmTlFunc(packetType, rtp.hpid, FIELD_OFFSET(RTP, rgbVar), (LONG)(LPV) &rtp);
    }
    else {
        lprtp = (LPRTP)MHAlloc(FIELD_OFFSET(RTP, rgbVar)+cbPacket);
        memcpy(lprtp, &rtp, FIELD_OFFSET(RTP, rgbVar));
        memcpy(lprtp->rgbVar, lpbPacket, cbPacket);

        DmTlFunc(packetType, rtp.hpid,(WORD)(FIELD_OFFSET(RTP, rgbVar)+cbPacket),
                 (LONG)(LPV) lprtp);

        MHFree(lprtp);
    }

    if (toFree) {
        MHFree(toFree);
    }

    DPRINT(3, (_T("\n")));

    switch(eventCode){

      case CREATE_THREAD_DEBUG_EVENT:
        if (packetType == tlfRequest) {
            hthd->htid = *((HTID *) abEMReplyBuf);
        }
        SetEvent(hthd->hprc->hEventCreateThread);
        break;

      case CREATE_PROCESS_DEBUG_EVENT:
        if (packetType == tlfRequest) {
            ((HPRCX)lparam)->hpid = *((HPID *) abEMReplyBuf);
        } else {
            XOSD xosd = xosdNone;
            DmTlFunc( tlfReply,
                      ((HPRCX)lparam)->hpid,
                      sizeof(XOSD),
                      (LONG)(LPV) &xosd);
        }

        SetEvent(hEventCreateProcess);

        break;

      case OUTPUT_DEBUG_STRING_EVENT:
        // just here to synchronize
        break;

      case INPUT_DEBUG_STRING_EVENT:
        de->u.DebugString.nDebugStringLength = _ftcslen(abEMReplyBuf) + 1;
        memcpy(de->u.DebugString.lpDebugStringData,
               abEMReplyBuf,
               de->u.DebugString.nDebugStringLength);
        break;

#ifdef KERNEL
      // as noted above, synchronization is done differently in
      // the user and KD versions:
      case LOAD_DLL_DEBUG_EVENT:
        if (_ftcsicmp( de->u.LoadDll.lpImageName, KERNEL_IMAGE_NAME ) == 0) {
          GetKernelSymbolAddresses();
        }
        break;
#endif

      default:
        break;
    }

    //ValidateHeap();
    return;
}                               /* NotifyEM() */

/*** ACTIONTERMINATEPROCESS
 *
 * PURPOSE:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * EXCEPTIONS:
 *
 * IMPLEMENTATION:
 *
 ****************************************************************************/

VOID
ActionTerminateProcess (
    DEBUG_EVENT   * pde,
    HTHDX           hthd,
    DWORD           unused,
    LPVOID          lpv
    )
{
    ConsumeAllProcessEvents ( (HPRCX)lpv, TRUE );

    AddQueue( QT_CONTINUE_DEBUG_EVENT,
              hthd->hprc->pid,
              hthd->tid,
              DBG_CONTINUE,
              0);

    hthd->hprc->pstate |= ps_dead;

} /* ACTIONTERMINATEPROCESS */

/*** ACTIONPROCESSLOADFAILED
 *
 * PURPOSE:
 *      When we issued the CreateProcess, that succeeded; but then we
 *      got an exception with the EXCEPTION_NONCONTINUABLE flag set,
 *      which means after letting the debuggee continue we would get
 *      a process-termination notification.  (For example, if the
 *      system can't find a DLL that the process needs, it sends a
 *      STATUS_DLL_NOT_FOUND notification with this flag set.)  So
 *      we've said ok, and now the system has sent us a procterm
 *      notification for it.
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * EXCEPTIONS:
 *
 * IMPLEMENTATION:
 *
 ****************************************************************************/

VOID
ActionProcessLoadFailed (
    DEBUG_EVENT *pde,
    HTHDX hthd,
    DWORD unused,
    LPVOID lpv
    )
{
    /* Call another action proc & do what it does */
    ActionTerminateProcess(pde, hthd, 0, hthd->hprc);
}

/*** ACTIONNONCONTINUABLEEXCEPTION
 *
 * PURPOSE:
 *      When we issued the CreateProcess, that succeeded; but then we
 *      got an exception with the EXCEPTION_NONCONTINUABLE flag set,
 *      which means after letting the debuggee continue we would get
 *      a process-termination notification.  (For example, if the
 *      system can't find a DLL that the process needs, it sends a
 *      STATUS_DLL_NOT_FOUND notification with this flag set.)  So
 *      we've said ok, and now the system has sent us a last-chance
 *      notification for it.
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * EXCEPTIONS:
 *
 * IMPLEMENTATION:
 *
 ****************************************************************************/

VOID
ActionNoncontinuableException (
    DEBUG_EVENT *pde,
    HTHDX hthd,
    DWORD unused,
    LPVOID lpv
    )
{
    // [cuda#5673 7/8/93 mikemo]  This is very strange -- it seems that
    // in some situations NT has now sent us a SECOND first-chance
    // notification, even though it already sent us one.  I don't know
    // why this is, but if this happens, I register another event expecting
    // a last-chance notification, instead of expecting exit-process.
    if (pde->u.Exception.dwFirstChance) {
        RegisterExpectedEvent(hthd->hprc,
                              hthd,
                              EXCEPTION_DEBUG_EVENT,
                              (DWORD)pde->u.Exception.ExceptionRecord.ExceptionCode,
                              DONT_NOTIFY,
                              ActionNoncontinuableException,
                              FALSE,
                              NULL
                              );
    } else {
        RegisterExpectedEvent(hthd->hprc, hthd, EXIT_PROCESS_DEBUG_EVENT,
            NO_SUBCLASS, DONT_NOTIFY, ActionProcessLoadFailed, FALSE,
            NULL);
    }

    AddQueue( QT_CONTINUE_DEBUG_EVENT,
              hthd->hprc->pid,
              hthd->tid,
              (DWORD)DBG_EXCEPTION_NOT_HANDLED,
              0);
}


void
ProcessExceptionEvent(
    DEBUG_EVENT* de,
    HTHDX hthd
    )
{
    DWORD       subclass = de->u.Exception.ExceptionRecord.ExceptionCode;
    DWORD       firstChance = de->u.Exception.dwFirstChance;
    PBREAKPOINT bp=NULL;

    //
    //  If the thread is in a pre-start state we failed in the
    //  program loader, probably because a link couldn't be resolved.
    //
    if ( hthd->hprc->pstate & ps_preStart ) {
        BOOL  fNonContinuable;
        DPRINT(1, (_T("Exception during init\n")));

        fNonContinuable = !!(de->u.Exception.ExceptionRecord.ExceptionFlags &
            EXCEPTION_NONCONTINUABLE);
        if (fNonContinuable) {
            DWORD dwRet;
            LPTSTR lszError = NULL;

            hthd->hprc->pstate &= ~ps_preStart;
            hthd->tstate |= ts_stopped;
            /*
            ** The debugger is expecting either an OS-specific error code
            ** or an XOSD (XOSDs are negative).  We don't have an OS
            ** error code (all we have is an exception number).  The debugger
            ** treats STATUS_DLL_NOT_FOUND as an error code, but doesn't
            ** recognize any other exception codes, so for other exception
            ** codes, just return xosdLoadChild.
            */
            if (subclass == STATUS_DLL_NOT_FOUND) {
                TCHAR rgch[256]; // same as magic number in em\errors.c
#ifdef XBOX // LOCBUG
				strcpy(lszError = rgch, "dll not found");
#else
                if (!LoadString(hInstance, IDS_STATUS_DLL_NOT_FOUND, rgch, _tsizeof(rgch))) {
                    assert(FALSE);
                } else {
                    lszError = rgch;
                }
#endif
                dwRet = xosdFileNotFound;
            }
            else {
                dwRet = xosdGeneral;
            }
            SendNTError(hthd->hprc, dwRet, lszError);

            /*
            ** Actually the CreateProcess succeeded, and then we
            ** received a notification with an exception; so we need
            ** to keep calling ContinueDebugEvent until the process
            ** is really dead.
            */
            hthd->hprc->pstate |= ps_exited;
            ConsumeAllProcessEvents(hthd->hprc, TRUE);
            RegisterExpectedEvent(hthd->hprc,
                                  hthd,
                                  EXCEPTION_DEBUG_EVENT,
                                  subclass,
                                  DONT_NOTIFY,
                                  ActionNoncontinuableException,
                                  FALSE,
                                  NULL);
            AddQueue( QT_CONTINUE_DEBUG_EVENT,
                      hthd->hprc->pid,
                      hthd->tid,
                      (DWORD)DBG_EXCEPTION_NOT_HANDLED,
                      0);
            return;
        }
        //
        // since we will probably never see the expected BP,
        // clear it out, and clear the prestart flag on the
        // thread, then go ahead and deliver the exception
        // to the shell.
        //
        ConsumeAllProcessEvents(hthd->hprc, TRUE);
        hthd->hprc->pstate &= ~ps_preStart;
        hthd->tstate |= ts_stopped;
    }


    switch(subclass) {

    case (DWORD)EXCEPTION_SINGLE_STEP:
#if defined(DOLPHIN)
#if defined(NO_TRACE_BIT)
        //
        // BUGBUG kentf This next bit of code is a rather overcomplicated
        // BUGBUG   way of creating a deadlock.  The same task could be
        // BUGBUG   accomplished with considerably less code.
        // BUGBUG
        // BUGBUG The problem which this intends to solve is as follows:
        // BUGBUG   When a thread is stepped using a code breakpoint, it
        // BUGBUG   may happen that another thread will execute the
        // BUGBUG   breakpoint.  Since that thread is not intended to stop
        // BUGBUG   there, we must replace the instruction and allow that
        // BUGBUG   thread to continue.  During the time that the "wrong"
        // BUGBUG   thread is executing the instruction, it may happen that
        // BUGBUG   the "right" thread will hit that instruction, and run
        // BUGBUG   through it, thereby missing the breakpoint.
        // BUGBUG
        // BUGBUG   The following code attempts to solve the problem by
        // BUGBUG   suspending all of the threads in the process while
        // BUGBUG   stepping the "wrong" thread off of the breakpoint.
        // BUGBUG
        // BUGBUG   This algorithm assumes that the "wrong" thread will be
        // BUGBUG   able to complete the instruction while all of the other
        // BUGBUG   threads are suspended.  This assumption is easily
        // BUGBUG   disproven.  For example, try this on a call to
        // BUGBUG   WaitForSingleObject.
        // BUGBUG
        // BUGBUG   Secondly, this code assumes that the next debug event
        // BUGBUG   will be the exception from the "wrong" thread completing
        // BUGBUG   the new step.  This is a absolutely not the case, and
        // BUGBUG   the code below will fail in ways much more disastrous
        // BUGBUG   than running through a step.
        // BUGBUG
        // BUGBUG
        // BUGBUG   Note 1:  This particular condition cannot arise on
        // BUGBUG   a processor with a step flag, since the step flag
        // BUGBUG   cannot cause a single step exception on the
        // BUGBUG   wrong thread (except as a result of an OS bug).
        //
        {
            /* This single step was intended for another thread */
            HPRCX hprc = hthd->hprc;
            HTHDX hthdCurr = NULL;
            BREAKPOINT* tmpBP = NULL;
            UOFF32 NextOffset;
            ADDR addr;
            BP_UNIT opcode = BP_OPCODE;
            DWORD i;

            AddrFromHthdx(&addr, hthd);
            if (bp = AtBP(hthd)) {
                RestoreInstrBP(hthd, bp);
            }
            /* Suspend all threads except this one */
            for (hthdCurr = hprc->hthdChild; hthdCurr != NULL; hthdCurr = hthdCurr->nextSibling) {
                if (hthdCurr != hthd) {
                    if (SuspendThread(hthdCurr->rwHand) == -1L) {
                        ; // Internal error;
                    }
                }
            }
            /* Set BP at next offset so we can get off this SS */
            NextOffset = GetNextOffset( hthd, FALSE);
            if (NextOffset != 0) {
                ADDR tmpAddr;
                AddrInit(&tmpAddr, 0, 0, NextOffset, TRUE, TRUE, FALSE, FALSE);
                tmpBP = SetBP( hprc, hthd, bptpExec, bpnsStop, &tmpAddr, (HPID) INVALID );
                assert(tmpBP);
            } else {
                assert(FALSE);
            }
            /* Get off SS */
            VERIFY(ContinueDebugEvent(hprc->pid, hthd->tid, DBG_CONTINUE));
            if (WaitForDebugEvent(de, INFINITE)) {
                assert(de->dwDebugEventCode == EXCEPTION_DEBUG_EVENT);
            } else {
                //
                // BUGBUG kentf This is certain to occur.
                //
                assert(FALSE);
            }
            hthd->context.ContextFlags = CONTEXT_FULL | CONTEXT_FLOATING_POINT;
            VERIFY(GetThreadContext(hthd->rwHand, &hthd->context));
            hthd->fContextDirty = FALSE;
            hthd->fIsCallDone = FALSE;
            RemoveBP(tmpBP);
            /* Resume all threads except this one */
            for (hthdCurr = hprc->hthdChild; hthdCurr != NULL; hthdCurr = hthdCurr->nextSibling) {
                if (hthdCurr != hthd) {
                    if (ResumeThread(hthdCurr->rwHand) == -1L) {
                        assert(FALSE); // Internal error;
                    }
                }
            }

            if (bp == NULL) {
                bp = SetBP(hprc, hthd, bptpExec, bpnsStop, &addr, FALSE);
            } else {
            /* Put back SS */
                AddrWriteMemory(hprc, hthd, &bp->addr, (LPBYTE)&opcode, BP_SIZE, &i);
                bp->instances++;
            }
            bp->isStep++;
            SetDebugEventThreadState (hthd->hprc, ts_running);
            AddQueue( QT_CONTINUE_DEBUG_EVENT,
                      hthd->hprc->pid,
                      hthd->tid,
                      (DWORD)DBG_CONTINUE,
                      0);
            return;
        }
#endif // NO_TRACE_BIT
#endif // DOLPHIN
        break;


    default:


        /*
         *  The user can define a set of exceptions for which we do not do
         *      notify the shell on a first chance occurance.
         */

#if defined (TARGET_i386) && !defined (KERNEL)

        //
        // If we are stepping (=> trace flag set) and we are attempting
        // to step on a statement that raising an exception that gets
        // caught somewhere else, then having the trace flag set
        // prevents us from ending up where the NLG thing takes us.
        // Turn it off & assume that we will get a second chance.
        // This is wrong for disasm stepping and may be otherwise
        // problematic.
        //

        HthdReg(hthd,EFlags) &= ~TF_BIT_MASK;
        hthd->fContextDirty = TRUE;
#endif

        if (!firstChance) {

            DPRINT(3, (_T("2nd Chance Exception %08lx.\n"),subclass));
            hthd->tstate |= ts_second;

        } else {

            hthd->tstate |= ts_first;

            switch (ExceptionAction(hthd->hprc,subclass)) {

              case efdNotify:

                NotifyEM(de,hthd,0,(LPVOID)bp);
                // fall through to ignore case

              case efdIgnore:

                DPRINT(3, (_T("Ignoring Exception %08lx.\n"),subclass));
                SetDebugEventThreadState (hthd->hprc, ts_running);
                AddQueue( QT_CONTINUE_DEBUG_EVENT,
                          hthd->hprc->pid,
                          hthd->tid,
                          (DWORD)DBG_EXCEPTION_NOT_HANDLED,
                          0);
                return;

              case efdStop:
              case efdCommand:
                break;
            }
        }

        break;
    }

    // Cleanup ...

    hthd->fReturning = FALSE;
    ConsumeAllThreadEvents(hthd, FALSE);

    //ExprBPStop(hthd->hprc, hthd);
    NotifyEM(de,hthd,0,(LPVOID)bp);
}                                   /* ProcessExceptionEvent() */


void
ProcessRipEvent(
    DEBUG_EVENT   * de,
    HTHDX           hthd
    )
{
    if (hthd) {
        hthd->tstate |= ts_rip;
    }
    NotifyEM( de, hthd, 0,NULL );
}                               /* ProcessRipEvent() */


void
ProcessBreakpointEvent(
    DEBUG_EVENT   * pde,
    HTHDX           hthd
    )
{
	BOOL		fBreak = FALSE;
    MSGI		msgi;
	LPVOID		lpv = 0;
    PBREAKPOINT pbp = (BREAKPOINT*)pde->u.Exception.ExceptionRecord.ExceptionCode;


    DPRINT(1, (_T("Hit a breakpoint -- ")));

    if (!pbp) {

        DPRINT(1, (_T("[Embedded BP]\n")));
		fBreak = TRUE;
		lpv = pbp;

    } else if (!pbp->hthd || pbp->hthd == hthd) {

        DPRINT(1, (_T("[One of our own BP's.]\n")));
        if (pbp->bpType == bptpMessage) {

            if (!GetWndProcMessage (hthd, &msgi.dwMessage)) {
                assert (FALSE);
			}

            msgi.dwMask = 0;        // mask is currently unused in the dm
            msgi.addr = pbp->addr;
			pde->dwDebugEventCode = MESSAGE_DEBUG_EVENT;

			lpv = &msgi;
			fBreak = TRUE;

        } else if (CheckDataBP (hthd, pbp)) {

            if ((pbp->bpNotify == bpnsStop) ||
                (pbp->bpNotify == bpnsCheck && CheckBpt(hthd, pbp))) {
				
				fBreak = TRUE;
				lpv = pbp;
				
            } else {

                if (pbp->bpNotify == bpnsContinue) {
                    // NotifyEM(...);
                }
				fBreak = FALSE;
            }
        } else {
			fBreak = FALSE;
        }

    } else {

        DPRINT(1, (_T("[BP for another thread]\n")));

        //
        // data bp should never hit on the wrong thread
        //
        assert (pbp->hWalk == NULL);

		fBreak = FALSE;

    }

	if (hthd->hprc->pstate & ps_breaking) {

		if (pbp) {
			SetBPFlag (hthd, pbp);
		}
		_asm int 3
		//ActionAsyncStop (NULL, hthd, 0, NULL);

	} else if (fBreak) {

		if (pbp) {
			SetBPFlag (hthd, pbp);
		}
		ConsumeAllThreadEvents (hthd, FALSE);
		NotifyEM (pde, hthd, 0, lpv);

	} else {

		ContinueFromBP (hthd, pbp);
	}
}

VOID
ContinueFromBP(
    HTHDX hthd,
    PBREAKPOINT pbp
    )
{
    void MethodContinueSS(DEBUG_EVENT*, HTHDX, DWORD, METHOD*);
    METHOD *ContinueSSMethod;
    if (!AtBP(hthd)) {
        ExprBPContinue( hthd->hprc, hthd );
        SetDebugEventThreadState (hthd->hprc, ts_running);
        AddQueue( QT_CONTINUE_DEBUG_EVENT,
                  hthd->hprc->pid,
                  hthd->tid,
                  DBG_CONTINUE,
                  0);
    } else {
        ContinueSSMethod = (METHOD*)MHAlloc(sizeof(METHOD));
        ContinueSSMethod->notifyFunction = (ACVECTOR)MethodContinueSS;
        ContinueSSMethod->lparam         = ContinueSSMethod;
        ContinueSSMethod->lparam2        = pbp;
        if(pbp && pbp != EMBEDDED_BP)
            ++pbp->cthd;

        RestoreInstrBP(hthd, pbp);
        SingleStep(hthd, ContinueSSMethod, FALSE, FALSE);
    }
}


VOID
StuffHthdx(
    DEBUG_EVENT *de,
    HPRCX hprc,
    HTHDX hthd
    )
/*++

Routine Description:

    Common code for CreateProcess and CreateThread events.  Stuff
    the hthd with default values, add the new structure to the process'
    chain.  Critical section csThreadProcList must be held when calling
    this routine.

Arguments:

    de - Supplies debug event

    hprc - Supplies process structure

    hthd - Supplies empty thread struct, returns full one.

Return Value:


--*/
{
    hthd->next          = thdList->next;
    thdList->next       = hthd;

    hthd->nextSibling   = hprc->hthdChild;
    hprc->hthdChild     = (LPVOID)hthd;

    hthd->hprc          = hprc;
    hthd->htid          = 0;
    hthd->atBP          = (BREAKPOINT*)0;
    hthd->tstate        = ts_stopped;

    hthd->fAddrIsReal   = FALSE;
    hthd->fAddrIsFlat   = TRUE;
    hthd->fAddrOff32    = TRUE;
    hthd->fWowEvent     = FALSE;
    hthd->fStopOnNLG    = FALSE;
    hthd->fReturning    = FALSE;
    hthd->fDisplayReturnValues = TRUE;
#ifdef KERNEL
    hthd->fContextStale = TRUE;
#endif
#if 0
#ifndef KERNEL
    hthd->pcs           = NULL;
    hthd->poleret       = NULL;
#endif
#endif
    InitializeListHead(&hthd->WalkList);
    hthd->WalkData = NULL;

    hthd->tid           = de->dwThreadId;
    if (de->dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT) {
        hthd->rwHand        = de->u.CreateProcessInfo.hThread;
        hthd->offTeb        = (OFFSET) de->u.CreateProcessInfo.lpThreadLocalBase;
        hthd->lpStartAddress= (LPVOID) de->u.CreateProcessInfo.lpStartAddress;
    } else {
        hthd->rwHand        = de->u.CreateThread.hThread;
        hthd->offTeb        = (OFFSET) de->u.CreateThread.lpThreadLocalBase;
        hthd->lpStartAddress= (LPVOID) de->u.CreateThread.lpStartAddress;
    }
	hthd->szThreadName[0] = 0;
	//hthd->fSoftBroken = FALSE;
}


void
ProcessCreateProcessEvent(
    DEBUG_EVENT   * pde,
    HTHDX           hthd
    )
/*++

Routine Description:

    This routine does the processing needed for a create process
    event from the OS.  We need to do the following:

      - Set up our internal structures for the newly created thread
        and process
      - Get notifications back to the debugger

Arguments:

    pde    - Supplies pointer to the DEBUG_EVENT structure from the OS

    hthd   - Supplies thread descriptor that thread event occurred on

Return Value:

    none

--*/
{
    DEBUG_EVENT                 de2;
    CREATE_PROCESS_DEBUG_INFO  *pcpd = &pde->u.CreateProcessInfo;
    HPRCX                       hprc;
    LPBYTE                      lpbPacket;
    WORD                        cbPacket;

    DEBUG_PRINT(_T("ProcessCreateProcessEvent\r\n"));

    ResetEvent(hEventCreateProcess);

    hprc = InitProcess((HPID)INVALID);

    //
    // Stuff the process structure
    //

    if (fUseRoot) {
        hprc->pstate           |= ps_root;
        hprc->hpid              = hpidRoot;
        fUseRoot                = FALSE;
        hprc->f16bit = FALSE;
    }

    hprc->pid     = pde->dwProcessId;
	// mark the process as connecting so we can sort things out
    hprc->pstate  = ps_preStart | ps_connect;

#ifndef KERNEL
    hprc->lastTidDebugEvent = pde->dwThreadId;
#endif

    ResetEvent(hprc->hEventCreateThread);


    //
    // Create the first thread structure for this app
    //

    hthd = (HTHDX)MHAlloc(sizeof(HTHDXSTRUCT));
    memset(hthd, 0, sizeof(*hthd));

    EnterCriticalSection(&csThreadProcList);

    StuffHthdx(pde, hprc, hthd);

    LeaveCriticalSection(&csThreadProcList);


#ifndef KERNEL
#if defined(TARGET_i386)
    hthd->ctx.ContextFlags = CONTEXT_FULL;
    DbgGetThreadContext( hthd, &hthd->ctx );
    hprc->segCode = (SEGMENT) HthdReg(hthd,SegCs);
#endif  // i386
#endif  // !KERNEL


#if 0
#ifndef KERNEL
    //
    // try to create a handle with more permissions
    //
    if (!CrashDump) {
        hprc->rwHand = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pde->dwProcessId);
        if (!hprc->rwHand) {
            hprc->rwHand = pcpd->hProcess;
        }
    }
#endif // KERNEL
#endif

    /*
     * There is going to be a breakpoint to announce that the
     * process is loaded and runnable.
     */
    nWaitingForLdrBreakpoint++;
    if (pcpd->lpStartAddress == NULL) {
        // in an attach, the BP will be in another thread.
        RegisterExpectedEvent( hprc,
                               (HTHDX)NULL,
                               BREAKPOINT_DEBUG_EVENT,
                               NO_SUBCLASS,
                               DONT_NOTIFY,
                               (ACVECTOR)ActionDebugActiveReady,
                               FALSE,
                               hprc);
#ifndef KERNEL
#if defined(INTERNAL)
        // don't ever let it defer name resolutions
        hprc->fNameRequired = TRUE;
#endif
#endif

    } else {
        // On a real start, the BP will be in the first thread.
        RegisterExpectedEvent( hthd->hprc,
                               hthd,
                               BREAKPOINT_DEBUG_EVENT,
                               NO_SUBCLASS,
                               DONT_NOTIFY,
                               (ACVECTOR)ActionDebugNewReady,
                               FALSE,
                               hprc);
    }

    /*
     * Notify the EM of this newly created process.
     * If not the root proc, an hpid will be created and added
     * to the hprc by the em.
     */
    NotifyEM(pde, hthd, 0,hprc);

#ifndef KERNEL
    /*
     *  We also need to drop out a module load notification
     *  on this exe.
     */

    de2.dwDebugEventCode        = LOAD_DLL_DEBUG_EVENT;
    de2.dwProcessId             = pde->dwProcessId;
    de2.dwThreadId              = pde->dwThreadId;
    de2.u.LoadDll.hFile         = pde->u.CreateProcessInfo.hFile;
    de2.u.LoadDll.lpBaseOfDll   = pde->u.CreateProcessInfo.lpBaseOfImage;
    de2.u.LoadDll.lpImageName   = pde->u.CreateProcessInfo.lpImageName;
    de2.u.LoadDll.fUnicode      = pde->u.CreateProcessInfo.fUnicode;

    if (LoadDll(&de2, hthd, &cbPacket, &lpbPacket) && (cbPacket != 0)) {
        LeaveCriticalSection(&csProcessDebugEvent);
        NotifyEM(&de2, hthd, cbPacket, lpbPacket);
        EnterCriticalSection(&csProcessDebugEvent);
    }
#endif // !KERNEL
	// until we get loaddll working, let's try this
	SetDebugEventThreadState(hprc, ts_running);

    /*
     * Fake up a thread creation notification.
     */
    pde->dwDebugEventCode = CREATE_THREAD_DEBUG_EVENT;
    NotifyEM(pde, hthd, 0, hprc);

    /*
     * Dont let the new process run:  the shell will say Go()
     * after receiving a CreateThread event.
     */

}                              /* ProcessCreateProcessEvent() */


void
ProcessCreateThreadEvent(
    DEBUG_EVENT    *pde,
    HTHDX           creatorHthd
    )
{
    CREATE_THREAD_DEBUG_INFO  * ctd = &pde->u.CreateThread;
    HTHDX                       hthd;
    HTHDX                       hthdT;
    HPRCX                       hprc;
    CONTEXT                     context;
#if defined(KERNEL) && defined(HAS_DEBUG_REGS)
    KSPECIAL_REGISTERS          ksr;
#endif // KERNEL && i386

    Unreferenced(creatorHthd);

    DPRINT(3, (_T("\n***CREATE THREAD EVENT\n")));

    //
    // Determine the hprc
    //
    hprc= HPRCFromPID(pde->dwProcessId);

    ResetEvent(hprc->hEventCreateThread);

#if 0
    if (ctd->hThread == NULL) {
        DPRINT(1, (_T("BAD HANDLE! BAD HANDLE!(%08lx)\n"), ctd->hThread));
        AddQueue( QT_CONTINUE_DEBUG_EVENT, pde->dwProcessId, pde->dwThreadId, DBG_CONTINUE, 0);
        return;
    }
#endif

    if (!hprc) {
        DPRINT(1, (_T("BAD PID! BAD PID!\n")));
        AddQueue( QT_CONTINUE_DEBUG_EVENT, pde->dwProcessId, pde->dwThreadId, DBG_CONTINUE, 0);
        return;
    }

    //
    // Create the thread structure
    //
    hthd = (HTHDX)MHAlloc(sizeof(HTHDXSTRUCT));
    memset( hthd, 0, sizeof(*hthd));

    //
    // Stuff the structure
    //

    EnterCriticalSection(&csThreadProcList);

    StuffHthdx(pde, hprc, hthd);

    LeaveCriticalSection(&csThreadProcList);

#ifndef KERNEL
    hprc->lastTidDebugEvent = pde->dwThreadId;
#endif

    //
    //  Let the expression breakpoint manager that a new thread
    //  has been created.
    //

    ExprBPCreateThread( hprc, hthd );

    //
    // initialize cache entries
    //

    context.ContextFlags = CONTEXT_FULL;
    DbgGetThreadContext( hthd, &context );

#if defined(KERNEL) && defined(HAS_DEBUG_REGS)
    GetExtendedContext( hthd, &ksr );
#endif  // KERNEL && i386

    //
    //  Notify the EM of this new thread
    //

    if (fDisconnected) {
        SetDebugEventThreadState (hthd->hprc, ts_running);
        AddQueue (QT_CONTINUE_DEBUG_EVENT,
                  hthd->hprc->pid,
                  hthd->tid,
                  DBG_CONTINUE,
                  0);

    } else {
        NotifyEM(pde, hthd, 0, hprc);
    }

    return;
}


#ifndef KERNEL
VOID
UnloadAllModules(
    HPRCX           hprc,
    HTHDX           hthd
    )
{
    DEBUG_EVENT     de;
    DWORD           i;


    for (i=0; i<(DWORD)hprc->cDllList; i++) {
        if ((hprc->rgDllList[i].fValidDll) &&  (!hprc->rgDllList[i].fWow)) {
            de.dwDebugEventCode = UNLOAD_DLL_DEBUG_EVENT;
            de.dwProcessId = hprc->pid;
            de.dwThreadId = hthd ? hthd->tid : 0;
            de.u.UnloadDll.lpBaseOfDll = (LPVOID)hprc->rgDllList[i].offBaseOfImage;
            NotifyEM( &de, hthd, 0, NULL );
            DestroyDllLoadItem( &hprc->rgDllList[i] );
        }
    }

    return;
}
#endif // !KERNEL


void
ProcessExitProcessEvent(
    DEBUG_EVENT* pde,
    HTHDX hthd
    )
{
    HPRCX               hprc;
    XOSD                xosd;
    HTHDX               hthdT;
	DWORD				flag;

    /*
     * do all exit thread handling:
     *
     * If thread was created during/after the
     * beginning of termination processing, we didn't
     * pick it up, so don't try to destroy it.
     */

    if (!hthd) {
        hprc = HPRCFromPID (pde->dwProcessId);
    } else {
        hprc = hthd->hprc;
    }

	flag = InterlockedSetFlag (&hprc->fExited);

	if (hthd) {
		hthd->tstate |= ts_dead;
		hthd->dwExitCode = pde->u.ExitProcess.dwExitCode;
	}

    ConsumeAllProcessEvents (hprc, TRUE);

	/*
     * If process hasn't initialized yet, we were expecting
     * a breakpoint to notify us that all the DLLs are here.
     * We didn't get that yet, so reply here.
     */

	if (hprc->pstate & ps_preStart) {
		assert (FALSE); // MATH: review
        xosd = xosdUnknown;
        DmTlFunc( tlfReply, hprc->hpid, sizeof(XOSD), (LONG)(LPV) &xosd);
    }

	for (hthdT = hprc->hthdChild; hthdT; hthdT = hthdT->nextSibling) {
		if ( !(hthdT->tstate & ts_dead) ) {
			hthdT->tstate |= ts_dead;
			hthdT->tstate &= ~ts_stopped;
		}
	}

	hprc->pstate |= ps_dead;
	hprc->dwExitCode = pde->u.ExitProcess.dwExitCode;

	RemoveFiberList (hprc);


	if (!(hprc->pstate & ps_killed)) {

		assert (hprc->hExitFailed == NULL);
		UnloadAllModules (hprc, hthd ? hthd : hprc->hthdChild);
		RemoveAllHprcBP (hprc);

        assert (hthd);
        pde->dwDebugEventCode = EXIT_THREAD_DEBUG_EVENT;
        pde->u.ExitThread.dwExitCode = hprc->dwExitCode;
        NotifyEM (pde, hthd, 0, (LPVOID)0);

	} else {
	
		/*
         * If ProcessTerminateProcessCmd() killed this,
         * silently continue the event and release the semaphore.
         *
         * Don't notify the EM of anything; ProcessUnloadCmd()
         * will do that for any undestroyed threads.
         */

		if (hprc->hExitFailed) {
			VERIFY (RemoveTimerEvent (hprc, hprc->hExitFailed));
			hprc->hExitFailed = NULL;
		}

		assert (hprc->pstate & ps_killed);

#if 0
		AddQueue( QT_CONTINUE_DEBUG_EVENT,
                  pde->dwProcessId,
                  pde->dwThreadId,
                  DBG_CONTINUE,
                  0);
#endif

		if (flag == FLAG_SET) {
			ProcessUnloadCmd (hprc, NULL, NULL);
		}
	}

    //DMSqlTerminate (hprc);
}


void
ProcessExitThreadEvent(
    DEBUG_EVENT* pde,
    HTHDX hthd
    )
{
    HPRCX       hprc = hthd->hprc;

    DPRINT(3, (_T("***** ProcessExitThreadEvent, hthd == %x\n"), (DWORD)hthd));


    hthd->tstate |= ts_dead;

    if (hthd->tstate & ts_frozen) {
        //ResumeThread(hthd->rwHand);
        hthd->tstate &= ~ts_frozen;
    }

    hthd->dwExitCode = pde->u.ExitThread.dwExitCode;

    /*
     *  Free all events for this thread
     */

    ConsumeAllThreadEvents(hthd, TRUE);

    //
    //  Let the Expression Breakpoint manager know that this thread
    //  is gone.
    //
    ExprBPExitThread( hprc, hthd );


    if (hprc->pstate & ps_killed) {

    // BUGBUG: This should be set to stopped.
//      hthd->tstate &= ~ts_stopped;
        AddQueue( QT_CONTINUE_DEBUG_EVENT,
                  hthd->hprc->pid,
                  hthd->tid,
                  DBG_CONTINUE,
                  0);
    } else if (fDisconnected) {
        hthd->hprc->pstate |= ps_exited;
        AddQueue( QT_CONTINUE_DEBUG_EVENT,
                  hthd->hprc->pid,
                  hthd->tid,
                  DBG_CONTINUE,
                  0);
    } else {
        NotifyEM(pde, hthd, 0, (LPVOID)0);
    }

    return;
}                                      /* ProcessExitThreadEvent() */


void
ProcessLoadDLLEvent(
    DEBUG_EVENT* de,
    HTHDX hthd
)
{
#ifndef KERNEL
    LPBYTE lpbPacket;
    WORD   cbPacket;

    if (LoadDll(de, hthd, &cbPacket, &lpbPacket) || (cbPacket == 0)) {
        LeaveCriticalSection(&csProcessDebugEvent);
        NotifyEM(de, hthd, cbPacket, lpbPacket);
        EnterCriticalSection(&csProcessDebugEvent);
    }

	SetDebugEventThreadState (hthd->hprc, ts_running);

	/* Can't continue if we're still connecting */
	if(!(hthd->hprc->pstate & ps_connect)) {
		AddQueue( QT_CONTINUE_DEBUG_EVENT,
				  hthd->hprc->pid,
				  hthd->tid,
				  DBG_CONTINUE,
				  0);
	}

#endif // !KERNEL


#ifdef KERNEL
    NotifyEM(de, hthd, 0, (LPVOID)0);

    SetDebugEventThreadState (hthd->hprc, ts_running);
    AddQueue( QT_CONTINUE_DEBUG_EVENT,
              hthd->hprc->pid,
              hthd->tid,
              DBG_CONTINUE,
              0);
#endif // KERNEL

    return;
}                                      /* ProcessLoadDLLEvent() */


void
ProcessUnloadDLLEvent(
    DEBUG_EVENT* pde,
    HTHDX hthd
    )
{
    int         iDll;
    HPRCX       hprc = hthd->hprc;

    DPRINT(10, (_T("*** UnloadDll %x\n"), pde->u.UnloadDll.lpBaseOfDll));

    for (iDll = 0; iDll < hprc->cDllList; iDll++) {
        if (hprc->rgDllList[iDll].fValidDll &&
            (hprc->rgDllList[iDll].offBaseOfImage ==
             (OFFSET) pde->u.UnloadDll.lpBaseOfDll)) {
            break;
        }
    }

    /*
     *  Make sure that we found a legal address.  If not then assert
     *  and check for problems.
     */

#ifndef KERNEL
    // this happens all the time in kernel mode
    // when user mode modloads are enabled
    assert( iDll != hprc->cDllList );
#endif

    if (iDll != hprc->cDllList) {

        if (!fDisconnected) {
			OutputDebugString("Unloading DLL ");
			OutputDebugString(hprc->rgDllList[iDll].szDllName);
            LeaveCriticalSection(&csProcessDebugEvent);
            NotifyEM(pde, hthd, 0, (LPVOID)0);
            EnterCriticalSection(&csProcessDebugEvent);
        }

        DestroyDllLoadItem( &hprc->rgDllList[iDll] );
    }

    SetDebugEventThreadState (hthd->hprc, ts_running);
    AddQueue( QT_CONTINUE_DEBUG_EVENT,
              hthd->hprc->pid,
              hthd->tid,
              DBG_CONTINUE,
              0);
    return;
}

#ifdef OMNI
//
//  Omni JIT support
//
VOID ProcessOmniDllLoadEvent(DEBUG_EVENT* pdeIn, HTHDX hthd)
{
    HPRCX hprc = hthd->hprc;


    EXCEPTION_DEBUG_INFO *pExceptDebugInfo = &(pdeIn->u.Exception);
	DEBUG_EVENT de;
    LPBYTE lpbPacket;
    WORD   cbPacket;

 	assert(pExceptDebugInfo->ExceptionRecord.NumberParameters == 5);
	de.dwDebugEventCode = LOAD_DLL_DEBUG_EVENT;
    de.dwProcessId = hprc->pid;
    de.dwThreadId = hthd->tid;
    de.u.LoadDll.hFile = NULL;
    de.u.LoadDll.lpBaseOfDll = (LPVOID) pExceptDebugInfo->ExceptionRecord.ExceptionInformation[0];
    de.u.LoadDll.dwDebugInfoFileOffset = (DWORD)pExceptDebugInfo->ExceptionRecord.ExceptionInformation[1];
    de.u.LoadDll.nDebugInfoSize = 0;
    de.u.LoadDll.lpImageName = (LPVOID) pExceptDebugInfo->ExceptionRecord.ExceptionInformation[2];
    de.u.LoadDll.fUnicode = (WORD) pExceptDebugInfo->ExceptionRecord.ExceptionInformation[3];

    if (LoadDll5Parm(&de, hthd, &cbPacket, &lpbPacket, (LPVOID)pExceptDebugInfo->ExceptionRecord.ExceptionInformation[4]) ||
		(cbPacket == 0)) {
        LeaveCriticalSection(&csProcessDebugEvent);
        NotifyEM(&de, hthd, cbPacket, lpbPacket);
        EnterCriticalSection(&csProcessDebugEvent);
    }

    SetDebugEventThreadState (hthd->hprc, ts_running);
    AddQueue( QT_CONTINUE_DEBUG_EVENT,
              hthd->hprc->pid,
              hthd->tid,
              DBG_CONTINUE,
              0);


	return;

}

#endif

void
DestroyDllLoadItem(
    PDLLLOAD_ITEM pDll
    )
{
    if (pDll->szDllName) {
        MHFree(pDll->szDllName);
        pDll->szDllName = NULL;
    }

#ifdef KERNEL
    if (pDll->sec) {
        MHFree(pDll->sec);
        pDll->sec = NULL;
    }
#endif

	if (pDll->Sections) {
		MHFree (pDll->Sections);
		pDll->Sections = NULL;
	}

    pDll->offBaseOfImage = 0;
    pDll->cbImage = 0;
    pDll->fValidDll = FALSE;

    return;
}


void
ProcessOutputDebugStringEvent(
    DEBUG_EVENT* de,
    HTHDX hthd
    )
/*++

Routine Description:

    Handle an OutputDebugString from the debuggee

Arguments:

    de      - Supplies DEBUG_EVENT struct

    hthd    - Supplies thread descriptor for thread
              that generated the event

Return Value:

    None

--*/
{
    int     cb = de->u.DebugString.nDebugStringLength;

#if DBG
    char    rgch[256];
    HANDLE  rwHand;

    if (FVerbose) {
        cb = min(cb, 256);
        rwHand = hthd->hprc->rwHand;
		memcpy(rgch, de->u.DebugString.lpDebugStringData, cb);
        rgch[cb] = 0;

        DPRINT(3, (_T("%s\n"), rgch));
    }
#endif

    NotifyEM(de, hthd, 0, NULL);

    SetDebugEventThreadState (hthd->hprc, ts_running);
    AddQueue( QT_CONTINUE_DEBUG_EVENT,
              hthd->hprc->pid,
              hthd->tid,
              DBG_CONTINUE,
              0);

    return;
}

#ifndef KERNEL

void
ProcessBogusSSEvent(
    DEBUG_EVENT* de,
    HTHDX hthd
    )
/*++

Routine Description:

    Handle a Bogus Win95 Single step event. Just continue execution
        as if nothing happened.

Arguments:

    de      - Supplies DEBUG_EVENT struct

    hthd    - Supplies thread descriptor for thread
              that generated the event

Return Value:

    None

--*/
{
    SetDebugEventThreadState (hthd->hprc, ts_running);
    AddQueue( QT_CONTINUE_DEBUG_EVENT,
              hthd->hprc->pid,
              hthd->tid,
              DBG_CONTINUE,
              0);
    return;
}

void 
HandleDisappearedProcess(
    HPRCX hprc
)
{
    DEBUG_EVENT de;
    HTHDX hthd;

    assert(hprc);

    de.dwProcessId = hprc->pid;
    de.dwDebugEventCode = EXIT_PROCESS_DEBUG_EVENT;
    de.u.ExitProcess.dwExitCode = 0;
    hthd = hprc->hthdChild;
    if (hthd) {
        de.dwThreadId = hthd->tid;
    } else {
        de.dwThreadId = 0;
    }

    ProcessDebugEvent(&de);
    return;
}       /* HandleDisappearedProcess */

#endif


void
Reply(
    UINT   length,
    LPVOID lpbBuffer,
    HPID   hpid
    )
/*++

Routine Description:

    Send a reply packet to a tlfRequest from the EM

Arguments:

    length      - Supplies length of reply
    lpbBuffer   - Supplies reply data
    hpid        - Supplies handle to EM process descriptor

Return Value:

    None

--*/
{
    /*
     *  Add the size of the packet header to the length
     */

    length += FIELD_OFFSET(DM_MSG, rgb);

    DPRINT(5, (_T("Reply to EM [%d]\n"), length));

    assert(length <= sizeof(abDMReplyBuf) || lpbBuffer != abDMReplyBuf);

    if (DmTlFunc) { // IF there is a TL loaded, reply
        DmTlFunc(tlfReply, hpid, length, (LONG)(LPV) lpbBuffer);
    }

    return;
}


VOID FAR PASCAL
DMFunc(
    DWORD cb,
    LPDBB lpdbb
    )
/*++

Routine Description:

    This is the main entry point for the DM.  This takes dmf
    message packets from the debugger and handles them, usually
    by dispatching to a worker function.

Arguments:

    cb      - supplies size of data packet

    lpdbb   - supplies pointer to packet

Return Value:


--*/
{
    DMF     dmf;
    HPRCX   hprc;
    HTHDX   hthd;
    XOSD    xosd = xosdNone;

    dmf = (DMF) (lpdbb->dmf & 0xffff);
    DEBUG_PRINT(_T("DMFunc\r\n"));

    DPRINT(5, (_T("DmFunc [%2x] "), dmf));

    hprc = HPRCFromHPID(lpdbb->hpid);
    hthd = HTHDXFromHPIDHTID(lpdbb->hpid, lpdbb->htid);


    switch ( dmf ) {
#ifndef OSDEBUG4
      case dmfGetPrompt:
        {
        LPPROMPTMSG pm;
        DPRINT(5, (_T("dmfGetPrompt\n")));
        pm = (LPPROMPTMSG) LpDmMsg->rgb;
        *pm = *((LPPROMPTMSG) lpdbb->rgbVar);
        memcpy( pm, lpdbb->rgbVar, pm->len+sizeof(PROMPTMSG) );

#if   defined(KERNEL) && defined(TARGET_i386)
        _ftcscpy( pm->szPrompt, _T("KDx86> ") );
#elif defined(KERNEL) && defined(TARGET_PPC)
        _ftcscpy( pm->szPrompt, _T("KDppc> ") );
#elif defined(KERNEL) && defined(TARGET_MIPS)
        _ftcscpy( pm->szPrompt, _T("KDmips> ") );
#elif defined(KERNEL) && defined(TARGET_ALPHA)
        _ftcscpy( pm->szPrompt, _T("KDalpha> ") );
#elif defined(KERNEL)

#pragma error( "unknown target machine" );

#else

        // don't change prompt in user mode

#endif

        LpDmMsg->xosdRet = xosdNone;
        Reply( pm->len+sizeof(PROMPTMSG), LpDmMsg, lpdbb->hpid );
        }
        break;
#endif  // !OSDEBUG4

      case dmfSelLim:
        DPRINT(5, (_T("dmfSelLim\n")));
        ProcessSelLimCmd(hprc, hthd, lpdbb);
        break;

      case dmfSetMulti:
        DPRINT(5, (_T("dmfSetMulti\n")));
        LpDmMsg->xosdRet = xosdNone;
        Reply( 0, LpDmMsg, lpdbb->hpid );
        break;

      case dmfClearMulti:
        DPRINT(5, (_T("dmfClearMulti\n")));
        LpDmMsg->xosdRet = xosdNone;
        Reply( 0, LpDmMsg, lpdbb->hpid );
        break;

      case dmfDebugger:
        DPRINT(5, (_T("dmfDebugger\n")));
        LpDmMsg->xosdRet = xosdNone;
        Reply( 0, LpDmMsg, lpdbb->hpid );
        break;

      case dmfCreatePid:
        DPRINT(5,(_T("dmfCreatePid\r\n")));
        ProcessCreateProcessCmd(hprc, hthd, lpdbb);
        break;

      case dmfDestroyPid:
        DPRINT(5, (_T("dmfDestroyPid\n")));
        LpDmMsg->xosdRet = FreeProcess(hprc, TRUE);
        Reply( 0, LpDmMsg, lpdbb->hpid);
        break;

      case dmfSpawnOrphan:
        DPRINT(5, (_T("dmfSpawnOrphan\n")));
        ProcessSpawnOrphanCmd (hprc, hthd, lpdbb);
        break;

      case dmfProgLoad:
        DPRINT(5, (_T("dmfProgLoad\n")));
        if(!fApprovedAttach) {
            fApprovedAttach = TRUE;
            xosd = AttachOrReboot();
        }
        if(xosd == xosdNone)
            ProcessProgLoadCmd(hprc, hthd, lpdbb);
        else
            Reply( 0, &xosd, lpdbb->hpid);
        break;

      case dmfProgFree:
        DPRINT(5, (_T("dmfProgFree\n")));


#ifndef KERNEL

        ProcessTerminateProcessCmd(hprc, hthd, lpdbb);

#else // KERNEL
        if (!hprc) {
            LpDmMsg->xosdRet = xosdNone;
            Reply( 0, LpDmMsg, lpdbb->hpid);
            break;
        }

        if (KdOptions[KDO_GOEXIT].value) {
            HTHDX hthdT;
            PBREAKPOINT bp;
            KdOptions[KDO_GOEXIT].value = 0;
            for (hthdT = hprc->hthdChild; hthdT; hthdT = hthdT->nextSibling) {
                if (hthdT->tstate & ts_stopped) {
                    if (bp = AtBP(hthdT)) {
                        if (!hthdT->fDontStepOff) {
                            IncrementIP(hthdT);
                        }
                    }
                    if (hthdT->fContextDirty) {
                        DbgSetThreadContext( hthdT, &hthdT->context );
                        hthdT->fContextDirty = FALSE;
                    }
                    KdOptions[KDO_GOEXIT].value = 1;
                    break;
                }
            }
        }

        ClearBps();

        ProcessTerminateProcessCmd(hprc, hthd, lpdbb);
        ProcessUnloadCmd(hprc, hthd, lpdbb);

        if (KdOptions[KDO_GOEXIT].value) {
            ContinueTargetSystem( DBG_CONTINUE, NULL );
        }


#endif  // KERNEL

        LpDmMsg->xosdRet = xosdNone;
        Reply( 0, LpDmMsg, lpdbb->hpid);
        break;

      case dmfBreakpoint:
        DEBUG_PRINT(_T("dmfBreakpoint\r\n"));
        ProcessBreakpointCmd(hprc, hthd, lpdbb);
        break;

      case dmfReadMem:
        // This replies in the function
        DPRINT(5, (_T("dmfReadMem\n")));
        ProcessReadMemoryCmd(hprc, hthd, lpdbb);
        break;

      case dmfWriteMem:
        DPRINT(5, (_T("dmfWriteMem\n")));
        ProcessWriteMemoryCmd(hprc, hthd, lpdbb);
        break;

      case dmfReadReg:
        DPRINT(5, (_T("dmfReadReg\n")));
        ProcessGetContextCmd(hprc, hthd, lpdbb);
        break;

      case dmfWriteReg:
        DPRINT(5, (_T("dmfWriteReg\n")));
        ProcessSetContextCmd(hprc, hthd, lpdbb);
        break;

#ifdef HAS_DEBUG_REGSNOTREALLY
      case dmfReadRegEx:
        DPRINT(5, (_T("dmfReadRegEx\n")));
#ifdef KERNEL
        ProcessGetExtendedContextCmd(hprc, hthd, lpdbb);
#else
        ProcessGetDRegsCmd(hprc, hthd, lpdbb);
#endif
        break;

      case dmfWriteRegEx:
        DPRINT(5, (_T("dmfWriteRegEx\n")));
#ifdef KERNEL
        ProcessSetExtendedContextCmd(hprc, hthd, lpdbb);
#else
        ProcessSetDRegsCmd(hprc, hthd, lpdbb);
#endif
        break;

#else   // HAS_DEBUG_REGS
      case dmfReadRegEx:
      case dmfWriteRegEx:
        DEBUG_PRINT(_T("Read/WriteRegEx\r\n"));
        //assert(dmf != dmfReadRegEx && dmf != dmfWriteRegEx);
        LpDmMsg->xosdRet = xosdUnknown;
        Reply( 0, LpDmMsg, lpdbb->hpid );
        break;
#endif  // HAS_DEBUG_REGS

      case dmfGo:
        DPRINT(5, (_T("dmfGo\n")));
#if !defined(KERNEL)
        //Turn off fiber debugging
        hprc->pFbrCntx = NULL;
#endif
        ProcessContinueCmd(hprc, hthd, lpdbb);
        break;

#if defined(KERNEL)
      case dmfTerm:
        DPRINT(5, (_T("dmfTerm\n")));
        ProcessTerminateProcessCmd(hprc, hthd, lpdbb);
        break;
#endif

      case dmfStop:
        DPRINT(5, (_T("dmfStop\n")));
        ProcessAsyncStopCmd(hprc, hthd, lpdbb);
        break;

      case dmfFreeze:
        DPRINT(5, (_T("dmfFreeze\n")));
        ProcessFreezeThreadCmd(hprc, hthd, lpdbb);
        break;

      case dmfResume:
        DPRINT(5, (_T("dmfResume\n")));
        ProcessAsyncGoCmd(hprc, hthd, lpdbb);
        break;

      case dmfInit:
        DPRINT(5, (_T("dmfInit\n")));
        fApprovedAttach = FALSE;
        Reply( 0, &xosd, lpdbb->hpid);
        break;

      case dmfUnInit:
        DPRINT(5, (_T("dmfUnInit\n")));
#ifdef KERNEL
        DmPollTerminate();
#else
        Cleanup();
#endif
        Reply ( 1, LpDmMsg, lpdbb->hpid);
        break;


      case dmfGetDmInfo:
        DEBUG_PRINT(_T("getDmInfo\r\n"));
        ProcessGetDmInfoCmd(hprc, lpdbb, cb);
        break;

    case dmfSetupExecute:
        DPRINT(5, (_T("dmfSetupExecute\n")));
        ProcessSetupExecuteCmd(hprc, hthd, lpdbb);
        break;

    case dmfStartExecute:
        DPRINT(5, (_T("dmfStartExecute\n")));
        ProcessStartExecuteCmd(hprc, hthd, lpdbb);
        break;

    case dmfCleanUpExecute:
        DPRINT(5, (_T("dmfCleanupExecute\n")));
        ProcessCleanUpExecuteCmd(hprc, hthd, lpdbb);
        break;

    case dmfSystemService:
        ProcessSystemServiceCmd( hprc, hthd, lpdbb );
        break;

    case dmfDebugActive:
        DPRINT(5, (_T("dmfDebugActive\n")));
        ProcessDebugActiveCmd( hprc, hthd, lpdbb);
        break;

    case dmfSetPath:
        DPRINT(5, (_T("dmfSetPath\n")));
        ProcessSetPathCmd( hprc, hthd, lpdbb );
        break;

    case dmfQueryTlsBase:
        DPRINT(5, (_T("dmfQueryTlsBase\n")));
        ProcessQueryTlsBaseCmd(hprc, hthd, lpdbb );
        break;

    case dmfQuerySelector:
        //DPRINT(5, (_T("dmfQuerySelector\n")));
        ProcessQuerySelectorCmd(hprc, hthd, lpdbb );
        break;

    case dmfVirtualQuery:
        DEBUG_PRINT(_T("VirtualQuery\r\n"));
        ProcessVirtualQueryCmd(hprc, lpdbb);
        break;

    case dmfRemoteQuit:
        DEBUG_PRINT(_T("RemoteQuit\r\n"));
        ProcessRemoteQuit();
        break;

#ifdef KERNEL
    case dmfGetSections:
        ProcessGetSectionsCmd( hprc, hthd, lpdbb );
        break;
#endif

    case dmfSetExceptionState:
        DEBUG_PRINT(_T("SetExceptionState\r\n"));
        ProcessSetExceptionState(hprc, hthd, lpdbb);
        break;

    case dmfGetExceptionState:
        DEBUG_PRINT(_T("GetExceptionState\r\n"));
        ProcessGetExceptionState(hprc, hthd, lpdbb);
        break;

    case dmfSingleStep:
        DEBUG_PRINT(_T("SingleStep\r\n"));
#if !defined(KERNEL)
        //Turn off fiber debugging
        hprc->pFbrCntx = NULL;
#endif
        ProcessSingleStepCmd(hprc, hthd, lpdbb);
        break;

    case dmfRangeStep:
        DEBUG_PRINT(_T("RangeStep\r\n"));
#if !defined(KERNEL)
        //Turn off fiber debugging
        hprc->pFbrCntx = NULL;
#endif
        ProcessRangeStepCmd(hprc, hthd, lpdbb);
        break;

    case dmfReturnStep:
        DEBUG_PRINT(_T("ReturnStep\r\n"));
        ProcessReturnStepCmd(hprc, hthd, lpdbb);
        break;

#ifndef KERNEL
    case dmfNonLocalGoto:
        DEBUG_PRINT(_T("NonLocalGoto\r\n"));
        //Turn off fiber debugging
        hprc->pFbrCntx = NULL;
        ProcessNonLocalGoto(hprc, hthd, lpdbb);
        break;
#endif

    case dmfThreadStatus:
        DEBUG_PRINT(_T("ThreadStatus\r\n"));
        Reply( ProcessThreadStatCmd(hprc, hthd, lpdbb), LpDmMsg, lpdbb->hpid);
        break;

    case dmfProcessStatus:
        DEBUG_PRINT(_T("ProcessStatus\r\n"));
                LpDmMsg->xosdRet = xosdNone;
        Reply( ProcessProcStatCmd(hprc, hthd, lpdbb), LpDmMsg, lpdbb->hpid);
        break;

    case dmfGetTimeStamp:
        if(!fApprovedAttach) {
            fApprovedAttach = TRUE;
            xosd = AttachOrReboot();
        }
        if(xosd == xosdNone)
            Reply (ProcessGetTimeStamp (hprc, hthd, lpdbb), LpDmMsg, lpdbb->hpid);
        else
            Reply( 0, &xosd, lpdbb->hpid);
        break;

	case dmfCreateUserCrashDump:

		//
		// NYI
		//
		
		LpDmMsg->xosdRet = xosdUnknown;
		Reply( 0, LpDmMsg, lpdbb->hpid );
		break;

	case dmfGetSpecialReg:
		Reply( ProcessGetSpecialReg(hprc, hthd, lpdbb), LpDmMsg, lpdbb->hpid);
		break;

	case dmfSetSpecialReg:
		Reply( ProcessSetSpecialReg(hprc, hthd, lpdbb), LpDmMsg, lpdbb->hpid);
		break;

    default:
        DPRINT(5, (_T("Unknown\n")));
        assert(FALSE);
        break;
    }

    return;
}                         /* DMFunc() */

#ifndef KERNEL

BOOL
StartDmPollThread(
    void
    )
/*++

Routine Description:

    This creates the DM poll thread.

Arguments:

    none

Return Value:

    TRUE if the thread was successfully created or already
    existed.

--*/
{
    DWORD   tid;

    if (hDmPollThread) {
        return TRUE;
    }

	fDmPollQuit = FALSE; // need this set before the thread starts
    hDmPollThread = CreateThread(0,0,CallDmPoll,0,0,&tid);

    return hDmPollThread != 0;
}

#if 0
BOOL
StartCrashPollThread(
    void
    )
/*++

Routine Description:

    This creates the DM poll thread for a crash dump file.

Arguments:

    none

Return Value:

    TRUE if the thread was successfully created or already existed.

--*/
{
    DWORD   tid;

    if (hDmPollThread) {
        return TRUE;
    }

    hDmPollThread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)CrashDumpThread,0,0,&tid);

    return hDmPollThread != 0;
}

VOID
CrashDumpThread(
    LPVOID lpv
    )
{
    HPRCX            hprc;
    HTHDX            hthd;
    HTHDX            hthdNew;
    LPBYTE           lpbPacket;
    WORD             cbPacket;
    PCRASH_MODULE    CrashModule;
    DEBUG_EVENT      de;
    DWORD            i;
    CHAR             buf[_MAX_FNAME];
    DWORD            DumpVer = CrashDumpHeader->MajorVersion;
	CHAR             szMainName[_MAX_FNAME + _MAX_EXT];


    CrashDump = TRUE;
	fDmPollQuit = FALSE;

    //
    // simulate a create process debug event
    //
    CrashModule = (PCRASH_MODULE)((PUCHAR)CrashDumpHeader+CrashDumpHeader->ModuleOffset);
    de.dwDebugEventCode = CREATE_PROCESS_DEBUG_EVENT;
    de.dwProcessId = 1;
    de.dwThreadId  = 1;
    de.u.CreateProcessInfo.hFile = NULL;
    de.u.CreateProcessInfo.hProcess = NULL;
    de.u.CreateProcessInfo.hThread = NULL;
    de.u.CreateProcessInfo.lpBaseOfImage = (LPVOID)CrashModule->BaseOfImage;
    de.u.CreateProcessInfo.dwDebugInfoFileOffset = 0;
    de.u.CreateProcessInfo.nDebugInfoSize = 0;
    de.u.CreateProcessInfo.lpStartAddress = NULL;
    de.u.CreateProcessInfo.lpThreadLocalBase = NULL;
    if (!CrashModule->ImageName[0]) {
		if (nameBuffer[0])
		{
			// Dr Watson didn't give us the name of the exe, so make one up based
			// on the .dmp filename
			_splitpath( nameBuffer, NULL, NULL, buf, NULL );
			_makepath( szMainName, NULL, NULL, buf, ".exe" );
			de.u.CreateProcessInfo.lpImageName = szMainName;
		}
		else
			de.u.CreateProcessInfo.lpImageName = "unknown.exe";
    } else {
       de.u.CreateProcessInfo.lpImageName = CrashModule->ImageName;
    }
    _ftcscpy( nameBuffer, de.u.CreateProcessInfo.lpImageName);
    de.u.CreateProcessInfo.fUnicode = FALSE;
    ProcessDebugEvent(&de);
    WaitForSingleObject( hEventCreateProcess, INFINITE );

    //
    // mark the process as 'being connected' so that the continue debug
    // events that are received from the shell are ignored
    //
    hprc = HPRCFromPID(1);
    hthd = hprc->hthdChild;
    hprc->pstate |= ps_connect;

    if (DumpVer >= 4) {
        DmpGetThread( 0, &hthd->CrashThread );
    } else {
        ZeroMemory(&hthd->CrashThread, sizeof(CRASH_THREAD));
        hthd->CrashThread.ThreadId = hthd->tid;
    }

    //
    // generates the mod load events
    //
    for (i=1; i<CrashDumpHeader->ModuleCount; i++) {

		int	len = CrashModule->ImageName [0] ? CrashModule->ImageNameLength : 1;

		CrashModule = (PCRASH_MODULE)
						((PUCHAR)CrashModule + sizeof(CRASH_MODULE) + len );
										
        de.dwDebugEventCode                = LOAD_DLL_DEBUG_EVENT;
        de.dwProcessId                     = 1;
        de.dwThreadId                      = 1;
        de.u.LoadDll.hFile                 = NULL;
        de.u.LoadDll.lpBaseOfDll           = (LPVOID)CrashModule->BaseOfImage;
        if (!CrashModule->ImageName[0]) {
           sprintf( buf, "mod_%x.dll", CrashModule->BaseOfImage );
           de.u.LoadDll.lpImageName = buf;
        } else {
           de.u.LoadDll.lpImageName = CrashModule->ImageName;
        }
        de.u.LoadDll.fUnicode              = FALSE;
        de.u.LoadDll.nDebugInfoSize        = 0;
        de.u.LoadDll.dwDebugInfoFileOffset = 0;
        if (LoadDll(&de,hthd,&cbPacket,&lpbPacket) && (cbPacket != 0)) {
            NotifyEM(&de, hthd, cbPacket, lpbPacket);
        }
    }

    //
    // create all of the threads
    //
    for (i=1; i<CrashDumpHeader->ThreadCount; i++) {
        //
        // generate a thread create event
        //
        ResetEvent( hprc->hEventCreateThread );
        ResetEvent( hEventContinue );
        de.dwDebugEventCode = CREATE_THREAD_DEBUG_EVENT;
        de.dwProcessId = hprc->pid;
        de.dwThreadId = i + 1;
        de.u.CreateThread.hThread = (HANDLE) (i + 1);
        de.u.CreateThread.lpThreadLocalBase = 0;
        de.u.CreateThread.lpStartAddress = 0;

        //
        // Take critical section here so that it is still
        // held after leaving ProcessDebugEvent
        //
        EnterCriticalSection(&csProcessDebugEvent);
        ProcessDebugEvent(&de);
        hthdNew = HTHDXFromPIDTID(1,(i+1));
        DbgGetThreadContext( hthdNew, &hthdNew->context );
        if (DumpVer >= 4) {
            DmpGetThread( i, &hthdNew->CrashThread );
        } else {
            ZeroMemory(&hthdNew->CrashThread, sizeof(CRASH_THREAD));
            hthdNew->CrashThread.ThreadId = hthdNew->tid;
        }
        LeaveCriticalSection(&csProcessDebugEvent);
        WaitForSingleObject( hprc->hEventCreateThread, INFINITE );

        //
        // wait for the shell to continue the new thread
        //
        WaitForSingleObject( hEventContinue, INFINITE );
    }

    //
    // generate a load complete debug event
    //

	ConsumeAllProcessEvents (hprc, FALSE);
	
    de.dwProcessId = hprc->pid;
    de.dwThreadId = hprc->hthdChild->tid;
    de.dwDebugEventCode = LOAD_COMPLETE_DEBUG_EVENT;
    NotifyEM( &de, hthd, 0, 0L);

    WaitForSingleObject( hEventContinue, INFINITE );

    //
    // Get the debug event for the crash
    //
    if (DumpVer >= 4) {
        de = *(LPDEBUG_EVENT)((PUCHAR)CrashDumpHeader+CrashDumpHeader->DebugEventOffset);
        //
        // convert the thread and process ids into the internal version...
        //
        de.dwProcessId = 1;
        for (hthd = hprc->hthdChild; hthd; hthd = hthd->nextSibling) {
            if (de.dwThreadId == hthd->CrashThread.ThreadId) {
                de.dwThreadId = hthd->tid;
                break;
            }
        }
    } else {
        //
        // if this is an old crashdump file, try to find the crashed thread
        //
        de.dwDebugEventCode = EXCEPTION_DEBUG_EVENT;
        de.u.Exception.ExceptionRecord  = *CrashException;
        for (hthd = hprc->hthdChild; hthd; hthd = hthd->nextSibling) {
            if (PC(hthd) == (DWORD)CrashException->ExceptionAddress) {
                de.dwThreadId = hthd->tid;
            }
        }
    }

    ProcessDebugEvent( &de );

    while (!fDmPollQuit) {
        //
        // Handle kill commands
        //
        if (KillQueue) {
            CompleteTerminateProcessCmd();
        }
        Sleep( 500 );
    }
}
#endif // 0


DWORD WINAPI
CallDmPoll(
    LPVOID lpv
    )

/*++

Routine Description:

    This is the debug event loop.  This routine creates or
    attaches to child process, and monitors them for debug
    events.  It serializes the dispatching of events from
    multiple process, and continues the events when the
    worker functions have finished processing the events.

Arguments:

    lpv - Supplies an argument provided by CreateThread.

Return Value:

    None.

--*/

{
    DEBUG_EVENT de;
	DEQ deq;
    int         nprocs = 0;
    UINT        ErrMode;
	HPRCX       hprc;

    Unreferenced( lpv );
    DEBUG_PRINT(_T("CallDmPoll\r\n"));

#if 0
#ifdef _DEBUG
	// lets make it obvious which thread this is, Debug only
	SetThreadName( GetCurrentThreadId(), "DMPoll", 0);
#endif
#endif // 0

    //
    // Crank up priority to improve performance, and improve our
    // chances of winning races with the debuggee.
    //
    //
    // NOTE:
    //
    //  Setting the thread priority here makes emulation a LOT faster.
    //  Unfortunately, it also starves out the UI threads when we are
    //  emulating.
    //
    
    // SetThreadPriority (GetCurrentThread (), THREAD_PRIORITY_ABOVE_NORMAL);


	//fDmPollQuit is set in StartPollThread
    //fDmPollQuit = FALSE;
    while (!fDmPollQuit) {

        //
        // Handle kill commands
        //
        if (KillQueue) {
            CompleteTerminateProcessCmd();
            goto doContinues;
        }

#if 0
        //
        // Handle spawn commands
        //
        if (SpawnStruct.fSpawn) {
            SpawnStruct.fSpawn = FALSE;
            ErrMode = SetErrorMode( SEM_NOALIGNMENTFAULTEXCEPT | 0 );
            SpawnStruct.fReturn =
                CreateProcess( SpawnStruct.szAppName,
                             SpawnStruct.szArgs,
                             NULL,
                             NULL,
                             SpawnStruct.fInheritHandles,
                             SpawnStruct.fdwCreate,
                             NULL,
                             SpawnStruct.pszCurrentDirectory,
                             &SpawnStruct.si,
                             &pi
                             );
            SetErrorMode(  SEM_NOALIGNMENTFAULTEXCEPT | ErrMode );
            if (SpawnStruct.fReturn == 0) {
                SpawnStruct.dwError = GetLastError();
            } else {
                SpawnStruct.dwError = 0;
                CloseHandle(pi.hThread);
                CloseHandle(pi.hProcess);
                Close3Handles(rgh);
            }

            SetEvent(SpawnStruct.hEventApiDone);
        }

        //
        // Handle attach commands
        //
        if (DebugActiveStruct.fAttach) {
            DebugActiveStruct.fAttach = FALSE;
            SetDebugPrivilege ();
            DebugActiveStruct.fReturn = DebugActiveProcess(DebugActiveStruct.dwProcessId);
            if (DebugActiveStruct.fReturn == 0) {
                DebugActiveStruct.dwError = GetLastError();
            } else {
                DebugActiveStruct.dwError = 0;
            }
            SetEvent(DebugActiveStruct.hEventApiDone);
        }
#endif

        if (WtStruct.fWt) {
            WtStruct.fWt = FALSE;
            switch(WtStruct.dwType) {
                case IG_WATCH_TIME_STOP:
                    WtStruct.hthd->wtmode = 2;
                    break;

                case IG_WATCH_TIME_RECALL:
                    break;

                case IG_WATCH_TIME_PROCS:
                    break;
            }
        }

#if 0
#ifndef KERNEL
        // In some cases the process just disappears from under us with no notifications.
        // This is only known to happen on Win95.

        if (IsChicago() && prcList->next != NULL) {
            hprc = NULL;

            EnterCriticalSection(&csThreadProcList);

            for (hprc = prcList->next; hprc ; hprc = hprc->next) {
                if (!(hprc->pstate & (ps_dead|ps_killed)))  {
                    if (WaitForSingleObject(hprc->rwHand, 0) == WAIT_OBJECT_0) {
                        break;
                    }
                }
            }
            
            LeaveCriticalSection(&csThreadProcList);

            // Fake up the process terminate notification. 
            if (hprc != hprcxNull)  {
                HandleDisappearedProcess(hprc);
            }
        }
#endif
#endif

        if (WaitDEQ(&deq, (DWORD) WAITFORDEBUG_MS)) {
			de = deq.de;

#if 0
			//
			// This code simulates the OS not sending us an EXIT_PROCESS
			// event.  Enable it ONLY to test that termination case.
			//

			assert (FALSE);
			if (de.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) {

				AddQueue (QT_CONTINUE_DEBUG_EVENT,
						  de.dwProcessId,
						  de.dwThreadId,
						  DBG_CONTINUE,
						  0);
				continue;
			}
#endif
						
			UserTimerStop();


            if ( de.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT ) {
                assert(HPRCFromPID(de.dwProcessId) == NULL);
                     
                if (nprocs == 0) {
                    ResetEvent(hEventNoDebuggee);
                }
                ++nprocs;
            } else if ( de.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) {
                --nprocs;
            }

            if (fDisconnected) {
                if (de.dwDebugEventCode == LOAD_DLL_DEBUG_EVENT       ||
                    de.dwDebugEventCode == UNLOAD_DLL_DEBUG_EVENT     ||
                    de.dwDebugEventCode == CREATE_THREAD_DEBUG_EVENT  ||
                    de.dwDebugEventCode == EXIT_THREAD_DEBUG_EVENT      ) {

                    //
                    // we can process these debug events very carefully
                    // while disconnected from the shell.  the only requirement
                    // is that the dm doesn't call NotifyEM while disconnected.
                    //

                } else
#if 0
                if (de.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) {

                    //
                    // if the exception is to be ignored, we can handle it here.
                    // if not, we have to wait for a reconnect.
                    //

                } else
#endif
                {

                    WaitForSingleObject( hEventRemoteQuit, INFINITE );
                    ResetEvent( hEventRemoteQuit );
                    fDisconnected = FALSE;
                    //
                    // this is a remote session reconnecting
                    //
                    ReConnectDebugger( &de, FALSE );

                }
            }

            if ( fxFlagSet && 
                (de.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) &&
                (de.u.Exception.dwFirstChance == 0) ) {
                    //
                    //	v-vadimp
                    //
                    // this is not pretty, but there seems to be not other way...
                    //
                    // when a second chance exception is generated, for it the OS reports the record of the SS (or BP) 
                    // exception generated by the FXSAVE/FXRSTOR hack code hit while processing the first chance exception, 
                    // hence the debugger gets totally hosed. So what we do here is restore u.Exception.ExceptionRecord from
                    // the backup we made when the first chance exception hit (presumably they are the same, and the backup
                    // area has not been used again, i.e. no exceptions have been reported between the first anc second chance). 
                    // Also the OS not only wacks the exception record, but also updates the thread's IP to point to where it thinks
                    // the exception is occuring (i.e. the trojan FXSAVE/FXRSTOR code location), so the IP has to be restored too.
                    //
                    CONTEXT ContextRestore;
                    HTHDX hthd = HTHDXFromPIDTID(de.dwProcessId,de. dwThreadId);
					
                    assert (deBackup.dwDebugEventCode == EXCEPTION_DEBUG_EVENT);
                    assert (deBackup.u.Exception.dwFirstChance == 1);
                    assert (deBackup.dwProcessId == deBackup.dwProcessId);
                    assert (deBackup.dwThreadId == deBackup.dwThreadId);
					
                    de.u.Exception.ExceptionRecord = deBackup.u.Exception.ExceptionRecord;
                    
                    //do not use CONTEXT_FULL - it will do FXSAVE/FXRSTOR again!!!
                    ContextRestore.ContextFlags = CONTEXT_CONTROL; 
                    VERIFY(DbgGetThreadContext(hthd, &ContextRestore));
                    ContextRestore.Eip = (DWORD)de.u.Exception.ExceptionRecord.ExceptionAddress;
                    VERIFY(DbgSetThreadContext(hthd, &ContextRestore));

            }

            fxFlagSet = FALSE;
            deBackup = de;

            ProcessDebugEvent(&de);

			if(de.dwDebugEventCode == OUTPUT_DEBUG_STRING_EVENT)
				/* We need to free the string */
				MHFree(de.u.DebugString.lpDebugStringData);

        } else if (WaitForSingleObject( hEventRemoteQuit, 0 ) ==
                                                               WAIT_OBJECT_0) {

            //
            // this is a remote session reconnecting
            //
            ResetEvent( hEventRemoteQuit );
            ReConnectDebugger( NULL, FALSE );

        } else if (nWaitingForLdrBreakpoint) {

            // look for processes that are looking for a loader bp.
            // See how long it has been since they got an event.

            EnterCriticalSection(&csThreadProcList);
            for (hprc = prcList->next; hprc; hprc = hprc->next) {
                if (hprc->pstate & ps_preStart) {
                    if (++hprc->cLdrBPWait > LDRBP_MAXTICKS) {
                        // Signal a timeout for this one.
                        // just jump out of this loop - if
                        // another one is going to time out,
                        // it can do it on the next pass.
                        break;
                    }
                }
            }
            LeaveCriticalSection(&csThreadProcList);

            if (hprc) {
                HandleDebugActiveDeadlock(hprc);
            }

        } else if (nprocs == 0) {
			
			Sleep (WAITFORDEBUG_MS);

        } else {

			EnterCriticalSection (&csThreadProcList);

			for (hprc = prcList->next; hprc; hprc = hprc->next) {
				FireOutstandingTimerEvents (hprc);
			}

			LeaveCriticalSection (&csThreadProcList);
		}
		

    doContinues:
        if (DequeueAllEvents(TRUE, FALSE) && nprocs <= 0) {
            SetEvent(hEventNoDebuggee);
        }
    }

    DEBUG_PRINT(_T("CallDmPoll Exit\r\n"));

	return 0;	// NOTREACHED

}


#if 0

BOOL
SetDebugPrivilege(
    void
    )
/*++

Routine Description:

    Enables SeDebugPrivilege if possible.

Arguments:

    none

Return Value:

    TRUE on success, FALSE on failure

--*/
{
    HANDLE  hToken;
    LUID    DebugValue;
    TOKEN_PRIVILEGES tkp;
    BOOL    rVal = TRUE;

    if (!OpenProcessToken(GetCurrentProcess(),
         TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        return FALSE;
    }

    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &DebugValue)) {

        rVal = FALSE;

    } else {

        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Luid = DebugValue;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        AdjustTokenPrivileges(hToken,
            FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);

        if (GetLastError() != ERROR_SUCCESS) {
            rVal = FALSE;
        }
    }

    CloseHandle(hToken);

    return rVal;
}

#endif  // !KERNEL
#endif // 0


/********************************************************************/
/*                                                                  */
/* Dll Version                                                      */
/*                                                                  */
/********************************************************************/

#ifdef KERNEL

#ifdef DEBUGVER
DEBUG_VERSION('D','M',"NT Kernel Debugger Monitor")
#else
RELEASE_VERSION('D','M',"NT Kernel Debugger Monitor")
#endif

#else // KERNEL

#ifdef DEBUGVER
DEBUG_VERSION('D','M',"WIN32 Debugger Monitor")
#else
RELEASE_VERSION('D','M',"WIN32 Debugger Monitor")
#endif

#endif  // KERNEL

DBGVERSIONCHECK()

#define EXTENDED_REGISTERS_SUPPORT_KEY     _T("Software\\Microsoft\\Devstudio\\6.0\\Debug")


int
WINAPI
DmDllMain(
    HANDLE hModule,
    DWORD  dwReason,
    DWORD  dwReserved
    )
/*++

Routine Description:

    Entry point called by the loader during DLL initialization
    and deinitialization.  This creates and destroys some per-
    instance objects.

Arguments:

    hModule     - Supplies base address of dll

    dwReason    - Supplies flags describing why we are here

    dwReserved  - depends on dwReason.

Return Value:

    TRUE

--*/
{
    Unreferenced(hModule);
    Unreferenced(dwReserved);
	//SetErrorMode(SEM_NOALIGNMENTFAULTEXCEPT);
    switch (dwReason) {

      case DLL_THREAD_ATTACH:
        break;

      case DLL_THREAD_DETACH:
        break;

      case DLL_PROCESS_DETACH:
		  if(psessNotify)
			  DmCloseNotificationSession(psessNotify);

#ifndef XBOX // XBUG
        if(hDmPollThread > 0) {
		//Badness has happened do our best to minimize the fallout
            TerminateThread(hDmPollThread,0);
            CloseHandle(hDmPollThread);
		}
		if(hTerminateThread > 0) {
        //Badness has happened do our best to minimize the fallout
            TerminateThread(hTerminateThread,0);
            CloseHandle(hTerminateThread);
		}
#endif
        //CloseHandle(SpawnStruct.hEventApiDone);
        CloseHandle(hEventCreateProcess);
        CloseHandle(hEventRemoteQuit);
        CloseHandle(hEventNoDebuggee);
        CloseHandle(hEventContinue);

        DeleteCriticalSection(&csProcessDebugEvent);
        DeleteCriticalSection(&csThreadProcList);
        DeleteCriticalSection(&csFXOps);
        DeleteCriticalSection(&csEventList);
        DeleteCriticalSection(&csWalk);
#if !defined(KERNEL)
        CloseHandle(DebugActiveStruct.hEventApiDone);
        CloseHandle(DebugActiveStruct.hEventReady);
        DeleteCriticalSection(&csKillQueue);
#endif
        break;

      case DLL_PROCESS_ATTACH:

        InitializeCriticalSection(&csProcessDebugEvent);
        InitializeCriticalSection(&csThreadProcList);
        InitializeCriticalSection(&csFXOps);
        InitializeCriticalSection(&csEventList);
        InitializeCriticalSection(&csWalk);

        hEventCreateProcess = CreateEvent(NULL, TRUE, FALSE, NULL);
        hEventRemoteQuit = CreateEvent(NULL, TRUE, FALSE, NULL);
        hEventContinue = CreateEvent(NULL, TRUE, FALSE, NULL);
        hEventNoDebuggee    = CreateEvent(NULL, TRUE, FALSE, NULL);
        //SpawnStruct.hEventApiDone = CreateEvent(NULL, TRUE, FALSE, NULL);

#if !defined(KERNEL)
        InitializeCriticalSection(&csKillQueue);
        DebugActiveStruct.hEventApiDone = CreateEvent(NULL, TRUE, TRUE, NULL);
        DebugActiveStruct.hEventReady   = CreateEvent(NULL, TRUE, TRUE, NULL);

        /*
         * These parameters are from SCOTTLU
         */

#ifndef XBOX
        SetProcessShutdownParameters(0x3ff, 0);
#endif

#endif

#ifndef XBOX
		hInstance = hModule;
#endif

#if 0
#if !defined(KERNEL)
        //
        // get helpful info
        //

        GetSystemInfo(&SystemInfo);
        OsVersionInfo.dwOSVersionInfoSize = sizeof(OsVersionInfo);
        GetVersionEx(&OsVersionInfo);

#if defined (TARGET_MIPS)

        if (OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
            (OsVersionInfo.dwBuildNumber & 0xffff) > 1144) {
            MipsContextSize = Ctx64Bit;
        } else {
            MipsContextSize = Ctx32Bit;
        }

#endif // TARGET_MIPS

#if 0
        FChicago = ((GetVersion() & 0x80000000) && ((GetVersion() & 0xff)>3));
#endif // TARGET_i386

#endif // KERNEL
#endif

        break;
    }

#ifdef XBOX
	return 0;
#else
    return TlDllMain(hModule, dwReason, dwReserved);
#endif
}

#if 0
BOOL
PASCAL
IsChicago(
    VOID
    )
/*++

Routine Description:

    This routine allows the caller to determine if the DM is running on
    the Win95 family

Return Value:

    TRUE if running on Win95 and FALSE othewise.

--*/
{
    return FChicago;
}
#endif

#ifndef XBOX

BOOL
PASCAL
DmDllInit(
    LPDBF  lpb
    )

/*++

Routine Description:

    This routine allows the shell (debugger or remote stub)
    to provide a service callback vector to the DM.

Arguments:

    lpb - Supplies an array of functions for callbacks

Return Value:

    TRUE if successfully initialized and FALSE othewise.

--*/

{
    lpdbf = lpb;
    return TRUE;
}                                   /* DmDllInit() */
#endif


#ifdef KERNEL
void
ParseDmParams(
    LPTSTR p
    )
{
    DWORD                       i;
    CHAR                        szPath[MAX_PATH];
    CHAR                        szStr[_MAX_PATH];
    LPTSTR                       lpPathNext;
    LPTSTR                       lpsz1;
    LPTSTR                       lpsz2;
    LPTSTR                       lpsz3;


    for (i=0; i<MAX_MODULEALIAS; i++) {
        if (!ModuleAlias[i].Special) {
            ZeroMemory( &ModuleAlias[i], sizeof(MODULEALIAS) );
        }
    }

    do {
        p = _ftcstok( p, _T("=") );
        if (p) {
            for (i=0; i<MAXKDOPTIONS; i++) {
                if (_ftcsicmp(KdOptions[i].keyword,p)==0) {
                    break;
                }
            }
            if (i < MAXKDOPTIONS) {
                p = _ftcstok( NULL, _T(" ") );
                if (p) {
                    switch (KdOptions[i].typ) {
                        case KDT_DWORD:
                            KdOptions[i].value = atol( p );
                            break;

                        case KDT_STRING:
                            KdOptions[i].value = (DWORD) _ftcsdup( p );
                            break;
                    }
                    p = p + (_ftcslen(p) + 1);
                }
            } else {
                if (_ftcsicmp( p, _T("alias") ) == 0) {
                    p = _ftcstok( NULL, _T("#") );
                    if (p) {
                        for (i=0; i<MAX_MODULEALIAS; i++) {
                            if (ModuleAlias[i].ModuleName[0] == 0) {
                                break;
                            }
                        }
                        if (i < MAX_MODULEALIAS) {
                            _ftcscpy( ModuleAlias[i].ModuleName, p );
                            p = _ftcstok( NULL, _T(" ") );
                            if (p) {
                                _ftcscpy( ModuleAlias[i].Alias, p );
                                p = p + (_ftcslen(p) + 1);
                            }
                        } else {
                            p = _ftcstok( NULL, _T(" ") );
                        }
                    }
                } else {
                    p = _ftcstok( NULL, _T(" ") );
                }
            }
        }
    } while(p && *p);

    if (KdOptions[KDO_VERBOSE].value > 1) {
        FVerbose = KdOptions[KDO_VERBOSE].value;
    }
    else {
        FVerbose = 0;
    }

    szPath[0] = 0;
    lpPathNext = _ftcstok((LPTSTR)KdOptions[KDO_SYMBOLPATH].value, _T(";"));
    while (lpPathNext) {
        lpsz1 = szStr;
        while ((lpsz2 = _ftcschr(lpPathNext, _T('%'))) != NULL) {
            _ftcsncpy(lpsz1, lpPathNext, lpsz2 - lpPathNext);
            lpsz1 += lpsz2 - lpPathNext;
            lpsz2++;
            lpPathNext = _ftcschr(lpsz2, _T('%'));
            if (lpPathNext != NULL) {
                *lpPathNext++ = 0;
                lpsz3 = getenv(lpsz2);
                if (lpsz3 != NULL) {
                    _ftcscpy(lpsz1, lpsz3);
                    lpsz1 += _ftcslen(lpsz3);
                }
            } else {
                lpPathNext = _T("");
            }
        }
        _ftcscpy(lpsz1, lpPathNext);
        _ftcscat( szPath, szStr );
        _ftcscat( szPath, _T(";") );
        lpPathNext = _ftcstok(NULL, _T(";"));
    }

    if ( szPath[0] != 0 ) {
        if (szPath[_ftcslen(szPath)-1] == _T(';')) {
            szPath[_ftcslen(szPath)-1] = _T('\0');
        }
        _ftcscpy( (LPTSTR)KdOptions[KDO_SYMBOLPATH].value, szPath );
    }
}


VOID
ProcessRemoteQuit(
    VOID
    )
{
    HPRCX      hprc;
    PBREAKPOINT pbp;
    PBREAKPOINT pbpT;


    EnterCriticalSection(&csThreadProcList);

    for(hprc=prcList->next; hprc; hprc=hprc->next) {
        for (pbp = BPNextHprcPbp(hprc, NULL); pbp; pbp = pbpT) {
            pbpT = BPNextHprcPbp(hprc, pbp);
            RemoveBP(pbp);
        }
    }

    LeaveCriticalSection(&csThreadProcList);

    fDisconnected = TRUE;
    ResetEvent( hEventRemoteQuit );
}

#endif  // KERNEL



XOSD FAR PASCAL
DMInit(
    DMTLFUNCTYPE lpfnTl,
    LPTSTR        lpch
    )
/*++

Routine Description:

    This is the entry point called by the TL to initialize the
    connection from DM to TL.

Arguments:

    lpfnTl  - Supplies entry point to TL

    lpch    - Supplies command line arg list

Return Value:

    XOSD value: xosdNone for success, other values reflect reason
    for failure to initialize properly.

--*/
{
    int i, n;
    XOSD xosd;

    DEBUG_PRINT(_T("DMInit\r\n"));

    if (lpfnTl != NULL) {

#if 0
		/*
         **  Parse out anything interesting from the command line args
         */

        while (*lpch) {

            while (isspace(*lpch)) {
                lpch++;
            }

            if (*lpch != _T('/') && *lpch != _T('-')) {
                break;
            }

            lpch++;

            switch (*lpch++) {

              case 0:   // don't skip over end of string
                --lpch;

              default:  // assert, continue is ok.
                assert(FALSE);
                break;


              case 'v':
              case 'V':

                while (isspace(*lpch)) {
                    lpch++;
                }
                FVerbose = atoi(lpch);
                while (isdigit(*lpch)) {
                    lpch++;
                }
                break;

              case 'r':
              case 'R':
                FDMRemote = TRUE;
                break;

              case 'd':
              case 'D':
                FUseOutputDebugString = TRUE;
                break;

            }
        }
#endif // 0
#ifdef KERNEL
        ParseDmParams( lpch );
#endif


        /* Define a false single step event */
        falseSSEvent.dwDebugEventCode = EXCEPTION_DEBUG_EVENT;
        falseSSEvent.u.Exception.ExceptionRecord.ExceptionCode
          = EXCEPTION_SINGLE_STEP;

        falseBPEvent.dwDebugEventCode = BREAKPOINT_DEBUG_EVENT;
        falseBPEvent.u.Exception.ExceptionRecord.ExceptionCode
          = EXCEPTION_BREAKPOINT;

        FuncExitEvent.dwDebugEventCode = FUNC_EXIT_EVENT;
        FuncExitEvent.u.Exception.ExceptionRecord.ExceptionCode
          = EXCEPTION_SINGLE_STEP;

        /* Define the standard notification method */
        EMNotifyMethod.notifyFunction = ConsumeThreadEventsAndNotifyEM;
        EMNotifyMethod.lparam     = (LPVOID)0;

        SearchPathString[0] = _T('\0');
        SearchPathSet       = FALSE;

        InitEventQueue();
		InitDEQ();

        //
        // initialize data breakpoint handler
        //
        ExprBPInitialize();

		// Xbox has known hardware
        g_ProcFlags = PROCESSOR_FLAGS_EXTENDED | PROCESSOR_FLAGS_I386_MMX |
			PROCESSOR_FLAGS_I386_MMX2 | PROCESSOR_FLAGS_I386_MMX3;

#if 0
#ifdef TARGET_i386
{
        DWORD mmx = 0, mmx2 = 0, mmx3 = 0, threednow = 0;

        _asm
        {
            push ebx
            push ecx
            push edx

            pushfd
            pop eax
            mov ebx,eax
            xor eax,00200000h
            push eax
            popfd
            pushfd
            pop eax
            cmp eax,ebx
            jz nocpuid

            mov eax, 1  ;
            cpuid

            mov eax,edx ;
            shr edx,23  ;   //MMX
            and edx,1   ;
            mov mmx,edx ;

            mov edx,eax ;
            shr edx,25  ;   //Katmai
            and edx,1   ;
            mov mmx2,edx;   

            mov edx,eax	;
            shr edx,26	;	//Willamette
            and edx,1	;
            mov mmx3,edx;	

            mov eax,80000000h
            cpuid
            cmp eax,80000000h
            jbe notamd
            mov eax,80000001h
            cpuid
            shr edx,31
            and edx,1
            mov threednow,edx
notamd:
nocpuid:
            pop edx
            pop ecx
            pop ebx
        }

        if (mmx == 1) {
            //Old MMX           
            g_ProcFlags |= PROCESSOR_FLAGS_I386_MMX;
        }

        if (threednow) {
            // 3DNow
            g_ProcFlags |= PROCESSOR_FLAGS_I386_3DNOW;
        }

		if (mmx2) {
			//
			// v-vadimp - On NT4 or Win9x try executing FXSAVE to see if the OS supports it 
			// (instrumental for the hack to get extended registers), then check the registry - 
			// we only support the hack if the user explicitly requested it by creating the above 
			// registry key. Set FXSAVESupport accordingly. Win2K supports the extended registers 
			// through the regular Get/SetContext calls
			//
#if 0 // ignore this code until I know what it does
			if (FChicago || !(!(GetVersion() & 0x80000000) && ((GetVersion() & 0xff) == 5))) {
				// We need to have SIMD support in the CPU and in the OS task switcher
				// See Intel AP-900 note for details
				// (this currently means NT4 SP5 and later, or Win98)
				DWORD dwVal;
				DWORD dwLen=4;
				DWORD dwType;
				HKEY hkey = NULL;
				BOOL bDoHack = (
					(RegOpenKey(HKEY_CURRENT_USER, EXTENDED_REGISTERS_SUPPORT_KEY, &hkey)==ERROR_SUCCESS) &&
					(RegQueryValueEx(hkey, "SIMDRegisters", NULL, &dwType, (LPBYTE)&dwVal, &dwLen)==ERROR_SUCCESS) &&
					dwVal && 
					(dwType==REG_DWORD)
					);
				if (bDoHack)
				{
					__try
					{
						// xorps xmm0,xmm0
						_asm _emit 0Fh
						_asm _emit 57h
						_asm _emit 0C0h
					}
					__except(EXCEPTION_EXECUTE_HANDLER)
					{
						bDoHack = FALSE;
					}
				}
				FXSAVESupported = bDoHack;
				if (!bDoHack)
				{
					mmx2 = 0;				// disable KNI regs if we can't get to them
					mmx3 = 0;
				}
				if (hkey)
					RegCloseKey(hkey);
			}
#endif // 0

			if (mmx2 == 1) {
				//Katmai
				g_ProcFlags |= (PROCESSOR_FLAGS_EXTENDED | PROCESSOR_FLAGS_I386_MMX2);
			}
        
			if (mmx3 == 1) {
				//Willamette
				g_ProcFlags |= (PROCESSOR_FLAGS_EXTENDED | PROCESSOR_FLAGS_I386_MMX3);
			}
		}
}
#endif
#endif // 0

        // SetDebugErrorLevel(SLE_WARNING);

        /*
         **  Save the pointer to the Transport layer entry function
         */

        DmTlFunc = lpfnTl;

        /*
         **  Try and connect up to the other side of the link
         */

        DmTlFunc( tlfSetBuffer, NULL, sizeof(abEMReplyBuf), (LONG)(LPV) abEMReplyBuf );

        if ((xosd = DmTlFunc( tlfConnect, NULL, 0, 0)) != xosdNone ) {
            return(xosd);
        }

        DPRINT(10, (_T("DM & TL are now connected\n")));

    } else {

        DmTlFunc( tlfDisconnect, NULL, 0, 0);
        DmTlFunc( tlfSetBuffer, NULL, 0, 0);
        FDMRemote = FALSE;
        DmTlFunc = (DMTLFUNCTYPE) NULL;

    }

    return xosdNone;
}                               /* DmInit() */



#ifndef KERNEL
VOID
Cleanup(
    VOID
    )
/*++

Routine Description:

    Cleanup of DM, prepare for exit.

Arguments:

    None

Return Value:

    None

--*/
{
    HTHDX           pht, phtt;
    HPRCX           php, phpt;
    BREAKPOINT      *bp, *bpt;
    int             iDll;


    /* Free all threads and close their handles */
    for (pht = thdList->next; pht; pht = phtt) {
        phtt = pht->next;
        if (pht->rwHand != (HANDLE)INVALID) {
            CloseHandle(pht->rwHand);
        }
        MHFree(pht);
    }
    thdList->next = NULL;


    /* Free all processes and close their handles */
	
    for(php = prcList->next; php; php = phpt) {
        phpt = php->next;

        RemoveExceptionList(php);
        // Free any fibers that may be left
        RemoveFiberList(php);

        for (iDll = 0; iDll < php->cDllList; iDll++) {
            DestroyDllLoadItem(&php->rgDllList[iDll]);
        }
        MHFree(php->rgDllList);

        if (php->rwHand != (HANDLE)INVALID) {
            CloseHandle(php->rwHand);
        }
        CloseHandle(php->hEventCreateThread);
		DeleteTimerEventQueue (&php->TimerQueue);
        MHFree(php);
    }
    prcList->next = NULL;

    /* Free all breakpoints */
    for(bp = bpList->next; bp; bp = bpt) {
        bpt = bp->next;
        MHFree(bp);
    }
    bpList->next = NULL;


#if 0
    // Ask the disassembler to clean itself up.
    CleanupDisassembler( );

    if (hModPsapi) {
        FreeLibrary(hModPsapi);
        hModPsapi = NULL;
        fAttemptedLoadingPsapi = FALSE;
        pFnGetModuleFileNameExA = NULL;
    }
#endif

    if (hDmPollThread) {
        fDmPollQuit = TRUE;
        WaitForSingleObject(hDmPollThread, INFINITE);
		CloseHandle(hDmPollThread);
        hDmPollThread = 0;
    }

}
#endif  // !KERNEL


#ifndef XBOX
BOOL
WINAPIV
DMPrintShellMsgEx(
	DWORD	ids,
	...
	)
{
	TCHAR		szFormat [512];
	TCHAR		buf[512];
    DWORD		bufLen;
    va_list		marker;
    LPINFOAVAIL lpinf;
    LPRTP		lprtp = NULL;
    BOOL		rVal = TRUE;

	if (!LoadString (hInstance, ids, szFormat, sizeof (szFormat))) {
		assert (FALSE);
		return FALSE;
	}

    va_start( marker, ids );
    bufLen = _vsnprintf(buf, sizeof(buf), szFormat, marker );
    va_end( marker);

    if (bufLen == -1) {
        buf[sizeof(buf) - 1] = _T('\0');
    }

    __try {
        bufLen   = _ftcslen(buf) + 1;
        lprtp    = (LPRTP) MHAlloc( FIELD_OFFSET(RTP, rgbVar)+sizeof(INFOAVAIL)+bufLen );
        lpinf    = (LPINFOAVAIL)(lprtp->rgbVar);

        lprtp->dbc  = dbcInfoAvail;
        lprtp->hpid = hpidRoot;
        lprtp->htid = NULL;
        lprtp->cb   = (int)bufLen;

        lpinf->fReply    = FALSE;
        lpinf->fUniCode  = FALSE;
        memcpy( lpinf->buffer, buf, bufLen );

        DmTlFunc( tlfDebugPacket,
                  lprtp->hpid,
                  (FIELD_OFFSET(RTP, rgbVar)+sizeof(INFOAVAIL)+bufLen),
                  (LONG)(LPV) lprtp
                );

    } __except(EXCEPTION_EXECUTE_HANDLER) {

        rVal = FALSE;

    }

    if (lprtp) {
       MHFree( lprtp );
    }

    return rVal;
}
#endif
	
BOOL
WINAPIV
DMPrintShellMsg(
    LPTSTR szFormat,
    ...
    )
/*++

Routine Description:

   This function prints a string on the shell's
   command window.

Arguments:

    szFormat    - Supplies format string for sprintf

    ...         - Supplies variable argument list

Return Value:

    TRUE      -> all is ok and the string was printed
    FALSE     -> something's hosed and no string printed

--*/
{
    TCHAR     buf[512];
    DWORD    bufLen;
    va_list  marker;
    LPINFOAVAIL lpinf;
    LPRTP    lprtp = NULL;
    BOOL     rVal = TRUE;

    va_start( marker, szFormat );
    bufLen = _vsnprintf(buf, sizeof(buf), szFormat, marker );
    va_end( marker);

    if (bufLen == -1) {
        buf[sizeof(buf) - 1] = _T('\0');
    }

    __try {
        bufLen   = _ftcslen(buf) + 1;
        lprtp    = (LPRTP) MHAlloc( FIELD_OFFSET(RTP, rgbVar)+sizeof(INFOAVAIL)+bufLen );
        lpinf    = (LPINFOAVAIL)(lprtp->rgbVar);

        lprtp->dbc  = dbcInfoAvail;
        lprtp->hpid = hpidRoot;
        lprtp->htid = NULL;
        lprtp->cb   = (int)bufLen;

        lpinf->fReply    = FALSE;
        lpinf->fUniCode  = FALSE;
        memcpy( lpinf->buffer, buf, bufLen );

        DmTlFunc( tlfDebugPacket,
                  lprtp->hpid,
                  (FIELD_OFFSET(RTP, rgbVar)+sizeof(INFOAVAIL)+bufLen),
                  (LONG)(LPV) lprtp
                );

    } __except(EXCEPTION_EXECUTE_HANDLER) {

        rVal = FALSE;

    }

    if (lprtp) {
       MHFree( lprtp );
    }

    return rVal;
}

void
WINAPI
_DbgTrace(
	ULONG	Flag,
	LPCTSTR	szFormat,
	...
	)
{
    va_list  marker;
    int n;

	if (gTraceFlag & Flag) {

		va_start( marker, szFormat );
		n = _vsnprintf(rgchDebug, _tsizeof(rgchDebug), szFormat, marker );
		va_end( marker);

		if (n == -1) {
			rgchDebug[_tsizeof(rgchDebug)-1] = 0;
		}

		OutputDebugString( rgchDebug );
	}
}
	
VOID
WINAPIV
DebugPrint(
    LPTSTR szFormat,
    ...
    )
{
    va_list  marker;
    int n;

    va_start( marker, szFormat );
    n = _vsnprintf(rgchDebug, _tsizeof(rgchDebug), szFormat, marker );
    va_end( marker);

    if (n == -1) {
        rgchDebug[_tsizeof(rgchDebug)-1] = 0;
    }

    OutputDebugString( rgchDebug );
}

#if 0
BOOL
InitializeDump(
	IN	LPSTR				szAppName,
	OUT	PCONTEXT*			Context,
	OUT	PEXCEPTION_RECORD*	Exception,
	OUT	PVOID*				DmpHeader
	)
{
	CRASHDUMP_VERSION_INFO	CrashVersionInfo = {0};

	if (!DmpInitialize (szAppName, Context, Exception, DmpHeader)) {
		return FALSE;
	}

	if (!DmpDetectVersionParameters (&CrashVersionInfo)) {
		return FALSE;
	}

	//
	// If DmpDetectVersionParameters succeeded, it automatically initialized
	// the version parameters.
	//
	
	return TRUE;
}
	

int
pCharMode(
    LPTSTR        szAppName,
    PIMAGETYPE    pImageType
    )
/*++

Routine Description:

    This routine is used to determine the type of exe which we are going
    to be debugging.  This is decided by looking for exe headers and making
    decisions based on the information in the exe headers.

Arguments:

    szAppName  - Supplies the path to the debugger exe

    pImageType - Returns the type of the image

Return Value:

    System_Invalid     - could not find the exe file
    System_GUI         - GUI application
    System_Console     - console application

--*/

{
    IMAGE_DOS_HEADER    dosHdr;
    IMAGE_OS2_HEADER    os2Hdr;
    IMAGE_NT_HEADERS    ntHdr;
    DWORD               cb;
    HANDLE              hFile;
    int                 ret;
    BOOL                GotIt;
    _tcscpy(nameBuffer, szAppName);

    // don't use OpenFile as it fails paths >127 bytes long

    hFile = CreateFile( szAppName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL, NULL );

    if (hFile == (HANDLE)-1) {

        /*
         *      Could not open file!
         */

        DEBUG_PRINT_2(_T("CreateFile(%s) --> %u\r\n"), szAppName, GetLastError());
        return System_Invalid;

    }

    /*
     *  Try and read an MZ Header.  If you can't then it can not possibly
     *  be a legal exe file.  (Not strictly true but we will ignore really
     *  short com files since they are unintersting).
     */

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    if ((!ReadFile(hFile, &dosHdr, sizeof(dosHdr), &cb, NULL)) ||
        (cb != sizeof(dosHdr))) {

        if (_ftcsicmp(&szAppName[_ftcslen(szAppName) - 4], _T(".COM")) == 0) {
            *pImageType = Image_16;
        } else {
            DPRINT(1, (_T("dosHdr problem.\n")));
            *pImageType = Image_Unknown;
        }

        CloseHandle(hFile);
        return System_GUI;

    }

    /*
     *  Verify the MZ header.
     *
     *  NOTENOTE        Messup the case of no MZ header.
     */

    if (dosHdr.e_magic != IMAGE_DOS_SIGNATURE) {
        /*
         *  We did not start with the MZ signature.  If the extension
         *      is .COM then it is a COM file.
         */

        if (_ftcsicmp(&szAppName[_ftcslen(szAppName) - 4], _T(".COM")) == 0) {
            *pImageType = Image_16;
        } else {
            DPRINT(1, (_T("MAGIC problem(MZ).\n")));
            *pImageType = Image_Unknown;
        }

        CloseHandle(hFile);
#ifndef KERNEL
        if (InitializeDump ( szAppName, &CrashContext, &CrashException, &CrashDumpHeader )) {
            if (
#if defined(TARGET_i386)
                CrashDumpHeader->MachineImageType != IMAGE_FILE_MACHINE_I386
#elif defined(TARGET_MIPS)
                CrashDumpHeader->MachineImageType != IMAGE_FILE_MACHINE_R4000 ||
                CrashDumpHeader->MachineImageType != IMAGE_FILE_MACHINE_R10000
#elif defined(TARGET_ALPHA)
                CrashDumpHeader->MachineImageType != IMAGE_FILE_MACHINE_ALPHA
#elif defined(TARGET_PPC)
                CrashDumpHeader->MachineImageType != IMAGE_FILE_MACHINE_POWERPC
#else
#error( "unknown target machine" );
#endif
                                    ) {
                return System_Invalid;
            }
            *pImageType = Image_Dump;
        }
#endif  // !KERNEL
        return System_Console;
    }

    if ( dosHdr.e_lfanew == 0 ) {
        /*
         *  Straight DOS exe.
         */

        DPRINT(1, (_T("[DOS image].\n")));
        *pImageType = Image_16;

        CloseHandle(hFile);
        return System_Console;
    }

    /*
     *  Now look at the next EXE header (either NE or PE)
     */

    SetFilePointer(hFile, dosHdr.e_lfanew, NULL, FILE_BEGIN);
    GotIt = FALSE;
    ret = System_GUI;

    /*
     *  See if this is a Win16 program
     */

    if (ReadFile(hFile, &os2Hdr, sizeof(os2Hdr), &cb, NULL)  &&
        (cb == sizeof(os2Hdr))) {

        if ( os2Hdr.ne_magic == IMAGE_OS2_SIGNATURE ) {
            /*
             *  Win16 program  (may be an OS/2 exe also)
             */

            DPRINT(1, (_T("[Win16 image].\n")));
            *pImageType = Image_16;
            GotIt  = TRUE;
        } else if ( os2Hdr.ne_magic == IMAGE_OS2_SIGNATURE_LE ) {
            /*
             *  OS2 program - Not supported
             */

            DPRINT(1, (_T("[OS/2 image].\n")));
            *pImageType = Image_Unknown;
            GotIt  = TRUE;
        }
    }

    /*
     *  If the above failed, see if it is an NT program
     */

    if ( !GotIt ) {
        SetFilePointer(hFile, dosHdr.e_lfanew, NULL, FILE_BEGIN);

        if (ReadFile(hFile, &ntHdr, sizeof(ntHdr), &cb, NULL) &&
            (cb == sizeof(ntHdr))                             &&
            (ntHdr.Signature == IMAGE_NT_SIGNATURE)) {
            /*
             *  All CUI (Character user interface) subsystems
             *  have the lowermost bit set.
             */

            DPRINT(1, ((ntHdr.OptionalHeader.Subsystem & 1) ?
                       _T("[*Character mode app*]\n") : _T("[*Windows mode app*]\n")));

            ret = ((ntHdr.OptionalHeader.Subsystem & 1)) ?
              System_Console : System_GUI;
            *pImageType = Image_32;
        } else {
            DWORD   FileSize;

            FileSize = SetFilePointer(hFile, 0, NULL, FILE_END);

            if ( (DWORD)dosHdr.e_lfanew > FileSize ) {
                //
                //  Bogus e_lfanew, assume DOS
                //
                DPRINT(1, (_T("[DOS image assumed].\n")));
                *pImageType = Image_16;
                ret =  System_Console;

            } else {

                //
                //  Not an NT image.
                //
                DPRINT(1, (_T("MAGIC problem(PE).\n")));
                *pImageType = Image_Unknown;
            }
        }
    }

    CloseHandle(hFile);
    return ret;
}                               /* pCharMode() */
#endif


VOID
ReConnectDebugger(
    DEBUG_EVENT *lpde,
    BOOL        fNoDllLoad
    )

/*++

Routine Description:

    This function handles the case where the dm/tl is re-connected to
    a debugger.  This function must re-instate the debugger to the
    correct state that existed before the disconnect action.

    (wesw) 11-3-93

Arguments:

    None.

Return Value:

    None.

--*/

{
    DWORD            i;
    DEBUG_EVENT      de;
    HPRCX            hprc;
    HTHDX            hthd;
    HTHDX            hthd_lb;
    DWORD            id;
    HANDLE           hThread;
    HPID             hpidNext = hpidRoot;
    BOOL             fException = FALSE;


    //
    // the dm is now connected
    //
    fDisconnected = FALSE;

    //
    // check to see if a re-connection is occurring while the
    // process is running or after a non-servicable debug event
    //
    if (lpde && lpde->dwDebugEventCode == EXCEPTION_DEBUG_EVENT) {

        hprc = HPRCFromPID(lpde->dwProcessId);
        hthd = HTHDXFromPIDTID((PID)lpde->dwProcessId,(TID)lpde->dwThreadId);

        if (lpde->u.Exception.dwFirstChance) {
            hthd->tstate |= ts_first;
        } else {
            hthd->tstate |= ts_second;
        }

        hthd->tstate &= ~ts_running;
        hthd->tstate |= ts_stopped;
    }

    //
    // generate a create process event
    //
    hprc=prcList->next;
    hprc->hpid = hpidNext;
    hpidNext = (HPID) INVALID;
    hthd=hprc->hthdChild;
    ResetEvent(hEventCreateProcess);
    de.dwDebugEventCode = CREATE_PROCESS_DEBUG_EVENT;
    de.dwProcessId = hprc->pid;
    de.dwThreadId = hthd->tid;
    de.u.CreateProcessInfo.hFile = NULL;
    de.u.CreateProcessInfo.hProcess = hprc->rwHand;
    de.u.CreateProcessInfo.hThread = hthd->rwHand;
    de.u.CreateProcessInfo.lpBaseOfImage = (LPVOID)hprc->rgDllList[0].offBaseOfImage;
    de.u.CreateProcessInfo.dwDebugInfoFileOffset = 0;
    de.u.CreateProcessInfo.nDebugInfoSize = 0;
    de.u.CreateProcessInfo.lpStartAddress = (LPVOID)(DWORD)PC(hthd);
    de.u.CreateProcessInfo.lpThreadLocalBase = NULL;
    de.u.CreateProcessInfo.lpImageName = NULL;
    de.u.CreateProcessInfo.fUnicode = 0;
    NotifyEM(&de, hthd, 0, hprc);
    WaitForSingleObject(hEventCreateProcess, INFINITE);

    //
    // mark the process as 'being connected' so that the continue debug
    // events that are received from the shell are ignored
    //
    hprc->pstate |= ps_connect;


    //
    // look for a thread that is stopped and not dead
    //
    for (hthd=hprc->hthdChild,hthd_lb=NULL; hthd; hthd=hthd->nextSibling) {
        if ((!(hthd->tstate & ts_dead)) && (hthd->tstate & ts_stopped)) {
            hthd_lb = hthd;
            break;
        }
    }

    if (hthd_lb == NULL) {
        //
        // if we get here then there are no threads that are stopped
        // so we must look for the first alive thread
        //
        for (hthd=hprc->hthdChild,hthd_lb=NULL; hthd; hthd=hthd->nextSibling) {
            if (!(hthd->tstate & ts_dead)) {
                hthd_lb = hthd;
                break;
            }
        }
    }

    if (hthd_lb == NULL) {
        //
        // if this happens then we are really screwed.  there are no valid
        // threads to use, so lets bail out.
        //
        return;
    }

    if ((hthd_lb->tstate & ts_first) || (hthd_lb->tstate & ts_second)) {
        fException = TRUE;
    }

    //
    // generate mod loads for all the dlls for this process
    //
    // this MUST be done before the thread creates because the
    // current PC of each thread can be in any of the loaded
    // modules.
    //
    hthd = hthd_lb;
    if (!fNoDllLoad) {
        for (i=0; i<(DWORD)hprc->cDllList; i++) {
            if (hprc->rgDllList[i].fValidDll) {
                de.dwDebugEventCode        = LOAD_DLL_DEBUG_EVENT;
                de.dwProcessId             = hprc->pid;
                de.dwThreadId              = hthd->tid;
                de.u.LoadDll.hFile         = NULL;
                de.u.LoadDll.lpBaseOfDll   = (LPVOID)hprc->rgDllList[i].offBaseOfImage;
                de.u.LoadDll.lpImageName   = hprc->rgDllList[i].szDllName;
                de.u.LoadDll.fUnicode      = FALSE;
                NotifyEM(&de, hthd, 0, hprc);
            }
        }
    }


    //
    // loop thru all the threads for this process and
    // generate a thread create event for each one
    //
    for (hthd=hprc->hthdChild; hthd; hthd=hthd->nextSibling) {
        if (!(hthd->tstate & ts_dead)) {
            if (fException && hthd_lb == hthd) {
                //
                // do this one last
                //
                continue;
            }

            //
            // generate a thread create event
            //
            ResetEvent( hprc->hEventCreateThread );
            ResetEvent( hEventContinue );
            de.dwDebugEventCode = CREATE_THREAD_DEBUG_EVENT;
            de.dwProcessId = hprc->pid;
            de.dwThreadId = hthd->tid;
            NotifyEM( &de, hthd, 0, hprc );

            WaitForSingleObject( hprc->hEventCreateThread, INFINITE );

            //
            // wait for the shell to continue the new thread
            //
            WaitForSingleObject( hEventContinue, INFINITE );
        }
    }

    if (fException) {
        hthd = hthd_lb;
        //
        // generate a thread create event
        //
        ResetEvent( hprc->hEventCreateThread );
        ResetEvent( hEventContinue );
        de.dwDebugEventCode = CREATE_THREAD_DEBUG_EVENT;
        de.dwProcessId = hprc->pid;
        de.dwThreadId = hthd->tid;
        NotifyEM( &de, hthd, 0, hprc );

        WaitForSingleObject( hprc->hEventCreateThread, INFINITE );

        //
        // wait for the shell to continue the new thread
        //
        WaitForSingleObject( hEventContinue, INFINITE );
    }

    //
    // generate a breakpoint event
    //
    hthd = hthd_lb;

    if (hthd->tstate & ts_running) {

#if 0
        //
        // this will create a thread in the debuggee that will
        // immediatly stop at a breakpoint.  this will cause the
        // shell to think that we are processing a normal attach.
        //

        HMODULE hModule = GetModuleHandle("ntdll.dll");
        FARPROC ProcAddr = GetProcAddress(hModule, "DbgBreakPoint" );


        hThread = CreateRemoteThread( (HANDLE) hprc->rwHand,
                                      NULL,
                                      4096,
                                      (LPTHREAD_START_ROUTINE) ProcAddr,
                                      0,
                                      0,
                                      &id
                                    );

#endif
		DmHaltThread(0);

    } else if (!lpde) {

        de.dwProcessId                  = hprc->pid;
        de.dwThreadId                   = hthd->tid;
        if ((hthd->tstate & ts_first) || (hthd->tstate & ts_second)) {
            de.dwDebugEventCode         = EXCEPTION_DEBUG_EVENT;
        } else {
            de.dwDebugEventCode         = BREAKPOINT_DEBUG_EVENT;
        }
        de.u.Exception.dwFirstChance    = hthd->tstate & ts_first;
        de.u.Exception.ExceptionRecord  = hthd->ExceptionRecord;
        NotifyEM(&de, hthd, 0, 0);

    }

    //
    // reset the process state
    //
    hprc->pstate &= ~ps_connect;

    return;
}

/*
 * HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK
 *
 * In NT 3.x, it is impossible for a GUI app to spawn a console
 * app and have one or two of the console app's standard handles
 * redirected away from the console; you can only do it with
 * either none of the handles redirected, or all three of them.
 * See also the struct STARTUPINFO in the Win32 Programmer's Reference
 *
 * However, I learned by experimentation that it just so happens
 * that there are a few magic values I can pass in for stdin,
 * stdout, and stderr, which will have the desired effect of
 * leaving that handle attached to the console.  For example,
 * if I pass ((HANDLE)3) for stdin, stdin will stay attached
 * to the console.  stdout is ((HANDLE)7), and stderr is
 * ((HANDLE)11)
 *
 * In UNIX, you would probably use 0, 1, and 2.  Perhaps it is no
 * conicidence that for x={0, 1, 2}, the magic handles are (x<<2 | 3)
 *
 * HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK
 */
HANDLE const rghMagic[3] = {(HANDLE)3, (HANDLE)7, (HANDLE)11};

/*
 * Close3Handles: helper function for I/O redirection
 */

void
Close3Handles(
    HANDLE *rgh
    )
{
    int iFile;

    for (iFile=0; iFile<3; ++iFile) {
        if (rgh[iFile] != 0 &&
             rgh[iFile] != INVALID_HANDLE_VALUE &&
             rgh[iFile] != rghMagic[iFile])
        {
             VERIFY(CloseHandle(rgh[iFile]));
             rgh[iFile] = 0;
        }
    }
}

#if 0
XOSD
ProcessDebuggeeRedirection(
LPTSTR lszCommandLine,
STARTUPINFO FAR * psi
)
/*++

Routine Description:

    Parse command line redirection

Arguments:

    szCommandLine           A string representing the command line.
    psi                     Where we leave our mark


Return Value:

    xosdNone                        If no error
    xosdIORedirBadFile      If we can't open the file
    xosdIORedirSyntax       If the redir syntax is bad

Notes:

    The redirection-related text is removed from lszCommandLine
    Redirection is a hack in NT
    psi -> dwFlags += STARTF+USESTDHANDLES
    psi -> hStd{Input,Output,Error} are set

--*/
{

    LPTSTR          lszArg;
    LPTSTR          lszBegin;
    LPTSTR          lszEnd;

    BOOL            fInQuote = FALSE;
    BOOL            fAppend;

    // iFile is 0,1,2 for std{in,out,err}
    int             iFile, iFileFrom;
    CHAR            ch;

    lszArg = lszCommandLine;

    while (*lszArg) {
        // skip over quoted text on command line
        if (*lszArg == '"') {
            fInQuote = !fInQuote;
        }
        if (fInQuote) {
            // If in quoted text, increment lszArg
            lszArg = _ftcsinc( lszArg );
            continue;
        }

        lszBegin = lszArg;

        // recognize leading digit for "2>blah", "0<blah", etc.
        // Put it in iFile
        if (*lszArg >= '0' && *lszArg <= '2') {
            // iFile holds the file descriptor
            iFile = *lszArg - '0';
            lszArg = _ftcsinc( lszArg );
            if (*lszArg == '\0') {
                break;
            }
        } else {
            // For 'foo.exe > bar.txt' (no file number), we'll figure it out later.
            iFile = -1;
        }

        // If there is redirection going on, process it.
        if (*lszArg == '<' || *lszArg == '>') {
            psi -> dwFlags |= STARTF_USESTDHANDLES;

            // if there was no explicit leading digit, figure out the
            // implicit one: 0 for "<", 1 for ">" or ">>"
            if (iFile == -1) {
                if (*lszArg == '<') {
                    iFile = 0;
                } else {
                    iFile = 1;
                }
            } else if (iFile == 0) {
                if (*lszArg == '>') {
                    Close3Handles(rgh);
#if 0 // The error handling upstairs is bad, so just don't bother redirecting.
                    return xosdIORedirSyntax;
#else
					rgh[iFile] = 0; // Don't redirect, then
#endif
                }
            } else {
                if (*lszArg == '<') {
                    Close3Handles(rgh);
#if 0 // The error handling upstairs is bad, so just don't bother redirecting.
                    return xosdIORedirSyntax;
#else
					rgh[iFile] = 0; // Don't redirect, then
#endif
                }
            }

            if (lszArg[0] == '>' && lszArg[1] == '>') {
                fAppend = TRUE;
                lszArg = _ftcsinc( lszArg );
            } else {
                fAppend = FALSE;
            }
            lszArg = _ftcsinc( lszArg );

            // deal with "2>&1" and so on
            if (*lszArg == '&') {
                lszArg = _ftcsinc( lszArg );

                while (*lszArg == ' ' || *lszArg == '\t') {
                    lszArg = _ftcsinc( lszArg );
                }

                // error conditions:
                //              1<&x    where ix not in [012]
                //              1<&1
                //              2>>&1
                if (*lszArg < '0' || *lszArg > '2' ||
                        *lszArg - '0' == iFile || fAppend) {
                    Close3Handles(rgh);
#if 0 // The error handling upstairs is bad, so just don't bother redirecting.
                    return xosdIORedirSyntax;
#else
					rgh[iFile] = 0; // Don't redirect, then
#endif
                }

                iFileFrom = *lszArg - '0';

                if (rgh[iFileFrom] == 0 ||
                        rgh[iFileFrom] == INVALID_HANDLE_VALUE) {
                    rgh[iFile] = rgh[iFileFrom];
                } else {
                    HANDLE hProcess = GetCurrentProcess();

                    if (!DuplicateHandle(
                                    hProcess, rgh[iFileFrom],
                                    hProcess, &rgh[iFile],
                                    0, TRUE, DUPLICATE_SAME_ACCESS))
                    {
                        Close3Handles(rgh);
#if 0 // The error handling upstairs is bad, so just don't bother redirecting.
                        return xosdIORedirBadFile;
#else
						rgh[iFile] = 0; // Don't redirect, then
#endif
                    }
                }
                lszArg = _ftcsinc( lszArg );    // get past last digit
            } else {
                static char rgchEndFilename[] = "\t \"&,;<=>";
                static SECURITY_ATTRIBUTES sa = {sizeof(sa), 0, TRUE};

                // skip blanks after "<" or ">"
                while (*lszArg == ' ' || *lszArg == '\t') {
                    ++lszArg;
                }

                // append null to szArg
                lszEnd = lszArg;
                while (*lszEnd && !_tcschr(rgchEndFilename, *lszEnd)) {
                    lszEnd = _ftcsinc( lszEnd );
                }
                ch = *lszEnd;
                *lszEnd = '\0';

                if (iFile) {

                    // std{out,err}
                    rgh[iFile] = CreateFile (
					  lszArg,
					  GENERIC_READ|GENERIC_WRITE,
					  FILE_SHARE_READ,
					  &sa,
					  fAppend ? OPEN_EXISTING : CREATE_ALWAYS,
					  FILE_ATTRIBUTE_NORMAL,
					  0);
				
                } else {
                    // stdin
                    rgh[iFile] = CreateFile (
                                        lszArg,
                                        GENERIC_READ,
                                        FILE_SHARE_READ|FILE_SHARE_WRITE,
                                        &sa,
                                        OPEN_EXISTING,
                                        FILE_ATTRIBUTE_NORMAL,
                                        0);
                }

                if (rgh[iFile] == INVALID_HANDLE_VALUE) {
                    //  // The file could not be opened as desired.
                    //  _ftcsncpy(szErrArg, lszArg, sizeof(szErrArg));
                    //  szErrArg[sizeof(szErrArg)-1] = '\0';
                    //  _ftcsncpy(szErrExe, lszExe, sizeof(szErrExe));
                    //  szErrExe[sizeof(szErrExe)-1] = '\0';

                    // restore byte after arg
                    *lszEnd = ch;

                    Close3Handles(rgh);
#if 0 // The error handling upstairs is bad, so just don't bother redirecting.
                    return xosdIORedirBadFile;
#else
					rgh[iFile] = 0; // Don't redirect, then
#endif
                }

                // restore byte after arg
                *lszEnd = ch;

                // if ">>", move to end of file
                if (fAppend) {
                    SetFilePointer(rgh[iFile], 0, NULL, FILE_END);
                }

                // advance lszArg to end of string
                lszArg = lszEnd;
            }

            // remove the redirection from the command line
            if (*lszArg == ' ' || *lszArg == '\t') {
                lszArg++;
            }
            _fmemmove(lszBegin, lszArg,
                        (_ftcslen(lszArg)+1) * sizeof(TCHAR));
            lszArg = lszBegin;
        } else {
            lszArg = _ftcsinc( lszArg );
        }

    } // while

    if (lszCommandLine[0] == ' ' && lszCommandLine[1] == '\0') {
        lszCommandLine[0] = '\0';
    }


    // If we're redirecting at all
    if (psi -> dwFlags & STARTF_USESTDHANDLES) {

        OSVERSIONINFO ver;


        ver.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

        GetVersionEx (&ver);

        for (iFile=0; iFile<3; ++iFile) {

            // If they're still unset.

            if (rgh[iFile] == 0) {
                // If we are using NT 3.x or greater,
                // use the hack magic handles. See comments
                // near the definition of rghMagic

                // Unluckily it looks like it is still needed for 4.0
                if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT) {
                    rgh[iFile] = rghMagic[iFile];
                } else {
                    rgh[iFile] = INVALID_HANDLE_VALUE;
                }
            }
        }

        psi -> hStdInput  = rgh[0];
        psi -> hStdOutput = rgh[1];
        psi -> hStdError  = rgh[2];
    }

    return xosdNone;
} //ProcessDebuggeeRedirection
#endif


VOID
SetExceptionAddress(
    DEBUG_EVENT*    pde,
    DWORD           addr
    )
{
    pde->u.Exception.ExceptionRecord.ExceptionAddress = (PVOID) addr;
}



#if DBG

VOID
DumpThdHeader(
    )
{
    DebugPrint ("hthd      htid\n");
}

VOID
DumpThread(
    HTHDX   hthd
    )
{
    if (hthd == NULL) {
        DebugPrint ("<Invalid hthd == NULL>\n");
    }

    DebugPrint ("%#x %#x\n", hthd, hthd->tid);
}


// hthd tid

VOID
DumpThreads(
    )
{
    HTHDX   hthd;
    
    DebugPrint ("\n");
    DumpThdHeader ();

    for (hthd = thdList->next; hthd; hthd = hthd->next) {
        DumpThread (hthd);
    }
}

#endif // DBG
    
