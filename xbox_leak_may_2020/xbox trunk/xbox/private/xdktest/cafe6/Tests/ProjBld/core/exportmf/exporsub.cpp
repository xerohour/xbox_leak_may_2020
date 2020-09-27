///////////////////////////////////////////////////////////////////////////////
//	exporsub.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CExportMakefileSubSuite class
//

#include "stdafx.h"
#include "exporsub.h"
#include "afxdllx.h"

#include "expor1.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CExportMakefileSubSuite

IMPLEMENT_SUBSUITE(CExportMakefileSubSuite, CIDESubSuite, "Export Makefile", "VCQA ProjBuild")

BEGIN_TESTLIST(CExportMakefileSubSuite)
	TEST(CExportMakefileTest1, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
