// mfcawexeView.h : interface of the CMfcawexeView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCAWEXEVIEW_H__CAA16AB2_CAD7_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_MFCAWEXEVIEW_H__CAA16AB2_CAD7_11D0_84BF_00AA00C00848__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#define XXXXTEST

class CMfcawexeView : public CView
{
protected: // create from serialization only
	CMfcawexeView();
	DECLARE_DYNCREATE(CMfcawexeView)

// Attributes
public:
	CMfcawexeDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcawexeView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMfcawexeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMfcawexeView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in mfcawexeView.cpp
inline CMfcawexeDoc* CMfcawexeView::GetDocument()
   { return (CMfcawexeDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCAWEXEVIEW_H__CAA16AB2_CAD7_11D0_84BF_00AA00C00848__INCLUDED_)
