///////////////////////////////////////////////////////////////////////////////
//	ComponentsSuite.Cpp
//
//	Description :
//		Implementation of the CComponentsSuite class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "ComponentsSuite.h"

// ADD INCLUDES FOR CASES HERE
#include "AddnBuildCases.h"
// EXAMPLE: #include "Feature1Cases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CComponentsSuite

IMPLEMENT_SUBSUITE(CComponentsSuite, CIDESubSuite, "Components", "")

BEGIN_TESTLIST(CComponentsSuite)
// ADD "TEST" ENTRY FOR CASES HERE
	TEST(CAddnBuildCases, RUN)
// EXAMPLE: TEST(CFeature1Cases, RUN)
END_TESTLIST()

void CComponentsSuite::SetUp(BOOL bCleanUp)
{
	CIDESubSuite::SetUp(bCleanUp);
	ExpectedMemLeaks(0);
}

void CComponentsSuite::CleanUp(void)
{
}
