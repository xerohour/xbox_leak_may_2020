///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CRegistersSubSuite class
//

#include "stdafx.h"
#include "Regsub.h"
#include "afxdllx.h"
#include "..\..\DbgTestBase.h"

#include "Regcase.h"
#include "RegistersPP.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CRegistersSubSuite

IMPLEMENT_SUBSUITE(CRegistersSubSuite, CIDESubSuite, "Registers", "VCQA Debugger")

BEGIN_TESTLIST(CRegistersSubSuite)
	TEST(CRegistersIDETest, RUN)
	TEST(CRegistersPP, RUN)
END_TESTLIST()

void CRegistersSubSuite::CleanUp(void)
	{
	::CleanUp("testgo01");
	}

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization

void CRegistersSubSuite::SetUp(BOOL bCleanUp)

{
	CIDESubSuite::SetUp(bCleanUp);
	// TODO(michma - 9/2/98): re-enable when memory leak bugs are fixed.
	ExpectedMemLeaks(-1);
}
