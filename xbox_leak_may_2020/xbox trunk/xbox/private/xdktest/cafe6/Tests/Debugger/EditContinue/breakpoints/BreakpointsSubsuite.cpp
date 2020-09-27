///////////////////////////////////////////////////////////////////////////////
//	BreakpointsSubsuite.cpp
//
//	Created by: MichMa		Date: 3/12/98			
//
//	Description:
//		implementation of the CECBreakpointsSubsuite class
//

#include "stdafx.h"
#include "BreakpointsSubsuite.h"
#include "afxdllx.h"

#include "..\cleanup.h"
#include "BreakpointsCases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


/////////////////////////////////////////////////////////////////////////////
// CECBreakpointsSubsuite

IMPLEMENT_SUBSUITE(CECBreakpointsSubsuite, CIDESubSuite, "Edit & Continue: Breakpoints", "VCQA Debugger")

BEGIN_TESTLIST(CECBreakpointsSubsuite)
	TEST(CECBreakpointsCases, RUN)
END_TESTLIST()

