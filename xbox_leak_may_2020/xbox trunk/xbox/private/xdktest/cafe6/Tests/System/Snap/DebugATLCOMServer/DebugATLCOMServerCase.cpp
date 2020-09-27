///////////////////////////////////////////////////////////////////////////////
//	DebugATLCOMServerCase.CPP
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Implementation of the CDebugATLCOMServerCase class

#include "stdafx.h"
#include "DebugATLCOMServerCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CDebugATLCOMServerCase, CSystemTestSet, "Debug ATL COM Server", -1, CDebugATLCOMServerSubsuite)

void CDebugATLCOMServerCase::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void CDebugATLCOMServerCase::Run(void)

{
	// need to do this because each subsuite has its own COProject object.
	prj.Attach();

	// select the AppWiz App project so we can debug it.
	prj.SetTarget((CString)m_strAppProjName + " - " + 
					GetLocString(IDSS_PLATFORM_WIN32) + " " +
					GetLocString(IDSS_BUILD_DEBUG));

	CString strATLCOMServerBinaryFullPath =
		GetCWD() +
		m_strWorkspaceLoc + 
		m_strATLCOMServerProjName + "\\" +
		GetLocString(IDSS_BUILD_DEBUG) + "\\" + 
		m_strATLCOMServerProjName + ".DLL";

	// add the atl com server to the additional dlls list so we can set a bp in it.
	LOG->RecordCompare(prj.SetAdditionalDLLs(strATLCOMServerBinaryFullPath) == ERROR_SUCCESS,
		"Adding ATL COM Server DLL to additional DLLs list for project '%s'", m_strAppProjName);

	// set a bp in the DllMain of the atl com server. 
	LOG->RecordCompare(bp.SetBreakpoint("DllMain") != NULL,
		"Set a breakpoint in the DllMain function of the ATL COM server.");

	// hit the bp.
	LOG->RecordCompare(dbg.Go("DllMain"), "Hit the bp in the DllMain function of the ATL COM server.");
	// stop debugging.
	LOG->RecordCompare(dbg.StopDebugging(), "Stop debugging the ATL COM Server");
	// clear the breakpoint at DllMain. 
	LOG->RecordCompare(bp.ClearAllBreakpoints(),"Clear thebp in the DllMain function of the ATL COM Server.");
}
