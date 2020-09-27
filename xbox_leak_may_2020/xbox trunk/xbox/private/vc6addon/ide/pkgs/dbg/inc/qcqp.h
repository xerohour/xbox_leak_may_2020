#ifndef QCQP_H
#define QCQP_H

#ifndef OSDEBUG4
#define OSDAPI PASCAL
#endif

#define _pascal		PASCAL
#define _loadds
#define _export
#define _fcalloc	calloc
#define _huge
#define __huge
#define _far
#define __far
#define alloc_text(x,y) warning(disable:4762)	/* disable a warning we will never get in win32! */
#define gUnlockNull	0	/* prevent warnings */

// critical-section macros
#define QCInitCritSection(pcs)		InitializeCriticalSection(pcs)
#define QCLeaveCritSection(pcs)		LeaveCriticalSection(pcs)
#define QCEnterCritSection(pcs)		EnterCriticalSection(pcs)
#define QCDeleteCritSection(pcs)	DeleteCriticalSection(pcs)

// prune the tree of windows crap...

#define NOGDICAPMASKS
#define NOMETAFILE
#define NOSOUND
#define NOCOMM
#define NOKANJI
#define NOICONS
#define NORASTEROPS
#define NOCOLOR
#define NOSERVICE
#define NOPROFILER


// end of pruning...

// turn off min rebuild info for sure on mfc/windows stuff
#pragma component(minrebuild,off)

#if defined (USE_DMALLOC) && defined (_DEBUG)
	#define HOOK_WINDOWS_MEM
	#include <dmalloc.h>
#else
	#define RealGlobalAlloc		GlobalAlloc
	#define RealGlobalLock		GlobalLock
	#define RealGlobalUnlock	GlobalUnlock
	#define RealGlobalSize		GlobalSize
	#define RealGlobalFree		GlobalFree
	#define RealGlobalReAlloc	GlobalReAlloc
#endif

// Packets
// REVIEW: Why not enumerate?
#define PACKET_TEXTEDIT  1
#define PACKET_BASE      2
#define PACKET_MULTI     3

// Dockable Windows
// REVIEW: Why not enumerate?
#define IDDW_VCPP_BASE          (IDDW_PACKAGE_BASE + 0x0100)
#define IDDW_VCPP_WATCH_WIN     (IDDW_VCPP_BASE+1)
#define IDDW_VCPP_VARIABLES_WIN (IDDW_VCPP_BASE+2)
#define IDDW_VCPP_CPU_WIN       (IDDW_VCPP_BASE+3)
#define IDDW_VCPP_MEMORY_WIN    (IDDW_VCPP_BASE+4)
#define IDDW_VCPP_CALLS_WIN     (IDDW_VCPP_BASE+5)
//#define IDDW_VCPP_BROWSE_WIN  (IDDW_VCPP_BASE+6)
#define IDDW_VCPP_DISASSY_WIN   (IDDW_VCPP_BASE+7)
#define IDDW_VCPP_BREAKPT_WIN   (IDDW_VCPP_BASE+8)
#define IDDW_VCPP_OLDWATCH_WIN	(IDDW_VCPP_BASE+9)
#define IDDW_VCPP_LOCALS_WIN    (IDDW_VCPP_BASE+10)

// Offsets from IDTB_VCPP_BASE
#define IDTB_VCPP_BASE          (IDTB_PACKAGE_BASE + 0x0100)
#define IDTB_VCPP_DEBUG			(IDTB_VCPP_BASE+0)


// For all our modules to avoid multiple redefinitions
typedef unsigned short USHORT ;
typedef void * 			PV;			// Pointer to generic Data
typedef LONG			ILINE;
typedef LONG			DCP;		// Delta-CP
typedef ULONG			_CB;
typedef _CB				CP;
typedef CP	*			PCP;
typedef UINT			HDCMP;
typedef UINT			IDCMP;
typedef _CB				IB;
typedef _CB				ICH;		// Character Index
typedef ICH				CCH;		// Count of characters
typedef const char *	PCCH;
typedef HANDLE 			HHEAP;

#define ilineNil (ILINE)-1
#define ichNil (ICH)-1
#define ibNil (IB)-1
#define hdcmpNil (HDCMP)-1
#define cpNil (CP)-1

// turn on min rebuild info for rest of the bld sys stuff in the pch
#pragma component(minrebuild,on)

//Compilation options
#ifndef RC_INVOKED
#ifdef _DEBUG
		#pragma message( "Debugging Version" )
#else
		#pragma message( "Release Version" )
#endif
#endif

// INTERNAL ERROR SECTION
//
// you can add to this section whatever is too far to be checked
// in nested calls and use it with the following functions:
//
// void SetGlolbalError(ERR_TYPE errnum)
// ERR_TYPE CheckGlobalError(void)
// void ClearGlobalError(void)
//
// REVIEW: Why not enumerate?
typedef int ERR_TYPE ;

#define ERR_NOERROR		0
#define ERR_FILELOCK	1
#define ERR_FILEOPEN	2
#define ERR_FILEREAD	3
#define ERR_FILEMAX		4

//Font types and defines

typedef HANDLE HFONT_OBJECT;
typedef HANDLE HCHAR_WIDTHS;

#define NASCII_WIDTHS	256					// Char-width array length for lead byte of 0
#define NTRAIL_WIDTHS  	(0xFC - 0x40 + 1)	// Char-width array length for all other lead bytes
#define NLEAD_BYTES		(0xFC - 0x81 + 1)	// Number of valid lead bytes

typedef struct font {
	LOGFONT			lf;
	HFONT			hFont;
	TEXTMETRIC		tm;
	int				cRef;								// Reference count
	int 			asciiWidths[NASCII_WIDTHS];			// Array of ascii widths
	BOOL			bIsDefault;							// Default font indicator
	HCHAR_WIDTHS 	rgh[NLEAD_BYTES];					// Lead-byte table of handles to char width data
} FONT_OBJECT, FAR *LPFONT_OBJECT;


//Undo/Redo : Type of Editing Action

typedef enum {

		eaMark,							// indicates beginning/end of next set of records
		eaInsertStream,
		eaDeleteStream,
		eaReplaceChar,
		eaNone				// used just to init buffer

} EDIT_ACTION;

// Undo/Redo: What action is next on list to undo/redo
typedef enum {

		mtTyping,
		mtDelete,
		mtReplaceAll,
		mtReplaceOne,
		mtOvertype,
		mtChangeCase,
		mtRecording,
		mtRecordingEnd,
		mtTranspose,
		mtModifyWord,
		mtPaste,
		mtReformat,
		mtMove,
		mtAutomated,	// Automated edits, such as those from ClassWizard, etc.
		mtAutomatedEnd,
		mtFileInsert,
		mtClean,		// If this mark is on the top of either of the stacks, the file is clean
						// w/respect to undo/redo actions
		mtBookmark,		// Not actually recorded, just used to set g_mtMoveType so that the Epsilon
						// bookmark move knows what action occured last
		mtCut,			// Similarly for the Epsilon 'kill' behavior
		mtEpsIndentPrevious,	// And Epsilon's "indent-previous" command
		mtBriefHome,	// And the Brief Home/End behavior
		mtBriefHomeHome,
		mtBriefEnd,
		mtBriefEndEnd,  // Do not fragment the range mtBookmark -> mtBriefEndEnd as it is used in
						// undoredo.cpp to distinguish these marks from *real* undo/redo marks
		mtNil

} MARK_TYPE;


// Undo/Redo : Character classifications for typing
typedef enum {

	ccAlpha,		// includes all non-alpha symbols that aren't punc, space, tab, newline
	ccPunc,
	ccSpace,
	ccTab,
	ccNewline,
	ccNone			// for all non-typing marks

} CHAR_CLASS;


// Undo/Redo : To store DeleteStream's col2, line2, and "type"
typedef struct {

	ILINE	iline;
	IB		ib;

} TXUPT;		// TeXt Undo PoinT

// Undo/Redo : To store InsertStream's len and chars
typedef struct {
	_CB		cbStream;
	char	rgch[1];
} STREAM;

// Undo/redo : Mark data
typedef struct {
	MARK_TYPE	mt;
	CHAR_CLASS	cc;		// for use when mt == mtTyping : class of last char typed
	BOOL		fRestoreSelection;
} MARK;

// Undo/redo : Variant record
typedef union {
	TXUPT		txuptLim;		// Stores the second point for a delete operation
	char		ch;
	STREAM		s;
	MARK		mark;
} X;

// Undo/Redo : Structure of record definition
typedef struct {

	_CB			cbPrevRec;		// MUST BE FIRST FIELD !
	ILINE		ilineFirst;
	IB			ibFirst;

	EDIT_ACTION	edAction;		// Type of logical editing action
	X			x;				// Variant part

} UNDO_REC;
typedef UNDO_REC UNALIGNED *PUNDO_REC;

// Undo/Redo : Size of variant components
#define HDR_INSERTSTREAM_SIZE (sizeof(UNDO_REC) - sizeof(X) + sizeof(STREAM))
#define HDR_DELETESTREAM_SIZE (sizeof(UNDO_REC) - sizeof(X) + sizeof(TXUPT))
#define HDR_REPLACECHAR_SIZE (sizeof (UNDO_REC) - sizeof (X) + sizeof (STREAM))
#define HDR_MARK_SIZE (sizeof(UNDO_REC) - sizeof(X) + sizeof(MARK))

// Undo/Redo: Mark state
typedef enum {

	msNormal,			// Allow marks to be pushed
	msIgnoreNormal,		// Don't allow marks to be pushed -- part of a large edit
	msIgnoreError		// Don't allow marks to be pushed -- had error

} MARK_STATE;

// #3623 [mannyv] 21-Apr-1994.
// #define REC_CANNOTUNDO   -1
// (we should really use LONG_MIN here, but this is fine for now [mikemo])
#define REC_CANNOTUNDO  0x80000001

// Undo/redo : Information in a document.  This must be created by calling "CreateRecBuf"
// in the document's initialization process.
typedef struct {

	HANDLE		hUndoRecBuf;	// Handle to memory, ptr stored in pUndoRecBuf when locked
	_CB			cbUndoBuf;		// No. bytes currently allocated in pUndoRecBuf
	IB			ibCurOffset;	// Offset of current undo/redo rec in buffer
	UINT		cRecsTotal;		// Count of records in the buffer (total)
	UINT		cRecsMark;		// Count of mark records in the buffer (i.e., undoable actions)
	UINT		cRecsMove;		// Count of Move records in the buffer
	MARK_STATE	ms;				// Should marks be pushed/ignored
	UINT		cNestLevel;		// Counter for the nesting level of Begin/EndCompoundEdit
	PUNDO_REC	pUndoRecBuf;	// Pointer to stream of undo/redo records

} UNDO_REC_HDR, *PUNDO_REC_HDR;


//Messages : User messages
// These must be unique, and DEBUGGERMSGBASE must have
// the greatest value.
// REVIEW: why not enumerate? :
// enum {
//   WMU_RELOADFILE = W_USER + 10,
//   WMU_STATUSREADONLY,
//	 ...
// };
#define WMU_RELOADFILE 				(WM_USER + 10)
#define WMU_STATUSREADONLY			(WM_USER + 11)
#define WU_CLOSECHILDCLIENT			(WM_USER + 12)
#define WU_MESSAGEBOX				(WM_USER + 13)
#define WU_REMOTEQUIT				(WM_USER + 14)
#define WU_FINDSYMBOLS				(WM_USER + 15)
#define WU_DEBUGGERMSGBASE			(WM_USER + 16)  // Must be last!

//CPU : Actions
// REVIEW: Why not enumerate?
#define CPU_FIRST			0
#define CPU_LAST			1
#define CPU_FIRSTONLINE		2
#define CPU_LASTONLINE		3
#define CPU_NEXT			4
#define CPU_PREVIOUS		5

//CPU : Areas
// REVIEW: Why not enumerate?
#define CPU_NONE			0
#define CPU_REGISTERSAREA	1
#define CPU_FLAGSAREA		2

// Full Screen : Structure definition
typedef struct {
	BOOL	bFullScreen;
	BOOL	bFirstTime;	// Is this the first time into full screen ever?
	RECT	rectFullScreen;
	BOOL	bVert;	// Were there scroll bars before?
	BOOL	bHorz;
} FULLSCREENDATA;


//Run/Debug : Structure definition
typedef struct {
	char commandLine[50];
	BYTE animateSpeed;
	BYTE debugMode ;
	BOOL extendedRegisters;
	BOOL fHardMode ;
	BOOL fOleRpc;			// whether to trace into OLE remote proc calls
    BOOL fReturnValues;		// Display Return Values in Auto Tab
    BOOL fDataTips;			// Display Data Tips
    BOOL fWatchExt;			// Display Watch window extensions (auto/this)
    BOOL decimalRadix;      // hex or decimal display mode
	BOOL fFiberDebugging;		// whether to enable fiber debugging support

	// Toolbars
	BOOL fLocalsToolbar;	// locals toolbar enabled
	BOOL fWatchToolbar;		// watch window toolbar enabled
	BOOL fMemoryToolbar;	// memory window toolbar enabled

	// Call stack window
	BOOL fCallParams ;
	BOOL fCallTypes ;

	// Disassembly window
	BOOL fDAMAnnotate ;
	BOOL fDAMCodeBytes ;
	BOOL fDAMLowerCase ;
	BOOL fDAMSymbols ;

	// Register window
	BOOL fCPURegs ;
	BOOL fCPUFlags ;
	BOOL fCPUEA ;
	BOOL fCPUFloat ;

	// Memory window
	CString strExpr;
	UINT iMWFormat;
	BOOL fMWLive;
	BOOL fMWShowRaw;
	UINT iMWItemsPerLine;
	int	 iMWAutoTypes;

	// Watch window.
	UINT iWatchNameColWidth;
	UINT iVarsNameColWidth;
	UINT iQWNameColWidth;
	BOOL fVarsToolbar;
	BOOL fUnicodeStrings;
	BOOL fLoadExports;

#ifdef CODECOVERAGE
	BOOL fCodeCoverage;
#endif

    // Edit and Continue
    BOOL fENCDebugging;     // whether to enable edit & continue debugging
    BOOL fENCRelink;        // relink after ENC session?
    UINT iENCTraceLevel;    // Trace level for ENC warning messages 
                            // Messages with level <= iENCTraceLevel will appear in the output window
	BOOL fENCRepro;			// Switch to assist reproducing / debugging ENC test cases
	BOOL fENCLoadIdb;		// load .idb files to detect header dependencies?

} RUNDEBUGPARAMS;
typedef RUNDEBUGPARAMS near *NPRUNDEBUGPARAMS;
typedef RUNDEBUGPARAMS far *LPRUNDEBUGPARAMS;

//Status Bar : Actions
// REVIEW: Why not enumerate?
#define STATUS_SIZE        1
#define STATUS_HIDE        2
#define STATUS_UNHIDE      3

//Status Bar : Display Text type
#define STATUS_INFOTEXT							0
#define STATUS_MENUTEXT							1
#define STATUS_ERRORTEXT						2
#define STATUS_CHILDMENUTEXT					3

//Status Bar : Size of strings components
#define STATUS_OVERTYPE_SIZE 					3
#define STATUS_READONLY_SIZE					4
#define STATUS_LINE_SIZE						5
#define STATUS_COLUMN_SIZE						3
#define STATUS_TID_SIZE							8
#define STATUS_COL_SIZE							3

//Status Bar : Pens and Brushes colors
#define GRAYLIGHT 				0x00C0C0C0
#define GRAYDARK 				0x00808080
#define WHITEBKGD				0x00FFFFFF

#ifdef BYEBYE
//Status Bar : Structure definition
typedef struct {
	BOOL errormsg;								// status text is an error message

	BOOL overtype;								//Overtype status
	char overtypeS[STATUS_OVERTYPE_SIZE + 1];	//Overtype string

	BOOL readOnly;								//ReadOnly status
	char readOnlyS[STATUS_READONLY_SIZE + 1];	//ReadOnly string

	BOOL colSelect;
	char colSelectS[STATUS_COL_SIZE + 1];		//Col string

	int line;									//Current line (Not displayed when values 0)
	char lineS[STATUS_LINE_SIZE + 1];			//Line string

	int column;									//Current column (Not displayed when values 0)
	char columnS[STATUS_COLUMN_SIZE + 1];		//Column string

	DWORD tid;
	char tidS[STATUS_TID_SIZE + 1];				//TID string
	
	UINT lastTxt;								//Last text ressource # loaded
		
} STATUS;
typedef STATUS near *NPSTATUS;
typedef STATUS far *LPSTATUS;
#endif // BYEBYE

//Workspace : Basic window information
typedef struct {
	RECT coord;
	long style;
} WININFO;
typedef WININFO near *NPWININFO;
typedef WININFO far *LPWININFO;



//Editor & Project: Type of file kept
#define 	EDITOR_FILE		0

//Editor : Horizontal scroll ratio (1/5 of the window)
#define SCROLL_RATIO		5

//Editor : Code for No View
#define NOVIEW 						255

//Editor : Standard chars in text files
#define LF 						10
#define CR                 		13
#define TAB                		9
#define BS                 		8

//Editor : Status of a line
// REVIEW: Why not use shift?
// #define COMMENT_LINE				(1 << 0)
// #define MULTISTRING_LINE			(1 << 1)
// ...
#define COMMENT_LINE				0x1		//This line is fully commented
#define MULTISTRING_LINE			0x2  	//This line is a multiline string
#define TAGGED_LINE					0x4  	//Tagged by the user
#define BRKPOINT_LINE				0x8  	//Brk Point Commited
#define CURRENT_LINE				0x10 	//Current line when debugging
#define ERROR_LINE					0x20 	//Line with compile error
#define ASSEMBLY_LINE	  			0x40 	//AL.
#define AFX_LINE					0x080	//In AFX modified block
#define AFX_BEGIN					0x100 	//Start AFX modified block
#define AFX_END						0x200	//End AFX modified block
#define AFX_BITS					(AFX_BEGIN 	|\
									 AFX_LINE	|\
									 AFX_END)
#ifdef CODECOVERAGE
#define COVERAGE_LINE				0x1000	// Has been current line while debugging
#endif
#define CALLER_LINE					0x2000  // Viewed line when walking back
											// the call chain
#define DISABLED_BRKPOINT_LINE	    0x4000  // Disabled Breakpoints
#define MIXED_BRKPOINT_LINE	        0x8000  // Mixed Enabled/Disabled Breakpoints
#define HAS_BRKPOINT	            (BRKPOINT_LINE |\
                                     DISABLED_BRKPOINT_LINE |\
                                     MIXED_BRKPOINT_LINE) // any breakpoint line

//Editor : State when reading a file
// REVIEW: Why not enumerate?
#define END_OF_LINE		0
#define END_OF_FILE 	1
#define END_ABORT 		2

//Editor : Last line convention
#define LAST_LINE	 					MAX_LINE_NUMBER + 1

//Editor : Line status action
enum LINESTATUSACTION
{
	LINESTATUS_ON,
	LINESTATUS_OFF,
	LINESTATUS_TOGGLE
};

#define MAX_USER_LINE		MAX_LINE_SIZE	//Max length of user line
typedef USHORT LSTAT;		// Line STATus


// The selection margin feature allows the editor views to display lines
// starting at a settable offset from the left edge of their window.  This
// margin can be used for line/function selection as well as the display of
// icons indicating the "current line (IP)", breakpoints, etc...
//
// The selection margin is set to DEF_SELECT_MARGIN for all	document views.
// Debug and output views to not have margins by default.
// The margin of a view may be set at any time by simply changing the view's
// cxSelectMargin member and invalidating it.

// Default width of a selection margin for a DOC_WIN (others use 0)
#define DEF_SELECT_MARGIN 20

//Document : Type of document

typedef enum {
	CPU_WIN			= 0,		// registers
	MEMORY_WIN,					// memory
	CALLS_WIN,					// call stack
	DISASSY_WIN,				// disassembly
	PHINWATCH_WIN,				// watch
	PHINLOCALS_WIN,				// variables
	BREAKPT_WIN,				// breakpoints ?? is this used??
	DOC_WIN,					// source document
	UNKNOWN_WIN					// unknown window type
} DOCTYPE;


#if 0
//
//	These are the different types of MDI windows that are of interest to the
//	debugger.
	
enum DBG_WINDOW_TYPE
{
	DBG_WINDOW_UNKNOWN = 0,
	DBG_WINDOW_OUTPUT,
	DBG_WINDOW_WATCH,
	DBG_WINDOW_VARIABLES,
	DBG_WINDOW_REGISTERS,
	DBG_WINDOW_MEMORY,
	DBG_WINDOW_CALL_STACK,
	DBG_WINDOW_DISASSEMBLY,
	DBG_WINDOW_SOURCE
};

#endif
struct DOCK_IDS_TAB {
	UINT	nID;
	UINT	nIDPacket;
	UINT	nIDResource;
	UINT	nIDString;
	DOCTYPE	type;
};

const DOCK_IDS_TAB* DockTabFromID(UINT nID);
const DOCK_IDS_TAB* DockTabFromType(DOCTYPE type);

//Document : Mode when opening files
typedef enum {
	MODE_OPENCREATE		= 0,// Open in new window, prompt to create if not exist
	MODE_CREATE,			// Create in new window
	MODE_OPEN,				// Open in new window, error if not exist
	MODE_DUPLICATE,			// Duplicate window
	MODE_RELOAD,			// Reload file in current window (?)
	MODE_RELOADOTHER,		// Reload file in other window (?)
	MODE_SAMEWINDOW,		// Open in same window, prompt to create if not exist
} FOPENMODE;

//Debugger : Animate speeds
// REVIEW: Why not enumerate?
#define SLOW_SPEED			0
#define MEDIUM_SPEED		1
#define FAST_SPEED			2

//Debugger : Debugging Mode
// REVIEW: Why not enumerate?
#define SOFT_DEBUG			0
#define HARD_DEBUG			1

//Debugger : Special UOFFSET value
#define UOFF_NIL			((UOFFSET) -1)

//Debugger : Breakpoint buffer sizes
#define BKPT_LOCATION_SIZE	128
#define BKPT_WNDPROC_SIZE 	128
#define BKPT_EXPR_SIZE 		128

//Degugger : Breakpoints types
typedef enum
{
	BRK_AT_LOC,
	BRK_AT_LOC_EXPR_TRUE,
	BRK_AT_LOC_EXPR_CHGD,
	BRK_EXPR_TRUE,
	BRK_EXPR_CHGD,
	BRK_AT_WNDPROC,
	BRK_AT_WNDPROC_EXPR_TRUE,
	BRK_AT_WNDPROC_EXPR_CHGD,
	BRK_AT_WNDPROC_MSG_RECVD
} BREAKPOINTACTIONS;

//Debugger : Set Breakpoint structure definition
typedef struct {
	BREAKPOINTACTIONS nAction;
	char szLocation[BKPT_LOCATION_SIZE];
	char szWndProc[BKPT_WNDPROC_SIZE];
	char szExpression[BKPT_EXPR_SIZE];
	UINT wLength;
	UINT MessageClass;
	UINT Message;
} BRKPTSTRUC;

//Debugger : Current debugging informations
typedef struct
{
	CDocument   *pDoc;
	int         CurTraceLine;
	UOFFSET     CurTraceDelta;
	int         StepMode;
#if 0
	BOOL        fInHardMode;
#endif
	BOOL        fModeChanged ;
}  TRACEINFO;

typedef enum {HARDMODE, SOFTMODE, CHECKMODE} DEBUGMODE;

// Structure for STATUS file
typedef struct {
	char ucInitialState;
	RECT rcInit;
} INITSTS;


// DIALOG PARAMETERS Structure
typedef struct {
	LPSTR lpCaption;
	LPSTR lpParam;
} DIALOGPARAM, _far * LPDIALOGPARAM;

// Calling of Dirs Dialog with Params
// REVIEW: Why not enumerate?
#define DIRS_HELP_FOCUS					1
#define DIRS_LIB_FOCUS					2
#define DIRS_INC_FOCUS					3
#define DIRS_MFC_FOCUS					4

//Windows 3.1 defines
#define WM_MDITILEVERTICAL		0
#define WM_MDITILEHORIZONTAL	1
#define SCALABLE_FONTTYPE		0x0004

// Debugging DLL's
#define 	NB_DLLS			5

// Timer ID's
#define TID_RELOADFILES		8
#define TID_PREREAD			9

#define INITIAL_PRELOAD_INTERVAL	2000
#define MINIMUM_PRELOAD_INTERVAL	500

// signed versions of LOWORD/HIWORD macros for routines that take int parms
// and must have the sign extension take place (eg Mouse messages!)
#define SIGNED_LOWORD(x)		((SHORT)LOWORD(x))
#define SIGNED_HIWORD(x)		((SHORT)HIWORD(x))


/****************************************************************************

	CALL BACKS:

****************************************************************************/
extern void ExitInstance();

// WIN32 - change all WndProc prototypes...
long FAR PASCAL EXPORT MainWndProc(HWND, UINT, UINT, LONG);

//Call Back to Handle About Dialog BOX
BOOL FAR PASCAL EXPORT DlgAbout(HWND, UINT, UINT, LONG);

//Call Back to Handle File Print Dialog BOX
BOOL FAR PASCAL EXPORT DlgPrint(HWND, UINT, UINT, LONG);

//Call Back to Handle Page Setup Dialog BOX
BOOL FAR PASCAL EXPORT DlgPageSetup(HWND, UINT, UINT, LONG);

//Call Back to Handle Edit Syntax Dialog BOX
BOOL FAR PASCAL EXPORT DlgSyntax(HWND, UINT, UINT, LONG);

//Call Back to Handle Edit Find Dialog BOX
BOOL FAR PASCAL EXPORT DlgFind(HWND, UINT, UINT, LONG);

//Call Back to Handle Edit Replace Dialog BOX
BOOL FAR PASCAL EXPORT DlgReplace(HWND, UINT, UINT, LONG);

//Call Back to Handle View Line Dialog BOX
BOOL FAR PASCAL EXPORT DlgLine(HWND, UINT, UINT, LONG);

//Call Back to Handle View Function Dialog BOX
BOOL FAR PASCAL EXPORT DlgFunction(HWND, UINT, UINT, LONG);

//Call Back to Handle Debug Calls Dialog BOX
BOOL FAR PASCAL EXPORT DlgCalls(HWND, UINT, UINT, LONG);

//Call Back to Handle Debug Modify Value Dialog BOX
BOOL FAR PASCAL EXPORT DlgModify(HWND, UINT, UINT, LONG);

//Call Back to Handle Debug QuickWatch Dialog BOX
BOOL FAR PASCAL EXPORT DlgQuickW(HWND, UINT, UINT, LONG);

//Call Back to Handle Debug Watch Value Dialog BOX
BOOL FAR PASCAL EXPORT DlgWatch(HWND, UINT, UINT, LONG);

//Call Back to Handle Debug Set Break Point Message Dialog BOX
BOOL FAR PASCAL EXPORT DlgMessage(HWND, UINT, UINT, LONG);

//Call Back to Handle Debug Set Breakpoint Dialog BOX
BOOL FAR PASCAL EXPORT DlgSetBreak(HWND, UINT, UINT, LONG);

//Call Back to Handle Option Tools Dialog BOX
BOOL FAR PASCAL EXPORT DlgTools(HWND, UINT, UINT, LONG);

//Call Back to Handle Tool Arguments Dialog BOX
BOOL FAR PASCAL EXPORT DlgToolArgs(HWND, UINT, UINT, LONG);

//Call Back to Handle Options Workspace Dialog BOX
BOOL FAR PASCAL EXPORT DlgWorkspace(HWND, UINT, UINT, LONG);

//Call Back to Handle Options Directories Dialog BOX
BOOL FAR PASCAL EXPORT DlgDirs(HWND, UINT, UINT, LONG);

//Call Back to Handle Options Colors Dialog BOX
BOOL FAR PASCAL EXPORT DlgColors(HWND, UINT, UINT, LONG);

//Call Back to Handle Fonts Select Dialog BOX
BOOL FAR PASCAL EXPORT DlgFonts(HWND, UINT, UINT, LONG);

// call back to handle user control buttons
LONG FAR PASCAL EXPORT QCQPCtrlWndProc (HWND, UINT, UINT, LONG) ;

//Call Back to Handle File Open Save and Open Project
BOOL FAR PASCAL EXPORT DlgFile(HWND, UINT, UINT, LONG);

//Call Back to Handle Edit Confirm Replace Dialog BOX
BOOL FAR PASCAL EXPORT DlgConfirm(HWND, UINT, UINT, LONG);

//Call Back to Handle Edit Find Next Dialog BOX
BOOL FAR PASCAL EXPORT DlgFindNext(HWND, UINT, UINT, LONG);

BOOL FAR PASCAL DlgMemory(HWND,unsigned,UINT,LONG);

#define FMultiEditView(pv)	((pv)->IsKindOf(RUNTIME_CLASS(CMultiEdit)))
#define FMultiEditDocType(dt) ((dt)==MEMORY_WIN||(dt)==CALLS_WIN||(dt)==DISASSY_WIN||(dt)==CPU_WIN)

class CMultiEdit;

void OnViewActivate( CMultiEdit *, BOOL );

//	inline helper functions.

inline BOOL IsCmdFromPopupMenu(CCmdUI * pCmdUI)
{
	return ((pCmdUI->m_pMenu != NULL) ||
		CBMenuPopup::IsCBMenuCmdUI(pCmdUI));
}

// For functions that we export up to the shell:

#ifdef __cplusplus
#define IDE_EXPORT extern "C" __declspec (dllexport)
#else
#define IDE_EXPORT __declspec (dllexport)
#endif	// __cplusplus

// Include CodeView types, prototypes, and variables

#define INC_ASSERT
#define INC_CVINFO
#define INC_CW
#define INC_EXPRESSIONS
#define INC_GLOBALS
#define INC_LINKEDLIST
#define INC_MEMORY
#define INC_MESSAGES
#define INC_OSDEBUG
#define INC_PROTOTYPES
#define INC_REGISTERS
#define INC_SYMBOLS
#define GUIDE
#include "cv.h"

#endif

#include "path.h" // For BAM-BAM _chdir fix

