///////////////////////////////////////////////////////////////////////////////
//	optsub.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CBuildOptionsSubSuite class
//

#include "stdafx.h"
#include "optsub.h"
#include "afxdllx.h"

#include "opt1.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CBuildOptionsSubSuite

IMPLEMENT_SUBSUITE(CBuildOptionsSubSuite, CIDESubSuite, "Build Options", "VCQA ProjBuild")

BEGIN_TESTLIST(CBuildOptionsSubSuite)
	TEST(CBuildOptionsTest1, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
