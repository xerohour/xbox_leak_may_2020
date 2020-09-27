// DMCoreTest.h : main header file for the DMCORETEST application
//

#if !defined(AFX_DMCORETEST_H__9A7470E5_D2F8_11D1_819E_0060083316C1__INCLUDED_)
#define AFX_DMCORETEST_H__9A7470E5_D2F8_11D1_819E_0060083316C1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDMCoreTestApp:
// See DMCoreTest.cpp for the implementation of this class
//

class CDMCoreTestApp : public CWinApp
{
public:
	CDMCoreTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDMCoreTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDMCoreTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DMCORETEST_H__9A7470E5_D2F8_11D1_819E_0060083316C1__INCLUDED_)
