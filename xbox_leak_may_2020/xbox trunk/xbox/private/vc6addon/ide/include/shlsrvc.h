/////////////////////////////////////////////////////////////////////////////
//	SHLSRVC.H
//		Various services provided by the shell.

#ifndef __SHLSRVC_H__
#define __SHLSRVC_H__

#ifndef __DLGBASE_H__
#include "dlgbase.h"
#endif

#ifndef __SHLBAR_H__
#include "shlbar.h"
#endif

#ifndef __AFXCMN_H__
#include "afxcmn.h"
#endif

//////////////////////////////////////////////////////////////////////////
// Header files which need to be merged with shlsrvc.h

#include "filechng.h"
#include "fmtinfo.h"
#include "spawner.h"

//////////////////////////////////////////////////////////////////////////

class CPartView;

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

//////////////////////////////////////////////////////////////////////////
// Status Bar API

void ShowStatusBar(BOOL bShow = TRUE);
BOOL IsStatusBarVisible();
BOOL IsStatusBarIndicator(UINT nID);
int  GetStatusBarHeight();
void InvalidateStatusBar(BOOL bErase = FALSE);
void ClearStatusBarSize();
void ClearStatusBarPosition();
void ClearStatusBarPositionAndSize();
void ResetStatusBar();
void SetStatusBarPosition(const CPoint& pos);
void SetStatusBarSize(const CSize& size);
void SetStatusBarPositionAndSize(const CRect& rect);
void SetStatusBarZoom(int zoom);
void SetStatusBarLineColumn(int line, int column, BOOL fForce = FALSE);
void SetStatusBarOffsetExtent(long lOffset, long lExtent);
void SetStatusBarOverStrike(BOOL bOverStrike);
void SetPrompt(const char*, BOOL bRedrawNow = FALSE,
               COLORREF textColor = 0, COLORREF bkColor = 0);
void SetPrompt(UINT ids = AFX_IDS_IDLEMESSAGE, BOOL bRedrawNow = FALSE,
               COLORREF textColor = 0, COLORREF bkColor = 0);
void SetScrollingPrompt(const char*, BOOL bRedrawNow = FALSE,
               COLORREF textColor = 0, COLORREF bkColor = 0);
void SetIdlePrompt(const char*, BOOL bRedrawNow = FALSE);
void SetIdlePrompt(UINT ids = AFX_IDS_IDLEMESSAGE, BOOL bRedrawNow = FALSE);
void SetPromptUntilIdle(const char* szMessage, COLORREF textColor = 0, COLORREF bkColor = 0);
BOOL IsPromptSetUntilIdle(void);

void StatusBeginPercentDone(const char* szPrompt);
void StatusEndPercentDone();
void StatusPercentDone(int percentDone);
void StatusOnIdle();

/////////////////////////////////////////////////////////////////////////////
// CSushiBarInfo
//

typedef struct tagSTATINFO
{
	UINT nID;
	int nGroup;
	int nBitmap;
} STATINFO;

class CSushiBarInfo : public CObject
{
public:
	CSushiBarInfo(const UINT FAR* pIndicators = NULL, int cIndicators = 0,
	              STATINFO FAR* pBitmaps = NULL, int cBitmaps = 0);

	const UINT FAR* m_pIndicators;
	int m_cIndicators;

	STATINFO FAR* m_pBitmaps;
	int m_cBitmaps;
};


/////////////////////////////////////////////////////////////////////////////
// Standard font handling

enum FontType
{
	font_Normal,			// MS Sans Serif 8, MS âSâVâbâN 10, System 10
	font_Bold,
	font_Italic,
		
	font_Small,				// Small Fonts (-9), Terminal (-9), Terminal (-9)
	font_SmallBold,
	font_SmallItalic,
		
	font_Fixed,				// Courier (14), FixedSys (14), FixedSys (14)
	font_FixedBold,
	font_FixedItalic,
	
	font_Large,             // MS Sans Serif 14. MS Sans Serif 14, MS Sans Serif 14,
	font_LargeBold,
	font_LargeItalic,

	font_count				// Not a font, just provides a count
};

CFont* GetStdFont(const int iType);

// NOTE: The LOGFONT returned by this is temporary
const LOGFONT* GetStdLogfont(const int iType, CDC *pDC = NULL);
void GetStdFontInfo(int iType, CString& strFace, int& ptSize);

////////////////////////////////////////////////////////////////////////////
//	Help

enum EOpenURLOption{ eIVOnly = 0, eHonorIVOption, eExternalOnly };
void GetHelpDir(CString & strHelpDir);
void OpenURL( CString& strURL, EOpenURLOption eURLOpt = eHonorIVOption);

///////////////////////////////////////////////////////////////////////////////
//	Find  and replace

#define FDS_WHOLEWORD	0x00000001
#define FDS_MATCHCASE	0x00000002
#define FDS_REGEXP		0x00000004
#define FDS_MARKALL		0x00000008
#define FDS_SEARCHALL	0x00000010

#define LIMIT_FIND_COMBO_TEXT 250	// Magic number based on line length limit.

//Find/Replace : Type of pick list
typedef enum { reZibo, reUnix, reBrief, reEpsilon } reSyntax;

extern reSyntax g_reCurrentSyntax;

//Find/Replace : Structure Definition (saved on disk)
struct patType;

// TODO(CFlaat): split the cursed FINDREPLACEDISK structure into two parts, one for global
//    find options and another to represent options for an individual find operation

struct FINDREPLACEDISK
{
	patType *pat;					// Compiled regular expression
	BOOL regExpr;					//Regular expression

	void DisposeOfPattern(void);
	BOOL CompileRegEx(reSyntax syntax);

	BOOL matchCase;					//Match Upper/Lower case
	BOOL bWasLineMatch;  // whether the last regex search was a BOL or EOL match
	BOOL wholeWord;
	BOOL goUp;						// Search direction
	BOOL loop;						// Loop when reaches the end (equivalent to NOT search all open documents)
	BOOL startFromCursor;			// start from cursor or from the beginning
									// (or from the end if going backwards)

	char findWhat[LIMIT_FIND_COMBO_TEXT + 1];		//Input string
	char replaceWith[LIMIT_FIND_COMBO_TEXT + 1];	//Output string
	int nbReplaced;							//Actual number of replacements

	//Construction
	FINDREPLACEDISK();
	~FINDREPLACEDISK();
};

extern FINDREPLACEDISK AFX_DATA findReplace;

enum PickListType { FIND_PICK = 0, REPLACE_PICK = 1 };
enum { MAX_PICK_LIST = 16 };

// the CPickList class tracks previous search strings employed by the user (via both UI and automation)

class CPickList
{
protected:

	int nbInPick[REPLACE_PICK + 1];			//Number of strings in picklist
	LPTSTR aaszPickList[REPLACE_PICK + 1][MAX_PICK_LIST]; //PickList for old search strings

	void AllocateEntry(PickListType type, UINT iIndex);
	void DeallocateEntry(PickListType type, UINT iIndex);

public:

	CPickList(void);
	~CPickList(void);

	int GetEntryCount(PickListType type) { return nbInPick[type]; }

	BOOL InsertEntry(PickListType type, LPCTSTR szText);
	BOOL InsertEntry(PickListType type, FINDREPLACEDISK *pfr);
	void RemoveEntry(PickListType type);

	LPTSTR GetEntry(PickListType type, UINT iIndex)
		{
			ASSERT(iIndex >= 0); ASSERT(iIndex < MAX_PICK_LIST);
			return aaszPickList[type][iIndex]; 
		}

	void LoadFromRegistry(FINDREPLACEDISK *pfr);
	void SaveToRegistry(void);
};

CPickList &ShellPickList();

void SetFindFocus();
void UpdateFindCombos();
BOOL IsFindComboActive();

// SetRegExprSyntax: Allow a package to change the default syntax
//  for regular expression searches in response to user input.
//  Valid syntaxes are: reUnix, reBrief, reEpsilon
//  Default syntax is reUnix.
BOOL SetRegExprSyntax(reSyntax _reSyntax);

// GetRegExprSyntax: Allow a package to determine the default syntax
//  for regular expression searches in response to user input.
//  Valid syntaxes are: reUnix, reBrief, reEpsilon
reSyntax GetRegExprSyntax(void);

/*
	GetCurrentRegexHintTable

	This function returns an array of regex hint->string mappings for
	the current editor emulation.  In the future we should use a safer
	means for doing this.
*/
LPCTSTR *GetCurrentRegexHintTable(void);

typedef char flagType;
// RECompile: regular expression compiler. Returns a compiled expression
// that must be freed by the caller. The returned expression can
// then be used as an argument to RESearch or you can set the 'pat'
// member of a FINDREPLACEDISK struct that is passed to FindInString(...)
patType *RECompile(char *szRegExpr, flagType fCase, reSyntax);

// RESEarch: performs a Regular expression search
BOOL RESearch(
	LPCTSTR line,
	ULONG *ichStart,	// byte index into line
	ULONG *xEnd,
	patType *pat,		// pointer to reg expression
	LPCTSTR szString,
	/* out */ BOOL *pbLineMatch);
char			 RETranslate (struct patType *,char *,char *);
int			 RETranslateLength (struct patType *,char *);

/////////////////////////////////////////////////////////////////////////////
//
// Text pattern recognition routines
//

// don't hittest
#define NO_HITTEST ULONG_MAX

//////////////////////////////////////////////////////////////////////////
// FREMatchAt
//
// Find regular expression in sz, starting from ibStart
//

BOOL FREMatch(
	/* IN     */ LPCTSTR sz,       // string to search
	/* IN     */ LPCTSTR szPat,    // Regular expression to find
	/* IN/OUT */ ULONG & ibStart,  // starting index (IN) start of match (OUT)
	/*    OUT */ ULONG & ibEnd     // end of match (noninclusive)
	);

//////////////////////////////////////////////////////////////////////////
// FFindURL
//
// Find URL in sz, starting from ibStart, that contains position ibAt.
// When ibAt is NO_HITTEST, finds the first URL in sz, starting from ibStart
//
// Returns:
//
//   TRUE if [intersecting] URL found.
//
// Example:
//
//     0123456789_123456789_12345678
//    "  HTTP://www.microsoft.com  "
//
//   Returns (TRUE): 
//     ibStart       =  2
//     ibEndProtocol =  7
//     ibEnd         = 26

BOOL FFindURL(
	/* IN     */ LPCTSTR sz,              // string to search
	/* IN     */ ULONG   ibAt,            // index to intersect, or NO_HITTEST to find first match
	/* IN/OUT */ ULONG & ibStart,         // starting index (IN) start of URL (OUT)
	/*    OUT */ ULONG & ibEndProtocol,   // end of URL protocol/scheme
	/*    OUT */ ULONG & ibEnd            // end of URL (noninclusive)
	);

/////////////////////////////////////////////////////////////////////////////
//	Goto
//		The following two class definitions provide goto support, generally
//		available to all packages.

class CGoToItem;
class CGotoDialog;

enum GOTO_TYPE {goGoTo, goNext, goPrevious};
#define dwItemDataInvalid ((DWORD)(-1))

// CGoToDialog dialog

class CGoToDialog : public C3dDialog
{
	DECLARE_DYNAMIC(CGoToDialog)

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// Construction
	CGoToDialog(UINT idHelp, LPCTSTR szItemInit, CWnd* pParent = NULL);   // constructor
    ~CGoToDialog();
	
	// call this from CPackage:AddGoToItems for each item to add (editor-
	// specific items should be added even if the right kind of editor 
	// is not currently added.
	void AddItem(CGoToItem *pItemNew);

	// call this from the CGoToItem callbacks to change the style of buttons
	// being used. (This must be called from CGoToItem::OnSelectWhat at the very least!)
	enum BUTTON_STYLE { bsGoTo, bsNextPrev };
	void SetButtonStyle (BUTTON_STYLE buttonstyle);

	// call this from the CGoToItem callbacks to change the style of selection control
	// being used. (This must be called from CGoToItem::OnSelectWhat at the very least!)
	enum CONTROL_STYLE { csUninit = -1, csEdit, csDropList, csDropCombo, csNone };
	void SetControlStyle (CONTROL_STYLE controlstyle);
	 
	// call this to change the selection prompt. (This must be called from 
	// CGoToItem::OnSelectWhat at the very least!)
	void SetPrompt (const TCHAR * szPrompt);
	 
	// call this to change the helper text. If not called from 
	// CGoToItem::OnSelectWhat it will be blank by default.
	void SetHelperText (const TCHAR * szHelperText);

	// call this to enable or disable dialog controls depending on whether the item
	// can be active besed on the active editor. This is called internally after
	// CGoToItem::CanGoViewActive is called. Ignored if pGoToItem is not the current
	// selection in the Go To What list.
	void SetItemEnabled(CGoToItem *pGoToItem, BOOL fEnabledItem);

	// call this to empty the selection list when csDropList or csDropCombo are used.
	void EmptyList ();

	// call this to add a string to the selection list
	void AddString (const TCHAR * szSelect, DWORD dwItemData = 0);

	// call this to lock/unlock the selection list when csDropList or csDropCombo are used.
	// while the control is being filled.
	void LockList (BOOL fLock = TRUE);

	// call this to get the dialog id of the "selection control" in the rara
	// case when the List and Select functions are not adequate
	// pcs will be set to the control style of the control, since it can be
	// either and edit control . a combo or a list box. Use GetDlgItem to 
	// get a pointer to the selection control.
	int IdCtlSelection (CONTROL_STYLE *pcs);

	// call one of these to initialize the current selection control
	void Select (const TCHAR * szSelect);
	void Select (int iSelect);

	// call this when a CGoToItem::OnSelectChange needs to change the button state
	// OR the msk values into grfButton to set the state of one or more buttons
	#define mskButtonGoToNext	(0x0001)
	#define mskButtonPrevious	(0x0002)
	#define mskButtonClose		(0x0004)
	#define mskButtonHelp		(0x0008)

	void EnableButton (WORD grfButton, BOOL fEnable = TRUE);

// Dialog Data
	//{{AFX_DATA(CGoToDialog)
	enum { IDD = IDD_GOTODIALOG };
	CComboBox	m_gotoCombo;
	CStatic	m_helpText;
	CStatic	m_gotoCaption;
	CListBox	m_gotoWhat;
	CListBox	m_gotoList;
	CEdit	m_gotoEdit;
	//}}AFX_DATA

public:
	CPartView* m_pActiveView;  // reset when gotowhat selection changes and after OnGoTo

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGoToDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	CToolTipCtrl m_tooltip;

//	void OnOK( void );
	void OnCancel( void );	

// Implementation
public:
	virtual void PostNcDestroy();
	virtual BOOL OnHelpInfo(HELPINFO* pInfo); 

public:
	// static member!
	static CGoToDialog * c_pGotodialog; // used to prevent multiple instances of modeless dialog
	static CGoToDialog * PGoToDialogCur();   // returns c_pGotodialog
	BOOL m_fInInitDialog;  // true only while initdialog processing. Some items behave differently in that case
	CToggleBitmapButton	m_btnPushPin;  // subclassed via Autoload call
	void GotoCtlSel();

private:
	CWnd * m_pctlSelection;  // pointer to current edit, list or combo used to select destination
	CONTROL_STYLE m_csSelection;
	CGoToItem * m_pGoToItemCur;
	BUTTON_STYLE m_bsCur;
	CString m_strItemInit;  // item string to select in goto what list at startup
	// cLock... is a sentinal so the modeless dialog can reset up itself when you click away from the dialog
	// but do not reselect when the dialog loses focus for errors/asserts etc
	int m_cLockSelectAtActivate; // counter (for nesting). If non-zero, do not reselect when dialog is activated.
	int m_cBusyNoClose; // counter (for nesting). If non-zero, disallow closing dialog since goto operation still in progress.
	BOOL m_fFirstMoveMsg;
	BOOL m_fForcePinForHelp;

private:
	void GetSelectData(CString &strData, 
			DWORD *pdwItemData, 
			int *pidSelect,
			BOOL *pfSelInList,
			BOOL fComboSelChanged); 

	void OnOKPrevious(GOTO_TYPE go); 
	BOOL DoInitDialog(void);
	void DoSelchangeGoToWhat(); 

protected:
	// Generated message map functions
	//{{AFX_MSG(CGoToDialog)
	afx_msg void OnSelchangeGoToList();
	afx_msg void OnSelchangeGoToWhat();
	afx_msg void OnSelchangeGoToCombo();
	afx_msg void OnPrevious();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeGoToEdit();
	afx_msg void OnEditchangeGoToCombo();
	afx_msg void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);
	afx_msg void OnMove(int cx, int cy);
	afx_msg void OnGotoGotoAndNext();
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// CGoToItem
//  subclassed in each package that adds an entry to the Go To What listbox
//  in a CGoToDialog dialog

class CGoToItem 
{
public:
	CGoToItem(CGoToDialog * pDlg);

	// ***Initialization. These functions are used to initialize the Go To 
	// dialog. THey will be called when the dialog is invoked but may
	// actually be called at any time.

	// REQUIRED: Return the string to show in the Go To What listbox

	virtual const TCHAR * GetItemString() = 0;

	// ***Callbacks. These functions are called as the user manipulates the Go To
	// dialog or active editor view

	// Called when the item is selected in the Go To What listbox,
	// and at CGoToDialog startup and reactivation.
	// CGoToDialog will call CanGoViewActive after returning from this
	// call, so let it handle situational enabling/disabling

	// Note that this function needs to be able to tolerate a NULL m_pDlg->m_pActiveView
	// or one of the incorrect type. It should set what it can and leave other controls empty.

	// Do not put up error messages or dialogs during this function, as it can be called when focus
	// is changing. If you need to check a state that might bring up such a message it would be better
	// to enable the goto button and check at CanGoTo or OnGoTo rather than doing it here

	virtual void OnSelectWhat();

	// Called when the active editor changes. A FALSE return indicates that the
	// item should not be enabled in the dialog for this editor, TRUE indicates
	// that the item may be active in this editor view.

	virtual BOOL CanGoViewActive(CPartView* pActiveView);

	// Called when the selection control changes (either the user changed
	// the text in the edit control or they selected something from the
	// drop list). For csEdit style selection controls, the fSelInList
	// and dwItemData parameters should be ignored (FALSE/-1 will always 
	// be passed here).
	// fSelInList will be true if there is an item selected in the list 
	// or combo box

	// SzSelect note: For csEdit it is the edit text, for csDropList, the
	// text of the list box selection. For csDropCombo, szSelect will be the 
	// text in the LIST BOX if fSelInList, otherwise it will be the edit text. If you need
	// to get the edit text when fSelInEdit call CGoToDialog::IdCtlSelection
	// to get at the "selection control"

	// generally used for simple test like handling empty strings

	virtual void OnSelectChange	(const TCHAR * szSelect, 
								BOOL fSelInList,
								DWORD dwItemData);

	// Override this to indicate whether or not a go to operation can be
	// attempted. Done when an action button is hit.
	// For goNext and goPrevious operations, the szSelect 
	// and dwItemData items are not used.
	// For goGoTo operations, they describe the string and optional
	// user-data selected in the Go To dialog.
	// fSelInList will be true if there is an item selected in the list 
	// or combo box

	// SzSelect note: For csEdit it is the edit text, for csDropList, the
	// text of the list box selection. For csDropCombo, szSelect will be the 
	// text in the LIST BOX if fSelInList, otherwise it will be the edit text. If you need
	// to get the edit text when fSelInEdit call CGoToDialog::IdCtlSelection
	// to get at the "selection control"

	// pDlg and idDlgItemError are used to return focus to in case of error

	virtual BOOL CanGoTo(int idDlgItemError, 
						GOTO_TYPE go, 
						const TCHAR * szSelect, 
						BOOL fSelInList,
						DWORD dwItemData);


	// Override this to handle a go to operation. For goNext and goPrevious
	// operations, the szSelect and iSelList, and dwItemData items are not used. For goGoTo
	// operations, they describe the string and optional listbox user-data
	// selected in the Go To dialog, and the list selection, if any.
	// fSelInList will be true if there is an item selected in the list 
	// or combo box

	// SzSelect note: For csEdit it is the edit text, for csDropList, the
	// text of the list box selection. For csDropCombo, szSelect will be the 
	// text in the LIST BOX if fSelInList, otherwise it will be the edit text. If you need
	// to get the edit text when fSelInEdit call CGoToDialog::IdCtlSelection
	// to get at the "selection control"

	virtual void OnGoTo(GOTO_TYPE go,
						const TCHAR * szSelect,
						BOOL fSelInList,
						DWORD dwItemData);

public:
	CGoToDialog *m_pDlg; // set during construction
};

void DoGoToDialog(int iddHelp, LPCTSTR szItemInit);

/////////////////////////////////////////////////////////////////////////////
//	Dockable Windows support

/////////////////////////////////////////////////////////////////////////////
// Dock IDs
#define IDDW_SHELL_BASE         0x7000
// This range reserved for shell.
#define IDDW_SHELL_MAX			(IDDW_SHELL_BASE + 0x00FF)

#define IDDW_PACKAGE_BASE		(IDDW_SHELL_MAX + 1)
// Package defines actually used in the shell for things like hiding the
// output and topic windows when the Esc key is pressed.  Do not add to these.
#define IDDW_OUTPUT				(IDDW_PACKAGE_BASE + 0x100)	// PACKAGE_VCPP
#define IDDW_PROJECT            (IDDW_PACKAGE_BASE + 0x210)	// PACKAGE_PRJSYS
#define IDDW_GLOBAL				(IDDW_PACKAGE_BASE + 0x211) // PACKAGE_PRJSYS
#define IDDW_MSIN               (IDDW_PACKAGE_BASE + 0x220)	// PACKAGE_MSIN

#define IDDW_IV_BASE          (IDDW_PACKAGE_BASE + 0x0240)
#define IDDW_IV                IDDW_IV_BASE
#define IDDW_IVTOPIC          (IDDW_IV_BASE+0)
#define IDDW_IVRESULTS        (IDDW_IV_BASE+1)


// Usable for packages.  IDs must be unique for each item in a package,
// since the ID will be used to retrieve the window from GetDockableWindow(),
// The IDs need not be unique across packages.  These IDs are also used to
// determine relative priority in ordering in the context pop-up.
#define IDDW_EXT_PACKAGE_BASE	(IDDW_PACKAGE_BASE + 0x0260)	// Default for package wizard.
#define IDDW_PACKAGE_MAX		(IDDW_PACKAGE_BASE + 0x02FF)

#define IDTB_SHELL_BASE         (IDDW_PACKAGE_MAX + 1)
// This range reserved for shell.
#define IDTB_MENUBAR			(IDTB_SHELL_BASE+0)	
#define IDTB_STANDARD			(IDTB_SHELL_BASE+1)
#define IDTB_PROJECT			(IDTB_SHELL_BASE+2)
#define IDTB_FULLSCREEN			(IDTB_SHELL_BASE+3)
#define IDTB_SHELL_MAX			(IDTB_SHELL_BASE + 0x00FF)

#define IDTB_PACKAGE_BASE		(IDTB_SHELL_MAX + 1)
// Usable for packages.  IDs must be unique for each item in a package,
// since the ID will be used to retrieve the window from GetToolbarData(),
// The IDs need not be unique across packages.  These IDs are also used to
// determine relative priority in ordering in the context pop-up.
#define IDTB_EXT_PACKAGE_BASE	(IDTB_PACKAGE_BASE + 0x0260)	// Default for package wizard.
#define IDTB_PACKAGE_MAX		(IDTB_PACKAGE_BASE + 0x02FF)
#define IDTB_CUSTOM_BASE        (IDTB_PACKAGE_MAX + 1)	// No base IDs greater than this!
/////////////////////////////////////////////////////////////////////////////

//	Dock states.
// _FS_ == Full Screen
// Bit 0 - Edit/Debug
// Bit 1 - Normal/FullScreen

#define STATE_EDIT		0
#define STATE_DEBUG 	1
#define STATE_FS_EDIT	2
#define STATE_FS_DEBUG	3

#define STATE_FULLSCREEN 2
#define MASK_FULLSCREEN  2

#define IS_STATE_DEBUG(x)		(x & 1)
#define IS_STATE_FULLSCREEN(x)	(x & 2)

//	Styles.

#define INIT_VISIBLE	   0x00000001L	// Make visible when registered.
#define INIT_CANHORZ 	   0x00000002L	// Can reside in horizontal dock.
#define INIT_CANVERT	   0x00000004L	// Can reside in vertical dock.
#define INIT_POSITION	   0x00000008L	// No floating position given.
#define INIT_ASKAVAILABLE  0x00000010L	// Hide/show on idle depending on availability.
#define INIT_AVAILABLE	   0x00000020L	// Internal use only for layout.
#define INIT_MDI		   0x00000040L	// MDI window.
#define INIT_DESKPLACE	   0x00000080L  // We have MDI window placement info.
#define INIT_DOCKPLACE	   0x00000100L  // We have CDockWnd placement info.
#define INIT_DOCKSERIAL    0x00000200L	// We have absolute CDockWnd placement info.
#define INIT_LASTAVAILABLE 0x00000400L	// Used for AskAvailable stretchy windows.
#define INIT_NODESTROY     0x00000800L  // Don't destroy window on Hide (dtEdit windows only).
#define INIT_STRETCHSIZE   0x00001000L	// Init struct has a size for stretching.
#define INIT_TRANSPOSE_SIZE 0x00002000L	// When docking use vertical size for horizontal dock.
#define INIT_REMOVE			0x00008000L	// To remove old workers from layout.
#define INIT_HOST			0x00010000L  // True if the window is the host of an embedded window (internal)

#define DWS_STRETCHY       0x00000001L	// Stretch to fit in dock.
#define DWS_SIZABLE		   0x00000002L	// Snap sizing when in palette.
#define DWS_8SIZABLE	   0x00000004L	// Snap sizing with corners.
#define DWS_MENUBAR        0x00000008L  // Stretches in top dock, wraps as needed, repels invaders

//	Messages.

// Window messages.  Handle for enhanced functionality.  You must register this
// yourself in your dll.  To do so, copy the following definitions.
//		UINT DWM_GETDATA = RegisterWindowMessage("DockGetWindowData");
//		UINT DWM_GETMOVESIZE = RegisterWindowMessage("DockWndMoveSize");
//		UINT DWM_GETTRACKSIZE = RegisterWindowMessage("DockWndTrackSize");
//		UINT DWM_ENABLECHILD = RegisterWindowMessage("DockEnableChild");
//		UINT DWM_SHOWWINDOW = RegisterWindowMessage("DockShowWindow");
//		UINT DWM_CLOSE = RegisterWindowMessage("DockWndClose");

extern UINT DWM_GETDATA;		// Sent to the window to get initialization data.
extern UINT DWM_GETMOVESIZE;	// Allows window to change size on dock position.
extern UINT DWM_GETTRACKSIZE;	// Allows window to control snap sizing.
extern UINT DWM_ENABLECHILD;	// Disable children for toolbar customizing.
extern UINT DWM_SHOWWINDOW;		// Notification of visibility state change.

// Datatypes.

enum DOCKPOS
{
    dpNil,
    dpTop,		// Top through Hide must be in order.
    dpBottom,
    dpLeft,
    dpRight,
    dpHide,
	dpMDI,
    dpCurrent,
	dpHorzInit,
	dpVertInit,
	dpEmbedded
};

#define dpFirst 1	// These refer to Top through Hide.
#define dpCount 5

enum DOCKTYPE
{
	dtNil,
	dtHwnd,
	dtWindow,
	dtToolbar,
	dtBorder,
	dtEdit,
	// a toolbar embedded in another window
	dtEmbeddedToolbar,
};

enum UPDATE_SHOW {
	updNow,
	updNoRecalc,
	updDelay
};

enum MAX_STATE {
	stateMaximize,
	stateRestore,
	stateDisabled,
};

#define _MAX_DOCK_CAPTION 63

#pragma pack(2)
struct DOCKINIT {
	UINT nIDWnd;
	UINT nIDPackage;
	UINT nIDPacket;
	char szCaption[_MAX_DOCK_CAPTION + 1];
	DWORD fInit;	// Initialization flags.
	DOCKTYPE dt;
    DOCKPOS dpInit, dpPrev;

    RECT rectFloat;		// Accessed only if !INIT_POSITION
	SIZE sizeStretch;	// Accessed only if INIT_STRETCHSIZE
};

struct TOOLBARINIT {
	UINT nIDWnd;        // full (MAKEDOCKID) docked window id - including package id and window id
	int nIDCount;
};
#pragma pack()

// Function prototypes.

HGLOBAL DkInfoData(UINT nInfoCount, DOCKINIT FAR* lpInfoArray);
HGLOBAL DkToolbarData(const TOOLBARINIT* ptbi, const UINT FAR* lpIDArray);
BOOL DkRegisterWnd(HWND hwnd, DOCKINIT* pDockInit);
BOOL DkRegisterToolbar(DOCKINIT* pdi, TOOLBARINIT* ptbi, UINT FAR* lpIDArray);
BOOL DkPreHandleMessage(HWND hwnd, UINT message,
	UINT wParam, LPARAM lParam, LRESULT* lResult);

#define MAKEDOCKID(pkg, id) MAKELONG(id, pkg)

DOCKPOS DkWGetDock(UINT nID);
void DkWShowWindow(UINT nID, BOOL bShow);
void DkWToggleMDIState(UINT nID);
void DkWMoveWindow(UINT nID, DOCKPOS dp, LPCRECT lpRect);
void DkWGetTitle(UINT nID, CString& str);
void DkWSetTitle(UINT nID, const CString& str);
void DkWRemove(UINT nID);
// call this to embed a bar in a docking window
HWND DkWEmbed(UINT nID, HWND hwndHost);
// Once the embedding host is created, it should call this to complete the association
void DkWAssociate(UINT nID, UINT nIDHost);
MAX_STATE DkWGetMaxState(UINT nID);
void DkWMaximizeRestore(UINT nID);

void DkWMapDockingCmd(UINT nID, UINT nCmd);
void DkWHandleDockingCmd(UINT nCmd, BOOL bToggle, CCmdUI* pCmdUI = NULL);

void DkRecalcBorders(CWnd* pWnd);

int DkGetDockState();
void DkSetDockState(int nState, int nMask = 1);
void DkShowManager(BOOL bShow, UPDATE_SHOW update = updNow);
BOOL DkDocObjectMode(BOOL bShow);
void DkUpdateAvailableWnds(BOOL bUpdateNow = FALSE);
void DkLockWorker(BOOL bLock);

/////////////////////////////////////////////////////////////////////////////

#undef AFX_DATA
#define AFX_DATA NEAR

#endif // __SHLSRVC_H__
