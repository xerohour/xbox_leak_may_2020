///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CRetValSubSuite class
//

#include "stdafx.h"
#include "RetVsub.h"
#include "afxdllx.h"
#include "..\cleanup.h"

#include "RetVcase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CRetValSubSuite

IMPLEMENT_SUBSUITE(CRetValSubSuite, CIDESubSuite, "Return Value", "VCQA Debugger")

BEGIN_TESTLIST(CRetValSubSuite)
	TEST(CRetValIDETest, RUN)
END_TESTLIST()

void CRetValSubSuite::CleanUp(void)
	{
	::CleanUp("retval");
	}

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization

