#if !defined(AFX_FILELISTBOX_H__C9DE6DD0_DAD1_4B19_848C_F23B44BA860F__INCLUDED_)
#define AFX_FILELISTBOX_H__C9DE6DD0_DAD1_4B19_848C_F23B44BA860F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileListBox window

class CFileListBox : public CListBox
{
// Construction
public:
	CFileListBox();

// Attributes
public:

// Operations
public:
	int IndexFromFile( void *pVoid );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileListBox)
	public:
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFileListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFileListBox)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILELISTBOX_H__C9DE6DD0_DAD1_4B19_848C_F23B44BA860F__INCLUDED_)
