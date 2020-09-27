#if !defined(AFX_OTHERFILESLISTBOX_H__281511A2_871B_4722_A588_D7BA09DFA652__INCLUDED_)
#define AFX_OTHERFILESLISTBOX_H__281511A2_871B_4722_A588_D7BA09DFA652__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileListBox.h"

// OtherFilesListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COtherFilesListBox window

class COtherFilesListBox : public CFileListBox
{
// Construction
public:
	COtherFilesListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COtherFilesListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COtherFilesListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(COtherFilesListBox)
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTHERFILESLISTBOX_H__281511A2_871B_4722_A588_D7BA09DFA652__INCLUDED_)
