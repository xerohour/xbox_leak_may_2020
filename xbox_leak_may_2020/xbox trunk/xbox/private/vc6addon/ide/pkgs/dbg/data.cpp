#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/****************************************************************************

	GLOBAL VARIABLES :

****************************************************************************/

// Handle to main window
HWND hwndFrame	= NULL;

// Handle to edit control
HWND hwndActiveEdit  = NULL;

//Handle to instance data
HINSTANCE hInst;


//RunDebug Params
RUNDEBUGPARAMS runDebugParams;

//Temporary storage for a file path
char szPath[_MAX_PATH];
char szDrive[_MAX_DRIVE];
char szDir[_MAX_DIR];
char szFName[_MAX_FNAME];
char szExt[_MAX_EXT];

//Temporary string variable used by all modules
char szTmp[TMP_STRING_SIZE];

//Currently highlighted trace line (F8/F10)
TRACEINFO TraceInfo;

//Empty string
char const szNull[1] = "";

// Path where the current executable was created - used as base
// for file searching with source line information
char ExecutablePath[_MAX_PATH];

// Number of dialog/message boxes currently open
int BoxCount = 0 ;

CIDEDoc *pLastDocWin = NULL;

//Autotest mode for test suites
BOOL AutoTest = FALSE;

BOOL fExcepListInitialized = FALSE;

// Messages for just-in-time debugging
//
UINT WU_QUERYJIT = RegisterWindowMessage("QueryJit");
UINT WU_BEGINJIT = RegisterWindowMessage("BeginJit");

//
// Indicates that we're loading the EXE project for JIT debugging
//
BOOL fLoadingJitProject = FALSE;

BOOL fVisTestPresent = FALSE; // Is the Visual Test package present.
BOOL fVisTestDebugActive = FALSE; // Is a Visual Test debug session active currently.

//Full Screen Data
FULLSCREENDATA FullScreenData;

//Debugger window view
CIDEView *	pViewCpu = (CIDEView *)NULL;
CIDEView *	pViewMemory = (CIDEView *)NULL;
CIDEView *	pViewCalls = (CIDEView *)NULL;
CIDEView *	pViewDisassy = (CIDEView *)NULL;
CIDEView *	pViewNewWatch = (CIDEView *)NULL;
CIDEView *	pViewNewLocals = (CIDEView *)NULL;

//CrLf
char const CrLf[3] = "\r\n";

HWND    g_hWndShell;

UINT WM_VIEWACTIVATE = RegisterWindowMessage("ViewActivate");

DWORD g_dwDragDelay          = (DWORD)::GetProfileInt(_T("windows"), _T("DragDelay"), DD_DEFDRAGDELAY);
DWORD g_dwDragScrollInset    = (DWORD)::GetProfileInt(_T("windows"), _T("DragScrollInset"), DD_DEFSCROLLINSET);
DWORD g_dwDragScrollDelay    = (DWORD)::GetProfileInt(_T("windows"), _T("DragScrollDelay"), DD_DEFSCROLLDELAY);
DWORD g_dwDragScrollInterval = (DWORD)::GetProfileInt(_T("windows"), _T("DragScrollInterval"), DD_DEFSCROLLINTERVAL);
DWORD g_dwDragMinRadius      = (DWORD)::GetProfileInt(_T("windows"), _T("DragMinDist"), DD_DEFDRAGMINDIST);

