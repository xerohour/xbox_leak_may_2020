///////////////////////////////////////////////////////////////////////////////
//	BRZCASES.CPP
//
//	Created by :			Date :
//		DougT					10/13/93
//
//	Description :
//		Implementation of the CBrzTestCases class
//

#include "stdafx.h"
#include "brzcases.h"

// for _stat CRT function
#include "sys\types.h"
#include "sys\stat.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CBrzTestCases, CTest, "Resource Browser Tests", -1, CSniffDriver)

void CBrzTestCases::Run(void)
{
	
	XSAFETY;
	BrzEnumRes();
	BrzCreateRCT();
	UIWB.CloseAllWindows();
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

BOOL CBrzTestCases::BrzEnumRes(void)
{
	CString strPropTxt;
	
	// Recycle the RC file from the IO_ test. Substitute a perhaps-larger
	// one later that has more interesting cases. But for now, just use
	// this one to come up with a generic Enumeration of Any algorithm
	// That way, all that has to be done is to change the name here.
	HWND hwnd = UIWB.OpenFile(m_strCWD + "io_open.rc");
	
	Sleep(400);
	MST.DoKeys("{DOWN}");  		// Accelerator resource type indicator
	Sleep(400);
	MST.DoKeys("{ENTER}");		// expose accelerator tables
	Sleep(400);
	
	MST.DoKeys("{DOWN}");	// first accelerator table resource
	strPropTxt = UIWB.GetProperty(3);
	if (strPropTxt != "IDR_ACCELERATOR1") {
		m_pLog->RecordFailure("strPropTxt != IDR_ACCELERATOR1");
	}
	
	Sleep(500);
	MST.DoKeys("{DOWN}");	// second accelerator table resource
	strPropTxt = UIWB.GetProperty(3);
	if (strPropTxt != "IDR_ACCELERATOR2") {
		m_pLog->RecordFailure("strPropTxt != IDR_ACCELERATOR2");
	}
	
	Sleep(500); 
	MST.DoKeys("{DOWN}");	// Dialog resource type indicator
	Sleep(400);
	MST.DoKeys("{ENTER}");		// expose dialog resources
	strPropTxt = UIWB.GetProperty(3);
	if (strPropTxt != "Empty Property") {
		m_pLog->RecordFailure("strPropTxt != Empty Property");
	}
	
	Sleep(1000);
	MST.DoKeys("{DOWN}");	// first (and only) dialog resource
	strPropTxt = UIWB.GetProperty(3);
	if (strPropTxt != "IDD_DIALOG1") {
		m_pLog->RecordFailure("strPropTxt != IDD_DIALOG1");
	}
	
	Sleep(400);
	MST.DoKeys("{DOWN}");	// Menu resource type indicator
	Sleep(400);
	MST.DoKeys("{ENTER}");		// expose menu resources
	strPropTxt = UIWB.GetProperty(3);
	if (strPropTxt != "Empty Property") {
		m_pLog->RecordFailure("strPropTxt != Empty Property");
	}
	
	Sleep(400);
	MST.DoKeys("{DOWN}");	// first menu resource
	strPropTxt = UIWB.GetProperty(3);
	if (strPropTxt != "IDR_MENU1") {
		m_pLog->RecordFailure("strPropTxt != IDR_MENU1");
	}
	
	Sleep(400);
	MST.DoKeys("{DOWN}");	// String Table resource type indicator (no symbols)
	strPropTxt = UIWB.GetProperty(3);
	if (strPropTxt != "Empty Property") {
		m_pLog->RecordFailure("strPropTxt != Empty Property");
	}
	
	Sleep(400);
	MST.DoKeys("{DOWN}");	// Version resource type indicator
	Sleep(400);
	MST.DoKeys("{ENTER}");		// expose version resource
	strPropTxt = UIWB.GetProperty(3);
	if (strPropTxt != "Empty Property") {
		m_pLog->RecordFailure("strPropTxt != Empty Property");
	}
	
	MST.DoKeys("{DOWN}");
	strPropTxt = UIWB.GetProperty(3);
	if (strPropTxt != "IDR_VERSION1") {
		m_pLog->RecordFailure("strPropTxt != IDR_VERSION1");
	}
	
	Sleep(400);
	MST.DoKeys("{ENTER}");		// close version resource display
	MST.DoKeys("{UP 3}");		// move past String Table and prepare to close menu list
	MST.DoKeys("{ENTER}");		// close menu resource display
	MST.DoKeys("{UP 2}");		// move past ID into Dialog
	MST.DoKeys("{ENTER}");		// close dialog resource display
	MST.DoKeys("{UP 3}");		// move past IDs into Accelerator
	MST.DoKeys("{ENTER}");		// close accelerator resource display
	MST.DoKeys("{UP}");			// move to RC hierarchy entry
	MST.DoKeys("{ENTER}");		// close all RC resource type displays

	return TRUE;  
}

BOOL CBrzTestCases::BrzCreateRCT(void)
{
	
	XSAFETY;

	UIWB.DoCommand(ID_FILE_NEW, DC_ACCEL);	
	UIDialog nd(0x64,GL_LISTBOX);
	if( !nd.WaitAttachActive(1000) )
	{
		LOG->RecordInfo("Could not open File/New dialog");
		return NULL;
	}

	MST.DoKeys("{DOWN 3}");
	nd.OK();
	nd.WaitUntilGone(1000);
	
	UIEditor ed = UIWB.GetActiveEditor();

	return TRUE;  
}
