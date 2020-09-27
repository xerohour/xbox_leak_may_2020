//-----------------------------------------------------------------------------
// Notify.h : main header file for the NOTIFY application
//

#if !defined(AFX_NOTIFY_H__25CB49C9_AB8D_11D0_9667_00A0249611DC__INCLUDED_)
#define AFX_NOTIFY_H__25CB49C9_AB8D_11D0_9667_00A0249611DC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <alerts.h>
#include <mmsystem.h>
//#include <mapi.h>

//-----------------------------------------------------------------------------
// INI File Constants

#if 0

#define iniMAILTARGETS		"Mail Targets"
#define iniFAILURE			"Failure"
#define iniPROGRESS			"Progress"
#define iniCOMPLETION		"Completion"

#define iniMAILSOURCE		"Mail Source"
#define iniFROM				"From"

#else

#define iniMAILTARGETS		"Sounds"
#define iniFAILURE			"Failure"
#define iniPROGRESS			"Progress"
#define iniCOMPLETION		"Completion"

#endif

//-----------------------------------------------------------------------------
// CNotifyApp:
// See Notify.cpp for the implementation of this class
//

class CNotifyApp : public CWinApp {

public:

			CNotifyApp		( );
			~CNotifyApp		( );
	
	//BOOL	SendMail		( void );
	//BOOL	InitMapi		( void );
	//BOOL	MapiInstalled	( void );

	//HINSTANCE hlibMAPI;
	//CString fromAddress;

	CString	m_sINIname;

	CString failureTarget;
	CString progressTarget;
	CString completionTarget;
	
	DWORD	alertFlags;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNotifyApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNotifyApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOTIFY_H__25CB49C9_AB8D_11D0_9667_00A0249611DC__INCLUDED_)
