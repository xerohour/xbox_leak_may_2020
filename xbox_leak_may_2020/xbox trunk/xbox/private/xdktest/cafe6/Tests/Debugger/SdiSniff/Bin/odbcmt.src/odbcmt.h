// ODBCMT.h : main header file for the ODBCMT application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CODBCMTApp:
// See ODBCMT.cpp for the implementation of this class
//

class CODBCMTApp : public CWinApp
{
public:
	CODBCMTApp();
	int m_commandrun;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CODBCMTApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CODBCMTApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
