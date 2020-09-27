///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			Date :
//		DavidGa					10/27/93
//
//	Description :
//		implementation of the Wizards' CSniffDriver class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "sniff.h"

#include "appcases.h"
#include "clscases.h"
#include "awxcases.h"
#include "ctrcases.h"
#include "og_cases.h"
#include "wzbcases.h"
#include "ATLCOMCases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSniffDriver

IMPLEMENT_SUBSUITE(CSniffDriver, CIDESubSuite, "Wizards Sniff Test", "Anita George x68862")

BEGIN_TESTLIST(CSniffDriver)
	TEST(CObjGalTestCases, RUN)
	TEST(CAppTestCases, RUN)
	TEST(CClsTestCases, RUN)
	TEST(CWzbTestCases, RUN)
	TEST(CAwxTestCases, RUN)
	TEST(CCtrlWizardCases, RUN)
	TEST(CATLCOMCases, RUN)
END_TESTLIST()

// flag to indicate to class wizard whether app wizard was successful or not
BOOL gbAppWiz;

void CSniffDriver::SetUp(BOOL bCleanUp)
{
	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
	ExpectedMemLeaks(0);

	gbAppWiz = FALSE;

}
