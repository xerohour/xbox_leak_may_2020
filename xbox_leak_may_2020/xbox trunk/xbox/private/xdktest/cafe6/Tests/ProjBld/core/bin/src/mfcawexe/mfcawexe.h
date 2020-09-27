// mfcawexe.h : main header file for the MFCAWEXE application
//

#if !defined(AFX_MFCAWEXE_H__CAA16AA8_CAD7_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_MFCAWEXE_H__CAA16AA8_CAD7_11D0_84BF_00AA00C00848__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMfcawexeApp:
// See mfcawexe.cpp for the implementation of this class
//

class CMfcawexeApp : public CWinApp
{
public:
	CMfcawexeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcawexeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMfcawexeApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCAWEXE_H__CAA16AA8_CAD7_11D0_84BF_00AA00C00848__INCLUDED_)
