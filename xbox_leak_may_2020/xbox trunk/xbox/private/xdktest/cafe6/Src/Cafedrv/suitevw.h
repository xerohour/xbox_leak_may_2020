/////////////////////////////////////////////////////////////////////////////
// suitevw.h
//
// email	date		change
// briancr	10/25/94	created
//
// copyright 1994 Microsoft

// Interface of the CSuiteView class

#ifndef __SUITEVW_H__
#define __SUITEVW_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "treectl.h"
#include "suitedoc.h"


/////////////////////////////////////////////////////////////////////////////
// CSuiteView view

class CSuiteView : public CView
{
protected:
	CSuiteView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSuiteView)

// Attributes
public:

// Operations
public:

// attributes (internal)
protected:
	#ifdef _DEBUG
		CSuiteDoc* GetDocument();
	#else
		inline CSuiteDoc* CSuiteView::GetDocument() { return (CSuiteDoc*)m_pDocument; }
	#endif


// operations (internal)
protected:
	void FillTree(void);
	BOOL UpdateSuite(void);

// data
protected:
	CTreeCtl* m_ptreeSuite;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSuiteView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSuiteView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CSuiteView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSubSuiteRun();
	afx_msg void OnSubSuiteAdd();
	afx_msg void OnSubSuiteDelete();
	afx_msg void OnSubSuiteSelectAll();
	afx_msg void OnSubSuiteDeselectAll();
	afx_msg void OnSubSuiteToggleAll();
	afx_msg void OnSubSuiteProperties();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnOptionsSuite();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __SUITEVW_H__
