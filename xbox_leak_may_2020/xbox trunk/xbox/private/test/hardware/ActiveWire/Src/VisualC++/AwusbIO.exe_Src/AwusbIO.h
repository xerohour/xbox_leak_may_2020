// AwusbIO.h : main header file for the AWUSBIO application
//

#if !defined(AFX_AWUSBIO_H__7802B6A8_0C7F_11D3_AC25_00104B306BEE__INCLUDED_)
#define AFX_AWUSBIO_H__7802B6A8_0C7F_11D3_AC25_00104B306BEE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAwusbIOApp:
// See AwusbIO.cpp for the implementation of this class
//

class CAwusbIOApp : public CWinApp
{
public:
	CAwusbIOApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAwusbIOApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAwusbIOApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AWUSBIO_H__7802B6A8_0C7F_11D3_AC25_00104B306BEE__INCLUDED_)
