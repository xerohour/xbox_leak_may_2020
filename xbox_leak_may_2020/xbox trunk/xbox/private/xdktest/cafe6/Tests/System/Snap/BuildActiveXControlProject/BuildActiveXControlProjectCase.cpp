///////////////////////////////////////////////////////////////////////////////
//	BuildActiveXControlProjectCase.CPP
//
//	Created by :			Date :
//		MichMa					5/21/97
//
//	Description :
//		Implementation of the CBuildActiveXControlProjectCase class

#include "stdafx.h"
#include "BuildActiveXControlProjectCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CBuildActiveXControlProjectCase, CSystemTestSet, "Build ActiveX Control Project", -1, CBuildActiveXControlProjectSubsuite)

void CBuildActiveXControlProjectCase::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void CBuildActiveXControlProjectCase::Run(void)
{	 

	// need to do this because each subsuite has its own COProject object.
	prj.Attach();

	// select the ActiveX Control project so we can build it.
	prj.SetTarget((CString)m_strActiveXControlProjName + " - " + 
					GetLocString(IDSS_PLATFORM_WIN32) + " " +
					GetLocString(IDSS_BUILD_DEBUG));

	// build the project.
	LOG->RecordCompare(prj.Build() == ERROR_SUCCESS, "Build '%s' project.", m_strActiveXControlProjName);
	
	// verify no errors or warnings were generated.
	int iErr, iWarn;
	LOG->RecordCompare(prj.VerifyBuild(TRUE, &iErr, &iWarn, TRUE) == ERROR_SUCCESS,
		"Verify build of '%s' project", m_strActiveXControlProjName);
}
