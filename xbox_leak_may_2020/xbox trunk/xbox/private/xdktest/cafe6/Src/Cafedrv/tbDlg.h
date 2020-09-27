///////////////////////////////////////////////////////////////////////////////
//	tbdlg.h
//
//	Created by :			Date :
//		BrianCr				5/24/95
//
//	Description :
//		Definition of the CToolbarsDlg class
//

#include "caferes.h"

/////////////////////////////////////////////////////////////////////////////
// CToolbarsDlg dialog

class CToolbarsDlg : public CDialog
{
// Construction
public:
	CToolbarsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CToolbarsDlg)
	enum { IDD = IDD_Toolbars };
	BOOL	m_bShowCAFEToolbar;
	BOOL	m_bShowStatusBar;
	BOOL	m_bShowSubSuiteToolbar;
	BOOL	m_bShowViewportToolbar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolbarsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CToolbarsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
