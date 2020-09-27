// apiwiz.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "apiwiz.h"
#include "apiwidlg.h"
#include "apipage.h"
#include "custpage.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

TEXTMETRIC tm ;
int cxChar, cxCaps, cyChar ;

// Pens and Brushes we need to show selections.
CBrush brSelRect ;
CBrush brUnSelRect ;
CPen   penUnSelRect ;
CPen   penSelRect ;
CBitmap bmapUnCheck, bmapCheck, bmapNone, bmapTriangle;							 


/////////////////////////////////////////////////////////////////////////////
// CApiwizApp

BEGIN_MESSAGE_MAP(CApiwizApp, CWinApp)
	//{{AFX_MSG_MAP(CApiwizApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApiwizApp construction

CApiwizApp::CApiwizApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CApiwizApp object

CApiwizApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CApiwizApp initialization

BOOL CApiwizApp::InitDrawingObjects() 
{
	static int FirstTime = 1 ;
	// Only do this stuff once.
	
/*	CClientDC ccldc(this) ;         
    ccldc.GetTextMetrics(&tm);
    cxChar = tm.tmAveCharWidth ;
    cxCaps = (tm.tmPitchAndFamily &1 ? 3 : 2) * cxChar/ 2 ;
    cyChar = tm.tmHeight + tm.tmExternalLeading ;
*/  
	// Create the brushes we need.
	// Only do this stuff once.
	if(FirstTime)
	{ 
		brSelRect.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT)) ;
	 	brUnSelRect.CreateSolidBrush(GetSysColor(COLOR_WINDOW)) ; 
	   	penUnSelRect.CreatePen(PS_SOLID,2,GetSysColor(COLOR_WINDOW)) ;
	   	penSelRect.CreatePen(PS_SOLID,1,GetSysColor(COLOR_WINDOWFRAME)) ; 

		bmapUnCheck.LoadBitmap(IDB_UNCHECK) ;
		bmapCheck.LoadBitmap(IDB_MYCHECK);
		bmapNone.LoadBitmap(IDB_BLANKBITMAP);
		bmapTriangle.LoadBitmap(IDB_TRIANGLE);

		FirstTime = 0 ;
	}

	return TRUE; 
}


BOOL CApiwizApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	Enable3dControls();
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Construct API Wizard Tabbed Dialog

	CCustomizePage CustomizePage ;
	CAPIPage	ApiPage ;

	CApiwizDlg APITabDlg("CAFE Browser") ;
	m_pMainWnd = &APITabDlg;

	APITabDlg.AddPage(&ApiPage) ;
	APITabDlg.AddPage(&CustomizePage) ;

	InitDrawingObjects() ;// Sets up the pens, brushes, and loads the bitmaps.

	if(APITabDlg.DoModal() == IDOK)
	{

		//Cleanup all the allocated memory.
	}
	else 
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
