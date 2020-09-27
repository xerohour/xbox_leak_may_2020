///////////////////////////////////////////////////////////////////////////////
//	custsub.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CCustomBuildSubSuite class
//

#include "stdafx.h"
#include "custsub.h"
#include "afxdllx.h"

#include "cust1.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CCustomBuildSubSuite

IMPLEMENT_SUBSUITE(CCustomBuildSubSuite, CIDESubSuite, "Custom Build", "VCQA ProjBuild")

BEGIN_TESTLIST(CCustomBuildSubSuite)
	TEST(CCustomBuildTest1, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
