///////////////////////////////////////////////////////////////////////////////
//      TypeInfoCases.CPP
//
//      Description :
//              Implementation of CTypeInfoCases

#include "stdafx.h"
#include "TypeInfoCases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CTypeInfoCases, CTest, "TypeInfo", -1, CAutoCompleteSuite)

void CTypeInfoCases::Run(void)
{
	EditSource();
}

BOOL CTypeInfoCases::EditSource()
{
	BOOL	bReturn = TRUE;
	int		nCount = 0;
	int		nResult;
	LRESULT lLength;
	HWND	hWnd;
	char*	pText;
	CString strInfo = "void __cdecl AfxEnableControlContainer(COccManager *)";		// Need to put this in a string resource

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
		src.GoToLine(53);
		
		// Wait for Go To dialog to go away
		while ((MST.WFndWnd("Go To", FW_DEFAULT)) && (nCount < 1000)) {
			nCount++;
		}

		// Move the caret onto the call to AfxEnableControlContainer()
		MST.DoKeys("{RIGHT 6}");
		Sleep(2000);

		// Edit.Type Info menu choice
		//MST.DoKeys("%ey");
		MST.DoKeys("^t");
		Sleep(1000);
		
		// Ctrl+T (Type Info)
		//MST.DoKeys("^t");
		//Sleep(1000);

		hWnd = MST.WFndWndWaitC("", "VcTipWindow", FW_DEFAULT, 10);

		if (hWnd == 0) {
			m_pLog->RecordFailure("Couldn't find TypeInfo window.");
			bReturn = FALSE;
#ifdef _DEBUG
			Beep(440, 10);
			AfxMessageBox("Type Info failed!");
#endif
		}
		else {
			if (MST.WIsVisible(hWnd)) {
				m_pLog->RecordSuccess("Found TypeInfo window.");

				// Tell the IDE to put the text in the window title
				SendMessage(hWnd, WM_SETTITLETOTEXT, 0, 0);
			
				// Get the window title
				lLength = GetWindowTextLength(hWnd) + sizeof(TCHAR);
				pText = new TCHAR[lLength];
				GetWindowText(hWnd, pText, lLength);
			
				if (strcmp(pText, strInfo) == 0) {
					m_pLog->RecordSuccess("The TypeInfo matches the expected text.");
				}
				else {
					m_pLog->RecordFailure("TypeInfo does not match expectation.\n\t\tTypeInfo = '%s'\n\t\tExpected '%s'", pText, strInfo);
					bReturn = FALSE;
				}

				delete [] pText;
			}
			else {
				m_pLog->RecordFailure("TypeInfo window is not visible.");
				bReturn = FALSE;
			}
		}

		// Dismiss the TypeInfo window
		MST.DoKeys("{ESC}");

		src.Close();
	}

	return(bReturn);
}