// spytreev.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpyTreeView view

class CSpyTreeView : public CView
{
	DECLARE_DYNCREATE(CSpyTreeView)
protected:
	CSpyTreeView();		 // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	void SetFont(CFont* pFont, BOOL bRedraw = TRUE)
	{
		m_SpyTreeCtl.SetFont(pFont, bRedraw);
		m_hfont = (HFONT)pFont->m_hObject;
	}

// Implementation
protected:
	virtual ~CSpyTreeView();
	virtual void OnDraw(CDC* pDC);	  // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

protected:
	CSpyTreeCtl m_SpyTreeCtl;
	HFONT m_hfont;

	// Generated message map functions
protected:
	//{{AFX_MSG(CSpyTreeView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnViewFont();
	afx_msg void OnViewProperties();
	afx_msg void OnUpdateViewProperties(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CSpyTreeDoc;
	friend class CWndTreeDoc;
	friend class CPrcTreeDoc;
	friend class CThdTreeDoc;
};

/////////////////////////////////////////////////////////////////////////////
