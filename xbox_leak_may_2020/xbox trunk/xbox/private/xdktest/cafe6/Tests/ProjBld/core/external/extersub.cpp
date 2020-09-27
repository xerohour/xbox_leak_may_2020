///////////////////////////////////////////////////////////////////////////////
//	extersub.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CextersubSuite class
//

#include "stdafx.h"
#include "extersub.h"
#include "afxdllx.h"

#include "extern1.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CExternalMakefileSubSuite

IMPLEMENT_SUBSUITE(CExternalMakefileSubSuite, CIDESubSuite, "External Makefiles", "VCQA ProjBuild")

BEGIN_TESTLIST(CExternalMakefileSubSuite)
	TEST(CExternalMakefileTest1, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
