// mfcawex3.h : main header file for the MFCAWEX3 application
//

#if !defined(AFX_MFCAWEX3_H__1DEE14E9_CC4B_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_MFCAWEX3_H__1DEE14E9_CC4B_11D0_84BF_00AA00C00848__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#define TESTTEST
#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMfcawex3App:
// See mfcawex3.cpp for the implementation of this class
//

class CMfcawex3App : public CWinApp
{
public:
	CMfcawex3App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcawex3App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMfcawex3App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCAWEX3_H__1DEE14E9_CC4B_11D0_84BF_00AA00C00848__INCLUDED_)
