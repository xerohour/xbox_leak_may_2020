// mfcawex2View.h : interface of the CMfcawex2View class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCAWEX2VIEW_H__1DEE14B6_CC4B_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_MFCAWEX2VIEW_H__1DEE14B6_CC4B_11D0_84BF_00AA00C00848__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CMfcawex2CntrItem;

class CMfcawex2View : public CView
{
protected: // create from serialization only
	CMfcawex2View();
	DECLARE_DYNCREATE(CMfcawex2View)

// Attributes
public:
	CMfcawex2Doc* GetDocument();
	// m_pSelection holds the selection to the current CMfcawex2CntrItem.
	// For many applications, such a member variable isn't adequate to
	//  represent a selection, such as a multiple selection or a selection
	//  of objects that are not CMfcawex2CntrItem objects.  This selection
	//  mechanism is provided just to help you get started.

	// TODO: replace this selection mechanism with one appropriate to your app.
	CMfcawex2CntrItem* m_pSelection;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcawex2View)
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
	virtual ~CMfcawex2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMfcawex2View)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEditCntr();
	afx_msg void OnCancelEditSrvr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in mfcawex2View.cpp
inline CMfcawex2Doc* CMfcawex2View::GetDocument()
   { return (CMfcawex2Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCAWEX2VIEW_H__1DEE14B6_CC4B_11D0_84BF_00AA00C00848__INCLUDED_)
