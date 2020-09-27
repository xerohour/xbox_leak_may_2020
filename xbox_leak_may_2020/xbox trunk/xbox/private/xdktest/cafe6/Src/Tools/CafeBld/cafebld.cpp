// cafebld.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "cafebld.h"
#include "cafedlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCAFEBuildApp

BEGIN_MESSAGE_MAP(CCAFEBuildApp, CWinApp)
	//{{AFX_MSG_MAP(CCAFEBuildApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCAFEBuildApp construction

CCAFEBuildApp::CCAFEBuildApp()
: m_strRegistryKey("CAFE Build System")
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCAFEBuildApp object

CCAFEBuildApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCAFEBuildApp initialization

BOOL CCAFEBuildApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	Enable3dControls();
//	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// parse the command line
	ParseCmdLine(m_lpCmdLine);

	CCAFEBuildDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CCAFEBuildApp::ParseCmdLine(LPCSTR pszCmdLine)
{
	CString strCmdLine = pszCmdLine;

	// eliminate any leading white space
	EliminateLeadingChars(strCmdLine, " \t");
	// iterate through all command line parameters
	while (!strCmdLine.IsEmpty()) {
		if (strCmdLine[0] == '-') {
			switch (strCmdLine[1]) {
				case 'k':
				case 'K': {
					strCmdLine = strCmdLine.Mid(2);
					m_strRegistryKey = GetToken(strCmdLine);
					break;
				}
				case '?': {
					strCmdLine = strCmdLine.Mid(2);
					Usage();
					break;
				}
				default: {
					Usage("Unrecognized command line switch '" + GetToken(strCmdLine) + "'.");
					break;
				}
			}
		}
		else {
			Usage("Invalid command line parameter '" + GetToken(strCmdLine) + "'.");
		}
		EliminateLeadingChars(strCmdLine, " \t");
	}
}

void CCAFEBuildApp::EliminateLeadingChars(CString &str, CString strSet)
{
    // the set string should not be empty
    ASSERT(!strSet.IsEmpty());

    while (str.FindOneOf(strSet) == 0) {
        str = str.Mid(1);
    }
}

// this function returns the next token in str (assuming it starts at the beginning of str
// and also changes str so that it points to the next character immediately following the token delimeter
CString CCAFEBuildApp::GetToken(CString &str)
{
	CString strToken;
	int nDelimeter;

	// find the end of the token (assume the token starts at the beginning of the string
	nDelimeter = str.FindOneOf(" \t");
	// if no delimeter found, the token is the entire string
	if (nDelimeter == -1) {
		strToken = str;
		str.Empty();
	}
	// the token goes from the beginning of the string to the position of the delimeter
	else {
		strToken = str.Left(nDelimeter);
		str = str.Mid(nDelimeter+1);
	}

	return strToken;
}

void CCAFEBuildApp::Usage(LPCSTR pszMessage /*= NULL*/)
{
	CString strUsage = "CAFE Build System Application.\nUsage: cafebld [-k<keyname>]\n-k: Specify the registry key to use.";

	if (pszMessage != NULL) {
		strUsage = CString(pszMessage) + "\n\n" + strUsage;
	}

	AfxMessageBox(strUsage);
	exit(1);
	::PostQuitMessage(1);
}
