#if !defined(JAZZVIEW_H__B6AED012_3BDF_11D0_89AC_00A0C9054129__INCLUDED_)
#define JAZZVIEW_H__B6AED012_3BDF_11D0_89AC_00A0C9054129__INCLUDED_

// JazzView.h : interface of the CJazzView class
//
/////////////////////////////////////////////////////////////////////////////

class CJazzCntrItem;

class CJazzView : public CView
{
protected: // create from serialization only
	CJazzView();
	DECLARE_DYNCREATE(CJazzView)

// Attributes
public:
	CJazzDoc* GetDocument();
	// m_pSelection holds the selection to the current CJazzCntrItem.
	// For many applications, such a member variable isn't adequate to
	//  represent a selection, such as a multiple selection or a selection
	//  of objects that are not CJazzCntrItem objects.  This selection
	//  mechanism is provided just to help you get started.

	// TODO: replace this selection mechanism with one appropriate to your app.
	CJazzCntrItem*  m_pSelection;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJazzView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL IsSelected(const CObject* pDocItem) const;// Container support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CJazzView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CJazzView)
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEditCntr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in JazzView.cpp
inline CJazzDoc* CJazzView::GetDocument()
   { return (CJazzDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(JAZZVIEW_H__B6AED012_3BDF_11D0_89AC_00A0C9054129__INCLUDED)
