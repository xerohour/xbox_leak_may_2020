/////////////////////////////////////////////////////////////////////////////
// portview.h
//
// email	date		change
// briancr	10/25/94	created
//
// copyright 1994 Microsoft

// Interface of the CViewportView class

#ifndef __PORTVIEW_H__
#define __PORTVIEW_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include "logview.h"
#include "portdoc.h"

///////////////////////////////////////////////////////////////////////////////
//	Syntax Coloring struct and enum

struct SyntaxColor
{
	LPCSTR szStart;
	LPCSTR szEnd;
	int scType;		// enum
	COLORREF color;
};

#define WM_UPDATE_FROM_PIPE WM_USER + 1

///////////////////////////////////////////////////////////////////////////////
//	CViewportView class

class CViewportView : public CView
{
protected: // create from serialization only
	CViewportView();
	DECLARE_DYNCREATE(CViewportView)

// Data
public:
	CFont m_font;
	int m_cxFont, m_cyFont;
	int m_cColsInView, m_cLinesInView;
	int m_rSel1, m_cSel1;	// row and column of beginning of selection
	int m_rSel2, m_cSel2;	// row and column of end of selection
#define NUM_TABS	10
	int m_aTabs[NUM_TABS];
	int m_cxTabInterval;
	int m_cxTabAdjust;
	CString m_strFind;
	CSize m_sizeFont;

private:
	BOOL selecting; //are we in selecting mode?
// Attributes
public:
	CViewportDoc* GetDocument();

/*
// Drawing Utilities
public:
	int ColorText(CDC* pDC, int x, int y, LPCSTR sz);
	int ColorTextLeft(CDC* pDC, int x, int y, CString& str, int left);
	int ColorTextColor(CDC* pDC, int x, int y, CString& str, int len, COLORREF rgb);
	int ColorTextRight(CDC* pDC, int x, int y, CString& str, int right);
	int TextOutShift(CDC* pDC, int x, int y, LPCSTR sz);
	int GetNextTab(int x);
	inline void AdjustTabs(int cx = 0)
		{	m_cxTabAdjust = cx; }

*/

// Implementation
public:
	virtual ~CViewportView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	// Need to call it from SuiteDoc.
	void SelectLine(int n);			 

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// operations (internal)
	BOOL DrawLine(CDC* pDC, int nLineX, int nLineY, COLORREF color, CString& strLine);

// Scrolling utilities
protected:
	void UpdateScrollbars(int hMax = -1, int hPos = -1, int vMax = -1, int vPos = -1);

// Printing support
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// data structures (internal)
protected:
	struct LineType {
		COLORREF color;
		CString strSetting;
		LineType(COLORREF c, CString str) : color(c), strSetting(str) { }
	};

// data
protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CViewportView)
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEditMark();
	afx_msg void OnUpdateEditMark(CCmdUI* pCmdUI);
	afx_msg void OnSelectAll();
	afx_msg void OnUpdateSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnViewportFilter(UINT nId);
	afx_msg void OnUpdateViewportFilter(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//	LRESULT OnAppendLine(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateFromPipe(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG	// debug version in portview.cpp
inline CViewportDoc* CViewportView::GetDocument()
   { return (CViewportDoc*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

#endif //__PORTVIEW_H__
