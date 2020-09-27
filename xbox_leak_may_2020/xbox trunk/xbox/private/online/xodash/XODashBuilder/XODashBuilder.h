// XODashBuilder.h : main header file for the XODASHBUILDER application
//

#if !defined(AFX_XODASHBUILDER_H__AF6BEC48_71D1_4F0A_85CE_FEC9218741FA__INCLUDED_)
#define AFX_XODASHBUILDER_H__AF6BEC48_71D1_4F0A_85CE_FEC9218741FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CXODashBuilderApp:
// See XODashBuilder.cpp for the implementation of this class
//

class CXODashBuilderApp : public CWinApp
{
public:
	CXODashBuilderApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXODashBuilderApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CXODashBuilderApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XODASHBUILDER_H__AF6BEC48_71D1_4F0A_85CE_FEC9218741FA__INCLUDED_)
