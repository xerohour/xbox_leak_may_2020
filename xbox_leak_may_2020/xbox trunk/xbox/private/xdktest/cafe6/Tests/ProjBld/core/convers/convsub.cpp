///////////////////////////////////////////////////////////////////////////////
//	convsub.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CConversionSubSuite class
//

#include "stdafx.h"
#include "convsub.h"
#include "afxdllx.h"

#include "conv1.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CConversionSubSuite

IMPLEMENT_SUBSUITE(CConversionSubSuite, CIDESubSuite, "Project Conversion", "VCQA ProjBuild")

BEGIN_TESTLIST(CConversionSubSuite)
	TEST(CConversionTest1, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
