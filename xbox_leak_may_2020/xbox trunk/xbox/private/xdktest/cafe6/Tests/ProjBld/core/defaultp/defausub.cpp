///////////////////////////////////////////////////////////////////////////////
//	defausub.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CDefaultPrjSubSuite class
//

#include "stdafx.h"
#include "defausub.h"
#include "afxdllx.h"

#include "defau1.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CDefaultPrjSubSuite

IMPLEMENT_SUBSUITE(CDefaultPrjSubSuite, CIDESubSuite, "Default Project", "VCQA ProjBuild")

BEGIN_TESTLIST(CDefaultPrjSubSuite)
	TEST(CDefaultProjectTest1, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
