// ODBCMTView.h : interface of the CODBCMTView class
//
/////////////////////////////////////////////////////////////////////////////

class CODBCMTView : public CScrollView
{
protected: // create from serialization only
	CODBCMTView();
	DECLARE_DYNCREATE(CODBCMTView)

// Attributes
public:
	CODBCMTDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CODBCMTView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CODBCMTView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	LRESULT OnExecuteDone(WPARAM wParam, LPARAM lParam);

	//{{AFX_MSG(CODBCMTView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ODBCMTView.cpp
inline CODBCMTDoc* CODBCMTView::GetDocument()
   { return (CODBCMTDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
