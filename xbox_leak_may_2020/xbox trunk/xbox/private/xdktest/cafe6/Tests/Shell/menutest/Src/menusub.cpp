///////////////////////////////////////////////////////////////////////////////
//	Menucase.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CMenuTesterSubSuite class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "Menusub.h"

#include "Menucase.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// The CMenuTesterSubSuite object

IMPLEMENT_SUBSUITE(CMenuTesterSubSuite, CIDESubSuite, "OpenClose & Menus sniff", "ChrisKoz")

BEGIN_TESTLIST(CMenuTesterSubSuite)
	brunmenus=m_SuiteParams->GetBooleanValue("RUNMENUS", TRUE);
	TEST(CMenuTesterIDETest, RUN)
		pTest->SetName(brunmenus?"Test Menus":"Empty");
END_TESTLIST()


void CMenuTesterSubSuite::SetUp(BOOL bCleanUp)
{
	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
	ExpectedMemLeaks(0);
}

