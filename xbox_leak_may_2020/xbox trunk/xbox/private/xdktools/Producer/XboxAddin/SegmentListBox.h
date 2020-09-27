#if !defined(AFX_SEGMENTLISTBOX_H__12C270B9_4438_4283_908C_CB5BFADD59CB__INCLUDED_)
#define AFX_SEGMENTLISTBOX_H__12C270B9_4438_4283_908C_CB5BFADD59CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileListBox.h"

// SegmentListBox.h : header file
//

class CSegment;

/////////////////////////////////////////////////////////////////////////////
// CSegmentListBox window

class CSegmentListBox : public CFileListBox
{
// Construction
public:
	CSegmentListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSegmentListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSegmentListBox();

protected:
    HICON m_hPlayIcon;
    HICON m_hStopIcon;
    HICON m_hTransitionIcon;

	CSegment	*m_pClickedSegment;
	RECT		m_rectClicked;

	// Generated message map functions
protected:
	//{{AFX_MSG(CSegmentListBox)
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEGMENTLISTBOX_H__12C270B9_4438_4283_908C_CB5BFADD59CB__INCLUDED_)
