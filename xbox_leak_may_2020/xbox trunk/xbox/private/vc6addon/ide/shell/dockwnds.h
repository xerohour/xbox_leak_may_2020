///////////////////////////////////////////////////////////////////////////////
//	DOCKWNDS.H
//		Declarations for the window classes associated with the docking
//		architecture.
//

#ifndef __DOCKWNDS_H__
#define __DOCKWNDS_H__

class CDockWorker;
class CDockManager;
class CPartView;

///////////////////////////////////////////////////////////////////////////////
//	CPaletteWnd window

class CPaletteWnd : public CMiniFrameWnd
{
public:
    static void GetPaletteRect(CRect& rect, CWnd* pChildWnd = NULL);
    static void GetChildRect(CRect& rect);

    CPaletteWnd(CDockManager* pManager);
    ~CPaletteWnd();

    BOOL Create(LPCSTR lpClassName, LPCSTR lpTitle, CWnd* pChildWnd, CWnd* pParentWnd, CRect rect, UINT nID = 0, BOOL bHasClose=TRUE);
        
	void SetManager(CDockManager* pManager);
	CDockWorker* GetWorker() const;

    void ActivateWindow(BOOL bActive);
	void ActivateNoFocus();
        
// Generated message map functions
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

    //{{AFX_MSG(CPaletteWnd)
    afx_msg BOOL OnNcActivate(BOOL bActive);
    afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint pt);
    afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint pt);
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnRouteMessage(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnBarSizeChanging(WPARAM wParam, LPARAM lParam);
    
    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(CPaletteWnd)

// Helping functions    
protected:
	void SaveFocus();
	void RestoreFocus();

// Implementation
protected:
	HWND m_hwndFocus;
    CDockManager* m_pManager;
};

inline void CPaletteWnd::SetManager(CDockManager* pManager)
{	m_pManager = pManager;	}

/////////////////////////////////////////////////////////////////////////////
// CDockWnd window

struct  _DWI {
    int iRow, iPos, cAlong, cAcross;
    CWnd* pWnd;
};

struct DOCKRESTORE {
	ULONG nIDRow;
	int iPos, cAlong, cAcross;
};

struct ROWSTRUCT {
	int nStart;
	ULONG nIDRow;
	BOOL bStretchy:1;
	BOOL bMenuBar:1;		// A row containing the menu bar contains nothing else
	BOOL bMaxy:1;
	BOOL bInit:1;
	union
	{
		CWnd* pWndMax;
		UINT nIDMaxWindow;
	};
	int nSplitters;
	int* pnPositions;
};

class CDockWnd : public CWnd
{
// Construction
protected:
	CDockWnd();
	
public:
    CDockWnd(DOCKPOS dp);
    ~CDockWnd();
    
    BOOL Create(CFrameWnd* pFrame, UINT nID, CDockManager* pManager);

// Operations
public:
	void GetDockRestore(DOCKRESTORE* pdr, CWnd* pWnd);
    void GetDockInfo(_DWI* pdwi, CWnd* pWnd);
    _DWI* GetRowInfo(int iRow = -1);

    BOOL AddWnd(CWnd* pWnd, CSize size);
	BOOL AddWnd(CWnd* pWnd, DOCKRESTORE* pdr);
    BOOL AddWnd(CWnd* pWnd, const CRect& rect, ULONG nID = ULONG_MAX);
    void MoveWnd(CWnd* pWnd, CRect rect,
    	BOOL bAdd = FALSE, ULONG nID = ULONG_MAX);
    void RemoveWnd(CWnd* pWnd, CWnd* pNewParent);

	BOOL IsInsert(CRect rect, int* piRow, BOOL bMenu);

	MAX_STATE GetMaxState(CWnd* pWnd);
	void MaximizeRestore(CWnd* pWnd);
    
// Implementation
public:
	BOOL m_bInitialized:1;
	BOOL m_bChicagoSizingHack:1;

	DOCKPOS m_dp;
	int m_nAlong;	// Window size along a row.
    int m_nRows;
    ROWSTRUCT* m_arRows;
	ULONG m_nIDNext, m_nIDPrev;
    CDockManager* m_pManager;
    
    static HCURSOR s_hcurHSize, s_hcurVSize;

// Layout helping functions.
protected:
    _DWI* BeginLayout();
    void EndLayout(_DWI* rgdwi, BOOL bRecalc = TRUE);
    
    _DWI* FindDockInfo(_DWI* pdwi, CWnd* pWndFind);

    void RemoveRow(_DWI* pdwi, int iRow);
    void InsertRow(_DWI* pdwi, int iRow, int iSize, ULONG nID = ULONG_MAX);
    void SizeRow(_DWI* pdwi, int iRow, int iSize = 0);
    void RecalcRow(_DWI* rgdwi, int iRow, CWnd* pWnd = NULL,
    	BOOL bForceWrap = FALSE);
    void SortRow(_DWI* pdwi, int iRow, CWnd* pWnd = NULL);
    
    BOOL IsRowEmpty(_DWI* pdwi, int iRow);

	int GetWindowOffset(int iRow);
	void GetSplitterRect(int iRow, CRect& rect, BOOL bTracking = FALSE);
	void GetSplitterRect(int iRow, int iPosition, CRect& rect,
		BOOL bTracking = FALSE);
	void GetBoundaryRect(CPoint ptHitTest, CRect& rect);
	CPoint HitTest(CPoint point);
	
	BOOL TrackSize(CPoint pt, CPoint ptHitTest, int& nFinalOffset);
	void DrawTrackRect(CDC* pdc, const CRect* pRect1,
		const CRect* pRect2 = NULL);

	void OnDrawChicago(CDC* pDC);
	void OnDrawNT(CDC* pDC);
	
// Generated message map functions
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
    //{{AFX_MSG(CDockWnd)
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpParams);
	afx_msg void OnNcPaint();
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	//}}AFX_MSG
    afx_msg LRESULT OnSizeParent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnRouteMessage(WPARAM wParam, LPARAM lParam);
    
    virtual void Serialize(CArchive& ar);

    DECLARE_MESSAGE_MAP()
    DECLARE_SERIAL(CDockWnd)
};

void DockTranspose(DOCKPOS dp, SIZE FAR* lpsize);
void DockTranspose(DOCKPOS dp, LPPOINT lppt);
void DockTranspose(DOCKPOS dp, LPRECT lprect);

///////////////////////////////////////////////////////////////////////////////
//	CMDIChildDock window
//

class CMDIChildDock : public CPartFrame
{
public:
    CMDIChildDock(CDockManager* pManager);
    ~CMDIChildDock();

    BOOL Create(LPCSTR lpClassName, LPCSTR lpTitle, CWnd* pChildWnd,
        CMDIFrameWnd* pParentWnd, CRect rect);

	void ParentChild(CWnd* pWndNewParent);
	void SetManager(CDockManager* pManager);
	CDockWorker* GetWorker() const;

public:
	virtual BOOL IsInWorkspace() { return FALSE; }

protected:
	void SetChildID(CWnd* pChildWnd, UINT nID);

    //{{AFX_MSG(CMDIChildDock)
	afx_msg void OnClose();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CMDIChildDock)

protected:
    CDockManager* m_pManager;
	UINT m_nIDSave;
};

inline void CMDIChildDock::SetManager(CDockManager* pManager)
{	m_pManager = pManager;	}

#endif	// __DOCKWNDS_H__
