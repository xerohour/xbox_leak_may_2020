///////////////////////////////////////////////////////////////////////////////
//      SNIFF.CPP
//
//      Created by :                    Date :
//              RickKr                                  8/30/93
//
//      Description :
//              implementation of the CSniffDriver class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "sniff.h"

#include "syscases.h"
#include "srccases.h"
#include "rescases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

int ImeInit = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CSniffDriver

IMPLEMENT_SUBSUITE(CSysSubSuite, CIDESubSuite, "Sys sniff test", "MichMa")
PLATFORM_TYPE target_platform = (PLATFORM_TYPE)GetUserTargetPlatforms();

BEGIN_TESTLIST(CSysSubSuite)
	TEST(CSysAppWizTest, RUN)
	TEST(CSysClassesTest, RUN)  DEPENDENCY(CSysAppWizTest)
	TEST(CSysResTest, RUN)		DEPENDENCY(CSysAppWizTest)
	TEST(CSysBuildTest, RUN)	DEPENDENCY(CSysAppWizTest)
	TEST(CSysDebugPrjTest, RUN)	DEPENDENCY(CSysBuildTest)
	TEST(CSysDebugExeTest, RUN)	DEPENDENCY(CSysBuildTest)
END_TESTLIST()

void CSysSubSuite::SetUp(BOOL bCleanUp)
{
	// remove the project directory
	KillAllFiles(m_strCWD + PROJDIR, TRUE);

	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
	ExpectedMemLeaks(2);
}

void CSysSubSuite::CleanUp(void)
{
	// call the base class
	CIDESubSuite::CleanUp();
}
