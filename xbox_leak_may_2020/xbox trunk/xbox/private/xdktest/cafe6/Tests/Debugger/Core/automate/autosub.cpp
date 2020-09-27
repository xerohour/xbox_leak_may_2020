///////////////////////////////////////////////////////////////////////////////
//	autosub.cpp
//
//	Created by : MichMa		Date: 9-17-96			
//		VCBU QA		
//
//	Description :
//		implementation of the CAutomationSubSuite class
//

#include "stdafx.h"
#include "autosub.h"
#include "afxdllx.h"
#include "..\cleanup.h"

#include "autodbg.h"
#include "autobps.h"
#include "autobp.h"
#include "autorec.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CAutomationSubSuite

IMPLEMENT_SUBSUITE(CAutomationSubSuite, CIDESubSuite, "Automation", "VCQA Debugger")

BEGIN_TESTLIST(CAutomationSubSuite)
	TEST(CDebuggerAutomationIDETest, RUN)
	TEST(CBreakpointsAutomationIDETest, RUN)
	//TEST(CBreakpointAutomationIDETest, RUN)
	//TEST(CRecordingAutomationIDETest, RUN)
END_TESTLIST()


void CAutomationSubSuite::CleanUp(void)
	{
	fProjectReady = FALSE;
	::CleanUp();
	}


void CAutomationSubSuite::LoadMacroFile(void)
	{
	// load macro file
	MST.DoKeys("%tc");
	MST.WFndWndWait("Customize", FW_DEFAULT, 10);
	MST.DoKeys("^({PGDN})");
	MST.DoKeys("^({PGDN})");
	MST.DoKeys("^({PGDN})");
	MST.DoKeys("^({PGDN})");
	MST.WFndWndWaitC("Browse...", "BUTTON", FW_DEFAULT, 10);
	MST.WButtonClick("Browse...");													  
	MST.WFndWndWait("Browse for Macro File or add-in", FW_DEFAULT, 10);
	MST.WEditSetText("@1", "automate.dsm");
	MST.DoKeys("{ENTER}");
	MST.WFndWndWaitC("OK", "BUTTON", FW_DEFAULT, 10);
	MST.WButtonClick("OK");		// in case macro is already loaded
	MST.WButtonClick("Close");
	}


void CAutomationSubSuite::RunMacro(LPCSTR macro)
	{
	MST.DoKeys("%tm");
	MST.WFndWndWaitC("OK", "BUTTON", FW_DEFAULT, 3);
	MST.WButtonClick("OK");		// in case macro file needs to be reloaded
	MST.WFndWndWait("Macro", FW_DEFAULT, 10);
	MST.WComboItemClk("@2", "automate");
	MST.DoKeys("%a");
	MST.DoKeys(macro);
	MST.WButtonClick("Run");

	// macro will throw user exception when done
	if(!MST.WFndWndWaitC("MACRO HAS FINISHED", "Static", FW_PART, 180))
		m_pLog->RecordFailure("THE MACRO DID NOT FINISH WITHIN 3 MINUTES");
		
	MST.WButtonClick("No");
	}


void CAutomationSubSuite::WriteOutputWindowToLog(void)
	{
	UIOutput OutputWnd;
	OutputWnd.Activate();
	MST.DoKeys("^({PGDN})");
	MST.DoKeys("^({PGDN})");
	MST.DoKeys("^({PGDN})");
	MST.DoKeys("^({HOME})");
	MST.DoKeys("+(^({END}))");
	MST.DoKeys("^(c)");
	CString cstrOutputWnd;
	GetClipText(cstrOutputWnd);
	
	if(cstrOutputWnd.Find("FAIL") != -1)
		m_pLog->RecordFailure("THERE IS A FAILURE IN THE FOLLOWING OUTPUT");
	
	m_pLog->RecordInfo("\n%s", cstrOutputWnd);
	}


void CAutomationSubSuite::CloseWorkspaceAndCreateOptionsFile(void)
	{
	// TODO(michma): we need to create a workspace options file for automate
	// via cafe because the vbs project object doesn't support such creation.
	// however, cafe doesn't have a close function that supports this so we
	// need to use ms-test.
	MST.DoKeys("%fk");
	MST.WFndWndWaitC("Yes", "BUTTON", FW_DEFAULT, 3);
	MST.WButtonClick("Yes");
	}

