///////////////////////////////////////////////////////////////////////////////
//	scn01sub.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CCreateBuildDebugAppWizAppSubSuite class
//

#include "stdafx.h"
#include "scn01sub.h"
#include "afxdllx.h"

#include "scn01.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CCreateBuildDebugAppWizAppSubSuite

IMPLEMENT_SUBSUITE(CCreateBuildDebugAppWizAppSubSuite, CIDESubSuite, "Scenario01", "VCQA")

BEGIN_TESTLIST(CCreateBuildDebugAppWizAppSubSuite)
	TEST(CCreateAppWizAppTest, RUN)
	TEST(CAddDialogTest, RUN)
//	TEST(CAddMenuItemTest, RUN)
//	TEST(CBuildTest, RUN)
//	TEST(CSmallDebugTest, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
