#if !defined(AFX_AUDIOPATHLISTBOX_H__6FBD1C60_49BE_459B_9B81_0B0065D6169B__INCLUDED_)
#define AFX_AUDIOPATHLISTBOX_H__6FBD1C60_49BE_459B_9B81_0B0065D6169B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileListBox.h"

// AudiopathListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAudiopathListBox window

class CAudiopathListBox : public CFileListBox
{
// Construction
public:
	CAudiopathListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudiopathListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAudiopathListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAudiopathListBox)
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOPATHLISTBOX_H__6FBD1C60_49BE_459B_9B81_0B0065D6169B__INCLUDED_)
