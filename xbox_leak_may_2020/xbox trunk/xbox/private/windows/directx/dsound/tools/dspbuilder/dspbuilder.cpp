/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	dspbuilder.cpp

Abstract:

	dspbuilder app class

Author:

	Robert Heitkamp (robheit) 10-Jan-2002

Revision History:

	10-Jan-2002 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes
//------------------------------------------------------------------------------
#include "dspbuilder.h"
#include "MainFrm.h"
#include "xboxverp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDspbuilderApp

BEGIN_MESSAGE_MAP(CDspbuilderApp, CWinApp)
	//{{AFX_MSG_MAP(CDspbuilderApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDspbuilderApp construction

CDspbuilderApp::CDspbuilderApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDspbuilderApp object

static CDspbuilderApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDspbuilderApp initialization

BOOL CDspbuilderApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Microsoft"));

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.
	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW, NULL, NULL);

	// Resize the window to look nice
	RECT rect;
	pFrame->GetWindowRect(&rect);
	pFrame->MoveWindow(rect.left, rect.top, 800, 600);

	// The one and only window has been initialized, so show and update it.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDspbuilderApp message handlers





/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(void);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

//------------------------------------------------------------------------------
//	CAboutDlg::OnInitDialog
//------------------------------------------------------------------------------
BOOL 
CAboutDlg::OnInitDialog(void)
/*++

Routine Description:

	Initializes the dialog by setting the text

Arguments:

	None

Return Value:

	TRUE on success, FALSE on failure

--*/
{
	CStatic*	pStatic;
	CString		string;

	if(CDialog::OnInitDialog())
	{
		string.Format(_T("dspbuilder Version 1.4.%4d"), VER_PRODUCTBUILD);
		pStatic = (CStatic*)GetDlgItem(IDC_ABOUT_VERSION);
		pStatic->SetWindowText(string);

		string.Format(_T("Copyright (C) 2001-2002 %s"), _T(VER_COMPANYNAME_STR));
		pStatic = (CStatic*)GetDlgItem(IDC_ABOUT_COPYRIGHT);
		pStatic->SetWindowText(string);
		return TRUE;
	}

	return FALSE;
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CDspbuilderApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CDspbuilderApp message handlers

