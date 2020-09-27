#ifndef __PROPERTYBAR_H__
#define __PROPERTYBAR_H__

// PropertyBar.h : header file
//

#include "PropertySheetCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyBar control bar

class CPropertyBar : public CDialogBar
{
// Construction
public:
	CPropertyBar();
	virtual ~CPropertyBar();

public:
	CPropertySheetCtrl* m_pPropertySheet;

//Operations
public:

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyBar)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:

// Generated message map functions
	//{{AFX_MSG(CPropertyBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __PROPERTYBAR_H__
