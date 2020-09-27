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
#include "DisAsub.h"
#include "afxdllx.h"
#include "..\..\DbgTestBase.h"

#include "DisAcase.h"
#include "DisAsmPP.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CDisAsmSubSuite

IMPLEMENT_SUBSUITE(CDisAsmSubSuite, CIDESubSuite, "Disassembly", "VCQA Debugger")

BEGIN_TESTLIST(CDisAsmSubSuite)
	TEST(CDisAsmIDETest, RUN)
	TEST(CDisAsmPP, RUN)
END_TESTLIST()

void CDisAsmSubSuite::CleanUp(void)
	{
	::CleanUp("testgo01");
	}

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization

void CDisAsmSubSuite::SetUp(BOOL bCleanUp)

{
	CIDESubSuite::SetUp(bCleanUp);
	// TODO(michma - 9/2/98): re-enable when memory leak bugs are fixed.
	ExpectedMemLeaks(-1);
}
