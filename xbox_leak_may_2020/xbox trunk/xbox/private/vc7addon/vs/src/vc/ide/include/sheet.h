/////////////////////////////////////////////////////////////////////////////
//	SHEET.H

#ifndef __SHEET_H__
#define __SHEET_H__

#include "slob.h"
#include "proppage.h"
#include "tabdlg.h"

//#undef AFX_DATA
//#define AFX_DATA AFX_EXT_DATA

#define cxPageMargin 6
#define cyPageMargin 4
#define cxWndMargin 5
#define cyWndMargin 5

#define MAX_TEXTSIZE 255 // normal edit controls

#define	INITIAL_LEVELCOMBO_HEIGHT	26

/////////////////////////////////////////////////////////////////////////////
// CSheetSlob

class CSheetSlob : public CSlob
{
	DECLARE_DYNAMIC(CSheetSlob)
public:
	CSheetSlob();
	~CSheetSlob();

	void OnInform(CSlob* pChangedSlob, UINT idChange, DWORD_PTR dwHint);
	void NukeInforms();
	void FlushInforms();
	inline BOOL HasInforms() { return !m_informList.IsEmpty(); }

	class CSheetWnd* m_pSheetWnd;
	CPtrList m_informList;
};

class CInformRecord
{
public:
	CInformRecord(CSlob*, UINT);

	CSlob*	m_pChangedSlob;
	UINT	m_idChange;
};

/////////////////////////////////////////////////////////////////////////////
//	CPropFrameToolBar
//		This is the minature toolbar with pushpin and help buttons

class CPropFrameToolBar : public CToolBar
{
public:
	enum {
		sizeButtonX = 21,
		sizeButtonY = 20,
		sizeImageX = 15,
		sizeImageY = 14,
	};

	CPropFrameToolBar();

	void SetSizes();

	BOOL IsPinned();
	void SetPushPin(BOOL bPushed = TRUE);
};

/////////////////////////////////////////////////////////////////////////////
//	CSheetWnd
// 		Implements the property frame, displaying C3dPropertyPage objects
//		in it's client area.

class CSheetWnd : public CMiniFrameWnd
{
public:
	static CObList s_listLoadedPages;
	static int s_cyBreakLine;	// y-coord of top of page

public:
	enum SELSTYLE { 					// selection state
		none,
		editor,
		multiple,
		blank,
        editorupdate                    // force an update even if sel same
	};

	enum {
		cxSize = 395,
		cySize = 157,
		xDefaultPos = 60,
		yDefaultPos = 470,
	};

// Construction
public:
	CSheetWnd();
	virtual ~CSheetWnd();

	BOOL Create(CWnd * pParentWnd);

public:
	void InitialUpdate(CPoint);
	void IdleUpdate();

	virtual void RecalcLayout(BOOL bNotify = TRUE);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void CancelPage();
	BOOL IsInCancel() const { return m_bInCancel; }

	void SetActive(BOOL bActive);
	BOOL IsActive() const { return m_bActive; }

	BOOL Validate();
	BOOL ValidateRequest();
	BOOL IsValidating() const { return m_bValidating; }

	void ResetFocusSave() { m_iFocusSave = 0; }
	void SetDefaultPage();
	BOOL FlipToPage(int iStringID);
	BOOL FlipToPage(const CString& str);
	void IncrementPage(int nInc);
	void ClearPage();
	void SelectionChange(CSheetWnd::SELSTYLE, CSlob *, int nPage = -1);
	void CurrentSlobDestroyed();
	void SetPushPin(BOOL fPushed = TRUE);
	BOOL IsPinned();

	CWnd* SetEditFocus(UINT nProp);
	CWnd* GetControl(UINT nControlID);

	BOOL SaveFocus();
	BOOL RestoreFocus();
	void DrawFocusRect();
	
	BOOL UndoPendingValidate();

	void FlushUnusedPages();
	void RemovePageFromUse(C3dPropertyPage* pPage);
	inline void NukeInforms() { m_sheetSlob.NukeInforms(); }
	inline void FlushInforms() { m_sheetSlob.FlushInforms(); }
	inline BOOL HasInforms() { return m_sheetSlob.HasInforms(); }

	// Methods used during CSlob::SetupPropertyPages()
	int SetupPropertyPages(void);
	int AddPropertyPage(C3dPropertyPage* pPage, CSlob* pSlob);
	void SetCaption(LPCTSTR sz);
	BOOL InhibitExtraPages(BOOL bInhibit = TRUE);
	int StartNewPageSet(void);
	int MergePageSets(void);
	CSize CalcMaxPageSize(BOOL bNewPages = FALSE);

	// please use the method GetCurrentPage rather than
	// access the m_pCurPage data directly .. the latter
	// may not be valid at all times!
	inline C3dPropertyPage* GetCurrentPage() { IdleUpdate(); return m_pCurPage; }

	int GetLevelComboHeight();

	C3dPropertyPage* m_pCurPage;
	CSlobPage m_noPage;
	CSlobPage m_nullPage;
	CSlobPage m_multiPage;

	// For pages which want to expand beyond their minimum size, if some other page
	// has already forced the sheet to be larger.
	// Set in C3dPropertyPage::SetupPage() ... the page can look at the value during
	// C3dPropertyPage::InitializePage() and change its size if desired.
	CSize m_sizePageMax;

protected:
	BOOL m_bInCancel:1;
	BOOL m_bSheetChanged:1;
	BOOL m_bValidating:1;
	BOOL m_bValidateEnabled:1;

	// This bool is true whenever the property window is the active app
	// window (still true if the property window was active when the
	// app was deactivated), or false if some other app window (usually
	// an MDI child, or the main frame) is active (or was when the app
	// was deactivated).
	BOOL m_bActive:1;

//	BOOL m_bExtraPagesAdded:1;	// used by AddOnExtraPages()
	int m_refInhibitExtraPages;	// ref-counts InhibitExtraPages

	CObList m_listCurPages;	// current list of pages for m_pCurSlob
	CObList m_listNewPages;	// list of pages m_pNewSlob adds with AddPropertyPage
	int m_nFirstPageSet;	// used when merging pages in m_listNewPages

	CTabRow m_tabRow;
	CPropFrameToolBar m_toolBar;

	HWND m_hwndFocusSave;	// HWND of the last control w/ focus.
	int m_iFocusSave;	// ID of the last control w/ focus.
	int m_iActiveTab;

	CSlob *m_pCurSlob;	// The currently attached editor
	CSlob *m_pNewSlob;	// Editor to attach at next idle moment
	CSheetWnd::SELSTYLE m_selState;
	CSheetWnd::SELSTYLE m_newSelState;
	int m_nCurPage;
	int m_nNewPage;

	CSheetSlob m_sheetSlob;

	CString m_strDefaultPage;

	CComboBox	m_cbLevel;
	int			m_cyRowHeight;

// Message handling
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nTimerID);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnIsError(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnValidateReq(WPARAM wParam, LPARAM lParam);
	
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);

	afx_msg void OnSelPage();

	afx_msg void CmdOkPage();
	afx_msg void CmdCancelPage();
	afx_msg void CmdNextPage();
	afx_msg void CmdPrevPage();

	afx_msg void OnCmdPushPin();
	afx_msg void OnCmdHelp();

	afx_msg void OnSelChangeLevel();

	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	
	DECLARE_MESSAGE_MAP()

	friend class CSheetSlob;
};

// Globals
extern CSheetWnd *g_PropSheet;
extern BOOL g_PropSheetVisible;

// Global helper functions

CSheetWnd* GetPropertyBrowser();
void SetPropertyBrowserSelection(CSlob* pSlob, int nPage = -1, 
	CSheetWnd::SELSTYLE style = CSheetWnd::editor); // select slob or NULL

/////////////////////////////////////////////////////////////////////////////

//#undef AFX_DATA
//#define AFX_DATA NEAR

#endif // __SHEET_H__
