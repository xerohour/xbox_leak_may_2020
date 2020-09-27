// mfcawex2.h : main header file for the MFCAWEX2 application
//

#if !defined(AFX_MFCAWEX2_H__1DEE14AB_CC4B_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_MFCAWEX2_H__1DEE14AB_CC4B_11D0_84BF_00AA00C00848__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMfcawex2App:
// See mfcawex2.cpp for the implementation of this class
//

class CMfcawex2App : public CWinApp
{
public:
	CMfcawex2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcawex2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	COleTemplateServer m_server;
		// Server object for document creation

	//{{AFX_MSG(CMfcawex2App)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCAWEX2_H__1DEE14AB_CC4B_11D0_84BF_00AA00C00848__INCLUDED_)
