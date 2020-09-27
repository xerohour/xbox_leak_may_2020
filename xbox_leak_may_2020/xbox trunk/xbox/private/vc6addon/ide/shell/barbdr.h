///////////////////////////////////////////////////////////////////////////////
//	BDRBAR.H
//      Contains declarations relavant to CBorderBar.
///////////////////////////////////////////////////////////////////////////////

#ifndef __BDRBAR_H__
#define __BDRBAR_H__

class CDockManager;

/////////////////////////////////////////////////////////////////////////////
// CBorderBar window

class CBorderBar : public CWnd
{
// Construction
public:
	CBorderBar();
	virtual ~CBorderBar();
	
	BOOL Create(CWnd* pParent, CDockManager* pManager, DWORD dwStyle,
		HWND hwndChild, UINT nID, BOOL bRegister = FALSE, DOCKINIT FAR* lpdi = NULL);

	void RecalcLayout();

// Attributes
public:
	UINT m_nID;
	int m_cxLeftBorder, m_cxRightBorder;
	int m_cyTopBorder, m_cyBottomBorder;

	BOOL m_bCaption:1;
	BOOL m_bHorz:1;
	BOOL m_bBtnClose:1;
	BOOL m_bBtnMax:1;
	BOOL m_bBtnPushed:1;

	int m_nBtnCapture;
	
// Operations
public:

	// Generated message map functions
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	
	//{{AFX_MSG(CBorderBar)
	afx_msg void OnNcPaint();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LRESULT OnRouteChildMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEnableChild(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetMoveSize(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGetTrackSize(WPARAM wParam, LPARAM lParam);

	LRESULT OnSizeMessage(UINT message, DOCKPOS dp,
		WPARAM wParam, LPARAM lParam);

	enum
	{
		BUTTON_NIL = -1,
		BUTTON_CLOSE = 0,
		BUTTON_MAX,
	};

	int InCaptionButtonRect(CPoint point) const;
	void GetCaptionButtonRect(int nType, CRect& rectButton) const;
	void DrawCaptionButton(int nType, CDC* pdc, BOOL bPushed);
	
	DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(CBorderBar)
};

#endif	// __BDRBAR_H__
