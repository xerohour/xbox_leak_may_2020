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
#include "AppWizsub.h"
#include "afxdllx.h"
#include "..\..\IdeTestBase.h"

#include "AppWizcase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CDisAsmSubSuite

IMPLEMENT_SUBSUITE(CAppWizSubSuite, CIDESubSuite, "IDE AppWizard", "emmang@xbox.com")

BEGIN_TESTLIST(CAppWizSubSuite)
	TEST(CAppWizTest, RUN)
END_TESTLIST()

void CAppWizSubSuite::CleanUp(void)
	{
	::CleanUp("testgo01");
	}

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization

void CAppWizSubSuite::SetUp(BOOL bCleanUp)

{
	CIDESubSuite::SetUp(bCleanUp);
	// TODO(michma - 9/2/98): re-enable when memory leak bugs are fixed.
	ExpectedMemLeaks(-1);
}
