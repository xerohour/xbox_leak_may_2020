///////////////////////////////////////////////////////////////////////////////
//	sprjsub.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CSubprojectsSubSuite class
//

#include "stdafx.h"
#include "sprjsub.h"
#include "afxdllx.h"

#include "subprj1.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSubprojectsSubSuite

IMPLEMENT_SUBSUITE(CSubprojectsSubSuite, CIDESubSuite, "Subprojects", "VCQA ProjBuild")

BEGIN_TESTLIST(CSubprojectsSubSuite)
	TEST(CSubprojectsTest1, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
