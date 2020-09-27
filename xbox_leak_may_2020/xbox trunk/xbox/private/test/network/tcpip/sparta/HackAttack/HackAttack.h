// HackAttack.h : main header file for the HACKATTACK application
//

#if !defined(AFX_HACKATTACK_H__1A56E991_69C7_406A_89F0_6B71257424E2__INCLUDED_)
#define AFX_HACKATTACK_H__1A56E991_69C7_406A_89F0_6B71257424E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHackAttackApp:
// See HackAttack.cpp for the implementation of this class
//

class CHackAttackApp : public CWinApp
{
public:
	CHackAttackApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHackAttackApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CHackAttackApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HACKATTACK_H__1A56E991_69C7_406A_89F0_6B71257424E2__INCLUDED_)
