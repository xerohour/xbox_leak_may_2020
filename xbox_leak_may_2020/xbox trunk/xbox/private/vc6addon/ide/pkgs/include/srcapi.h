/////////////////////////////////////////////////////////////////////////////
//	SRCAPI.H
//		Source package interface declarations.

#ifndef __SRCAPI_H__
#define __SRCAPI_H__

#ifndef STRICT
typedef DWORD HLEXER;
typedef DWORD HPARSER;
typedef DWORD HBSC;
typedef DWORD HNCB;
#else	// STRICT
DECLARE_HANDLE(HLEXER);
DECLARE_HANDLE(HPARSER);
DECLARE_HANDLE(HBSC);
DECLARE_HANDLE(HNCB);
#endif	// STRICT

interface ISourceEdit;
interface ISourceQuery;
interface IParserDataBase;
interface IEditDebugStatus;
interface IParserAssist;
interface IOutputWindow;
interface IFileAccess;

interface ITextDocument;
interface ITextSelection;

typedef ISourceEdit* LPSOURCEEDIT;
typedef ISourceQuery* LPSOURCEQUERY;
typedef IParserDataBase* LPPARSERDATABASE;
typedef IEditDebugStatus* LPEDITDEBUGSTATUS;
typedef IParserAssist* LPPARSERASSIST;
typedef IOutputWindow* LPOUTPUTWINDOW;
typedef IFileAccess* LPFILEACCESS;

enum EXEFROM	// where did GetExecutableFilename get name from?
{
	exefromNone,	// nowhere (no name found)
	exefromProj,	// from project
	exefromLastDoc,	// from last open src window
	exefromExe,		// from currently active exe window
	exefromPid,		// from PID on command line (just-in-time debugging)
};

////////////////////////////////////////////////////////////////////////////////
//	ISourceEdit interface
//
//	A pointer to this interface can be obtained via the ISourceQuery interface.  (See below)
//
//	To edit a source file via this interface, use the following sequence of steps:
//
//		BeginEdits()
//		<perform edits, queries>
//		EndEdits()
//		CommitChanges()		/* if you want your changes saved to disk */
//
//	The editing methods will fail unless BeginEdit is called first.
//
//	Other notes:
//
//			THIS IS NOT YET THREAD-SAFE!
//			Line numbers are zero-based.

typedef enum { 
	BREAKPOINT,
	CURRENT_DEBUG,    // instruction pointer
	DISABLED_BREAKPOINT,
	MIXED_BREAKPOINT,
	CALLER_DEBUG      // instruction pointer in stack
} LineMark;

typedef enum { SET, CLEAR, TOGGLE } LineMarkAction;

typedef void (*pfnLineChangeCallback)(LPCSTR LPCSTRFileName, ULONG iDocID, ULONG ilineStart, ULONG cLines, BOOL fAdded);	// See RegisterLineChangeCallback (below)

#undef INTERFACE
#define INTERFACE	ISourceEdit

DECLARE_INTERFACE_(ISourceEdit, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// ISourceEdit methods : Query
	STDMETHOD(GetLineText)(THIS_ ULONG, LPCSTR *ppstr, ULONG *pcb) PURE;
	STDMETHOD(GetLineTextSansCrLf)(THIS_ ULONG, LPCSTR *ppstr, ULONG *pcb) PURE;
	STDMETHOD(GetTextRunAtFileOffset)(ULONG cpStart, LPCSTR *ppsz, ULONG *pcb) PURE;
	STDMETHOD(GetLanguage)(CString& str) PURE; 	// Non-standard COM.
	STDMETHOD(GetLineColumnFromFileOffset)(ULONG cpOffset, ULONG *pIline, ULONG *pIb) PURE;
	STDMETHOD(GetLineFromFileOffset)(ULONG cpOffset, ULONG *piline) PURE;
	STDMETHOD(FFileOffsetAtBOLN)(ULONG cpOffset) PURE;
	STDMETHOD(GetFileOffsetFromLine)(ULONG iline, ULONG *pcpOffset) PURE;
	STDMETHOD(FUserModified)(VOID) PURE;
	STDMETHOD(FFileWriteable)(VOID) PURE;
	STDMETHOD(GetCaretLine)(ULONG *piLine) PURE;
	STDMETHOD(GetCaretColumn)(ULONG *piColumn) PURE; // column of caret -- view coords, tabs expanded
    STDMETHOD(GetCaretIndex)(ULONG *pib) PURE;       // index to byte at caret -- doc coords, tabs not expanded
	STDMETHOD(GetPath)(CString& str) PURE; 	// Non-standard COM.
	STDMETHOD(CbDoc)(ULONG *pul) PURE;
	STDMETHOD(CLines)(ULONG *pul) PURE;
	STDMETHOD(DocID)(ULONG *pul) PURE;

	// ISourceEdit methods : Edit

	/*
	**	RevertContents	--	Restores the contents of the document back to that which
	**						existed prior to the call to BeginEdits.
	**				**	->	NOTE:  This must be called after BeginEdits, no later than
	**						immediately after EndEdits!  We're just playing back the last
	**						action(s) on the undo stack (if the doc is open in the editor),
	**						so we don't want the user to add to it first.
	*/
	STDMETHOD(BeginEdits)(BOOL fKeepUndoBuffer, BOOL fSuspendUI) PURE;
	STDMETHOD(EndEdits)(VOID) PURE;
	STDMETHOD(RevertContents)(VOID) PURE;
	STDMETHOD(CommitChanges)(VOID) PURE;
	STDMETHOD(InsertTextWithCrLf)(LPCSTR psz, ULONG cbInsert, ULONG ilineInsert, ULONG ibInsert) PURE;
	STDMETHOD(InsertText)(LPCSTR psz, ULONG cbInsert, ULONG ilineInsert, ULONG ibInsert) PURE;
	STDMETHOD(DeleteLine)(ULONG iline) PURE;
	STDMETHOD(DeleteText)(ULONG iline, ULONG ibStart, ULONG ibLim) PURE;
	STDMETHOD(DeleteText)(ULONG ilineStart, ULONG ibStart, ULONG ilineLast, ULONG ibLim) PURE;
	STDMETHOD(EnsureFileEndsWithCrLf)(VOID) PURE;

	// ISourceEdit methods : Search
	STDMETHOD(FindStr)(ULONG cpSearchStart, char *strFind, ULONG *pilineHit, ULONG *pibHit) PURE;

	/*
	**	RegisterLineChangeCallback	--
	**
	**	Register a function to be called from the editor every time lines of
	**	text (cLines) are added or deleted.
	**
	**	If fAdded == TRUE, lines were added, otherwise, lines were deleted.
	**	Insertions are always performed at column 0 (or BEFORE) ilineStart.
	**	Deletions are always performed INCLUDING ilineStart.
	*/
	STDMETHOD(RegisterLineChangeCallback)(pfnLineChangeCallback) PURE;

	// ISourceEdit methods : Misc.

	/*
	**	MarkLine		--	Adds/deletes/toggles a glyph on the line
	**	IsLineMarked	--	Indicates whether or not a glyph is set on the line
	**	FHasOpenView	--	Is there currently UI associated with this interface's doc?
	**	SetCaretPosInActiveView
	**					--	Set the caret position in the view associated with the document.
	**					--	Only sets focus to that view if fSetFocus is TRUE.
	**					--	If fManageBPFocus is true, the function will 1) bring the breakpoint
	**						into view and 2) keep focus in the previously-active window if it
	**						was other than a text window.  ** fSetFocus ** overrides this behavior!
	*/
	STDMETHOD(MarkLine)(ULONG iline, LineMark lm, LineMarkAction lma) PURE;
	STDMETHOD(IsLineMarked)(ULONG iline, LineMark lm) PURE;
	STDMETHOD(FHasOpenView)(VOID) PURE;
	STDMETHOD(SetCaretPosInActiveView)(ULONG iline, ULONG ich, BOOL fSetFocus, BOOL fManageBPFocus) PURE;

	/*
	**	WriteContentsToTempFile	--	Write the current contents of the document to
	**								memory and return a pointer to the beginning of the file.
	**						**	->	NOTE:  THIS MUST BE CALLED FROM THE MAIN THREAD!
	**	ReleaseTempFile			--	Destroy the contents of the temporary file.
	*/
	STDMETHOD(WriteContentsToTempFile)(LPCCH *ppFileContents, ULONG *pcbFile) PURE;
	STDMETHOD(ReleaseTempFile)(VOID) PURE;
	STDMETHOD(SetLexer)(LPCTSTR) PURE;	
	STDMETHOD(GetLXS)(ULONG iLine, ULONG *plxs) PURE;
	STDMETHOD(GetLexer)(LPVOID *plexer) PURE;

	/* Automation interaction */

	STDMETHOD(GetITextDocument)(ITextDocument **pTextDoc) PURE;
	STDMETHOD(GetITextSelection)(ITextSelection **pTextSel) PURE;

	// Debugger will use this function to get a "pseudo-path" for
	// code that doesn't live on the file system (SQL SP's for instance)
	STDMETHOD(GetPseudoPathName)(LPTSTR ppszPathName, ULONG cbBufferSize, ULONG* pcbStrLen) PURE;
	STDMETHOD(SetPseudoPathName)(LPCTSTR pszPathName) PURE;

	// converts a byte index into a line to a view column index (with tabs expanded)
	STDMETHOD(GetIchViewFromIbDoc)(ULONG *plColumn, ULONG ibDoc, ULONG iline) PURE;

	// Is there a selection? S_OK = has active selection : S_FALSE = no selection
	STDMETHOD(FHasSelection)(VOID) PURE;

	// Line # and byte index (Doc coordinates) of selection start/end
	STDMETHOD(GetSelection)(ULONG * pilineStart, ULONG *pulIndexStart, ULONG * pilineEnd, ULONG *pulIndexEnd, BOOL *pbColumnSelection) PURE;

	// converts a view column index (with tabs expanded) into a line to a byte index
	STDMETHOD(GetIbDocFromIchView)(ULONG *plIndex, ULONG ichViewCol, ULONG iline) PURE;

	STDMETHOD(GetSourceBuffer)(THIS_ ULONG iStartLine, ULONG iEndLine, ULONG cbBuffer, LPSTR pBuf, ULONG *pcb) PURE;
};

///////////////////////////////////////////////////////////////////////////////
//	ISourceQuery interface

typedef enum { gbrvUnavailable = -1, gbrvError, gbrvOkay } GetBscRetVal;

#undef INTERFACE
#define INTERFACE	ISourceQuery

typedef enum { CB_FILE_SAVE_AS, CB_FILE_OPEN, CB_FILE_CLOSE, CB_FILE_PRE_SAVE } SOURCE_CALLBACK_TYPE;

typedef void (*pfnFileOpenCallback)(LPCSTR LPCSTRFileName, ULONG iDocID);       // Correspond to the enum above:
typedef void (*pfnFileCloseCallback)(LPCSTR LPCSTRFileName, ULONG iDocID);      // See RegisterEditorCallback
typedef void (*pfnFileSaveAsCallback)(LPCSTR LPCSTRFileName, ULONG iDocID);
typedef void (*pfnFilePreSaveCallback)(LPCTSTR lpctstrFileName, ULONG iDocID, BOOL bSaveAs, int*);

DECLARE_INTERFACE_(ISourceQuery, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	/*
	** ISource methods
	**
	** CreateSourceEditForDoc		-- file may or may not already be open in the editor
	** CreateSourceEditForOpenDoc	-- only succeeds if file is already open in the editor
	** CreateSourceEditForActiveDoc -- returns an interface for the currently active document
	** CreateNewSourceEdit			-- creates a new document with the given name (NULL ok)
	** GotoFileLineCol				-- put the editor at the given file, line, column
	*/

	STDMETHOD(CreateSourceEditForDoc)(THIS_ LPCSTR pctstrFullPath, ISourceEdit **ppISourceEdit, BOOL fWriteable) PURE;
	STDMETHOD(CreateSourceEditForOpenDoc)(THIS_ LPCSTR pctstrFullPath, ISourceEdit **ppISourceEdit, BOOL fWriteable) PURE;
	STDMETHOD(CreateSourceEditForActiveDoc)(THIS_ ISourceEdit **ppISourceEdit, BOOL fWriteable) PURE;
	STDMETHOD(CreateNewSourceEdit)(THIS_ LPCSTR pctstrFullPath, ISourceEdit **ppISourceEdit, BOOL fWriteable) PURE;
	STDMETHOD(GotoFileLineColumn)(THIS_ LPCSTR pctstrFullPath, ULONG iline, ULONG ich) PURE;
	STDMETHOD(GotoFileClassMember)(THIS_ LPCSTR lpszFileName, LPCSTR lpszClassName, LPCSTR lpszMemberName) PURE;
	STDMETHOD(RegisterEditorCallback)(THIS_ void *pfnCallback, SOURCE_CALLBACK_TYPE sct, INT *iHandle) PURE;
	STDMETHOD(UnregisterEditorCallback)(THIS_ INT hCallback, SOURCE_CALLBACK_TYPE sct) PURE;
	STDMETHOD(IsFileOpen)(THIS_ LPCSTR lpstrFullPath) PURE;
	STDMETHOD(IsEmulationVC20)(VOID) PURE;
	STDMETHOD(IsSaveBeforeRunning)(VOID) PURE;
	STDMETHOD(IsPromptBeforeSaving)(VOID) PURE;
	STDMETHOD(FindHeaderFile)(THIS_ LPCSTR lpszFileName, BOOL fUseCurView, LPTSTR pszPathName, ULONG cbPathSize, ULONG* pcbPathLen ) PURE;

	/*
	** FUTURE: This function is used by Test to tell the VC debugger that it is
	** starting or ending a Test debug session. It belongs in an independent interface.
	*/
	STDMETHOD(VTestDebugActive)(THIS_ BOOL *bActive) PURE;
};

/////////////////////////////////////////////////////////////////////////////
//	IParserDataBase interface

#undef  INTERFACE
#define INTERFACE IParserDataBase

#include <parse.h>
// Non-standard COM.
//////////////////////////////////////////////////////////////////////////////
// CSymbolInfo
//    |_____________________________
//    |             |              |
// CVarInfo   CFunctionInfo   CClassInfo
//
// contains information about a particular info in the database.
// enum, typedef use CSymbolInfo

class CSymbolInfo: public CObject
{
public:
	CString		m_strName;			// name of symbols
	parsetype	m_type;				// type of info: var, function, enum, typedef, class, etc
	BYTE		m_typ;				// parsetype:TYP
	BOOL		m_bConflict;		// used by the CheckForConflictSymbols
public:
	virtual ~CSymbolInfo(){};
};

//////////////////////////////////////////////////////////////////////////////
// CVarSymbol: contains a type as an extra information

class CVarSymbol: public CSymbolInfo
{
public:	
	CString		m_strType;
public:
	virtual ~CVarSymbol(){};
};

//////////////////////////////////////////////////////////////////////////////
// CFunctionSymbol: contains a return type and parameters as info

class CFunctionSymbol: public CSymbolInfo
{
public:
	CStringList 	m_slParams; // parameters
	CString			m_strType;	// return type
public:
	virtual ~CFunctionSymbol(){};
};

//////////////////////////////////////////////////////////////////////////////
// CClassSymbol: contains base classes as an extra info

class CClassSymbol: public CSymbolInfo
{
public:
	CStringList	m_slBaseClasses; // base classes
public:
	virtual ~CClassSymbol(){};
};
	
typedef CList<CSymbolInfo *, CSymbolInfo *&> CSymbolList;

DECLARE_INTERFACE_(IParserDataBase, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IParserDataBase methods
	STDMETHOD(GetGlobalSymbolsFromFiles)(const CStringList &lFileNames, CSymbolList &lSymbols) PURE;
	STDMETHOD(CheckForConflictSymbols)(CSymbolList &lSymbols) PURE;
	STDMETHOD(GetLexer)(const CString &strFileName, HLEXER *phLexer) PURE;
	STDMETHOD(GetLexerAndParser)(const CString &strFileName, HLEXER *phLexer, HPARSER *phParser) PURE;
	STDMETHOD(GetDerivedClass)(CStringList &slClasses, const CString &strBaseClass, UINT hTarget, BOOL bOnlyOne) PURE;
	STDMETHOD(GetInfo)(void *&pSymbolInfo, UINT hTarget) PURE;
	STDMETHOD(GetClassMembers)(CString &strClassName, CStringList &slFunctions, CStringList &slVars, UINT hTarget) PURE;
	STDMETHOD(GetClassList)(CStringList &slClasses, UINT hTarget) PURE;
	STDMETHOD(GetBsc)(int *gbrv, UINT hTarget, HBSC *phBsc) PURE;
	STDMETHOD(IsTerminateThread)() PURE;
	STDMETHOD(GetDBase)(HNCB *phNcbParse) PURE;
	STDMETHOD(IsSuspendThread)() PURE;
	STDMETHOD(NotifyParseDone)(LPCSTR lpszFileName) PURE;
	STDMETHOD(WaitParserSuspended)() PURE;
	STDMETHOD(ReparseTarget)(UINT hTarget) PURE;
	STDMETHOD(ParseFile)(UINT hTarget, CString strFileName) PURE;
	STDMETHOD(GetQSize)(int *piSize) PURE;
	STDMETHOD(GetSuspendHandle)(HANDLE *phSuspend) PURE;
};

/////////////////////////////////////////////////////////////////////////////
//	IEditDebugStatus interface

#undef  INTERFACE
#define INTERFACE IEditDebugStatus

DECLARE_INTERFACE_(IEditDebugStatus, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IEditDebugStatus methods
	STDMETHOD(GetExecutableFilename)(PSTR executable, UINT size, EXEFROM *piExeFrom) PURE;
	STDMETHOD(GetLastDocWin)(CDocument **ppDocument) PURE; 	// Non-standard COM.
	STDMETHOD(IsSaveBeforeRunningTools)(BOOL *pbQuery) PURE;
	STDMETHOD(ProjectFilesModified)() PURE;
	STDMETHOD(IsGrepRunning)() PURE;
	STDMETHOD(SetLoadNextFileNoPrompt)(BOOL f) PURE;
};

/////////////////////////////////////////////////////////////////////////////
//	IParser interface

#undef  INTERFACE
#define INTERFACE IParserAssist

DECLARE_INTERFACE_(IParserAssist, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IParserAssist methods
	STDMETHOD(GetLine)(void *pTextDoc, int iLine, LPSTR &pszLine, UINT &uLength, DWORD &lxs) PURE;
	STDMETHOD(RegisterFileToWatch)(CString &strFileName) PURE; 	// Non-standard COM.
	STDMETHOD(UnregisterFileToWatch)(CString &strFileName) PURE; 	// Non-standard COM.
	STDMETHOD(AddToQueue)(const CPath * pPath, HTARGET hTarget, Action action) PURE; // Non-standard COM

	STDMETHOD(SetDynamicParsing) (BOOL fDyParse) PURE;
	STDMETHOD(GetCurrentParseFileBuffer)(LPCCH *ppFileContents, ULONG *pcbFile) PURE;
	STDMETHOD(GetEditorFrame) (HWND *phWndFrame) PURE;
};

/////////////////////////////////////////////////////////////////////////////
//	IOutputWindow interface

#undef  INTERFACE
#define INTERFACE IOutputWindow

DECLARE_INTERFACE_(IOutputWindow, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IOutputWindow methods
	STDMETHOD(ShowOutputWindow)() PURE;
	STDMETHOD(ClearOutputWindow)() PURE;
	STDMETHOD(WriteStringToOutputWindow)(LPCSTR lpszError, BOOL fToolError, BOOL fAppendCrLf) PURE;
	STDMETHOD(OutputWindowQueueHit)(UINT nCommandID, LPCSTR lpszError,
		BOOL fToolError, BOOL fAppendCrLf, BOOL fForceFlush) PURE;
	STDMETHOD(OutputWindowDeleteVwin)(UINT nCommandID) PURE;
	STDMETHOD(OutputWindowShowVwin)(UINT nCommandID) PURE;
	STDMETHOD(OutputWindowSelectVwin)(UINT nCommandID, BOOL fOnTop) PURE;
	STDMETHOD(OutputWindowAddVwin)(LPSTR szToolName, BOOL fCurrentVwin, UINT *puRet) PURE;
	STDMETHOD(OutputWindowModifyVwin)(UINT nCommandID, LPSTR szToolName, UINT *puRet) PURE;
	STDMETHOD(OutputWindowVwinClear)(UINT nCommandID) PURE;
	STDMETHOD(OutputWindowClearWindow)(UINT nCommandID) PURE;
	STDMETHOD(OutputWindowClearContents)(UINT nCommandID) PURE;
	STDMETHOD(OutputWindowGetLine)(ULONG iline, ULONG *pcbLine, LPCSTR *ppLine) PURE;
	STDMETHOD(OwinGetBuildCommandID)(UINT *puRet) PURE;
	STDMETHOD(OwinGetDebugCommandID)(UINT *puRet) PURE;
	STDMETHOD(OwinIsBufferFull)(BOOL* fFull) PURE;	
};

/////////////////////////////////////////////////////////////////////////////
//	IFileAccess interface

#undef  INTERFACE
#define INTERFACE IFileAccess

DECLARE_INTERFACE_(IFileAccess, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IFileAccess methods
	STDMETHOD(CreateFile)(LPCTSTR lpszName, DWORD fdwAccess, DWORD fdwShareMode, LPSECURITY_ATTRIBUTES lpsa, DWORD fdwCreate, DWORD fdwAttrsAndFlags, HANDLE hTemplateFile, HANDLE *hRet) PURE;
	STDMETHOD(OpenFile)(const char *pszFileName, const char *pszMode, FILE **ppRet) PURE;
	STDMETHOD(CreateFileForParse)(LPCTSTR lpszName, DWORD fdwAccess, DWORD fdwShareMode, LPSECURITY_ATTRIBUTES lpsa, DWORD fdwCreate, DWORD fdwAttrsAndFlags, HANDLE hTemplateFile, HANDLE *hRet) PURE;
	STDMETHOD(CloseFileForParse)(HANDLE handle) PURE;
	STDMETHOD(Replace)(const char *pszOldName, const char *pszNewName, int *iRet) PURE;
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//	ICustomContextMenu interface
//		Access to custom context menus on a per-language / lexer basis

#undef  INTERFACE
#define INTERFACE ICustomContextMenu

/////////////////////////////////////////////////////////////////////////////
//	forward declaration
struct POPDESC;
typedef POPDESC* LPPOPDESC;


DECLARE_INTERFACE_(ICustomContextMenu, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// ICustomContextMenu methods
	STDMETHOD(GetContextMenu)(THIS_ LPCTSTR, LPPOPDESC*) PURE;
};

#ifdef _SQLDBG
#include "sqlpkapi.h"
#endif // _SQLDBG

#endif	// __SRCAPI_H__
