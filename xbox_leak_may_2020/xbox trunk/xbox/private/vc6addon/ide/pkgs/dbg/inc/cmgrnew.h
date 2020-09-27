#ifndef CMGRNEW_H
#define CMGRNEW_H

#include "path.h"		// for CPath
#include "msgboxes.h"	// for MsgBoxType

#define DOUG_HACK // enable Doug Rosen's temp hacks

// Process ID.  Note that ULONG is used because it's (hoped to be)
// OS-independent; code shouldn't assume that a PID is actually exactly
// 4 bytes (it may be just 2).
typedef ULONG PID;

// StepN return codes
#define STEPN_NOERROR      0   // StepN with no error
#define STEPN_BREAK        1   // StepN with BP break
#define STEPN_NOENTRY      2   // StepN with no entry point
#define STEPN_BPFAILED     3   // StepN with not all BP applied
#define STEPN_FAILED       4   // StepN really failed.   

// Step return codes
#define STEPOUT_NOERROR    0   // Step out no error
#define STEPOUT_NOSOURCE   1   // Step out no source available
#define STEPOUT_NOCALLER   2   // Step out no calling function
#define STEPOUT_NOBPSET    3   // Step out cannot set BP

// Step to cursor return codes
#define STEPCUR_NOERROR    0   // Step to cursor no error
#define STEPCUR_NOCODE     1   // Step to cursor no code at line
#define STEPCUR_NOMOVE     2   // Step to cursor on actual line
#define STEPCUR_NOBPSET    3   // Step to cursor cannot set BP

#define SRCSTEPPING	0
#define ASMSTEPPING	1

// Timer ID for WM_TIMER messages
#define DBGCALLBACKTIMERID	1	// To dequeue debugger callbacks

// This is used for CV messages
#define SHOW_IMMEDIATE		0
#define SHOW_ASYNCHRONOUS   1
#define SHOW_REPORT			2

// Debugger Messages - Generic for ALL debugger windows

// For WU_UPDATEDEBUG the #defines below specify what lParam is
typedef enum {
	// lParam specifies what frame to use.  (See CallStack walking
	// for user)  If lParam is NULL, there is no context change.
	// Probably the result of updating a register, memory, or data
	// Upon updating after debuggee execution, this will point to
	// cxfIp
	wPCXF	= 0,

	// lParam is a rectangle to invalidate for repainting, this
	// will go away whent he newwatch replaces the watch and locals
	// windows.
	wRECT

} UPDATEDEBUGLPARAM;

typedef enum {
	// Sent when the debuggee has restarted and returned from initial
	// execution.  The window should reinitialize itself
	WU_INITDEBUGWIN	= WU_DEBUGGERMSGBASE,

	// Sent after the debuggee has returned from exeution or the UI
	// is being updated as the result of an edit in a debug window...
	// if wParam specifies the meaning of lParam see comments above
	// for values.
	WU_UPDATEDEBUG,

	// Sent when the debuggee has died.  Any debugger memory allocations
	// should be freed here.  After this message is processed, all CodeView
	// memory allocations will be freed.
	WU_CLEARDEBUG,

	// Sent when a MultiEdit window has been resized, or it's contents
	// require the scroll bar(s) range{s} to be updated.
	WU_UPDATESCROLLBARS,

	// Sent when a breakpoint has been added/deleted.
	// lParam should contain information about the BP (file, address, hbpt)???
	WU_BREAKPTCHANGE,

	// Sent when a debug function requires the address of the line
	// where the cursor is.  This is for Execute to cursor, toggle
	// breakpoint, set PC...  lParam contains a pointer to a GCAINFO
	// (description of struct below) packet to be filled in by the window.
	// If this packet is filled in the WndProc should return a gcafXXX enum
	// specified below.
	WU_GETCURSORADDR,

	// Sent when the current window should flush edits before the debuggee
	// is about to restart (only sent to the active debug window)
	WU_FLUSHEDITS,

	// Sent just before resuming execution of the debuggee.  When this message
	// is received, the window should save any data necessary to show
	// differences
	WU_FREEZEDATA,

	// Sent after the debuggee has returned from an edit & continue operation
	WU_UPDATEENC,

	// Place holder for last debugger message
	WU_DEBUGGERMSGMAX

} DEBUGGERMSGS;

// Values to be returned from wndprocs when replying
// to a WU_GETCURSORADDR message
typedef enum {
	// No address ad cursor location or not supported
	gcafNone,

	// Single specific address, addr packet filled in
	gcafAddress,

	// File/Address expression returned, must be parsed
	// to check for multiple occurances
	gcafExpression
} GCAF;

// Information returned by replying wndproc to
// WU_GETCURSORADDR message
typedef struct _gcainfo {
	ADDR	addr;			// Address of cursor (gcafAddress)
	char	szExpr[ 1024 ];	// A VERY large string to
							// reply (gcafExpression)
} GCAINFO;	// Get Cursor Address Information
typedef GCAINFO FAR *	PGCAINFO;

// Go
void PASCAL Go(HTHD,BOOL,BOOL);

void PASCAL StepAndGo(void);

int PASCAL Step(BOOL fOverCalls, STO sto);

// Go to the current cursor position
WORD PASCAL ContinueToCursor(BOOL fPassException, TCHAR * ptchGotoExpression);

// Step to the return address for the current function
WORD PASCAL StepToReturn(BOOL fPassException, int iStepMode );

// Move the instruction pointer to where the cursor is
BOOL PASCAL MoveIPToCursor(int iLine = -1);

// Process one OSDebug callback and queue its return code
void DoCallBackAndQueueReturn(void);

#ifdef _DEBUG
VOID ToggleHackFlipScreen(VOID);
#endif

BOOL TerminateDebuggee(void);

BOOL PASCAL ClearDebuggee(ULONG Reason);

VOID AsyncStopDebuggee(VOID);

BOOL TryingToAsyncStop(VOID);

BOOL PASCAL CheckEXEForDebug(LPCSTR lpszEXE, BOOL InANSI, BOOL fQuiet);

void LoadJITProject(void);

BOOL LoadDebuggee (LPTSTR	lszLocalExe,
				   LPTSTR	lszRemoteExe,
				   LPTSTR	lszDebuggeeArgs,
				   BOOL		fDebugActive,
				   ULONG	DebugActivePid,
				   HANDLE	DebugActiveEvent,
				   BOOL		fRestarting,
				   BOOL		fCrashDump);

BOOL FSpawnOrphan(HPID hpid, LPCTSTR lszRemoteExe, LPCTSTR lszCmdLine,
	LPCTSTR lszRemoteDir, PID FAR *lppid);

BOOL RestartDebuggee(LPTSTR lszLocalExe, LPTSTR lszRemoteExe,
	LPTSTR lszDebuggeeArgs, BOOL fCrashDump);

void SetUpNotification(struct dbg _far *pWDbg);

BOOL DEBUGGlobalInit(VOID);	// Perform 1-time debugger initialization

BOOL DEBUGInitialize (BOOL fQuickRestart = FALSE);

VOID DEBUGDestroy(VOID);	// UnInitialize debugger

BOOL DebuggeeAlive(void);

BOOL DebuggingWin32s(void);

BOOL PASCAL StoppedBecauseOfBrk(void);

// Get name of executable to run.  This is usually the same as the target
// to be built, but if the target is a DLL, this will be different.
IDE_EXPORT EXEFROM GetExecutableFilename(PSTR executable, UINT size);

// Get the name of the last document (source) window that was active.
IDE_EXPORT CDocument * GetLastDocWin();

// Get the name of the current debugging target.  This is usually the same
// as the target to be built, but it may be differernt if you did a
// project-less build and have since then loaded other source files.
BOOL PASCAL GetDebugTarget(PSTR target, UINT cch);

// Set the name which will be returned by later calls to GetDebugTarget.
VOID PASCAL SetDebugTarget(PCSTR target);

VOID PASCAL GetDebuggeeCommandLine(CString& CommandLine);

BOOL PASCAL FFindDLL(LPCTSTR szDLL, CString& strDLLPath, BOOL fQuiet = FALSE);
BOOL PASCAL LoadAdditionalDLLs(
	CStringList *pslDLLs,
	BOOL fStarting,
	BOOL fQuiet = FALSE,
	POSITION *piPos = NULL);

// Return filename portion of a remote path
LPTSTR LtszRemoteFileFromPath(LPCTSTR ltszRemotePath);

// Mappings between local & remote filenames
//VOID BuildMpStrLocalDllStrRemoteDll(CString strLocalDllList, CString strRemoteDllList);
BOOL MapRemoteFileToLocal(LPCTSTR ltszRemote, CString& strLocal);
VOID MapLocalFileToRemote(LPCTSTR ltszLocal, CString& strRemote);

// #defines for UpdateFlags passed to UpdateDebuggerState
#define UPDATE_CPU			0x0001
#define UPDATE_LOCALS		0x0002
#define UPDATE_WATCH  		0x0004
#define UPDATE_SOURCE  		0x0008
#define UPDATE_DEBUGGEE		0x0010
#define UPDATE_NOSETFOCUS	0x0020
#define UPDATE_MEMORY		0x0040
#define UPDATE_CALLS        0x0080
#define UPDATE_DISASSY      0x0100
#define UPDATE_BREAKPT      0x0200
#define UPDATE_ENC			0x0400	 // Indicates special update mode after ENC

// UPDATE_ALLDBGWIN no longer includes the source window.  We do
// not want to update the source window when data edits occur, only
// when we need to redisplay the PC or breakpoints, so use that
// seperately
#define UPDATE_ALLDBGWIN (UPDATE_CPU|UPDATE_LOCALS|UPDATE_WATCH\
						 |UPDATE_MEMORY|UPDATE_CALLS|UPDATE_DISASSY|UPDATE_BREAKPT)
#define UPDATE_ALLSTATES (UPDATE_ALLDBGWIN|UPDATE_SOURCE|UPDATE_DEBUGGEE)


// Sends messages to appropriate windows to update their displays according
// to the value of UpdateFlags
void PASCAL UpdateDebuggerState(WORD UpdateFlags);

// Forces a repaint of all debug windows with the given cxf
VOID PASCAL UpdateAllDebugWindows(struct CXF FAR * pcxf);

//Set in an newly opened file, visual breakpoints and current line
#ifdef DOUG_HACK
void SetDebugLines(CDocument * pTextDoc, BOOL ResetTraceInfo, BOOL fBPOnly = FALSE);
void PASCAL AdjustDebugLines(CDocument * pTDoc, int StartLine, int NumberLines, BOOL fAdded, BOOL fLineBreak, BOOL fWhiteSpaceOnly);
#else
void SetDebugLines(BOOL ResetTraceInfo);
void PASCAL AdjustDebugLines(int StartLine, int NumberLines, BOOL fAdded, BOOL fLineBreak);
#endif // DOUG_HACK

BOOL PASCAL CheckExpression(
	LPCTSTR Expr, int Radix, int Case);

void PASCAL AnimatePeekMessage(
	void);

void PASCAL UpdateRadix(
	BOOL Decimal);

void PASCAL UpdateHexCase(
	BOOL fUpper);

BOOL PASCAL DebuggeeRunning(
	void);

void PASCAL SetDebuggeeRunning(
	BOOL Runnning);

void PASCAL ZapInt3s(void);


// Errors returned by the Unassemble function
typedef enum {
	DIS_NOERROR,		// Unassembling was OK
	DIS_NOEXE,			// Couldn't find a corresponding EXE
	DIS_NOMODULE,		// Couldn't find the corresponding module
	DIS_AMBIGUOUS,		// Ambiguous module
	DIS_NOLINEADDR,		// No code for this line
	DIS_NOFIXUP			// Couldn't get fixup for address
	} DIS_RET ;

// Unassemble the given line in a formatted buffer
DIS_RET PASCAL Unassemble(int,UINT,LPSTR far *) ;

// Get the current line and delta for a given address
BOOL PASCAL GetLineOffsetFromAddr(
	void far *lpaddr,
	int *pLine,
	long *pOffset) ;

// Get the current address for a given line and delta
BOOL PASCAL GetAddrFromLineOffset(
	int Doc,
	UINT uLine,
	UOFFSET uoff,
	void far *lpaddr,
	BOOL fAddrli) ;

// Erases the current CS:IP and returns the doc that owned the CS:IP
CDocument *PASCAL EraseTraceLine(VOID) ;

// Restore the trace line, to be used with EraseTraceLine()
VOID PASCAL RestoreTraceLine(CDocument *, BOOL fEditing = FALSE) ;

// Sets the current caller hilite
void PASCAL SetCallerLine(CDocument *, int, BOOL );

// Erases the current caller hilite
void PASCAL EraseCallerLine(void);

// Debugger message box: give IDE focus & display message box
int DebuggerMsgBox(
	MsgBoxTypes	MsgBoxType,
	LPCTSTR		pszMessage,
	UINT		nButtonIDs		= DEFAULT_BUTTONS,
	UINT		nHelpContext	= DEFAULT_HELP);

// Debugger message box: give IDE focus & display message box
int DebuggerMsgBox(
	MsgBoxTypes	MsgBoxType,
	int			nMessageID,
	UINT		nButtonIDs		= DEFAULT_BUTTONS,
	UINT		nHelpContext	= DEFAULT_HELP);

extern BOOL
SetCrashDumpDebugging(
	BOOL	fCrashDump
	);

#endif	// CMGRHIGH_H (whole file)
