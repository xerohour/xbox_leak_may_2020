// mfcawdll.h : main header file for the MFCAWDLL DLL
//

#if !defined(AFX_MFCAWDLL_H__7EFBEBEF_CAA8_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_MFCAWDLL_H__7EFBEBEF_CAA8_11D0_84BF_00AA00C00848__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#define MFCDLLXXX
#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMfcawdllApp
// See mfcawdll.cpp for the implementation of this class
//

class CMfcawdllApp : public CWinApp
{
public:
	CMfcawdllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcawdllApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMfcawdllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCAWDLL_H__7EFBEBEF_CAA8_11D0_84BF_00AA00C00848__INCLUDED_)
