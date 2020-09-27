///////////////////////////////////////////////////////////////////////////////
//	INTCASES.CPP
//
//	Created by :			Date :
//		DougT					8/13/93
//
//	Description :
//		Implementation of the CIntTestCases class
//

#include "stdafx.h"
#include "intcases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CIntTestCases, CTest, "Integration Tests", -1, CSniffDriver)

void CIntTestCases::Run(void)
{
	KillFile(m_strCWD, "INT*.APS");
	
	IntOpenErrorLadenRC();
	UIWB.CloseAllWindows();
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

BOOL CIntTestCases::IntOpenErrorLadenRC(void)
{
	HWND hwnd = UIWB.OpenFile(m_strCWD + "intopen.rc");

// REVIEW(davidga): Bug #535 used to assert here
	
	UIDialog rcerr(VRES_IDC_EDITCODE, GL_BUTTON);
	if( !rcerr.WaitAttachActive(3000) )
	{
		m_pLog->RecordFailure("(1) Error-laden RC did not cause error message");
		return FALSE;
	}
	else if( rcerr.GetDlgItem(VRES_IDC_EDITCODE) == NULL )
	{
		m_pLog->RecordFailure("(1) Expected 'Edit Code' button not found");
		return FALSE;
	}

	MST.WButtonClick(GetLabel(VRES_IDC_EDITCODE));

	hwnd = UIWB.GetActiveEditor();
	
	CString str = GetText(hwnd);
	str.MakeUpper();

	if(str.Find("INTOPEN.RC") < 0)		// if INTOPEN.RC not anywhere in titlebar
	{							 
		m_pLog->RecordFailure("(1) Resource script code window: Found %s active.", (LPCSTR)str);
		return FALSE;
	}
	return TRUE;
}
