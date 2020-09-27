#include "stdafx.h"
#pragma hdrstop
#include "fbrdbg.h"
#include "idbgproj.h"

void
InitializeExecutionExtTable(
	HINSTANCE	hInst
	);

////////////////////////////////////////////////////////////////////////////
// This function will be called by the shell right after it loads our DLL.
// We must register our DLL with MFC and the shell and can then do any
// other initialization that is required.  We return TRUE if initialization
// goes okay, or FALSE if this package cannot be started.  If FALSE is
// returned, the DLL will be unloaded and ExitPackage() will not be called.
// Make sure to verify that TRUE will be returned BEFORE registering!
//
extern "C" BOOL PASCAL AFX_EXPORT InitPackage(HWND hWndShell)
{
	// This registers our package with the MSVC shell...
	//
	CDebugPackage* pPackage = new CDebugPackage;

	theApp.RegisterPackage(pPackage);


	g_hWndShell = hWndShell;

    return( TRUE );
}

void InitDefaults(void)
{
	InitDefaultRunDebugParams(&runDebugParams);

	g_persistWatch.InitDefault();
	g_persistVars.InitDefault();
}

BOOL InitDebugPackage(HINSTANCE hInst)
{
    WNDCLASS    wc;

	//Register the main window class
	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = NULL;
	wc.hbrBackground = NULL;
	wc.hCursor = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _T("MSVCDBG50");

	if (!RegisterClass (&wc) )
		return( FALSE );

	//Create the frame
	hwndFrame = CreateWindow(_T("MSVCDBG50"), NULL,
							 WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
							 CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
							 NULL, NULL, hInst, NULL);

	if ( !hwndFrame )
		return( FALSE );
	
	// Grab required interfaces - abort if we don't get them all
	//
	if (!SUCCEEDED(theApp.FindInterface(IID_ISrcUtil, (LPVOID FAR *)&gpISrc)))
	{
		goto L_ErrorNoInterface;
	}

	if (!SUCCEEDED(theApp.FindInterface(IID_ISourceQuery, (LPVOID FAR *)&gpISrcQuery)))
	{
		goto L_ErrorNoInterface;
	}

	if (!SUCCEEDED(theApp.FindInterface(IID_IOutputWindow, (LPVOID FAR *)&gpIOutputWindow)))
	{
		goto L_ErrorNoInterface;
	}
	
	if (!SUCCEEDED(theApp.FindInterface(IID_IBuildPlatforms, (LPVOID FAR *)&gpIBldPlatforms)))
	{
		goto L_ErrorNoInterface;
	}

	if (!SUCCEEDED(theApp.FindInterface(IID_IBuildSystem, (LPVOID FAR *)&gpIBldSys)))
	{
		goto L_ErrorNoInterface;
	}

	if (!SUCCEEDED(theApp.FindInterface(IID_IProjectWorkspace, (LPVOID FAR *)&gpIProjectWorkspace)))
	{
		goto L_ErrorNoInterface;
	}

	ASSERT(gpISrc && gpISrcQuery && gpIBldPlatforms && gpIBldSys && gpIOutputWindow && gpIProjectWorkspace);

	if (DEBUGGlobalInit())
	{
		InitDefaults();
		extern void LoadIniFile();
		LoadIniFile();
		UpdateRadix(runDebugParams.decimalRadix);
		InitializeExecutionExtTable (hInst);
		return( TRUE );
	}

	return( FALSE );

L_ErrorNoInterface:
	MsgBox(FatalError, IDS_NOINTERFACE);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
// This function will be called by the shell when it is terminating.
// Perform any necessary cleanup at this point.  This DLL will be unloaded
// immediately after this function is called.
//
extern "C" void AFX_API_EXPORT ExitPackage()
{
return;
}

void CDebugPackage::OnExit(void)
{
if ( gpISrc )
gpISrc->Release();

if ( gpISrcQuery )
gpISrcQuery->Release();

if ( gpIBldSys )
gpIBldSys->Release();

if ( gpIBldPlatforms )
gpIBldPlatforms->Release();

if ( gpIOutputWindow )
gpIOutputWindow->Release();

if ( gpActiveIDBGProj)
gpActiveIDBGProj->Release();

if ( gpActiveIPkgProject)
gpActiveIPkgProject->Release();

if ( gpIProjectWorkspace )
gpIProjectWorkspace->Release();

DEBUGDestroy();						// new for 5.0: tell debugger DLLs to go away

#ifdef _INSTRAPI
LogClose(instrLog) ;
instrLog = 0 ;
#endif

// Clear the various debug/build lists:
ClearBreakpointNodeList();
ClearPathMappings();

// Free memory used by grep & exceptions
FreeList();

DestroyWindow(hwndFrame);
}

BOOL CDebugPackage::OnInit(void)
{

if (!InitDebugPackage (hInst)) {
return FALSE;
}

//
// These are commands which are only valid when the CPP package is
// loaded => native debugger support.
//

UINT rgCPPOnlyCmds [] =
{
IDM_RUN_ATTACH_TO_ACTIVE
};

if (!(IsPackageLoaded (PACKAGE_LANGCPP)||IsPackageLoaded(PACKAGE_LANGFOR)))
{
for (int i = 0;
	 i < sizeof (rgCPPOnlyCmds) / sizeof (rgCPPOnlyCmds [0]);
	 i++)
{
	ShowCommandUI (rgCPPOnlyCmds [i], FALSE);
}
}


fVisTestPresent = IsPackageLoaded(PACKAGE_LANGMST);

if ( gpIBldSys && gpIBldSys->IsActiveBuilderValid() == S_OK )
{
int i;

// These are the commands which are shared between VC and Visual Test
// Disable them iff Visual Test is not present as well.
UINT rgSharedDbgCmds[ ] =
{
	IDM_RUN_GO,
	IDM_RUN_STOPDEBUGGING,
	IDM_RUN_BREAK,
	IDM_RUN_STEPOVER,
	IDM_RUN_TRACEINTO,
	IDM_RUN_TOGGLEBREAK,
	IDM_RUN_STOPAFTERRETURN,
};

if ( !fVisTestPresent )
{
	for ( i = 0 ; i < sizeof(rgSharedDbgCmds)/sizeof(UINT) ; i++ )
		ShowCommandUI(rgSharedDbgCmds[i], FALSE);
}

// Disable debugger specific commands which are not
// meaningful when the build system is not available.
UINT rgVCOnlyCmds[ ] =
{
	IDM_RUN_RESTART,
	IDM_RUN_CLEARALLBREAK,
	IDM_RUN_DISABLEALLBREAK,
	IDM_RUN_ENABLEBREAK,
	IDM_RUN_TOCURSOR,
	IDM_RUN_SETNEXTSTMT,
	IDM_RUN_QUICKWATCH,
	IDM_GOTO_DISASSY,
	IDM_GOTO_SOURCE,
	IDM_GOTO_CODE,
	IDM_DEBUG_ACTIVE,
	IDM_RUN_EXCEPTIONS,
	IDM_RUN_THREADS,
	IDM_RUN_FIBERS,
	IDM_RUN_SETBREAK,
	IDM_ACTIVATE_WATCH,
	IDM_ACTIVATE_VARIABLES,
	IDM_ACTIVATE_REGISTERS,
	IDM_ACTIVATE_MEMORY,
	IDM_ACTIVATE_CALLSTACK,
	IDM_ACTIVATE_DISASSEMBLY,
	IDM_TOGGLE_WATCH,
	IDM_TOGGLE_VARIABLES,
	IDM_TOGGLE_CALLSTACK,
	IDM_TOGGLE_DISASSEMBLY,
	IDM_TOGGLE_MEMORY,
	IDM_TOGGLE_REGISTERS,
	IDM_DEBUG_RADIX,
	IDM_MEMORY_NEXTFORMAT,
	IDM_MEMORY_PREVFORMAT,
	IDM_DBGSHOW_FLOAT,
	IDM_DBGSHOW_STACKTYPES,
	IDM_DBGSHOW_STACKVALUES,
	IDM_DBGSHOW_SOURCE,
	IDM_DBGSHOW_CODEBYTES,
	IDM_DBGSHOW_MEMBYTE,
	IDM_DBGSHOW_MEMSHORT,
	IDM_DBGSHOW_MEMLONG,
	IDM_DBGSHOW_MEMORY_BAR,
	IDM_RUN_TRACEFUNCTION,
	IDM_DBGSHOW_VARS_BAR,
};		

for ( i = 0 ; i < sizeof(rgVCOnlyCmds)/sizeof(UINT) ; i++ )
	ShowCommandUI(rgVCOnlyCmds[i], FALSE);



}

if(runDebugParams.fFiberDebugging){
ShowCommandUI(IDM_RUN_FIBERS,TRUE);
} else {
ShowCommandUI(IDM_RUN_FIBERS,FALSE);
}

InitColors();
RegisterDevSoundEvent(IDS_SOUND_DEBUG_HITBP, IDS_SOUND_DEBUG_HITBP2);

return( TRUE );
}

/****************************************************************************

FUNCTION:   InitDefaultRunDebugParams


PURPOSE:    Reset the QpQc Run/Debug params to their default
		values

RETURN:   void
****************************************************************************/

VOID InitDefaultRunDebugParams(LPRUNDEBUGPARAMS pParams)
{
	// Toolbars

	pParams->commandLine[0]     = 0;
	pParams->animateSpeed       = FAST_SPEED;
	pParams->debugMode          = SOFT_DEBUG;
	pParams->fOleRpc            = TRUE;
	pParams->fReturnValues      = TRUE;
	pParams->fDataTips          = TRUE;
	pParams->fWatchExt          = TRUE;
	pParams->decimalRadix       = TRUE;
	pParams->fFiberDebugging	= FALSE;

	
	pParams->fLocalsToolbar     = TRUE;
	pParams->fWatchToolbar      = TRUE;
    pParams->fMemoryToolbar     = TRUE;

	// Call stack window
	pParams->fCallParams        = TRUE;
	pParams->fCallTypes         = TRUE;

	// Disassembly window
	pParams->fDAMAnnotate       = TRUE;
	pParams->fDAMCodeBytes      = FALSE;
	pParams->fDAMLowerCase      = TRUE;
	pParams->fDAMSymbols        = TRUE;

	// CPU window
	pParams->fCPURegs           = TRUE;
	pParams->fCPUFlags          = TRUE;
	pParams->fCPUEA             = TRUE;
	pParams->fCPUFloat          = TRUE;

	// Memory Window
	pParams->strExpr.Empty();
	pParams->iMWFormat          = MW_BYTE;
	pParams->fMWLive            = FALSE;
	pParams->fMWShowRaw         = FALSE;
	pParams->iMWItemsPerLine    = (UINT)-1;
	pParams->iMWAutoTypes       = MW_NO_AUTO_TYPE;

	// Init watch/variables window.
	pParams->iWatchNameColWidth = 0;	//  0 implies unspecified width.
	pParams->iVarsNameColWidth  = 0;
	pParams->iQWNameColWidth    = 0;
	pParams->fVarsToolbar       = TRUE;

	// Edit and Continue settings
	pParams->fENCDebugging      = TRUE;
	pParams->fENCRelink			= TRUE;
	pParams->fENCRepro			= FALSE;
	pParams->fENCLoadIdb		= TRUE;
	pParams->iENCTraceLevel		= 2;


#ifdef CODECOVERAGE
	pParams->fCodeCoverage      = TRUE;
#endif
}



bool
IsCxxOperator(
	char	ch
	)
{
	return (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' ||
			ch == '^' || ch == '&' || ch == '|' || ch == '~' || ch == '!' ||
			ch == '=' || ch == '<' || ch == '>' || ch == ',' || ch == '*' ||
			ch == '(' || ch == ')' || ch == '[' || ch == ']');
}

void
InitializeExecutionExtTable(
	HINSTANCE	hInst
	)
{
	TCHAR	szAutoName [_MAX_PATH];
	TCHAR   szDrive[_MAX_DRIVE];
	TCHAR   szDir  [_MAX_DIR];
	TCHAR   szFile [_MAX_FNAME];
	TCHAR   szExt  [_MAX_EXT];
	char*	buf = NULL;
	ULONG	dwSize = 0;
	char	bufT [10];
	char*	pchT;
	ULONG	sizeOfBuffer = 0;
	
	_tsplitpath (theApp.m_pszHelpFilePath, szDrive, szDir, szFile, szExt);
	_tmakepath (szAutoName, szDrive, szDir, "autoexp", "dat");


	sizeOfBuffer = 2000;
	dwSize = 0;
	
	do {

		delete buf;
		buf = NULL;

		sizeOfBuffer *= 2;
		buf = new char [sizeOfBuffer];

		dwSize = GetPrivateProfileString ("ExecutionControl",
										  NULL,
										  "",
										  buf,
										  sizeOfBuffer,
										  szAutoName);

	} while (dwSize == sizeOfBuffer - 2);
		

	for (pchT = buf; pchT && *pchT; pchT += strlen (pchT) + 1) {

		CString	key = pchT;
		CString	val;
		
		GetPrivateProfileString ("ExecutionControl",
								 key,
								 "",
								 val.GetBuffer (256),
								 256,
								 szAutoName);

		val.ReleaseBuffer ();

		//
		// This solves the problem of CString::operator= and the like
		//


		if (IsCxxOperator (val [0])) {

			int i = 0;
		
			while (IsCxxOperator (val [i])) {
				key += val [i++];
			}

			val = val.Mid (i, val.GetLength () + i);
		}

		if (!val.IsEmpty () && val.CompareNoCase ("NoStepInto") == 00) {
			VERIFY (g_ExecutionExts.Enter (key, FUNCTION_NOSTEPINTO));
		}
	}


	delete buf;
	buf = NULL;
}

	
		
								 
									  
	
	

	
