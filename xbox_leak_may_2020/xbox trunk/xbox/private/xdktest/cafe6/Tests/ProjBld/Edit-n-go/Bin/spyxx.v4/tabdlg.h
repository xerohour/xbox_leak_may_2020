// tabdlg.h : header file
//

#ifndef __TABDLG_H__
#define __TABDLG_H__

//{patbr] we are not a DLL
//#undef AFXAPP_DATA
//#define AFXAPP_DATA AFXEXT_DATA

class CDlgTab;			// One "tab" page in the tabbed dialog (a child)
class CTabRow;			// The row of tabs along top of tabbed dialog
class CTabbedDialog;	// A tabbed "dialog" (really a modal popup window)


#define cxTabMargin		6	// margins between non-client and outlines
#define cyTabMargin		6	//                  ''


////////////////////////////////////////////////////////////////////////////
// CDlgTab -- one page of a tabbed dialog

class CDlgTab : public CDialog
{
protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CDlgTab)
	
// Construction
public:
	CDlgTab(UINT nIDTemplate, UINT nIDCaption);
	CDlgTab(LPCSTR lpszTemplateName, UINT nIDCaption);

protected:
// Implementation
	CDlgTab();

	virtual BOOL Activate(CTabbedDialog* pParentWnd, CPoint position);
	virtual void Deactivate(CTabbedDialog* pParentWnd);

public:
	virtual BOOL ValidateTab();

protected:
	virtual void CommitTab();
	virtual void CancelTab();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

#ifdef _DEBUG	
	void EndDialog(int nEndID);
#endif

	CString m_strCaption;
	
	friend class CTabbedDialog;

private:
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
	enum FC { FC_THIS, FC_NEXT, FC_PREVIOUS };

	CTabbedDialog(UINT nIDCaption, CWnd* pParentWnd = NULL,
	              UINT iSelectTab = -1, COMMIT_MODEL = commitOnOk);
	
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
	void SelectTab(int nTab);
	int GetTabCount() const
		{ return m_tabs.GetSize(); }
	void SetCaption( const char *szCaption )
		{ m_strCaption = szCaption; }

	// PreTranslate processing
	BOOL MessageStolenByTabbedDialog(MSG* pMsg);
	void HandleTab(BOOL bShift, BOOL bControl);
	BOOL HandleAcceleratorKey(char ch);
	BOOL HandleTabRowKey(char ch);

// Implementation
protected:
	CDlgTab* GetTab(int nTab) {return 0 <= nTab && nTab < GetTabCount() ? (CDlgTab*)m_tabs[nTab] : (CDlgTab *)NULL;}
	void DrawFocusRect();
	BOOL ActivateTab(int nTab);
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

	CTabRow	m_tabRow;		// entire row of tabs at top of dialog

	CButton m_btnOk;
	CButton m_btnCancel;
	CButton m_btnHelp;
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
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CTabbedDialog);
};

//[patbr] we are not a DLL
//#undef AFXAPP_DATA
//#define AFXAPP_DATA NEAR

/////////////////////////////////////////////////////////////////////////////
#endif // __TABDLG_H__
