// dspbuilder.h : main header file for the DSPBUILDER application
//

#if !defined(AFX_DSPBUILDER_H__A4399BA9_E747_41EE_86C6_5881A0DA3E7E__INCLUDED_)
#define AFX_DSPBUILDER_H__A4399BA9_E747_41EE_86C6_5881A0DA3E7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDspbuilderApp:
// See dspbuilder.cpp for the implementation of this class
//

class CDspbuilderApp : public CWinApp
{
public:
	CDspbuilderApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDspbuilderApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CDspbuilderApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSPBUILDER_H__A4399BA9_E747_41EE_86C6_5881A0DA3E7E__INCLUDED_)
