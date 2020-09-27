///////////////////////////////////////////////////////////////////////////////
//	AutoCompleteSuite.Cpp
//
//	Description :
//		Implementation of the CAutoCompleteSuite class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "AutoCompleteSuite.h"

// ADD INCLUDES FOR CASES HERE
// EXAMPLE: #include "Feature1Cases.h"
#include "AutoCompleteCases.h"
#include "ParameterHelpCases.h"
#include "CodeCommentsCases.h"
#include "TypeInfoCases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CAutoCompleteSuite

IMPLEMENT_SUBSUITE(CAutoCompleteSuite, CIDESubSuite, "AutoComplete", "a-JohnC x27223")

BEGIN_TESTLIST(CAutoCompleteSuite)
// ADD "TEST" ENTRY FOR CASES HERE
// EXAMPLE: TEST(CFeature1Cases, RUN)
	TEST(CAutoCompleteCases, RUN)
	TEST(CParameterHelpCases, RUN)
	TEST(CCodeCommentsCases, RUN)
	TEST(CTypeInfoCases, RUN)
END_TESTLIST()

void CAutoCompleteSuite::SetUp(BOOL bCleanUp)
{

	int nResult;
	int nCount;

	KillAllFiles(m_strCWD + "baseln\\AutoComp01");
	nResult = prj.NewProject(UIAW_PT_APPWIZ, "AutoComp01", m_strCWD + "baseln\\AutoComp01", PLATFORM_WIN32_X86);

	if (nResult == ERROR_SUCCESS) {
		m_pLog->RecordSuccess("Created the project.");

		// Expand the ClassView tree
		MST.DoKeys("%0");
		Sleep(2000);
		MST.DoKeys("{UP}");
		Sleep(2000);
		MST.DoKeys("*");
		Sleep(2000);
		MST.DoKeys("*");
		Sleep(2000);
	}
	else {
		m_pLog->RecordFailure("Failed creating project %sbaseln\\AutoComp01", m_strCWD);
	}

	// Open a source file
	nResult = src.Open(m_strCWD + "baseln\\AutoComp01\\"+"AutoComp01.cpp");
	Sleep(1000);

	if (nResult == ERROR_ERROR) {
		m_pLog->RecordFailure("Couldn't open source file AutoComp01.cpp");
	}
	else {
		// Make some modifications to the CAboutDlg class
		src.GoToLine(112);
	
		// Wait for Go To dialog to go away
		nCount = 0;
		while ((MST.WFndWnd("Go To", FW_DEFAULT)) && (nCount < 1000)) {
			nCount++;
		}

		MST.DoKeys("{END}{ENTER 2}");
		MST.DoKeys("// Nonsense function for testing purposes{ENTER}");
		MST.DoKeys("void foo{(}int i, float f, char* str{)};{ENTER}");

		src.GoToLine(148);
	
		// Wait for Go To dialog to go away
		nCount = 0;
		while ((MST.WFndWnd("Go To", FW_DEFAULT)) && (nCount < 1000)) {
			nCount++;
		}

		MST.DoKeys("{END}{ENTER 2}");
		MST.DoKeys("void CAboutDlg::foo{(}int i, float f, char* str{)}{ENTER}");
		MST.DoKeys("{{}");
		MST.DoKeys("{ENTER}");
		MST.DoKeys("return;{ENTER}");
		MST.DoKeys("{}}");
		MST.DoKeys("{ENTER}");
		Sleep(1000);

		// Save changes and close source
		src.Save();
		src.Close(TRUE);
	}
}

void CAutoCompleteSuite::CleanUp(void)
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
