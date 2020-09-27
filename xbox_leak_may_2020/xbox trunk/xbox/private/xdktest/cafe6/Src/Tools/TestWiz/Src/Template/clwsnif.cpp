///////////////////////////////////////////////////////////////////////////////
//	$$cases_cpp$$.CPP
//
//	Created by :			
//		$$TestOwner$$		
//
//	Description :
//		implementation of the $$DriverClass$$ class
//

#include "stdafx.h"
#include "$$type_cpp$$.h"

#include "$$cases_cpp$$.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// The $$DriverClass$$ object

IMPLEMENT_SUBSUITE($$DriverClass$$, CIDESubSuite, "$$TestTitle$$", "$$TestOwner$$")

BEGIN_TESTLIST($$DriverClass$$)
	TEST($$TestClass$$, RUN)
END_TESTLIST()


void $$DriverClass$$::SetUp(BOOL bCleanUp)
{
	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
}

