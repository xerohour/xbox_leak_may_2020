#if !defined(MAINFRM_H__B6AED00C_3BDF_11D0_89AC_00A0C9054129__INCLUDED_)
#define MAINFRM_H__B6AED00C_3BDF_11D0_89AC_00A0C9054129__INCLUDED_

// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "JazzToolBar.h"
#include "TreeBar.h"
#include "PropertyBar.h"


/////////////////////////////////////////////////////////////////////////////
// CJzWaitCursor

class CJzWaitCursor
{
// Constructor
public:
	CJzWaitCursor( CFrameWnd* pMainFrame, CWnd* pWndCapture );
	virtual ~CJzWaitCursor();
	ULONG StartWait();
	ULONG EndWait();

// Attributes
private:
	DWORD		m_dwRef;
	CWnd*		m_pWndCapture;
	CFrameWnd*	m_pMainFrame;
};


/////////////////////////////////////////////////////////////////////////////
// CJzStatusBarPane

class CJzStatusBarPane
{
// Constructor
public:
	CJzStatusBarPane( HANDLE hKey, short nLifeSpan, short nIndex );
	virtual ~CJzStatusBarPane();

// Attributes
public:
	HANDLE			m_hKey;
	short			m_nLifeSpan;		// List sorted high to low
	short			m_nStatusBarIndex;	// Ascending order in list
	short			m_nIndex;			// Index within this hKey
	short			m_nWidth;
	CString			m_strText;
	UINT			m_nStyle;
};


/////////////////////////////////////////////////////////////////////////////
// CMyMDIClient

class CMyMDIClient : public CWnd
{
public:
	CMyMDIClient();
	virtual ~CMyMDIClient();

	void LoadWallpaper();

private:
	CBitmap*	m_pbmpWallpaper;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyMDIClient)
	public:
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyMDIClient)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	CTreeCtrl* GetProjectTreeCtrl();
	void EndTrack( int nWidth );
	CWnd* GetFirstMDIChild();
	BOOL ActivateMDIChild( int nPos );
	HRESULT GetStatusBarPane( HANDLE hKey, int nIndex, CJzStatusBarPane** ppJzStatusBarPane );
	HRESULT SyncStatusBarPanes( void );
	void InsertStatusBarPane( CJzStatusBarPane* pJzStatusBarPane );

protected:
	void InitWindowMenu( CMenu* pWindowMenu );
	BOOL OnSelectWindowMenu( UINT nCommandID );

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual void GetMessageString( UINT nID, CString& rMessage ) const;
	virtual void OnUpdateFrameMenu( HMENU hMenuAlt );
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	virtual BOOL DestroyWindow();
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
	void DropFiles( HDROP hDropInfo );

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	CMyMDIClient		m_wndMDIClient;

public:
	CToolBar			m_wndToolBar;
	CBookmarkToolBar	m_wndBookmarkToolBar;
	CTreeBar			m_wndTreeBar;
	CStatusBar			m_wndStatusBar;
	CPropertyBar		m_wndProperties;
	CJzWaitCursor*		m_pWaitCursor;
	CProgressCtrl*		m_pProgressCtrl;
	HANDLE				m_hProgressCtrl;
    
	CTypedPtrList<CPtrList, CJzStatusBarPane*> m_lstStatusBarPanes;

private:
	BOOL CreateToolBar( void );
	BOOL CreateBookmarkBar( void );
	BOOL CreateStatusBar( void );
	BOOL CreateTreeBar( void );
	BOOL CreatePropertiesBar( void );
	void OnHelp( void );
	LRESULT OnApp( WPARAM wParam, LPARAM lParam );
	
// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewProjectTree();
	afx_msg void OnUpdateViewProjectTree(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnViewStatusBar();
	afx_msg void OnViewToolbar();
	afx_msg void OnViewProperties();
	afx_msg void OnUpdateViewProperties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnViewToggle();
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	afx_msg void OnViewBookmarks();
	afx_msg void OnUpdateViewBookmarks(CCmdUI* pCmdUI);
	afx_msg void OnTransPlay();
	afx_msg void OnTransPlayFromStart();
	afx_msg void OnTransRecord();
	afx_msg void OnDestroy();
	afx_msg void OnTransTransition();
	afx_msg void OnTransSpace();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(MAINFRM_H__B6AED00C_3BDF_11D0_89AC_00A0C9054129__INCLUDED)
