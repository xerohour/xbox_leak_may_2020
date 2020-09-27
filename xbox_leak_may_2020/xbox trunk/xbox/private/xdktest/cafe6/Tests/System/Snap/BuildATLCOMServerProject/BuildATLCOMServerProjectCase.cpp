///////////////////////////////////////////////////////////////////////////////
//	BuildATLCOMServerProjectCase.CPP
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Implementation of the CBuildATLCOMServerProjectCase class

#include "stdafx.h"
#include "BuildATLCOMServerProjectCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CBuildATLCOMServerProjectCase, CSystemTestSet, "Build ATL COM Server Project", -1, CBuildATLCOMServerProjectSubsuite)

void CBuildATLCOMServerProjectCase::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void CBuildATLCOMServerProjectCase::Run(void)
{	 
	// need to do this because each subsuite has its own COProject object.
	prj.Attach();

	// select the ATL COM Server project so we can build it.
	prj.SetTarget((CString)m_strATLCOMServerProjName + " - " + 
					GetLocString(IDSS_PLATFORM_WIN32) + " " +
					GetLocString(IDSS_BUILD_DEBUG));

	// build the project.
	LOG->RecordCompare(prj.Build() == ERROR_SUCCESS, "Build '%s' project.", m_strATLCOMServerProjName);
	
	// verify no errors or warnings were generated.
	int iErr, iWarn;
	LOG->RecordCompare(prj.VerifyBuild(TRUE, &iErr, &iWarn, TRUE) == ERROR_SUCCESS,
		"Verify build of '%s' project", m_strATLCOMServerProjName);
}
