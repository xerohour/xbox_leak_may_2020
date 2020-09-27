///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			Date :
//		RickKr					8/27/93
//
//	Description :
//		implementation of the CSniffDriver class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "sniff.h"

#include "grdcases.h"
#include "lfncases.h"
#include "lytcases.h"
#include "tb_cases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSniffDriver

IMPLEMENT_SUBSUITE(CSniffDriver, CIDESubSuite, "VShell sniff test", "Ernest Jugovic x31522")

BEGIN_TESTLIST(CSniffDriver)
//	TEST(CLytTestCases, RUN)
    TEST(CLfnTestCases, RUN)
	TEST(CTabTestCases, RUN)
	TEST(CTBTestCases, RUN)
	TEST(CGrdTestCases, DONTRUN)
END_TESTLIST()

void CSniffDriver::SetUp(BOOL bCleanUp)
{
	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
	ExpectedMemLeaks(0);
}
