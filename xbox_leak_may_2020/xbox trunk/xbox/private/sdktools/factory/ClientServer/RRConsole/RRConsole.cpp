// RRConsole.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "RRConsole.h"
#include "RRConsoleDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CRRConsoleApp

BEGIN_MESSAGE_MAP(CRRConsoleApp, CWinApp)
	//{{AFX_MSG_MAP(CRRConsoleApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRRConsoleApp construction

CRRConsoleApp::CRRConsoleApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRRConsoleApp object

CRRConsoleApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRRConsoleApp initialization

BOOL CRRConsoleApp::InitInstance()
{
	int nResponse;

	// Standard initialization

	
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	CRRConsoleDlg dlg;
	m_pMainWnd = &dlg;

	nResponse = dlg.DoModal();

	if(nResponse == IDCANCEL || nResponse == IDCLOSE)
	{
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
//		ExitWindows(0,0);  // log user off
		return FALSE;
	}
	return TRUE;
}
