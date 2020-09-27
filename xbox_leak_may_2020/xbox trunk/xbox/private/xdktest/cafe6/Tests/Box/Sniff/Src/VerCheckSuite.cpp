///////////////////////////////////////////////////////////////////////////////
//	VerCheckSuite.CPP
//	Created by :
//		VSUpdate QA
//		DougMan
//
//	Description :	implementation of the CVerCheckSuite class

#include "stdafx.h"
#include "VerCheckSuite.h"

// Testcase includes
#include "FileVerTestcase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// The CVerCheckSuite object

IMPLEMENT_SUBSUITE(CVerCheckSuite, CIDESubSuite, "VC6 - Verfication Check", "DougMan")

BEGIN_TESTLIST(CVerCheckSuite)
	TEST(CFileVerTestcase, RUN)	// Add a list of all the testcases here. “RUN” flag selects the case to run by default.
END_TESTLIST()					// Use the NORUN flag to deselect the test by default when the suite is loaded in the driver.

void CVerCheckSuite::SetUp(BOOL bCleanUp)
{
	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
}

void CVerCheckSuite::CleanUp(void)
{
	CIDESubSuite::CleanUp();
}