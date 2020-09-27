#if !defined(EDIT_H__0F42D4C5_284D_11D0_8BFE_00A0C903A030__INCLUDED_)
#define EDIT_H__0F42D4C5_284D_11D0_8BFE_00A0C903A030__INCLUDED_

// Edit.h : main header file for the EDIT application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CEditApp:
// See Edit.cpp for the implementation of this class
//

class CEditApp : public CWinApp
{
public:
	CEditApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CEditApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(EDIT_H__0F42D4C5_284D_11D0_8BFE_00A0C903A030__INCLUDED)
