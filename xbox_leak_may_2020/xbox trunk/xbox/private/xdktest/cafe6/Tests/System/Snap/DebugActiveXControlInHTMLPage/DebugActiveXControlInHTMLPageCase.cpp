///////////////////////////////////////////////////////////////////////////////
//	DebugActiveXControlInHTMLPageCase.CPP
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Implementation of the CDebugActiveXControlInHTMLPageCase class

#include "stdafx.h"
#include "DebugActiveXControlInHTMLPageCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CDebugActiveXControlInHTMLPageCase, CSystemTestSet, "Debug ActiveX Control In HTML Page", -1, CDebugActiveXControlInHTMLPageSubsuite)

void CDebugActiveXControlInHTMLPageCase::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void CDebugActiveXControlInHTMLPageCase::Run(void)
{	 	
	// need to do this because each subsuite has its own COProject object.
	prj.Attach();

	// select the ActiveX Control project so we can debug it.
	prj.SetTarget((CString)m_strActiveXControlProjName + " - " + 
					GetLocString(IDSS_PLATFORM_WIN32) + " " +
					GetLocString(IDSS_BUILD_DEBUG));

	// set the executable for debug session for the ActiveX control project to the default browser.
	LOG->RecordCompare(prj.SetExeForDebugSession(NULL, m_strActiveXControlProjName, DEFAULT_DBG_EXE_BROWSER) == ERROR_SUCCESS, 
		"Set executable for the ActiveX control's debug session to the default browser.");

	// set the program arguements for the ActiveX control project to the html page.

	LOG->RecordCompare(prj.SetProgramArguments(GetCWD() + m_strHTMLPageRelPath, m_strActiveXControlProjName) == ERROR_SUCCESS, 
		"Set program arguments for ActiveX control's debug session to the HTML page.");

	// set a bp in the OnDraw function of the ActiveX control.
	CString strOnDraw = m_strActiveXControlViewClass + "::OnDraw";
	LOG->RecordCompare(bp.SetBreakpoint(strOnDraw) != NULL,
		"Set a breakpoint in the OnDraw function of the ActiveX control.");

	// hit the bp.
	LOG->RecordCompare(dbg.Go(strOnDraw), "Hit the bp in the OnDraw function of the ActiveX control.");
	// stop debugging.
	LOG->RecordCompare(dbg.StopDebugging(), "Stop debugging the ActiveX control.");
}
