///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CNLGSubSuite class
//

#include "stdafx.h"
#include "NLGsub.h"
#include "afxdllx.h"
#include "..\cleanup.h"

#include "NLGcase.h"
#include "NLGcase2.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


/////////////////////////////////////////////////////////////////////////////
// CNLGSubSuite

IMPLEMENT_SUBSUITE(CNLGSubSuite, CIDESubSuite, "Non-Local Goto", "VCQA Debugger")

BEGIN_TESTLIST(CNLGSubSuite)
	TEST(CNLGIDETest, RUN)
	TEST(CNLG2IDETest, RUN)
END_TESTLIST()

void CNLGSubSuite::CleanUp(void)
	{
	::CleanUp("nlg");
	::CleanUp("seh01");
	}

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
