#if !defined(AFX_READONLYEDIT_H__A1D70182_B723_11D2_B0E3_00105A26620B__INCLUDED_)
#define AFX_READONLYEDIT_H__A1D70182_B723_11D2_B0E3_00105A26620B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// readonlyedit.h : header file
//

#include <afxwin.h>

/////////////////////////////////////////////////////////////////////////////
// CReadOnlyEdit window


class CReadOnlyEdit : public CEdit
{
// Construction
public:
	CReadOnlyEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReadOnlyEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CReadOnlyEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CReadOnlyEdit)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_READONLYEDIT_H__A1D70182_B723_11D2_B0E3_00105A26620B__INCLUDED_)
