///////////////////////////////////////////////////////////////////////////////
//	SNIFFSUB.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CSniffSubSuite class
//

#include "stdafx.h"
#include "sniffsub.h"
#include "afxdllx.h"
#include "..\cleanup.h"

#include "modify.h"
#include "add.h"
#include "remove.h"
#include "bp.h"
#include "mix.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSniffSubSuite

IMPLEMENT_SUBSUITE(CSniffSubSuite, CIDESubSuite, "EditAndContinue Sniff", "VCQA Debugger")

BEGIN_TESTLIST(CSniffSubSuite)
	TEST(CModifyingCodeTestEXE, RUN)
	TEST(CModifyingCodeTestEXT, DONTRUN)
	TEST(CModifyingCodeTestINT, DONTRUN)
	TEST(CAddingCodeTestEXE, RUN)
	TEST(CAddingCodeTestEXT, DONTRUN)
	TEST(CAddingCodeTestINT, DONTRUN)
	TEST(CRemovingCodeTestEXE, RUN)
	TEST(CRemovingCodeTestEXT, DONTRUN)
	TEST(CRemovingCodeTestINT, DONTRUN)
	TEST(CBreakpointsTestEXE, RUN)
	TEST(CBreakpointsTestEXT, DONTRUN)
	TEST(CBreakpointsTestINT, DONTRUN)
	TEST(CMixTestEXE, RUN)
	TEST(CMixTestEXT, DONTRUN)
	TEST(CMixTestINT, DONTRUN)
END_TESTLIST()

void CSniffSubSuite::CleanUp(void)
	{
	::CleanUp("ecconsol");
	::CleanUp("ecwinapp");
	}

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization

void CSniffSubSuite::SetUp(BOOL bCleanUp)
{
	fProjBuilt = FALSE;
	
	CSubSuite::SetUp(bCleanUp);

}

