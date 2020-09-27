///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CstackSubSuite class
//

#include "stdafx.h"
#include "stacsub.h"
#include "afxdllx.h"
#include "..\cleanup.h"

#include "cs_base.h"
#include "cs_dlls.h"
#include "cspecial.h"
#include "cs_stres.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


/////////////////////////////////////////////////////////////////////////////
// CstackSubSuite

IMPLEMENT_SUBSUITE(CstackSubSuite, CIDESubSuite, "CallStack", "VCQA Debugger")

BEGIN_TESTLIST(CstackSubSuite)
	TEST(Cstack_baseIDETest, RUN)
	TEST(Cstack_dllsIDETest, RUN)
	TEST(Cstack_specialIDETest, RUN)
	TEST(Cstack_stressIDETest, RUN)
END_TESTLIST()


void CstackSubSuite::CleanUp(void)
	{
	::CleanUp("stack01");
	::CleanUp("testgo01");
	::CleanUp("dbg");
	::CleanUp("dllapp");
	}
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization

