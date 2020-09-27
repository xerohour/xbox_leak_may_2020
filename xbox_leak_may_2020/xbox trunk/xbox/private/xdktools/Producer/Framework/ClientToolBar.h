#if !defined(AFX_CLIENTTOOLBAR_H__04E207C2_C636_11D0_89AE_00A0C9054129__INCLUDED_)
#define AFX_CLIENTTOOLBAR_H__04E207C2_C636_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ClientToolBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CClientToolBar window

class CClientToolBar : public CToolBar
{
// Construction
public:
	CClientToolBar( IDMUSProdToolBar* pIToolBar );

// Attributes
public:
	HINSTANCE			m_hInstance;
	IDMUSProdToolBar*	m_pIToolBar;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientToolBar)
	//}}AFX_VIRTUAL
	virtual BOOL SetStatusText(int nHit);
    virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
    virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);

protected:
	CSize CalcLayout(DWORD nMode, int nLength = -1);
	CSize CalcSize(TBBUTTON* pData, int nCount);
	int WrapToolBar(TBBUTTON* pData, int nCount, int nWidth);
	void SizeToolBar(TBBUTTON* pData, int nCount, int nLength, BOOL bVert = FALSE);
	void _SetButton(int nIndex, TBBUTTON* pButton);
	void _GetButton(int nIndex, TBBUTTON* pButton);

	CSize m_sizeDefaultButton; // default button size

// Implementation
public:
	virtual ~CClientToolBar();


	// Generated message map functions
protected:
	//{{AFX_MSG(CClientToolBar)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTTOOLBAR_H__04E207C2_C636_11D0_89AE_00A0C9054129__INCLUDED_)
