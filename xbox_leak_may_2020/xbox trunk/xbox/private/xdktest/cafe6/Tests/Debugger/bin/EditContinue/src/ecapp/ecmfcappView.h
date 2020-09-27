// ecmfcappView.h : interface of the CEcmfcappView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECMFCAPPVIEW_H__49433FCF_FD1C_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_ECMFCAPPVIEW_H__49433FCF_FD1C_11D0_84BF_00AA00C00848__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CEcmfcappView : public CView
{
protected: // create from serialization only
	CEcmfcappView();
	DECLARE_DYNCREATE(CEcmfcappView)

// Attributes
public:
	CEcmfcappDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEcmfcappView)
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
	virtual ~CEcmfcappView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEcmfcappView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ecmfcappView.cpp
inline CEcmfcappDoc* CEcmfcappView::GetDocument()
   { return (CEcmfcappDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECMFCAPPVIEW_H__49433FCF_FD1C_11D0_84BF_00AA00C00848__INCLUDED_)
