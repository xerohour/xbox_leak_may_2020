///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CDisAsmSubSuite class
//

#include "stdafx.h"
#include "BldXQuakesub.h"
#include "afxdllx.h"
#include "..\..\IdeTestBase.h"

#include "BldXQuakecase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CDisAsmSubSuite

IMPLEMENT_SUBSUITE(CBldXQuakeSubSuite, CIDESubSuite, "BldXQuake", "emmang@xbox.com")

BEGIN_TESTLIST(CBldXQuakeSubSuite)
	TEST(CBldXQuakeTest, RUN)
END_TESTLIST()

void CBldXQuakeSubSuite::CleanUp(void)
	{
	::CleanUp("testgo01");
	}

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization

void CBldXQuakeSubSuite::SetUp(BOOL bCleanUp)

{
	CIDESubSuite::SetUp(bCleanUp);
	// TODO(michma - 9/2/98): re-enable when memory leak bugs are fixed.
	ExpectedMemLeaks(-1);
}
