///////////////////////////////////////////////////////////////////////////////
//      ParameterHelpCases.CPP
//
//      Description :
//              Implementation of CParameterHelpCases

#include "stdafx.h"
#include "ParameterHelpCases.h"

#define new DEBUG_NEW

#define MTT_TYPEPREFIX 0
#define MTT_TYPEPOSTFIX 1
#define MTT_NAME 2
#define MTT_DESCRIPTION 3

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CParameterHelpCases, CTest, "ParameterHelp", -1, CAutoCompleteSuite)

void CParameterHelpCases::Run(void)
{
	EditSource();
}

BOOL CParameterHelpCases::EditSource()
{
	BOOL	bReturn = TRUE;
	int		nCount = 0;
	int		nResult;
	LRESULT lLength;
	HWND	hWnd;
	TCHAR*	pText;
	CString strMethod = "Collate";		// Need to put this in a string resource
	CString strParam = "LPCTSTR lpsz";  // Need to put this in a string resource

	m_pLog->Comment("Open a source file.");

	// Open a source file
	nResult = src.Open(m_strCWD + "baseln\\AutoComp01\\"+"AutoComp01.cpp");

	if (nResult == ERROR_ERROR) {
		m_pLog->RecordFailure("Couldn't open source file AutoComp01.cpp");
		bReturn = FALSE;
	}
	else {
		// Move down the file to a place where 
		// we can enter some AutoComplete code.
		src.GoToLine(165);
		
		// Wait for Go To dialog to go away
		while ((MST.WFndWnd("Go To", FW_DEFAULT)) && (nCount < 1000)) {
			nCount++;
		}

		MST.DoKeys("{ENTER 2}");
		MST.DoKeys("{UP 2}");
		MST.DoKeys("CString str;{ENTER 2}");
		MST.DoKeys("str.");
		MST.DoKeys("Colla");
		MST.DoKeys("{TAB}");
		MST.DoKeys("{(}");

		hWnd = MST.WFndWndWaitC("", "VsTipWindow", FW_DEFAULT, 10);

		if (hWnd == 0) {
			m_pLog->RecordFailure("Couldn't find ParameterHelp window.");
			bReturn = FALSE;
		}
		else {
			if (MST.WIsVisible(hWnd)) {
				m_pLog->RecordSuccess("Found ParameterHelp window.");

				// Get the method name
				SendMessage(hWnd, WM_SETTITLETOMETHOD, MTT_NAME, 0);
				lLength = GetWindowTextLength(hWnd) + sizeof(TCHAR);
				pText = new TCHAR[lLength];
				GetWindowText(hWnd, pText, lLength);
			
				if (strcmp(pText, strMethod) == 0) {
					m_pLog->RecordSuccess("ParameterHelp matches the expected text.");
				}
				else {
					m_pLog->RecordFailure("ParameterHelp does not match expectation.\n\t\tParameterHelp = '%s'\n\t\tExpected '%s'", pText, strMethod);
				}

				delete [] pText;

				// Get the parameter
				SendMessage(hWnd, WM_SETTITLETOPARAM, 0, 0);
				lLength = GetWindowTextLength(hWnd) + sizeof(TCHAR);
				pText = new TCHAR[lLength];
				GetWindowText(hWnd, pText, lLength);
			
				if (strcmp(pText, strParam) == 0) {
					m_pLog->RecordSuccess("ParameterHelp matches the expected text.");
				}
				else {
					m_pLog->RecordFailure("ParameterHelp does not match expectation.\n\t\tParameterHelp = '%s'\n\t\tExpected '%s'", pText, strParam);
					bReturn = FALSE;
				}

				delete [] pText;
			}
			else {
				m_pLog->RecordFailure("ParameterHelp window is not visible.");
				bReturn = FALSE;
			}
		}

		// Dismiss the ParameterHelp window.
		MST.DoKeys("{ESC}");

		src.Close();
	}

	return(bReturn);
}
