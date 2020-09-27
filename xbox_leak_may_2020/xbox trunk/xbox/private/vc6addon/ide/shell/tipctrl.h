// TipCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTipCtrl window

class CTipCtrl : public CWnd
{
// Construction
public:
	CTipCtrl();

// Attributes
public:

// Operations
public:
	void SetTip( const CString & sTip );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTipCtrl)
public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTipCtrl();

protected:
	int m_margin;
	CString m_sDidya;
	CBitmap m_bitmap;
	CString m_sTip;
	CRect m_rectText;
	void CalcTextRect();

protected:
	// Generated message map functions
	//{{AFX_MSG(CTipCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
