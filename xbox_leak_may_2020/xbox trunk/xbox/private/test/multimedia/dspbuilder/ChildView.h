/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	ChildView.h

Abstract:

	Main window

Author:

	Robert Heitkamp (robheit) 03-May-2001

Revision History:

	03-May-2001 robheit
		Initial Version

--*/
#if !defined(AFX_CHILDVIEW_H__147C6C30_763C_4DA0_B48D_424E36301991__INCLUDED_)
#define AFX_CHILDVIEW_H__147C6C30_763C_4DA0_B48D_424E36301991__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Grid.h"

//------------------------------------------------------------------------------
//	CChildView
//------------------------------------------------------------------------------
class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildView();
	void EnableMenuItem(CMenu*	pPopupMenu, UINT nIndex);

	// Generated message map functions
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnFileSaveAsImage();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CGrid	m_grid;
	int		m_gridVSize;
	int		m_gridHSize;
	int		m_x;
	int		m_y;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__147C6C30_763C_4DA0_B48D_424E36301991__INCLUDED_)
