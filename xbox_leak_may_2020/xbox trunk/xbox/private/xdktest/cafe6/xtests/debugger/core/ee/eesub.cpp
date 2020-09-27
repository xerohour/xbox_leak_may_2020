///////////////////////////////////////////////////////////////////////////////
//	EESUB.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CEESubSuite class
//

#include "stdafx.h"
#include "eesub.h"
#include "afxdllx.h"

#include "eecase.h"
#include "eespec.h"
#include "eecasepp.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CRetValSubSuite

IMPLEMENT_SUBSUITE(CEESubSuite, CIDESubSuite, "Expression Evaluator", "VCQA Debugger")

BEGIN_TESTLIST(CEESubSuite)
	TEST(CEEIDETest, RUN)
	TEST(CSpecialTypesTest, RUN)
	TEST(CEEPP, RUN)
END_TESTLIST()

void CEESubSuite::CleanUp(void)
	{
	::CleanUp("ee");
	}

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization

