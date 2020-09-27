///////////////////////////////////////////////////////////////////////////////
//	ParamHlpSuite.cpp
//
//	Description :
//		Implementation of the CParamHlpSuite class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "ParamHlpSuite.h"

// ADD INCLUDES FOR CASES HERE
// EXAMPLE: #include "Feature1Cases.h"
#include "ParamHlpCases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CParamHlpSuite

IMPLEMENT_SUBSUITE(CParamHlpSuite, CIDESubSuite, "ParamHlp", "MHopkins x60946")

BEGIN_TESTLIST(CParamHlpSuite)
// ADD "TEST" ENTRY FOR CASES HERE
// EXAMPLE: TEST(CFeature1Cases, RUN)
	TEST(CParamHlpCases, RUN)
END_TESTLIST()

void CParamHlpSuite::SetUp(BOOL bCleanUp)
{

	int nResult;

	KillAllFiles(m_strCWD + "baseln\\ParamHlp01");
	nResult = prj.NewProject(UIAW_PT_APPWIZ, "ParamHlp01", m_strCWD + "baseln\\ParamHlp01", PLATFORM_WIN32_X86);

	if (nResult == ERROR_SUCCESS) {
		m_pLog->RecordSuccess("Created the project.");
	}
	else {
		m_pLog->RecordFailure("Failed creating project %sbaseln\\ParamHlp01", m_strCWD);
	}

}

void CParamHlpSuite::CleanUp(void)
{

	int nResult;

	// Close the project;
	nResult = prj.Close(FALSE);

	if (nResult != ERROR_SUCCESS) {
		m_pLog->RecordFailure(_T("Failed closing the project."));
	}
	else {
		m_pLog->RecordSuccess(_T("Closed the project."));
	}

	CIDESubSuite::CleanUp();
}
