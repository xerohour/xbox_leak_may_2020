///////////////////////////////////////////////////////////////////////////////
//	DLGBASE.H
//		Declarations for the sushi dialog base classes.
//
#ifndef __DLGBASE_H__
#define __DLGBASE_H__

#ifndef __MAIN_H__
#include "main.h"
#endif

#ifndef __PATH_H__
#include "path.h"
#endif

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

// This class is for pre-loading and munging dialog tempalates.
// For now, it is only used to change the font of a dialog...
//
class C3dDialogTemplate : public CDialogTemplate
{
public:
	void SetStdFont();
	HGLOBAL GetTemplate() const { return m_hTemplate; }
};

// Switch to enable on-the-fly dialog font changing...
#define MUNGE_DLG_FONTS

// Set standard fonts in dialog template
void SetStdFont(C3dDialogTemplate & dt);

// All Sushi dialog boxes should derive from this class...
//
class C3dDialog : public CDialog
{
public:
	C3dDialog(LPCSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	C3dDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);

	virtual int DoModal();
	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	
	afx_msg void OnRobustOK();
	afx_msg virtual void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg virtual BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg LRESULT OnTestMenu(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

extern BOOL FindAccel( LPCTSTR lpstr, MSG *pMsg );

class C3dFileDialog : public CFileDialog
{
public:
	C3dFileDialog(BOOL bOpenFileDialog, // TRUE for Open, FALSE for SaveAs
		LPCSTR lpszDefExt = NULL,
		LPCSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL,
		UINT nHelpID = 0, UINT nExplorerDlg = (UINT)-1);
	~C3dFileDialog();

	LPCTSTR GetFilterExtension(int n);
	void UpdateType(LPCTSTR lpszExt = NULL);
	void ApplyDefaultExtension(void);
	void SetOkButtonText(UINT ids);

	virtual int DoModal();

	afx_msg void OnRobustOK();
	afx_msg virtual void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg virtual BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnNameKillFocus();
	afx_msg void OnFileListSelChange();
	afx_msg void OnScc();

	void UpdateMultiSelectOnNT(void);

	virtual void OnOK();
	virtual void OnCancel ();
	virtual BOOL OnFileNameOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();

	BOOL IsExplorer()
		{ return m_ofn.Flags & OFN_EXPLORER; }

	// essentially overrides for MFC's GetStartPosition and GetNextPathName,
	// but which help us deal with limitations on multiple-selection handling.
	POSITION GetFullFileStartPosition();
	CString GetNextFullFileName(POSITION pos);

	DECLARE_MESSAGE_MAP()

protected:
	CDir m_dirCurBefore;	// Current dir before dialog goes up
	CDir m_dirCurAfter;		// Current dir after dismissal

public:
	CStringArray* m_psaFileNames;
	UINT m_iddWin95;
	UINT m_iddWinNT;
	BOOL m_bMultiSelectOnNT;
	BOOL m_bSccEnabled;
};

extern void AppendFilterSuffix(CString& filter, OPENFILENAME& ofn,
	const CString& strFilterName, BOOL bSetDefExt = FALSE);

extern void AppendFilterSuffix(CString& filter, OPENFILENAME& ofn,
	UINT idFilter, BOOL bSetDefExt = FALSE);

extern BOOL AddRegistryFilters(CString& strFilter, int nDesiredType, OPENFILENAME& ofn);

extern void AddWildFilter(CString& strFilter, OPENFILENAME& ofn);

extern void SetFileFilters(CString& strFilter, OPENFILENAME& ofn, int nDesiredType, 
	UINT* rgidDefaults, int nDefaults, const TCHAR* szCommon = NULL);

#define FLT_OPEN			1	// The File Open dialog
#define FLT_PROJECTFILES	2	// The Project Files dialog
#define FLT_TEXTFILES		4	// The Find in Files dialog

/////////////////////////////////////////////////////////////////////////////
//	CDirChooser
//			The semantics for dismissing the dialog are as follows:
//			It will be dismissed when the user selects a directory and then 
//			presses Enter a second time without changing the directory string
//			eg  - Type a name, hit Enter, hit Enter again
//				- Type a name, hit Enter, click OK
//				- Double-click on a directory name, click OK
//			Directories are not checked for existance.
//		Parameters
//			strOpenDir - Directory name that should be initially placed in 
//				the edit box. Default is the current directory.
//			strTitle - Window title. Default is "Choose Directory"
//			strPrompt - prompt to place just above the dialog's edit box.
//				It's recommended that this string contain an accelerator (&)
//				so the user can go to the edit box via the mouse.  (Make
//				sure the accelerator doesn't conflict with the others in
//				the dialog!). Default is Directory &Name:
class CDirChooser
{
	public:
		CDirChooser(LPCTSTR lpszOpenDir = NULL, LPCTSTR lpszTitle = NULL,
			LPCTSTR lpszPrompt = NULL, UINT nDlgID = 0);

		// GetPathName returns the path name the user selected
		CString GetPathName() const;

		// DoModal allows the user to perform the selection. Returns IDOK/IDCANCEL.
		// Returns 0 on error.
		virtual int DoModal();

		void SetMustExist(BOOL bMustExist);

	protected:
		// fDismissOpenDir returns TRUE if the dialog can be dismissed:
		//		szNew is the path when OK is hit or the directory list box selection
		//		is chosen.
		// This function *must* update m_strCurDir to be a copy of szNew.
		virtual BOOL	fDismissOpenDir(const LPCSTR szNew){return fDismissOpenDir (szNew, NULL);}
		virtual BOOL	fDismissOpenDir(const LPCSTR szNew, HWND hStatus);

		// m_dwHelpContext gives the help ID to link to the button.
		// CDirChooser supplies a default context, which is the directory picker description.
		DWORD	m_dwHelpContext;
		CString	m_strCurDir;
		CString	m_strPrompt;
		CString	m_strTitle;

	 	OPENFILENAME	m_ofn;
 		static UINT APIENTRY DirOpenHookProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
		static CDirChooser *pCurrentChooser;	// Only one chooser open at a time.

	public:
		BOOL m_bMustExist;

	private:
		char m_szFileName[_MAX_PATH];			// Common dialog proc fills this
};


typedef BOOL (FAR *LPFNFILEFINDVALIDATOR)(LPCTSTR szPath, LPCTSTR strFilename,	
	LONG lParam);


/////////////////////////////////////////////////////////////////////////////
//	CFileFindDlg
//			The semantics for dismissing the dialog are as follows:
//			The dialog will dismiss as soon as the user selects a directory in 
//			which a file with the name passed in to the constructor exists.
//		Parameters
//			strFileToFind - the filename for which we're searching.
//				DoModal won't return until the user specifies a directory in 
//				which this file can be found.
//			strOpenDir - Directory name that should be initially placed in 
//				the edit box. Default is the current working directory.
//			strTitle - Window title. Default is "Choose Directory"
//			strPrompt - prompt to place just above the dialog's edit box.
//				It's recommended that this string contain an accelerator (&)
//				so the user can go to the edit box via the mouse.  (Make
//				sure the accelerator doesn't conflict with the others in
//				the dialog!). Default is Directory &Name:

// Use the DoModal function to invoke the chooser. It returns IDOK / IDCANCEL,
// or 0 if there was an error.

class CFileFindDlg : public CDirChooser
{
	public:
		CFileFindDlg(LPCTSTR lpszFileToFind, LPCTSTR lpszOpenDir = NULL, 
					 LPCTSTR lpszTitle = NULL, LPCTSTR lpszPrompt = NULL);

		void SetValidator(LPFNFILEFINDVALIDATOR pfnValidator, LONG lparam);
		void ClearValidator (void) { SetValidator(NULL, 0);}

	protected:
		// fDismissOpenDir returns TRUE if the dialog can be dismissed:
		//		szNew is the path when OK is hit or the directory list box selection
		//		is chosen.
		// This function *must* update m_strCurDir to be a copy of szNew.
		virtual BOOL fDismissOpenDir(const LPCSTR szNew){return fDismissOpenDir (szNew, NULL);}
		virtual BOOL fDismissOpenDir(const LPCSTR szNew, HWND hStatus);

		CString m_strFileToFind;

		// Validation stuff
		CString m_strStatusMessage;
		LPFNFILEFINDVALIDATOR m_pfnValidator;
		LONG m_lValidatorParam;
	
};

////////////////////////////////////////////////////////////////////////////
// CDlgTab -- one page of a tabbed dialog

class CTabbedDialog;	// A tabbed "dialog" (really a modal popup window)

class CDlgTab : public CDialog
{
protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CDlgTab)
	
// Construction
public:
	CDlgTab(UINT nIDTemplate, UINT nIDCaption);
	CDlgTab(LPCSTR lpszTemplateName, UINT nIDCaption);

	virtual BOOL Activate(CTabbedDialog* pParentWnd, CPoint position);
	virtual void Deactivate(CTabbedDialog* pParentWnd);

protected:
// Implementation
	CDlgTab();

	virtual void OnContextMenu(CWnd* pWnd, CPoint point);
	virtual BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual UINT GetHelpID() { return m_nIDHelp; }

	// always call this to enable or disable a button in a dialog tab
	// it will ensure that the default button is passed correctly to the 
	// appropriate button in the parent dialog.  martynl 15Mar96
	virtual BOOL EnableButton(CWnd *button, BOOL bEnable);

public:
	virtual BOOL ValidateTab();

	virtual void CommitTab();
	virtual void CancelTab();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

#ifdef _DEBUG	
	void EndDialog(int nEndID);
#endif

	CString m_strCaption;
	
	friend class CTabbedDialog;

private:
	// Derived classes should use CommitTab() and CancelTab()
	virtual void OnOK();
	virtual void OnCancel();

// Attributes
public:
	int m_nOrder;
};

extern BOOL IsTabNextFocus(CDialog* pDlg, UINT nCmd);

////////////////////////////////////////////////////////////////////////////
// CTabs -- implementation for a generic row of tabs along the top of dialog

class CTabRecord : public CObject
{
public:
	CTabRecord(const char* szCaption, int nWidth, int nOrder, DWORD dw);

	CString	m_strCaption;
	CRect	m_rect;
	int 	m_nWidth;
	BOOL	m_bClipped;
	int     m_nOrder;
	DWORD	m_dw;
};

class CTabRow : public CObject
{
public:
	enum {
		SCROLL_LEFT = -5,		// all the SCROLL_ items must be less
		SCROLL_RIGHT = -6,		// than -1 to avoid ID conflict
		SCROLL_NULL = -7,

		cxScrollSize = 31,		// size of the scroll button bitmap
		cyScrollSize = 17,
		cxScrollMargin = 10,	// blank margin between scroll buttons and tabs

		cxSelectedTab = 2,		// amount to inflate the selected tab
		cySelectedTab = 2,
		cxTabTextMargin = 10,	// extra width for a tab past text width
		cyTabTextMargin = 3,	// extra height for a tab past text height

		TIMER_ID = 15,
		TIMER_DELAY = 500
	};

	// Construction
	CTabRow();
	~CTabRow();

	void	ResetContent();
	void	AddTab(CString& szCaption, CDC* pDC, int nOrder, DWORD dw = 0);
	void	SetPos(CRect rect);		// set position of tab row within window
	int 	DesiredHeight(CDC* pDC);	// how much space does tab row need?
	void	SetParent(CWnd* pWndOwner)
	    		{ m_pWndOwner = pWndOwner; }

	// Operations
	int		SetActiveTab(int nTab);
	int		SetActiveTab(const CString& str);
	int 	GetActiveTab()
	    		{ return m_curTab; }

	void	PutFocus(BOOL bTabsHaveFocus);
	BOOL	HasFocus()
	    		{ return m_bHasFocus; }

	void	Scroll(int nDirection);
	void	ScrollIntoView(int nTab);

	// Drawing stuff
	void	Draw(CDC* pDC);
	void	DrawFocusRect(CDC* pDC);
	void	DrawPageRect(CDC* pDC, const CRect& rectPage);
	void	InvalidateTab(int nTab, BOOL bInflate = TRUE);
	CRect	GetRect() const
				{ return m_rowRect; }

	// Mouse stuff
	int 	TabFromPoint(CPoint pt);
	void	OnMouseMove(CPoint pt);
	void	OnLButtonUp(CPoint pt);
	void	OnTimer();
	void	Capture(int nDirection);
	BOOL	HasCapture()
	    		{ return m_nScrollState != SCROLL_NULL; }

	// Misc
	DWORD	GetTabDWord(int nTab)
				{ return GetTab(nTab)->m_dw; }
	LPCTSTR GetTabString(int nTab)
				{ return GetTab(nTab)->m_strCaption; }
	int 	MaxTab()
	    		{ return m_tabs.GetSize() - 1; }

private:
	void	DrawTab(CDC* pDC, int nTab, BOOL bCurTab);
	void	DrawScrollers(CDC* pDC);

	BOOL	CanScroll();
	void	SetFirstTab(int nTab);
	CTabRecord*	GetTab(int nTab)
				{ return (CTabRecord*)m_tabs[nTab]; }
	BOOL	IsTabVisible(int nTab, BOOL bPartials = TRUE);

	// Member variables
	CRect	m_rowRect;		// location of entire tab row within owning window
	CRect	m_scrollRect;	// location of scroll buttons
	int 	m_curTab;		// index of current selected tab
	int 	m_firstTab;		// index of leftmost visible tab
	BOOL	m_bHasFocus;	// whether tab row has the focus
	int 	m_nScrollState;	// shows whether left or right scroll btn is down
	BOOL	m_bScrollPause;	// if we have capture, has the mouse wandered off btn?

	CWnd*	m_pWndOwner;	// pointer to owning window (used for InvalidateRect)
	CObArray	m_tabs;		// list of CTabRecords, in order
};


////////////////////////////////////////////////////////////////////////////
// CTabbedDialog -- a tabbed "dialog" (really a popup-window)
class CLastTabMap : public CMapWordToOb
{
	public:
		~CLastTabMap();
		__inline void RememberTab(WORD wCaptionID, CString & strCaption)
		{
			CString * pStr; if (!Lookup(wCaptionID, (CObject *&)pStr))	pStr = new CString;
			*pStr = strCaption; SetAt(wCaptionID, (CObject *&)pStr);
		}
		__inline BOOL LookupTab(WORD wCaptionID, CString & strCaption)
		{
			CString * pStr; if (!Lookup(wCaptionID, (CObject *&)pStr))	return FALSE;
			strCaption = *pStr; return TRUE;
		}
};

class CTabbedDialog : public CWnd
{
// Construction
public:
	enum COMMIT_MODEL { commitOnOk, commitOnTheFly };
	enum FC { FC_THIS, FC_NEXT, FC_PREVIOUS, FC_FIRST };
	enum DLGPOSITION { POS_APPCENTER, POS_AVOIDBARS };

	CTabbedDialog(UINT nIDCaption, CWnd* pParentWnd = NULL,
			UINT iSelectTab = -1, COMMIT_MODEL = commitOnOk, DLGPOSITION pos=POS_APPCENTER);
	
	// Modeless operation
	BOOL Create();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Modal operation
	int DoModal();
	void EndDialog(int nEndID);


// Operations
public:
	virtual void OnOK();
	virtual void OnCancel();
	virtual void OnHelp();

	virtual void OnSelectTab(int nTab) {};

	void AddTab(CDlgTab* pDlgTab);
	// may return failure (FALSE) if the tabs cannot be
	// cleared (most likely because the current one
	// failed the validation check)
	BOOL ClearAllTabs();
	// nTab == -1 will de-select the current tab
	// and not re-select any other
	void SelectTab(int nTab, BOOL bGoToControl = FALSE);
	void SelectTab(const CString& strCaption, BOOL bGoToControl = FALSE);
	void SelectNextTab();
	void SelectPreviousTab();
	int GetTabCount() const
		{ return m_tabs.GetSize(); }
	void SetCaption( const char *szCaption )
		{ m_strCaption = szCaption; }
	int GetCurrentTab() const
		{ return m_nTabCur; }

	// PreTranslate processing
	BOOL MessageStolenByTabbedDialog(MSG* pMsg);
	void HandleTab(BOOL bShift, BOOL bControl);
	BOOL HandleAcceleratorKey(char ch);
	BOOL HandleTabRowKey(char ch);
	BOOL IsDone() { return m_nID != -1; }	// HACK:  This is to fix ClassWiz bug 14201

// Implementation
protected:
	CDlgTab* GetTab(int nTab) {return 0 <= nTab && nTab < GetTabCount() ? (CDlgTab*)m_tabs[nTab] : (CDlgTab *)NULL;}
	void DrawFocusRect();
	BOOL ActivateTab(int nTab, BOOL bGoToControl);
	void BuildTabItemList(CObList& list, CMapWordToOb& map, CWnd* pWndCur = NULL);
	CWnd* FindControl(CWnd* pWndGoal, CObList& list, FC fc = FC_NEXT);
	void SetFocusToControl(CWnd* pControl, CObList* list = NULL, BOOL bSetSel = TRUE);


	// Sizing
	CPoint GetDlgTabPos();
	virtual void GetTabSize(CRect& rect);
	virtual void GetMargins(CRect& rect);

	// Buttons
	BOOL IsButton(HWND hWnd);
	virtual void CreateButtons();
	virtual void GetButtonExtent(CSize& size);
	virtual void MoveButtons(int nLeft, int nCenter);
	virtual CButton* GetButtonFromIndex(int index);
	void SetDefButtonIndex(int index);

	enum {
		cyBottomMargin = 6,	// plus button height, of course
		duButtonWidth = 45,
		duButtonHeight = 14,
		duBetweenButtons = 6
	};

	CSize m_buttonSize;
	int   m_cxBetweenButtons;

	CObArray m_tabs;	// array of CDlgTab pointers
	int m_nTabCur;  	// which is the current tab
	//CWnd* m_pFocusWnd; // focus when we lost activation
	HWND m_hFocusWnd;	// focus when we lost activation
	CWnd* m_pParentWnd; // owner of the tabbed dialog
	int m_nID;      	// ID passed to EndDialog and returned from DoModal
	CString m_strCaption; // caption of the pseudo-dialog
	UINT nCaptionID;
	static CLastTabMap m_DialogMap;	// Used to remember the last used page in all tabbed dialogs
	COMMIT_MODEL	m_commitModel;
	DLGPOSITION m_position;

	CTabRow	m_tabRow;		// entire row of tabs at top of dialog

	CButton m_btnOk;
	CButton m_btnCancel;
 	int m_iDefBtnIndex;

	// Generated message map functions
	//{{AFX_MSG(CTabbedDialog)
	afx_msg void OnPaint();
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpcs);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nTimerID);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	virtual void OnContextMenu(CWnd* pWnd, CPoint point);
	virtual BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CTabbedDialog);
};

#define cxTabMargin		6	// margins between non-client and outlines
#define cyTabMargin		6	//                  ''

/////////////////////////////////////////////////////////////////////////////
// CAddFilesDialog

#include <prjapi.h>
#include <prjguid.h>

// Note: does not derive from CWnd.  Dialog is created through call to
// GetOpenFileName instead of the usual MFC process.

class CAddFilesDialog : public C3dFileDialog
{
public:
	CAddFilesDialog();
	CAddFilesDialog(IPkgProject *pProject, const CString &strFolderName);
	virtual ~CAddFilesDialog();
	virtual int DoModal();

	IPkgProject *GetProject() const { return(m_pProject); }

	static void ResetCurrentDir();

	CStringArray m_saFileNames;

protected:
	// Generated message map functions
	//{{AFX_MSG(CAddProjectDialog)
	virtual BOOL OnInitDialog();
	virtual BOOL OnFileNameOK();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP();

private:
	BOOL m_fSuccess;

	IPkgProject *m_pProject;
	CString m_strFolderName;

	const static UINT msgValidateFileMessage;
	static CDir c_dirInitial;
};

/////////////////////////////////////////////////////////////////////////////

extern void PreModalWindow();
extern void PostModalWindow();
        
#undef AFX_DATA
#define AFX_DATA NEAR

#endif	// __DLGBASE_H__
