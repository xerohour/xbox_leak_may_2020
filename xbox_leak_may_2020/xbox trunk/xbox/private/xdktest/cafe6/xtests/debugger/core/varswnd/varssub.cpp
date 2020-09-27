///////////////////////////////////////////////////////////////////////////////
//	VARSSUB.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CVarsWndSubSuite class
//

#include "stdafx.h"
#include "varssub.h"
#include "afxdllx.h"

#include "autocase.h"
#include "loccase.h"
#include "thiscase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CVarsWndSubSuite

IMPLEMENT_SUBSUITE(CVarsWndSubSuite, CIDESubSuite, "Variables Window", "VCQA Debugger")

BEGIN_TESTLIST(CVarsWndSubSuite)
	TEST(CAutoPaneIDETest, RUN)
	TEST(CLocalsCases, RUN)
	TEST(CThisPaneIDETest, DONTRUN)	//	TODO : dverma : thispane currently is not implemented
END_TESTLIST()

void CVarsWndSubSuite::CleanUp(void)
	{
	::CleanUp("autopane");
	}

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization

