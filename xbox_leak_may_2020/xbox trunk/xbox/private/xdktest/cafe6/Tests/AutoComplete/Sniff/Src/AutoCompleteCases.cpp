///////////////////////////////////////////////////////////////////////////////
//      AutoCompleteCases.CPP
//
//      Description :
//              Implementation of CAutoCompleteCases

#include "stdafx.h"
#include "AutoCompleteCases.h"
#include <afxpriv.h>

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CAutoCompleteCases, CTest, "AutoComplete", -1, CAutoCompleteSuite)

void CAutoCompleteCases::Run(void)
{
	EditSource();
}

BOOL CAutoCompleteCases::EditSource()
{
	BOOL		bReturn = TRUE;
	int			nCount = 0;
	int			nResult;
	HWND		hWnd;
	LRESULT		lIndex;
	LRESULT		lLength;
	TCHAR*		pItem;
	COClipboard clp;
	CString		strClip;
	CString		strRoot = _T("pMainFrame->");
	CString		strFunc = _T("PreCreateWindow");

	m_pLog->Comment("Open a source file.");

	// Open a source file
	nResult = src.Open(m_strCWD + "baseln\\AutoComp01\\"+"AutoComp01.cpp");

	if (nResult == ERROR_ERROR) {
		m_pLog->RecordFailure("Couldn't open source file AutoComp01.cpp");
		bReturn = FALSE;
	}
	else {
	
		// Move down the file to a place where we can enter
		// some AutoComplete code.
		src.GoToLine(101);
	
		// Wait for Go To dialog to go away
		while ((MST.WFndWnd("Go To", FW_DEFAULT)) && (nCount < 1000)) {
			nCount++;
		}

		MST.DoKeys("{TAB}");

		// Invoke AutoComplete
		MST.DoKeys(strRoot);

		hWnd = MST.WFndWndWaitC("", TITLE_AUTOCOMPLETE, FW_DEFAULT, 10);

		if (hWnd == 0) {
			m_pLog->RecordFailure("Couldn't find namelist for AutoComplete on pMainFrame->.");
		}
		else {
			if (MST.WIsVisible(hWnd)) {
				m_pLog->RecordSuccess("Found namelist for AutoComplete on pMainFrame->.");
			}
			else {
				m_pLog->RecordFailure("Namelist is not visible for AutoComplete on pMainFrame->");
			}
		}

		// Type in a partial function name and wait 
		// for the namelist to settle on the choice 
		// we're after (PreCreateWindow)
		MST.DoKeys("PreCreateW");

	// CHECK FOR THE CORRECT NAMELIST CHOICE

		// Get the index of the current selection in the list
		lIndex = SendMessage(hWnd, WM_GETLISTSELECTION, 0, 0);

		if (lIndex >= 0 ) {
			// Tell the IDE to put the selection in the window title
			SendMessage(hWnd, WM_SETTITLETOITEM, lIndex, 0);
			
			// Get the window title
			lLength = GetWindowTextLength(hWnd) + sizeof(TCHAR);
			pItem = new TCHAR[lLength];
			GetWindowText(hWnd, pItem, lLength);
			
			// Check that the selection in the 
			// namelist is the text we expect
			if (strcmp(pItem, strFunc) == 0) {
				m_pLog->RecordSuccess("The correct item was selected in the namelist.");
			}
			else {
				m_pLog->RecordFailure("Item selected does not match expectation.\n\t\tNamelist item = '%s'\n\t\tExpected '%s'", pItem, strFunc);
			}

			delete [] pItem;

			// Select the item in the namelist
			MST.DoKeys("{TAB}");

			// Select the line that was inserted into
			// the editor and copy it to the clipboard
			//MST.DoKeys("{UP}{HOME}");

			// Wait a little to paste the text
			Sleep(1000);
			MST.DoKeys("+{HOME}");
			MST.DoKeys("^c");

			strClip = clp.GetText();

			// Check that the namelist put the 
			// text we expect in the editor
			if (strClip == (strRoot + strFunc)) {
				m_pLog->RecordSuccess("Correct code was inserted by AutoComplete.");
				bReturn = TRUE;
			}
			else {
				m_pLog->RecordFailure("Text inserted into editor by AutoComplete does not match expectation.\n\t\tEditor string = '%s'\n\t\tExpected '%s'", strClip, (strRoot + strFunc));
				bReturn = FALSE;
			}
		}
		else {
			m_pLog->RecordFailure("Couldn't get the index of the item selected in the namelist.");
			bReturn = FALSE;
		}

		src.Close();
	}

	return(bReturn);
}