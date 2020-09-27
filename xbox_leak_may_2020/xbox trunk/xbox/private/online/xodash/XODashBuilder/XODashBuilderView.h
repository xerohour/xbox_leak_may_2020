// XODashBuilderView.h : interface of the CXODashBuilderView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_XODASHBUILDERVIEW_H__74AC8383_E147_4209_AFE5_FA7571318EF2__INCLUDED_)
#define AFX_XODASHBUILDERVIEW_H__74AC8383_E147_4209_AFE5_FA7571318EF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXODashBuilderCntrItem;

class CXODashBuilderView : public CView
{
protected: // create from serialization only
	CXODashBuilderView();
	DECLARE_DYNCREATE(CXODashBuilderView)

// Attributes
public:
	CXODashBuilderDoc* GetDocument();
	// m_pSelection holds the selection to the current CXODashBuilderCntrItem.
	// For many applications, such a member variable isn't adequate to
	//  represent a selection, such as a multiple selection or a selection
	//  of objects that are not CXODashBuilderCntrItem objects.  This selection
	//  mechanism is provided just to help you get started.

	// TODO: replace this selection mechanism with one appropriate to your app.
	CXODashBuilderCntrItem* m_pSelection;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXODashBuilderView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL IsSelected(const CObject* pDocItem) const;// Container support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXODashBuilderView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CXODashBuilderView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEditCntr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in XODashBuilderView.cpp
inline CXODashBuilderDoc* CXODashBuilderView::GetDocument()
   { return (CXODashBuilderDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XODASHBUILDERVIEW_H__74AC8383_E147_4209_AFE5_FA7571318EF2__INCLUDED_)
