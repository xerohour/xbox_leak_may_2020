/*********************************************************************

    File:               makeeng.c

    Date created:       27/8/90

    Author:             Tim Bell

    Description:

    Windows Make Engine API

    Modified:

*********************************************************************/

// Ported to QCQP 10/2/91

#include "stdafx.h"
#pragma hdrstop

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <dos.h>
#include <io.h>
#include <errno.h>
#include <process.h>

#include "sqlprxky.h"
#include "sqlguid.h"
#include "sqlpkapi.h"
#include "autodbg.h"
#include "dbgenc.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// g_PidExecList is a list of pointers to PROCINFO structs for all
// processes which have been started with Project.Execute and
// are still running.  Here's why we keep this info:
//
// Say someone starts their program with Project.Execute, and then the
// app GP faults.  NT will start up a second copy of MSVC as an
// active-process debugger (assuming we've been registered as such).
// The second copy will see that there's already a running copy of MSVC,
// and will ask us if we spawned this PID.  We use g_PidExecList to
// check whether we spawned it.  If so, the new copy of MSVC is going
// to tell us to debug this app.
struct PROCINFO
{
    DWORD   pid;
    CPath   path;
};
CPtrList g_PidExecList;     // list of PROCESS_INFORMATION ptrs

// Return type for MkEngStartDebuggee
typedef enum
{
    SC_ATMAINORDEAD,    // debuggee is at entry point or has died
    SC_FAILED,          // attempt to start debuggee failed
    SC_GONE,            // debuggee running, couldn't find entry point
    SC_BPFAILED,        // attempt to set a breakpoint failed
    SC_STOPPED,         // debuggee stopped, e.g. BP or GPF before main()
} STARTCODE;

/* lifted from cmd.h / cmd.c */
/* Array of legitimate extensions for entries added to program list.
*/
#define CSZPLEXT_TOTAL    7
#define CSZPLEXT_OLDALLOWED 9
#define CSZPLEXT_SRC      6     // NUMBER of source files
#define CSZPLEXT_INC      6     // array POSITION of first include type file
#define CSZPLEXT_INC_MAX  6     // array POSITION of last include type file

static BOOL fForceEdit;     // A new list was specified in Set Program List
static BOOL fListWasNew;    // tested on Cancel from Edit Program List

BOOL fUpdateRemoteTarget;   // Indicates that we need to update the remote
                // target before debugging
#ifdef _DEBUG
BOOL g_bDebugTime = TRUE;   // report debug start times to the output window.
#else
BOOL g_bDebugTime = FALSE;
#endif

//
// Prototypes required for alloc_text pragma
//
void PASCAL ExecuteTarget(void);
STARTCODE PASCAL MkEngStartDebuggee(void);

// Java: prototypes for additional Java debugging support
BOOL GetJavaDebugInfo(BOOL bExecute = FALSE);

ULONG
GetWindowType(
	CView*	view
	)
/*++

Routine Description:

	Returns the type of the window.

--*/
{
	ULONG	WindowType = UNKNOWN_WIN;

	if (view)
	{
		if (view->IsKindOf (RUNTIME_CLASS (CMultiEdit)))
		{
			CMultiEdit* medit = (CMultiEdit*) view;

			WindowType = medit->m_dt;
		}
		else if (gpISrc && gpISrc->ViewBelongsToSrc (view))
		{
			WindowType = DOC_WIN;
		}
	}

	return WindowType;
}
		
	

ULONG
GetStepMode(
	)
/*++

Routine Description:

	The stepping mode is detirmined by the highest MDI window on the Z order
	that is either a source window or a disassembly window.  If the window
	is a disassembly window, then we are in disassembly mode, if the window
	is a source window, then we are in source mode, if there is no highest
	winow, we are in source mode.

Return Value:

	SRCSTEPPING for source mode; ASMSTEPPING for assembly mode.

--*/	
{

	CWnd*	mainFrm = theApp.m_pMainWnd;
	CWnd*	Wnd;

	ASSERT (theApp.m_pMainWnd->IsKindOf (RUNTIME_CLASS (CMDIFrameWnd)));
	
	CMDIFrameWnd*	mainFrame = (CMDIFrameWnd*)theApp.m_pMainWnd;
	CMDIChildWnd*	Frame = NULL;
	ULONG			WindowType;
	CView*			View;
	HWND			hWndForeground;

	View = GetCurView ();

	if (View)
	{
		WindowType = GetWindowType (View);

		switch (WindowType)
		{
			case DISASSY_WIN:
				return ASMSTEPPING;

			case DOC_WIN:
				return SRCSTEPPING;
		}
	}


	//
	//	The current view wasn't a source or debugger window
	//	look for the top view on the MDI list
	//
	
	for (Wnd = mainFrame->MDIGetActive ();
		 Wnd ;
		 Wnd = Wnd->GetWindow (GW_HWNDNEXT)
		 )
	{
		ULONG	WindowType; 

		if (Wnd->IsKindOf (RUNTIME_CLASS (CMDIChildWnd)))
			Frame = (CMDIChildWnd*) Wnd;

		if (!Frame)
			continue;

		View = Frame->GetActiveView ();
			
		WindowType = GetWindowType (View);
		
		switch (WindowType)
		{
			case DISASSY_WIN:
				return ASMSTEPPING;


			case DOC_WIN:
				return SRCSTEPPING;

		}

	}

	//
	//	If we've run through all the windows and none are either a source
	//	window or a disassembly window, then default to source mode stepping.
	//
		
	return SRCSTEPPING;
}

	
/*********************************************************************

    Function:           MkEngSetMakeDriveDir

    Description:

    Changes to the "Make" drive and directory.  This is either the
    directory where the make file is stored if one exists, otherwise
    it is the directory where the single-source file is stored
    (in the case of a Compile/Build/Rebuild command).

*********************************************************************/
BOOL PASCAL MkEngSetMakeDriveDir(void)
{
    char    szTargetDir[_MAX_DIR];
    CDir    ProjDir;
    INT     iType;

    if ( !gpIBldSys )
        return( FALSE );

    HBUILDER hBld;

    gpIBldSys->GetActiveBuilder(&hBld);

    if (hBld != NO_BUILDER && (gpIBldSys->GetBuilderType(ACTIVE_BUILDER, &iType) != ExeBuilder) )
    {
        LPCSTR pszPath;

        gpIBldSys->GetBuilderFile(ACTIVE_BUILDER, &pszPath);

        if ( ProjDir.CreateFromPath(pszPath) )
            _tcscpy(szTargetDir, (char *)(const char *)ProjDir);
        else
            return FALSE;
    }
    else
        return FALSE;

    return FsSetCurDir(szTargetDir);
}

/*********************************************************************

    Function:   DocInProject

    Description:

*********************************************************************/

BOOL PASCAL DocInProject(LPCTSTR docFileName)
{
    CPath path;

    if (!path.Create(docFileName))
        return(FALSE);

    if ( !gpIBldSys && gpIBldSys->IsActiveBuilderValid() != S_OK )
    {
        // if no project open the file can't be a member
        return(FALSE);
    }
    if ( gpIBldSys->IsProjectFile(&path, FALSE, ACTIVE_BUILDER) == S_OK )
        return(TRUE);

    return(FALSE);
}


/*********************************************************************

    Function:       FParentOfProcess

    Description:    Returns TRUE if we're the parent of the specified
                    process, FALSE if not.  This is used for
                    just-in-time debugging: if the user did Ctrl-F5
                    (Project.Execute), and then the app faults, then
                    the new copy of MSVC spawned by the system will
                    ask us if we are that process's parent.  If we
                    are, then it will let us be the process's debugger
                    rather than the new MSVC being the debugger.

*********************************************************************/

BOOL FParentOfProcess(DWORD pid, const char **pszPath)
// If pszPath is not NULL, it will be pointed to the process's path
{
    POSITION    p = g_PidExecList.GetHeadPosition();
    PROCINFO *  pprocinfo;

    while (p)
    {
        pprocinfo = (PROCINFO *) g_PidExecList.GetNext(p);

        if (pprocinfo->pid == pid)
        {
            if (pszPath)
                *pszPath = pprocinfo->path;
            return TRUE;
        }
    }

    return FALSE;
}

/*********************************************************************

    Function:       ClearPidExecList

    Description:    Clears the list of PIDs for which this instance
                    of the IDE will be the debugger.

*********************************************************************/

void ClearPidExecList(void)
{
    POSITION    p = g_PidExecList.GetHeadPosition();
    PROCINFO *  pprocinfo;

    while (p)
    {
        pprocinfo = (PROCINFO *) g_PidExecList.GetNext(p);
        delete pprocinfo;
    }

    g_PidExecList.RemoveAll();
}

/*********************************************************************

    Function:   Executes the target

    Description:

*********************************************************************/

void PASCAL ExecuteTarget(void)
{
    char szLocalExe[_MAX_PATH];
    CString strCmdLine;
    BOOL bRetry ;

    // no project that generates a target
    if ( gpIBldSys->IsActiveBuilderValid() != S_OK )
    {
        MessageBeep(0);
        return;
    }

	// Java: make sure the class file name and app type are specified
	// by the user; if not, get the from the user
	if (pDebugCurr && pDebugCurr->IsJava())
	{
		CString strLocalExe;

		// get the class file name, app type, and HTML viewer
		if (!GetJavaDebugInfo(TRUE))
			return;
		// fill in szLocalExe with the class name
		CString strClassName;
		gpIBldSys->GetJavaClassName(ACTIVE_BUILDER, strClassName);
		_tcscpy(szLocalExe, strClassName);
	}
	else
	{
		bRetry = TRUE;

		// Get the PROGRAM name (Caller for a DLL or EXE on program)
		while ((GetExecutableFilename(szLocalExe, sizeof(szLocalExe)) == exefromNone) && bRetry)
		{
			// We didn't find a 'Caller Executable' to use
			// In this case prompt for a new name
			bRetry = (gpIBldSys->GetInitialExeForDebug(ACTIVE_BUILDER, TRUE) == S_OK);
			if(!bRetry)
				// No Retry, abort out of here
				return;
		}
		// Try to get the Executable file name again
		if (GetExecutableFilename(szLocalExe, sizeof(szLocalExe)) == exefromNone)
			return;
	}

    PID                 pid;
    PROCINFO *          pprocinfo;
    BOOL                fOk;
    CString             strRemoteExe;
    CString             strRemoteDir;

    // get remote exe name
    // FUTURE: add code here similar to the code above which
    // prompts user to enter remote exe name
    if (pDebugCurr && pDebugCurr->MtrcRemoteTL())
    {
        gpIBldSys->GetRemoteTargetFileName(ACTIVE_BUILDER, strRemoteExe);

        if (strRemoteExe.IsEmpty())
        {
            if ( gpIBldSys->GetInitialRemoteTarget(ACTIVE_BUILDER, FALSE, FALSE) == S_OK )
                gpIBldSys->GetRemoteTargetFileName(ACTIVE_BUILDER, strRemoteExe);
            else
                return;
        }
    }
    else
    {
        strRemoteExe = szLocalExe;
    }

	// get the initial remote directory
	gpIBldSys->GetWorkingDirectory(ACTIVE_BUILDER, strRemoteDir);

	// If no working dir specified, and project is local, use project dir
	if (strRemoteDir.IsEmpty() && !pDebugCurr->MtrcRemoteTL())
	{
		CDir dir;
		LPCSTR pszPath;

		gpIBldSys->GetBuilderFile(ACTIVE_BUILDER, &pszPath);
		dir.CreateFromPath(pszPath);
		strRemoteDir = dir;
	}

	// Java: we'll pass slightly different parameters to OSDSpawnOrphan for Java
	if (pDebugCurr && pDebugCurr->IsJava())
	{
		// set strRemoteExe to the interpreter
		// set strCmdLine to the command line for the interpreter
		// set strRemoteDir to the class name if not OSDEBUG4 (for FSpawnOrphan)

		// are we executing this Java program using a browser or a stand-alone interpreter?
		ULONG nDebugUsing;
		gpIBldSys->GetJavaDebugUsing(ACTIVE_BUILDER, &nDebugUsing);
		if (nDebugUsing == Java_DebugUsing_Browser)
		{
			GetJavaBrowser(strRemoteExe, strCmdLine);
		}
		else if (nDebugUsing == Java_DebugUsing_Standalone)
		{
			GetJavaStandalone(strRemoteExe, strCmdLine);
		}
		else
		{
			// only Java applets and applications are supported
			ASSERT(FALSE);
		}


		// get the exe to launch, its args, and the class name
		CString strExe;
		CString strExeArgs;

		CDir dirWorking;
		{
			CString strWorkingDir;

			gpIBldSys->GetWorkingDirectory (ACTIVE_BUILDER, strWorkingDir);
			// If no working dir specified, and project is local, use project dir
			if (strWorkingDir.IsEmpty () && !pDebugCurr->MtrcRemoteTL ())
			{
				LPCSTR pszPath;
				gpIBldSys->GetBuilderFile(ACTIVE_BUILDER, &pszPath);
				dirWorking.CreateFromPath(pszPath);
			}
			else
			{
				dirWorking.CreateFromString (strWorkingDir);
			}
		}

		CPath pathClass;
		{
			CString strClassName;
			// get the class name
			gpIBldSys->GetJavaClassName(ACTIVE_BUILDER, strClassName);

			pathClass.CreateFromDirAndFilename (dirWorking, strClassName);
			strRemoteDir = pathClass;
		}

		// set the CLASSPATH environment variable in the current environment,
		// so that the browser/interpreter launched will inherit the correct CLASSPATH
		CString strClassPath;
		gpIBldSys->GetJavaClassPath(ACTIVE_BUILDER, strClassPath);
		SetEnvironmentVariable(_T("CLASSPATH"), strClassPath);

	}
	else
	{
		// get the program arguments
		GetDebuggeeCommandLine(strCmdLine);
		// if we have args to pass, prepend a space to separate from exe name.
		if(strCmdLine.GetLength())
			strCmdLine = " " + strCmdLine;

	}

	TCHAR *strWrkDir;
	if(!_tcslen(strRemoteDir)){
		strWrkDir = NULL;
	} else {
		strWrkDir = LPSTR(LPCSTR(strRemoteDir));
	}
	// NOTE, hpidCurr may be NULL now, but that's okay
	fOk = FSpawnOrphan(hpidCurr, strRemoteExe, strCmdLine, strWrkDir, &pid);

    if (fOk && !(pDebugCurr && pDebugCurr->MtrcRemoteTL()))
    {
        pprocinfo = new PROCINFO;
        // Create CPath for filename
        VERIFY(pprocinfo->path.Create(strRemoteExe));
        pprocinfo->pid = pid;

        // Add this PROCINFO to the list
        g_PidExecList.AddTail(pprocinfo);
    }
}


STARTCODE PASCAL
MkEngStartDebuggee(
	)
{
    int         iRet ;
    char        szLocalExe[_MAX_PATH];
    CString     strRemoteExe;
	CString     strDebuggeeArgs;	
    EXT         exeType;
    STARTCODE   sc;
    EXEFROM     exefrom;
	BOOL		fSucc;
	BOOL		fCrashDump = FALSE;
	BOOL		fDebugActive = FALSE;
	IMAGE_INFO	ImageInfo = {0};

	ImageInfo.Size = sizeof (ImageInfo);

	exefrom = GetExecutableFilename (szLocalExe, sizeof(szLocalExe));

	if (GetImageInfo (szLocalExe, &ImageInfo) &&
		ImageInfo.ImageType == IMAGE_TYPE_CRASHDUMP) {

		fCrashDump = TRUE;
	}

	if (theApp.m_jit.GetActive () && theApp.m_jit.GetEvent ()  == NULL) {
		fDebugActive = TRUE;
	}

	SetCrashDumpDebugging (fCrashDump);
		

	if (exefrom == exefromNone)	{

		//
		// Bring up an appropriate error saying that the caller for
		// the dynamic link library should be set.
		//

		if (gpIBldSys) {
		
			if ( (gpIBldSys->IsActiveBuilderValid() != S_OK) ||
				(gpIBldSys->GetInitialExeForDebug(ACTIVE_BUILDER, FALSE) != S_OK) ) {
				
				return SC_FAILED;
			}

			exefrom = GetExecutableFilename (szLocalExe, sizeof(szLocalExe));

			if (exefrom == exefromNone) {
				return SC_FAILED;
			}

		} else {
			ASSERT (FALSE);
			return SC_FAILED;
		}
	}
	

    if (gpIBldSys && (gpIBldSys->IsActiveBuilderValid() == S_OK) &&
		pDebugCurr && pDebugCurr->MtrcRemoteTL()) {
		
        gpIBldSys->GetRemoteTargetFileName(ACTIVE_BUILDER, strRemoteExe);

        if (strRemoteExe.IsEmpty()) {

			HRESULT hr;
			
            hr = gpIBldSys->GetInitialRemoteTarget (ACTIVE_BUILDER,
													FALSE,
													FALSE);
			if (hr == S_OK) {
			
                gpIBldSys->GetRemoteTargetFileName (ACTIVE_BUILDER,
													strRemoteExe
													);
			} else {
                return SC_FAILED;
			}
        }
	} else {
        strRemoteExe = szLocalExe;
    }

    GetDebuggeeCommandLine (strDebuggeeArgs);
	
	if (strDebuggeeArgs.GetLength ()) {
		strDebuggeeArgs = " " + strDebuggeeArgs;
	}

    _tcscpy(ExecutablePath, szLocalExe);

	//
    // Save the name of the current target for later use
	//

	if (GetDebugTarget (szPath, sizeof(szPath))) {
        SetDebugTarget (szPath);
	}

	
    fSucc = RestartDebuggee (szLocalExe,
							 (LPTSTR) (const char *) strRemoteExe,
							 (LPTSTR)(const char *)strDebuggeeArgs,
							 fCrashDump
							 );

	if (!fSucc) {
		return SC_FAILED;
	}

    TraceInfo.pDoc = NULL;

	//
	// If this is a crashdump do not step
	//

	if (fCrashDump) {
		return SC_STOPPED;
	}

	//
	// HACKHACK: this should probablly be SC_RUNNING, but the caller of
	// MkEngStartDebuggee () doesn't expect that, so this will due.
	// The point is, do not step here.
	
	if (fDebugActive) {
		return SC_ATMAINORDEAD;
	}

	iRet = Step (TRUE, stoQueryStep);

    switch(iRet)
    {
        case STEPN_NOENTRY :
            sc = SC_GONE ;
            break;

        case STEPN_BPFAILED:
            sc = SC_BPFAILED;
            break;

        case STEPN_BREAK:
            sc = SC_STOPPED ; // [cuda#2761 3/20/93 mikemo]
            break;
        
        case STEPN_FAILED:
            sc = SC_FAILED;
            break;

        case STEPN_NOERROR:
        default:
            sc = SC_ATMAINORDEAD;
            break ;
    }

    return( sc );
}



void PASCAL
MkEngStartSqlInProcDebuggee(
	)
/*++

Routine Description:

	Fire up sqlproxy for debugging.

--*/
{

	ASSERT(!DebuggeeAlive());

	ASSERT(pDebugCurr != NULL);

	// This effectively loads the debuggee
	char		szLocalExe[_MAX_PATH];
	char		szPath[_MAX_PATH];
	CString		strRemoteExe;
	EXT			exeType;

	// check the registry is OK for SQL debugging else all sorts of strange
	// things will happen
	{
		const TCHAR *szTL, *szEM, *szSH, *szEE;

		if (
			(!pDebugCurr->GetComponents( "NMSQL", &szTL, &szEM, &szSH, &szEE)) ||
			_tcsicmp(szEM, "EM") ||
			_tcsicmp(szSH, "SHSQL" ) ||
			_tcsicmp(szEE, "EESWT" )
		   )
		{
			ErrorBox( IDS_ERR_BADREGISTRY, NULL );
			return;
		}

		if (_tcsicmp(szTL, "TLLOC"))
		{
			ErrorBox( IDS_ERR_SQLNOTLOCAL, NULL );
			return;
		}
	}

	// get file name of sqlproxy.exe - should reside as same as this exe
	DWORD cbPath;
	if (!(cbPath = GetModuleFileName(NULL, szPath, _MAX_PATH)) ||
		(cbPath > MAX_PATH))
		// something bad happened 
	{
		ErrorBox(ERR_Exe_Open, szLocalExe);
		return;
	}

	char rgchDrive[_MAX_CVDRIVE];
	char rgchDir[_MAX_CVDIR];
	SplitPath(szPath, rgchDrive, rgchDir, NULL, NULL);
	_tcscpy(szLocalExe, rgchDrive);
	_tcsncat(szLocalExe, rgchDir, _MAX_PATH);
	_tcscat(szLocalExe, szSqlProxyBase ".EXE" );

	strRemoteExe = szLocalExe;

	
	exeType = GetExeType(szLocalExe, NULL);

	IMAGE_INFO	ImageInfo = {0};
	
	ImageInfo.Size = sizeof (ImageInfo);
	
	if (!GetImageInfo (szLocalExe, &ImageInfo) ||
		ImageInfo.ImageType != IMAGE_TYPE_WIN32) {

		ErrorBox(ERR_Exe_Open, szLocalExe);
		return;
	}

	// Ensure we start in the project directory - if we don't
	// do a make before running we could be anywhere.  (Also
	// need this for the potential ProjectDiskTimestamp() call)

	MkEngSetMakeDriveDir();



	//
	//	Debugging is taking place for the first
	//	time. Debuggee was not alive. We call
	//	StartDebuggee for loading the debuggee
	//	and check what is returned.
	//

	DWORD dwStartTime;

	dwStartTime = ::GetTickCount();

	// Set the title bar to running
//	SetDebuggeeRunning(TRUE);

	// First debugging command, erase the output window
	// Erase any previous Hits & switch to the virtual build window
	//
	OutputWindowVwinClear(OwinGetDebugCommandID());

	fUpdateRemoteTarget = FALSE;
	_ftcscpy(ExecutablePath, szLocalExe);

	// Note that RestartDebuggee disables the appropriate
	// ribbon controls.  Dll symbol loading + bp application
	// is processed in LoadDebuggee.

	// Build proxy command-line, of a key and a dynamic event name
	char rgchDebugArgs[256];
	strcpy( rgchDebugArgs, " " SZSQLPROXYKEY " ");
	char *p = rgchDebugArgs + strlen(rgchDebugArgs);
	BuildProxyEventName( p );

	if (!RestartDebuggee(szLocalExe, (LPTSTR) (const char *) strRemoteExe,
		rgchDebugArgs, FALSE))
	{
		ErrorBox(ERR_Exe_Open, szLocalExe);
		return;
	}

	TraceInfo.pDoc = NULL;

	// Now step to WinMain

	// [CAVIAR #6390 01/04/93 v-natjm]
	int iRet = Step (TRUE, stoQueryStep);
	g_retVals.CleanUp ();

	// Set our debuggee flag to running before
	// setting the ribbon button controls
//	SetDebuggeeRunning(TRUE);

	// Handle the title bar mode state to reflect
	// the actual debuggee status.
	SetModeName(TBR_Mode_Run) ;

	// [CAVIAR #5848 12/01/92 v-natjm]
	// Clear any bp message from status bar
//	StatusText(SYS_StatusClear,STATUS_INFOTEXT,FALSE) ;

 	// Reset exception flags
	lpprcCurr->SetFirstChance(FALSE);

// 	fIsLaunchingDebuggee = FALSE;

	// interface must exist, else we couldn't have got here
	struct ISqlExec *pISqlExec;
	VERIFY(SUCCEEDED(theApp.FindInterface( IID_ISqlExec, (LPVOID*)&pISqlExec )));

	// and tell the Ent package to issue the query before this final Go
	pISqlExec->DebuggeeLoadNotify(lntSQLThreadCreated, NULL);
	pISqlExec->Release();

	Go (NULL, stoNone, TRUE);
 	UpdateDebuggerState(UPDATE_ALLSTATES);

}

/*********************************************************************

    Function:   ExecDebuggee

    Description:

        Executes the debuggee in the passed manner.  Makes all
        the necessary tests against the make to see if a
        build should take place.

*********************************************************************/
void PASCAL ExecDebuggee(EXECTYPE ExecType)
{    
    BOOL        fShowCSIP = FALSE;	  
    BOOL        fDoBuild = FALSE;
    STARTCODE   StartCode;
    BOOL        fTryExeBuild = FALSE;
    UINT        wError = STEPCUR_NOERROR ;
    UINT        wRes ;
    HCURSOR     hOldCursor = 0;
    BOOL        fPassException = FALSE;
    BOOL        fIsFirstChance = lpprcCurr->IsFirstChance();
    TCHAR       tchRibbonEdit[ 1024 ];
    TCHAR *     ptchRibbonEdit = (TCHAR *)NULL;
    BOOL        fIsExternal;
    BOOL        fEncFailed = FALSE;
    BOOL        fEncSucceeded = FALSE;
    BOOL        fEncCompiledFiles = FALSE;
    BOOL        fEncCompileError = FALSE;
    BOOL        fEncCanceled = FALSE;
    BOOL        fDisableBuildUI = FALSE;

    // Clear any bp message from status bar
    gpISrc->StatusText(SYS_StatusClear, STATUS_INFOTEXT, FALSE);

    // [bug #4984 11-4-92 v-natal]
    // moved the following code line and all the following block if { ... }, to avoid to double ask
    // the user in the case of the exe doesn't exists.
    // This call also handles external project types...
    // [bug #6570 4-1-93 v-natal]
	if (FIsActiveProjectDebuggable( NULL, &fIsExternal, &fTryExeBuild, NULL ))
	{
		// OK to debug and the BOOLs are set by the above call
		;
	}
    else if (theApp.m_jit.GetActive())
    {
        fTryExeBuild = TRUE;
        fIsExternal = FALSE;
    }
    else
    {
        // can't do anything here--no project and not JIT debugging--blow
        if (hOldCursor)
            SetCursor(hOldCursor);

        return;
    }

    if (!DebuggeeAlive())
    {
        if(!theApp.SaveAll(gpISrc->GetSrcEnvironParam(ENV_PROMPT_BEFORE_SAVE), FILTER_PROJECT | FILTER_DEBUG | FILTER_NEW_FILE))
			return;
		gpISrc->InitProjectState();
    }

	if (FIsSqlInproc() && !DebuggeeAlive())
	{
		MkEngStartSqlInProcDebuggee();
		return;
	}

    // If we are not alive yet, can we build the caller (ie. is it our target?
    if (!DebuggeeAlive() && !fDoBuild && fTryExeBuild)
    {
        EXEFROM exefrom;
        char    Executable[_MAX_PATH];

        exefrom = GetExecutableFilename(Executable, sizeof(Executable));

        // if fTryExeBuild is TRUE we already know there is a valid exename.
        ASSERT(exefrom != exefromNone);

        if (exefrom == exefromPid)
        {
            // We're getting the EXE name from the PID that was specified
            // on the command line.  This EXE name may be bogus (it is
            // for now), so don't do any checking on it.
        }
        else if (*Executable)
        {
            // Check if it exists

            if (!FileExist(Executable))  // The FileExists function should be moved from util.cpp to a debugger .cpp file
            {
                int iType;

                if ( (gpIBldSys->IsActiveBuilderValid() == S_OK) && (SUCCEEDED(gpIBldSys->GetBuilderType(ACTIVE_BUILDER, &iType))) && (iType != ExeBuilder))
                {
#ifndef RAD_DEMO
                    if ((exefrom != exefromLastDoc) && (QuestionBox(ERR_No_Exe_Build_It, MB_YESNO, (LPSTR)Executable) == IDYES))
                    {
#endif  // ndef RAD_DEMO
                        // User wants to build the EXE
                        fDoBuild = TRUE;
#ifndef RAD_DEMO
                    }
                    else
                    {
                        // No EXE and user doesn't want to create it!
                        if ( hOldCursor )
                            SetCursor ( hOldCursor );

                        return;
                    }
#endif // ndef RAD_DEMO
                }
                else
                {
                    // dolphin 3642 [patbr] build not allowed.  user
                    // opened EXE as project but EXE does not exist!
                    // Tell him and bail (he has no other option).
                    ErrorBox(ERR_No_Exe_No_Options, (LPSTR)Executable);
                    if ( hOldCursor )
                        SetCursor ( hOldCursor );

                    return;
                }
            }
        }
        else
        {
            ErrorBox(ERR_No_TargetName);

            if ( hOldCursor )
                SetCursor ( hOldCursor );
            return ;
        }
    }

    // See if we should attempt to Edit & Continue
    BOOL fForceENC = ExecType == EXEC_EDITANDCONTINUE ||
                     ExecType == EXEC_ENC_FORCEGO;
    if (DebuggeeAlive() &&
        gpISrc->ENCStateModified() &&
        (fForceENC ||
            (runDebugParams.fENCDebugging && 
            // no ENC if restarting (vc98 #26114)
            ExecType != EXEC_RESTART))
    ){

        DWORD encFlags = DoEditAndContinue();
        WORD updateFlags = UPDATE_ALLDBGWIN | UPDATE_DEBUGGEE | UPDATE_ENC;
        fEncSucceeded = encFlags & EncUpdatedImage;
        fEncCanceled = encFlags & EncCancel;
#if 0
        // Disabled (11/18/97): This is now done by UpdateDebuggerState
        if (pViewDisassy) {
            // have dam window update its map so that it doesn't
            // point to stale line info [ds98 6886]

			// We need to do this no matter whether ENC succeeded or failed
			// since the SAPI cache may have been voided by the ENC engine
            pViewDisassy->SendMessage( WU_UPDATEENC, wPCXF, (DWORD)&cxfIp );
        }
#endif
        if ((ExecType == EXEC_TRACEFUNCTION || ExecType == EXEC_SRC_TRACEFUNCTION)) {
            // Need to rerun SetupStep after ENC  [vc98 18570]
            ASSERT(g_pTraceFunction);
            if (!g_pTraceFunction->SetupStep ()) {
                UpdateDebuggerState(updateFlags);
                if (!fEncCanceled) {
                    ENCOutputErrorSummary();
                }
                return;
            }
        }
        if (!DebuggeeAlive() || (encFlags & EncCancel)) {
            // Stop debugging was invoked by the user during the recompile
            // Or user has canceled the operation
            UpdateDebuggerState(updateFlags);
            if (ExecType != EXEC_ENC_FORCEGO || !DebuggeeAlive()) {
                if (!fEncCanceled) {
                    ENCOutputErrorSummary();
                }
                return;
            }
        }
        if (fEncSucceeded) {
            if (!(encFlags & EncBoundBPs) || (encFlags & EncMovedIP)) {
                // image update successful, but couldn't bind BPs
                // or the ip has been moved to a different location
                // Cancel last debugging operation
                // Update source to provide visual indication of the IP

                if ( ExecType != EXEC_EDITANDCONTINUE ) {
                    // Notify user that EXEC was canceled
                    CString strReason;
                    int idsReason = (encFlags & EncMovedIP) ?
                        IDS_EncEditMoveIp : ERR_Multiple_BP_ENC;
                    strReason.LoadString(idsReason);
                    ENCOutputErr(Enc_Err_CmdNotExecuted, (LPCTSTR)strReason);
                }

                UpdateDebuggerState(updateFlags | UPDATE_SOURCE);

                if (encFlags & EncMovedIP) {
                    // Display msg box after calling UpdateDebuggerState
                    // so that the IP glyph point to the new current line
                    MsgBox(Information, IDS_EncMsgMoveIp);
                }

                ENCOutputErrorSummary();
                return;
            }

            UpdateDebuggerState(updateFlags);
        }
        else {
            if (!(encFlags & EncCancel)) {
                if (encFlags & EncCompileError) {
                    // a non-ZX related compiler error has occurred. In this case
                    // we want to avoid prompting for rebuild, as the rebuild will most
                    // likely fail with the same error
                    fEncCompileError = TRUE;
                    gpISrc->StatusText(IDS_Enc_CompileError, STATUS_ERRORTEXT, FALSE);
                }
                else if (encFlags & EncUnsupportedEdit) {
                    gpISrc->StatusText(IDS_Enc_UnsupportedEdit, STATUS_ERRORTEXT, FALSE);
                }
                else {
                    gpISrc->StatusText(STA_EncFailed, STATUS_ERRORTEXT, FALSE);
                }
            }
            fEncFailed = TRUE;
            fEncCompiledFiles = encFlags & EncCompiledFiles;
        }

        fDisableBuildUI = fEncCompileError || fEncCanceled;
    }

    if (fEncSucceeded || (fEncFailed && !fEncCanceled)) {
        ENCOutputErrorSummary();
    }

    BOOL fPromptUser = FALSE;

    // has the build state of the debuggee changed?
    // we check
    // o timestamps out of date
    // o chaged options
    // o files dirty in memory?
    if (!fDisableBuildUI && !fDoBuild && gpISrc->ProjectStateModified() && !fIsExternal )
    {
        fPromptUser = TRUE;
        switch( gpISrc->HandleBuildStateChange() )
        {
            case IDYES:
                fDoBuild = TRUE;
                break;
            case IDNO:
                break;
            case IDCANCEL:
                if (fEncCompiledFiles) 
                    ENCRestoreObjs();
                if (ExecType == EXEC_ENC_FORCEGO) {
                    break;
                }
                else {
                    return;
                }
            default:
                // No Dlg box was shown to the user
                fPromptUser = FALSE;
                break;
        }
    }
    // have files that are members of the project been changed (but not saved)?
    else if (!fDisableBuildUI && !fDoBuild && gpISrc->DebuggeeStateModified() && !fIsExternal )
    {
        fPromptUser = TRUE;
        switch (gpISrc->HandleProjectStateChange())
        {
            case IDYES:
                fDoBuild = TRUE;
                break;
            case IDNO:
                break;
            case IDCANCEL:
                if (fEncCompiledFiles) 
                    ENCRestoreObjs();
                if (ExecType == EXEC_ENC_FORCEGO) {
                    break;
                }
                else {
                    return;
                }
            default:
                // No Dlg box was shown to the user
                fPromptUser = FALSE;
                break;
        }
    }

    if (fEncCompiledFiles && !fDoBuild) {
        ENCRestoreObjs();
    }

    if (fEncFailed && !fEncCanceled && !fPromptUser) {
        // Notify user about ENC failure 
        if ( ExecType != EXEC_EDITANDCONTINUE &&
            ExecType != EXEC_ENC_FORCEGO) {       
            CString strMsg;
            strMsg.LoadString(fEncCompileError ? 
                IDS_Enc_HandleCompileError : IDS_Enc_CannotComplete);
            switch (MsgBox(Question, strMsg, MB_YESNO | MB_DEFBUTTON2)) {
            case IDYES:
                gpISrc->ResetProjectState();
                break;
            
            case IDNO:
                ShowFirstError();
                if (ExecType == EXEC_ENC_FORCEGO) {
                    break;
                }
                else {
                    return;
                }
            }
        }
        else {
            ShowFirstError();
            MessageBeep(0);
        }
    }

	//
	//	At this point, the project has to be rebuilt. If debuggee was
	//	running, we kill it or let it run depending on its status.
	//

    if( fDoBuild )
    {
        // Check if Debuggee was running...
        if (DebuggeeAlive())
        {
            // Kill him
            if (!ClearDebuggee(FALSE))
            {
                if (hOldCursor)
                    SetCursor(hOldCursor);

                return;
            }
        }

        // Do the build
        if (hOldCursor)
            SetCursor(hOldCursor);

        // If the build fails, bail
        // this is the time where we need to set the values of the 3 global
        // variables since they are always set to FALSE at the end of build.
       if ( gpIBldSys->DoTopLevelBuild(ACTIVE_BUILDER, FALSE, NULL, fcaRebuild, FALSE, TRUE) != S_OK )
            return;

        // NOTE: we need to return if DoTopLevelBuild() returns FALSE

        // OLYMPUS 7688: Re-enable this given incremental builds
        // this is very annoying....
        // matthewt 4/28/95
        //
        // DOLHPIN 9066: This next line will ALWAYS
        // exit if a build happened.  Ideally, this
        // wouldn't happen, but there are concerns
        // that if the build took a long time (define
        // long???) that the user may have forgotten
        // that they wanted a build.  This will disable
        // the fix for 9066 (which has been postponed)
        // markbro  3/21/94
    }


	if (ExecType == EXEC_EDITANDCONTINUE) {
		// The user only wanted to apply code changes
		// and/or rebuild the debuggee
		if (fEncSucceeded) {
			UpdateDebuggerState(UPDATE_ALLSTATES);
		}
		return;
	}

	//
	//	At this point, we check if just running	the program instead of
	//	debugging it.

    if (ExecType == EXEC_TARGET) {
        // Call our magic
        if ( hOldCursor ) SetCursor ( hOldCursor );
        ExecuteTarget() ;
        return ;
    }

	//	
	//	Debugging is taking place for the first time. Debuggee was not alive.

	if(!DebuggeeAlive())
    {
        // Ensure we start in the project directory - if we don't
        // do a make before running we could be anywhere.  (Also
        // need this for the potential ProjectDiskTimestamp() call)

		MkEngSetMakeDriveDir();
    }


	//
    //	Debugging is taking place for the first time. Debuggee was not
	//	alive. We call StartDebuggee for loading the debuggee and check what
	//	is returned.
	//

    DWORD dwStartTime;

    // Check if it is the first time
    if (!DebuggeeAlive()) {

		dwStartTime = ::GetTickCount ();

		//
        // If continue to cursor and the focus is on the
        // ribbon, save the text since the change to the
        // debug layout will change the focus and this
        // it won't work (as the user expects it to)
		//

		if ( ExecType == EXEC_TOCURSOR && IsFindComboActive ()) {
		
            SendMessage (GetFocus(),
						 WM_GETTEXT,
						 sizeof (tchRibbonEdit),
						 (LPARAM)tchRibbonEdit
						 );
						 
            ptchRibbonEdit = tchRibbonEdit;
        }


		//
        // First debugging command, erase the output window
        // Erase any previous Hits & switch to the virtual build window
    
        OutputWindowVwinClear (OwinGetDebugCommandID());
        OutputWindowSelectVwin (OwinGetDebugCommandID(), FALSE);
        fUpdateRemoteTarget = FALSE;

		//
        // This effectively loads the debuggee
		//
		
        StartCode = MkEngStartDebuggee ();

        if (hOldCursor) {
			SetCursor (hOldCursor);
		}

	
        // Debuggee is either at entry point or dead
        if (StartCode == SC_ATMAINORDEAD) {

            if ( DebuggeeAlive () ) {

                ASSERT (IS_STATE_DEBUG(DkGetDockState()));

                // Check mode that was asked for
                switch (ExecType){
                    // Let's fall thru the code that is processed
                    // on a normal GO
                    case EXEC_GO:
                    case EXEC_ENC_FORCEGO:
                        break;

                    // We are at WinMain() or main(), if ToCursor
                    // fails, let's show the current CSIP on current line.
                    case EXEC_TOCURSOR:
                        fShowCSIP = TRUE;
                        break;

                    // All of these situations lead to a Stop
                    // at either WinMain() or main().
                    case EXEC_STEPTORETURN:
                    case EXEC_RESTART:
                    case EXEC_TRACEINTO:
                    case EXEC_TRACEFUNCTION:
                    case EXEC_STEPOVER:
                    case EXEC_SRC_TRACEINTO:
                    case EXEC_SRC_TRACEFUNCTION:
                    case EXEC_SRC_STEPOVER:
                        // Directly update the current CS:IP
                        UpdateDebuggerState (UPDATE_ALLSTATES);

                        DWORD dwEndTime = ::GetTickCount( );
                        DWORD dwElapsedTime = dwEndTime - dwStartTime;
                        int nMinutes = dwElapsedTime/60000;
                        int nSeconds = (dwElapsedTime%60000)/1000;
                        int nTenthsOfSecond = (dwElapsedTime % 1000)/100;

                        if ( g_bDebugTime )
                        {
                            CString strElapsedTime;
                            strElapsedTime.Format("Debug start time: %d:%2.2d:%d",
                                            nMinutes, nSeconds, nTenthsOfSecond);
                            OutputWindowQueueHit(OwinGetDebugCommandID(), strElapsedTime, FALSE, TRUE);
                        }

                        return;
                }
            }


			if (!DebuggeeAlive()) {

                if ((ExecType == EXEC_GO) || (ExecType == EXEC_TOCURSOR) ||
                    (ExecType == EXEC_ENC_FORCEGO)) {
                    UpdateDebuggerState(UPDATE_ALLSTATES);
                }
                // Just return at this point
                return;
            }
        }

		if (StartCode == SC_GONE) {
		
            UpdateDebuggerState (UPDATE_ALLSTATES);
            return;
        }

		//
		// HACKHACK: this is horrible . . .
		//

		if (theApp.m_jit.GetActive () && theApp.m_jit.GetEvent () == NULL) {
			ExecType = EXEC_GO;
		}

		if (StartCode == SC_STOPPED || StartCode == SC_BPFAILED) {

			UpdateDebuggerState(UPDATE_ALLSTATES);
            return;
        }

		//
        // We failed while loading the debuggee. It could have
        // been a problem in loading OSDEBUG, CV410, loading the
        // symbols, wrong debugging information
        //
		
        if (StartCode == SC_FAILED) {
            // Just exit at this point
            return ;
        }

    }

	//
	// Debugging is already in place and this
	// code is a fall thru for the above code.
	//

    g_retVals.CleanUp ();


	// Handle the title bar mode state to reflect
    // the actual debuggee status.

	if (IsCrashDump ()) {
		SetModeName (TBR_Mode_CrashDump);
	} else {
		SetModeName (TBR_Mode_Run);
	}

    // Clear any bp message from status bar
    gpISrc->StatusText(SYS_StatusClear,STATUS_INFOTEXT,FALSE) ;

    // Determine whether to pass exception (if any) to debuggee
    switch (ExecType)
    {
        case EXEC_GO:
        case EXEC_ENC_FORCEGO:
        case EXEC_TOCURSOR:
        case EXEC_STEPTORETURN:
        case EXEC_TRACEINTO:
        case EXEC_TRACEFUNCTION:
        case EXEC_STEPOVER:
        case EXEC_SRC_TRACEINTO:
        case EXEC_SRC_TRACEFUNCTION:
        case EXEC_SRC_STEPOVER:
            // Check if there's a pending exception when we try
            // to run the debuggee
			if (pDebugCurr->IsJava()) {
				fPassException = TRUE;
			}
			else if (fIsFirstChance)
            {
                int ret = IDYES;//BUGBUG: with #ifdefs below
                        //  must init this to something!!
                        //

                // There's one. We may choose to pass exception to debuggee
                // or not, or cancel the Go command.
                ret = QuestionBox(DBG_Pass_Exception,MB_YESNOCANCEL);
                if (ret == IDCANCEL)
                {
                    // Repaint the ribbon controls according to the
                    // new debuggee status.
//                    fIsLaunchingDebuggee = FALSE;

                    // Handle the title bar mode state

					if (DebuggeeAlive ()) {
						if (IsCrashDump ()) {
							SetModeName (TBR_Mode_CrashDump);
						} else {
							SetModeName (TBR_Mode_Break);
						}
					} else {
						SetModeName ( (UINT)0);
					}
                    return;
                }
                if (ret == IDYES)
                    fPassException = TRUE;
            }
    }
    // Reset exception flags
    lpprcCurr->SetFirstChance(FALSE);


    // Handle debugging commands
    switch (ExecType)   {
        // User wants to restart, call StartDebuggee
        // which actually takes care of an eventual
        // restart if debuggee was already running
        // or stopped.
        case EXEC_RESTART:
            dwStartTime = ::GetTickCount();
			theApp.m_jit.SetActive (FALSE);			// clear Attach flag

            // If we fail, just return
            if (MkEngStartDebuggee() == SC_FAILED) {
                // We don't change the dock state when we
                // kill the debuggee for a restart. If the
                // restart fails for some reason, set back the
                // dock state to normal ( not debugging ).
                DkSetDockState(STATE_EDIT);
                return;
            }
            // Otherwise, we update the debugger state
            // which means we just break
            break;

        // We never return from a GO which handles
        // an internal message loop for the debugger.
        // This is mostly an Emulate issue...
        case EXEC_GO:
        case EXEC_ENC_FORCEGO:
            // [matthewt] CUDA bug fix #4179
            // Repaint the ribbon controls according to the
            // new debuggee status.
//            fIsLaunchingDebuggee = FALSE;
            {
                BOOL fFlipScreen = TRUE;
                if (ExecType == EXEC_ENC_FORCEGO &&
                    fEncFailed &&
                    !fEncCanceled) {
                    fFlipScreen = FALSE;
                }
                Go (NULL,fPassException,fFlipScreen);
            }

            // Coming back, we update the debugger,
            // just break
            break;
        // We want to execute to the next cursor position
        // This is done by a GO until temporary BP is
        // reached. This function will return FALSE if
        // we couldn't set the temporary BP at the specified
        // line or if we were already sitting on this line.
        case EXEC_TOCURSOR:
            // GO until...
            if ((wError = ContinueToCursor(fPassException,ptchRibbonEdit)) !=
                STEPCUR_NOERROR)
            {
                // Is that enough ? Well, could be bothering to
                // get a message...
                MessageBeep(0);

                switch (wError) {
                    case STEPCUR_NOCODE: wRes = STA_StepCurNoCode ; break ;
                    case STEPCUR_NOBPSET: wRes = STA_StepCurNoBPSet ; break ;
                    case STEPCUR_NOMOVE: wRes = STA_StepCurNoMove ; break ;
                }
                // [CAVIAR #5848 12/01/92 v-natjm]
                gpISrc->StatusText(wRes,STATUS_ERRORTEXT,FALSE);

                // Remember whether we were in a first-chance exception
                lpprcCurr->SetFirstChance(fIsFirstChance);

                // Update the ribbon button controls
//                fIsLaunchingDebuggee = FALSE;

                // Handle the title bar mode state

				if (DebuggeeAlive ()) {
					if (IsCrashDump ()) {
						SetModeName (TBR_Mode_CrashDump);
					} else {
						SetModeName (TBR_Mode_Break);
					}
				} else {
					SetModeName ( (UINT)0);
				}

                if (fEncSucceeded) {
                    // We are doing an Edit & Continue and run to cursor
                    // has failed. Since E&C was successful, update the 
                    // debugger state
                    UpdateDebuggerState(UPDATE_ALLSTATES);
                }

                // The default is to not update the current CS:IP
                // since the function failed but this code is also
                // a fall thru and we need to update after loading
                // the debuggee ( see EXEC_TOCURSOR above )
                // [CAVIAR #5937 12/01/92 v-natjm]
                if (!fShowCSIP) {
                    return ;
                }
            }
            // Just break if ContinueToCursor was successful or
            // if it failed and we have to show the CS:IP
            break;

        // This look into the call stack, set a temporary BP
        // and run a GO until. The process is the same as an
        // EXEC_TOCURSOR
        case EXEC_STEPTORETURN:
            // GO until...

			wError = StepToReturn (fPassException, lpprcCurr->GetStepMode ());

			if (wError != STEPOUT_NOERROR)
			{
                MessageBeep(0);

				switch(wError)
                {
                    case STEPOUT_NOCALLER : wRes = STA_StepOutNoCaller ;break ;
                    case STEPOUT_NOSOURCE : wRes = STA_StepOutNoSource ;break ;
                    case STEPOUT_NOBPSET  : wRes = STA_StepOutNoBPSet ; break ;
                }

                gpISrc->StatusText(wRes,STATUS_ERRORTEXT,FALSE) ;

                // Remember whether we were in a first-chance exception
                lpprcCurr->SetFirstChance(fIsFirstChance);

                // Repaint the ribbon controls according to the
                // new debuggee status.
//                fIsLaunchingDebuggee = FALSE;

                // Handle the title bar mode state
			
				if (DebuggeeAlive ()) {
					if (IsCrashDump ()) {
						SetModeName (TBR_Mode_CrashDump);
					} else {
						SetModeName (TBR_Mode_Break);
					}
				} else {
					SetModeName ( (UINT) 0);
				}

                // Same as above code. This is a fall thru for
                // EXEC_STEPTORETURN when we ran the first time
                if (!fShowCSIP) {
                    return;
                }
            }
            // Just break if StepToReturn was successful or
            // if it failed and we have to show the CS:IP
            break;


		case EXEC_STEPOVER:
        case EXEC_TRACEINTO:
        case EXEC_TRACEFUNCTION:
        case EXEC_SRC_TRACEFUNCTION:
        case EXEC_SRC_TRACEINTO:
        case EXEC_SRC_STEPOVER:
			{
				ULONG	sto 	  = stoNone;
				BOOL	fStepOver = FALSE;


				if (fPassException)
					sto |= stoPassEx;

                if (ExecType == EXEC_TRACEINTO || ExecType == EXEC_STEPOVER)
                {
					if (lpprcCurr->GetStepMode () == SRCSTEPPING)
                        sto |= stoQueryStep;
                }
                else
                {
                    sto = (STO) (sto | stoQueryStep);
                }


                if (ExecType == EXEC_STEPOVER || ExecType == EXEC_SRC_STEPOVER)
                    fStepOver = TRUE;

                if (ExecType == EXEC_TRACEFUNCTION ||
					ExecType == EXEC_SRC_TRACEFUNCTION)
				{
                    sto |= stoTraceFunction;
				}
				
				Step (fStepOver, (STO) sto);
			}
			break;

        // Any other command is not supported.
        default:
            ASSERT(FALSE);
            break ;
    }


	//
	//  At this point it is possible that we are actually shutting
	//	down; check for this -- in which case we just want to get out

	if (!lpdbf || !lpprcCurr)
	{
		return;
	}

	// Give a chance to ENC engine to handle a pending tmp breakpoint
	ENCHandleTmpBp();

    // If we didn't return before executing this code, it means we
    // have to update the debugger state i.e. the current CS:IP if
    // any and every window that shows some information related to
    // the current context ( LOCALS , REGISTERS , WATCH )
    if (!lpprcCurr->m_bEncGoPending)
    {
        UpdateDebuggerState(UPDATE_ALLSTATES);
    }
    else {
        // We are doing a break/go for ENC
        // Do not update sources to avoid showing the DAM window
        // if we broke in system code. 
        // We'll only need to update sources if ENC fails
        UpdateDebuggerState(UPDATE_ALLSTATES & ~UPDATE_SOURCE);
        AfxGetMainWnd()->PostMessage (WM_COMMAND, IDM_DEBUG_UPDATEIMAGE);
    }

	// Did any of the event handlers try to do a Debugger.Go?
	if (lpprcCurr->m_bGoDeferred)
	{
		// Yes, so do it now
		lpprcCurr->m_bGoDeferred = FALSE;
		pAutoDebugger->PublicGo();
	}

	if (lpprcCurr->m_bStopDeferred)
	{
		lpprcCurr->m_bStopDeferred = FALSE;
		pAutoDebugger->PublicStop(); 
	}

    if (g_bDebugTime && ExecType == EXEC_RESTART)
    {
        DWORD dwEndTime = ::GetTickCount();
        DWORD dwElapsedTime = dwEndTime - dwStartTime;
        int nMinutes = dwElapsedTime/60000;
        int nSeconds = (dwElapsedTime%60000)/1000;
        int nTenthsOfSecond = (dwElapsedTime % 1000)/100;

        if ( g_bDebugTime )
        {
            CString strElapsedTime;
            strElapsedTime.Format("Debug restart time: %d:%2.2d:%d",
                            nMinutes, nSeconds, nTenthsOfSecond);

            OutputWindowQueueHit(OwinGetDebugCommandID(), strElapsedTime, FALSE, TRUE);
        }
    }

}

// get the pertinent Java debug info: the class file name, what to debug the app under (browser
// or stand-alone), and the browser or stand-alone interpreter name.
// If we can't get this from the props, ask the user. If we fail to get the info, return FALSE.
BOOL GetJavaDebugInfo(BOOL bExecute /*= FALSE*/)
{
	// this should only be called if the current project is Java
	ASSERT(pDebugCurr && pDebugCurr->IsJava());

	// get the current project
	if (gpIBldSys != NULL)
	{

		BOOL bDebugInfo = FALSE;

		CString strClassName;
		ULONG nDebugUsing = Java_DebugUsing_Unknown;
		CString strBrowser;
		CString strStandalone;

		// get the information needed to debug
		gpIBldSys->GetJavaClassName(ACTIVE_BUILDER, strClassName);
		gpIBldSys->GetJavaDebugUsing(ACTIVE_BUILDER, &nDebugUsing);
		gpIBldSys->GetJavaBrowser(ACTIVE_BUILDER, strBrowser);
		gpIBldSys->GetJavaStandalone(ACTIVE_BUILDER, strStandalone);

		// do we have the information needed to debug?
		//   - class file name &&
		//     ((debug using browser && browser name) ||
		//     (debug using stand-alone interpreter && interpreter name))
		bDebugInfo = (!strClassName.IsEmpty() &&
			((nDebugUsing == Java_DebugUsing_Browser && !strBrowser.IsEmpty()) ||
			(nDebugUsing == Java_DebugUsing_Standalone && !strStandalone.IsEmpty())));

		// do we know the information needed to debug
		if (!bDebugInfo)
		{
			if (gpIBldSys->GetInitialJavaInfoForDebug(ACTIVE_BUILDER, bExecute) == S_FALSE)
				return FALSE;

			// get the information needed to debug again
			gpIBldSys->GetJavaClassName(ACTIVE_BUILDER, strClassName);
			gpIBldSys->GetJavaDebugUsing(ACTIVE_BUILDER, &nDebugUsing);
			gpIBldSys->GetJavaBrowser(ACTIVE_BUILDER, strBrowser);
			gpIBldSys->GetJavaStandalone(ACTIVE_BUILDER, strStandalone);

			// do we have the information needed to debug?
			//   - class file name &&
			//     ((debug using browser && browser name) ||
			//     (debug using stand-alone interpreter && interpreter name))
			bDebugInfo = (!strClassName.IsEmpty() &&
				((nDebugUsing == Java_DebugUsing_Browser && !strBrowser.IsEmpty()) ||
				(nDebugUsing == Java_DebugUsing_Standalone && !strStandalone.IsEmpty())));
		}

		return bDebugInfo;
	}
	else
	{
		// we should not hit this; we can't debug without a project
		ASSERT(FALSE);
		return FALSE;
	}


}

BOOL GetJavaBrowser(CString& strBrowser, CString& strBrowserArgs)
{
	// this should only be called if the current project is Java
	ASSERT(pDebugCurr && pDebugCurr->IsJava());

	// get the current project
	if (gpIBldSys != NULL)
	{

		// get the browser name from the project
		// REVIEW(briancr): assume that this is the file name
		// should we worry about any args to the browser?
		gpIBldSys->GetJavaBrowser(ACTIVE_BUILDER, strBrowser);

		// get the html page from the project
		gpIBldSys->GetJavaHTMLPage(ACTIVE_BUILDER, strBrowserArgs);

		return TRUE;
	}

	return FALSE;
}

BOOL GetJavaStandalone(CString& strStandalone, CString& strStandaloneArgs)
{
	// this should only be called if the current project is Java
	ASSERT(pDebugCurr && pDebugCurr->IsJava());

	// get the current project
	if (gpIBldSys != NULL)
	{

		// get the stand-alone interpreter name from the project
		gpIBldSys->GetJavaStandalone(ACTIVE_BUILDER, strStandalone);

		// get the stand-alone interpreter args from the project
		gpIBldSys->GetJavaStandaloneArgs(ACTIVE_BUILDER, strStandaloneArgs);

		// is the stand-alone interpreter jview?
		CString strStandaloneT = strStandalone;
		strStandaloneT.MakeLower();
		if (strStandalone.Find(_T("jview")) != -1)
		{
			// add -p to the command line
			strStandaloneArgs += _T(" -p");

			// add -a to the command line if the type of stand-alone
			// debugging is applet
			ULONG nStandaloneDebug;
			gpIBldSys->GetJavaStandaloneDebug(ACTIVE_BUILDER, &nStandaloneDebug);
			if (nStandaloneDebug == 1)
			{
				strStandaloneArgs += _T(" -a");
			}
		}

		// get the class name
		CString strClassName;
		gpIBldSys->GetJavaClassName(ACTIVE_BUILDER, strClassName);
		
		// append it to the args
		strStandaloneArgs += _T(" ") + strClassName;

		// get the class args
		CString strClassArgs;
		gpIBldSys->GetProgramRunArguments(ACTIVE_BUILDER, strClassArgs);

		// append to the args
		strStandaloneArgs += _T(" ") + strClassArgs;

		return TRUE;
	}

	return FALSE;
}

