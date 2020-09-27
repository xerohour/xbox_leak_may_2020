///////////////////////////////////////////////////////////////////////////////
//      CodeCommentsCases.CPP
//
//      Description :
//              Implementation of CCodeCommentsCases

#include "stdafx.h"
#include "CodeCommentsCases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CCodeCommentsCases, CTest, "CodeComments", -1, CAutoCompleteSuite)

void CCodeCommentsCases::Run(void)
{
	EditSource();
}

BOOL CCodeCommentsCases::EditSource()
{
	BOOL	bReturn = TRUE;
	int		nCount = 0;
	int		nResult;
	LRESULT	lLength;
	HWND	hWnd;
	TCHAR*	pText;
	CString strComment = "Nonsense function for testing purposes";		// Need to put this in a string resource

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
		src.GoToLine(167);
		
		// Wait for Go To dialog to go away
		while ((MST.WFndWnd("Go To", FW_DEFAULT)) && (nCount < 1000)) {
			nCount++;
		}

		MST.DoKeys("{ENTER}");
		MST.DoKeys("{UP}");
		MST.DoKeys("{TAB}");
		MST.DoKeys("aboutDlg.");
		Sleep(1000);
		MST.DoKeys("fo");

		hWnd = MST.WFndWndWaitC("", "VcTipWindow", FW_DEFAULT, 10);

		if (hWnd == 0) {
			m_pLog->RecordFailure("Couldn't find CodeComments window.");
			bReturn = FALSE;
		}
		else {
			if (MST.WIsVisible(hWnd)) {
				m_pLog->RecordSuccess("Found CodeComments window.");

				// Tell the IDE to put the text in the window title
				SendMessage(hWnd, WM_SETTITLETOTEXT, 0, 0);
			
				// Get the window title
				lLength = GetWindowTextLength(hWnd) + sizeof(TCHAR);
				pText = new TCHAR[lLength];
				GetWindowText(hWnd, pText, lLength);
			
				if (strcmp(pText, strComment) == 0) {
					m_pLog->RecordSuccess("The comment matches the expected text.");
				}
				else {
					m_pLog->RecordFailure("Comment does not match expectation.\n\t\tComment = '%s'\n\t\tExpected '%s'", pText, strComment);
					bReturn = FALSE;
				}

				delete [] pText;
			}
			else {
				m_pLog->RecordFailure("CodeComments window is not visible.");
				bReturn = FALSE;
			}
		}

		// Dismiss the AutoComplete namelist
		MST.DoKeys("{ESC}");

		src.Close();
	}

	return(bReturn);
}