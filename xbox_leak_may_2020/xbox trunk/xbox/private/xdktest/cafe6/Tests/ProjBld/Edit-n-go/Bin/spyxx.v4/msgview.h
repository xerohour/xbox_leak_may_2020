// msgview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgView view

class CMsgDoc;

class CMsgView : public CView
{
	DECLARE_DYNCREATE(CMsgView)
protected:
	CMsgView();		 // protected constructor used by dynamic creation
	CMsgDoc * GetDocument() const { return (CMsgDoc *)CView::GetDocument(); }

// Attributes
public:

// Operations
public:
	void AddLine(LPSTR psz, PMSGSTREAMDATA pmsd);
	void SetFont(CFont* pFont, BOOL bRedraw = TRUE)
	{
		m_MsgLog.SetFont(pFont, bRedraw);
		m_hfont = (HFONT)pFont->m_hObject;
	}
	inline void SetMaxLines(int cLinesMax);
	inline int GetMaxLines();

// Implementation
protected:
	virtual ~CMsgView();
	virtual void OnDraw(CDC* pDC);	  // overridden to draw this view

	CMsgLog m_MsgLog;
	HFONT m_hfont;

	friend class CMsgDoc;
	friend class CMsgLog;

	// Generated message map functions
protected:
	//{{AFX_MSG(CMsgView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnViewFont();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnViewProperties();
	afx_msg void OnUpdateViewProperties(CCmdUI* pCmdUI);
	afx_msg void OnViewHelpOnMessage();
	afx_msg void OnUpdateViewHelpOnMessage(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline void CMsgView::SetMaxLines(int cLinesMax)
{
	m_MsgLog.SetMaxLines(cLinesMax);
}

inline int CMsgView::GetMaxLines()
{
	return m_MsgLog.GetMaxLines();
}

/////////////////////////////////////////////////////////////////////////////
