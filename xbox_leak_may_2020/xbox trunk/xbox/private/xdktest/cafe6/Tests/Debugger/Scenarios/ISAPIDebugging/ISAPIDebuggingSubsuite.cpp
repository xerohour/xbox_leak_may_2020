///////////////////////////////////////////////////////////////////////////////
//	ISAPIDebuggingSubsuite.cpp
//
//	Created by: MichMa		Date: 11/20/97
//
//	Description:
//		Implementation of CISAPIDebuggingSubsuite

#include "stdafx.h"
#include "afxdllx.h"
#include "ISAPIDebuggingSubsuite.h"
#include "ISAPIDebuggingCases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CISAPIDebuggingSubsuite

IMPLEMENT_SUBSUITE(CISAPIDebuggingSubsuite, CIDESubSuite, "ISAPI Debugging Scenario", "MichMa")

BEGIN_TESTLIST(CISAPIDebuggingSubsuite)
	TEST(CISAPIDebuggingCases, RUN)
END_TESTLIST()

void CISAPIDebuggingSubsuite::SetUp(BOOL bCleanUp)
{
	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
	ExpectedMemLeaks(0);
}
