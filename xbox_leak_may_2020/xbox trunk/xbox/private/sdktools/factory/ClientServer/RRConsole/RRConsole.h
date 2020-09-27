// RRConsole.h : main header file for the RRCONSOLE application
//

#if !defined(AFX_RRCONSOLE_H__256848CF_3180_495B_A930_464E87FD03F8__INCLUDED_)
#define AFX_RRCONSOLE_H__256848CF_3180_495B_A930_464E87FD03F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#include "resource.h"		// main symbols
#include <init.h>
typedef LONG NTSTATUS; // for xconfig.h
#include <xconfig.h>
#include <cryptkeys.h>
#include <PerBoxData.h>
#include "CNetwork.h"

extern UINT CNetErrorIDs[];



/////////////////////////////////////////////////////////////////////////////
// CRRConsoleApp:
// See RRConsole.cpp for the implementation of this class
//

class CRRConsoleApp : public CWinApp
{
public:
	CRRConsoleApp();
	
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRRConsoleApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRRConsoleApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RRCONSOLE_H__256848CF_3180_495B_A930_464E87FD03F8__INCLUDED_)
