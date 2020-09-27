///////////////////////////////////////////////////////////////////////////////
//	localssub.cpp
//
//	Created by: MichMa		Date: 10/22/97			
//
//	Description:
//		implementation of the CECLocalsSubSuite class
//

#include "stdafx.h"
#include "localssub.h"
#include "afxdllx.h"

#include "..\cleanup.h"
#include "localscases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


/////////////////////////////////////////////////////////////////////////////
// CLocalsSubSuite

IMPLEMENT_SUBSUITE(CECLocalsSubSuite, CIDESubSuite, "Edit & Continue: Locals", "VCQA Debugger")

BEGIN_TESTLIST(CECLocalsSubSuite)
	TEST(CECLocalsCases, RUN)
END_TESTLIST()

