///////////////////////////////////////////////////////////////////////////////
//	scn02sub.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CDebugAppWizAppSubSuite class
//

#include "stdafx.h"
#include "scn02sub.h"
#include "afxdllx.h"

#include "scn02.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CDebugAppWizAppSubSuite

IMPLEMENT_SUBSUITE(CDebugAppWizAppSubSuite, CIDESubSuite, "Scenario02", "VCQA")

BEGIN_TESTLIST(CDebugAppWizAppSubSuite)
	TEST(CEditBuildTest, RUN)
	TEST(CDebugTest, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
