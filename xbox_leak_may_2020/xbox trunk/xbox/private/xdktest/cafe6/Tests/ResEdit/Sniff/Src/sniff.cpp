///////////////////////////////////////////////////////////////////////////////
//      SNIFF.CPP
//
//      Created by :                    Date :
//              DavidGa                                 8/30/93
//
//      Description :
//              implementation of the CSniffTest class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "sniff.h"

#include "acccases.h"
#include "strcases.h"
#include "bincases.h"
#include "dlgcases.h"
#include "mencases.h"
#include "imgcases.h"
#include "vercases.h"
#include "io_cases.h"
#include "intcases.h"
#include "symcases.h"
#include "brzcases.h"
#include "cpicases.h"
#include "tbrcases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSniffDriver

IMPLEMENT_SUBSUITE(CSniffDriver, CIDESubSuite, "VRes Sniff Test", "EHarding X34536")

BEGIN_TESTLIST(CSniffDriver)
	TEST(CAccTestCases, RUN)
	TEST(CStrTestCases, RUN)
	TEST(CBinTestCases, RUN)
	TEST(CDlgTestCases, RUN)
	TEST(CMenTestCases, RUN)
        TEST(CImgTestCases, RUN)
	TEST(CVerTestCases, RUN)
	TEST(CIO_TestCases, RUN)
	TEST(CIntTestCases, RUN)
	TEST(CSymTestCases, RUN)
	TEST(CBrzTestCases, RUN)
	TEST(CCpiTestCases, RUN)
	TEST(CTbrTestCases, RUN)
END_TESTLIST()

void CSniffDriver::SetUp(BOOL bCleanUp)
{
	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
	ExpectedMemLeaks(0);
}

void CSniffDriver::CleanUp(void)
{
	// close all open windows
	UIWB.CloseAllWindows();
}
