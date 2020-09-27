///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CmemSubSuite class
//

#include "stdafx.h"
#include "memsub.h"
#include "afxdllx.h"
#include "..\..\DbgTestBase.h"

#include "memcase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


/////////////////////////////////////////////////////////////////////////////
// CmemSubSuite

IMPLEMENT_SUBSUITE(CmemSubSuite, CIDESubSuite, "Memory Window", "VCQA Debugger")

BEGIN_TESTLIST(CmemSubSuite)
	TEST(CmemIDETest, RUN)
END_TESTLIST()

void CmemSubSuite::CleanUp(void)
	{
	::CleanUp("testgo01");
	::CleanUp("dbg");
	}

void CmemSubSuite::SetUp(BOOL bCleanUp)
{
	CIDESubSuite::SetUp(bCleanUp);
	// TODO(michma - 9/2/98): re-enable when memory leak bugs are fixed.
	ExpectedMemLeaks(-1);
}
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization

